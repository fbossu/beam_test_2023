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

void Residuals( banco::track trk, XYZVector p, TH1F *hres, char ax = 'x' ){
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
  hres->Fill( r );
}

// global settings
std::string basedir = "";
int NEVENTS = -1;
bool DoRES = false;
float MaxR = 1.;

// =================================================================

void recoBanco(std::vector<std::string> fnamesIn){
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
  TFile *fout =TFile::Open( "fout.root", "recreate" );
  // tree
  TTree *nt = new TTree("events","");
  uint64_t trEvId = 0;
  std::vector<banco::track> *tracks = new std::vector<banco::track>();
  nt->Branch("tracks", &tracks);
  nt->Branch("eventId", &trEvId);

  // histograms
  auto hdir = fout->mkdir("histos");

  axis *acy = createAxis( "centroid y (mm)", 200, 0, 15. ); 
  axis *acx = createAxis( "centroid x (mm)", 200, 0, 15. );

  axis *arescx = createAxis( "residual x (mm)", 500, -MaxR, MaxR ); 
  axis *arescy = createAxis( "residual y (mm)", 500, -MaxR, MaxR ); 

  std::map<std::string, TH2F*> mh2xy;
  std::map<std::string, TH1F*> mhresx;
  std::map<std::string, TH1F*> mhresy;
  std::map<std::string, TH1F*> mhUresx;
  std::map<std::string, TH1F*> mhUresy;
  for( auto s : tnames ){
    mh2xy[s] = create2DHisto( Form("h2xy_%s",s.c_str()), Form("xy %s",s.c_str()), acx, acy );
    mh2xy[s]->SetDirectory(hdir);
    mhresx[s] = createHisto( Form("hresx_%s",s.c_str()), Form(" biased res x %s",s.c_str()), arescx );
    mhresx[s]->SetDirectory(hdir);
    mhresy[s] = createHisto( Form("hresy_%s",s.c_str()), Form("biased res y %s",s.c_str()), arescy );
    mhresy[s]->SetDirectory(hdir);
    mhUresx[s] = createHisto( Form("hUresx_%s",s.c_str()), Form("unbiased res x %s",s.c_str()), arescx );
    mhUresx[s]->SetDirectory(hdir);
    mhUresy[s] = createHisto( Form("hUresy_%s",s.c_str()), Form("unbiased res y %s",s.c_str()), arescy );
    mhUresy[s]->SetDirectory(hdir);
  }
  std::vector<std::pair<std::string,std::string>> hcornames;
  hcornames.push_back( {"ladder162","ladder157"} );
  hcornames.push_back( {"ladder162","ladder163"} );
  hcornames.push_back( {"ladder162","ladder160"} );
  hcornames.push_back( {"ladder157","ladder163"} );
  hcornames.push_back( {"ladder157","ladder160"} );
  hcornames.push_back( {"ladder163","ladder160"} );
  std::map<std::pair<std::string,std::string>, TH2F *> mh2corx;
  std::map<std::pair<std::string,std::string>, TH2F *> mh2cory;
  for( auto a : hcornames ){
    auto n = "h" + a.first + a.second;
    TH2F *hcorx12 = create2DHisto( (n+"x").c_str(), n.c_str(), acx, acx );
    hcorx12->SetDirectory(hdir);
    mh2corx[a] = hcorx12;
    TH2F *hcory12 = create2DHisto( (n+"y").c_str(), n.c_str(), acy, acy );
    hcory12->SetDirectory(hdir);
    mh2cory[a] = hcory12;

  }

  // loop over events
  // ================
  int i=0;
  int nentries = reader.GetEntries();
  //while( reader.Next() && i<5e4 ){
  while( reader.Next() ){

    if( NEVENTS > 0 && i > NEVENTS ) break;
    i++;
    trEvId = *eventId + 1;
 
    if( i%1000 == 0 ){
      std::cout << " [ ";
      for( int j=0; j < (float)i/nentries * 50; j++ )
        std::cout << "-";
      for( float j = (float)i/nentries * 50; j < 50 ; j++ )
        std::cout << " ";
      std::cout<<" ] - " << std::setw(8) << i << "/" << nentries << "\r";
      std::cout << std::flush;

    }

    //fill some histos
    for( auto s : tnames ){
      for( auto cl : *(*cls[s]) ) { 
        XYZVector c0;
        geom[s].CentroidToLocal( cl, &c0 );
        geom[s].LocalToGlobal(  &c0 );
        mh2xy[s]->Fill( c0.X(), c0.Y() );
      }
    }
    for( auto a : hcornames ){
      auto d1 = a.first;
      auto d2 = a.second;
      for( auto cl1 : *(*cls[d1]) ){
        XYZVector c1;
        geom[d1].CentroidToLocal( cl1, &c1 );
        geom[d1].LocalToGlobal(  &c1 );

        for( auto cl2 : *(*cls[d2]) ){ 
          XYZVector c2;
          geom[d2].CentroidToLocal( cl2, &c2 );
          geom[d2].LocalToGlobal(  &c2 );

          mh2corx[a]->Fill( c1.X(), c2.X() );
          mh2cory[a]->Fill( c1.Y(), c2.Y() );
          
        }
      }
    }

    // clean events: no more than 1 cluster per ladder
    bool good = true;
    for( auto s : tnames ){
      if( (*cls[s])->size() != 1 ) good = false;
    }
    if( ! good ){
      nt->Fill();
      tracks->clear();
      continue; 
    }

    // find combinations of clusters and consider them as a seed // TODO, allow for combinatorics
    //std::vector< std::vector<XYZVector> > lseeds;
    std::vector<XYZVector> seed;
    std::vector<std::string> seeddet;

    // loop over the ladders
    for( auto icl : cls ){
      XYZVector p;
      geom[icl.first].CentroidToLocal( (*icl.second)->at(0), &p ); // TODO check if there are any cls
      geom[icl.first].LocalToGlobal(&p);
      seed.push_back(p);
      seeddet.push_back(icl.first);// maybe not useful if detector are ordered
    }

    auto track = Fit( seed );
    tracks->push_back(track);

    for( auto trk : *tracks ){
        for( int i=0;i<seed.size(); i++){
          Residuals( trk, seed[i], mhresx[seeddet.at(i)], 'x');
          Residuals( trk, seed[i], mhresy[seeddet.at(i)], 'y');
        }
    }

    if( DoRES ){
      // unbiased residuals
      for( int i=0;i<seed.size(); i++){
        auto trk = Fit( seed, i );
        Residuals( trk, seed[i], mhUresx[seeddet.at(i)], 'x');
        Residuals( trk, seed[i], mhUresy[seeddet.at(i)], 'y');
      }
    }


    nt->Fill();
    tracks->clear();
  }

  fout->Write();
  fout->Close();
}


int main(int argc, char *argv[])
{
  basedir = argv[0];
  basedir = basedir.substr(0, basedir.size()-10);
  std::cout << " basedir " <<  basedir << std::endl;

  // reading some options
  int opt;
  while((opt = getopt(argc, argv, "rn:m:")) != -1) { 
    switch(opt) { 
      case 'r':
        DoRES = true;
        std::cout << "RES ON " << DoRES << std::endl;
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
  recoBanco(fnames);
  std::cout<<std::endl;
	return 0;
}
