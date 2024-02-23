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
/// \file EventAction.cc
/// \brief Implementation of the EventAction class

#include "EventAction.hh"
#include "MicromegasHit.hh"
#include "BancoHit.hh"
// #include "Constants.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4AnalysisManager.hh"
#include "G4GenericMessenger.hh"

#include "TGraphErrors.h"
#include "TF1.h"

namespace ED
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction() : fMMpos(5, std::vector<G4double>(3,0.)), fMMedep(5,0.), fLpos(4, std::vector<G4double>(3,0.)), fLedep(4,0.)
{
  // Generic messenger
  // Define /ED/event commands using generic messenger class
  fMessenger
    = new G4GenericMessenger(this, "/ED/event/", "Primary generator control");

  // Define /ED/event/setVerbose command
  fMessenger
    ->DeclareProperty("setVerbose",
                      fVerbose,
                      "Activate/Inactivate verbose option");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* event)
{
  if ( fVerbose ) {
    G4cout << ">>> Start event: " << event->GetEventID() << G4endl;
  }
  if (fBancoHCID == -1) {
    auto sdManager = G4SDManager::GetSDMpointer();
    auto analysisManager = G4AnalysisManager::Instance();
    fBancoHCID = sdManager->GetCollectionID("BancoHitsCollection");
    fMicromegasHCID = sdManager->GetCollectionID("MicromegasHitsCollection");
  }
  for( int i=0; i<5; i++ ){
    fMMpos[i] = {0.,0.,0.};
    fMMedep[i] = 0.;
  }
  for( int i=0; i<4; i++ ){
    fLpos[i] = {0.,0.,0.};
    fLedep[i] = 0.;
  }
  if ( fVerbose ) {
    G4cout << ">>> Event init successfull: " << event->GetEventID() << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{
  auto analysisManager = G4AnalysisManager::Instance();
  auto hce = event->GetHCofThisEvent();
  auto hcBanco = hce->GetHC(fBancoHCID);
  auto hcMM = hce->GetHC(fMicromegasHCID);
  if ( hcBanco ){
    // trackingBanco(hcBanco);
    std::vector<int> nHit = {0,0,0,0};
    for( int i=0; i<hcBanco->GetSize(); i++ ){
      auto hit = static_cast<BancoHit*>(hcBanco->GetHit(i));
      nHit[hit->GetLadderNumber()]++;
      fLpos[hit->GetLadderNumber()][0] += hit->GetPosition().x()/mm;
      fLpos[hit->GetLadderNumber()][1] += hit->GetPosition().y()/mm;
      fLpos[hit->GetLadderNumber()][2] += hit->GetPosition().z()/mm;
      fLedep[hit->GetLadderNumber()]   += hit->GetEnergyDeposit()/keV;
    }
    for( int i=0; i<4; i++ ){
      if( nHit[i]==0 ){
        fLpos[i] = {-999.*mm,-999.*mm,-999.*mm};
        fLedep[i] = -1.;
        continue;
      }
      fLpos[i][0] /= nHit[i];
      fLpos[i][1] /= nHit[i];
      fLpos[i][2] /= nHit[i];
    }
  }
  if ( hcMM ){
    std::vector<int> nHit = {0,0,0,0,0};
    for( int i=0; i<hcMM->GetSize(); i++ ){
      auto hit = static_cast<MicromegasHit*>(hcMM->GetHit(i));
      nHit[hit->GetLayerNumber()]++;
      fMMpos[hit->GetLayerNumber()][0] += hit->GetPosition().x()/mm;
      fMMpos[hit->GetLayerNumber()][1] += hit->GetPosition().y()/mm;
      fMMpos[hit->GetLayerNumber()][2] += hit->GetPosition().z()/mm;
      fMMedep[hit->GetLayerNumber()]   += hit->GetEnergyDeposit()/keV;
    }
    for( int i=0; i<5; i++ ){
      if( nHit[i]==0 ){
        fMMpos[i] = {-999.*mm,-999.*mm,-999.*mm};
        fMMedep[i] = -1.;
        continue;
      }
      fMMpos[i][0] /= nHit[i];
      fMMpos[i][1] /= nHit[i];
      fMMpos[i][2] /= nHit[i];
    }
  }
  for(int i=0; i<5; i++){
    analysisManager->FillNtupleDColumn(0, i, fMMedep[i]);
  }
  for(int i=0; i<4; i++){
    analysisManager->FillNtupleDColumn(0, i+5, fLedep[i]);
  }
  analysisManager->AddNtupleRow();
  
  if ( fVerbose ) {
    G4cout << ">>> End event: " << event->GetEventID() << G4endl;
  }
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::trackingBanco(G4VHitsCollection* hc)
{
  TGraphErrors gry;
  TGraphErrors grx;
  bool l0 = false, l1 = false, l2 = false, l3 = false;

  for( int i=0; i<hc->GetSize(); i++ ){
    auto hit = static_cast<BancoHit*>(hc->GetHit(i));
    auto pos = hit->GetPosition();
    l0 = (!l0 && (hit->GetLadderNumber()==0)) || (hit->GetLadderNumber()!=0 && l0);
    l1 = (!l1 && (hit->GetLadderNumber()==1)) || (hit->GetLadderNumber()!=1 && l1);
    l2 = (!l2 && (hit->GetLadderNumber()==2)) || (hit->GetLadderNumber()!=2 && l2);
    l3 = (!l3 && (hit->GetLadderNumber()==3)) || (hit->GetLadderNumber()!=3 && l3);

    grx.SetPoint(i,pos.z()/mm,pos.x()/mm);
    gry.SetPoint(i,pos.z()/mm,pos.y()/mm);
    grx.SetPointError(i,0,0.028/sqrt(12));
    gry.SetPointError(i,0,0.028/sqrt(12));
  }

  // auto ptrx = grx.Fit("pol1","Q0S");
  // auto ptry = gry.Fit("pol1","Q0S");

  // if( l0 && l1 && l2 && l3 && (int)ptrx==0 && (int)ptry==0){
  //   fx0 = ptrx->Parameter(0);
  //   fmx = ptrx->Parameter(1);
  //   fchi2x = ptrx->Chi2()/ptrx->Ndf();

  //   fy0 = ptry->Parameter(0);
  //   fmy = ptry->Parameter(1);
  //   fchi2y = ptry->Chi2()/ptry->Ndf();
  // }
  // else{
  //   fx0 = -1.;
  //   fmx = -1.;
  //   fchi2x = -1.;

  //   fy0 = -1.;
  //   fmy = -1.;
  //   fchi2y = -1.;
  // }
}
}
