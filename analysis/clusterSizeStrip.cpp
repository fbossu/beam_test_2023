#include <string>

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TTreeReader.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TLatex.h"

#include "../reco/definitions.h"
#include "../map/StripTable.h"
#include "clusterSize.h"


int main(int argc, char const *argv[])
{

  std::string basedir = argv[0];
  basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";
  std::cout << basedir << std::endl;

  StripTable det(basedir+"../map/strip_map.txt");
  std::vector<int> zoneRuns = {16, 15, 14, 11, 13, 8, 6};

  std::map<std::string, std::vector<double>> avgPos;
  std::map<std::string, int> nEvents;


  TChain* chain = new TChain("events");
  std::string detName = "test";
  for( int i = 1; i < argc; i++) {

    TString input = argv[i];

    if( input.Contains( "root" ) ){
      chain->Add( input );
      avgPos[argv[i]] = std::vector<double>(2, 0);
      nEvents[argv[i]] = 0;
    }
    else{
      std::cout<<"Detector Name: "<<argv[i]<<std::endl;
      detName = argv[i];
    }
  }

  // det.setTransform(0, 0, 0, -3.14159/2, 3.14159, 0.0);
  // det.setTransform(0, 0, 0, 0., 3.14159, 3.14159);

  TTreeReader reader(chain);
  TTreeReaderValue< std::vector<hit> > hits( reader, "hits");
  TTreeReaderValue< std::vector<cluster> > cls( reader, "clusters");

  // TH2F* h2test = new TH2F("h2test", "strip position test", 500, -30, 130, 500, -130, 30);
  TH2F* h2test = new TH2F("h2test", "strip position test", 500, -130, 30, 500, -30, 130);

  while( reader.Next()){
    if(hits->size() == 0) continue;
    auto maxX = maxSizeClX(*cls);
    auto maxY = maxSizeClY(*cls);
    if(!maxX or !maxY) continue;
    std::vector<double> pos = det.pos3D(maxX->stripCentroid, maxY->stripCentroid);
    h2test->Fill(pos[0], pos[1]);
    avgPos[chain->GetCurrentFile()->GetName()][0] += pos[0];
    avgPos[chain->GetCurrentFile()->GetName()][1] += pos[1];
    nEvents[chain->GetCurrentFile()->GetName()]++;
  }

  TCanvas *c = new TCanvas("c", "c", 1000,1000);
  h2test->Draw("colz");
  gPad->SetLogz();
  TLatex* tex = new TLatex();
  tex->SetTextSize(0.03);
  for(auto& p : avgPos){
    if(nEvents[p.first] < 10000) continue;
    p.second[0] /= nEvents[p.first];
    p.second[1] /= nEvents[p.first];
    std::string run = p.first.substr(p.first.find("POS")+3, 2);
    tex->DrawLatex(p.second[0], p.second[1], run.c_str());
  }
  c->Print("test.png", "png");

  // for( int i = 2; i < argc; i++) {
  //   std::string fname = argv[i];
  //   int pos = std::stoi( fname.substr(fname.find("POS")+3, fname.find("POS")+5) );
  //   if(std::find(zoneRuns.begin(), zoneRuns.end(), pos) != zoneRuns.end()){
  //     // clusterSizeLims(chain, detName, det, fname);
  //     clSize_Amp(fname, detName, det); 
  //   }
  // }

  // clusterSizeRegion(chain, detName, det);
  // clusterSizeLims(chain, detName, det, {55, 62}, {30, 50}); //1, 1
  // clusterSizeLims(chain, detName, det, {70, 85}, {30, 50}); //1, 1.5
  // clusterSizeLims(chain, detName, det, {70, 85}, {75, 90}); //1.5, 1.5
  // clusterSizeLims(chain, detName, det, {55, 62}, {75, 90}); //1.5, 1.
  // clusterSizeLims(chain, detName, det, {95, 105}, {75, 90}); //1.5, 0.5
  // clusterSizeLims(chain, detName, det, {95, 105}, {30, 50}); //1., 0.5


  return 0;
}

