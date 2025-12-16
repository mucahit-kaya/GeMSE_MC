// Make this appear first!
#include "G4Timer.hh"

#include "GeMSE_Analysis.hh"
#include "GeMSE_RunAction.hh"
#include "GeMSE_TrackingAction.hh"
#include "GeMSE_RunMessenger.hh"

#include <sys/time.h>
#include <vector>
#include <string>
#include "TTree.h"
#include "TFile.h"
#include "TBranch.h"
#include "TSystem.h"
#include <TParameter.h>

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4Version.hh"
#include "G4GenericAnalysisManager.hh"
#include "G4Threading.hh"

using G4AnalysisManager = G4GenericAnalysisManager;
using std::string;

G4double energy_ = 0, efficiency_ = 0, efficiency_err_ = 0, eff_BR_ = 0;

GeMSE_RunAction::GeMSE_RunAction(G4String OutputFolder) {
  fOutputFolder = OutputFolder;
  timer = new G4Timer;
  mcVersionTag = "0.0.0";

  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetVerboseLevel(0);
  // create run analysis
  fRunAnalysis = new GeMSE_Analysis();
  // create a messenger for this class
  runMessenger = new GeMSE_RunMessenger(fRunAnalysis);
}

GeMSE_RunAction::~GeMSE_RunAction() {
  delete timer;
  // delete fRunAnalysis;
  delete runMessenger;
}

void GeMSE_RunAction::BeginOfRunAction(const G4Run* aRun) {
  // set random seed
  // CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  struct timeval hTimeValue;
  //gettimeofday(&hTimeValue, NULL);
  aSeed = hTimeValue.tv_usec;
  // CLHEP::HepRandom::setTheSeed(aSeed);
  G4cout << "\nStarting run with seed = " << aSeed << G4endl;

  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetVerboseLevel(0);

  TString ResultFileName;  
  G4int RunID = aRun->GetRunID();
  // add thread ID info to output file name in MT mode
  #ifdef G4MULTITHREADED
    G4int tid = G4Threading::G4GetThreadId(); // worker: 0..N-1, master: -1
      // Prevent Master thread to create output file
    if (G4Threading::IsMasterThread()) {
      // G4cout << "Master thread: skipping ROOT output setup." << G4endl;
      timer->Start();
      return;
    }
  #else
    G4int tid = -1;
  #endif

  // G4cout << "DEBUG RunID=" << RunID << " ThreadID=" << tid << " IsMaster=" << G4Threading::IsMasterThread() << G4endl;  // for debugging

  // set output file name (MT: add thread ID)
  if (fOutputFolder != "") {
    if (analysisManager->GetFileName()=="") {
      std::ostringstream convert;   // stream used for the int->str conversion
      convert << RunID;
      if (tid >= 0) {
        std::ostringstream tconvert;
        tconvert << tid;
        ResultFileName = "results_run" + TString(convert.str()) + "_t" + TString(tconvert.str()) + ".root";
      }
      else {
        // master or single-thread
        ResultFileName = "results_run" + TString(convert.str()) + ".root";
      }
    }
    else
      ResultFileName = analysisManager->GetFileName();

    // try to open results directory
    if (!gSystem->OpenDirectory(fOutputFolder)) {
      // if directory does not exist make one
      if (gSystem->MakeDirectory(fOutputFolder)==-1) {
        G4cout << "###### ERROR: could not create directory " << fOutputFolder
               << G4endl;
        return;
      }
    }

    ResultFile = new TFile(fOutputFolder+"/"+ResultFileName,"Create");

    if (ResultFile->IsZombie()) {
      G4cout << "#### " << ResultFileName << " -> Overwriting!" << G4endl;
      ResultFile = new TFile(fOutputFolder+"/"+ResultFileName,"recreate");
    }
  ResultFile->cd();
  }

  // Create trees
  tree = new TTree("tree", "Ge efficiency calculation");
  GeHitTree = new TTree("GeHits", "All energy-depositing Ge hits");
  MuonHitTree = new TTree("MuonHits", "Information on all energy-depositing muon veto hits");
  PrimariesTree = new TTree("Primaries", "Information on all generated particles in the run");
  RunTree = new TTree("RunInfo", "Run information");

  GeHitTree->Branch("EventID", &HEventID);
  GeHitTree->Branch("NHits", &NHits);
  GeHitTree->Branch("TotEdep", &TotEdep);
  GeHitTree->Branch("TrackID", &HTrackID);
  GeHitTree->Branch("ParticleID", &HParticleID);
  GeHitTree->Branch("Edep", &Edep);
  GeHitTree->Branch("xPos", &xPos);
  GeHitTree->Branch("yPos", &yPos);
  GeHitTree->Branch("zPos", &zPos);
  GeHitTree->Branch("Time", &Time);
  GeHitTree->Branch("Ekin", &HEkin);

  MuonHitTree->Branch("EventID", &HEventID);
  MuonHitTree->Branch("NHits", &NHits);
  MuonHitTree->Branch("TotEdep", &TotEdep);
  MuonHitTree->Branch("TrackID", &HTrackID);
  MuonHitTree->Branch("ParticleID", &HParticleID);
  MuonHitTree->Branch("PanelNr", &PanelNr);
  MuonHitTree->Branch("Edep", &Edep);
  MuonHitTree->Branch("xPos", &xPos);
  MuonHitTree->Branch("yPos", &yPos);
  MuonHitTree->Branch("zPos", &zPos);
  MuonHitTree->Branch("Time", &Time);
  MuonHitTree->Branch("Ekin", &HEkin);

  PrimariesTree->Branch("EventID", &PEventID);
  PrimariesTree->Branch("TrackID", &PTrackID);
  PrimariesTree->Branch("ParentID", &ParentID);
  PrimariesTree->Branch("xPriPos", &xPriPos);
  PrimariesTree->Branch("yPriPos", &yPriPos);
  PrimariesTree->Branch("zPriPos", &zPriPos);
  PrimariesTree->Branch("Ekin", &PEkin);
  PrimariesTree->Branch("xDir", &xDir);
  PrimariesTree->Branch("yDir", &yDir);
  PrimariesTree->Branch("zDir", &zDir);
  PrimariesTree->Branch("ParticleID", &PParticleID);
  PrimariesTree->Branch("CreatorProcess", &Process);

  RunTree->Branch("nEvents", &NEvents);
  RunTree->Branch("nDecays", &NDecays);

  tree->Branch("energy", &energy_);
  tree->Branch("efficiency", &efficiency_);
  tree->Branch("efficiency_err", &efficiency_err_);
  tree->Branch("eff_BR", &eff_BR_);
  
  NEvents = aRun->GetNumberOfEventToBeProcessed();
  fNDecays = 0;

  // set Nb of events
  fRunAnalysis->SetNEvents(NEvents);

  G4cout << "### Run " << RunID << " started with " << NEvents << " events."
         << G4endl;
  timer->Start();
}

void GeMSE_RunAction::EndOfRunAction(const G4Run* aRun) {
  #ifdef G4MULTITHREADED
    if (G4Threading::IsMasterThread()) return;
  #endif

  // if a tree is defined
  if (tree) {
    // calculate efficiencies
    fRunAnalysis->CalcEfficiencies();

    // fill ROOT Tree
    int nlines = fRunAnalysis->GetNLines();
    for (int i = 0; i < nlines; ++i) {
      energy_ = fRunAnalysis->GetEnergy(i);
      efficiency_ = fRunAnalysis->GetEfficiency(i);
      efficiency_err_ = fRunAnalysis->GetEfficiency_err(i);
      eff_BR_ = fRunAnalysis->GetEffBR(i);

      tree->Fill();
    }
    // clear data
    fRunAnalysis->Clear();
  }

  timer->Stop();
  
  NDecays=fNDecays;
  
  G4cout << "\n### Finished ###" << G4endl;
  G4cout << "Runtime: " << *timer << G4endl;
  
  // write trees and close file
  if (fOutputFolder != "") {
    ResultFile->cd();

    TNamed *G4version = new TNamed("G4VERSION_TAG", G4VERSION_TAG);
    G4version->Write(); 
    TNamed *MCversiontag = new TNamed("MCVERSION_TAG", mcVersionTag);
    MCversiontag->Write();

    G4double dt = timer->GetRealElapsed();
    RunTree->Branch("RunTime", &dt);
    G4double Seed = aSeed; // this is ugly, rewrite at some point
    RunTree->Branch("Seed", &Seed);
    RunTree->Fill();

    tree->Write();
    GeHitTree->Write();
    MuonHitTree->Write();
    PrimariesTree->Write();
    RunTree->Write();  

    ResultFile->Close();
  }
}

TTree* GeMSE_RunAction::GetGeHitTree()
{
  fGeHitTree=GeHitTree;
  return fGeHitTree;
}

TTree* GeMSE_RunAction::GetMuonHitTree()
{
  fMuonHitTree=MuonHitTree;
  return fMuonHitTree;
}

TTree* GeMSE_RunAction::GetPrimariesTree()
{
  fPrimariesTree=PrimariesTree;
  return fPrimariesTree;
}

void GeMSE_RunAction::AddDecay()
{
  fNDecays++;
}

void GeMSE_RunAction::FillPrimariesTree(int eventID, int trackID, int parentID_,
                                       double xPriPos_, double yPriPos_, double zPriPos_,
                                       double ekin_,
                                       double xDir_, double yDir_, double zDir_,
                                       const std::string& particle,
                                       const std::string& creator)
{
  if (!PrimariesTree) return;

  PEventID  = eventID;
  PTrackID  = trackID;
  ParentID  = parentID_;

  xPriPos = xPriPos_;
  yPriPos = yPriPos_;
  zPriPos = zPriPos_;

  PEkin = ekin_;

  xDir = xDir_;
  yDir = yDir_;
  zDir = zDir_;

  PParticleID = particle;
  Process     = creator;

  PrimariesTree->Fill();
}
