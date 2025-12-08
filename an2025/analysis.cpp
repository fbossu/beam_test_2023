#include "reco/definitions.h"
#include "banco/definition_banco.h"
#include "map/StripTable.h"
#include <string>

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TTreeReader.h"
#include "TStyle.h"
#include "TLegend.h"
#include <map>
#include <iostream>

#include <getopt.h>
#include <libgen.h>

// this is the main analysis program
// analysis plugins must have this structure
// init( TTreeReader MM, TTreeReader *banco);
// loop();
// end();

#include "eff.h"
#include "res.h"
#include "hits.h"

using namespace std;

// ------------------------------------------
void print_help(){
  std::cout << " analysis manager for micromegas prototypes \n"
            << "Usage: an -d DETNAME -m MMfile.root -b Bancofile.root\n"
            << "Options: \n"
            << " -d [string] detector name\n"
            << " -m [string] MM root file\n"
            << " -b [string] banco root file\n"
            << " -B [float] banco y position\n"
            << " -M [string] mapping file\n"
            << " -A [string] alignment direcotry\n"
            << " -a [string] choose analyses [ehrt]\n"
            << " -t [float] tolerance for match\n"
            << " -n [int] max numer of events\n"
            << " -s [int] skip events\n";

}
// ------------------------------------------
int main( int argc, char* argv[]) {

  string basedir = argv[0];
  basedir = basedir.substr(0, basedir.find_last_of("\\/") + 1);
  //cout << basedir << endl;
  string detName; 
  string fnameBanco;
  string fnameMM;
  string mapName;
  string alignDirName;
  string anChoice = "ehrt";
  int NEV   = -1;
  int NSKIP = 0;
  float bancoY = 0.;
  float tolerance = 0.;
  int opt;
  while( (opt = getopt( argc, argv, "d:m:b:B:M:A:n:s:a:t:" )) != -1 ){
    switch(opt) { 
      case 'm':
        fnameMM = optarg;
        cout << " MM file name " <<  fnameMM << endl;
        break;
      case 'b':
        fnameBanco = optarg;
        cout << " Banco file name " <<  fnameBanco << endl;
        break;
      case 'a':
        anChoice = optarg;
        cout << " chosen analyses " <<  anChoice << endl;
        break;
      case 'M':
        mapName = optarg;
        cout << " map file name " <<  mapName << endl;
        break;
      case 'A':
        alignDirName = optarg;
        cout << " alignDir file name " <<  alignDirName << endl;
        break;
      case 'd':
        detName = optarg;
        cout << " detector name " <<  detName << endl;
        break;
      case 'B':
        bancoY = atof(optarg);
        cout << " banco position " <<  bancoY << endl;
        break;
      case 't':
        tolerance = atof(optarg);
        cout << " tolerance " <<  tolerance << endl;
        break;
      case 'n':
        NEV = atoi(optarg);
        cout << " max number of events " <<  NEV << endl;
        break;
      case 's':
        NSKIP = atoi(optarg);
        cout << " skip events " <<  NSKIP << endl;
        break;
      default:
        print_help();
        return 1;
    }
  }
  if ( fnameMM == "" or fnameBanco == "" ){
    cout << "### missing input files\n";
    print_help();
    return 1;
  }

  if( mapName == "" )
    mapName = basedir + "/../map/rd542_map.txt";
  
  string alignName = "alignFiles/" + detName + ".txt";
  if( alignDirName != "" ) alignName = alignDirName + "/" + detName + ".txt";

  StripTable det(mapName, alignName);

  TFile* fMM = TFile::Open(fnameMM.c_str(), "read");
  TFile* fbanco = TFile::Open(fnameBanco.c_str(), "read");

  TTreeReader MM("events", fMM);
  TTreeReader banco("events", fbanco);
  TTreeReaderValue< unsigned long > banco_evId( banco, "eventId");
  TTreeReaderValue< unsigned long > MM_evId( MM, "eventId");

  // ========== create the plugins =====
  anhits H(&det, detName, bancoY );
  aneff E(&det, detName, bancoY );
  anres R(&det, detName, bancoY );
  R.setTolerance(tolerance);
  std::map<char,anplug*> plugs;
  for( auto a : anChoice ){
    switch( a ){
      case 'h':
      case 'H':
        plugs['h'] = &H;
        break;
      case 'e':
      case 'E':
        plugs['e'] = &E;
        break;
      case 'r':
      case 'R':
        plugs['r'] = &R;
        break;
      default:
        break;
    }
  }
  // ========== call the inits of the plugins =====
  for( auto p : plugs ) p.second->init( &MM, &banco);

  // ========== LOOP over events ==================
  for( int iev = NSKIP; iev < MM.GetEntries(); iev++ ){
    if( NEV > -1 && iev >= NEV ) break;
    // load entry and checks if entry exists in the two files
    if( MM.SetEntry( iev ) != 0) { std::cout <<iev <<  " MM event not loaded\n"; break; }
    if( banco.SetEntry( iev ) != 0) { std::cout <<iev <<  " banco event not loaded\n"; break; }
      if( *banco_evId != *MM_evId ) { std::cout << iev << " *** different ev id\n"; break; }


      if( iev%1000 == 0 ) std::cout << std::setw(12) << iev << "\r" << std::flush;
    for( auto p : plugs ) p.second->run();
  }

  // ========== finalize ==========================
  for( auto p : plugs ) p.second->end();
  return 0;
}



