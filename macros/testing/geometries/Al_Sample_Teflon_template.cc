////////////////////////////////////////////////////////////
// To be called from the GeMSE_DetectorConstruction class //
////////////////////////////////////////////////////////////

//Cubic Aluminum sample with Teflon holder
//Sample  2024-12-08
//Aluminum cube of 3.9cm side length, weight not measured, I will make approximation later


// Materials
// For cube
G4Element* Al = nist->FindOrBuildElement("Al");
G4Material *Al_mat = new G4Material("Al_Cube",2.70*g/cm3,1,kStateSolid);
  Al_mat->AddElement(Al,1);

//For holder
// G4Material* Teflon = G4Material::GetMaterial("Teflon");
//G4Material* teflon_mat = nist->FindOrBuildMaterial("G4_TEFLON"); // -> Already imported in GeMSE_DetectorConstruction.cc

// Sample position
G4RotationMatrix rmr;
rmr.rotateX(0);
rmr.rotateZ(0); // No rotation needed
// From txt file, we know :PLACE  Sample_Al  1    World R0 0. 0. (3.95+1.95)*cm

//Just for aluminium for testing right now
G4ThreeVector sample_pos = G4ThreeVector(-1.95*cm, 0, 0*cm); // Adjusted to have bottom at 0 cm

G4LogicalVolume *al_sample_logical;
G4LogicalVolume *ptfe_holder_logical;


// Import sample and holder from text file
auto al_sample_mesh = CADMesh::TessellatedMesh::FromSTL
   ("custom/geometries/stl/v1/Al_Cube_v1_ascii.stl");
al_sample_mesh->SetScale(1.0); // Default is 1, corresponding to mm in Geant4
al_sample_mesh->SetOffset(G4ThreeVector(0*mm, -19.5*mm, 59.5*mm)); // Centering the cube at origin
auto al_sample_solid = al_sample_mesh->GetSolid();
al_sample_logical = new G4LogicalVolume(al_sample_solid,Al_mat,
                                     "al_sample_logical", 0, 0, 0);
new G4PVPlacement(G4Transform3D(rmr, sample_pos), al_sample_logical,
                  "al_sample", expHall_log, false, 0);

// Import holder
auto ptfe_holder_mesh = CADMesh::TessellatedMesh::FromSTL("custom/geometries/stl/v1/Al_Cube_Teflon_Holder_v1_ascii.stl");
ptfe_holder_mesh->SetScale(1.0);
ptfe_holder_mesh->SetOffset(G4ThreeVector(0*mm, -19.5*mm, 29.5*mm)); // Centering the holder at origin
auto ptfe_holder_solid = ptfe_holder_mesh->GetSolid();
ptfe_holder_logical = new G4LogicalVolume(ptfe_holder_solid,teflon_mat,
                                     "ptfe_holder_logical", 0, 0, 0);
new G4PVPlacement(G4Transform3D(rmr, sample_pos), ptfe_holder_logical,
                  "ptfe_holder", expHall_log, false, 0);



const G4double al_sample_mass = al_sample_logical->GetMass(true)/g;
const G4double ptfe_holder_mass = ptfe_holder_logical->GetMass(true)/g;


G4cout << "\n############################" << G4endl;
G4cout << "Al sample mass: " << al_sample_mass << " g" << G4endl;
G4cout << "Teflon holder mass: " << ptfe_holder_mass << " g" << G4endl;
G4cout << "############################\n" << G4endl;


//Visualization G4VisAttributes
//White for teflon  
G4VisAttributes* white = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
//Grey-white for aluminium
G4VisAttributes* greywhite = new G4VisAttributes(G4Colour(0.9,0.9,0.9));
al_sample_logical->SetVisAttributes(greywhite);
ptfe_holder_logical->SetVisAttributes(white);