#include "GeMSE_ActionInitialization.hh"

#include "GeMSE_RunAction.hh"
#include "GeMSE_PrimaryGeneratorAction.hh"
#include "GeMSE_TrackingAction.hh"

GeMSE_ActionInitialization::GeMSE_ActionInitialization(const G4String& outputFolder,
                                                       const G4String& gitTag)
  : G4VUserActionInitialization(),
    fOutputFolder(outputFolder),
    fGitTag(gitTag)
{}


void GeMSE_ActionInitialization::BuildForMaster() const
{
  // Only RunAction, usually OutputFolder is empty / separate.
  auto* runAction = new GeMSE_RunAction(fOutputFolder);
  runAction->SetVersionTag(std::string(fGitTag));
  SetUserAction(runAction);
}


void GeMSE_ActionInitialization::Build() const
{
  // Worker thread actions (and single-thread)
  auto* runAction = new GeMSE_RunAction(fOutputFolder);
  runAction->SetVersionTag(std::string(fGitTag));
  SetUserAction(runAction);

  SetUserAction(new GeMSE_PrimaryGeneratorAction());
  SetUserAction(new GeMSE_TrackingAction());
}

