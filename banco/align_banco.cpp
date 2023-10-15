// beam test 2023
// alignement of BANCO
// author: F. Bossù

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TTreeReader.h"
#include "definition_banco.h"

#include "TGraphErrors.h"
#include "TFitResult.h"

#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <iomanip>

#include "Math/Rotation3D.h"
#include "Math/Vector3D.h"
#include "Math/Functor.h"
#include "Fit/Fitter.h"
#include <getopt.h>
#include <cstdlib>

#include "utils.h"

#include "ladder.h"

using namespace ROOT::Math;

// =================================================================

banco::track Fit( std::vector<XYZVector> seed, int ignore=-1 ){

  TGraphErrors grx;
  TGraphErrors gry;
  for( int i=0; i<seed.size(); i++ ){
    if( i==ignore) continue;
    auto p = seed[i];
    grx.SetPoint(i,p.Z(),p.X());
    gry.SetPoint(i,p.Z(),p.Y());
    grx.SetPointError(i,0,0.028/sqrt(12));
    gry.SetPointError(i,0,0.028/sqrt(12));
  }

  auto ptrx = grx.Fit("pol1","Q0S");
  auto ptry = gry.Fit("pol1","Q0S");

  banco::track tr;
  tr.x0 = ptrx->Parameter(0);
  tr.mx = ptrx->Parameter(1);
  tr.ex0 = ptrx->Error(0);
  tr.emx = ptrx->Error(1);
  tr.chi2x = ptrx->Chi2()/ptrx->Ndf();
  tr.y0 = ptry->Parameter(0);
  tr.my = ptry->Parameter(1);
  tr.ey0 = ptry->Error(0);
  tr.emy = ptry->Error(1);
  tr.chi2y = ptry->Chi2()/ptry->Ndf();

  return tr;
}

void Residuals( banco::track trk, XYZVector p, TH1 *hres, char ax = 'x' ){
  float z = p.Z();
  float c = 0.;
  float m = 0.;
  float r = 0.;
  if( ax == 'x' ){
    c = trk.x0;
    m = trk.mx;
    r = ( c + m*z ) - p.X();
  }
  else {
    c = trk.y0;
    m = trk.my;
    r = ( c + m*z ) - p.Y();
  }
  if( strcmp( hres->ClassName() , "TH2F" ) == 0 )
    hres->Fill( (c+m*z) , r );
  else
    hres->Fill( r );
}


// global settings
std::string basedir = "";
int NEVENTS = -1;
float MaxR = 1.;

// =================================================================

void alignBanco(std::vector<std::string> fnamesIn){
  // chain the N ladders together
  TTree *tree = 0x0;

  std::vector<std::string> tnames;

  for( auto f : fnamesIn ){
	  TFile* fin = TFile::Open(f.c_str(),"read");
    TTree *tin = fin->Get<TTree>("events");
    auto lid = f.substr( f.find( "ladder"), 6 + 3 );
    std::cout << lid << "\n";
   
    tnames.push_back( lid );

    if( ! tree ) {
      tree = tin;
      tree->SetName( lid.c_str() );
    }
    else 
      tree->AddFriend( tin, lid.c_str() );
    
  }
	TTreeReader reader( tree );

  TTreeReaderValue<uint64_t> eventId( reader, "eventId");

  // link the cluster branches for the different ladders
  // use a map for accessing them by name "ladder#"
  std::map< std::string, TTreeReaderValue<std::vector<banco::cluster>>* > cls;
  for( auto s : tnames ){
    TTreeReaderValue<std::vector<banco::cluster>> *r = new TTreeReaderValue<std::vector<banco::cluster>>(reader, (s+".clusters").c_str() );
    cls[s] = r;
  }

  // geometry
  // --------
  std::map< std::string, banco::Ladder > geom;
  for( auto s : tnames ){
    std::cout << s << std::endl;
    geom[s];
    geom[s].LoadGeometry(s,Form("%s/geometries.txt",basedir.c_str())); // TODO relative paths

    geom[s].PrintGeometry();
  }


  // some outputs
  // -------------
  //TFile *fout =TFile::Open( "fout.root", "recreate" );

  std::vector< std::vector<XYZVector> > evcls;
  std::vector< banco::track > tracks;
  
  // loop over events
  // ================
  int i=0;
  int nentries = reader.GetEntries();
  //while( reader.Next() && i<5e4 ){
  while( reader.Next() ){

    if( NEVENTS > 0 && i > NEVENTS ) break;
    i++;

    // clean events: no more than 1 cluster per ladder
    bool good = true;
    for( auto s : tnames ){
      if( (*cls[s])->size() != 1 ) good = false;
    }
    if( ! good ){
      continue; 
    }

    // find combinations of clusters and consider them as a seed // TODO, allow for combinatorics
    //std::vector< std::vector<XYZVector> > lseeds;
    std::vector<XYZVector> seed;

    // loop over the ladders
    for( auto icl : cls ){
      XYZVector p;
      geom[icl.first].CentroidToLocal( (*icl.second)->at(0), &p ); // TODO check if there are any cls
      geom[icl.first].LocalToGlobal(&p);
      seed.push_back(p);
    }
    if( seed.size() != tnames.size() ) std::cout << "Warning\n";
    evcls.push_back(seed);
    
    auto trk = Fit( seed );
    tracks.push_back(trk);


  }

  std::cout << "\n ===  align === \n";


  // ================ align

  auto chi2f = [evcls,tracks]( const double *pp ){

    double d2=0;

    for( int j=0; j<evcls.size(); j++ ){
      auto seed = evcls.at(j);
      //auto trk  = tracks.at(j);

      for( int i=0; i<seed.size(); i++){
        auto mp = seed.at(i);
        auto trk = Fit( seed, i );

        double mx = mp.X();
        double my = mp.Y();
        double mz = mp.Z();

        double hx = trk.x0 + trk.mx * mz;
        double hy = trk.y0 + trk.my * mz;

        double ehx = trk.ex0*trk.ex0+ trk.emx*trk.mx * trk.emx*trk.mx  ;
        double ehy = trk.ey0*trk.ey0+ trk.emy*trk.my * trk.emy*trk.my  ;

        double dx = hx - (mx + pp[2*i]);
        double dy = hy - (my + pp[2*i+1]);

        double ex = 0.028*0.028/12 + ehy ;
        double ey = 0.028*0.028/12 + ehy ;
        //d2 += dx*dx/ex + dy*dy/ey;
        d2 += dx*dx + dy*dy;
        //d2 += dx*dx ;
      }
    }
    return d2;
  };

  ROOT::Math::Functor fcn(chi2f,8);
  ROOT::Fit::Fitter  fitter;

  std::map<std::string, int> posname;
  for( int i=0;i<tnames.size();i++) posname[tnames[i]] = i;

  //double pStart[8] = {0.03,1.5,0.01,-0.7,0.1,0.1,-0.01,0.8};
  double pStart[8] = {0.};
  fitter.SetFCN(fcn, pStart);

  fitter.Config().ParSettings( 2*posname["ladder162"] ).Fix();
  fitter.Config().ParSettings( 2*posname["ladder162"]+1 ).Fix();
  fitter.Config().ParSettings( 2*posname["ladder162"] ).SetValue(0);
  fitter.Config().ParSettings( 2*posname["ladder162"]+1 ).SetValue(0);

  //fitter.Config().ParSettings( 2*posname["ladder157"] ).SetValue(0.05);
  //fitter.Config().ParSettings( 2*posname["ladder157"]+1 ).SetValue(1.5);

  //fitter.Config().ParSettings( 2*posname["ladder163"] ).SetValue(-0.04);
  //fitter.Config().ParSettings( 2*posname["ladder163"]+1 ).SetValue(-0.8);

  //fitter.Config().ParSettings( 2*posname["ladder160"] ).SetValue(0.04);
  //fitter.Config().ParSettings( 2*posname["ladder160"]+1 ).SetValue(0.8);

  for( int i=0; i<tnames.size(); i++){
    fitter.Config().ParSettings(2*i).SetName(Form("x%d_%s",i,tnames[i].c_str()));
    fitter.Config().ParSettings(2*i+1).SetName(Form("y%d_%s",i,tnames[i].c_str()));
    fitter.Config().ParSettings(2*i).SetStepSize(0.01);
    fitter.Config().ParSettings(2*i+1).SetStepSize(0.01);
    //fitter.Config().ParSettings(2*i+1).Fix();
  }

 bool ok = fitter.FitFCN();
 if (!ok) {
    Error("align_banco","fit failed");
 }

  const ROOT::Fit::FitResult & result = fitter.Result();
  result.Print(std::cout);

  std::cout << "\n =========== \n";
  for( int i=0; i<tnames.size(); i++){
    std::cout << tnames[i] << std::setw(15) <<
     fitter.Config().ParSettings(2*i).Name() << std::setw(15) << 
     fitter.Config().ParSettings(2*i).Value() - geom[tnames[i]].GetTranslation().X() << std::setw(15) << 
     fitter.Config().ParSettings(2*i+1).Value() - geom[tnames[i]].GetTranslation().Y() << std::endl;
  }
  


}


int main(int argc, char *argv[])
{
  basedir = argv[0];
  basedir = basedir.substr(0, basedir.size()-11);
  std::cout << " basedir " <<  basedir << std::endl;

  // reading some options
  int opt;
  while((opt = getopt(argc, argv, "n:m:g:")) != -1) { 
    switch(opt) { 
      case 'g':
        basedir = optarg;
        std::cout << " basedir " <<  basedir << std::endl;
        break;
      case 'm':
        MaxR=std::atof(optarg);
        std::cout << "limits res hists "<< MaxR << std::endl;
        break;
      case 'n':
        NEVENTS=std::atoi(optarg);
        std::cout << "N Events "<< NEVENTS << std::endl;
        break;
      default:
      break;
    }
  }
  

  std::vector<std::string> fnames;
  for( int i=1; i<argc; i++){
    std::string fnameIn = argv[i];
    if( fnameIn.find( ".root" ) != std::string::npos ){
      std::cout << fnameIn << std::endl;
       fnames.push_back( fnameIn );
	  }
  }
  if( fnames.size() < 3 ) {
    std::cout << "not enough ladder files\n";
    return -1;
  }
  
  alignBanco(fnames);

  std::cout << std::setw(100) << " " <<std::endl;
	return 0;
}
