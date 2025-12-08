#include "res.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TProfile.h"
#include "TF1.h"

anres::anres( StripTable *d, std::string dn, float by ){
 det = d;
 detname = dn;
 bancoY = by;
 dtol   = 5.;
 sr     = 60.;
 cftst  = 10.;
 nt = 0x0;
}

void anres::init( TTreeReader *MM, TTreeReader *banco ){
  cls  = new TTreeReaderValue< std::vector<cluster> >( *MM, "clusters");
  hits  = new TTreeReaderValue< std::vector<hit> >( *MM, "hits");
  tracks = new TTreeReaderValue< std::vector<banco::track> >( *banco, "tracks");

  ftst = new TTreeReaderValue< unsigned short>(*MM,"ftst");

  std::string oname = "res_" + detname ;
  fout = TFile::Open( (oname+".root").c_str(),"recreate");
  if (!fout ){ std::cerr << "*** ERROR res, issues in opening fout\n"; return; }

  nt = new TNtuple("nt", "nt", "icl:xtr:ytr:xdet:ydet:"\
      "xres:yres:Xclsize:Yclsize:Xmaxamp:Ymaxamp:"\
      "stX:stY:chX:chY:"\
      "Xt:stXt:Yt:stYt:Xtf:stXtf:Ytf:stYtf");
  nt->SetDirectory(fout);

}

void anres::end() {
  if( ! nt ) {
    std::cerr << " **** ERROR anres::end(); nt is null\n";
    return;
  }
  // find the maxima and rms for plotting
  axis *ax_x = createAxis("x", 800, -80, 80);
  axis *ax_y = createAxis("y", 800, -80, 80);

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

  TCanvas *c = new TCanvas("cres", oname.c_str(), 1600,1000);
  c->Divide(2,1);
  c->cd(1);
  hresx->Fit("gaus","R","", hresx->GetMean() - hresx->GetRMS(), hresx->GetMean() + hresx->GetRMS());
  c->cd(2);
  hresy->Fit("gaus","R","", hresy->GetMean() - hresy->GetRMS(), hresy->GetMean() + hresy->GetRMS());
  c->Update();
  c->SaveAs( (oname + ".png").c_str() );

  // some plots about cluster size and time resolution

  axis *axclsize = createAxis( "cluster size", 15, 0, 15);
  TH1F *hXcsize  = createHisto( "hXcsize", "cluster size", axclsize);
  TH1F *hYcsize  = createHisto( "hYcsize", "cluster size", axclsize);
  nt->Project( "hXcsize", "Xclsize", "icl==0" );
  nt->Project( "hYcsize", "Yclsize", "icl==0" );

  axis *axcltime = createAxis( "cluster time", 300, 150, 750);
  TH1F *hXctime  = createHisto( "hXctime", "cluster time", axcltime);
  TH1F *hYctime  = createHisto( "hYctime", "cluster time", axcltime);
  TH1F *hXctime1  = createHisto( "hXctime1", "cluster time 1st", axcltime);
  TH1F *hYctime1  = createHisto( "hYctime1", "cluster time 1st", axcltime);
  nt->Project( "hXctime", "Xt", "icl==0" );
  nt->Project( "hYctime", "Yt", "icl==0" );
  nt->Project( "hXctime1", "Xtf", "icl==0" );
  nt->Project( "hYctime1", "Ytf", "icl==0" );

  TCanvas *ct = new TCanvas("ct","clusters and time", 1600, 1600);
  ct->Divide(2,2);
  ct->cd(1);
  hXcsize->SetLineColor( kBlue );
  hYcsize->SetLineColor( kOrange + 2 );
  hXcsize->Draw();
  hYcsize->Draw("same");
  TLegend *l1 = new TLegend( 0.6,0.6, 0.9,0.9); 
  l1->AddEntry( hXcsize, "X", "l");
  l1->AddEntry( hYcsize, "Y", "l");
  l1->Draw();

  ct->cd(2);
  hXctime->SetLineColor( kBlue );
  hYctime->SetLineColor( kOrange + 2);
  hXctime->Draw();
  hYctime->Draw("same");
  hXctime1->SetLineColor( kViolet );
  hYctime1->SetLineColor( kGreen+3 );
  hXctime1->Draw("same");
  hYctime1->Draw("same");

  TLegend *l2 = new TLegend( 0.6,0.6, 0.9,0.9); 
  l2->AddEntry( hXctime, "X", "l");
  l2->AddEntry( hYctime, "Y", "l");
  l2->AddEntry( hXctime1, "X 1st", "l");
  l2->AddEntry( hYctime1, "Y 1st", "l");
  l2->Draw();

  ct->cd(3);
  TProfile *prx = new TProfile("prx", "xres:xtr", 100, 20, 140, -10., 10.);
  nt->Draw("xres:xtr");
  nt->Project("prx","xres:xtr");
  prx->Fit("pol1","0");
  prx->Draw("same");
  auto f1 = prx->GetFunction("pol1");
  auto p1x = f1->GetParameter(1);

  ct->cd(4);
  TProfile *pry = new TProfile("pry", "yres:ytr", 100, bancoY-20., bancoY+8., -10., 10.);
  nt->Draw("yres:ytr");
  nt->Project("pry","yres:ytr");
  pry->Draw("same");
  //pry->Fit("pol1","same");

  ct->Update();
  ct->SaveAs( ("time_"+oname+".png").c_str() );

  hresx->SetDirectory(fout);
  hresy->SetDirectory(fout);

  fout->Write();
  fout->Close();

  std::cout << p1x << std::endl;
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

    // get the time information of the cluster with max amplitude and the strip with the lowest time of max
    auto Xhits = getHits( clusterX.id ); // select his for the current cluster and sort them by amplitude
    float Xmatofm = stToTime(Xhits[0].timeofmax);
    int XmatofmStr = Xhits[0].strip;
    sortHitsByTime(Xhits);
    float Xfirsttofm = stToTime(Xhits[0].timeofmax) ;
    int XfirsttofmStr = Xhits[0].strip;

    for( auto clusterY : clsY ){ 
      float xGerber = det->pos3D(-1,clusterY.stripCentroid)[0];
      if( abs(xtr - xGerber ) > dtol ) break;// since cls are sorted, we can break

      // get the time information of the cluster with max amplitude and the strip with the lowest time of max
      auto Yhits = getHits( clusterY.id ); // select his for the current cluster and sort them by amplitude
    float Ymatofm = stToTime(Yhits[0].timeofmax);
    int YmatofmStr = Yhits[0].strip;
    sortHitsByTime(Yhits);
    float Yfirsttofm = stToTime(Yhits[0].timeofmax) ;
    int YfirsttofmStr = Yhits[0].strip;

      // get the position of the xy cluster in the lab ref
      std::vector<double> detPos = det->pos3D(clusterX.stripCentroid, clusterY.stripCentroid);
      float xdet = detPos[0];
      float ydet = detPos[1];

      // prepare to fill the ntuple
      float data[23] = { 
        (float) icl, 
        xtr, ytr, xdet, ydet, 
        xtr-xdet, ytr-ydet, 
        (float)clusterX.size, (float)clusterY.size, 
        (float)clusterX.ampsum, (float)clusterX.ampsum , 
        clusterX.stripCentroid, clusterY.stripCentroid, 
        clusterX.centroid, clusterY.centroid,
        Xmatofm, (float)XmatofmStr,
        Ymatofm, (float)YmatofmStr,
        Xfirsttofm, (float)XfirsttofmStr,
        Yfirsttofm, (float)YfirsttofmStr
      };
      
      nt->Fill( data );
    }
  }

  return true;
}


std::vector<hit> anres::getHits(int clId){
  std::vector<hit> h;
  std::copy_if ((*hits)->begin(), (*hits)->end(), std::back_inserter(h),
      [clId](const hit& h){return h.clusterId==clId;} );
  std::sort (h.begin(), h.end(),
      [](const hit& a, const hit& b) {return a.maxamp > b.maxamp;});
  return h;
}

void anres::sortHitsByTime( std::vector<hit> &h ){

  std::sort( h.begin(), h.end(),
      [](const hit& a, const hit& b) {
        return a.timeofmax < b.timeofmax;
      }
      );
}

float anres::stToTime( float t ){
    return t * sr + cftst * (**ftst);
}
