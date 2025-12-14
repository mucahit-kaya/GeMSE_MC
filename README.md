# GeMSE_MC

Geant4 code to simulate the GeMSE detector efficiency

[![CodeFactor](https://www.codefactor.io/repository/github/ag-schumann/gemse_mc/badge)](https://www.codefactor.io/repository/github/ag-schumann/gemse_mc)

## Compilation

If you are sitting on top of the [GeMSE_environment](https://github.com/AG-Schumann/GeMSE_environment), run:
```bash
git clone https://github.com/AG-Schumann/GeMSE_MC.git
cd GeMSE_MC
source /opt/GeMSE/setup_sims.sh
make -j # "-j" for parallel compilation using all available CPU
```
and your executables will be built under `./bin/Linux-g++/GeMSE_MC`, where `.` is the default working directory (`$G4WORKDIR`).

If running locally, make sure you have ROOT and Geant4 (10.3 recommended) installed. Compilation errors will arise because of the nonexistent paths that the `CADMesh.hh` class needs to execute. You can either install these extra libraries (ask Diego) or just run the code without CADMesh, assuming you do not want to simulate a 3D sample. In order to do that:
+ remove the [include/CADMesh.hh](include/CADMesh.hh) file from its directory,
+ comment out these lines from the top of the [src/GeMSE_DetectorConstruction.cc](src/GeMSE_DetectorConstruction.cc) file:      
    ```cpp
    #define USE_CADMESH_TETGEN // To use tetgen
    #include <CADMesh.hh>
    ```
+ comment out these two lines from the `Makefile`
    ```make
    EXTRALIBS +=/opt/GeMSE/assimp/bin/libassimp.so
    EXTRALIBS +=/opt/GeMSE/tetgen/tetlib.so
    ```
and you're ready to go:
```bash
git clone https://github.com/AG-Schumann/GeMSE_MC.git
cd GeMSE_MC
export G4WORKDIR=${PWD} # optional, otherwise defaulted to /home/<username>/geant4workdir/
make -j # "-j" for parallel compilation using all available CPU
```

## Execution

Run the executable with:
```bash
$G4WORKDIR/bin/Linux-g++/GeMSE_MC -m <macrofile.mac> -o <results_folder> -g <sample_geometry.txt>
```
where the `-g` option is only for the cases in which you define your sample shape via txt file (see [sample_geometries](sample_geometries) README).

### macrofile.mac
* Contains the information which isotopes or gamma lines are simulated
* See example [macros/
efficiency_standard_isotopes_G4103p3.mac ](macros/
efficiency_standard_isotopes_G4103p3.mac ), where the various output tree option commands and gamma efficiency settings are explained.
* For visualization only use `macros/<visualization_macro.mac>` (no need for `-o` argument in this case).

### results_folder
* Folder where result files are written. **No output will be written if this argument is not specified**

## Output format

The simulation output is a `*.root` file with several layers of information:
* TNamed `G4VERSION_TAG`, with the information on the Geant4 version used during the run
* TNamed `MCVERSION_TAG`, with the information on the GeMSE_MC version used during the run
* TTree `RunInfo`: tree with basic run information
  * TBranch `nEvents`: number of simulated events
  * TBranch `nDecays`: number of primary decays in the run (i.e., decays on the primary particle)
  * TBranch `RunTime`: guess what
  * TBranch `Seed`: random seed of the run, so that you can reproduce it
* TTree `tree`: tree with information about detection efficiency and branching ratio (emission probability) for every gamma line chosen (empty branches if no `/gammaline/` commands are provided in the run macro)
  * TBranch `energy`: energy of gamma line
  * TBranch `efficiency`: detection efficiency
  * TBranch `efficiency_err`: statistical uncertainty of detection efficiency
  * TBranch `eff_BR`: product of detection efficiency and branching ratio
* TTree `GeHits`: tree with information on every particle hit (energy deposit > 0) in the Ge
  * TBranch `EventID`: run event id
  * TBranch `NHits`: number of Ge hits for a given event
  * TBranch `TotEdep`: total energy deposit in the Ge crystal for a given event
  * TBranch `TrackID`: id of the tracks producing the hits in the Ge crystal
  * TBranch `ParticleID`: Geant4 particle identifier for the hits
  * TBranch `Edep`: energy deposit of the individual hits
  * TBranch `xPos`, `yPos`, `zPos`: position of the individual hits
  * TBranch `Time`: time of the individual hits
  * TBranch `Ekin`: kinetic energy of the particle at the time of the hit
* TTree `MuonHits`: tree with information on every particle hit in the muon veto panels
  * Contains the exact same branches as the `GeHits` tree, with the addition of `PanelNr`, to extract the information on the panel that saw the hit (`0` for top, `1` for side panel).
* TTree `Primaries`: Tree with information on all generated particles in the run (deactivated by default, add `/writePrimaries true` to your macro to fill the branches below)
  * TBranch `EventID` 
  * TBranch `TrackID` 
  * TBranch `ParentID`: run id of the parent particle
  * TBranch `xPriPos`, `yPriPos`, `zPriPos`: primary position
  * TBranch `Ekin`
  * TBranch `ParticleID`
  * TBranch `xDir`, `yDir`, `zDir`: initial momentum direction

*Note*: The `tree` tree must keep that name for backwards compatibility with simulations and analysis scripts.

## Import 3D CAD samples

The [CADMesh](https://github.com/christopherpoole/CADMesh) interface, specifically [v2.0.3](https://github.com/christopherpoole/CADMesh/releases/tag/v2.0.3) is integrated into this code. This means that one can import CAD samples in STL, PLY or OBJ format into Geant4 for their construction. See an example in [sample_geometries/banana_scan.cc](sample_geometries/banana_scan.cc).

*Note*: Contrary to the CADMesh version used up to (and including) [GeMSE_MC v1.2.0](https://github.com/AG-Schumann/GeMSE_MC/releases/tag/v1.2.0), this one does not support binary STL files (see [sample_geometries/scans_3d/banana_LRT_talk_binary.stl](sample_geometries/scans_3d/banana_LRT_talk_binary.stl)). These can be converted to ASCII using commercial CAD software.


