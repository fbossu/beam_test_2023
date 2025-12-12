#pragma once

#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TH1.h"
#include "TH2.h"
#include "TNtuple.h"
#include <map>
#include <string>
#include <vector>

#include "reco/definitions.h"
#include "banco/definition_banco.h"
#include "anplug.h"
#include "utils.h"

class anres : public anplug {
  public:
    anres( StripTable *d ) : anplug(d) {}
    anres( StripTable *, std::string, float );
    virtual void init( TTreeReader *, TTreeReader* );
    virtual bool run();
    virtual void end();

    void setSampleRate( float d ){ sr = d; }
    void setCftst( float d ){ cftst = d; }
  protected:
    virtual std::vector<hit> getHits( int );
    virtual void sortHitsByTime( std::vector<hit> & );
    inline float stToTime( float );
  private:
    TTreeReaderValue<std::vector<cluster>> *cls;
    TTreeReaderValue<std::vector<hit>> *hits;
    TTreeReaderValue< std::vector<banco::track> > *tracks;
    TTreeReaderValue< unsigned short> *ftst;
    
    std::map<std::string, TH1*> out_m;
    TNtuple *nt;

    float sr;    // sample rate
    float cftst; // ftst factor
};
