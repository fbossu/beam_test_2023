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

  TFile *inf = TFile::Open( fname.c_str() );

  TTreeReader reader("events", inf);

  TTreeReaderValue< std::vector<hit> > hits( reader, "hits");
  TTreeReaderValue< std::vector<cluster> > cls( reader, "clusters");

  TH1F *dreamID = new TH1F("dreamID", "dream ID", 8, 0, 7);

  TH1F *hcentroidX = new TH1F("hcentroidX", "Strip centroid x", 100,0,128);
  hcentroidX->SetXTitle("centroid");
  TH1F *hcentroidY = new TH1F("hcentroidY", "Strip centroid y", 100,0,128);
  hcentroidY->SetXTitle("centroid");

  TH2F *hsizeX = new TH2F("hsizeX", "cluster size in x", 100,0,128, 20,0,20);
  hsizeX->SetXTitle("centroid");
  hsizeX->SetYTitle("cluster size");

  TH2F *hsizeY = new TH2F("hsizeY", "cluster size in y", 100,0,128, 20,0,20);
  hsizeY->SetXTitle("centroid");
  hsizeY->SetYTitle("cluster size");

  TH2F *h2c = new TH2F("h2c", "cluster map", 100,0,128,100,0,128);
  h2c->SetXTitle("centroid in x");
  h2c->SetYTitle("centroid in y (strip nb inverted)");

  std::vector<cluster> clX, clY;


  while( reader.Next() ){

    if( hits->size() == 0 ) continue;
    clX.clear();
    clY.clear();

    for (auto h : *hits){
      dreamID->Fill(int(h.channel/64));
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
        h2c->Fill(x->stripCentroid, 127-y->stripCentroid);
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

  inf->Close();


  TCanvas *c2 = new TCanvas("c2", "c2", 1000,1000);
  dreamID->Draw();
  gPad->SetLogy();
  c2->Print(graphname2.c_str(), "png");
}

int main(int argc, char const *argv[])
{
  std::string fname = argv[1];
  hitmap(fname);

  return 0;
}

