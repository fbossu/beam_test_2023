#include <string>

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TTreeReader.h"
#include "TStyle.h"
#include "TLegend.h"

#include "../reco/definitions.h"
#include "../map/StripTable.h"
#include "clusterSize.h"

int main(int argc, char const *argv[])
{

  std::string basedir = argv[0];
  basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";
  std::cout << basedir << std::endl;

  TChain* chain = new TChain("events");
  std::string detName = "test";
  for( int i = 1; i < argc; i++) {

    TString input = argv[i];

    if( input.Contains( "root" ) ){
      chain->Add( input );
    }
    else{
      std::cout<<"Detector Name: "<<argv[i]<<std::endl;
      detName = argv[i];
    }
  }

  StripTable det(basedir+"../map/inter_map.txt");

  TTreeReader reader(chain);
  TTreeReaderValue< std::vector<hit> > hits( reader, "hits");

  TH2F* h2test = new TH2F("h2test", "strip number test", 128, 256, 384, 128, 384, 512);

  while( reader.Next()){
    if(hits->size() == 0) continue;
    hit maxX, maxY;
    int maxAmpX = 0, maxAmpY = 0;
    for(auto& h : *hits){
      if(h.axis == 'x'){
        if(h.maxamp > maxAmpX){
          maxX = h;
          maxAmpX = h.maxamp;
        }
      }
      else if(h.axis == 'y'){
        if(h.maxamp > maxAmpY){
          maxY = h;
          maxAmpY = h.maxamp;
        }
      }
    }
    if(maxAmpX == 0 or maxAmpY == 0) continue;
    h2test->Fill(maxY.channel, maxX.channel);
  }

  TCanvas *c = new TCanvas("c", "c", 1000,1000);
  h2test->Draw("colz");
  gPad->SetLogz();
  c->Print("test.png", "png");

  // clusterSizeRegion(chain, detName, det);
  //clusterSizeLims(chain, detName, det, {80, 90}, {90, 100});

  return 0;
}

