#include <string>

#include "TFile.h"
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

void hitmap(std::string fname) {

  std::string graphname = fname.substr(0, fname.size()-5)+"_hitmap.png";
  std::string graphname2 = fname.substr(0, fname.size()-5)+"_dreamID.png";
  std::string graphname3 = fname.substr(0, fname.size()-5)+"_map.png";

  TFile *inf = TFile::Open( fname.c_str() );

  TTreeReader reader("events", inf);

  TTreeReaderValue< std::vector<hit> > hits( reader, "hits");
  TTreeReaderValue< std::vector<cluster> > cls( reader, "clusters");

  TH1F *dreamID = new TH1F("dreamID", "dream ID", 8, 0, 7);
  TH1F *channels = new TH1F("channels", "channels", 256, 0, 256);

  TH1F *hcentroidX = new TH1F("hcentroidX", "Centroid strips in x direction", 128,0,128);
  hcentroidX->SetXTitle("centroid");
  TH1F *hcentroidY = new TH1F("hcentroidY", "Centroid strips in y direction", 128,0,128);
  hcentroidY->SetXTitle("centroid");

  TH2F *hsizeX = new TH2F("hsizeX", "cluster size in x strips", 128,0,128, 20,0,20);
  hsizeX->SetXTitle("centroid");
  hsizeX->SetYTitle("cluster size");

  TH2F *hsizeY = new TH2F("hsizeY", "cluster size in y strips", 128,0,128, 20,0,20);
  hsizeY->SetXTitle("centroid");
  hsizeY->SetYTitle("cluster size");

  TH2F *h2c = new TH2F("h2c", "cluster map", 128,0,128,128,0,128);
  h2c->SetXTitle("centroid on y direction strips");
  h2c->SetYTitle("centroid on x direction strips");

  std::vector<cluster> clX, clY;


  while( reader.Next() ){

    if( hits->size() == 0 ) continue;
    clX.clear();
    clY.clear();

    for (auto h : *hits){
      channels->Fill(h.channel);
      dreamID->Fill(int(h.channel/64));
      // std::cout<<int(h.channel/64)<<std::endl;
    }

    for( auto c : *cls ){

      if( c.axis == 'x' ){
        clX.push_back(c);
        hcentroidX->Fill(c.stripCentroid);
        hsizeX->Fill(c.stripCentroid, c.size);
      }else if( c.axis == 'y' ){
        clY.push_back(c);
        hcentroidY->Fill(c.stripCentroid);
        hsizeY->Fill(c.stripCentroid, c.size);
      }
    }

    for( auto x = clX.begin(); x < clX.end(); x++){
      for(auto y = clY.begin(); y < clY.end(); y++){
        h2c->Fill(y->stripCentroid, x->stripCentroid);
      }
    }
  }

  // auto c = new TCanvas();
  // c->Divide(1,3);
  // c->cd(1);

  // hcentroid->Draw();
  // c->cd(2)->SetLogz();
  // h2c->Draw("colz");
  // c->cd(3)->SetLogz();
  // hsize->Draw("colz");
  // c->cd(4)->SetLogz();
  // hsize->ProjectionY()->Draw();

  gStyle->SetOptStat(0);
  TCanvas *c = new TCanvas("c", "c", 1600,1000);
  c->Draw();
  TPad *p1 = new TPad("p1","p1",0., 0., 0.24, 1.);
  p1->Draw();
  p1->Divide(1, 2);
  TPad *p11 = (TPad*)p1->cd(1);
  hcentroidX->Draw();
  p11->SetLogy();
  p11->Draw();
  TPad *p12 = (TPad*)p1->cd(2);
  p12->SetLogz();
  hsizeX->Draw("colz");
  p12->Draw();

  c->cd(0);
  TPad *p2 = new TPad("p2","p2",0.26, 0., 0.5, 1.);
  p2->Draw();
  p2->Divide(1, 2);
  TPad *p21 = (TPad*)p2->cd(1);
  hcentroidY->Draw();
  p21->SetLogy();
  p21->Draw();
  TPad *p22 = (TPad*)p2->cd(2);
  p22->SetLogz();
  hsizeY->Draw("colz");
  p22->Draw();

  c->cd(0);
  TPad *p3 = new TPad("p3","p3",0.51, 0.1, 1., 0.9);
  p3->Draw();
  p3->cd(0),
  p3->SetRightMargin(0.12);
  h2c->Draw("colz");
  p3->SetLogz();
  // p3->Draw();
  // // h2c->Draw("colz");

  c->Print(graphname.c_str(), "png");


  TCanvas *c2 = new TCanvas("c2", "c2", 1000,1000);
  c2->Divide(2, 1);
  c2->cd(1);
  dreamID->Draw();
  gPad->SetLogy();

  c2->cd(2);
  channels->Draw();
  gPad->SetLogy();

  c2->Print(graphname2.c_str(), "png");

  TCanvas *c3 = new TCanvas("c3", "c3", 1000,1000);
  h2c->Draw();
  gPad->SetLogz();
  c3->Print(graphname3.c_str(), "png");

  inf->Close();
}

int main(int argc, char const *argv[])
{
  std::string fname = argv[1];
  hitmap(fname);

  return 0;
}

