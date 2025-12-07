#pragma once

#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "map/StripTable.h"
#include "TH1.h"
#include "TH2.h"
#include <map>
#include <string>

class anplug {
  public:
    anplug() : det(0x0), bancoY(0.), detname("") {}
    anplug( StripTable *d ) : bancoY(0.), detname("") { det = d;}
    virtual void init( TTreeReader *, TTreeReader* ) = 0;
    virtual bool run() = 0;
    virtual void end() = 0;

    virtual void setBancoY( float p ) { bancoY = p; }
    virtual void setDetName( std::string n ) { detname = n; }
  protected:
    StripTable *det;
    std::string detname;
    float bancoY;

};
