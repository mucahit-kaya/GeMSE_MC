#ifndef GeMSE_ActionInitialization_h
#define GeMSE_ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "globals.hh"


class GeMSE_ActionInitialization : public G4VUserActionInitialization
{
public:
  GeMSE_ActionInitialization(const G4String& outputFolder,
                             const G4String& gitTag);
  virtual ~GeMSE_ActionInitialization() {}
 
  // Only for master (in MT mode)
  virtual void BuildForMaster() const;

  // for Worker threads + single-thread run
  virtual void Build() const;


private:
  G4String fOutputFolder;
  G4String fGitTag;
};

#endif