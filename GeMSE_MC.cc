#include "GeMSE_DetectorConstruction.hh"
#include "GeMSE_PhysicsList.hh"
//#include "GeMSE_PrimaryGeneratorAction.hh"
//#include "GeMSE_RunAction.hh"
#include "GeMSE_TrackingAction.hh"
//#include "GeMSE_SensitiveDetector.hh"
#include "GeMSE_ActionInitialization.hh"

//#include <Randomize.hh>
#include <TCanvas.h>
#include <TFile.h>
#include <TString.h>
#include <TSystem.h>
#include <TTree.h>
#include <getopt.h>
#include <time.h>

#include "G4RunManager.hh"
#ifdef G4MULTITHREADED
  #include "G4MTRunManager.hh"
  #include "G4Threading.hh"
#endif
#include "G4UImanager.hh"
#include "G4UItcsh.hh"
#include "G4UIterminal.hh"
#include "G4VisExecutive.hh"
#include "G4ios.hh"
#include "TROOT.h"
#include "TH1D.h"


int main(int argc, char** argv)
{
  std::string git_tag = "2.0.0";
  G4cout << G4endl << "Welcome to GeMSE_MC version " << git_tag.c_str()
         << G4endl;

  // User Verbose output class
  //
  /*G4VSteppingVerbose* verbosity = new GeMSE_SteppingVerbose;
  G4VSteppingVerbose::SetInstance(verbosity);*/
  //-----------------------------

  int c = 0;
  bool Macro = false;
  G4String MacroFilename;
  G4String GeometryFilename = "src/worldVolume.txt";
  G4String OutputFolder = "";
  int nThreads=1;

  while ((c = getopt(argc, argv, "m:o:g:t:")) != -1) {
    switch (c) {
      case 'm':
        Macro = true;
        MacroFilename = optarg;
        break;

      case 'g':
        GeometryFilename = optarg;
        break;

      case 'o':
        OutputFolder = optarg;
        break;

      case 't':
        nThreads = std::max(1, std::stoi(optarg));
        break;
    }
  }
  
  // Check if output folder was specified
  if (OutputFolder == "") {
    G4cout << "\n######"
           << "\n###### WARNING: no folder specified, no output will be written "
           << "\n######" << G4endl;
    sleep(1.5);
  }

  // Enable ROOT thread safety in MT mode
  #ifdef G4MULTITHREADED
  if (nThreads > 1) {
    ROOT::EnableThreadSafety();
    G4cout << "ROOT thread safety enabled" << G4endl;
  }
  #endif

  // Run manager
  G4RunManager* runManager = nullptr;

  #ifdef G4MULTITHREADED
    runManager = new G4MTRunManager();
    runManager->SetNumberOfThreads(nThreads);
  #else
    runManager = new G4RunManager();
  #endif
  
  G4cout << "\nUsing " << runManager->GetNumberOfThreads() << " thread(s).\n" << G4endl;

  // User Initialization classes - mandatory
  G4VUserDetectorConstruction* detector =
      new GeMSE_DetectorConstruction(GeometryFilename);
  runManager->SetUserInitialization(detector);

  G4VUserPhysicsList* physics = new GeMSE_PhysicsList;
  runManager->SetUserInitialization(physics);

  // User Action Initialization
  G4VUserActionInitialization *actions = new GeMSE_ActionInitialization(OutputFolder, git_tag);
  runManager->SetUserInitialization(actions);

  // Initialize G4 kernel
   runManager->Initialize(); 

  // Visualization manager - ONLY for single thread or interactive mode
  G4VisManager* visManager = nullptr;
  
  // Visualization manager -- for only 1 thread
  if (runManager->GetNumberOfThreads() == 1) {  // MT mode check
    G4cout << "Initializing Visualization Manager" << G4endl;
    visManager = new G4VisExecutive;
    visManager->Initialize();
  }


  // Shell & Visualization
  if (!Macro) {
    // Define UI session for interactive mode
    G4UIsession* session = new G4UIterminal();
    session->SessionStart();
    delete session;
  }
  else {
    // Get the pointer to the User Interface manager
    G4UImanager* UI = G4UImanager::GetUIpointer();
    G4String command = "/control/execute " + MacroFilename;
    UI->ApplyCommand(command);
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !

  G4cout << "\n";
  delete visManager;
  delete runManager;
  //delete verbosity;

  return 0;
}
