#pragma once

#include "Math/Rotation3D.h"
#include "Math/Vector3D.h"

#include <stdint.h>

#include "definition_banco.h"

using namespace ROOT::Math;

namespace banco {
  class Ladder {
    public:
      Ladder() { 
        pitchy = 0.02688; // mm ; 28 µm
        pitchx = 0.02924; // mm ; 28 µm
        flipx = false;
        flipy = false;
        deltasensors = 0.015; // mm ; 15 µm ; distance between chips
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
  
      XYZVectorF GetTranslation(){ return translation; }
      void LoadGeometry( std::string, std::string );

      void PrintGeometry();

    private:
      float pitchx, pitchy;
      float deltasensors;
      bool flipx, flipy;
      XYZVectorF reference;
      XYZVectorF translation;
      Rotation3D rotation;

  };

};
