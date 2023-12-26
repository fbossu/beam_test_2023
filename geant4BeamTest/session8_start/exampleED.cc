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
/// \file exampleED.cc
/// \brief Main program of the ED example

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "G4RunManagerFactory.hh"
#include "G4GDMLParser.hh"
#include "G4UImanager.hh"
#include "G4PhysListFactory.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "TROOT.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

namespace {
  void PrintUsage() {
    G4cerr << " Usage: " << G4endl;
    G4cerr << " exampleB4a [-m macro ] [-p physList ] [-u UIsession] [-g GDMLfile] [-t nThreads]" << G4endl;
    G4cerr << "   note: -t option is used only in multi-threaded mode." << G4endl;
    G4cerr << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  // Evaluate arguments
  //
  if ( argc > 9 ) {
    PrintUsage();
    return 1;
  }

  G4String macro;
  G4String session;
  G4String physicsListName;
  G4String gdmlFileName;
  G4int nofThreads = 0;
  for ( G4int i=1; i<argc; i=i+2 ) {
    if      ( G4String(argv[i]) == "-m" ) macro = argv[i+1];
    else if ( G4String(argv[i]) == "-u" ) session = argv[i+1];
    else if ( G4String(argv[i]) == "-p" ) physicsListName = argv[i+1];
    else if ( G4String(argv[i]) == "-g" ) gdmlFileName = argv[i+1];
    else if ( G4String(argv[i]) == "-t" ) {
      nofThreads = G4UIcommand::ConvertToInt(argv[i+1]);
    }
    else {
      PrintUsage();
      return 1;
    }
  }

  // Detect interactive mode (if no arguments) and define UI session
  //
  G4UIExecutive* ui = nullptr;
  if ( ! macro.size() ) {
    ui = new G4UIExecutive(argc, argv, session);
  }

  // Construct the default run manager
  //
  auto* runManager =
    G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
  if ( nofThreads > 0 ) {
    runManager->SetNumberOfThreads(nofThreads);
  }
  ROOT::EnableThreadSafety();

  // Set mandatory initialization classes
  //
  // Detector construction
  runManager->SetUserInitialization(new ED::DetectorConstruction());

  // Physics list
  if ( physicsListName.size() == 0 ) physicsListName = "FTFP_BERT_EMZ";
  // if ( physicsListName.size() == 0 ) physicsListName = "G4EmStandardPhysics_option4";
  G4PhysListFactory physListFactory;
  if ( ! physListFactory.IsReferencePhysList(physicsListName)) {
    G4cerr << " Physics list " << physicsListName
           << " is not defined." << G4endl;
    return 1;
  }
  G4VModularPhysicsList* physicsList
    = physListFactory.GetReferencePhysList(physicsListName);
  physicsList->SetVerboseLevel(1);
  runManager->SetUserInitialization(physicsList);

  // User action initialization
  runManager->SetUserInitialization(new ED::ActionInitialization());

  // Export in GDML
  if ( gdmlFileName.size() ) {
    // Implement code to export geometry in GDML
    G4GDMLParser parser;
    G4LogicalVolume* worldLV
      = G4TransportationManager::GetTransportationManager()
        ->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume();
    parser.Write(gdmlFileName, worldLV);
  }

  // Initialize visualization
  //
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if ( macro.size() ) {
    // batch mode
    // batch mode
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command+macro);
  }
  else {
    // interactive mode : define UI session
    UImanager->ApplyCommand("/control/execute init_vis.mac");
    ui->SessionStart();
    delete ui;
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program !

  delete visManager;
  delete runManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
