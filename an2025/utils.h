#pragma once
#include <TH1F.h>
#include <TFile.h>
#include <TH2F.h>
#include <TH3F.h>

struct axis {
  const char *title; int nbins; float binl; float binh;
};

axis* createAxis( const char *title, int nbins, float binl, float binh );



TH1F* createHisto( const char *name, const char *title, axis *x );


TH1F* createHisto( const char *name, axis *x );

TH2F* create2DHisto( const char *name, const char *title, axis *x, axis *y );

TH2F* create2DHisto( const char *name, axis *x, axis *y );

TH3F* create3DHisto( const char *name, const char *title, axis *x, axis *y, axis *z );
