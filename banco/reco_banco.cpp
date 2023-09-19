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

#include "utils.h"

#include "ladder.h"

using namespace ROOT::Math;

// =================================================================

banco::track Fit( std::vector<XYZVector> seed ){

  TGraphErrors grx;
  TGraphErrors gry;
  for( int i=0; i<seed.size(); i++ ){
    auto p = seed[i];
    grx.SetPoint(i,p.Z(),p.X());
    gry.SetPoint(i,p.Z(),p.Y());
    grx.SetPointError(i,0,0.025);
    gry.SetPointError(i,0,0.025);
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

  TTreeReaderValue<int> eventId( reader, "eventId");

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
    geom[s];
    geom[s].LoadGeometry(s,"geometries.txt"); // TODO relative paths

    //std::cout << s << std::endl;
    //geom[s].PrintGeometry();
  }


  // some outputs
  // -------------
  TFile *fout =TFile::Open( "fout.root", "recreate" );
  // tree
  TTree *nt = new TTree("events","");
  int trEvId = 0;
  std::vector<banco::track> *tracks = new std::vector<banco::track>();
  nt->Branch("tracks", &tracks);
  nt->Branch("eventId", &trEvId);

  // histograms
  auto hdir = fout->mkdir("histos");

  axis *acy = createAxis( "centroid y", 2000, 0, 128. ); 
  axis *acx = createAxis( "centroid x", 200, 0, 12.8 );

  std::map<std::string, TH2F*> mh2xy;
  for( auto s : tnames ){
    mh2xy[s] = create2DHisto( Form("h2xy_%s",s.c_str()), Form("xy %s",s.c_str()), acx, acy );
    mh2xy[s]->SetDirectory(hdir);
  }
  TH2F *hcorx = create2DHisto( "hcorx", "corr x", acx, acx );
  hcorx->SetDirectory(hdir);
  TH2F *hcory = create2DHisto( "hcory", "corr y", acy, acy );
  hcory->SetDirectory(hdir);

  // loop over events
  // ================
  int i=0;
  while( reader.Next() && i<10 ){
    //i++;
    trEvId = *eventId;
    //fill some histos
    for( auto s : tnames ){
      for( auto cl : *(*cls[s]) ) { 
        XYZVector c0;
        geom[s].CentroidToLocal( cl, &c0 );
        mh2xy[s]->Fill( c0.X(), c0.Y() );
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

    // find combinations of clusters and consider them as a seed
    //std::vector< std::vector<XYZVector> > lseeds;
    std::vector<XYZVector> seed;

    // loop over the ladders
    for( auto icl : cls ){
      XYZVector p;
      geom[icl.first].CentroidToLocal( (*icl.second)->at(0), &p ); // TODO check if there are any cls
      geom[icl.first].LocalToGlobal(&p);
      seed.push_back(p);
    }

    auto track = Fit( seed );
    tracks->push_back(track);
    nt->Fill();
    tracks->clear();
  }

  fout->Write();
  fout->Close();
}

int main(int argc, char const *argv[])
{
  std::vector<std::string> fnames;
  for( int i=1; i<argc; i++){
	  std::string fnameIn = argv[i];
    std::cout << fnameIn << std::endl;
    fnames.push_back( fnameIn );
  }
  recoBanco(fnames);
	return 0;
}
