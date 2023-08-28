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


int getZone(float pitchX, float pitchY){
  int zone = -1;
  if(pitchY == 1.){
    zone = 0;
  }else if(pitchY == 1.5){
    zone = 1;
  }else if(pitchY == 0.5){
    zone = 2;
  }

  if(pitchX == 1){
    zone += 0;
  }else if(pitchX == 1.5){
    zone += 3;
  }else if(pitchX == 0.5){
    zone += 6;
  }

  return zone;
}


void clusterSizeRegion(TChain* chain, std::string detname) {

  std::string graphMap = detname+"_Map.png";
  std::string graphStrip = detname+"_strips.png";
  std::string graphClSize = detname+"_ClSize.png";

  TTreeReader reader(chain);

  TTreeReaderValue< std::vector<hit> > hits( reader, "hits");
  TTreeReaderValue< std::vector<cluster> > cls( reader, "clusters");

  int nbZones = 9;

  TH1F *hcentroidX[nbZones];
  TH1F *hcentroidY[nbZones];
  TH1F *hclSizeX[nbZones];
  TH1F *hclSizeY[nbZones];

  for(int i=0; i<nbZones; i++){
    std::string labelCentroid = "hcentroid"+std::to_string(i);
    hcentroidX[i] = new TH1F((labelCentroid+"X").c_str(), "Centroid strips in x direction", 128,0,128);
    hcentroidY[i] = new TH1F((labelCentroid+"Y").c_str(), "Centroid strips in y direction", 128,0,128);
    hcentroidX[i]->SetXTitle("strip centroid"); hcentroidY[i]->SetXTitle("strip centroid");

    std::string labelclSize = "hclSize"+std::to_string(i);
    hclSizeX[i] = new TH1F((labelclSize+"X").c_str(), "cluster size in x direction", 10,-0.5,10.5);
    hclSizeY[i] = new TH1F((labelclSize+"Y").c_str(), "cluster size in y direction", 10,-0.5,10.5);
    hclSizeX[i]->SetXTitle("cluster size"); hclSizeY[i]->SetXTitle("cluster size");
  }

  TH2F *h2c = new TH2F("h2c", "cluster map", 128,0,128,128,0,128);
  h2c->SetXTitle("centroid on y direction strips");
  h2c->SetYTitle("centroid on x direction strips");

  std::vector<std::string> titles = { "1/1mm (y/x)", "1.5/1mm", "0.5/1mm", "1/1.5mm", "1.5/1.5mm", "0.5/1.5mm", "1/0.5mm", "1.5/0.5mm", "0.5/0.5mm" };

  std::vector<cluster> clX, clY;

  while( reader.Next() ){

    if( hits->size() == 0 ) continue;
    clX.clear();
    clY.clear();

    for( auto c : *cls ){
      if( c.axis == 'x' ){
        clX.push_back(c);
      }else if( c.axis == 'y' ){
        clY.push_back(c);
      }
    }

    for( auto x = clX.begin(); x < clX.end(); x++){
      for(auto y = clY.begin(); y < clY.end(); y++){
        int zone  = getZone(x->pitch, y->pitch);
        std::cout<<zone<<endl;
        h2c->Fill(y->stripCentroid, x->stripCentroid);
        hcentroidX[zone]->Fill(x->stripCentroid);
        hcentroidY[zone]->Fill(y->stripCentroid);

        hclSizeX[zone]->Fill(x->size);
        hclSizeY[zone]->Fill(y->size);
      }
    }
  }

  gStyle->SetOptStat(0);

  TCanvas *cclSize = new TCanvas("cclSize", "cclSize", 1600,1600);
  cclSize->Divide(3, 3);
  for(int i=0; i<9; i++){
    cclSize->cd(i+1);
    gPad->SetLogy();
    hclSizeX[i]->SetTitle(titles[i].c_str());
    hclSizeX[i]->Draw();

    hclSizeY[i]->SetLineColor(kRed);
    hclSizeY[i]->Draw("same");
  }
  cclSize->Print(graphClSize.c_str(), "png");


  TCanvas *cstrips = new TCanvas("cstrips", "cstrips", 1600,1600);
  cstrips->Divide(3, 3);
  for(int i=0; i<4; i++){
    cstrips->cd(i+1);
    hcentroidX[i]->SetTitle((titles[i]).c_str());
    hcentroidX[i]->Draw();

    hcentroidY[i]->SetLineColor(kRed);
    hcentroidY[i]->Draw("same");
  }
  cstrips->Print(graphStrip.c_str(), "png");

  gStyle->SetOptStat(0);
  TCanvas *c3 = new TCanvas("c3", "c3", 1000,1000);
  h2c->Draw("colz");
  gPad->SetLogz();
  c3->Print(graphMap.c_str(), "png");

}

int main(int argc, char const *argv[])
{
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

  clusterSizeRegion(chain, detName);

  return 0;
}

