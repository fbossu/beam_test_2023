#pragma once

#include "Math/Rotation3D.h"
#include "Math/Vector3D.h"

#include "definition_banco.h"

using namespace ROOT::Math;

namespace banco {
  class Ladder {
    public:
      Ladder() { 
        pitchx = 0.028; // mm ; 28 µm
        pitchy = 0.028; // mm ; 28 µm
        flipx = false;
        flipy = false;
      } 
      virtual ~Ladder() {};
      
      void CentroidToLocal( cluster, XYZVector * ); // col (x), row (y)
      void LocalToGlobal( XYZVector * );
      void SetPitchX( float p ) { pitchx = p; }
      void SetPitchY( float p ) { pitchy = p; }

      void SetRotation( Rotation3D r ){ rotation = r; }
      void SetTranslation( XYZVectorF t ){ translation = t; }
      void SetReference( XYZVectorF r ){ reference = r; }
      void SetFlipX( bool f ){ flipx = f; }
      void SetFlipY( bool f ){ flipy = f; }
  
      void LoadGeometry( std::string, std::string );

      void PrintGeometry();

    private:
      float pitchx, pitchy;
      bool flipx, flipy;
      XYZVectorF reference;
      XYZVectorF translation;
      Rotation3D rotation;

  };

};
