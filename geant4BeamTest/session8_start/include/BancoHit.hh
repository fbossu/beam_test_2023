#ifndef BancoHit_h
#define BancoHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

namespace ED
{

class BancoHit : public G4VHit
{
  public:
    BancoHit();
    ~BancoHit() override;
    BancoHit(const BancoHit& right);
    const BancoHit& operator=(const BancoHit& right);
    int operator==(const BancoHit &right) const;

    inline void* operator new(size_t);
    inline void  operator delete(void* hit);

    void Print() override;
    void Draw() override;

    // setter methods
    void SetLadderNumber(G4int number) { fLadderNumber = number; }
    void SetTime(G4double time)       { fTime = time; }
    void SetPosition(G4ThreeVector position) { fPosition = position; }
    void SetEnergyDeposit(G4double energy) { fEnergy = energy; }

    // getter methods
    G4int          GetLadderNumber() const { return fLadderNumber;}
    G4double       GetTime() const        { return fTime; }
    G4ThreeVector  GetPosition() const    { return fPosition; }
    G4double       GetEnergyDeposit() const { return fEnergy; }

  private:
    // data members
    G4int          fLadderNumber = -1;
    G4double       fTime = 0.;
    G4double       fEnergy = 0.;
    G4ThreeVector  fPosition;
};

typedef G4THitsCollection<BancoHit> BancoHitsCollection;

extern G4ThreadLocal G4Allocator<BancoHit>* BancoHitAllocator;

inline void* BancoHit::operator new(size_t)
{
  if (! BancoHitAllocator)
        BancoHitAllocator = new G4Allocator<BancoHit>;
  return (void*)BancoHitAllocator->MallocSingle();
}

inline void BancoHit::operator delete(void* hit)
{
  BancoHitAllocator->FreeSingle((BancoHit*) hit);
}

}

#endif


