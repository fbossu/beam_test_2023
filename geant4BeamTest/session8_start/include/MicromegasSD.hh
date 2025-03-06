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
/// \file MicromegasSD.hh
/// \brief Definition of the MicromegasSD class
//

#ifndef MicromegasSD_h
#define MicromegasSD_h 1

#include "G4VSensitiveDetector.hh"
#include "MicromegasHit.hh"
#include "TF1.h"
#include "TRandom3.h"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

namespace ED
{

class MicromegasSD : public G4VSensitiveDetector
{
  public:
    MicromegasSD(const G4String& name);
    ~MicromegasSD() override;

    void   Initialize(G4HCofThisEvent* hce) override;
    G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
    void   EndOfEvent(G4HCofThisEvent* hce) override;

  private:
    MicromegasHitsCollection* fHitsCollection = nullptr;
    TF1 *landauFunc = nullptr;
};

}

#endif

