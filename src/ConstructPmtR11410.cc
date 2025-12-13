#include "GeMSE_DetectorConstruction.hh"
#include "GeMSE_SensitiveDetector.hh"

#include <TMath.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "globals.hh"

#include <G4SystemOfUnits.hh>
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4Cons.hh"
#include "G4EllipticalTube.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4PVPlacement.hh"
#include "G4Polycone.hh"
#include "G4RotationMatrix.hh"
#include "G4SDManager.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Torus.hh"
#include "G4Transform3D.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4VisAttributes.hh"

G4LogicalVolume* GeMSE_DetectorConstruction::ConstructPmtR11410() {
  // Colors for visualization properties
  G4Colour red(1.0, 0.0, 0.0);
  G4Colour blue(0.0, 0.0, 1.0);
  G4Colour yellow(1.0, 1.0, 0.0);
  G4Colour orange(0.75, 0.55, 0.0);
  G4Colour lblue(0.0, 0.0, 0.55);

  //------------------------------------------------------ materials
  G4double a;  // atomic mass
  G4double z;  // atomic number
  G4double density;
  G4double fractionmass;
  G4int ncomponents;
  G4String name, symbol;

  G4Element* H = new G4Element(name = "Hydrogen", symbol = "H", z = 1.,
                               a = 1.00794 * g / mole);
  G4Element* C = new G4Element(name = "Carbon", symbol = "C", z = 6.,
                               a = 12.011 * g / mole);
  G4Element* Fe = new G4Element(name = "Iron", symbol = "Fe", z = 26.,
                                a = 55.85 * g / mole);
  G4Element* Co = new G4Element(name = "Cobalt", symbol = "Co", z = 27.,
                                a = 58.9332 * g / mole);
  G4Element* N = new G4Element(name = "Nitrogen", symbol = "N", z = 7.,
                               a = 14.01 * g / mole);
  G4Element* O =
      new G4Element(name = "Oxygen", symbol = "O", z = 8., a = 16.0 * g / mole);
  G4Element* Al = new G4Element(name = "Aluminum", symbol = "Al", z = 13.,
                                a = 26.98 * g / mole);
  G4Element* F = new G4Element(name = "Flourine", symbol = "F", z = 9.,
                               a = 18.998 * g / mole);
  G4Element* Cr = new G4Element(name = "Chromium", symbol = "Cr", z = 24.,
                                a = 51.9961 * g / mole);
  G4Element* Ni = new G4Element(name = "Nickel", symbol = "Ni", z = 28.,
                                a = 58.6934 * g / mole);
  G4Element* Mn = new G4Element(name = "Manganese", symbol = "Mn", z = 25.,
                                a = 54.93805 * g / mole);
  // G4Element* Mo     = new G4Element(name="Molybdenum", symbol="Mo", z=42.,
  // a=95.94*g/mole); G4Element* Ti     = new G4Element(name="Titanium",
  // symbol="Ti", z=22., a=47.867*g/mole);
  G4Element* Si = new G4Element(name = "Silicon", symbol = "Si", z = 14.,
                                a = 28.0855 * g / mole);
  G4Element* P = new G4Element(name = "Phosphorus", symbol = "P", z = 15.,
                               a = 30.973762 * g / mole);
  G4Element* S = new G4Element(name = "Sulfur", symbol = "S", z = 16.,
                               a = 32.066 * g / mole);
  // G4Element* Na     = new G4Element(name="Sodium", symbol="Na", z=11.,
  // a=22.989769	*g/mole); G4Element* Mg     = new
  // G4Element(name="Magnesium", symbol="Mg", z=12., a=24.3050*g/mole);
  // G4Element* Ca     = new G4Element(name="Calcium", symbol="Ca", z=20.,
  // a=40.078*g/mole); G4Element* K = new G4Element(name="Potassium",
  // symbol="K", z=19., a=39.0983*g/mole); G4Element* Zn     = new
  // G4Element(name="Zinc", symbol="Zn", z=30., a=65.38*g/mole); G4Element* Th
  // = new G4Element(name="Thorium", symbol="Th", z=90., a=232.0381*g/mole);
  // G4Element* Au     = new G4Element(name="Gold", symbol="Au", z=79.,
  // a=196.9666*g/mole); G4Element* Pb_el  = new G4Element(name="Lead",
  // symbol="Pb", z=82., a=207.2*g/mole); G4Element* Cu_el  = new
  // G4Element(name="Copper", symbol="Cu", z=29., a=63.546*g/mole); G4Element* W
  // = new G4Element(name="Tungsten", symbol="W", z=74., a=183.84*g/mole);

  // G4Material* Pb = new G4Material("Lead", z=82., a=207.19*g/mole,
  // density=11.34*g/cm3);
  // G4Material* Pb2 = new G4Material("Lead3", z=82., a=207.19*g/mole,
  // density=11.34*g/cm3); G4Material* Pb4 = new G4Material("Lead4", z=82.,
  // a=207.19*g/mole, density=11.34*g/cm3);
  // G4Material* PoPb = new G4Material("PoPb", z=82., a=207.19*g/mole,
  // density=11.34*g/cm3);
  //  G4Material* CDMSPb = new G4Material("CDMSPb", z=82., a=207.19*g/mole,
  //  density=11.34*g/cm3);
  // G4Material* Cu = new G4Material("Copper", z=29., a=63.546*g/mole,
  // density=8.920*g/cm3);
  //  G4Material* Cu2 = new G4Material("Copper2", z=29., a=63.546*g/mole,
  //  density=8.920*g/cm3);
  // G4Material* Silver = new G4Material("Silver", z=13., a=107.868*g/mole,
  // density=10.49*g/cm3);
  G4Material* Aluminum =
      new G4Material("Aluminum", density = 2.7 * g / cm3, ncomponents = 1);
  Aluminum->AddElement(Al, fractionmass = 1.0);
  // G4Material* Ge = new G4Material("Germanium", z=32., a=72.64*g/mole,
  // density=5.332*g/cm3); G4Material* activeGe = new G4Material("activeGe",
  // z=32., a=72.64*g/mole, density=5.332*g/cm3); This is just a trick to change
  // the name of the same Germanium for the active part
  G4Material* Air =
      new G4Material(name = "Air", density = 1.290 * mg / cm3, ncomponents = 2);
  Air->AddElement(N, fractionmass = 0.7);
  Air->AddElement(O, fractionmass = 0.3);
  // G4Material* Vacuum = new G4Material(name="Vacuum", z=1., a=1.*g/mole,
  // density=1.e-20*g/cm3, kStateGas, temperature=0.1*kelvin,
  // pressure=1.e-20*bar);
  G4Material* Teflon = new G4Material(name = "Teflon", density = 2.20 * g / cm3,
                                      ncomponents = 2);
  Teflon->AddElement(C, 2);
  Teflon->AddElement(F, 4);
  G4Material* Teflon2 = new G4Material(
      name = "Teflon2", density = 2.20 * g / cm3, ncomponents = 2);
  Teflon2->AddElement(C, 2);
  Teflon2->AddElement(F, 4);
  G4Material* mylar = new G4Material(name = "mylar", density = 1.3925 * g / cm3,
                                     ncomponents = 3);
  mylar->AddElement(C, 10);
  mylar->AddElement(H, 8);
  mylar->AddElement(O, 8);
  G4Material* kapton = new G4Material(name = "kapton", density = 1.42 * g / cm3,
                                      ncomponents = 4);
  kapton->AddElement(C, 22);
  kapton->AddElement(H, 10);
  kapton->AddElement(O, 5);
  kapton->AddElement(N, 2);
  G4Material* kapton_F = new G4Material(
      name = "kapton_F", density = 1.59 * g / cm3, ncomponents = 4);
  kapton_F->AddElement(C, 22);
  kapton_F->AddElement(H, 10);
  kapton_F->AddElement(O, 5);
  kapton_F->AddElement(N, 2);
  G4Material* Steel_304 = new G4Material(
      name = "Steel_304", density = 8.03 * g / cm3, ncomponents = 8);
  Steel_304->AddElement(Fe, fractionmass = 0.6944);
  Steel_304->AddElement(C, fractionmass = 0.005);
  Steel_304->AddElement(Cr, fractionmass = 0.185);
  Steel_304->AddElement(Ni, fractionmass = 0.10);
  Steel_304->AddElement(Mn, fractionmass = 0.01);
  Steel_304->AddElement(Si, fractionmass = 0.005);
  Steel_304->AddElement(P, fractionmass = 0.0004);
  Steel_304->AddElement(S, fractionmass = 0.0002);
  //   G4Material* SS316L = new G4Material("SS316L", density=8.00*g/cm3,
  //   ncomponents=9, kStateSolid);
  //     SS316L->AddElement(Fe, fractionmass=0.6075);
  //     SS316L->AddElement(C , fractionmass= 0.008);
  //     SS316L->AddElement(Cr, fractionmass=0.17);
  //     SS316L->AddElement(Ni, fractionmass=0.12);
  //     SS316L->AddElement(Mo, fractionmass= 0.025);
  //     SS316L->AddElement(Mn, fractionmass= 0.02);
  //     SS316L->AddElement(Si, fractionmass= 0.0075);
  //     SS316L->AddElement(P, fractionmass=0.00041);
  //     SS316L->AddElement(N, fractionmass=0.001);
  G4Material* Cirlex = new G4Material(name = "Cirlex", density = 1.43 * g / cm3,
                                      ncomponents = 4);
  Cirlex->AddElement(C, 22);
  Cirlex->AddElement(H, 10);
  Cirlex->AddElement(N, 2);
  Cirlex->AddElement(O, 5);
  G4Material* Cirlex_sim = new G4Material(
      name = "Cirlex_sim", density = 0.59 * g / cm3, ncomponents = 4);
  Cirlex_sim->AddElement(C, 22);
  Cirlex_sim->AddElement(H, 10);
  Cirlex_sim->AddElement(N, 2);
  Cirlex_sim->AddElement(O, 5);
  G4Material* pmt_material = new G4Material(
      name = "pmt_material", density = 1.19 * g / cm3, ncomponents = 3);
  pmt_material->AddElement(Fe, fractionmass = 0.88);
  pmt_material->AddElement(C, fractionmass = 0.04);
  pmt_material->AddElement(Co, fractionmass = 0.08);
  G4Material* Polyethylene = new G4Material(
      name = "Polyethylene", density = 0.95 * g / cm3, ncomponents = 2);
  Polyethylene->AddElement(C, 2);
  Polyethylene->AddElement(H, 4);
  G4Material* poly_material = new G4Material(
      name = "poly_material", density = 1.75 * g / cm3, ncomponents = 2);
  poly_material->AddElement(C, 2);
  poly_material->AddElement(F, 4);
  G4Material* Ceramic = new G4Material(
      name = "Ceramic", density = 3.75 * g / cm3, ncomponents = 2);
  Ceramic->AddElement(Al, 2);
  Ceramic->AddElement(O, 3);
  G4Material* Polystyrene = new G4Material(
      name = "Polystyrene", density = 1.06 * g / cm3, ncomponents = 2);
  Polystyrene->AddElement(C, fractionmass = 0.922582);
  Polystyrene->AddElement(H, fractionmass = 0.077418);
  // NOTE: This Kovar is Co free, whereas in normal Kovar there is about 16% of
  // Co
  G4Material* PMTkovar_mat =
      new G4Material("PMTkovar_mat", density = 8.36 * g / cm3, 2);
  PMTkovar_mat->AddElement(Fe, 0.71);
  PMTkovar_mat->AddElement(Ni, 0.29);

  // G4Material* Steel_304 = G4Material::GetMaterial("Steel_304");
  G4Material* kovar = G4Material::GetMaterial("PMTkovar_mat");

  // Define the pmt envelope as a polycon with 4 z-surfaces (maybe more
  // z-surfaces is better)
  const G4double PMTOD_ext_bot = 76 * mm;
  const G4double PMTOD_ext_top = 53 * mm;
  const G4double PMT_h1 = 30 * mm;
  const G4double PMT_h2 = 13 * mm;
  const G4double PMT_h3 = 71 * mm;

  G4double PMT_envel_IRads[] = {0, 0, 0, 0};
  G4double PMT_envel_ORads[] = {PMTOD_ext_bot / 2., PMTOD_ext_bot / 2.,
                                PMTOD_ext_top / 2., PMTOD_ext_top / 2.};
  G4double PMT_envel_zPlanes[] = {0, PMT_h1, PMT_h1 + PMT_h2,
                                  PMT_h1 + PMT_h2 + PMT_h3};

  G4Polycone* PMT_envel =
      new G4Polycone("PMT_envel", 0, 360 * deg, 4, PMT_envel_zPlanes,
                     PMT_envel_IRads, PMT_envel_ORads);

  // Envelope material definition (SS)
  G4Material* PMT_steel = new G4Material("PMT_steel", 8.03 * g / cm3, 1);
  PMT_steel->AddMaterial(
      Steel_304,
      1.0);  // Just a trick to change the name of the steel ==> I can use the
             // routine to generate the random points in the proper material

  // Envelope Logical volume
  G4LogicalVolume* PMT_envel_log =
      new G4LogicalVolume(PMT_envel, kovar, "PMT_envel_log", 0, 0, 0);

  G4VisAttributes* PMT_envel_vis = new G4VisAttributes(red);
  PMT_envel_vis->SetVisibility(true);
  PMT_envel_vis->SetForceSolid(false);
  PMT_envel_log->SetVisAttributes(PMT_envel_vis);

  /******************************** Definition of the vacuum inside the PMTs
   * *************************************/
  // Define the pmt vacuum as a polycon with 5 z-surfaces (maybe more z-surfaces
  // is better)
  const G4double PMT_thickness =
      0.8 * mm;  // With this thickness the mass of the envelope is optimized to
                 // 101 g about (it fluctuates from run to run)

  const G4double PMTOD_int_bot = PMTOD_ext_bot - PMT_thickness;
  const G4double PMTOD_int_top = PMTOD_ext_top - PMT_thickness;
  const G4double PMTceram_OD =
      35. * mm;  // Put it before the definition of the ceramic
  const G4double PMTceram_ID = 0. * mm;
  // const G4double PMT_h1 = 30 * mm;
  // const G4double PMT_h2 = 13 * mm;
  // const G4double PMT_h3 = 71 * mm;

  G4double PMT_vacuum_IRads[] = {0, 0, 0, 0};
  G4double PMT_vacuum_ORads[] = {PMTOD_int_bot / 2., PMTOD_int_bot / 2.,
                                 PMTOD_int_top / 2., PMTOD_int_top / 2.};
  G4double PMT_vacuum_zPlanes[] = {0, PMT_h1, PMT_h1 + PMT_h2,
                                   (PMT_h1 + PMT_h2 + PMT_h3) - PMT_thickness};

  G4Polycone* PMT_vacuum_poly =
      new G4Polycone("vacuum_PMT_poly", 0, 360 * deg, 4, PMT_vacuum_zPlanes,
                     PMT_vacuum_IRads, PMT_vacuum_ORads);
  G4Tubs* PMT_vacuum_tubs =
      new G4Tubs("vacuum_PMT_tubs", 0.5 * PMTceram_ID, 0.5 * PMTceram_OD,
                 PMT_thickness, 0. * deg, 360. * deg);
  G4UnionSolid* PMT_vacuum = new G4UnionSolid(
      "vacuum_PMT", PMT_vacuum_poly, PMT_vacuum_tubs, 0,
      G4ThreeVector(0., 0., PMT_h1 + PMT_h2 + PMT_h3 - PMT_thickness));

  // Material definition for the vacuum inside the PMT
  G4Material* PMT_vacuum_mat =
      new G4Material("vacuum_PMT_mat", 1., 1. * g / mole, 1.e-20 * g / cm3,
                     kStateGas, 0.1 * kelvin, 1.e-20 * bar);

  // Vacuum Logical volume
  G4LogicalVolume* PMT_vacuum_log = new G4LogicalVolume(
      PMT_vacuum, PMT_vacuum_mat, "vacuum_PMT_log", 0, 0, 0);

  G4VisAttributes* PMT_vacuum_vis = new G4VisAttributes(yellow);
  PMT_vacuum_vis->SetVisibility(true);
  PMT_vacuum_vis->SetForceSolid(false);
  PMT_vacuum_log->SetVisAttributes(G4VisAttributes::GetInvisible());

  // Placement of the vacuum logical volume inside the PMT envelope volume
  G4ThreeVector PTM_vacuum_pos(0., 0., 0.);

  new G4PVPlacement(0, PTM_vacuum_pos, PMT_vacuum_log, "vacuum_PMT_phys",
                    PMT_envel_log, false, 0);

  /******************************** Definition of the ceramic stem to put in
   * PMTs *************************************/
  // Material definition for the ceramic inside the pmt
  G4Material* PMT_ceram_mat =
      new G4Material("PMT_ceram_mat", 2.811 * g / cm3, 2);
  PMT_ceram_mat->AddElement(Al, 2);
  PMT_ceram_mat->AddElement(O, 3);

  // Dimension of each cilinder
  // G4double PMTceram_ID = 0.*mm;
  // G4double PMTceram_OD = 35.*mm;
  G4double PMTceram_H = 5.916 * mm;

  // Definition of the geometry
  G4Tubs* PMT_ceram =
      new G4Tubs("PMT_ceram", 0.5 * PMTceram_ID, 0.5 * PMTceram_OD,
                 0.5 * PMTceram_H, 0. * deg, 360. * deg);

  // Construction of logical volume
  G4LogicalVolume* PMT_ceram_log =
      new G4LogicalVolume(PMT_ceram, PMT_ceram_mat, "PMT_ceram_log");

  // Set visibility properties
  G4VisAttributes* PMT_ceram_vis = new G4VisAttributes(orange);
  PMT_ceram_vis->SetVisibility(true);
  PMT_ceram_vis->SetForceSolid(false);
  PMT_ceram_log->SetVisAttributes(PMT_ceram_vis);

  // Put the ceramic stem in the PMT_envelop logical volume
  G4ThreeVector PMT_ceram_shift(0., 0.,
                                (PMT_h1 + PMT_h2 + PMT_h3) - (PMTceram_H / 2.));

  new G4PVPlacement(0, PMT_ceram_shift, PMT_ceram_log, "PMT_ceram_phys",
                    PMT_vacuum_log, false, 0);

  /********************* Definition of the quartz window as a flat thin cilinder
   * inside the PMTs vacuum ********************/
  // Material definition for the ceramic inside the pmt
  G4Material* PMT_quartz_mat =
      new G4Material("PMT_quartz_mat", 2.648 * g / cm3, 2);
  PMT_quartz_mat->AddElement(Si, 1);
  PMT_quartz_mat->AddElement(O, 2);

  // Dimension of each cilinder
  G4double PMTwindow_ID = 0. * mm;
  G4double PMTwindow_OD = 70.5 * mm;
  G4double PMTwindow_H = 3.1 * mm;

  // Definition of the geometry
  G4Tubs* PMT_window =
      new G4Tubs("PMT_window", 0.5 * PMTwindow_ID, 0.5 * PMTwindow_OD,
                 0.5 * PMTwindow_H, 0. * deg, 360. * deg);

  // Construction of logical volume
  G4LogicalVolume* PMT_window_log =
      new G4LogicalVolume(PMT_window, PMT_quartz_mat, "PMT_window_log");

  // Set visibility properties
  G4VisAttributes* PMT_window_vis = new G4VisAttributes(blue);
  PMT_window_vis->SetVisibility(true);
  PMT_window_vis->SetForceSolid(false);
  PMT_window_log->SetVisAttributes(PMT_window_vis);

  // Put the ceramic stem in the PMT_envelop logical volume
  G4ThreeVector PMT_window_shift(0., 0., PMTwindow_H / 2.);

  new G4PVPlacement(0, PMT_window_shift, PMT_window_log, "PMT_window_phys",
                    PMT_vacuum_log, false, 0);

  /********************* Definition of the stuff inside a cilinder inside the
   * PMTs vacuum ********************/
  // Dimension of the cilinder and mass of the internal cylinder
  G4double PMTinternal_ID = 0. * mm;
  G4double PMTinternal_OD = 35.0 * mm;
  G4double PMTinternal_H = 35.0 * mm;
  // G4double PMTinternal_mass = 33.8*g;

  // G4double PMTinternal_dens =
  // PMTinternal_mass/(TMath::Pi()*pow(PMTinternal_OD/2,2)*PMTinternal_H);

  // Material definition as a mesh respecting the mass model
  G4double mfrac_SiO2 = 0.426;             // This is for quartz
  G4double mfrac_SS304 = 1. - mfrac_SiO2;  // This for some electrodes
  // G4double mfrac_Ni = 1.-(mfrac_SiO2+mfrac_SS304);

  G4Material* PMT_nickel_mat =
      new G4Material("PMT_nickel_mat", 8.908 * g / cm3, 1);
  PMT_nickel_mat->AddElement(Ni, 1);

  G4Material* PMT_internal_mat =
      new G4Material("PMT_internal_mat", 1.014 * g / cm3, 2);
  PMT_internal_mat->AddMaterial(PMT_quartz_mat, mfrac_SiO2);
  PMT_internal_mat->AddMaterial(Steel_304, mfrac_SS304);
  // PMT_internal_mat -> AddMaterial(PMT_nickel_mat,mfrac_Ni);

  // Definition of the geometry
  G4Tubs* PMT_internal =
      new G4Tubs("PMT_internal", 0.5 * PMTinternal_ID, 0.5 * PMTinternal_OD,
                 0.5 * PMTinternal_H, 0. * deg, 360. * deg);

  // Contruction of logical volume
  G4LogicalVolume* PMT_internal_log =
      new G4LogicalVolume(PMT_internal, PMT_internal_mat, "PMT_internal_log");

  // Set visibility properties
  G4VisAttributes* PMT_internal_vis = new G4VisAttributes(lblue);
  PMT_internal_vis->SetVisibility(true);
  PMT_internal_vis->SetForceSolid(false);
  PMT_internal_log->SetVisAttributes(PMT_internal_vis);

  // Put the ceramic stem in the PMT_envelop logical volume
  G4ThreeVector PMT_internal_shift(0., 0.,
                                   (PMT_h1 + PMT_h2 + PMT_h3) -
                                       (PMTinternal_H / 2.) -
                                       (PMTceram_H + 1. * cm));

  new G4PVPlacement(0, PMT_internal_shift, PMT_internal_log,
                    "PMT_internal_phys", PMT_vacuum_log, false, 0);

  /********************* Definition of the kovar front flange the PMTs
   * ********************/
  // Code this like a ring inside the first bottom segmant of the PMT (from the
  // photocathode window)

  // Define the fron flange dimensions
  G4double PMT_frontflange_OD = PMTOD_ext_bot;
  G4double PMT_frontflange_ID = PMT_frontflange_OD - PMT_thickness;
  G4double PMT_frontflange_H = PMT_h1;

  // Definition of the geometry
  G4Tubs* PMT_frontflange = new G4Tubs(
      "PMT_frontflange", 0.5 * PMT_frontflange_ID, 0.5 * PMT_frontflange_OD,
      0.5 * PMT_frontflange_H, 0. * deg, 360. * deg);

  // Construction of the logical volume
  G4LogicalVolume* PMT_frontflange_log =
      new G4LogicalVolume(PMT_frontflange, kovar, "PMT_frontflange_log");

  // Set visibility properties
  G4VisAttributes* PMT_frontflange_vis = new G4VisAttributes(lblue);
  PMT_frontflange_vis->SetVisibility(true);
  PMT_frontflange_vis->SetForceSolid(false);
  PMT_frontflange_log->SetVisAttributes(PMT_frontflange_vis);

  // consider that the mother volume (the envelope) has z=0 at the surface level
  // of the quartz window
  G4ThreeVector PMT_frontflange_shift(0., 0., PMT_frontflange_H / 2.);

  new G4PVPlacement(0, PMT_frontflange_shift, PMT_frontflange_log,
                    "PMT_frontflange_phys", PMT_envel_log, false, 0);

  /********************* Definition of the kovar back flange of the PMTs
   * ********************/
  // This will be a ring with an internal radius the same as the ceramic
  // external radius

  G4double PMT_backflange_OD = PMTOD_ext_top;
  G4double PMT_backflange_ID = PMTceram_OD;
  G4double PMT_backflange_H = PMT_thickness;

  // Definition of the geometry
  G4Tubs* PMT_backflange = new G4Tubs(
      "PMT_backflange", 0.5 * PMT_backflange_ID, 0.5 * PMT_backflange_OD,
      0.5 * PMT_backflange_H, 0. * deg, 360. * deg);

  // Construction of the logical volume
  G4LogicalVolume* PMT_backflange_log =
      new G4LogicalVolume(PMT_backflange, kovar, "PMT_backflange_log");

  // Set visibility properties
  G4VisAttributes* PMT_backflange_vis = new G4VisAttributes(lblue);
  PMT_backflange_vis->SetVisibility(true);
  PMT_backflange_vis->SetForceSolid(false);
  PMT_backflange_log->SetVisAttributes(PMT_backflange_vis);

  // Consider that the mother volume (the envelope) has z=0 at the surface level
  // of the quartz window
  G4ThreeVector PMT_backflange_shift(
      0., 0., (PMT_h1 + PMT_h2 + PMT_h3) - PMT_backflange_H / 2.);

  new G4PVPlacement(0, PMT_backflange_shift, PMT_backflange_log,
                    "PMT_backflange_phys", PMT_envel_log, false, 0);

  return PMT_envel_log;
}
