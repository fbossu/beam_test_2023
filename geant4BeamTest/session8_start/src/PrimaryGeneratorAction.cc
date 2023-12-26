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
/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

#include "Randomize.hh"

namespace ED
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  // G4int nofParticles = 1;
  // fParticleGun  = new G4ParticleGun(nofParticles);

  // // Define particle properties
  // G4String particleName = "e-";
  // //G4String particleName = "geantino";
  // // G4ThreeVector position(0, 0, 152.*mm);
  // G4ThreeVector position(0, 0, 0.*mm);
  // G4ThreeVector momentum(0, 0, 880.*MeV);

  // // Default particle kinematics
  // G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  // G4ParticleDefinition* particle
  //   = particleTable->FindParticle(particleName);
  // fParticleGun->SetParticleDefinition(particle);
  // fParticleGun->SetParticleMomentum(momentum);
  // fParticleGun->SetParticlePosition(position);

  // // Generic messenger
  // // Define /ED/primary commands using generic messenger class
  // fMessenger
  //   = new G4GenericMessenger(this, "/ED/primary/", "Primary generator control");

  // // Define /B4/event/setPrintModulo command
  // fMessenger
  //   ->DeclareProperty("setRandom",
  //                     fRandom,
  //                     "Activate/Inactivate random option");

  fGPS = new G4GeneralParticleSource();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  //this function is called at the begining of ecah event
  //

  // if ( fRandom ) {
  //   // randomized direction
  //   G4double dtheta = 2.*deg;
  //   G4double dphi = 360*deg;
  //   G4double theta = G4UniformRand()*dtheta;
  //   G4double phi = G4UniformRand()*dphi;
  //   fParticleGun->SetParticleMomentumDirection(
  //     G4ThreeVector(sin(theta)*sin(phi), sin(theta)*cos(phi), cos(theta)));
  // }
  // else {
  //   fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0, 0, 1));
  // }

  // fParticleGun->GeneratePrimaryVertex(event);
  fGPS->GeneratePrimaryVertex(event);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}

