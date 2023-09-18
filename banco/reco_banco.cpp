#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "definition_banco.h"

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <iomanip>

#include "Math/Rotation3D.h"
#include "Math/Vector3D.h"

using namespace ROOT::Math;

namespace banco {
  class Ladder {
    public:
      Ladder() { pitch = 0.040; } // 40µm
      virtual ~Ladder() {};
      
      void CentroidToLocal( float, float, XYZVector * ); // col (x), row (y)
      void LocalToGlobal( XYZVector * );
      void SetPitch( float p ) { pitch = p; }

      void SetRotation( Rotation3D r ){ rotation = r; }
      void SetTranslation( XYZVectorF t ){ translation = t; }

    private:
      float pitch;
      Rotation3D rotation;
      XYZVectorF translation;

  };

};
void banco::Ladder::CentroidToLocal( float col, float row, XYZVector *pos ){
  pos->SetXYZ( col * pitch, row * pitch, 0. );
}

void banco::Ladder::LocalToGlobal( XYZVector *pos ){
  *pos = rotation*(*pos);
  *pos = translation + (*pos);
}

// =================================================================

void recoBanco(std::string fnameIn, std::string fnameOut){

	TFile* fin = TFile::Open(fnameIn.c_str(),"read");
	TTreeReader reader("events",fin);
	TTreeReaderValue<std::vector<banco::cluster>> cls(reader, "clusters");

  banco::Ladder lad;
  lad.SetRotation( Rotation3D(0,1,0,-1,0,0,0,0,1) );
  lad.SetTranslation( XYZVectorF(0,0,10) );
  lad.SetPitch( 10 );
  XYZVector v;
  int i=0;
  while( reader.Next() && i<10 ){
    i++;
    for( auto cl : *cls ){
      lad.CentroidToLocal( cl.colCentroid, cl.rowCentroid, &v );
      lad.LocalToGlobal( &v );
      std::cout << v << std::endl;
    }
  }

}

int main(int argc, char const *argv[])
{
	/* code */
	std::string fnameIn = argv[1];
  recoBanco(fnameIn, "" );
	return 0;
}
