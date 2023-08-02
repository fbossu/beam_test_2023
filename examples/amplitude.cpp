#include <string>

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TTreeReader.h"
#include "TStyle.h"

#include "../reco/definitions.h"

// struct hit {
//   uint16_t channel;
//   uint16_t maxamp;
//   uint16_t samplemax;
//   float    inflex;
//   uint16_t clusterId;

// };

// struct cluster {
//   float    centroid;
//   uint16_t size;
//   uint16_t id;
// };

void amplitude(std::string graphname, TChain* chain) {

  // chain->SetEntry(0);
  TTreeReader reader(chain);

  TTreeReaderValue< std::vector<hit> > hits( reader, "hits");
  // TTreeReaderValue< std::vector<cluster> > cls( reader, "clusters");

  TH2F *hamplitude[8];
  for(int i=0; i<8; i++){
    std::string label = "amplitude"+std::to_string(i);
    std::string title = "dream #"+std::to_string(i);
    hamplitude[i] = new TH2F(label.c_str(), title.c_str(), 64,0,63, 500,0,4096);
    hamplitude[i]->SetXTitle("channel ID");
    hamplitude[i]->SetYTitle("amplitude");
  }

  while( reader.Next() ){

    if( hits->size() == 0 ) continue;
    
    for (auto h : *hits){
      // std::cout<<h.channel<<" "<<h.maxamp<<std::endl;
      hamplitude[h.channel/64]->Fill(h.channel%64, h.maxamp);
    }
  }

  gStyle->SetOptStat(0);
  TCanvas *c = new TCanvas("c", "c", 1800,1000);
  c->Divide(4,2,0.01,0.01);
  for(int i=0; i<8; i++){
    c->cd(i+1);
    gPad->SetLeftMargin(0.15);
    gPad->SetRightMargin(0.15);
    hamplitude[i]->Draw("colz");
    gPad->SetLogz();
  }
  c->Print(graphname.c_str(), "png");
}


int main(int argc, char const *argv[])
{
  TChain* chain = new TChain("events");
  TString input;
  std::string graphname = "amplitude.png";

  for( int i = 1; i < argc; i++) {
    input = argv[i];
    if( input.Contains( "root" ) ){
      chain->Add(input);
      std::cout<<"File "<<input.Data()<<" added"<<std::endl;
    }
    if( input.Contains( ".png" ) )  graphname = input;
  }
  
  amplitude(graphname, chain);

  return 0;
}

