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
/// \file EventAction.hh
/// \brief Definition of the EventAction class

#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "TFitResult.h"
#include "TFitResultPtr.h"

class G4GenericMessenger;

/// Event action class

namespace ED
{

class EventAction : public G4UserEventAction
{
  public:
    EventAction();
    ~EventAction() override;

    void  BeginOfEventAction(const G4Event* event) override;
    void  EndOfEventAction(const G4Event* event) override;

    void trackingBanco(G4VHitsCollection* hc);

    G4double& getx0(){return fx0;}
    G4double& gety0(){return fy0;}
    G4double& getmx(){return fmx;}
    G4double& getmy(){return fmy;}
    G4double& getchi2x(){return fchi2x;}
    G4double& getchi2y(){return fchi2y;}
    std::vector<G4double>& getMMpos(int layer){return fMMpos[layer];}

  private:
    G4GenericMessenger*  fMessenger = nullptr;
    G4bool fVerbose = false;
    G4int fBancoHCID = -1;
    G4int fMicromegasHCID = -1;

    G4double fx0 = -1.;
    G4double fy0 = -1.;
    G4double fmx = -1.;
    G4double fmy = -1.;
    double fchi2x = -1.;
    double fchi2y = -1.;

    std::vector<std::vector<G4double>> fMMpos;
    std::vector<std::vector<G4double>> fbanco;

};

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


