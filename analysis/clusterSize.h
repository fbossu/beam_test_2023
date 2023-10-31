#ifndef CLUSTERSIZE_H
#define CLUSTERSIZE_H

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

#include "../reco/definitions.h"
#include "../map/StripTable.h"

// // x,y map of the clusters in the detector by channel number
// void stripMap(TChain* chain, std::string detname, StripTable det);
// void stripMapLims(TChain* chain, std::string detname, StripTable det, std::vector<int> xlim, std::vector<int> ylim);

// // x,y map of the clusters in the detector by position
// void posMap(TChain* chain, std::string detname, StripTable det);
// void posMapLims(TChain* chain, std::string detname, StripTable det, std::vector<int> xlim, std::vector<int> ylim);

// All XYcluster combinations are considered and they are plotted by zone
void clusterSizeRegion(TChain* chain, std::string detname, StripTable det);

// One cluter per event, one in X one in Y.
void clusterSizeLims(TChain* chain, std::string detname, StripTable det, std::vector<int> xlim, std::vector<int> ylim);

void clSize_Amp(std::string fname, std::string detname, StripTable det);

#endif