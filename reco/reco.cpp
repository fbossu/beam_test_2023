#include "TFile.h"
#include "TFile.h"
#include "TTree.h"
#include "TInterpreter.h"
#include "definitions.h"

#include <vector>
#include <string>
#include <map>
#include <iostream>
using namespace std;

void reco( string name) {

  TFile *infile = TFile::Open(name.data());

  TTree *nt = (TTree*)infile->Get("nt");

  uint64_t eventId;
  uint64_t timestamp;
  uint64_t delta_timestamp;
  uint16_t ftst;
  vector<unsigned short> *ampl   = new vector<unsigned short>();
  vector<unsigned short> *sample = new vector<unsigned short>();
  vector<unsigned short> *channel = new vector<unsigned short>();

  nt->SetBranchAddress( "amplitude", &ampl );
  nt->SetBranchAddress( "sample",    &sample );
  nt->SetBranchAddress( "channel",   &channel );
  nt->SetBranchAddress( "eventId", &eventId );
  nt->SetBranchAddress( "timestamp", &timestamp );
  nt->SetBranchAddress( "delta_timestamp", &delta_timestamp);
  nt->SetBranchAddress( "ftst", &ftst );

  //
  name = name.substr( name.rfind('/')+1  ).insert( 0, "rec_");
  TFile *fout  = TFile::Open( name.data(), "recreate");
  TTree outnt( "events","test_beam_2023");
  outnt.SetDirectory( fout );

  hit ahit;
  vector<hit> *hits = new vector<hit>();
  outnt.Branch( "hits", &hits );
  outnt.Branch( "eventId", &eventId );
  outnt.Branch( "timestamp", &timestamp );
  outnt.Branch( "delta_timestamp", &delta_timestamp);
  outnt.Branch( "ftst", &ftst );

  for ( int iev=0; iev<nt->GetEntries() ; iev++){
    nt->GetEntry(iev);

    map<int,int> maxamp;
    map<int,int> sampmax;

    for( uint64_t j=0; j < ampl->size() ; j++ ){
      int jch = channel->at(j);

      if( maxamp.find( jch ) == maxamp.end() ){
        maxamp[jch] = 0;
      }
      if( maxamp[jch]  <  ampl->at(j) ){
         maxamp[jch]   =  ampl->at(j);
         sampmax[jch]  = sample->at(j);
      }
    }

    hits->clear();
    for( auto &m : maxamp ){
      ahit.channel   = m.first;
      ahit.maxamp    = m.second;
      ahit.samplemax = sampmax[m.first];
      hits->push_back(ahit);
    }
    outnt.Fill();


  }
  fout->Write();
  fout->Close();
  infile->Close();

}

int main( int argc, char **argv ){

  if( argc < 2 ) {
    cerr << " no file name specified \n";
    return 1;
  }

  for( int i=1; i< argc; i++){
    string name = argv[i];
    if( name.find( ".root" ) > name.size() ){
      cerr << " warning : " << name << " is not a root file; we'll ignore it" << endl;
      continue;
    }
    reco( argv[i] );
  }
  return 0;
}

