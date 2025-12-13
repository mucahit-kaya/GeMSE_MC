#include "GeMSE_PhysicsList.hh"

#include "TFile.h"
#include "TH1.h"
#include "TNamed.h"
#include "TParameter.h"

#include <G4DecayPhysics.hh>
#include <G4EmCalculator.hh>
#include <G4EmExtraPhysics.hh>
#include <G4EmPenelopePhysics.hh>
#include <G4EmStandardPhysics.hh>
#include <G4HadronicProcessStore.hh>
#include <G4NistManager.hh>
#include <G4OpticalPhysics.hh>
#include <G4StoppingPhysics.hh>
#include <G4ios.hh>
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4HadronInelasticQBBC.hh"
#include "G4HadronPhysicsFTFP_BERT_HP.hh"
#include "G4HadronPhysicsShielding.hh"
#include "G4HadronPhysicsINCLXX.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4IonElasticPhysics.hh"
#include "G4IonINCLXXPhysics.hh"
#include "G4IonPhysics.hh"
//#include "GammaPhysics.hh" see Hadr03
#include "G4EmLivermorePhysics.hh"
#include "G4PhysListFactory.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4VModularPhysicsList.hh"

// GeMSE_PhysicsList::GeMSE_PhysicsList(G4String fName):G4VModularPhysicsList()
GeMSE_PhysicsList::GeMSE_PhysicsList() : G4VModularPhysicsList() {
  defaultCutValue = 1.0 * mm;
  cutForGamma = defaultCutValue;
  cutForElectron = defaultCutValue;
  
cutForPositron = defaultCutValue;

  VerboseLevel = 0;
  SetVerboseLevel(VerboseLevel);

  /// G4OpticalPhysics
  //  creates the following particles:
  //     bosons:  G4OpticalPhoton
  //
  //  and adds the following physical processes to these particles:
  //     G4OpAbsorption, G4OpRayleigh, G4OpMieHG, G4OpBoundaryProcess, G4OpWLS,
  //     G4Scintillation, G4Cerenkov
  RegisterPhysics(GeG4OpticalPhysics = new G4OpticalPhysics(VerboseLevel));
  //((G4OpticalPhysics*)GeG4OpticalPhysics)->Configure(kCerenkov, false); --> not compatible Geant4 11.x and above
  auto opticalParams = G4OpticalParameters::Instance();
  opticalParams->SetProcessActivation("Cerenkov", false); //Compatible with  Geant4 11.x 

  // Hadron Elastic scattering
  RegisterPhysics(new G4HadronElasticPhysicsHP(VerboseLevel));

  // Hadron Inelastic Physics
  RegisterPhysics(new G4HadronPhysicsQGSP_BERT_HP(VerboseLevel));
  // RegisterPhysics(new G4HadronPhysicsShielding(VerboseLevel));
  // RegisterPhysics(new G4HadronStoppingProcess(VerboseLevel));

  // Ion Elastic scattering
  RegisterPhysics(new G4IonElasticPhysics(VerboseLevel));

  // Ion Inelastic Physics
  RegisterPhysics(new G4IonPhysics(VerboseLevel));
  ////RegisterPhysics( new G4IonINCLXXPhysics(VerboseLevel));

  // Gamma-Nuclear Physics - see Hadr03
  // RegisterPhysics( new GammaPhysics("gamma") );

  // EM physics
  RegisterPhysics(new G4EmLivermorePhysics(VerboseLevel, ""));

  /// G4EmExtraPhysics
  //  creates the following particles:
  //     bosons:  G4Gamma
  //     leptons: G4Electron, G4Positron, G4MuonPlus, G4MuonMinus
  //
  //  and adds the following physical processes to these particles:
  //     G4SynchrotronRadiation, G4MuNuclearInteraction, G4ElectroNuclearBuilder
  RegisterPhysics(new G4EmExtraPhysics(VerboseLevel));

  // Decay
  /// G4DecayPhysics
  //  creates the following particles:
  //     bosons:     G4BosonConstructor
  //     leptons:    G4LeptonConstructor
  //     mesons:     G4MesonConstructor
  //     baryons:    G4BaryonConstructor
  //     ions:       G4IonConstructor
  //     resonances: G4ShortLivedConstructor
  //
  //  and adds the following physical processes to these particles:
  //     G4Decay
  RegisterPhysics(new G4DecayPhysics(VerboseLevel));

  // Radioactive decay
  RegisterPhysics(new G4RadioactiveDecayPhysics(VerboseLevel));

  /// G4StoppingPhysics
  //  like G4CaptureAtRestPhysics, but uses G4MuonMinusCaptureAtRest for muons
  RegisterPhysics(new G4StoppingPhysics(VerboseLevel));

  // physRootFile = fName;
}

GeMSE_PhysicsList::~GeMSE_PhysicsList() {}

void GeMSE_PhysicsList::SetCuts() {
  if (VerboseLevel > 1) G4cout << "GeMSE_PhysicsList::SetCuts:";

  if (VerboseLevel > 0) {
    G4cout << "GeMSE_PhysicsList::SetCuts:";
    G4cout << "CutLength : " << G4BestUnit(defaultCutValue, "Length") << G4endl;
  }

  // special for low energy physics
  G4double lowlimit = 250. * eV;

  G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(lowlimit,
                                                                  100. * GeV);

  // SetCutValue(0*mm, "proton");
  SetCutValue(cutForElectron, "e-");
  SetCutValue(cutForPositron, "e+");
  SetCutValue(cutForGamma, "gamma");
}
