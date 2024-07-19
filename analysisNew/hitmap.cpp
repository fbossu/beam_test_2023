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
#include "analysis.h"
#include "../style_sheet.h"

int main(int argc, char const *argv[])
{
  defStyle();
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

  StripTable det;
  if(detName.find("strip") != std::string::npos){
    det = StripTable(basedir+"../map/strip_map.txt");
  }
  else if(detName.find("asa") != std::string::npos){
    det = StripTable(basedir+"../map/asa_map.txt");
  }
  else if(detName.find("inter") != std::string::npos){
    det = StripTable(basedir+"../map/inter_map.txt");
  }
  else {
    std::cerr<<"Detector not found"<<std::endl;
    return 1;
  }

  TTreeReader reader(chain);
  TTreeReaderValue< std::vector<cluster> > clusters( reader, "clusters");
  TTreeReaderValue< std::vector<hit> > hits( reader, "hits");

  TH2F* h2strip = new TH2F("h2test", "strip number test", 129, -0.5, 128.5, 129, -0.5, 128.5);
  TH2F* h2gerber = new TH2F("h2gerber", "gerber test", 200, -120, 20, 200, -20, 120);
  TH1F* h1amp = new TH1F("hmaxamp", "maxamp per event", 500, 0, 500);

  std::cout<<"Nb triggers "<<chain->GetEntries()<<std::endl;
  while( reader.Next()){
    if(hits->size() == 0) continue;
    auto maxX = maxSizeClX(*clusters);
    auto maxY = maxSizeClY(*clusters);
    std::sort(hits->begin(), hits->end(), [](hit a, hit b){return a.maxamp > b.maxamp;});
    h1amp->Fill(hits->at(0).maxamp);
    if(maxX && maxY){
      auto hitX = getHits(*hits, maxX->id);
      auto hitY = getHits(*hits, maxY->id);
      h2strip->Fill(hitY[0].strip, hitX[0].strip);
      h2gerber->Fill(det.posY(maxX->stripCentroid)[0], det.posX(maxY->stripCentroid)[1]);
    }
  }

  TCanvas *c = new TCanvas("c", "c", 1000, 1000);
  h2strip->Draw("colz");
  // gPad->SetLogz();
  c->Print(Form("stripMap_%s.png", detName.c_str()));

  TCanvas *c2 = new TCanvas("c2", "c2", 1000, 1000);
  h2gerber->Draw("colz");
  // gPad->SetLogz();
  c2->Print(Form("gerberMap_%s.png", detName.c_str()));

  TCanvas *c3 = new TCanvas("c3", "c3", 1200, 800);
  h1amp->Draw();
  gPad->SetLogy();
  c3->Print(Form("maxamp_%s.png", detName.c_str()));

  // clusterSizeRegion(chain, detName, det);
  //clusterSizeLims(chain, detName, det, {80, 90}, {90, 100});

  return 0;
}

