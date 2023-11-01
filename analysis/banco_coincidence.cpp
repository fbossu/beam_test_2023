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


void plotStripMap(StripTable det, std::string fname){

  TFile *fMM = TFile::Open(fname.c_str(), "read");

  TTreeReader MM("events", fMM);
  TTreeReaderValue< std::vector<cluster> > cls( MM, "clusters");

  TH2F *h2test = new TH2F("h2test", "strip number test", 128, -0.5, 127.5, 128, -0.5, 127.5);

  while( MM.Next()){
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

    h2test->Fill(maxY.stripCentroid, maxX.stripCentroid);
  }

  TCanvas *c = new TCanvas("c", "c", 1000,1000);
  h2test->Draw("colz");
  gPad->SetLogz();
  c->Print("test.png", "png");

}

int main(int argc, char const *argv[])
{

  std::string basedir = argv[0];
  basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";
  std::cout << basedir << std::endl;

  // StripTable det(basedir+"../map/strip_map.txt");
  StripTable det(basedir+"../map/asa_map.txt");

  std::string fnameBanco =  argv[1];
  std::string fnameMM =  argv[2];

  plotStripMap(det, fnameMM);


  int pos = std::stoi( fnameMM.substr(fnameMM.find("POS")+3, fnameMM.find("POS")+5) );

  std::string graphname = "bancoCoincidence_POS"+std::to_string(pos)+"_stripFEU1_X100.png";
  // std::string graphname = "bancoCoincidence_POS"+std::to_string(pos)+"_asaFEU4_Y80.png";

  int stNb = 100; char axis = 'x';

  double zpos = -305.6;
  // double zpos = -785.6;
  // double zpos = -205.377; //FEU1 POS16

  // z pos on murwell strip: -305.6
  // z pos of asa strip: -785.6
  // residu(fnameBanco, fnameMM, det, -305.2, graphname);

  // TH2F *h2c = new TH2F("h2c", "Map banco with MM coincidence", 1500, 4*1024*0.02688, 5*1024*0.02688, 1500, 0, 512*0.02924);
  // TH2F *h2c = new TH2F("h2c", "Map banco with MM coincidence", 150, 6, 16, 150, 2, 12);
  TH2F *h2c = new TH2F("h2c", "Map banco with MM coincidence", 150, 0, 15, 150, -3, 17);
  h2c->SetXTitle("x (mm)");
  h2c->SetYTitle("y (mm)");

  // TH2F *h2f = new TH2F("h2f", "full beam spot", 150, 6, 16, 150, 2, 12);
  TH2F *h2f = new TH2F("h2f", "full beam spot", 150, -5, 20, 150, -5, 20);
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

      std::copy_if (hits->begin(), hits->end(), std::back_inserter(hitsAxis),
                    [axis](const hit& h){return h.axis==axis;} );
      if(hitsAxis.size() == 0) continue;

      auto maxHit = *std::max_element(hitsAxis.begin(), hitsAxis.end(),
                         [axis](const hit& a,const hit& b) { return a.maxamp < b.maxamp; });

      int Nmax = std::count_if(hitsAxis.begin(), hitsAxis.end(),
                                  [maxHit](const hit& h) { return h.maxamp > 0.90*maxHit.maxamp;});
      // if(Nmax>1) std::cout<<Nmax<<std::endl;

      // if(maxHit.clusterId != maxHit2.clusterId) std::cout<<"AHHHHHH"<<std::endl;
      // if(maxHit.strip == 75 and maxHit.axis == 'y'){
      
      if(maxHit.strip == stNb and Nmax==1){
        keepTrack = true; 
        // std::cout<<maxHit.maxamp<<std::endl;
      }
      if(keepTrack){
        h2c->Fill(xdet, ydet);
      } // else std::cout<<"cc"<<std::endl;
    }
  }
  std::cout<<n<<std::endl;

  if(banco.Next()) std::cout<<"WARNING: Missing MM event"<<std::endl;

  TCanvas *c3 = new TCanvas("c3", "c3", 1000,1000);
  h2c->SetStats(11111);
  h2c->Draw("colz");
  h2f->Draw("same");
  gPad->SetLogz();
  gPad->Modified();
  gPad->Update();

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

