#include <string>

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TTreeReader.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TGraphErrors.h"
#include "TLatex.h"

#include "../reco/definitions.h"
#include "../map/StripTable.h"
#include "clusterSize.h"


std::vector<double> beamPosition(std::string fname, StripTable det, TH2F* map=NULL){

  TH1F *hx = new TH1F("hx", "", 200,-29.5,129.5);
  TH1F *hy = new TH1F("hy", "", 200,-129.5,29.5);

  TFile *fin = TFile::Open(fname.c_str(), "read");
  TTreeReader reader("events", fin);

  TTreeReaderValue< std::vector<hit> > hits( reader, "hits");
  TTreeReaderValue< std::vector<cluster> > cls( reader, "clusters");
  std::vector<cluster> clX, clY;

  while( reader.Next() ){

    if( hits->size() == 0 ) continue;
    clX.clear(); clY.clear();

    for( auto c : *cls ){
      if(c.axis=='x') {clX.push_back(c);}
      else if(c.axis=='y') {clY.push_back(c);}
    }
    if(map != NULL){
      auto maxX = std::max_element(clX.begin(), clX.end(),
                      [](const cluster& a, const cluster& b) { return a.size < b.size; });
      auto maxY = std::max_element(clY.begin(), clY.end(),
                      [](const cluster& a, const cluster& b) { return a.size < b.size; });

      if (maxX == clX.end() or maxY == clY.end()) continue;
      if(det.zone(int(maxX->stripCentroid), int(maxY->stripCentroid)) < 0) continue;

      map->Fill(det.posY(maxY->stripCentroid)[0], det.posX(maxX->stripCentroid)[1]);
      hx->Fill(det.posX(maxX->stripCentroid)[1]);
      hy->Fill(det.posY(maxY->stripCentroid)[0]);
      // std::cout<<maxX->size<<" "<<maxY->size<<std::endl;
      // for( auto x = clX.begin(); x < clX.end(); x++){
      //   for(auto y = clY.begin(); y < clY.end(); y++){
      //     if(det.zone(int(x->stripCentroid), int(y->stripCentroid)) < 0) continue;
      //     map->Fill(det.posY(y->stripCentroid)[0], det.posX(x->stripCentroid)[1]);
      //   }
      // }
    }
  }
  fin->Close();
  return { hx->GetMean(), hx->GetStdDev(), hy->GetMean(), hy->GetStdDev() };
}


// void correlation1pos(int fname)

int main(int argc, char const *argv[])
{

  std::string basedir = argv[0];
  basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";
  std::cout << basedir << std::endl;

  TChain* chainStrip = new TChain("events");
  TChain* chainAsa = new TChain("events");

  StripTable detStrip(basedir+"../map/strip_map.txt");
  StripTable detAsa(basedir+"../map/asa_map.txt");

  // std::vector<std::string> validPos = {"POS02", "POS05", "POS11", "POS13", "POS15", "POS17"};
  std::map<int, std::vector<double>> mapAsa;
  std::map<int, std::vector<double>> mapStrip;

  TH2F *hposAsa = new TH2F("hposAsa", "cluster map", 500,-129,29,500,-29,129);
  hposAsa->SetXTitle("centroid on y direction strips (mm)");
  hposAsa->SetYTitle("centroid on x direction strips (mm)");

  TH2F *hposStrip = new TH2F("hposStrip", "cluster map", 500,-129,29,500,-29,129);
  hposStrip->SetXTitle("centroid on y direction strips (mm)");
  hposStrip->SetYTitle("centroid on x direction strips (mm)");

  for( int i = 1; i < argc; i++) {
    std::string input = argv[i];
    if( input.find( "FEU4.root" ) != std::string::npos ){
      int pos = std::stoi( input.substr(input.find("POS")+3, input.find("POS")+5) );
      std::cout<<input<<" pos: "<<pos<<std::endl;
      mapAsa[pos] = beamPosition(input, detAsa, hposAsa);
    }
    if( input.find( "FEU1_strip.root" ) != std::string::npos ){
      int pos = std::stoi( input.substr(input.find("POS")+3, input.find("POS")+5) );
      std::cout<<input<<" pos: "<<pos<<std::endl;
      mapStrip[pos] = beamPosition(input, detStrip, hposStrip);
    }
  }

  TGraphErrors *grX = new TGraphErrors();
  TGraphErrors *grY = new TGraphErrors();

  grX->SetTitle("X direction strips");
  grX->GetXaxis()->SetTitle("asacusa y positon (mm)"); grX->GetYaxis()->SetTitle("strip y positon (mm)");

  grY->SetTitle("Y direction strips");
  grY->GetXaxis()->SetTitle("asacusa x positon (mm)"); grY->GetYaxis()->SetTitle("strip x positon (mm)");

  for(auto itAsa = mapAsa.begin(); itAsa != mapAsa.end(); itAsa++){
    if(itAsa->second[1] > 10 or itAsa->second[3] > 10) continue;

    for(auto itStrip = mapStrip.begin(); itStrip != mapStrip.end(); itStrip++){
      if(itStrip->second[1] > 10 or itStrip->second[3] > 10) continue;
      
      if(itAsa->first == itStrip->first){
        if(itAsa->second[1] > 10 or itAsa->second[3] > 10);
        grX->SetPoint(grX->GetN(), itAsa->second[0], itStrip->second[0]);
        grX->SetPointError(grX->GetN()-1, itAsa->second[1], itStrip->second[1]);

        grY->SetPoint(grY->GetN(), itAsa->second[2], itStrip->second[2]);
        grY->SetPointError(grY->GetN()-1, itAsa->second[3], itStrip->second[3]);
      }
    }
  }

  
  TLatex latex;
  latex.SetTextSize(0.04);
  // latex.SetTextAlign(13);

  TCanvas *c1 = new TCanvas("c1", "c1", 1100,1000);
  hposAsa->SetStats(0);
  hposAsa->Draw("colz");
  gPad->SetLogz();
  for(auto itAsa = mapAsa.begin(); itAsa != mapAsa.end(); itAsa++){
    if(itAsa->second[1] > 10 or itAsa->second[3] > 10) continue;
      std::string label = std::to_string(itAsa->first);
      latex.DrawLatex(itAsa->second[2],itAsa->second[0],(label).c_str());
  }
  c1->Print("ASA_Feu4_map_label1.png", "png");

  TCanvas *c2 = new TCanvas("c2", "c2", 1100,1000);
  hposStrip->SetStats(0);
  hposStrip->Draw("colz");
  gPad->SetLogz();
  for(auto itStrip = mapStrip.begin(); itStrip != mapStrip.end(); itStrip++){
    if(itStrip->second[1] > 10 or itStrip->second[3] > 10) continue;
      std::string label = std::to_string(itStrip->first);
      latex.DrawLatex(itStrip->second[2],itStrip->second[0],(label).c_str());
  }
  c2->Print("strip_Feu4_map_label1.png", "png");

  TCanvas *c3 = new TCanvas("c3", "c3", 1600,1000);
  c3->Divide(2,1, 0.01, 0.01);
  c3->cd(1);
  // grX->SetStats(0);
  grX->Draw("AP");
  // gPad->SetLogz();
  c3->cd(2);
  // grX->SetStats(0);
  grY->Draw("AP");
  c3->Print("graph_correlation_strip_asa_1.png", "png");

  //   TString input = argv[i];
  //   // std::cout<<argv[i]<<std::endl;

  //   if( input.Contains( "FEU4.root" ) ){
  //     for(std::string pos : validPos){
  //       if(input.Contains(pos)) {
  //         chainAsa->Add( input ); 
  //         std::cout<<input<<std::endl;
  //       }
  //     }
  //   }
  //   else if( input.Contains( "FEU1_strip.root" ) ){
  //     for(std::string pos : validPos){
  //       if(input.Contains(pos)) {
  //         chainStrip->Add( input ); 
  //         std::cout<<input<<std::endl;
  //       }
  //     }
  //   }
  //   else{
  //     std::cout<<"Detector Name: "<<argv[i]<<std::endl;
  //     detName = argv[i];
  //   }
  // }

  // StripTable detStrip(basedir+"../map/strip_map.txt");
  // StripTable detAsa(basedir+"../map/asa_map.txt");

  // TTreeReader readerStrip(chainStrip);
  // TTreeReader readerAsa(chainAsa);

  // TTreeReaderValue< std::vector<hit> > hitsStrip( readerStrip, "hits");
  // TTreeReaderValue< std::vector<cluster> > clsStrip( readerStrip, "clusters");

  // TTreeReaderValue< std::vector<hit> > hitsAsa( readerAsa, "hits");
  // TTreeReaderValue< std::vector<cluster> > clsAsa( readerAsa, "clusters");
  
  // TH2F *h2x = new TH2F("h2x", "cluster correlation x strips", 100,-29,129,100,-29,129);
  // h2x->SetXTitle("asa y cluster position (mm)");
  // h2x->SetYTitle("strip y cluster position (mm)");

  // TH2F *h2y = new TH2F("h2y", "cluster correlation y strips", 100,-129,29,100,-129,29);
  // h2y->SetXTitle("asa x cluster position (mm)");
  // h2y->SetYTitle("strip x cluster position (mm)");

  // // std::vector<cluster> clXAsa, clYAsa;
  // // std::vector<cluster> clXStrip, clYStrip;

  // while( readerStrip.Next() ){

  //   bool isAsa = readerAsa.Next();
  //   if(!isAsa){
  //     std::cout<<"Strip has more events !"<<std::endl;
  //     break;
  //   }

  //   if( hitsStrip->size() == 0 or hitsAsa->size()==0 ) continue;

  //   for( auto cStrip : *clsStrip ){
  //     for( auto cAsa : *clsAsa ){
  //       if(cAsa.axis == 'x' and cStrip.axis =='x') h2x->Fill(detAsa.posX(cAsa.stripCentroid)[1], detStrip.posX(cStrip.stripCentroid)[1]);
  //       if(cAsa.axis == 'y' and cStrip.axis =='y') h2y->Fill(detAsa.posY(cAsa.stripCentroid)[0], detStrip.posY(cStrip.stripCentroid)[0]);
  //     }
  //   }
  // }

  // bool isAsa = readerAsa.Next();
  // if(isAsa){
  //   std::cout<<"Asa has more events !"<<std::endl;
  // }

  // TCanvas *c3 = new TCanvas("c3", "c3", 1800,1000);
  // c3->Divide(2,1, 0.05, 0.05);
  // c3->cd(1);
  // h2x->SetStats(0);
  // h2x->Draw("colz");
  // // gPad->SetLogz();
  // c3->cd(2);
  // h2y->SetStats(0);
  // h2y->Draw("colz");
  // c3->Print("correlation_strip_asa.png", "png");

  return 0;
}

