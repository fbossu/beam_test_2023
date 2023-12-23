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
#include "TFitResult.h"

namespace ED
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction() : fMMpos(5)
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
  // if ( fVerbose ) {
    G4cout << ">>> Start event: " << event->GetEventID() << G4endl;
  // }
  if (fBancoHCID == -1) {
    auto sdManager = G4SDManager::GetSDMpointer();
    auto analysisManager = G4AnalysisManager::Instance();
    fBancoHCID = sdManager->GetCollectionID("BancoHitsCollection");
    fMicromegasHCID = sdManager->GetCollectionID("MicromegasHitsCollection");
  }
  for( int i=0; i<5; i++ ){
    fMMpos[i].clear();
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
    trackingBanco(hcBanco);
  }
  if ( hcMM ){
    for( int i=0; i<hcMM->GetSize(); i++ ){
      auto hit = static_cast<MicromegasHit*>(hcMM->GetHit(i));
      std::vector<double> hitPos= {hit->GetPosition().x(), hit->GetPosition().y(), hit->GetPosition().z()};
      G4cout<<hitPos[0]<<" "<<hitPos[1]<<" "<<hitPos[2]<<G4endl;
      fMMpos[hit->GetLayerNumber()].insert(fMMpos[hit->GetLayerNumber()].end(), hitPos.begin(), hitPos.end());
    }
  }
  G4cout<<fx0/mm<<" "<<fy0/mm<<" "<<fmx/mm<<" "<<fmy/mm<<" "<<fchi2x<<" "<<fchi2y<<G4endl;
  analysisManager->FillNtupleDColumn(0, 0, fx0/mm);
  G4cout<<fx0/mm<<G4endl;
  analysisManager->FillNtupleDColumn(0, 1, fy0/mm);
  analysisManager->FillNtupleDColumn(0, 2, fmx/mm);
  analysisManager->FillNtupleDColumn(0, 3, fmy/mm);
  analysisManager->FillNtupleDColumn(0, 4, fchi2x);
  analysisManager->FillNtupleDColumn(0, 5, fchi2y);
  analysisManager->AddNtupleRow();
  
  if ( fVerbose ) {
    G4cout << ">>> End event: " << event->GetEventID() << G4endl;
  }
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::trackingBanco(G4VHitsCollection* hc)
{
  TGraphErrors grx;
  TGraphErrors gry;
  bool l0 = false, l1 = false, l2 = false, l3 = false;
  for( int i=0; i<hc->GetSize(); i++ ){
    auto hit = static_cast<BancoHit*>(hc->GetHit(i));
    auto pos = hit->GetPosition();
    l0 = (!l0 && (hit->GetLadderNumber()==0)) || (hit->GetLadderNumber()!=0 && l0);
    l1 = (!l1 && (hit->GetLadderNumber()==1)) || (hit->GetLadderNumber()!=1 && l1);
    l2 = (!l2 && (hit->GetLadderNumber()==2)) || (hit->GetLadderNumber()!=2 && l2);
    l3 = (!l3 && (hit->GetLadderNumber()==3)) || (hit->GetLadderNumber()!=3 && l3);

    grx.SetPoint(i,pos.z(),pos.x());
    gry.SetPoint(i,pos.z(),pos.y());
    grx.SetPointError(i,0,0.028/sqrt(12));
    gry.SetPointError(i,0,0.028/sqrt(12));
  }

  auto ptrx = grx.Fit("pol1","Q0S");
  auto ptry = gry.Fit("pol1","Q0S");

  if( l0 && l1 && l2 && l3 ){
    fx0 = ptrx->Parameter(0);
    fmx = ptrx->Parameter(1);
    fchi2x = ptrx->Chi2()/ptrx->Ndf();

    fy0 = ptry->Parameter(0);
    fmy = ptry->Parameter(1);
    fchi2y = ptry->Chi2()/ptry->Ndf();
  }
  else{
    fx0 = -1.;
    fmx = -1.;
    fchi2x = -1.;

    fy0 = -1.;
    fmy = -1.;
    fchi2y = -1.;
  }
}
}
