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

// void plotBeamPos(int argc, char const *argv[]){

//   std::string basedir = argv[0];
//   basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";
//   std::cout << basedir << std::endl;

//   TChain* chainStrip = new TChain("events");
//   TChain* chainAsa = new TChain("events");

//   StripTable detStrip(basedir+"../map/strip_map.txt");
//   StripTable detAsa(basedir+"../map/asa_map.txt");

//   // std::vector<std::string> validPos = {"POS02", "POS05", "POS11", "POS13", "POS15", "POS17"};
//   std::map<int, std::vector<double>> mapAsa;
//   std::map<int, std::vector<double>> mapStrip;

//   TH2F *hposAsa = new TH2F("hposAsa", "cluster map", 500,-129,29,500,-29,129);
//   hposAsa->SetXTitle("centroid on y direction strips (mm)");
//   hposAsa->SetYTitle("centroid on x direction strips (mm)");

//   TH2F *hposStrip = new TH2F("hposStrip", "cluster map", 500,-129,29,500,-29,129);
//   hposStrip->SetXTitle("centroid on y direction strips (mm)");
//   hposStrip->SetYTitle("centroid on x direction strips (mm)");

//   for( int i = 1; i < argc; i++) {
//     std::string input = argv[i];
//     if( input.find( "FEU4.root" ) != std::string::npos ){
//       int pos = std::stoi( input.substr(input.find("POS")+3, input.find("POS")+5) );
//       std::cout<<input<<" pos: "<<pos<<std::endl;
//       mapAsa[pos] = beamPosition(input, detAsa, hposAsa);
//     }
//     if( input.find( "FEU1_strip.root" ) != std::string::npos ){
//       int pos = std::stoi( input.substr(input.find("POS")+3, input.find("POS")+5) );
//       std::cout<<input<<" pos: "<<pos<<std::endl;
//       mapStrip[pos] = beamPosition(input, detStrip, hposStrip);
//     }
//   }

//   TGraphErrors *grX = new TGraphErrors();
//   TGraphErrors *grY = new TGraphErrors();

//   grX->SetTitle("X direction strips");
//   grX->GetXaxis()->SetTitle("asacusa y positon (mm)"); grX->GetYaxis()->SetTitle("strip y positon (mm)");

//   grY->SetTitle("Y direction strips");
//   grY->GetXaxis()->SetTitle("asacusa x positon (mm)"); grY->GetYaxis()->SetTitle("strip x positon (mm)");

//   for(auto itAsa = mapAsa.begin(); itAsa != mapAsa.end(); itAsa++){
//     if(itAsa->second[1] > 10 or itAsa->second[3] > 10) continue;

//     for(auto itStrip = mapStrip.begin(); itStrip != mapStrip.end(); itStrip++){
//       if(itStrip->second[1] > 10 or itStrip->second[3] > 10) continue;
      
//       if(itAsa->first == itStrip->first){
//         if(itAsa->second[1] > 10 or itAsa->second[3] > 10);
//         grX->SetPoint(grX->GetN(), itAsa->second[0], itStrip->second[0]);
//         grX->SetPointError(grX->GetN()-1, itAsa->second[1], itStrip->second[1]);

//         grY->SetPoint(grY->GetN(), itAsa->second[2], itStrip->second[2]);
//         grY->SetPointError(grY->GetN()-1, itAsa->second[3], itStrip->second[3]);
//       }
//     }
//   }

  
//   TLatex latex;
//   latex.SetTextSize(0.04);
//   // latex.SetTextAlign(13);

//   TCanvas *c1 = new TCanvas("c1", "c1", 1100,1000);
//   hposAsa->SetStats(0);
//   hposAsa->Draw("colz");
//   gPad->SetLogz();
//   for(auto itAsa = mapAsa.begin(); itAsa != mapAsa.end(); itAsa++){
//     if(itAsa->second[1] > 10 or itAsa->second[3] > 10) continue;
//       std::string label = std::to_string(itAsa->first);
//       latex.DrawLatex(itAsa->second[2],itAsa->second[0],(label).c_str());
//   }
//   c1->Print("ASA_Feu4_map_label1.png", "png");

//   TCanvas *c2 = new TCanvas("c2", "c2", 1100,1000);
//   hposStrip->SetStats(0);
//   hposStrip->Draw("colz");
//   gPad->SetLogz();
//   for(auto itStrip = mapStrip.begin(); itStrip != mapStrip.end(); itStrip++){
//     if(itStrip->second[1] > 10 or itStrip->second[3] > 10) continue;
//       std::string label = std::to_string(itStrip->first);
//       latex.DrawLatex(itStrip->second[2],itStrip->second[0],(label).c_str());
//   }
//   c2->Print("strip_Feu4_map_label1.png", "png");

//   TCanvas *c3 = new TCanvas("c3", "c3", 1600,1000);
//   c3->Divide(2,1, 0.01, 0.01);
//   c3->cd(1);
//   // grX->SetStats(0);
//   grX->Draw("AP");
//   // gPad->SetLogz();
//   c3->cd(2);
//   // grX->SetStats(0);
//   grY->Draw("AP");
//   c3->Print("graph_correlation_strip_asa_1.png", "png");

// }

void correlation1pos(char const *argv[]){

  TFile *fasa = TFile::Open(argv[1], "read");
  TFile *fstrip = TFile::Open(argv[2], "read");

  std::string fnameMM = argv[1];
  int pos = std::stoi( fnameMM.substr(fnameMM.find("POS")+3, fnameMM.find("POS")+5) );
  std::string graphname = "correlation_POS"+std::to_string(pos)+".png";

  double zposAsa = -785.6, TyAsa = -6.7189, TxAsa = 24.4; // POS05
  double zposStrip = -305.2, TyStrip = -7.4934, TxStrip = 29.08; //POS05
  // double zposAsa = -785.6, TyAsa = -61.5856, TxAsa = 24.817; // POS13
  // double zposStrip = -305.6, TyStrip = -63.382, TxStrip = 28.377; //POS13

  TTreeReader asa("events", fasa);
  TTreeReader strip("events", fstrip);

  std::string basedir = argv[0];
  StripTable detStrip("../map/strip_map.txt");
  StripTable detAsa("../map/asa_map.txt");
 
  TH2F* h2x = new TH2F("hx", "Correlation X strip VS asaFEU4", 200,0,18, 200,0,18);
  h2x->GetXaxis()->SetTitle("position y axis stripFEU1 (mm)");
  h2x->GetYaxis()->SetTitle("position y axis asaFEU4 (mm)");
  TH2F* h2y = new TH2F("hy", "Correlation Y strip VS asaFEU4",  200,0,16,  200,0,16);
  h2y->GetXaxis()->SetTitle("position x axis stripFEU1 (mm)");
  h2y->GetYaxis()->SetTitle("position x axis asaFEU4 (mm)");

  TTreeReaderValue< std::vector<cluster> > clsAsa( asa, "clusters");
  TTreeReaderValue< std::vector<cluster> > clsStrip( strip, "clusters");

  while(asa.Next()){
    bool ok = strip.Next();
    if(!ok){
      std::cout<<"WARNING: More asa events"<<std::endl;
      break;
    }
    std::vector<cluster> Xasa, Yasa, Xstrip, Ystrip;
    std::copy_if (clsAsa->begin(), clsAsa->end(), std::back_inserter(Xasa),
                  [](const cluster& c){return c.axis=='x';} );
    std::copy_if (clsAsa->begin(), clsAsa->end(), std::back_inserter(Yasa),
                  [](const cluster& c){return c.axis=='y';} );
    std::copy_if (clsStrip->begin(), clsStrip->end(), std::back_inserter(Xstrip),
                  [](const cluster& c){return c.axis=='x';} );
    std::copy_if (clsStrip->begin(), clsStrip->end(), std::back_inserter(Ystrip),
                  [](const cluster& c){return c.axis=='y';} );

    if(Xasa.empty() or Yasa.empty() or Xstrip.empty() or Ystrip.empty()) continue;

    auto maxXasa = *std::max_element(Xasa.begin(), Xasa.end(),
                       [](const cluster& a,const cluster& b) { return a.size < b.size; });
    auto maxYasa = *std::max_element(Yasa.begin(), Yasa.end(),
                       [](const cluster& a,const cluster& b) { return a.size < b.size; });
    auto maxXstrip = *std::max_element(Xstrip.begin(), Xstrip.end(),
                       [](const cluster& a,const cluster& b) { return a.size < b.size; });
    auto maxYstrip = *std::max_element(Ystrip.begin(), Ystrip.end(),
                       [](const cluster& a,const cluster& b) { return a.size < b.size; });

    h2x->Fill(detAsa.posX(maxXasa.stripCentroid)[1]+TyAsa, detStrip.posX(maxXstrip.stripCentroid)[1]+TyStrip);
    h2y->Fill(detAsa.posY(maxYasa.stripCentroid)[0]+TxAsa, detStrip.posY(maxYstrip.stripCentroid)[0]+TxStrip);
  }
  if(strip.Next()) std::cout<<"WARNING: More strip events"<<std::endl;

  TCanvas *c = new TCanvas("c", "c", 1600,1000);
  c->Divide(2,1, 0.01, 0.01);
  c->cd(1);
  h2x->Draw("colz");
  gPad->SetLogz();
  c->cd(2);
  h2y->Draw("colz");
  gPad->SetLogz();
  c->Print(graphname.c_str(), "png");
}



int main(int argc, char const *argv[])
{

  correlation1pos(argv);

  return 0;
}

