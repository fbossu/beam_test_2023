#include <string>
#include <numeric>

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
#include "../banco/definition_banco.h"
#include "../map/StripTable.h"
#include "../analysis/clusterSize.h"


void plotStripMap(std::string fbanconame, std::string fname){

  TFile *fMM = TFile::Open(fname.c_str(), "read");
  TFile *fbanco = TFile::Open(fbanconame.c_str(), "read");
  
  std::string run = fname.substr(fname.find("POS"), 5);
  std::string alignName = "../map/alignFiles/stripFEU1_" + run + ".txt";
  StripTable det("../map/strip_map.txt", alignName);

  TTreeReader MM("events", fMM);
  TTreeReaderValue< std::vector<cluster> > cls( MM, "clusters");

  TTreeReader banco("events", fbanco);
  TTreeReaderValue< std::vector<banco::track> > tracks( banco, "tracks");

  TH2F *h2test = new TH2F("h2test", "strip number test", 150, 5, 10, 150, 3, 9);

  while( MM.Next()){
    banco.Next();
    if(tracks->size() == 0) continue;
    if(tracks->at(0).chi2x>1. or tracks->at(0).chi2y>1.) continue;
    if(cls->size() == 0) continue;

    std::vector<cluster> Xcls, Ycls;
    std::copy_if (cls->begin(), cls->end(), std::back_inserter(Xcls),
                  [](const cluster& c){return c.axis=='x';} );
    std::copy_if (cls->begin(), cls->end(), std::back_inserter(Ycls),
                  [](const cluster& c){return c.axis=='y';} );

    if(Xcls.empty() or Ycls.empty()) continue;

    auto maxX = *std::max_element(Xcls.begin(), Xcls.end(),
                       [](const cluster& a,const cluster& b) { return a.size < b.size; });
    auto maxY = *std::max_element(Ycls.begin(), Ycls.end(),
                       [](const cluster& a,const cluster& b) { return a.size < b.size; });

    std::vector<double> detPos = det.pos3D(maxX.stripCentroid, maxY.stripCentroid);
    h2test->Fill(detPos[0], detPos[1]);
  }

  TCanvas *c = new TCanvas("c", "c", 1000,1000);
  h2test->Draw("colz");
  gPad->SetLogz();
  c->Print("test.png", "png");

}


void plotTracks(std::string fnameBanco){

    TFile *fbanco = TFile::Open(fnameBanco.c_str(), "read");
    TTreeReader banco("events", fbanco);
    TTreeReaderValue< std::vector<banco::track> > tracks( banco, "tracks");

    TH2F* h2tr = new TH2F("h2tr", "banco", 200, 5, 10, 200, 4, 8);
    h2tr->SetXTitle("x (mm)");
    h2tr->SetYTitle("y (mm)");

    TH1F* h1trx = new TH1F("h1trx", "x0", 200, 5, 10);
    h1trx->SetXTitle("x (mm)");
    TH1F* h1try = new TH1F("h1try", "y0", 200, 4, 8);
    h1try->SetXTitle("y (mm)");

    TH1F* h1trmx = new TH1F("h1trmx", "mx", 200, -0.005, 0.005);
    h1trmx->SetXTitle("mx");
    TH1F* h1trmy = new TH1F("h1trmy", "my", 200, -0.005, 0.005);
    h1trmy->SetXTitle("my");

    // double z = 0;
    double z= 785.6;

    while(banco.Next()){
      for(auto tr : *tracks){
        if(tr.chi2x>1. or tr.chi2y>1.) continue;
        h2tr->Fill(tr.x0 + z*tr.mx, tr.y0 + z*tr.my);
        h1trx->Fill(tr.x0 + z*tr.mx);
        h1try->Fill(tr.y0 + z*tr.my);
        h1trmx->Fill(tr.mx);
        h1trmy->Fill(tr.my);
      }
    }
  
    TCanvas *c2 = new TCanvas("c2", "c2", 1000,1000);
    h2tr->Draw("colz");
    // gPad->SetLogz();
    c2->Print("tracks_asaFEU4.png", "png");

    TLatex latex;
    latex.SetTextSize(0.035);
    std::string label;

    TCanvas *c3 = new TCanvas("c3", "c3", 1600,1000);
    c3->Divide(2,2);
    c3->cd(1);
    h1trx->Fit("gaus");
    h1trx->Draw();
    label = "#sigma_{x}: "+ std::to_string(h1trx->GetFunction("gaus")->GetParameter(2)).substr(0, 5)+"mm";
    latex.DrawLatexNDC(0.14, 0.85, (label).c_str());

    c3->cd(2);
    h1try->Fit("gaus");
    h1try->Draw();
    label = "#sigma_{y}: "+ std::to_string(h1try->GetFunction("gaus")->GetParameter(2)).substr(0, 5)+"mm";
    latex.DrawLatexNDC(0.14, 0.85, (label).c_str());

    c3->cd(3);
    h1trmx->Fit("gaus");
    h1trmx->Draw();
    label = "#sigma_{mx}: "+ std::to_string(h1trmx->GetFunction("gaus")->GetParameter(2)).substr(0, 7);
    latex.DrawLatexNDC(0.14, 0.85, (label).c_str());

    c3->cd(4);
    h1trmy->Fit("gaus");
    h1trmy->Draw();
    label = "#sigma_{my}: "+ std::to_string(h1trmy->GetFunction("gaus")->GetParameter(2)).substr(0, 7);
    latex.DrawLatexNDC(0.14, 0.85, (label).c_str());
    c3->Print("tracksXY_asaFEU4.png", "png");
}


void efficiencyMap(std::string fnameBanco, std::string fnameMM){
  
    std::string run = fnameMM.substr(fnameMM.find("POS"), 5);
    std::string alignName = "../map/alignFiles/stripFEU1_" + run + ".txt";
    StripTable det("../map/strip_map.txt", alignName);
  
  
    std::string graphname = "efficiencyMap_"+run+"_stripFEU1_cut.png";
  
    double zpos = -305.6;
  
    TH2F *h2tr = new TH2F("h2tr", "banco", 80, 4, 11, 80, 2, 10);
    h2tr->SetXTitle("x (mm)");
    h2tr->SetYTitle("y (mm)");
  
    TH2F *h2det = new TH2F("h2det", "MM",  80, 4, 11, 80, 2, 10);
    h2det->SetXTitle("x (mm)");
    h2det->SetYTitle("y (mm)");
    
    TFile *fbanco = TFile::Open(fnameBanco.c_str(), "read");
    TFile *fMM = TFile::Open(fnameMM.c_str(), "read");
  
    TTreeReader MM("events", fMM);
    TTreeReader banco("events", fbanco);
  
    TTreeReaderValue< std::vector<cluster> > cls( MM, "clusters");
    TTreeReaderValue< std::vector<hit> > hits( MM, "hits");
    TTreeReaderValue< std::vector<banco::track> > tracks( banco, "tracks");
  
    std::vector<float> Xstrip, Ystrip;
    std::vector<float> xtrack, ytrack;
  
    int ev = 0, n=0;
  
    while( MM.Next()){
      bool isBanco = banco.Next();
      if(!isBanco){
        std::cout<<"WARNING: Missing banco event"<<std::endl; 
        continue;
      }
      ev++;
      if(hits->size() == 0) continue;
  
      for(auto tr : *tracks){
        if(tr.chi2x>1. or tr.chi2y>1.) continue;
        double xtr = tr.x0 + zpos*tr.mx;
        double ytr = tr.y0 + zpos*tr.my;

        h2tr->Fill(xtr, ytr);
        auto maxX = maxSizeClX(*cls);
        auto maxY = maxSizeClY(*cls);
        if(maxX && maxY){
          // if(maxX->size != 1) continue;
          std::cout<<"\ncoincidence XY "<<maxX->stripCentroid<<" "<<maxY->stripCentroid<<std::endl;
          std::vector<double> detPos = det.pos3D(maxX->stripCentroid, maxY->stripCentroid);
          std::cout<<"pos axisxy "<<detPos[0]<<" "<<detPos[1]<<std::endl;
          h2det->Fill(detPos[0], detPos[1]);
        }
      }
    }
    if(banco.Next()) std::cout<<"WARNING: Missing MM event"<<std::endl;

    TCanvas *c3 = new TCanvas("c3", "c3", 1000,1000);
    h2det->SetStats(11111);
    gStyle->SetPalette(kRainBow);
    h2det->Divide(h2tr);
    h2det->Draw("colz");
    gPad->SetLogz();
    gPad->SetGrid();
    c3->Print(graphname.c_str(), "png");
    fbanco->Close();
    fMM->Close();
}


void coincidence(std::string fnameBanco, std::string fnameMM){

  StripTable det("../map/strip_map.txt");
  // StripTable det("../map/asa_map.txt");
  
  int pos = std::stoi( fnameMM.substr(fnameMM.find("POS")+3, fnameMM.find("POS")+5) );

  std::string graphname = "bancoCoincidence_POS"+std::to_string(pos)+"_stripFEU1_test_Y.png";
  // std::string graphname = "bancoCoincidence_POS"+std::to_string(pos)+"_asaFEU4_Y80.png";

  int stNb = 101; char axis = 'y';

  double zpos = -305.6;
  // double zpos = -785.6;
  // double zpos = -205.377; //FEU1 POS16

  // z pos on murwell strip: -305.6
  // z pos of asa strip: -785.6
  // residu(fnameBanco, fnameMM, det, -305.2, graphname);

  // TH2F *h2c = new TH2F("h2c", "Map banco with MM coincidence", 1500, 4*1024*0.02688, 5*1024*0.02688, 1500, 0, 512*0.02924);
  // TH2F *h2c = new TH2F("h2c", "Map banco with MM coincidence", 150, 6, 16, 150, 2, 12);
  TH2F *h2c = new TH2F("h2c", "Map banco with MM coincidence", 150, 4, 11, 150, 2, 10);
  h2c->SetXTitle("x (mm)");
  h2c->SetYTitle("y (mm)");

  // TH2F *h2f = new TH2F("h2f", "full beam spot", 150, 6, 16, 150, 2, 12);
  TH2F *h2f = new TH2F("h2f", "full beam spot",  150, 4, 11, 150, 2, 10);
  h2f->SetXTitle("x (mm)");
  h2f->SetYTitle("y (mm)");
  
  TFile *fbanco = TFile::Open(fnameBanco.c_str(), "read");
  TFile *fMM = TFile::Open(fnameMM.c_str(), "read");

  TTreeReader MM("events", fMM);
  TTreeReader banco("events", fbanco);

  TTreeReaderValue< std::vector<cluster> > cls( MM, "clusters");
  TTreeReaderValue< std::vector<hit> > hits( MM, "hits");
  TTreeReaderValue< std::vector<banco::track> > tracks( banco, "tracks");

  std::vector<float> Xstrip, Ystrip;
  std::vector<float> xtrack, ytrack;

  // MM.Next();
  int ev = 0, n=0;

  while( MM.Next() && ev<10000000){
    bool isBanco = banco.Next();
    if(!isBanco){
      std::cout<<"WARNING: Missing banco event"<<std::endl; 
      continue;
    }
    ev++;
    if(hits->size() == 0) continue;

    for(auto tr : *tracks){
      if(tr.chi2x>1. or tr.chi2y>1.) continue;
      double xdet = tr.x0 + zpos*tr.mx;
      double ydet = tr.y0 + zpos*tr.my;
      bool isXcl = false, isYcl = false, keepTrack = false;
      std::vector<hit> hitsAxis;
      h2f->Fill(xdet, ydet);

      // std::copy_if (hits->begin(), hits->end(), std::back_inserter(hitsAxis),
      //               [axis](const hit& h){return h.axis==axis;} );
      // if(hitsAxis.size() == 0) continue;

      // auto maxHit = *std::max_element(hitsAxis.begin(), hitsAxis.end(),
      //                    [axis](const hit& a,const hit& b) { return a.maxamp < b.maxamp; });

      // int Nmax = std::count_if(hitsAxis.begin(), hitsAxis.end(),
      //                             [maxHit](const hit& h) { return h.maxamp > 0.90*maxHit.maxamp;});
      
      // std::cout<<maxHit.strip<<std::endl;
      // if(maxHit.strip == stNb){
      //   keepTrack = true; 
      // }
      auto maxX = maxSizeClX(*cls);
      auto maxY = maxSizeClY(*cls);
    
      if( (maxX && axis=='x')){
        auto hitsX = getHits(*hits, maxX->id);
        if(maxX->size == 1 and hitsX.at(0).maxamp > 400){
          // if(hitsX[0].strip == stNb) keepTrack = true;
          keepTrack = true;
          std::cout<<hitsX.at(0).strip<<std::endl;
        }
      }
      if( (maxY && axis=='y')){
        auto hitsY = getHits(*hits, maxY->id);
        if(maxY->size == 1 and hitsY.at(0).maxamp > 400){
          // if(hitsY[0].strip == stNb) keepTrack = true;
          keepTrack = true;
          std::cout<<hitsY.at(0).strip<<std::endl;
        }
      }

      // if( (maxY && maxY)){
      //   auto hitsY = getHits(*hits, maxY->id);
      //   if(maxY->size == 1 and hitsY.at(0).maxamp > 400){
      //     // if(hitsY[0].strip == stNb) keepTrack = true;
      //     keepTrack = true;
      //     std::cout<<hitsY.at(0).strip<<std::endl;
      //   }
      // }
      if(keepTrack){
        h2c->Fill(xdet, ydet);
      }
    }
  }
  // std::cout<<n<<std::endl;

  if(banco.Next()) std::cout<<"WARNING: Missing MM event"<<std::endl;

  TCanvas *c3 = new TCanvas("c3", "c3", 1000,1000);
  h2c->SetStats(11111);
  h2c->Draw("colz");
  h2f->Draw("same");
  gPad->SetLogz();
  gPad->Modified();
  gPad->Update();
  gPad->SetGrid();

  TLatex latex;
  latex.SetTextSize(0.025);
  std::string label;
  label = "Strip Nb: "+ std::to_string(stNb) + axis;
  latex.DrawLatexNDC(0.14, 0.85, (label).c_str());
  label = "pitch: "+ std::to_string(det.pitchX(stNb)).substr(0, 5)+"mm";
  latex.DrawLatexNDC(0.14, 0.82, (label).c_str());

  c3->Print(graphname.c_str(), "png");

  fbanco->Close();
  fMM->Close();

}


int main(int argc, char const *argv[])
{

  std::string basedir = argv[0];
  basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";
  std::cout << basedir << std::endl;

  std::string fnameBanco =  argv[1];
  std::string fnameMM =  argv[2];

  // plotStripMap(fnameBanco, fnameMM);
  // efficiencyMap(fnameBanco, fnameMM);
  // coincidence(fnameBanco, fnameMM);
  plotTracks(fnameBanco);

  return 0;
}

