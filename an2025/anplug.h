#pragma once

#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "map/StripTable.h"
#include "TFile.h"
#include <string>

class anplug {
  public:
    anplug() : det(0x0), bancoY(0.), detname(""), fout(0x0), dtol(10.), tchi2(30.) {}
    anplug( StripTable *d ) : bancoY(0.), detname(""), fout(0x0), dtol(10.) { det = d;}
    virtual void init( TTreeReader *, TTreeReader* ) = 0;
    virtual bool run() = 0;
    virtual void end() = 0;

    virtual void setBancoY( float p ) { bancoY = p; }
    virtual void setDetName( std::string n ) { detname = n; }
    virtual void setTolerance( float p ) { dtol = p; }
    virtual void setTrackChi2Cut( float p ) { tchi2 = p; }
  protected:
    StripTable *det;
    std::string detname;
    float bancoY;    // vertical position of banco
    float dtol;      // possible use: distance of a cluster from a track
    float tchi2;     // cut on the track chi2/ndf
    TFile *fout;     // all plugins can have an output file

};
