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
  G4int trackParentId = step->GetTrack()->GetParentID();
  if ( charge == 0. ) return false;
  if ( trackParentId != 0 ) return false;

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

  G4ThreeVector position = preStepPoint->GetPosition();
  newHit->SetPosition(position);
  newHit->SetEnergyDeposit(step->GetTotalEnergyDeposit());

  // Add hit in the collection
  fHitsCollection->insert(newHit);


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

