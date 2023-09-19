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

using namespace ROOT::Math;

namespace banco {
  class Ladder {
    public:
      Ladder() { pitch = 0.025; } // 25µm
      virtual ~Ladder() {};
      
      void CentroidToLocal( float, float, XYZVector * ); // col (x), row (y)
      void LocalToGlobal( XYZVector * );
      void SetPitch( float p ) { pitch = p; }

      void SetRotation( Rotation3D r ){ rotation = r; }
      void SetTranslation( XYZVectorF t ){ translation = t; }
      void SetReference( XYZVectorF r ){ reference = r; }

      void LoadGeometry( std::string, std::string );
    private:
      float pitch;
      XYZVectorF reference;
      XYZVectorF translation;
      Rotation3D rotation;

  };

};

void banco::Ladder::LoadGeometry( std::string name, std::string fname ){
  std::ifstream fin;
  fin.open( fname );

  if( fin.is_open() ){
    
    std::string line;
    while( std::getline( fin, line ) ){
      if( line == name ){
        // read reference
        std::getline(fin, line );
        std::stringstream strstr(line);
        float x,y,z;
        strstr >> x >> y >> z;
        reference.SetXYZ(x,y,z);

        // read translation
        std::getline(fin, line );
        strstr.str(line);
        strstr >> x >> y >> z;
        translation.SetXYZ(x,y,z);

        // read rotation
        std::getline(fin, line );
        strstr.str(line);
        float xx,xy,xz;
        float yx,yy,yz;
        float zx,zy,zz;
        strstr  >> xx >> xy >> xz
                >> yx >> yy >> yz
                >> zx >> zy >> zz;

      }
    }

  }
  fin.close();
  
  
}

void banco::Ladder::CentroidToLocal( float col, float row, XYZVector *pos ){
  pos->SetXYZ( col * pitch, row * pitch, 0. );
}

void banco::Ladder::LocalToGlobal( XYZVector *pos ){
  *pos = (*pos) - reference ;
  *pos = rotation*(*pos);
  *pos = translation + (*pos);
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
    // TODO: read conf from file
    //geom[s].SetRotation( Rotation3D(0,1,0,-1,0,0,0,0,1) );
    //geom[s].SetTranslation( XYZVectorF(0,0,10) );
  }


  // some plots
  // ----------
  axis *acx = createAxis( "centroid x", 4000, 0, 128. ); 
  axis *acy = createAxis( "centroid y", 400, 0, 12.8 );

  TH2F *hcorx = create2DHisto( "hcorx", "corr x", acx, acx );
  TH2F *hcory = create2DHisto( "hcory", "corr y", acy, acy );

  // loop over events
  // ================
  int i=0;
  while( reader.Next() && i<10 ){
    //i++;
    XYZVector c0;
    XYZVector c1;
    //std::cout << (*cls[tnames[0]])->size() << " " << (*cls[tnames[1]])->size() << "\n";
    for( auto cls0 = (*cls[tnames[0]])->begin(); cls0 < (*cls[tnames[0]])->end(); cls0++ ){
      geom[tnames[0]].CentroidToLocal( cls0->colCentroid, cls0->rowCentroid, &c0);
      for( auto cls1 = (*cls[tnames[1]])->begin(); cls1 < (*cls[tnames[1]])->end(); cls1++ ){
        geom[tnames[1]].CentroidToLocal( cls1->colCentroid, cls1->rowCentroid, &c1);
        
        //std::cout <<  c0.X() << " " <<  c1.X() << "\n";
        //std::cout <<  c0.Y() << " " <<  c1.Y() << "\n";
        hcorx->Fill( c0.X(), c1.X() );
        hcory->Fill( c0.Y(), c1.Y() );
      }
    }
    
  }
  hcorx->SaveAs("hcorx.root");
  hcory->SaveAs("hcory.root");

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
