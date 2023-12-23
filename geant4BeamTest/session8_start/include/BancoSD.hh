
#ifndef BancoSD_h
#define BancoSD_h 1

#include "G4VSensitiveDetector.hh"
#include "BancoHit.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

namespace ED
{

class BancoSD : public G4VSensitiveDetector
{
  public:
    BancoSD(const G4String& name);
    ~BancoSD() override;

    void   Initialize(G4HCofThisEvent* hce) override;
    G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
    void   EndOfEvent(G4HCofThisEvent* hce) override;

  private:
    BancoHitsCollection* fHitsCollection = nullptr;
};

}

#endif

