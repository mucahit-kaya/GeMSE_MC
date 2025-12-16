#include "GeMSE_ParticleSource.hh"

#include "TH1.h"
#include "TFile.h"
#include "TCanvas.h"
#include <math.h>
#include <Randomize.hh>
#include <cmath>
#include <sstream>
#include <vector>
using std::ifstream;
using std::stringstream;
using std::vector;

#include <G4SystemOfUnits.hh>
#include <G4Event.hh>
#include <G4IonTable.hh>
#include <G4Ions.hh>
#include <G4ParticleDefinition.hh>
#include <G4ParticleTable.hh>
#include <G4PhysicalVolumeStore.hh>
#include <G4PrimaryParticle.hh>
#include <G4Track.hh>
#include <G4TrackingManager.hh>
#include <G4TransportationManager.hh>
#include <G4VPhysicalVolume.hh>

GeMSE_ParticleSource::GeMSE_ParticleSource() {
  m_hGenerator="generic";
  m_iNumberOfParticlesToBeGenerated = 1;
  m_pParticleDefinition = 0;
  G4ThreeVector hZero(0., 0., 0.);

  m_hParticleMomentumDirection = G4ParticleMomentum(1., 0., 0.);
  m_dParticleEnergy = 1.0 * MeV;
  m_hParticlePosition = hZero;
  m_dParticleTime = 0.0;
  m_hParticlePolarization = hZero;
  m_dParticleCharge = 0.0;

  m_hSourcePosType = "Volume";
  m_hShape = "NULL";
  m_dHalfz = 0.;
  m_dRadius = 0.;
  m_hCenterCoords = hZero;
  m_bConfine = false;
  m_hVolumeNames.clear();

  m_hAngDistType = "iso";
  m_dMinTheta = 0.;
  m_dMaxTheta = M_PI;
  
  m_dMinPhi = 0.;
  m_dMaxPhi = 2 * M_PI;

  m_hEnergyDisType = "Mono";
  m_dMonoEnergy = 1 * MeV;
  m_hEnergyFile = "";
  m_hEnergySpectrum = TH1D("EnergySpectrum", "", 1, 0.999, 1.001);
  m_hEnergySpectrum.SetBinContent(1, 1.);

  m_iVerbosityLevel = 0;

  m_pMessenger = new GeMSE_ParticleSourceMessenger(this);
  m_pNavigator = G4TransportationManager::GetTransportationManager()
                     ->GetNavigatorForTracking();
}

GeMSE_ParticleSource::~GeMSE_ParticleSource() { 
  delete m_pMessenger;
}

void GeMSE_ParticleSource::SetParticleDefinition(
    G4ParticleDefinition *aParticleDefinition) {
  m_pParticleDefinition = aParticleDefinition;
  m_dParticleCharge = m_pParticleDefinition->GetPDGCharge();
}

void GeMSE_ParticleSource::SetEnergyFile(G4String hEnergyFile) {
  m_hEnergyFile = hEnergyFile;
  ReadEnergySpectrum();
}

G4bool GeMSE_ParticleSource::ReadEnergySpectrum() {
  // read the energy spectrum from the file
  ifstream hIn(m_hEnergyFile.c_str());

  if (hIn.fail()) {
    G4cout << "Error: cannot open energy spectrum file " << m_hEnergyFile << "!"
           << G4endl;
    return false;
  }

  if (m_iVerbosityLevel >= 1)
    G4cout << "Source energy spectrum from file: " << m_hEnergyFile << G4endl;

  // read the header
  G4String hEnergyUnit;
  while (!hIn.eof()) {
    G4String hHeader;
    hIn >> hHeader;

    if (hHeader == "unit:")
      hIn >> hEnergyUnit;
    else if (hHeader == "spectrum:")
      break;
    else {
      G4cout << "Error: unknown tag in spectrum file!" << G4endl;
      return false;
    }
  }

  G4double dFactor;
  if (hEnergyUnit == "eV")
    dFactor = MeV / eV;
  else if (hEnergyUnit == "keV")
    dFactor = MeV / keV;
  else if (hEnergyUnit == "MeV")
    dFactor = MeV / MeV;
  else if (hEnergyUnit == "GeV")
    dFactor = MeV / GeV;

  vector<G4double> hEnergyBins;
  vector<G4double> hProbabilities;

  while (!hIn.eof()) {
    G4double dBinEnergy = 0., dProbability = 0.;
    hIn >> dBinEnergy >> dProbability;

    if (hIn.good()) {
      if (m_iVerbosityLevel >= 2)
        G4cout << std::setprecision(3) << std::scientific << dBinEnergy << "  "
               << dProbability << G4endl;

      hEnergyBins.push_back(dBinEnergy * dFactor);
      hProbabilities.push_back(dProbability);
    }
  }

  G4int iNbBins = hEnergyBins.size();
  G4double dMin = hEnergyBins.front();
  G4double dMax = hEnergyBins.back();
  G4double dBinWidth = (dMax - dMin) / (iNbBins - 1);

  m_hEnergySpectrum.Reset();
  m_hEnergySpectrum.SetBins(iNbBins, dMin - 0.5 * dBinWidth,
                            dMax + 0.5 * dBinWidth);
  vector<G4double>::iterator pIt;
  for (pIt = hProbabilities.begin(); pIt != hProbabilities.end(); pIt++)
    m_hEnergySpectrum.SetBinContent((pIt - hProbabilities.begin()) + 1, *pIt);

  return true;
}

void GeMSE_ParticleSource::ConfineSourceToVolume(G4String hVolumeList) {
  stringstream hStream;
  hStream.str(hVolumeList);
  G4String hVolumeName;

  // store all the volume names
  while (!hStream.eof()) {
    hStream >> hVolumeName;
    m_hVolumeNames.insert(hVolumeName);
  }

  // checks if the selected volumes exist and store all volumes that match
  G4PhysicalVolumeStore *PVStore = G4PhysicalVolumeStore::GetInstance();
  G4bool bFoundAll = true;

  set<G4String> hActualVolumeNames;
  for (set<G4String>::iterator pIt = m_hVolumeNames.begin();
       pIt != m_hVolumeNames.end(); ++pIt) {
    G4String hRequiredVolumeName = *pIt;
    G4bool bMatch = false;

    // --- old code ---:
    // if (bMatch = (hRequiredVolumeName.last('*') != std::string::npos))
    //   hRequiredVolumeName = hRequiredVolumeName.strip(G4String::trailing, '*');

    // --- new, compatible with Geant4 11.x ---
    // find last '*' character (use rfind() instead of last())
    std::size_t starPos = hRequiredVolumeName.rfind('*');
    if (bMatch = (starPos != G4String::npos)) {
      // remove trailing '*' characters (instead of strip(trailing,'*'))
      std::size_t lastNotStar = hRequiredVolumeName.find_last_not_of('*');
      if (lastNotStar == G4String::npos) {
        // if string is all '*' -> leave empty
        hRequiredVolumeName.clear();
      } else {
        // remove characters after the last non-'*' character
        hRequiredVolumeName.erase(lastNotStar + 1);
      }
    }

    G4bool bFoundOne = false;
    for (G4int iIndex = 0; iIndex < (G4int)PVStore->size(); iIndex++) {
      G4String hName = (*PVStore)[iIndex]->GetName();

      if ((bMatch && (hName.substr(0, hRequiredVolumeName.size())) ==
                         hRequiredVolumeName) ||
          hName == hRequiredVolumeName) {
        hActualVolumeNames.insert(hName);
        bFoundOne = true;
      }
    }

    bFoundAll = bFoundAll && bFoundOne;
  }

  if (bFoundAll) {
    m_hVolumeNames = hActualVolumeNames;
    m_bConfine = true;

    if (m_iVerbosityLevel >= 1)
      G4cout << "Source confined to volumes: " << hVolumeList << G4endl;

    if (m_iVerbosityLevel >= 2) {
      G4cout << "Volume list: " << G4endl;

      for (set<G4String>::iterator pIt = m_hVolumeNames.begin();
           pIt != m_hVolumeNames.end(); pIt++)
        G4cout << *pIt << G4endl;
    }
  } else if (m_hVolumeNames.empty())
    m_bConfine = false;
  else {
    G4cout << " **** /GeMSE/gun/confine: ERROR: One or more volumes do not exist **** " << G4endl;
    exit(1);
  }
}

void GeMSE_ParticleSource::GeneratePointSource() {
  // Generates Points given the point source.
  if (m_hSourcePosType == "Point")
    m_hParticlePosition = m_hCenterCoords;
  else if (m_iVerbosityLevel >= 1)
    G4cout << "Error SourcePosType is not set to Point" << G4endl;
}

void GeMSE_ParticleSource::GeneratePointsInVolume() {
  G4ThreeVector RandPos;
  G4double x = 0., y = 0., z = 0.;

  if (m_hSourcePosType != "Volume" && m_iVerbosityLevel >= 1)
    G4cout << "Error SourcePosType not Volume" << G4endl;

  if (m_hShape == "Sphere") {
    x = m_dRadius * 2.;
    y = m_dRadius * 2.;
    z = m_dRadius * 2.;
    while (((x * x) + (y * y) + (z * z)) > (m_dRadius * m_dRadius)) {
      x = G4UniformRand();
      y = G4UniformRand();
      z = G4UniformRand();

      x = (x * 2. * m_dRadius) - m_dRadius;
      y = (y * 2. * m_dRadius) - m_dRadius;
      z = (z * 2. * m_dRadius) - m_dRadius;
    }
  }

  else if (m_hShape == "Cylinder") {
    x = m_dRadius * 2.;
    y = m_dRadius * 2.;
    while (((x * x) + (y * y)) > (m_dRadius * m_dRadius)) {
      x = G4UniformRand();
      y = G4UniformRand();
      z = G4UniformRand();
      x = (x * 2. * m_dRadius) - m_dRadius;
      y = (y * 2. * m_dRadius) - m_dRadius;
      z = (z * 2. * m_dHalfz) - m_dHalfz;
    }
  }

  else if (m_hShape == "Box")
  {
    x = 2 * (G4UniformRand() - 0.5) * m_dHalfx;
    y = 2 * (G4UniformRand() - 0.5) * m_dHalfy;
    z = 2 * (G4UniformRand() - 0.5) * m_dHalfz;
  }

  else
    G4cout << "Error: Volume Shape Does Not Exist" << G4endl;

  RandPos.setX(x);
  RandPos.setY(y);
  RandPos.setZ(z);
  m_hParticlePosition = m_hCenterCoords + RandPos;
}

void GeMSE_ParticleSource::SetRandomSpherePos() {
  m_hAngDistType = "direction";
  G4double a_rad = 3000;
  int i;

  G4double theta, phi, rnd1, rnd2, pos_x, pos_y, pos_z;
  G4double r[3], dir[3], D[9];  // random  vector pointing "somewhere down",
                                // direction vector, rot. matrix (eulrer angles)
  //G4double pi = 3.1415926;
  //G4double twopi = 2 * pi;

  theta = pi * G4UniformRand();
  phi = twopi * G4UniformRand();

  rnd1 = G4UniformRand();
  rnd2 = G4UniformRand();

  pos_x = sin(theta) * cos(phi) * a_rad;
  pos_y = sin(theta) * sin(phi) * a_rad;
  pos_z = cos(theta) * a_rad;

  r[0] = sin((rnd1 + 1) * pi / 2) * cos(rnd2 * twopi);
  r[1] = sin((rnd1 + 1) * pi / 2) * sin(rnd2 * twopi);
  r[2] = cos((rnd1 + 1) * pi / 2);

  D[0] = cos(phi + pi / 2);
  D[1] = -1 * cos(theta) * sin(phi + pi / 2);
  D[2] = sin(theta) * sin(phi + pi / 2);
  D[3] = sin(phi + pi / 2);
  D[4] = cos(theta) * cos(phi + pi / 2);
  D[5] = sin(theta) * cos(phi + pi / 2);
  D[6] = 0;
  D[7] = sin(theta);
  D[8] = cos(theta);

  for (i = 0; i < 3; i++)
    dir[i] = 0;  // this is a stupid Matrix*vector multiplication: D*dir, there
                 // is a more elegant way for sure ...
  for (i = 0; i < 3; i++) dir[0] += D[i] * r[i];
  for (i = 0; i < 3; i++) dir[1] += D[i + 3] * r[i];
  for (i = 0; i < 3; i++) dir[2] += D[i + 6] * r[i];

  m_hParticlePosition.setX(pos_x);
  m_hParticlePosition.setY(pos_y);
  m_hParticlePosition.setZ(pos_z);  // (if necessary) shift center of the sphere
                                    // to suit shield geometry

  m_hParticleMomentumDirection.setX(dir[0]);
  m_hParticleMomentumDirection.setY(dir[1]);
  m_hParticleMomentumDirection.setZ(dir[2]);
}

G4bool GeMSE_ParticleSource::IsSourceConfined() {
  // Method to check point is within the volume specified
  if (m_bConfine == false) G4cout << "Error: Confine is false" << G4endl;
  G4ThreeVector null(0., 0., 0.);
  G4ThreeVector *ptr;

  ptr = &null;

  // Check m_hParticlePosition is within a volume in our list
  G4VPhysicalVolume *theVolume;

  theVolume =
      m_pNavigator->LocateGlobalPointAndSetup(m_hParticlePosition, ptr, true);
  G4String theVolName = theVolume->GetName();

  set<G4String>::iterator pIt;
  if ((pIt = m_hVolumeNames.find(theVolName)) != m_hVolumeNames.end()) {
    if (m_iVerbosityLevel >= 1)
      G4cout << "Particle is in volume " << *pIt << G4endl;
    return (true);
  } else
    return (false);
}

void GeMSE_ParticleSource::GenerateIsotropicFlux() {
  G4double rndm, rndm2;
  G4double px, py, pz;

  G4double sintheta, sinphi, costheta, cosphi;

  rndm = G4UniformRand();
  costheta = std::cos(m_dMinTheta) +
             rndm * (std::cos(m_dMaxTheta) - std::cos(m_dMinTheta));
  sintheta = std::sqrt(1. - costheta * costheta);

  rndm2 = G4UniformRand();
  m_dPhi = m_dMinPhi + (m_dMaxPhi - m_dMinPhi) * rndm2;
  sinphi = std::sin(m_dPhi);
  cosphi = std::cos(m_dPhi);

  px = sintheta * cosphi;
  py = sintheta * sinphi;
  pz = costheta;

  G4double ResMag = std::sqrt((px * px) + (py * py) + (pz * pz));

  px = px / ResMag;
  py = py / ResMag;
  pz = pz / ResMag;

  m_hParticleMomentumDirection.setX(px);
  m_hParticleMomentumDirection.setY(py);
  m_hParticleMomentumDirection.setZ(pz);

  // m_hParticleMomentumDirection now holds unit momentum vector.
  if (m_iVerbosityLevel >= 2)
    G4cout << "Generating isotropic vector: " << m_hParticleMomentumDirection
           << G4endl;
}

void GeMSE_ParticleSource::GenerateMonoEnergetic() {
  m_dParticleEnergy = m_dMonoEnergy;
}

void GeMSE_ParticleSource::GenerateEnergyFromSpectrum() {
  m_dParticleEnergy = m_hEnergySpectrum.GetRandom() * MeV;
}

void GeMSE_ParticleSource::GeneratePrimaryVertex(G4Event *evt) {
  if(m_hGenerator=="muon") {
    // Hardcoded muon generator for the spectra under macros/spectra/muons

    TFile zenithFile("./macros/spectra/muons/zenith_angle.root", "read");
    TFile energyFile("./macros/spectra/muons/energy_spectrum.root", "read");

    // muon charge ratio and particle generation
    G4String particletype;
    double charge_ratio=1.27;
    double rand = G4UniformRand();
    if (rand<=charge_ratio/(1+charge_ratio))
      particletype="mu+";
    else
      particletype="mu-";
    m_pParticleDefinition
       = G4ParticleTable::GetParticleTable()->FindParticle(particletype);
    // We overwrite the m_* variables, so that the same code can be used
    // for vertex definition for all generators (outside the if/else)
    
    // sample particle position from random point above cavern
    G4double height = 1.6*m;
    G4double size_x = 10.*m;
    G4double size_y = 10.*m;
    //G4double xPosCavern=2.925*m;
    G4double pos_x = size_x*(G4UniformRand()-0.5);
    G4double pos_y = size_y*(G4UniformRand()-0.5);
    G4double pos_z = height;
    m_hParticlePosition.setX(pos_x);
    m_hParticlePosition.setY(pos_y);
    m_hParticlePosition.setZ(pos_z);

    // generate particle direction
    // sample zenith angle from histogram
    TH1D* zenith_angle =
       (TH1D*)((TCanvas*)zenithFile.Get("c1"))->GetPrimitive("zenith angle");
    G4double theta = zenith_angle->GetRandom();
    G4double phi=twopi*G4UniformRand();
    m_hParticleMomentumDirection.setX(sin(theta)*cos(phi));
    m_hParticleMomentumDirection.setY(sin(theta)*sin(phi));
    m_hParticleMomentumDirection.setZ(-cos(theta));

    // sample energy from histogram
    TH1D* energy_spectrum =
       (TH1D*)((TCanvas*)energyFile.Get("c1"))->GetPrimitive("energy spectrum");
    m_dParticleEnergy = energy_spectrum->GetRandom()*GeV;
  }

  else if(m_hGenerator=="generic") {
    // Generate primaries according to the information provided via macro

    if (m_pParticleDefinition == 0) {
      G4cout << "No particle has been defined!" << G4endl;
      return;
    }

    // position
    G4bool srcconf = false;
    G4int LoopCount = 0;

    while (srcconf == false) {
      if (m_hSourcePosType == "Point")
        GeneratePointSource();
      else if (m_hSourcePosType == "Volume")
        GeneratePointsInVolume();
      else if (m_hSourcePosType == "RandomSphere")
        SetRandomSpherePos();
      else {
       G4cout << "Error: SourcePosType undefined" << G4endl;
       G4cout << "Generating point source" << G4endl;
       GeneratePointSource();
      }

      if (m_bConfine == true) {
        srcconf = IsSourceConfined();
        // if source in confined srcconf = true terminating the loop
        // if source isnt confined srcconf = false and loop continues
      } else if (m_bConfine == false)
        srcconf = true;  // terminate loop

      LoopCount++;
      if (LoopCount == 1000000) {
        G4cout << "*************************************" << G4endl;
        G4cout << "LoopCount = 1000000" << G4endl;
        G4cout << "Either the source distribution >> confinement" << G4endl;
        G4cout << "or any confining volume may not overlap with" << G4endl;
        G4cout << "the source distribution or any confining volumes" << G4endl;
        G4cout << "may not exist" << G4endl;
        G4cout << "If you have set confine then this will be ignored" << G4endl;
        G4cout << "for this event." << G4endl;
        G4cout << "*************************************" << G4endl;
        srcconf = true;  // Avoids an infinite loop
      }
    }

    // angular stuff
    if (m_hAngDistType == "iso")
      GenerateIsotropicFlux();
    else if (m_hAngDistType == "direction")
      SetParticleMomentumDirection(m_hParticleMomentumDirection);
    else
      G4cout << "Error: AngDistType has unusual value" << G4endl;

    // energy stuff
    if (m_hEnergyDisType == "Mono")
      GenerateMonoEnergetic();
    else if (m_hEnergyDisType == "Spectrum")
      GenerateEnergyFromSpectrum();
    else
      G4cout << "Error: EnergyDisType has unusual value" << G4endl;
  }
  
  else {
    G4cout << "**** /GeMSE/gun/generator ERROR: invalid option " << m_hGenerator
           << "\n**** Valid options are 'muon' and 'generic' (default)" << G4endl;
    return;
  }

  // create a new vertex
  G4PrimaryVertex *vertex =
     new G4PrimaryVertex(m_hParticlePosition, m_dParticleTime);

  if (m_iVerbosityLevel >= 2)
      G4cout << "Creating primaries and assigning to vertex" << G4endl;
  // create new primaries and set them to the vertex
  G4double mass = m_pParticleDefinition->GetPDGMass();
  G4double energy = m_dParticleEnergy + mass;
  G4double pmom = std::sqrt(energy * energy - mass * mass);
  G4double px = pmom * m_hParticleMomentumDirection.x();
  G4double py = pmom * m_hParticleMomentumDirection.y();
  G4double pz = pmom * m_hParticleMomentumDirection.z();

  if (m_iVerbosityLevel >= 1) {
    G4cout << "Particle name: " << m_pParticleDefinition->GetParticleName()
           << G4endl;
    G4cout << "       Energy: " << m_dParticleEnergy << G4endl;
    G4cout << "     Position: " << m_hParticlePosition << G4endl;
    G4cout << "    Direction: " << m_hParticleMomentumDirection << G4endl;
    G4cout << " NumberOfParticlesToBeGenerated: "
           << m_iNumberOfParticlesToBeGenerated << G4endl;
  }

  for (G4int i = 0; i < m_iNumberOfParticlesToBeGenerated; i++) {
    G4PrimaryParticle *particle =
        new G4PrimaryParticle(m_pParticleDefinition, px, py, pz);
    particle->SetMass(mass);
    particle->SetCharge(m_dParticleCharge);
    particle->SetPolarization(m_hParticlePolarization.x(),
                              m_hParticlePolarization.y(),
                              m_hParticlePolarization.z());
    vertex->SetPrimary(particle);
  }
  evt->AddPrimaryVertex(vertex);
  if (m_iVerbosityLevel > 1) G4cout << " Primary Vetex generated " << G4endl;
}

void GeMSE_ParticleSource::GeneratePrimaryVertexFromTrack(G4Track *pTrack,
                                                          G4Event *pEvent) {
  G4double dPX = pTrack->GetMomentum().x();
  G4double dPY = pTrack->GetMomentum().y();
  G4double dPZ = pTrack->GetMomentum().z();

  G4PrimaryVertex *pVertex =
      new G4PrimaryVertex(pTrack->GetPosition(), m_dParticleTime);

  G4PrimaryParticle *pPrimary =
      new G4PrimaryParticle(pTrack->GetDefinition(), dPX, dPY, dPZ);
  pPrimary->SetMass(pTrack->GetDefinition()->GetPDGMass());
  pPrimary->SetCharge(pTrack->GetDefinition()->GetPDGCharge());

  pVertex->SetPrimary(pPrimary);

  pEvent->AddPrimaryVertex(pVertex);
}
