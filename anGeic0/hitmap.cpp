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

  
  // load the detector geometry 
  // ==========================
  StripTable det( 
      basedir + "../map/geic0_map.txt" , // mapping (in the map folder)
      basedir + "geic0_align.txt"        // alignment (in the anGeic0 folder)
      );


  // prepare histograms 
  // ==================

  // cluster xy map
  axis *aStrx = createAxis( "strip x", 200 , 0, 200 ); 
  axis *aStry = createAxis( "strip y", 600 , 0, 600 ); 
  axis *ax = createAxis( "x [mm]", 800 , -100, 400 ); 
  axis *ay = createAxis( "y [mm]", 900 , -100, 900 ); 
  TH2F *hHitmap    = create2DHisto( "hHitmap","cluster map", ax, ay );
  TH2F *hHitmapStr = create2DHisto( "hHitmapStr","cluster map - strips", aStrx, aStry );

  // hit distributions
  TH1F *hHitx = createHisto( "hHitx", "x", aStrx );
  TH1F *hHity = createHisto( "hHity", "y", aStry );

  TFile* fMM = TFile::Open( argv[1], "read");
  TTreeReader MM("events", fMM);
  TTreeReaderValue< std::vector<cluster> > cls( MM, "clusters");
  TTreeReaderValue< std::vector<hit> > hits( MM, "hits");

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
        hHitmapStr->Fill( cx.centroid, cy.centroid);
      }
  }

  auto c = new TCanvas("c","",1000,900);
  c->Divide(2,1);
  c->cd(1);
  hHitmap->SetStats(0);
  hHitmap->Draw("colz");
  c->cd(2);
  hHitmapStr->SetStats(0);
  hHitmapStr->Draw("colz");
  c->SaveAs("map.png");
  return 0;

}


