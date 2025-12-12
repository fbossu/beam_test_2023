#pragma once

#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TH1.h"
#include "TH2.h"
#include <map>
#include <string>
#include <vector>

#include "reco/definitions.h"
#include "banco/definition_banco.h"
#include "anplug.h"
#include "utils.h"

class anhits : public anplug {
  public:
    anhits( StripTable *d ) : anplug(d) {}
    anhits( StripTable *, std::string, float );
    virtual void init( TTreeReader *, TTreeReader* );
    virtual bool run();
    virtual void end();
    
  private:
    TTreeReaderValue<std::vector<cluster>> *cls;
    /*TTreeReaderValue<hit>     *_hits;*/
    /*TTreeReaderValue< std::vector<hit> > *hits;*/
    TTreeReaderValue< std::vector<banco::track> > *tracks;

    std::map<std::string, TH1*> out_m;
};
