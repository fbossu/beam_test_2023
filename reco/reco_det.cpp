#include "TFile.h"
#include "TFile.h"
#include "TTree.h"
#include "TInterpreter.h"
#include "definitions.h"

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <iomanip>

#include "../map/DetectorTable.h"

using namespace std;

bool compareHits( hit &a, hit &b ) { return a.channel < b.channel; }

void niceBar( int tot, int i, int N=50 ){
  cout << "[ ";
  for( int j=0; j < (float)i/tot*N; j++)
    cout << "-";
  for( int j=(float)i/tot*N; j < N; j++)
    cout << " ";
  cout << " ]\r";
  cout << flush;
}

void reco( string name, DetectorTable det) {

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
  name = name.substr( name.rfind('/')+1  ).insert( 0, "rec_");
  TFile *fout  = TFile::Open( name.data(), "recreate");
  TTree outnt( "events","test_beam_2023");
  outnt.SetDirectory( fout );

  hit ahit;
  vector<hit> *hits = new vector<hit>();
  vector<cluster> *cls = new vector<cluster>();
  outnt.Branch( "hits", &hits );
  outnt.Branch( "clusters", &cls );
  outnt.Branch( "eventId", &eventId );
  outnt.Branch( "timestamp", &timestamp );
  outnt.Branch( "delta_timestamp", &delta_timestamp);
  outnt.Branch( "ftst", &ftst );

  // loop over the events
  // --------------------
  for ( int iev=0; iev<nt->GetEntries() ; iev++){
    if( iev%100 == 0 ) niceBar( nt->GetEntries(), iev );

    nt->GetEntry(iev);

    map<uint16_t,uint16_t> maxamp;
    map<uint16_t,uint16_t> sampmax;
    map<uint16_t,float> flex;
    map<uint16_t,vector<uint16_t>> amplitudes;

    // make hits
    // ---------

    // loop over the fired channels and organize them as hits
    for( uint64_t j=0; j < ampl->size() ; j++ ){
      int jch = channel->at(j);
      if(!det.isConnected(jch)) continue;
         
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

    // find the inflection point
    for( auto &a : amplitudes ){
      uint16_t max = 0;
      uint16_t imax = 0;
      //for( int i=0;i<a.second.size()-1; i++){
      for( int i=0;i<a.second.size()-1; i++){
        if( i > sampmax[a.first] ) break;
        uint16_t diff = a.second.at(i+1) - a.second.at(i);
        if( diff > max ){
          max = diff;
          imax = i;
        }
      }
      flex[a.first] = (float)(2*imax+1)/2.;
    }


    // fill a vector of hits
    hits->clear();
    for( auto &m : maxamp ){
      ahit.channel   = m.first;
      ahit.maxamp    = m.second;
      ahit.samplemax = sampmax[m.first];
      ahit.inflex    = flex[m.first];
      hits->push_back(ahit);
    }


    // make clusters
    // -------------
    // a cluster is a sequence of contiguous hits
    // TODO: 
    //   1.  check for hits begin in the same part of the detector
    //   2.  check for missing/dead strip

    // sort hits, probably not needed, but just in case
    sort( hits->begin(), hits->end(), compareHits );

    cls->clear();
    int oldch = -1;
    uint16_t clId = 1;
    for( auto it = hits->begin(); it < hits->end(); ){
      // std::cout<<"channel: "<<it->channel<<std::endl;
      // start a new cluster
      if( oldch < 0 ){
        oldch = it->channel;
        int size = 0;
        int numCh = 0;
        int denCh = 0;
        int numSt = 0;
        int denSt = 0;

        int pitch = det.getPitch(it->channel);
        int inter = det.getInter(it->channel);
        char axis = det.getAxis(it->channel);

        // check if the first hit is on the edge of a region
        bool edge = det.isEdge(oldch);

        // loop over the hits
        while( oldch >= 0 ){

          // compute the numerator and the denumerator for the centroid  
          numCh += it->channel * it->maxamp;
          denCh += it->maxamp;

          numSt += det.getStripNb(it->channel) * it->maxamp;  
          denSt += it->maxamp;

          // std::cout<<det.getAll(it->channel)<<std::endl;

          // assign the cluster Id to the hit. 
          it->clusterId = clId; 

          // increase the size of the cluster
          size++;

          // look for the next hit, check that the next hit is a neighbourg
          it++;
          if( it == hits->end() || (it->channel - oldch) > 1 || !det.isNeighbour(oldch, it->channel) ){
            // TODO add here some conditions to skip missing strips and so on
            edge = edge || det.isEdge(it->channel);
            break;
          }
          else {
            oldch = it->channel;
          }

        } // here the cluster is found

        // make a cluster
        cluster cl;
        cl.size     = size;
        cl.centroid = (float) numCh / denCh;
        cl.id       = clId;
        cl.stripCentroid = (float) numSt / denCh;
        cl.pitch = pitch;
        cl.inter = inter;
        cl.axis = axis;
        cls->push_back( cl );

        // if here, the cluster is finished. reset oldch and increase the clId for the next one
        oldch = -1;
        clId++;

      }// this was a cluster

    } // end loop over hits


    outnt.Fill();

    //if( eventId > 10  )break;

  }
  fout->Write();
  fout->Close();
  infile->Close();

  cout << setw(100) << "  \r"<< flush;
  cout << "finished processing " << name << endl;
}



int main( int argc, char **argv ){

  if( argc < 2 ) {
    // cerr << " no file name specified \n";
    cerr << " At least one argument need to be specified\n";
    return 1;
  }

  string fname = argv[1];

  DetectorTable  det = DetectorTable("../map/strip_map.txt", 4, 5, 6, 7);
  det.setInversion(false, true, false, true);

  reco( fname, det );

  return 0;
}

