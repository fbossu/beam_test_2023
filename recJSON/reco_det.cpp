#include "TFile.h"
#include "TChain.h"
#include "TTree.h"
#include "TInterpreter.h"
#include "../reco/definitions.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>
#include <iomanip>

#include "../map/DreamTable.h"
#include "parse_json.h"

#include <getopt.h>

using namespace std;


// =====================================================================
bool compareHits( hit &a, hit &b ) { return a.channel < b.channel; }

// bool goodHit( hit &a ) { return a.samplemax > 1 and a.samplemax < 11 and a.maxamp > 350; }
bool goodHit( hit &a ) { return true; }

void niceBar( int tot, int i, int N=50 ){
  cout << "[ ";
  for( int j=0; j < (float)i/tot*N; j++)
    cout << "-";
  for( int j=(float)i/tot*N; j < N; j++)
    cout << " ";
  cout << " ]\r";
  cout << flush;
}

cluster makeCluster( vector<hit*> &hitcl, int clId){

  double centroidNum = 0.;
  double centroidDen = 0.;
  double stripCentroidNum = 0.;
  char axis = hitcl.at(0)->axis;
  // cout << "axis " << axis << endl;

  for( auto h : hitcl ){
    // if( h->axis != axis ) cout << "ERROR: hits in the same cluster have different axis" << endl;
    h->clusterId = clId;
    centroidNum += h->channel * (h->maxamp-256);
    stripCentroidNum += h->strip * (h->maxamp-256);
    centroidDen += h->maxamp-256;
  }
  cluster cl;
  cl.id = clId;
  cl.size = hitcl.size();
  cl.centroid = centroidNum / centroidDen;
  cl.stripCentroid = stripCentroidNum / centroidDen;
  cl.axis = axis;
  return cl;
}

// =====================================================================
bool JustHits = false;
int NEV=-1;
int NSKIP=-1;
// =====================================================================
void reco( TChain *nt, DreamTable det, string outFile="frec.root", int nbSample=16, bool verbose=false) {

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
  TFile *fout  = TFile::Open(outFile.c_str(), "recreate");
  TTree outnt("events", "test_beam_2023");
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
  cout << "Processing " << nt->GetEntries() << " events " << endl;
  for ( int iev=0; iev<nt->GetEntries(); iev++){

    if( NSKIP>0 && iev < NSKIP ) continue;
    if( NEV>0 && iev > NEV ) break;
    if( iev%100 == 0 ) niceBar( nt->GetEntries(), iev );
    nt->GetEntry(iev);
    if(verbose) { cout << "Event " << eventId << " ampl size " << ampl->size() << " sample size " << sample->size() << " channel size " << channel->size() << endl; }

    // if( iev>10 ) break;
    // add empty events for those that have been lost
    while( tmp_evId < eventId ){
      out_timestamp = -1;
      out_delta_timestamp = -1;
      out_ftst = -1;
      out_eventId = tmp_evId;
      outnt.Fill();
      tmp_evId++;
    }

    // for(auto i : *channel) {
    //   cout << i << " ";
    // }
    // cout << endl;

    out_timestamp = timestamp;
    out_delta_timestamp = delta_timestamp;
    out_ftst = ftst;
    out_eventId = eventId;
    tmp_evId = out_eventId + 1;

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
      // cout << "channel " << jch << ", sample " << sample->at(j) << endl;
      if(!JustHits && !det.isConnected(jch)){
        if( verbose )cout << "Skipping unconnected channel " << jch << endl;
        continue;
      }

      if( amplitudes.find( jch ) == amplitudes.end() ){
        amplitudes[jch] = vector<uint16_t>(nbSample, 0);
      }
      amplitudes[jch][sample->at(j)] = ampl->at(j);

      // find maximum amplitude and its associated sample
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
      if(verbose) { cout << "   channel " << a.first << " has " << a.second.size() << " samples; sampleMax " << sampmax[a.first] << "; ampMax " << maxamp[a.first] << endl; }
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
      if( sm.second == 0 || sm.second==amp.size()-1 ) { tmax[sm.first] = sm.second; continue; } 

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
    //   2.  check for missing/dead strip

    // select and sort hits, sorting probably not needed, but just in case
    
    sort( hits->begin(), hits->end(), compareHits );
    
    cls->clear();
    vector<hit*> hitCl;
    int clId = 1;

    if( !JustHits ){
      for( auto &it : *hits){

        if( !goodHit(it) ){
          it.clusterId = 0;
          continue;
        }

        if( hitCl.size() == 0 ){
          hitCl.push_back(&it);
        }
        else if( det.isNeighbour(hitCl.back()->channel, it.channel) ){
          hitCl.push_back(&it);
        }
        else{
          cls->push_back( makeCluster(hitCl, clId) );
          hitCl.clear();
          hitCl.push_back(&it);
          clId++;
        }
      }
      if( hitCl.size() > 0){
        cls->push_back( makeCluster(hitCl, clId) );
        hitCl.clear();
      }
    }

    outnt.Fill();

    //if( eventId > 20  )break;

  }
  fout->Write();
  fout->Close();

  cout << setw(100) << "  \r"<< flush;
  cout << "finished processing " << endl;
}
// ------------------------------------------
void print_help(){
  std::cout << " reconstruction for micromegas prototypes \n"
            << "Usage:  reco_det -j PATH/TO/JSON -d DETNAME \n"
            << "Options: \n"
            << " -j [string] path to json file \n"
            << " -d [string] detector name\n"
            << " -n [int] max numer of events\n"
            << " -s [int] skip events\n"
            << " -m [string] map file path\n"
            << " -l [string] local run path\n"
            << " -H make only hits\n"
            << " -v verbose\n"
            << " -S [int] numer of samples\n"
            << "For Local Usage:  reco_det -l [path to root] -d [path to map file] \n";

}
// ------------------------------------------


int main( int argc, char **argv ){

  string basedir = argv[0];
  basedir = basedir.substr(0, basedir.size()-8);
  int nbSample = 16;
  bool verbose = false;

  string jsonPath   = "";
  string detName    = "";
  string localPath  = "";


  int opt;
  while((opt = getopt(argc, argv, "j:d:n:s:m:vHS:l:")) != -1) { 
    switch(opt) { 
      case 'j':
        jsonPath = optarg;
        cout << " json file " <<  jsonPath << endl;
        break;
      case 'd':
        detName = optarg;
        cout << " detector name " <<  detName << endl;
        break;
      case 'n':
        NEV = atoi(optarg);
        cout << " max number of events " <<  NEV << endl;
        break;
      case 's':
        NSKIP = atoi(optarg);
        cout << " skip events " <<  NSKIP << endl;
        break;
      case 'H':
        JustHits = true;
        cout << " hits only " << endl;
        break;
      case 'v':
        verbose = true;
        cout << " verbose " << endl;
        break;
      case 'S':
        nbSample = atoi(optarg);
        cout << " number of samples " <<  nbSample << endl;
        break;
      case 'l':
        localPath = optarg;
        cout << " local run path " <<  localPath << endl;
        break;
      default:
        print_help();
        return 1;
    }
  }

  // local use
  if( localPath != "" ){
    DreamTable det;
    //det = DreamTable( detName, pj.x1Dream(), pj.x2Dream(), pj.y1Dream(), pj.y2Dream());
    det = DreamTable( detName, 4,5,6,7);
    det.setInversion(true, true, false, false); // if connectors are plugged in the wrong direction
  
    string outFile = "reco_" + localPath;
    cout << "Output file: " << outFile << endl;
    TChain *ch = new TChain("nt");
    ch->AddFile(localPath.data());
    reco(ch, det, outFile, nbSample, verbose);
    return 0;
  }

  if( jsonPath == "" || detName == ""  ) {
    print_help();
    return 1;
  }

  ParseJson pj(jsonPath, detName);
  vector<string> subRuns = pj.subRuns();
  if(subRuns.size() == 0){
    cerr << "No subruns found in JSON file " << argv[1] << endl;
    return 1;
  }

  cout << "Found " << subRuns.size() << " subruns in JSON file: " << argv[1] << ":" << endl;

  for(auto sr : subRuns){
    cout << " "<< sr << ",";

    vector<string> decodedFiles = pj.decodeFiles( sr, localPath );
    cout << "Found " << decodedFiles.size() << " decoded files in subrun: " << sr << endl;
    TChain *ch = new TChain("nt");
    for(auto f : decodedFiles){
      ch->Add(f.c_str());
      cout << "Added file " << f << endl;
    }
 
    cout << "connectors: " << pj.x1Dream() << ", " << pj.x2Dream() << ", " << pj.y1Dream() << ", " << pj.y2Dream() << endl;
  
    DreamTable det;
    det = DreamTable(basedir + "../map/rd542_map.txt", pj.x1Dream(), pj.x2Dream(), pj.y1Dream(), pj.y2Dream());
    //det.setInversion(false, false, false, false); // if connectors are plugged in the wrong direction
    cout << "inversions " << pj.x1Inv()<<" "<<pj.x2Inv()<<" "<<pj.y1Inv()<<" "<<pj.y2Inv() << endl;
    det.setInversion(pj.x1Inv(),pj.x2Inv(),pj.y1Inv(),pj.y2Inv()); // if connectors are plugged in the wrong direction
  
    string outFile = "reco_" + detName+"_"+sr+ ".root";
    cout << "Output file: " << outFile << endl;
    reco(ch, det, outFile, nbSample, verbose);
 
  }
  return 0;
}
