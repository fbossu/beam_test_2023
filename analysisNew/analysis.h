#ifndef analysis_H
#define analysis_H

#include <string>

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TTreeReader.h"
#include "TStyle.h"
#include "TLegend.h"
#include <math.h>
#include <functional>
#include <iostream>
#include <numeric>
#include "THStack.h"

#include "../reco/definitions.h"
#include "../map/StripTable.h"

// // x,y map of the clusters in the detector by channel number
// void stripMap(TChain* chain, std::string detname, StripTable det);
// void stripMapLims(TChain* chain, std::string detname, StripTable det, std::vector<int> xlim, std::vector<int> ylim);

// // x,y map of the clusters in the detector by position
// void posMap(TChain* chain, std::string detname, StripTable det);
// void posMapLims(TChain* chain, std::string detname, StripTable det, std::vector<int> xlim, std::vector<int> ylim);

// Useful analysis functions
std::shared_ptr<cluster> maxSizeClX(std::vector<cluster> cls);
std::shared_ptr<cluster> maxSizeClY(std::vector<cluster> cls);
std::vector<hit> getHits(std::vector<hit> *hits, int clId);
int totAmp (std::vector<hit> hits, int clId);
int totMaxAmp (std::vector<hit> *hits, int clId);

// clusterSize.cpp functions
void clusterSizeRegion(TChain* chain, std::string detname, StripTable det);
void clusterSizeLims(TChain* chain, std::string detname, StripTable det, std::vector<int> xlim, std::vector<int> ylim);
void clSize_Amp(std::string fname, std::string detname, StripTable det);
void clusterSizeFile(std::string fname, std::string detname, StripTable det, int zone);

// residue.cpp functions
void residue(std::string resName, std::string fnameBanco, std::string fnameMM, StripTable det, float bancoY=0.);
std::vector<double> plotResidue(std::string resName, std::string graphname, double angleX=0., double angleY=0.);
void res3Dplot(std::string resName, std::string graphname);
void plotResidueClsize(std::string resName, std::string graphname);
void plotResidueChannel(std::string resName, std::string graphname);
void plotResidueSt(std::string resName, std::string graphname);
std::vector<double> ResiduePlotAll(StripTable det, std::string fnameBanco, std::string fnameMM, std::string prefix);

// xy_compare.cpp functions
std::vector<double> xy_compare(std::string fBanco, std::string fname, StripTable det, int zone, std::string graphName);
std::vector<double> xy_compareNoBanco(std::string fname, StripTable det, int zone, std::string graphName);

// timeRes.cpp functions
std::vector<double> timeRes(std::string fBanco, std::string fname, StripTable det, int zone, std::string graphName);

#endif
