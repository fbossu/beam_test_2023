#include "res.h"
#include "TCanvas.h"

anres::anres( StripTable *d, std::string dn, float by ){
 det = d;
 detname = dn;
 bancoY = by;
 dtol = 5.;
 nt = 0x0;
}

void anres::init( TTreeReader *MM, TTreeReader *banco ){
  cls  = new TTreeReaderValue< std::vector<cluster> >( *MM, "clusters");
  tracks = new TTreeReaderValue< std::vector<banco::track> >( *banco, "tracks");

  nt = new TNtuple("nt", "nt", "icl:xtr:ytr:xdet:ydet:xres:yres:Xclsize:Yclsize:Xmaxamp:Ymaxamp:stX:stY:stresX:stresY:chX:chY");

}

void anres::end() {
  if( ! nt ) {
    std::cerr << " **** ERROR anres::end(); nt is null\n";
    return;
  }
  // find the maxima and rms for plotting
  axis *ax_x = createAxis("x", 500, -50, 50);
  axis *ax_y = createAxis("y", 500, -50, 50);

  TH1F *htmpx = createHisto( "htmpx","htmpx", ax_x);
  TH1F *htmpy = createHisto( "htmpy","htmpy", ax_y);
  nt->Project( "htmpx", "xres" );
  nt->Project( "htmpy", "yres" );

  float meanresx = htmpx->GetBinCenter(htmpx->GetMaximumBin());
  float meanresy = htmpy->GetBinCenter(htmpy->GetMaximumBin());
  float rmsresx  = htmpx->GetRMS();
  float rmsresy  = htmpy->GetRMS();

  ax_x->binl = meanresx - 3.*rmsresx;
  ax_x->binh = meanresx + 3.*rmsresx;

  ax_y->binl = meanresy - 3.*rmsresy;
  ax_y->binh = meanresy + 3.*rmsresy;

  TH1F *hresx = createHisto( "hresx","hresx", ax_x);
  TH1F *hresy = createHisto( "hresy","hresy", ax_y);
  nt->Project( "hresx", "xres" );
  nt->Project( "hresy", "yres" );

  std::string oname = "res_" + detname ;

  TCanvas *c = new TCanvas("cres", oname.c_str(), 1000,600);
  c->Divide(2,1);
  c->cd(1);
  hresx->Fit("gaus");
  c->cd(2);
  hresy->Fit("gaus");
  c->SaveAs( (oname + ".png").c_str() );

  TFile *fout = TFile::Open( (oname+".root").c_str(),"recreate");

  nt->SetDirectory(fout);
  hresx->SetDirectory(fout);
  hresy->SetDirectory(fout);

  fout->Write();
  fout->Close();
}

bool anres::run(){
  if( (*tracks)->size() == 0 ) return false;

  // get the best track
  auto tr = *std::min_element((*tracks)->begin(), (*tracks)->end(),
                  [](const banco::track& a,const banco::track& b) { return a.chi2x+a.chi2y < b.chi2x+b.chi2y; });
  if(tr.chi2x>4 or tr.chi2y>20) return false;

  // at this point we have a good track

  float xtr = tr.x0 + tr.mx*det->getZpos();
  float ytr = tr.y0 + tr.my*det->getZpos() + bancoY; // taking in consideration the shift of banco

  // separate and sort cluster from the closest
  // separate the clusts in X and Y
  std::vector<cluster> clsX;
  std::copy_if ((*cls)->begin(), (*cls)->end(), std::back_inserter(clsX),
            [](const cluster& c){return c.axis=='x';} );
  std::vector<cluster> clsY;
  std::copy_if ((*cls)->begin(), (*cls)->end(), std::back_inserter(clsY),
            [](const cluster& c){return c.axis=='y';} );
  // sort clusters from the clostest to the furthest from the track
  std::sort( clsX.begin(), clsX.end(), 
      [ytr,this]( const cluster& a, const cluster& b){ 
        return abs(det->pos3D(a.stripCentroid,-1)[1]-ytr) <= abs(det->pos3D(b.stripCentroid,-1)[1]-ytr); 
      });

  std::sort( clsY.begin(), clsY.end(), 
      [xtr,this]( const cluster& a, const cluster& b){ 
        return abs(det->pos3D(-1,a.stripCentroid)[0]-xtr) <= abs(det->pos3D(-1,b.stripCentroid)[0]-xtr); 
        });


  // now, compute the resolution for all the clusters 
  // do the combinatorics, but cut on the distance to the track

  int icl=0; // this counts the "accepted" xy clusters in this event: remember, they are sorted, so the 0th is the closest to the track
  for( auto clusterX : clsX ){

    float yGerber = det->pos3D(clusterX.stripCentroid,-1)[1];
    if( abs( ytr - yGerber ) > dtol ) break; // since cls are sorted, we can break

    for( auto clusterY : clsY ){ 
      float xGerber = det->pos3D(-1,clusterY.stripCentroid)[0];
      if( abs(xtr - xGerber ) > dtol ) break;// since cls are sorted, we can break

      // get the position of the xy cluster in the lab ref
      std::vector<double> detPos = det->pos3D(clusterX.stripCentroid, clusterY.stripCentroid);
      float xdet = detPos[0];
      float ydet = detPos[1];

      // prepare to fill the ntuple
      float data[17] = { 
        (float) icl, 
        xtr, ytr, xdet, ydet, 
        xtr-xdet, ytr-ydet, 
        (float)clusterX.size, (float)clusterY.size, 
        (float)clusterX.ampsum, (float)clusterX.ampsum , 
        clusterX.stripCentroid, clusterY.stripCentroid, 
        ytr-clusterX.stripCentroid, xtr-clusterY.stripCentroid, 
        clusterX.centroid, clusterY.centroid
      };
      
      nt->Fill( data );
    }
  }

  return true;
}


