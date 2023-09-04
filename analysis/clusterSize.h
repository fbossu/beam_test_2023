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

#include "../reco/definitions.h"
#include "../map/StripTable.h"

// All XYcluster combinations are considered and they are plotted by zone
void clusterSizeRegion(TChain* chain, std::string detname, StripTable det);

// One cluter per event, one in X one in Y.
void clusterSizeLims(TChain* chain, std::string detname, StripTable det, std::vector<int> xlim, std::vector<int> ylim);

#endif