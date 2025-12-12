#include "eff.h"

aneff::aneff( StripTable *d, std::string dn, float by ){
 det = d;
 detname = dn;
 bancoY = by;
 dtol = 2.5;
}

void aneff::init( TTreeReader *MM, TTreeReader *banco ){
  cls  = new TTreeReaderValue< std::vector<cluster> >( *MM, "clusters");
  tracks = new TTreeReaderValue< std::vector<banco::track> >( *banco, "tracks");

  std::string oname = "eff_" + detname ;
  fout = TFile::Open( (oname+".root").c_str(),"recreate");

  axis *ax_x = createAxis("x", 300, -30, 160);
  axis *ax_y = createAxis("y", 300, -30, 160);

  out_m["hDenXY"] = create2DHisto( "hDenXY", "XY Den", ax_x, ax_y);
  out_m["hNumX"]  = create2DHisto( "hNumX",  "X Num",  ax_x, ax_y);
  out_m["hNumY"]  = create2DHisto( "hNumY",  "Y Num",  ax_x, ax_y);
  out_m["hNumXY"] = create2DHisto( "hNumXY", "XY Num", ax_x, ax_y);

  for( auto h : out_m ){
    ((TH1F*)h.second)->SetDirectory(fout);
  }
}

void aneff::end() {

  float Den = out_m["hDenXY"]->GetEntries();
  float NumX = out_m["hNumX"]->GetEntries();
  float NumY = out_m["hNumY"]->GetEntries();
  float NumXY = out_m["hNumXY"]->GetEntries();

  if( Den > 0 ){
    std::cout << "Eff X: " << std::setw(7) << NumX/Den << std::endl;
    std::cout << "Eff Y: " << std::setw(7) << NumY/Den << std::endl;
    std::cout << "Eff XY:" << std::setw(7) << NumXY/Den << std::endl;
  }
  else {
    std::cout << " **** WARNING Eff *** No counts\n";
  }

  fout->Write();
  fout->Close();
}

bool aneff::run(){

  if( (*tracks)->size() == 0 ) return false;

  // get the best track
  auto tr = *std::min_element((*tracks)->begin(), (*tracks)->end(),
                  [](const banco::track& a,const banco::track& b) { return a.chi2x+a.chi2y < b.chi2x+b.chi2y; });
  if(tr.chi2x > tchi2 or tr.chi2y > tchi2) return false;

  // at this point we have a good track

  double xtr = tr.x0 + tr.mx*det->getZpos();
  double ytr = tr.y0 + tr.my*det->getZpos() + bancoY; // taking in consideration the shift of banco

  // separate and sort cluster from the closest
  // separate the clusts in X and Y
  std::vector<cluster> clsX;
  std::copy_if ((*cls)->begin(), (*cls)->end(), std::back_inserter(clsX),
            [](const cluster& c){return c.axis=='x';} );
  std::vector<cluster> clsY;
  std::copy_if ((*cls)->begin(), (*cls)->end(), std::back_inserter(clsY),
            [](const cluster& c){return c.axis=='y';} );

  std::sort( clsX.begin(), clsX.end(), 
      [ytr,this]( const cluster& a, const cluster& b){ 
        return abs(det->pos3D(a.stripCentroid,-1)[1]-ytr) <= abs(det->pos3D(b.stripCentroid,-1)[1]-ytr); 
      });

  std::sort( clsY.begin(), clsY.end(), 
      [xtr,this]( const cluster& a, const cluster& b){ 
        return abs(det->pos3D(-1,a.stripCentroid)[0]-xtr) <= abs(det->pos3D(-1,b.stripCentroid)[0]-xtr); 
        });


  // fill the histograms
  
  // denominator
  out_m["hDenXY"]->Fill( xtr, ytr );

  // numerators
  bool xfound=false;
  bool yfound=false;
  // coordinate y <-> stripsX
  //cls are ordered, so I look at the first one
  if( clsX.size() > 0 and abs(det->pos3D(clsX[0].stripCentroid,-1)[1]-ytr) < dtol ){
    out_m["hNumX"]->Fill( xtr, ytr );
    xfound=true;
  }
  if( clsY.size() > 0 and  abs(det->pos3D(-1,clsY[0].stripCentroid)[0]-xtr) < dtol ){
    out_m["hNumY"]->Fill( xtr, ytr );
    yfound=true;
  }
  if( xfound and yfound ){
    out_m["hNumXY"]->Fill( xtr, ytr );
  }

  return true;
}





