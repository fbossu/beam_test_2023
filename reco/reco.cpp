#include "TFile.h"
#include "TFile.h"
#include "TTree.h"
#include "TInterpreter.h"
#include "definitions.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <iomanip>

#include "../map/DreamTable.h"

using namespace std;

// =====================================================================
bool compareHits( hit &a, hit &b ) { return a.channel < b.channel; }

bool checkHit( hit &a ) { return a.samplemax > 0 and a.samplemax < 13 and a.maxamp>300; }

void niceBar( int tot, int i, int N=50 ){
  cout << "[ ";
  for( int j=0; j < (float)i/tot*N; j++)
    cout << "-";
  for( int j=(float)i/tot*N; j < N; j++)
    cout << " ";
  cout << " ]\r";
  cout << flush;
}

// =====================================================================
bool JustHits = false;
// =====================================================================
void reco( string name, DreamTable det) {

  TFile *infile = TFile::Open(name.data());
  if( !infile ){
    return;
  }
  cout << "reading " << name << endl;

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
  // name = name.substr( name.rfind('/')+1  ).insert( 0, "rec_");
  TFile *fout  = TFile::Open( "frec.root", "recreate");
  TTree outnt( "events","test_beam_2023");
  outnt.SetDirectory( fout );

  uint64_t out_eventId   = 1;
  uint64_t out_timestamp = 0;
  uint64_t out_delta_timestamp = 0;
  uint16_t out_ftst = 0;
  hit ahit;
  vector<hit> *hits = new vector<hit>();
  vector<cluster> *cls = new vector<cluster>();
  outnt.Branch( "hits", &hits );
  outnt.Branch( "clusters", &cls );
  outnt.Branch( "eventId", &out_eventId );
  outnt.Branch( "timestamp", &out_timestamp );
  outnt.Branch( "delta_timestamp", &out_delta_timestamp);
  outnt.Branch( "ftst", &out_ftst );

  uint64_t tmp_evId = 1;
  // loop over the events
  // --------------------
  for ( int iev=0; iev<nt->GetEntries() ; iev++){
    if( iev%100 == 0 ) niceBar( nt->GetEntries(), iev );

    nt->GetEntry(iev);

    // add empty events for those that have been lost
    while( tmp_evId < eventId ){
      out_timestamp = -1;
      out_delta_timestamp = -1;
      out_ftst = -1;
      out_eventId = tmp_evId;
      outnt.Fill();
      tmp_evId++;
    }
    out_timestamp = timestamp;
    out_delta_timestamp = delta_timestamp;
    out_ftst = ftst;
    out_eventId = eventId;
    tmp_evId = out_eventId + 1;
      //

    unordered_map<uint16_t,uint16_t> maxamp;
    unordered_map<uint16_t,uint16_t> sampmax;
    unordered_map<uint16_t,float> flex;
    unordered_map<uint16_t,float> tmax;
    unordered_map<uint16_t,vector<uint16_t>> amplitudes;

    // make hits
    // ---------

    // loop over the fired channels and organize them as hits
    for( uint64_t j=0; j < ampl->size() ; j++ ){
      int jch = channel->at(j);
      if(!JustHits && !det.isConnected(jch)) continue;
         
      amplitudes[jch].push_back( ampl->at(j));

      /// find maximum amplitude and its associated sample
      if( maxamp.find( jch ) == maxamp.end() ){
        maxamp[jch] = 0;
      }
      if( maxamp[jch]  <  ampl->at(j) ){
         maxamp[jch]   =  ampl->at(j);
         sampmax[jch]  = sample->at(j);
      }
    }

    // find the absissa of the line passing by the two samples with the larger amp diff
    for( auto &a : amplitudes ){
      int dmax = 0;
      uint16_t imax = 0;
      int npos = 0;
      int i=0;
      uint16_t prev;
      int diff = 0;
      //for( int i=0;i<a.second.size()-1; i++){
      for( auto &s : a.second){
        if( i > sampmax[a.first] ) break;
        diff = s - prev;
        if( i > 0) {
          if( diff > 0 ) npos++;
          if( diff > dmax ){
            dmax = diff;
            imax = i;
          }
        }
        prev = s;
        i++;
      }
      //flex[a.first] = (float)(2*imax+1)/2.;
      //if( imax > 0  && a.second.at(imax)-a.second.at(imax-1) > 0){
      if( imax > 0 ){
        float m = a.second.at(imax) - a.second.at(imax-1);
        flex[a.first] = (float)imax-1. - a.second.at(imax-1)/m ;
      }
      else flex[a.first] = 999.;
    }

    // find the time of max with a parabolic fit of the three bins around the sampmax
    for( auto &sm : sampmax){
      auto amp = amplitudes[sm.first];
      if( sm.second == 0 || sm.second==15 ) { tmax[sm.first] = sm.second; continue; } // TODO fix max

      float x0 = (float) (sm.second - 1.);
      float x1 = (float) (sm.second) ;
      float x2 = (float) (sm.second + 1.);
      float y0 = (float) (amp.at( sm.second - 1 ));
      float y1 = (float) (amp.at( sm.second  ));
      float y2 = (float) (amp.at( sm.second + 1 ));

      float max = ( x0*x0*y1 - x0*x0*y2 - x1*x1*y0 + x1*x1*y2 + x2*x2*y0 - x2*x2*y1  )\
                  /(2*(x0*y1 - x0*y2 - x1*y0 + x1*y2 + x2*y0 - x2*y1 ));
      tmax[sm.first] = max;

    }
    

    // fill a vector of hits
    hits->clear();
    for( auto &m : maxamp ){
      ahit.channel   = m.first;
      ahit.maxamp    = m.second;
      ahit.strip     = det.stripNb(m.first);
      ahit.axis      = det.axis(m.first);
      ahit.samplemax = sampmax[m.first];
      ahit.tdiff     = flex[m.first];
      ahit.timeofmax = tmax[m.first];
      ahit.samples.assign( amplitudes[m.first].begin(),  amplitudes[m.first].end() );
      hits->push_back(ahit);
    }


    // make clusters
    // -------------
    // a cluster is a sequence of contiguous hits
    // TODO: 
    //   1.  check for hits begin in the same part of the detector
    //   2.  check for missing/dead strip

    // select and sort hits, sorting probably not needed, but just in case
    
    vector<hit> *goodHits = new vector<hit>();
    copy_if(hits->begin(), hits->end(), back_inserter(*goodHits), checkHit);
    sort( hits->begin(), hits->end(), compareHits );

    cls->clear();
    int oldch = -1;
    uint16_t clId = 1;
    for( auto it = goodHits->begin(); !JustHits && it < goodHits->end(); ){ // skip this portion if JustHits is true
      // std::cout<<"channel: "<<it->channel<<std::endl;
      // start a new cluster
      if( oldch < 0 ){
        oldch = it->channel;
        int size = 0;
        int numCh = 0;
        int denCh = 0;
        int numSt = 0;
        int denSt = 0;

        char axis = det.axis(it->channel);

        // loop over the hits
        while( oldch >= 0 ){

          // compute the numerator and the denumerator for the centroid  
          numCh += it->channel * it->maxamp;
          denCh += it->maxamp;

          numSt += det.stripNb(it->channel) * it->maxamp;  
          denSt += it->maxamp;

          // std::cout<<det.getAll(it->channel)<<std::endl;

          // assign the cluster Id to the hit. 
          it->clusterId = clId; 

          // increase the size of the cluster
          size++;

          // look for the next hit, check that the next hit is a neighbourg
          it++;
          if( it == goodHits->end() || (it->channel - oldch) > 1 || !det.isNeighbour(oldch, it->channel) ){
            // TODO add here some conditions to skip missing strips and so on
            break;
          }
          
          oldch = it->channel;

        } // here the cluster is found

        // make a cluster
        cluster cl;
        cl.size     = size;
        cl.centroid = (float) numCh / denCh;
        cl.id       = clId;
        cl.stripCentroid = (float) numSt / denCh;
        cl.axis = axis;
        cls->push_back( cl );

        // if here, the cluster is finished. reset oldch and increase the clId for the next one
        oldch = -1;
        clId++;

      }// this was a cluster

    } // end loop over hits


    outnt.Fill();

    //if( eventId > 20  )break;

  }
  fout->Write();
  fout->Close();
  infile->Close();

  cout << setw(100) << "  \r"<< flush;
  cout << "finished processing " << name << endl;
}



int main( int argc, char **argv ){

  string basedir = argv[0];
  basedir = basedir.substr(0, basedir.size()-4);
  cout << basedir << endl;
  if( argc < 3 ) {
    // cerr << " no file name specified \n";
    cerr << " At least two arguments need to be specified \n file name and FEU number (from 1 to 5)\n";
    return 1;
  }

  string fname = argv[1];
  DreamTable det;

  if( fname.find( ".root" ) > fname.size() ) {cerr << fname << " is not a root file " << endl; return 1;}

  // size_t posFeu = fname.find("FEU");
  // if(posFeu > fname.size()){
  //   std::cout << "Filename doesn't contain FEU info" << std::endl;
  //   return 1;
  // }
  // int nbFeu = std::stoi(fname.substr(posFeu+3, 1));

  int nbFeu = atoi(argv[2]);

  if(nbFeu == 1){
    if( argc < 4 ){
      cerr << " Feu 1 is is connnected to two detectors, input the detector number as second argument :\n 1=MUR_strip \n 2=MUR_inter\n";
      return 1;
    }
    int nbDet = atoi(argv[3]);

    if(nbDet == 1){
      det = DreamTable(basedir + "../map/strip_map.txt", 0, 1, 2, 3);
      det.setInversion(true, true, false, true);
    }
    else if(nbDet == 2){
      det = DreamTable(basedir + "../map/inter_map.txt", 4, 5, 6, 7);
      det.setInversion(true, true, false, false);
    }
    else {cerr << "detector number invalid \n"; return 1; }
  }

  else if(nbFeu == 2){
    det = DreamTable(basedir + "../map/asa_map.txt", -1, 5, 6, -1);
    // det.setInversion(true, true, false, false);
    det.setInversion(false, false, true, true);
  }
  else if(nbFeu == 3){
    det = DreamTable(basedir + "../map/strip_map.txt", 4, 5, 6, 7);
    det.setInversion(true, true, false, false);
  }
  else if(nbFeu == 4){
    det = DreamTable(basedir + "../map/asa_map.txt", 4, 5, 6, 7);
    det.setInversion(false, false, true, true);
  }
  else if(nbFeu == 5) {cout << "WARNING: P2 map not yet implemented, just making hits \n"; JustHits = true; }
  else {cerr << "Feu number is invalid \n"; return 1;}
  reco( fname, det );

  return 0;
}

