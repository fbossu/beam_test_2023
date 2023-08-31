#include <string>

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TTreeReader.h"
#include "TStyle.h"

#include "../reco/definitions.h"

#include <vector>
#include <map>
#include <cmath>

using std::vector;

void centroid( std::string graphname, TChain* chain) {

  // chain->SetEntry(0);
  TTreeReader reader(chain);

  TTreeReaderValue< std::vector<hit> > hits( reader, "hits");
  TTreeReaderValue< std::vector<cluster> > cls( reader, "clusters");


  TH2F *hsample = new TH2F("sample","sample",16,0,16,2050,0,4100);

  while( reader.Next() ){

    if( hits->size() == 0 ) continue;
   
    std::map<int,vector<int>> mh;
    for ( int i=0; i<hits->size(); i++){
      mh[hits->at(i).clusterId].push_back(i);
    }

    for( auto cl : *cls ){

      for( auto ih : mh[cl.id] ){
        if( hits->at(ih).channel == round(cl.centroid) ){
          for( int is=0; is<16; is++ ){
            hsample->Fill( is, hits->at(ih).samples.at(is) );
          }
        }
      }
    }

  }

  gStyle->SetOptStat(0);
  TCanvas *c = new TCanvas("c", "c", 1800,1000);
  //c->Divide(4,2,0.01,0.01);
  //for(int i=0; i<8; i++){
    //c->cd(i+1);
    //gPad->SetLeftMargin(0.15);
    //gPad->SetRightMargin(0.15);
    //hamplitude[i]->Draw("colz");
    //gPad->SetLogz();
  //}
  hsample->Draw("colz");
  c->Print(graphname.c_str(), "png");
  hsample->SaveAs("test.root");
}


int main(int argc, char const *argv[])
{
  TChain* chain = new TChain("events");
  TString input;
  std::string graphname = "centroid.png";

  for( int i = 1; i < argc; i++) {
    input = argv[i];
    if( input.Contains( "root" ) ){
      chain->Add(input);
      std::cout<<"File "<<input.Data()<<" added"<<std::endl;
    }
    if( input.Contains( ".png" ) )  graphname = input;
  }
  
centroid(graphname, chain);

  return 0;
}

