#include "reco/definitions.h"
#include "an2025/utils.h"
#include "map/StripTable.h"
#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TTreeReader.h"
#include "TStyle.h"
#include "TLegend.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]){

  if( argc < 2 ){
    cout << "missing root file\n";
    return 1;
  }

  string basedir = argv[0];
  basedir = basedir.substr(0, basedir.find_last_of("\\/") + 1);

  StripTable det( 
      basedir + "../map/geic0_map.txt" ,
      basedir + "geic0_align.txt"
      );


  axis *ax = createAxis( "x [mm]", 800 , -100, 400 ); 
  axis *ay = createAxis( "y [mm]", 800 , -100, 700 ); 
  TH2F *hHitmap = create2DHisto( "hHitmap","hitmap", ax, ay );
  TH1F *hHitx = createHisto( "hHitx", "x", ax );
  TH1F *hHity = createHisto( "hHity", "y", ay );

  TFile* fMM = TFile::Open( argv[1], "read");
  TTreeReader MM("events", fMM);
  TTreeReaderValue< std::vector<cluster> > cls( MM, "clusters");

  while( MM.Next() ){

    vector<cluster> clsx;
    std::copy_if (cls->begin(), cls->end(), std::back_inserter(clsx),
        [](const cluster& c){return c.axis=='x';} );
    vector<cluster> clsy;
    std::copy_if (cls->begin(), cls->end(), std::back_inserter(clsy),
        [](const cluster& c){return c.axis=='y';} );

    for( auto cx : clsx )
      for( auto cy : clsy ){
        //auto x = det.pos( cx.stripCentroid, 'x')[0];
        //auto y = det.pos( cy.stripCentroid, 'y')[1];
        auto v = det.pos3DG( cx.stripCentroid,cy.stripCentroid);
        auto x = v[0];
        auto y = v[1];
        hHitmap->Fill( x, y);
      }
  }

  auto c = new TCanvas();
  hHitmap->Draw("colz");
  //hHity->Draw("colz");
  c->SaveAs("map.png");
  return 0;

}


