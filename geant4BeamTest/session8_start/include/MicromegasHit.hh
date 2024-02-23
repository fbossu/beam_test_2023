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
/// \file MicromegasHit.hh
/// \brief Definition of the MicromegasHit class
//

#ifndef MicromegasHit_h
#define MicromegasHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

namespace ED
{

class MicromegasHit : public G4VHit
{
  public:
    MicromegasHit();
    ~MicromegasHit() override;
    MicromegasHit(const MicromegasHit& right);
    const MicromegasHit& operator=(const MicromegasHit& right);
    int operator==(const MicromegasHit &right) const;

    inline void* operator new(size_t);
    inline void  operator delete(void* hit);

    void Print() override;
    void Draw() override;

    // setter methods
    void SetLayerNumber(G4int number) { fLayerNumber = number; }
    void SetTime(G4double time)       { fTime = time; }
    void SetPosition(G4ThreeVector position) { fPosition = position; }
    void SetEnergyDeposit(G4double energy) { fEnergy = energy; }

    // getter methods
    G4int          GetLayerNumber() const   { return fLayerNumber;}
    G4double       GetTime() const          { return fTime; }
    G4ThreeVector  GetPosition() const      { return fPosition; }
    G4double       GetEnergyDeposit() const { return fEnergy; }

  private:
    // data members
    G4int          fLayerNumber = -1;
    G4double       fTime = 0.;
    G4double       fEnergy = 0.;
    G4ThreeVector  fPosition;
};

typedef G4THitsCollection<MicromegasHit> MicromegasHitsCollection;

extern G4ThreadLocal G4Allocator<MicromegasHit>* MicromegasHitAllocator;

inline void* MicromegasHit::operator new(size_t)
{
  if (! MicromegasHitAllocator)
        MicromegasHitAllocator = new G4Allocator<MicromegasHit>;
  return (void*)MicromegasHitAllocator->MallocSingle();
}

inline void MicromegasHit::operator delete(void* hit)
{
  MicromegasHitAllocator->FreeSingle((MicromegasHit*) hit);
}

}

#endif


