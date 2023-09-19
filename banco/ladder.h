#pragma once

#include "Math/Rotation3D.h"
#include "Math/Vector3D.h"

using namespace ROOT::Math;

namespace banco {
  class Ladder {
    public:
      Ladder() { 
        pitch = 0.025; // mm ; 25 µm
        flipx = false;
        flipy = false;
      } 
      virtual ~Ladder() {};
      
      void CentroidToLocal( float, float, XYZVector * ); // col (x), row (y)
      void LocalToGlobal( XYZVector * );
      void SetPitch( float p ) { pitch = p; }

      void SetRotation( Rotation3D r ){ rotation = r; }
      void SetTranslation( XYZVectorF t ){ translation = t; }
      void SetReference( XYZVectorF r ){ reference = r; }
      void SetFlipX( bool f ){ flipx = f; }
      void SetFlipY( bool f ){ flipy = f; }
  
      void LoadGeometry( std::string, std::string );

      void PrintGeometry();

    private:
      float pitch;
      bool flipx, flipy;
      XYZVectorF reference;
      XYZVectorF translation;
      Rotation3D rotation;

  };

};
