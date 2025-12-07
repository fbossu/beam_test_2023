#include "reco/definitions.h"
#include "map/StripTable.h"
#include "banco/definition_banco.h"
#include <string>

#include "TFile.h"
#include "TChain.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TCanvas.h"
#include "TTreeReader.h"
#include "TStyle.h"
#include "TLegend.h"
#include <map>
#include <iostream>

#include "hits.h"

anhits::anhits( StripTable *d, std::string dn, float by ){
 det = d;
 detname = dn;
 bancoY = by;
}

void anhits::init(  TTreeReader *MM, TTreeReader *banco){

  cls  = new TTreeReaderValue< std::vector<cluster> >( *MM, "clusters");
  //anhits = new TTreeReaderValue< std::vector<hit> >( *MM, "hits");
  tracks = new TTreeReaderValue< std::vector<banco::track> >( *banco, "tracks");

  axis *axch = createAxis( "channel", 256, 0, 128);
  out_m["hClsX"] = createHisto( "hChX", "Clusters X", axch );
  out_m["hClsY"] = createHisto( "hChY", "Clusters Y", axch );
  out_m["hClsXwT"] = createHisto( "hChXwT", "Clusters X with Tracks", axch );
  out_m["hClsYwT"] = createHisto( "hChYwT", "Clusters Y with Tracks", axch );
  
  axis *axnCls    = createAxis("# clusters", 40, 0, 40);
  out_m["hNClsX"]  = createHisto("hNClsX", "Number of clusters X", axnCls );
  out_m["hNClsXwT"]= createHisto("hNClsXwT", "Number of clusters X with Tracks", axnCls );
  out_m["hNClsY"]   = createHisto("hNClsY","Number of clusters Y", axnCls );
  out_m["hNClsYwT"] = createHisto("hNClsYwT", "Number of clusters Y with Tracks",axnCls );

  axis *axclssize = createAxis("cluster size", 20,0,20);
  out_m["hClsSizeX"]   = createHisto("hClsSizeX",   axclssize );
  out_m["hClsSizeY"]   = createHisto("hClsSizeY",   axclssize );
  out_m["hClsSizeXwT"]   = createHisto("hClsSizeXwT",   axclssize );
  out_m["hClsSizeYwT"]   = createHisto("hClsSizeYwT",   axclssize );
  out_m["hClsSizeXclosestT"] = createHisto("hClsSizeXclosestT", axclssize );
  out_m["hClsSizeYclosestT"] = createHisto("hClsSizeYclosestT", axclssize );
 
  // correlation cls size
  out_m["hClsSizeXvshClsSizeY"] = create2DHisto("hClsSizeXvshClsSizeY", "cls size X vs Y", axclssize, axclssize);
  out_m["hClsSizeXvshClsSizeYwT"] = create2DHisto("hClsSizeXvshClsSizeYwT", "cls size X vs Y", axclssize, axclssize);

  // hit maps
  axis *ax_x = createAxis("x", 500, -30, 160);
  axis *ax_y = createAxis("y", 500, -30, 160);
  out_m["hHitMap"] = create3DHisto("hHitMap", "hHitMap",
                            ax_x, ax_y, axclssize );

  out_m["hHitMapMax"] = create2DHisto("hHitMapMax", "HitMap MaxCluster", ax_x, ax_y);

  //// axis *axamp    = createAxis("amplitude", 1000,0,1e4);
  //// TH2F *h2clsAmp = 
  //TH2F *h2NclVsClsSizeX = create2DHisto("h2NclVsClsSizeX", "cls vs size X",axnCls, axclssize);
  //TH2F *h2NclVsClsSizeY = create2DHisto("h2NclVsClsSizeY", "cls vs size Y",axnCls, axclssize);
}

void anhits::end(){
  
  std::string oname = "hitmap_" + detname ;
  TCanvas *chits = new TCanvas("hm",oname.c_str(), 1000,1000 );
  out_m["hHitMapMax"]->Draw("colz");
  chits->SaveAs( (oname + ".png").c_str()  );

  TFile *fout = TFile::Open( (oname+".root").c_str(),"recreate");
  for( auto h : out_m ){
    ((TH1F*)h.second)->SetDirectory(fout);
  }
  fout->Write();
  fout->Close();
}

bool anhits::run() {
  // separate the clusts in X and Y
  std::vector<cluster> clsX;
  std::copy_if ((*cls)->begin(), (*cls)->end(), std::back_inserter(clsX),
            [](const cluster& c){return c.axis=='x';} );
  std::vector<cluster> clsY;
  std::copy_if ((*cls)->begin(), (*cls)->end(), std::back_inserter(clsY),
            [](const cluster& c){return c.axis=='y';} );
  // sort cluster by size
  std::sort( clsX.begin(), clsX.end(), 
      []( const cluster& a, const cluster& b){ 
        return a.size > b.size; 
      });

  std::sort( clsY.begin(), clsY.end(), 
      []( const cluster& a, const cluster& b){ 
        return a.size > b.size; 
        });
  // fill some histograms
  // --------------------

  // number of clusters
  out_m["hNClsX"]->Fill( clsX.size() );
  out_m["hNClsY"]->Fill( clsY.size() );

  // cluster size and cluster centroid position
  for( auto c : clsX ){
    out_m["hClsX"]->Fill( c.stripCentroid );
    out_m["hClsSizeX"]->Fill( c.size );
  }
  for( auto c : clsY ){
    out_m["hClsY"]->Fill( c.stripCentroid );
    out_m["hClsSizeY"]->Fill( c.size );
  }

  // hit map for max size clusters
  TH2* hhmmc = ((TH2*)out_m["hHitMapMax"]);
  if( clsX.size() > 0 && clsY.size() > 0){
    auto p = det->pos3D( clsX[0].stripCentroid, clsY[0].stripCentroid );
    hhmmc->Fill(p[0],p[1]);
  }

  TH3* hhm = ((TH3*)out_m["hHitMap"]);
  TH2* hsxsy = ((TH2*)out_m["hClsSizeXvshClsSizeY"]);
  for( auto cx : clsX )
    for( auto cy : clsY ){
      auto v = det->pos3D( cx.stripCentroid, cy.stripCentroid );
      hhm->Fill( v[0], v[1], std::min( cx.size, cy.size ));
      hsxsy->Fill( cx.size, cy.size);
    }
  // now look what happens in events where there is a banco track
  if( (*tracks)->size() == 0) return true;
  
  out_m["hNClsXwT"]->Fill( clsX.size() );
  out_m["hNClsYwT"]->Fill( clsY.size() );
  for( auto c : clsX ){
    out_m["hClsXwT"]->Fill( c.stripCentroid );
    out_m["hClsSizeXwT"]->Fill( c.size );
  }
  for( auto c : clsY ){
    out_m["hClsYwT"]->Fill( c.stripCentroid );
    out_m["hClsSizeYwT"]->Fill( c.size );
  }
 
  // if no clusters continue
  if ( clsY.size() == 0 || clsX.size() == 0 ) return false;

  //// find the clusters that are closest to the track
  banco::track tr = (**tracks)[0];
  double xtr = tr.x0 + tr.mx*det->getZpos();
  double ytr = tr.y0 + tr.my*det->getZpos() + bancoY;
  
  // sort cluster from the closest
  std::sort( clsX.begin(), clsX.end(), 
      [ytr,this]( const cluster& a, const cluster& b){ 
        return det->pos3D(a.stripCentroid,-1)[1]-ytr < det->pos3D(b.stripCentroid,-1)[1]-ytr; 
      });

  std::sort( clsY.begin(), clsY.end(), 
      [xtr,this]( const cluster& a, const cluster& b){ 
        return det->pos3D(-1,a.stripCentroid)[0]-xtr < det->pos3D(-1,b.stripCentroid)[0]-xtr; 
        });
  out_m["hClsSizeXclosestT"]->Fill( clsX[0].size );
  out_m["hClsSizeYclosestT"]->Fill( clsY[0].size );

  // do the combinatorics, but cut on the distance to the track
  TH2* hsxsywT = ((TH2*)out_m["hClsSizeXvshClsSizeYwT"]);
  for( auto clusterX : clsX ){

    double yGerber = det->pos3D(clusterX.stripCentroid,-1)[1];
    if( abs(tr.y0 + det->getZpos()*tr.my + bancoY - yGerber ) > 5. ) break;

    for( auto clusterY : clsY ){ 
      double xGerber = det->pos3D(-1,clusterY.stripCentroid)[0];
      if( abs(tr.x0 + det->getZpos()*tr.mx - xGerber ) > 5. ) break;

      hsxsywT->Fill( clusterX.size, clusterY.size );
    }
  }
  return true;
}

