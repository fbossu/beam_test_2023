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
/// \file MicromegasSD.cc
/// \brief Implementation of the MicromegasSD class
//

#include "MicromegasSD.hh"

#include <cmath>
#include "G4AnalysisManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4VTouchable.hh"
#include "G4Step.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

namespace ED
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MicromegasSD::MicromegasSD(const G4String& name)
 : G4VSensitiveDetector(name)
{
  G4String hcName = "MicromegasHitsCollection";
  collectionName.insert(hcName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

MicromegasSD::~MicromegasSD()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MicromegasSD::Initialize(G4HCofThisEvent* hce)
{
  G4String hcName = "MicromegasHitsCollection";

  fHitsCollection
    = new MicromegasHitsCollection(SensitiveDetectorName, hcName);

  G4int hcID
    = G4SDManager::GetSDMpointer()->GetCollectionID(hcName);
  hce->AddHitsCollection( hcID, fHitsCollection );

  landauFunc = new TF1("landauFunc", "TMath::Landau(x, [0], [1])", 0., 1.);
  landauFunc->SetParameters(0.45, 0.12);
  gRandom->SetSeed(0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool MicromegasSD::ProcessHits(G4Step* step,
                              G4TouchableHistory* /*history*/)
{
  // Change the following lines to get the charge of the tracked particle
  G4double charge = step->GetTrack()->GetDefinition()->GetPDGCharge();
  if ( charge == 0. ) return false;

  // Create new hit
  MicromegasHit* newHit = new MicromegasHit();

  // Time
  G4StepPoint* preStepPoint = step->GetPreStepPoint();
  G4double time = preStepPoint->GetGlobalTime();
  newHit->SetTime(time);

  // Layer number
  // = copy number of mother volume
  const G4VTouchable* touchable = preStepPoint->GetTouchable();
  G4int copyNo = touchable->GetCopyNumber(1);
  newHit->SetLayerNumber(copyNo);

  // Position
  // strips of pitch 1mm, center of strip on integers
  // double xtol = 0.4;
  // double ytol = 0.4;
  // double xtol = G4RandGauss::shoot(0.5, 0.3);
  double xStW = 0., yStW = 0.;
  double xtol = 0., ytol = 0;

  if(copyNo == 4){
    xStW = 0.8;
    yStW = 0.8;
    ytol = landauFunc->GetRandom();
    xtol = 0.4*ytol;
  }else{
    xStW = 0.25;
    yStW = 1.;
    ytol = landauFunc->GetRandom();
    xtol = ytol;
  }
  
  G4double smearSigma = 0.15*mm;
  bool xminOn = false, yminOn = false, xmaxOn = false, ymaxOn = false;

  G4ThreeVector position = preStepPoint->GetPosition();
  // position.setY(position.y()/mm+1*mm);
  double xStmin = std::floor(position.x()/mm);
  double yStmin = std::floor(position.y()/mm);

  // std::cout<<abs(position.x()/mm - (xStmin+xStW/2.))<< " " << abs(position.x()/mm - (xStmin+1 - xStW/2.)) << std::endl;
  if( position.x()/mm - (xStmin+xStW/2.) < xtol ) xminOn = true;
  if( position.y()/mm - (yStmin+yStW/2.) < ytol ) yminOn = true;
  if( position.x()/mm - (xStmin+1 - xStW/2.) > -xtol ) xmaxOn = true;
  if( position.y()/mm - (yStmin+1 - yStW/2.) > -ytol ) ymaxOn = true;

  // if(xminOn && xmaxOn) position.setX(position.x()/mm);
  if(xminOn && xmaxOn) position.setX(G4RandGauss::shoot(position.x()/mm, smearSigma));
  else if(xminOn) position.setX(xStmin*mm);
  else if(xmaxOn) position.setX((xStmin+1)*mm);
  else position.setX(-999);

  // if(yminOn && ymaxOn) position.setY(position.y()/mm);
  if(yminOn && ymaxOn) position.setY(G4RandGauss::shoot(position.y()/mm, smearSigma));
  else if(yminOn) position.setY(yStmin*mm);
  else if(ymaxOn) position.setY((yStmin+1)*mm);
  else position.setY(-999);

  // if(xminOn && xmaxOn) position.setX(-999);
  // else position.setX(std::round(position.x()/mm)*mm);
  
  // if(yminOn && ymaxOn) position.setY(-999);
  // else position.setY(std::round(position.y()/mm)*mm);

  newHit->SetPosition(position);

  // G4double pitch = 0.5*mm;
  // G4double xsmear = -pitch/2. + pitch * G4UniformRand();
  // G4double ysmear = -pitch/2. + pitch * G4UniformRand();
  // G4double xsmear = G4RandGauss::shoot(0., pitch/sqrt(12));
  // G4double ysmear = G4RandGauss::shoot(0., pitch/sqrt(12));

  // G4ThreeVector position = preStepPoint->GetPosition();
  // position.setX(position.x()/mm + xsmear);
  // position.setY(position.y()/mm + ysmear);
  // newHit->SetPosition(position);

  // G4ThreeVector position = preStepPoint->GetPosition();
  // newHit->SetPosition(position);


  // Add hit in the collection
  fHitsCollection->insert(newHit);
  if(copyNo == 4){
      auto analysisManager = G4AnalysisManager::Instance();
      analysisManager->FillH1(0, xtol);
  }

  // Add hits properties in the ntuple
  // auto analysisManager = G4AnalysisManager::Instance();
  // analysisManager->FillNtupleIColumn(0, 0, copyNo);
  // analysisManager->FillNtupleDColumn(0, 1, position.x()/mm);
  // analysisManager->FillNtupleDColumn(0, 2, position.y()/mm);
  // analysisManager->FillNtupleDColumn(0, 3, position.z()/mm);
  // analysisManager->AddNtupleRow(0);

  return false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MicromegasSD::EndOfEvent(G4HCofThisEvent* /*hce*/)
{
  // G4cout << "\n-------->" <<  fHitsCollection->GetName()
  //        << ": in this event: " << G4endl;

  // G4int nofHits = fHitsCollection->entries();
  // for ( G4int i=0; i<nofHits; i++ ) {
  //   (*fHitsCollection)[i]->Print();
  // }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}

