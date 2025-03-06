#include <string>

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TTreeReader.h"
#include "TStyle.h"
#include "TLegend.h"

#include "../reco/definitions.h"
#include "../map/StripTable.h"
#include "clusterSize.h"


int main(int argc, char const *argv[])
{
  
  std::string basedir = argv[0];
  basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";
  std::cout << basedir << std::endl;

  std::vector<int> zoneRuns = {16, 13, 5, 2};
  StripTable det(basedir+"../map/asa_map.txt");

  TChain* chain = new TChain("events");
  std::string detName = "test";
  for( int i = 1; i < argc; i++) {

    TString input = argv[i];

    if( input.Contains( "root" ) ){
      chain->Add( input );
    }
    else{
      std::cout<<"Detector Name: "<<argv[i]<<std::endl;
      detName = argv[i];
    }
  }
  // clusterSizeRegion(chain, detName, det);

  // for( int i = 2; i < argc; i++) {
  //   std::string fname = argv[i];
  //   int pos = std::stoi( fname.substr(fname.find("POS")+3, fname.find("POS")+5) );
  //   if(std::find(zoneRuns.begin(), zoneRuns.end(), pos) != zoneRuns.end()){
  //     // clusterSizeLims(chain, detName, det, fname);
  //     clSize_Amp(fname, detName, det); 
  //   }
  // }
  
  

  // clusterSizeRegion(chain, detName, det);
  clusterSizeLims(chain, detName, det, {2, 16}, {15, 25});
  clusterSizeLims(chain, detName, det, {37, 52}, {50, 65});
  clusterSizeLims(chain, detName, det, {70, 90}, {65, 85});
  clusterSizeLims(chain, detName, det, {100, 110}, {105, 115});

  return 0;
}

