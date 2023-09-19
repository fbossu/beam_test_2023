#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TTreeReader.h"
#include "definition_banco.h"

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


  // some plots
  // ----------
  TFile *fout = TFile::Open( "fout.root", "recreate" );
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

    //fill some histos
    for( auto s : tnames ){
      for( auto cl : *(*cls[s]) ) { 
        XYZVector c0;
        geom[s].CentroidToLocal( cl, &c0 );
        mh2xy[s]->Fill( c0.X(), c0.Y() );
      }
    }

    //XYZVector c0;
    //XYZVector c1;
    ////std::cout << (*cls[tnames[0]])->size() << " " << (*cls[tnames[1]])->size() << "\n";
    //for( auto cls0 = (*cls[tnames[0]])->begin(); cls0 < (*cls[tnames[0]])->end(); cls0++ ){
      //geom[tnames[0]].CentroidToLocal( cls0->colCentroid, cls0->rowCentroid, &c0);
      //for( auto cls1 = (*cls[tnames[1]])->begin(); cls1 < (*cls[tnames[1]])->end(); cls1++ ){
        //geom[tnames[1]].CentroidToLocal( cls1->colCentroid, cls1->rowCentroid, &c1);
        
        ////std::cout <<  c0.X() << " " <<  c1.X() << "\n";
        ////std::cout <<  c0.Y() << " " <<  c1.Y() << "\n";
        //hcorx->Fill( c0.X(), c1.X() );
        //hcory->Fill( c0.Y(), c1.Y() );
      //}
    //}
    
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
