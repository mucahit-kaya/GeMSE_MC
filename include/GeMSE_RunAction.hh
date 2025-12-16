#ifndef GeMSE_RunAction_h
#define GeMSE_RunAction_h 1

#include "GeMSE_Analysis.hh"

#include "TTree.h"
#include "TFile.h"
#include "globals.hh"
#include <vector>
#include <string>

#include "G4ThreeVector.hh"
#include "G4UserRunAction.hh"

using std::string;

class G4Timer;
class G4Run;
class GeMSE_RunMessenger;

class GeMSE_RunAction : public G4UserRunAction {
 public:
  GeMSE_RunAction(G4String Outputfolder);
  ~GeMSE_RunAction();  

 public:
  void BeginOfRunAction(const G4Run* aRun);
  void EndOfRunAction(const G4Run* aRun);
  
  TTree* GetGeHitTree();
  TTree* GetMuonHitTree();
  TTree* GetPrimariesTree();

  void AddDecay();
  void SetVersionTag(const G4String &hVersionTag) {
    mcVersionTag = hVersionTag;
  }

  GeMSE_Analysis* GetRunAnalysis() { return fRunAnalysis; };

void FillPrimariesTree(int eventID, int trackID, int parentID_,
                       double xPriPos_, double yPriPos_, double zPriPos_,
                       double ekin_,
                       double xDir_, double yDir_, double zDir_,
                       const std::string& particle,
                       const std::string& creator);
//  TTree* GetPrimariesTree(); // for debugging

 private:
  G4Timer* timer;
  G4long aSeed;
  G4int NDecays;
  G4int fNDecays;
  G4String mcVersionTag;
  G4int NEvents;
  G4int HEventID;
  G4int NHits;
  G4double TotEdep;
  
  std::vector<double> Edep;
  std::vector<double> HEkin;
  std::vector<double> Time;
  std::vector<double> xPos;
  std::vector<double> yPos;
  std::vector<double> zPos;
  std::vector<int> HParticleID;
  std::vector<int> HTrackID;
  std::vector<int> PanelNr;

  G4int PEventID;
  G4int PTrackID;
  G4int ParentID;
  std::string PParticleID;
  std::string Process;
  G4double xPriPos;
  G4double yPriPos;
  G4double zPriPos;
  G4double PEkin;
  G4double xDir;
  G4double yDir;
  G4double zDir;

  G4String fOutputFolder;
  TFile* ResultFile;
  TTree* tree;
  TTree* GeHitTree;
  TTree* MuonHitTree;
  TTree* PrimariesTree;
  TTree* RunTree;

  TTree* ftree;
  TTree* fGeHitTree;
  TTree* fMuonHitTree;
  TTree* fPrimariesTree;

  GeMSE_Analysis* fRunAnalysis;
  GeMSE_RunMessenger* runMessenger;
};

#endif
