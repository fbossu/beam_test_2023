//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id$
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"
#include "MicromegasSD.hh"
#include "BancoSD.hh"
#include "G4VSensitiveDetector.hh"

#include "G4NistManager.hh"
#include "G4SDManager.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4ProductionCuts.hh"
#include "G4SystemOfUnits.hh"
#include "G4RotationMatrix.hh"
#include "G4FieldManager.hh"
#include "G4PVReplica.hh"

namespace ED
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Get nist material manager
  G4NistManager* nistManager = G4NistManager::Instance();
  G4double A, Z;

  // Build materials
  G4Material* air = nistManager->FindOrBuildMaterial("G4_AIR");
  G4Material* argon = nistManager->FindOrBuildMaterial("G4_Ar");
  G4Material* kapton = nistManager->FindOrBuildMaterial("G4_KAPTON");
  G4Material* copper = nistManager->FindOrBuildMaterial("G4_Cu");
  G4Material* mylar = nistManager->FindOrBuildMaterial("G4_MYLAR");
  G4Material* carbon = nistManager->FindOrBuildMaterial("G4_C");
  G4Material* alu = nistManager->FindOrBuildMaterial("G4_Al");
  G4Material* silicon = nistManager->FindOrBuildMaterial("G4_Si");

  G4Element* elH  = new G4Element("Hydrogen","H",  Z=1.,  A=1.00794*g/mole);
  G4Element* elC  = new G4Element("Carbon",  "C",  Z=6.,  A= 12.011 *g/mole);
  G4Element* elN  = new G4Element("Nitrogen","N",  Z=7.,  A= 14.00674*g/mole);
  G4Element* elO  = new G4Element("Oxygen",  "O",  Z=8.,  A= 15.9994*g/mole);
  G4Element* elNa = new G4Element("Sodium",  "Na", Z=11., A= 22.989768*g/mole);
  G4Element* elSi = new G4Element("Silicon", "Si", Z=14., A= 28.0855*g/mole);
  G4Element* elAr = new G4Element("Argon",   "Ar", Z=18., A= 39.948*g/mole);
  G4Element* elI  = new G4Element("Iodine",  "I",  Z=53., A= 126.90447*g/mole);
  G4Element* elCs = new G4Element("Cesium",  "Cs", Z=55., A= 132.90543*g/mole);

  G4double density, massfraction;
  G4int natoms, nel;

  // temperature of experimental hall is controlled at 20 degree.
  const G4double expTemp = CLHEP::STP_Temperature+20.*kelvin;
  
  // vacuum
  density = CLHEP::universe_mean_density;
  G4Material* Vacuum = new G4Material("Vacuum", density, nel=2);
  Vacuum-> AddElement(elN, .7);
  Vacuum-> AddElement(elO, .3);

   // air
  density = 1.2929e-03 *g/cm3;  // at 20 degree
  G4Material* Air = new G4Material("Air", density, nel=3,
                                   kStateGas, expTemp);

  G4double ttt = 75.47+23.20+1.28;
  Air-> AddElement(elN,  massfraction= 75.47/ttt);
  Air-> AddElement(elO,  massfraction= 23.20/ttt);
  Air-> AddElement(elAr, massfraction=  1.28/ttt);

  // Print all materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;

  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;

  //
  // World
  //
  G4double hx = 1.*m;
  G4double hy = 1.*m;
  G4double hz = 3.*m;

  // world volume
  G4Box* worldS = new G4Box("World", hx, hy, hz);

  G4LogicalVolume* worldLV
    = new G4LogicalVolume(worldS, air, "World");

  G4VPhysicalVolume* worldPV
    = new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      worldLV,               //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking

  //
  // BeamPipe
  //
  G4double rmin = 0.;
  G4double rmax = 2.*cm;
  hz = 4.*cm;
  G4double phimin = 0.;
  G4double dphi = 360.*deg;

  // tube volume
  G4VSolid* tubeS = new G4Tubs("beamPipeS", rmin, rmax, hz, phimin, dphi);

  G4LogicalVolume* tubeLV
    = new G4LogicalVolume(tubeS, air, "beamPipeLV");

  G4RotationMatrix* rotTube = new G4RotationMatrix();
  rotTube->rotateX(0.*deg);

  // new G4PVPlacement(rotTube,
  //                   G4ThreeVector(0, 0, -4*cm),       //at (0,0,0)
  //                   tubeLV,                //its logical volume
  //                   "beamPipe",                //its name
  //                   worldLV,               //its mother  volume
  //                   false,                 //no boolean operation
  //                   0,                     //copy number
  //                   checkOverlaps);        //overlaps checking

  // First arm
  hx = 0.2*m;
  hy = 0.2*m;
  G4double arm_hz = 1*m;

  G4VSolid* firstArmS = new G4Box("FirstArmS", hx, hy, arm_hz);
  G4LogicalVolume* firstArmLV
    = new G4LogicalVolume(firstArmS, air, "FirstArmLV");

  new G4PVPlacement(0,
                    G4ThreeVector(0, 0, arm_hz),
                    firstArmLV,            //its logical volume
                    "FirstArm",            //its name
                    worldLV,               //its mother  volume
                    false,                 //no boolean operation
                    0,                     //copy number
                    checkOverlaps);        //overlaps checking


  // ---------------------- banco ---------------------
  // Volume for one arm
  hx = 7.*cm;
  hy = 1.5*cm;
  G4double banco_hz = 0.6*cm;

  G4VSolid* bancoArmS = new G4Box("bancoArmS", hx, hy, banco_hz);
  G4LogicalVolume* bancoArmLV = new G4LogicalVolume(bancoArmS, air, "bancoArmLV");

  new G4PVPlacement(0,
                    G4ThreeVector(0, 0, -arm_hz + banco_hz),
                    bancoArmLV,            //its logical volume
                    "bancoArm1",           //its name
                    firstArmLV,            //its mother  volume
                    false,                 //no boolean operation
                    0,                     //copy number
                    checkOverlaps);        //overlaps checking

  // ladder FCB
  G4double fcb_hz = 75.*um;
  G4VSolid* fcbS = new G4Box("fcbS", hx, hy, fcb_hz/2.);
  G4LogicalVolume* fcbLV = new G4LogicalVolume(fcbS, kapton, "fcbLV");

  // ladder alu on the FCB
  G4double alu_hz = 25.*um;
  G4VSolid* aluS = new G4Box("aluS", hx, hy, alu_hz/2.);
  G4LogicalVolume* aluLV = new G4LogicalVolume(aluS, alu, "aluLV");

  // sensor
  G4double si_hz = 50.*um;
  G4VSolid* siS = new G4Box("siS", hx, hy, si_hz/2.);
  G4LogicalVolume* siLV = new G4LogicalVolume(siS, silicon, "siLV");

  // lader carbon
  G4double carbon_hz = 0.3*mm;
  G4VSolid* carbonS = new G4Box("fcbS", hx, hy, carbon_hz/2.);
  G4LogicalVolume* carbonLV = new G4LogicalVolume(carbonS, carbon, "carbonLV");

  // Window Mylar
  G4double frame_hz = 50*um;
  G4VSolid* frameS = new G4Box("frameS", hx, hy, frame_hz/2.);
  G4LogicalVolume* frameLV = new G4LogicalVolume(frameS, mylar, "frameLV");

  new G4PVPlacement(0, G4ThreeVector(0, 0, -banco_hz + frame_hz/2.), 
                    frameLV, "frame1", bancoArmLV, false, 0, checkOverlaps);
  new G4PVPlacement(0, G4ThreeVector(0, 0, -banco_hz + frame_hz + si_hz/2.), 
                    siLV, "sensor1", bancoArmLV, false, 0, checkOverlaps);
  new G4PVPlacement(0, G4ThreeVector(0, 0, -banco_hz + frame_hz + si_hz + fcb_hz/2.), 
                    fcbLV, "fcb1", bancoArmLV, false, 0, checkOverlaps);
  new G4PVPlacement(0, G4ThreeVector(0, 0, -banco_hz + frame_hz + si_hz + fcb_hz + alu_hz/2.), 
                    aluLV, "alu1", bancoArmLV, false, 0, checkOverlaps);
  // new G4PVPlacement(0, G4ThreeVector(0, 0, -banco_hz + frame_hz + si_hz + fcb_hz + alu_hz + carbon_hz/2.), 
                    // carbonLV, "carbon1", bancoArmLV, false, 0, checkOverlaps);
  // 10 mm of air (Rohacel)
  // new G4PVPlacement(0, G4ThreeVector(0, 0, -banco_hz + frame_hz + si_hz + fcb_hz + alu_hz + carbon_hz + 10*mm + carbon_hz/2.), 
                    // carbonLV, "carbon2", bancoArmLV, false, 1, checkOverlaps);
  new G4PVPlacement(0, G4ThreeVector(0, 0, -banco_hz + frame_hz + si_hz + fcb_hz + alu_hz + carbon_hz + 10*mm + carbon_hz + alu_hz/2.), 
                    aluLV, "alu2", bancoArmLV, false, 1, checkOverlaps);
  new G4PVPlacement(0, G4ThreeVector(0, 0, -banco_hz + frame_hz + si_hz + fcb_hz + alu_hz + carbon_hz + 10*mm + carbon_hz + alu_hz + fcb_hz/2.), 
                    fcbLV, "fcb2", bancoArmLV, false, 1, checkOverlaps);
  new G4PVPlacement(0, G4ThreeVector(0, 0, -banco_hz + frame_hz + si_hz + fcb_hz + alu_hz + carbon_hz + 10*mm + carbon_hz + alu_hz + fcb_hz + si_hz/2.), 
                    siLV, "sensor2", bancoArmLV, false, 1, checkOverlaps);
  new G4PVPlacement(0, G4ThreeVector(0, 0, -banco_hz + frame_hz + si_hz + fcb_hz + alu_hz + carbon_hz + 10*mm + carbon_hz + alu_hz + fcb_hz + si_hz + frame_hz/2.), 
                    frameLV, "frame2", bancoArmLV, false, 1, checkOverlaps);


  // second banco arm
  new G4PVPlacement(0,
                    G4ThreeVector(0, 0, -arm_hz + banco_hz*2 + 134*mm),
                    bancoArmLV,            //its logical volume
                    "bancoArm2",           //its name
                    firstArmLV,            //its mother  volume
                    false,                 //no boolean operation
                    1,                     //copy number
                    checkOverlaps);        //overlaps checking

  // ---------------------- MM --------------------------
  // MM in First arm
  hx = 5.*cm;
  hy = 5.*cm;
  G4double MM_hz = 3.5*mm/2.;

  G4VSolid* eicS = new G4Box("eicS", hx, hy, MM_hz);
  G4LogicalVolume* eicLV
    = new G4LogicalVolume(eicS, argon, "eicLV");

  G4double dz = 120*mm;
  for (G4int i=0; i<5; ++i) {
    G4double zposi = -arm_hz + 305.6*mm + i*dz;
    new G4PVPlacement(0,
                    G4ThreeVector(0, 0, zposi),
                    eicLV,                 //its logical volume
                    "eic",                 //its name
                    firstArmLV,            //its mother  volume
                    false,                 //no boolean operation
                    i,                     //copy number
                    checkOverlaps);        //overlaps checking
  }

  // Mylar in MM on the beampipe facing side of the detector. It will make the sensitive detector
  hx = 5.*cm;
  hy = 5.*cm;
  G4double mylar_hz = 50*um/2.;

  G4VSolid* mylarPlaneS = new G4Box("mylarPlaneS", hx, hy, mylar_hz);
  G4LogicalVolume* mylarPlaneLV
    = new G4LogicalVolume(mylarPlaneS, mylar, "mylarPlaneLV");
  new G4PVPlacement(0,
                    G4ThreeVector(0, 0, -MM_hz + mylar_hz), 
                    mylarPlaneLV,          //its logical volume
                    "mylarPlane",          //its name
                    eicLV,            //its mother  volume
                    false,                 //no boolean operation
                    0,                     //copy number
                    checkOverlaps);        //overlaps checking

  // Kapton plane in MM
  hx = 5.*cm;
  hy = 5.*cm;
  G4double kapton_hz = 150*um/2.;

  G4VSolid* kaptonPlaneS = new G4Box("kaptonPlaneS", hx, hy, kapton_hz);
  G4LogicalVolume* kaptonPlaneLV
    = new G4LogicalVolume(kaptonPlaneS, kapton, "kaptonPlaneLV");
  new G4PVPlacement(0,
                    G4ThreeVector(0, 0, MM_hz - kapton_hz), 
                    kaptonPlaneLV,          //its logical volume
                    "kaptonPlane",          //its name
                    eicLV,            //its mother  volume
                    false,                 //no boolean operation
                    0,                     //copy number
                    checkOverlaps);        //overlaps checking

  // copper in MM
  hx = 5.*cm;
  hy = 5.*cm;
  G4double cu_hz = 50*um/2.;

  G4VSolid* copperPlaneS = new G4Box("copperPlaneS", hx, hy, cu_hz);
  G4LogicalVolume* copperPlaneLV
    = new G4LogicalVolume(copperPlaneS, copper, "copperPlaneLV");
  new G4PVPlacement(0,
                    G4ThreeVector(0, 0, MM_hz - 2*kapton_hz - cu_hz), 
                    copperPlaneLV,          //its logical volume
                    "copperPlane",          //its name
                    eicLV,            //its mother  volume
                    false,                 //no boolean operation
                    0,                     //copy number
                    checkOverlaps);        //overlaps checking

  // Define region
  // G4Region* EICregion = new G4Region("EICregion");
  // eicLV->SetRegion(EICregion);
  // EICregion->AddRootLogicalVolume(eicLV);

  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  // G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  //
  // Sensitive detectors
  //
  MicromegasSD* eicSD = new MicromegasSD("eicSD");
  G4SDManager::GetSDMpointer()->AddNewDetector(eicSD);
  SetSensitiveDetector("mylarPlaneLV", eicSD);

  BancoSD* bancoSD = new BancoSD("bancoSD");
  G4SDManager::GetSDMpointer()->AddNewDetector(bancoSD);
  SetSensitiveDetector("siLV", bancoSD);
}

}
