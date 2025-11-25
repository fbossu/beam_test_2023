// beam test 2023
// reconstruction and tracking of BANCO
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
    grx.SetPointError(i,0,3*0.028/sqrt(12));
    gry.SetPointError(i,0,3*0.028/sqrt(12));
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

void RotateTrack( banco::track &trk, float ax, float ay ){

  XYZVectorF x0 = { trk.x0, trk.y0, 0};
  XYZVectorF m  = { trk.mx, trk.my, 1};

  Rotation3D XZ; // a rotation around the y axis
  XZ.SetComponents(
        cos(ax),  0, -sin(ax),
             0,   1,        0,
        sin(ax),  0,  cos(ax)
      );

  Rotation3D YZ; // a rotation around the x axis
  YZ.SetComponents(
             1,   0,        0,
             0, cos(ay),  sin(ay),
             0,-sin(ay),  cos(ay)
      );
  x0 = XZ*YZ*x0;
  m  = XZ*YZ*m;
  trk.x0 = x0.X();
  trk.y0 = x0.Y();
  //trk.z0 = x0.Z(); // do we need z0?
  trk.mx = m.X()/m.Z();
  trk.my = m.Y()/m.Z();
}

struct gRotation {
  float axz = 0.; // angle around the y axis
  float ayz = 0.; // angle around the x axis
  void Read( std::string );
};
void gRotation::Read(std::string fname ){
  std::ifstream fin;
  fin.open( fname );
  if( fin.is_open() ){
    std::string line;
    while( std::getline( fin, line ) ){
      if( line[0] == '#' ) continue;
      std::stringstream strfl(line);
      strfl >> axz >> ayz;
      break;
    }
  }
  else { std::cerr << "error in opening rotation file, keeping default setting\n"; }
}

// global settings
std::string basedir = "";
std::string geofname = "geometries.txt";
std::string rotfname = "global_rotation.txt";
int NEVENTS = -1;
int SKIPEV  = -1;
bool DoRES = false;
float MaxR = 1.;
int NLADDERS  = 3;

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
    geom[s].LoadGeometry(s,(basedir+"/"+geofname).c_str()); 

    geom[s].PrintGeometry();
  }

  gRotation globalrot;
  globalrot.Read( (basedir+"/"+rotfname).c_str() );

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

  axis *acy = createAxis( "centroid y (mm)", 75, 0, 15. ); 
  axis *acx = createAxis( "centroid x (mm)", 300, 0, 150. );

  axis *arescx = createAxis( "residual x (mm)", 500, -MaxR, MaxR ); 
  axis *arescy = createAxis( "residual y (mm)", 500, -MaxR, MaxR ); 

  std::map<std::string, TH2F*> mh2xy;
  std::map<std::string, TH1F*> mhresx;
  std::map<std::string, TH1F*> mhresy;
  std::map<std::string, TH1F*> mhUresx;
  std::map<std::string, TH1F*> mhUresy;
  std::map<std::string, TH2F*> mh2Uresx;
  std::map<std::string, TH2F*> mh2Uresy;
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
    mh2Uresx[s] = create2DHisto( Form("h2Uresx_%s",s.c_str()), Form("unbiased res x %s",s.c_str()), acx, arescx );
    mh2Uresx[s]->SetDirectory(hdir);
    mh2Uresy[s] = create2DHisto( Form("h2Uresy_%s",s.c_str()), Form("unbiased res y %s",s.c_str()), acy, arescy );
    mh2Uresy[s]->SetDirectory(hdir);
  }

  std::vector<std::pair<std::string,std::string>> hcornames;
  for( auto i = tnames.begin(); i<tnames.end(); i++ ){
    for( auto j = i+1; j<tnames.end(); j++ ){
      hcornames.push_back( {*i,*j} );
    }
  }
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
 
    if( SKIPEV > 0 && i < SKIPEV ) continue;

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
      if( (*cls[s])->size() > 1 ) good = false;
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
    int ncl = 0 ; // counter for ladders without hits
    for( auto icl : cls ){
      XYZVector p;
      if( (*icl.second)->size() ==0  ) { ncl++ ; continue; }
      geom[icl.first].CentroidToLocal( (*icl.second)->at(0), &p ); 
      geom[icl.first].LocalToGlobal(&p);
      seed.push_back(p);
      seeddet.push_back(icl.first);// maybe not useful if detector are ordered
    }

    if( ncl > (4-NLADDERS) ) good = false; // skip seeds with less three hits
    if( ! good ){
      nt->Fill();
      tracks->clear();
      continue; 
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
        Residuals( trk, seed[i], mh2Uresx[seeddet.at(i)], 'x');
        Residuals( trk, seed[i], mh2Uresy[seeddet.at(i)], 'y');
      }
    }

    for( auto &t : *tracks )
      RotateTrack( t, globalrot.axz, globalrot.ayz ); // TODO read it from configuration file

    nt->Fill();
    tracks->clear();
  }

  fout->Write();
  fout->Close();
}

// ------------------------------------------
void print_help(){
  std::cout << " help! \n"
            << " -d [string] sets the basedir, where to find the alignments\n"
            << " -g [string] sets the geometry/alignement file \n"
            << " -R [string] sets the global rotation\n"
            << " -r activates the computation of the residuals\n"
            << " -m [float] sets the limits for the residuals histograms\n"
            << " -l [int] sets the minimum number of ladders used for tracking\n"
            << " -n [int] sets the max number of events\n"
            << " -s [int] skip s events\n";

}
// ------------------------------------------

int main(int argc, char *argv[])
{
  basedir = argv[0];
  // basedir = basedir.substr(0, basedir.size()-10);
  basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";
  std::cout << " basedir " <<  basedir << std::endl;

  // reading some options
  int opt;
  while((opt = getopt(argc, argv, "hrn:m:d:g:R:s:l:")) != -1) { 
    switch(opt) { 
      case 'd':
        basedir = optarg;
        std::cout << " basedir " <<  basedir << std::endl;
        break;
      case 'g':
        geofname = optarg;
        std::cout << " geometry file: " <<  geofname << std::endl;
        break;
      case 'R':
        rotfname = optarg;
        std::cout << " global rotation file: " <<  rotfname << std::endl;
        break;
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
      case 's':
        SKIPEV=std::atoi(optarg);
        std::cout << "Skipping "<< SKIPEV <<"  Events " << std::endl;
        break;
      case 'l':
        NLADDERS=std::atoi(optarg);
        std::cout << "N Ladders "<< NLADDERS << std::endl;
        break;
      case 'h':
        print_help();
        return -1;
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
    print_help();
    return -1;
  }
  
  recoBanco(fnames);

  std::cout << std::setw(100) << " " <<std::endl;
	return 0;
}
