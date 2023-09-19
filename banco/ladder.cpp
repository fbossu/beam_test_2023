#include "ladder.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>

void banco::Ladder::PrintGeometry(){
  std::cout << "\nflips: \n"      << flipx << " " << flipy
            << "\nreference \n"   << reference
            << "\ntranslation \n" << translation
            << "\nrotation " << rotation << std::endl;
}

void banco::Ladder::LoadGeometry( std::string name, std::string fname ){
  std::ifstream fin;
  fin.open( fname );

  if( fin.is_open() ){
    
    std::string line;
    while( std::getline( fin, line ) ){

      if( line == name ){

        // read flips
        std::getline(fin, line );
        std::stringstream strfl(line);
        strfl >> flipx >> flipy;

        // read reference
        std::getline(fin, line );
        float x,y,z;
        std::stringstream strref(line);
        strref >> x >> y >> z;
        reference.SetXYZ(x,y,z);

        // read translation
        std::getline(fin, line );
        std::stringstream strtrans(line);
        strtrans >> x >> y >> z;
        translation.SetXYZ(x,y,z);

        // read rotation
        std::getline(fin, line );
        float xx,xy,xz;
        float yx,yy,yz;
        float zx,zy,zz;
        std::stringstream strrot(line);
        strrot  >> xx >> xy >> xz
                >> yx >> yy >> yz
                >> zx >> zy >> zz;
        rotation.SetComponents(
                    xx , xy , xz ,
                    yx , yy , yz ,
                    zx , zy , zz 
                  );

      }
    }

  }
  fin.close();
  
  
}

void banco::Ladder::CentroidToLocal( banco::cluster cl, XYZVector *pos ){
  float col = cl.colCentroid;
  float row = cl.rowCentroid;
  pos->SetXYZ( 
      (flipx ? 512-row : row) * pitch, 
      (flipy ? 5*1024-col : col) * pitch, 
      0. );
}

void banco::Ladder::LocalToGlobal( XYZVector *pos ){
  *pos = (*pos) - reference ;
  *pos = rotation*(*pos);
  *pos = translation + (*pos);
}
