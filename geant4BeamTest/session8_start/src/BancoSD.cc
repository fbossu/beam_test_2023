#include "BancoSD.hh"

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

BancoSD::BancoSD(const G4String& name)
 : G4VSensitiveDetector(name)
{
  G4String hcName = "BancoHitsCollection";
  collectionName.insert(hcName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BancoSD::~BancoSD()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BancoSD::Initialize(G4HCofThisEvent* hce)
{
  G4String hcName = "BancoHitsCollection";

  fHitsCollection
    = new BancoHitsCollection(SensitiveDetectorName, hcName);

  G4int hcID
    = G4SDManager::GetSDMpointer()->GetCollectionID(hcName);
  hce->AddHitsCollection( hcID, fHitsCollection );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool BancoSD::ProcessHits(G4Step* step,
                              G4TouchableHistory* /*history*/)
{
  // Change the following lines to get the charge of the tracked particle
  G4double charge = step->GetTrack()->GetDefinition()->GetPDGCharge();
  G4int trackParentId = step->GetTrack()->GetParentID();
  if ( charge == 0. ) return false;
  if ( trackParentId != 0 ) return false;

  // Create new hit
  BancoHit* newHit = new BancoHit();

  // Time
  G4StepPoint* preStepPoint = step->GetPreStepPoint();
  G4double time = preStepPoint->GetGlobalTime();
  newHit->SetTime(time);
  newHit->SetEnergyDeposit(step->GetTotalEnergyDeposit());

  // Position

  G4double pixW = 0.018*mm;

  G4double xsmear = -pixW/2. + pixW * G4UniformRand();
  G4double ysmear = -pixW/2. + pixW * G4UniformRand();
  // G4cout<<xsmear<<" "<<ysmear<<G4endl;
  
  G4ThreeVector position = preStepPoint->GetPosition();
  position.setX(position.x() + xsmear);
  position.setY(position.y() + ysmear);
  newHit->SetPosition(position);

  // Layer number
  // = copy number of mother volume
  const G4VTouchable* touchable = preStepPoint->GetTouchable();
  G4int armNo = (touchable->GetCopyNumber(1)==1) ? 2 : 0;
  G4int copyNo = touchable->GetCopyNumber(0);
  newHit->SetLadderNumber(copyNo+armNo);

  // Add hit in the collection
  fHitsCollection->insert(newHit);

  // Add hits properties in the ntuple
//   auto analysisManager = G4AnalysisManager::Instance();
//   analysisManager->FillNtupleIColumn(0, 0, copyNo);
//   analysisManager->FillNtupleDColumn(0, 1, position.x()/mm);
//   analysisManager->FillNtupleDColumn(0, 2, position.y()/mm);
//   analysisManager->FillNtupleDColumn(0, 3, position.z()/mm);
//   analysisManager->AddNtupleRow(0);

  return false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void BancoSD::EndOfEvent(G4HCofThisEvent* /*hce*/)
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

