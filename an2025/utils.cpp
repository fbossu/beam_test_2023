#include "utils.h"

axis* createAxis( const char *title, int nbins, float binl, float binh ){
  axis *x = new axis();
  x->title = title;
  x->nbins = nbins;
  x->binl  = binl;
  x->binh  = binh;
  return x;
}



TH1F* createHisto( const char *name, const char *title, axis *x ){
  TH1F *h = new TH1F(name,title,x->nbins,x->binl,x->binh);
  h->SetXTitle(x->title);
  return h;
}


TH1F* createHisto( const char *name, axis *x ){
  return createHisto(name, name, x);
}

TH2F* create2DHisto( const char *name, const char *title, axis *x, axis *y ){
  TH2F *h = new TH2F(name,title,x->nbins,x->binl,x->binh,y->nbins,y->binl,y->binh);
  h->SetXTitle(x->title);
  h->SetYTitle(y->title);
  return h;
}

TH2F* create2DHisto( const char *name, axis *x, axis *y ){
  return create2DHisto(name, x, y );
}

TH3F* create3DHisto( const char *name, const char *title, axis *x, axis *y, axis *z ){
  TH3F *h = new TH3F(name,title,x->nbins,x->binl,x->binh,y->nbins,y->binl,y->binh, z->nbins,z->binl,z->binh);
  h->SetXTitle(x->title);
  h->SetYTitle(y->title);
  h->SetZTitle(z->title);
  return h;
}
