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

// if both cluster are not in the same zone return -1
int getZone(float pitchX, float pitchY=-1){
  
  int zone = -1;
  if(pitchX != pitchY and pitchY>=0) return zone;

  if(pitchX == 1.){
    zone = 0;
  }else if(pitchX == 0.8f){
    zone = 1;
  }else if(pitchX == 2.){
    zone = 2;
  }else if(pitchX == 1.5){
    zone = 3;
  }

  return zone;
}


void clusterSizeRegion(TChain* chain, std::string detname) {

  StripTable det("../map/asa_map.txt");

  std::string graphMap = detname+"_Map.png";
  std::string graphStrip = detname+"_strips.png";
  std::string graphClSize = detname+"_ClSize.png";

  TTreeReader reader(chain);

  TTreeReaderValue< std::vector<hit> > hits( reader, "hits");
  TTreeReaderValue< std::vector<cluster> > cls( reader, "clusters");

  TH1F *hcentroidX[4];
  TH1F *hcentroidY[4];
  TH1F *hclSizeX[4];
  TH1F *hclSizeY[4];
  std::vector<std::string> titles = { "1mm (x=y)", "0.8mm", "2mm", "1.5mm"};

  for(int i=0; i<4; i++){
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

  std::vector<cluster> clX, clY;

  while( reader.Next() ){

    if( hits->size() == 0 ) continue;
    clX.clear();
    clY.clear();

    for( auto c : *cls ){
      // std::cout<<c.pitch<<std::endl;
      // if(c.size<4) continue;

      if( c.axis == 'x' ){
        clX.push_back(c);
      }else if( c.axis == 'y' ){
        clY.push_back(c);
      }
    }

    for( auto x = clX.begin(); x < clX.end(); x++){
      for(auto y = clY.begin(); y < clY.end(); y++){
        float pitchX = det.pitchX(int(x->stripCentroid));
        float pitchY = det.pitchY(int(y->stripCentroid));
        int zone = getZone(pitchX, pitchY);
        if(zone>=0){
          h2c->Fill(y->stripCentroid, x->stripCentroid);
          hcentroidX[zone]->Fill(x->stripCentroid);
          hcentroidY[zone]->Fill(y->stripCentroid);

          hclSizeX[zone]->Fill(x->size);
          hclSizeY[zone]->Fill(y->size);
        }//else{ std::cout<<"WARNING "<<zone<<" "<<pitchX<<" "<<pitchY<<" "<<pitchX-pitchY<<std::endl; }
      }
    }
  }

  // gStyle->SetOptStat(0);

  TCanvas *cclSize = new TCanvas("cclSize", "cclSize", 1600,1000);
  cclSize->Divide(2, 2);
  for(int i=0; i<4; i++){
    cclSize->cd(i+1);
    gPad->SetLogy();
    hclSizeX[i]->SetTitle(("pitch: "+titles[i]).c_str());
    hclSizeX[i]->Draw();

    hclSizeY[i]->SetLineColor(kRed);
    hclSizeY[i]->Draw("same");
  }
  cclSize->cd(0);
  TLegend *leg = new TLegend(0.87,0.75,0.99,0.8);
  leg->AddEntry(hclSizeX[0],"cluster size in X","l");
  leg->AddEntry(hclSizeY[0],"cluster size in Y","l");
  leg->Draw();
  cclSize->Print(graphClSize.c_str(), "png");


  TCanvas *cstrips = new TCanvas("cstrips", "cstrips", 1600,1000);
  cstrips->Divide(2, 2);
  for(int i=0; i<4; i++){
    cstrips->cd(i+1);
    hcentroidX[i]->SetTitle(("pitch: "+titles[i]).c_str());;
    hcentroidX[i]->Draw();

    hcentroidY[i]->SetLineColor(kRed);
    hcentroidY[i]->Draw("same");
  }
  cstrips->cd(0);
  TLegend *legS = new TLegend(0.87,0.75,0.99,0.8);
  legS->AddEntry(hcentroidX[0],"strip centroid in X","l");
  legS->AddEntry(hcentroidY[0],"strip centroid in Y","l");
  legS->Draw();
  cstrips->Print(graphStrip.c_str(), "png");

  gStyle->SetOptStat(0);
  TCanvas *c3 = new TCanvas("c3", "c3", 1000,1000);
  h2c->Draw("colz");
  gPad->SetLogz();
  c3->Print(graphMap.c_str(), "png");

}



void clusterSizeLims(TChain* chain, std::string detname, std::vector<int> xlim, std::vector<int> ylim) {

  StripTable det("../map/asa_map.txt");
  std::vector<std::string> titles = { "1mm (x=y)", "0.8mm", "2mm", "1.5mm"};

  int zone = getZone(det.pitchX(xlim[0]));
  if(getZone(det.pitchX(xlim[1]))!=zone or getZone(det.pitchY(ylim[0]))!=zone or getZone(det.pitchY(ylim[1]))!=zone){
    std::cout<<"WARNING: The limits are no in the same region"<<std::endl;
  }

  float pitch = det.pitchX(xlim[0]);
  std::string strpitch = std::to_string(pitch).substr(0,3);

  std::string graphMap = detname+"_ref"+strpitch+"_Map.png";
  std::string graphStrip = detname+"_ref"+strpitch+"_strips.png";
  std::string graphClSize = detname+"_ref"+strpitch+"_ClSize.png";
  std::string graphAmpX = detname+"_ref"+strpitch+"_AmpX.png";
  std::string graphAmpY = detname+"_ref"+strpitch+"_AmpY.png";

  TTreeReader reader(chain);

  TTreeReaderValue< std::vector<hit> > hits( reader, "hits");
  TTreeReaderValue< std::vector<cluster> > cls( reader, "clusters");

  TH1F *hcentroidX = new TH1F("hcentroidX", "Centroid strips in x direction", 128,0,128);
  TH1F *hcentroidY = new TH1F("hcentroidY", "Centroid strips in y direction", 128,0,128);
  TH1F *hclSizeX = new TH1F("hclSizeX", "cluster size in x direction", 10,-0.5,10.5);
  TH1F *hclSizeY = new TH1F("hclSizeY", "cluster size in y direction", 10,-0.5,10.5);
  hcentroidX->SetXTitle("strip centroid"); hcentroidY->SetXTitle("strip centroid");
  hclSizeX->SetXTitle("cluster size"); hclSizeY->SetXTitle("cluster size");

  TH2F *h2c = new TH2F("h2c", "cluster map", 128,0,128,128,0,128);
  h2c->SetXTitle("centroid on y direction strips");
  h2c->SetYTitle("centroid on x direction strips");

  TH2F *h2ampX[6];
  TH2F *h2ampY[6];
  for(int i=0; i<6; i++){
    std::string label = "h2amp"+std::to_string(i);
    std::string title = "clusters size"+std::to_string(i+1);
    h2ampX[i] = new TH2F((label+"X").c_str(), ("X"+title).c_str(), xlim[1]-xlim[0]+1, xlim[0]-0.5, xlim[1]+0.5, 500, 200, 700);
    h2ampY[i] = new TH2F((label+"Y").c_str(), ("Y"+title).c_str(), ylim[1]-ylim[0]+1, ylim[0]-0.5, ylim[1]+0.5, 500, 200, 700);
    h2ampX[i]->SetXTitle("strip number"); h2ampY[i]->SetXTitle("strip number");
    h2ampX[i]->SetYTitle("amplitude"); h2ampY[i]->SetYTitle("amplitude");
  }

  std::vector<cluster> clX, clY;
  std::vector<hit> hX, hY;

  while( reader.Next() ){

    if( hits->size() == 0 ) continue;
    clX.clear(); clY.clear();
    hX.clear(); hY.clear();

    for(auto h : *hits){
      if(h.axis == 'x' and h.strip>xlim[0] and h.strip<xlim[1]) hX.push_back(h);
      if(h.axis == 'y' and h.strip>ylim[0] and h.strip<ylim[1]) hY.push_back(h);
    }

    for( auto c : *cls ){
      if( c.axis == 'x' and c.stripCentroid>xlim[0] and c.stripCentroid<xlim[1] ){
        clX.push_back(c);
        hcentroidX->Fill(c.stripCentroid);
        hclSizeX->Fill(c.size);
        if(c.size>6) continue;
        for( auto hitx = hX.begin(); hitx < hX.end(); hitx++){
          if(hitx->clusterId == c.id) h2ampX[c.size-1]->Fill(hitx->strip, hitx->maxamp);
        }
      }
      else if( c.axis == 'y' and c.stripCentroid>ylim[0] and c.stripCentroid<ylim[1] ){
        clY.push_back(c);
        hcentroidY->Fill(c.stripCentroid);
        hclSizeY->Fill(c.size);
        if(c.size>6) continue;
        for( auto hity = hY.begin(); hity < hY.end(); hity++){
          if(hity->clusterId == c.id) h2ampY[c.size-1]->Fill(hity->strip, hity->maxamp);
        }
      }
    }

    for( auto x = clX.begin(); x < clX.end(); x++){
      for(auto y = clY.begin(); y < clY.end(); y++){
          h2c->Fill(y->stripCentroid, x->stripCentroid);
      }
    }
  }

  TCanvas *cclSize = new TCanvas("cclSize", "cclSize", 1600,1000);
  gPad->SetLogy();
  hclSizeX->SetTitle(("pitch: "+titles[zone]).c_str());
  hclSizeX->Draw();

  hclSizeY->SetLineColor(kRed);
  hclSizeY->Draw("same");
  
  TLegend *leg = new TLegend(0.87,0.75,0.99,0.8);
  leg->AddEntry(hclSizeX,"cluster size in X","l");
  leg->AddEntry(hclSizeY,"cluster size in Y","l");
  leg->Draw();
  cclSize->Print(graphClSize.c_str(), "png");


  TCanvas *cstrips = new TCanvas("cstrips", "cstrips", 1600,1000);
  hcentroidX->SetTitle(("pitch: "+titles[zone]).c_str());;
  hcentroidX->Draw();

  hcentroidY->SetLineColor(kRed);
  hcentroidY->Draw("same");

  TLegend *legS = new TLegend(0.87,0.75,0.99,0.8);
  legS->AddEntry(hcentroidX,"strip centroid in X","l");
  legS->AddEntry(hcentroidY,"strip centroid in Y","l");
  legS->Draw();
  cstrips->Print(graphStrip.c_str(), "png");

  gStyle->SetOptStat(0);
  TCanvas *c3 = new TCanvas("c3", "c3", 1000,1000);
  h2c->Draw("colz");
  gPad->SetLogz();
  c3->Print(graphMap.c_str(), "png");

  TCanvas *campX = new TCanvas("campX", "campX", 1600,1000);
  campX->Divide(3, 2);
  for(int i=0; i<6; i++){
    campX->cd(i+1);
    h2ampX[i]->Draw("colz");
  }
  campX->Print(graphAmpX.c_str(), "png");

  TCanvas *campY = new TCanvas("campY", "campY", 1600,1000);
  campY->Divide(3, 2);
  for(int i=0; i<6; i++){
    campY->cd(i+1);
    h2ampY[i]->Draw("colz");
  }
  campY->Print(graphAmpY.c_str(), "png");
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

  // clusterSizeRegion(chain, detName);
  clusterSizeLims(chain, detName, {2, 16}, {15, 25});
  clusterSizeLims(chain, detName, {37, 52}, {50, 65});
  clusterSizeLims(chain, detName, {70, 90}, {65, 85});
  clusterSizeLims(chain, detName, {100, 110}, {105, 115});

  return 0;
}

