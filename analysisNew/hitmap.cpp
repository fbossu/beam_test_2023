#include <string>

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TTreeReader.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TParameter.h"
#include <map>

#include "../reco/definitions.h"
#include "../map/StripTable.h"
#include "analysis.h"
#include "../style_sheet.h"

struct hist{
  std::string name;
  TH1F *ampFracX, *ampFracY;
  TH1I *clsizeX, *maxAmpX, *clsizeY, *maxAmpY;
  hist() = default;
  hist(std::string name){
    this->name = name;
    clsizeX = new TH1I(Form("clsizeX_%s", name.c_str()), Form("Cluster size X %s", name.c_str()), 10, 0, 10);
    ampFracX = new TH1F(Form("ampFracX_%s", name.c_str()), Form("Amplitude fraction X %s", name.c_str()), 100, 0, 1);
    maxAmpX = new TH1I(Form("maxAmpX_%s", name.c_str()), Form("Max amplitude X %s", name.c_str()), 600, 0, 600);
    clsizeY = new TH1I(Form("clsizeY_%s", name.c_str()), Form("Cluster size Y %s", name.c_str()), 10, 0, 10);
    ampFracY = new TH1F(Form("ampFracY_%s", name.c_str()), Form("Amplitude fraction Y %s", name.c_str()), 100, 0, 1);
    maxAmpY = new TH1I(Form("maxAmpY_%s", name.c_str()), Form("Max amplitude Y %s", name.c_str()), 600, 0, 600);
  }
  double pitchX = 0, interX = 0;
  double pitchY = 0, interY = 0;
  void fillCl(std::shared_ptr<cluster> clX, std::shared_ptr<cluster> clY){
    clsizeX->Fill(clX->size);
    clsizeY->Fill(clY->size);
  }
  void fillMaxAmp(double ampX, double ampY){
    maxAmpX->Fill(ampX);
    maxAmpY->Fill(ampY);
  }
  void fillAmpFrac(double totAmpX, double totAmpY){
    ampFracX->Fill(totAmpX/(totAmpX+totAmpY));
    ampFracY->Fill(totAmpY/(totAmpX+totAmpY));
  }

  void save(TFile* f){
    f->cd();
    f->mkdir(name.c_str());
    f->cd(name.c_str());

    TParameter<Double_t> pitchXParam("pitchX", pitchX);
    pitchXParam.Write();
    TParameter<Double_t> interXParam("interX", interX);
    interXParam.Write();
    TParameter<Double_t> pitchYParam("pitchY", pitchY);
    pitchYParam.Write();
    TParameter<Double_t> interYParam("interY", interY);
    interYParam.Write();

    clsizeX->Write();
    ampFracX->Write();
    maxAmpX->Write();
    clsizeY->Write();
    ampFracY->Write();
    maxAmpY->Write();
  }
};

int main(int argc, char const *argv[])
{
  defStyle();
  std::string basedir = argv[0];
  basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";
  std::cout << basedir << std::endl;

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

  StripTable det;
  if(detName.find("strip") != std::string::npos && detName.find("asa") == std::string::npos){
    det = StripTable(basedir+"../map/strip_map.txt");
  }
  else if(detName.find("asa") != std::string::npos){
    det = StripTable(basedir+"../map/asa_map.txt");
  }
  else if(detName.find("inter") != std::string::npos){
    det = StripTable(basedir+"../map/inter_map.txt");
  }
  else{
    std::cerr<<"Detector not found"<<std::endl;
    return 1;
  }

  TTreeReader reader(chain);
  TTreeReaderValue< std::vector<cluster> > clusters( reader, "clusters");
  TTreeReaderValue< std::vector<hit> > hits( reader, "hits");

  TH2F* h2strip = new TH2F("h2test", "strip number test", 200, -0.5, 128.5, 200, -0.5, 128.5);
  TH2F* h2gerber = new TH2F("h2gerber", "gerber test", 200, -120, 20, 200, -20, 120);

  std::map<int, hist> histMap;

  std::cout<<"Nb triggers "<<chain->GetEntries()<<std::endl;
  while( reader.Next()){
    if(hits->size() == 0) continue;
    auto maxX = maxSizeClX(*clusters);
    auto maxY = maxSizeClY(*clusters);
  
    if(maxX && maxY){
      if(maxX->size < 2 || maxY->size < 2) continue;
      std::vector<hit> hitX = getHits(&(*hits), maxX->id);
      std::vector<hit> hitY = getHits(&(*hits), maxY->id);
      h2strip->Fill(maxY->stripCentroid, maxX->stripCentroid);
      h2gerber->Fill(det.posY(maxX->stripCentroid)[0], det.posX(maxY->stripCentroid)[1]);
      double ampY = totMaxAmp(&hitX, maxX->id);
      double ampX = totMaxAmp(&hitY, maxY->id);
      
      int zone = det.zone(maxX->stripCentroid, maxY->stripCentroid);
      if(histMap.find(zone) == histMap.end()){
        histMap[zone] = hist(Form("zone_%d", zone));
        histMap[zone].pitchX = det.pitchX(maxX->stripCentroid);
        histMap[zone].interX = det.interX(maxX->stripCentroid);
        histMap[zone].pitchY = det.pitchY(maxY->stripCentroid);
        histMap[zone].interY = det.interY(maxY->stripCentroid, maxX->stripCentroid);
      }
      histMap[zone].fillCl(maxX, maxY);
      histMap[zone].fillAmpFrac(ampX, ampY);
      histMap[zone].fillMaxAmp(hitX[0].maxamp, hitY[0].maxamp);
    }
  }

  TCanvas *c = new TCanvas("c", "c", 1000, 1000);
  h2strip->Draw("colz");
  // gPad->SetLogz();
  c->Print(Form("stripMap_%s.png", detName.c_str()));

  TCanvas *c2 = new TCanvas("c2", "c2", 1000, 1000);
  h2gerber->Draw("colz");
  // gPad->SetLogz();
  c2->Print(Form("gerberMap_%s.png", detName.c_str()));

  std::ofstream outfile;
  outfile.open(Form("table_cl1cut_%s.txt", detName.c_str()));
  outfile<<"#run\tzone\tgain"<<std::endl;
  outfile<<"#\t\tXpitch\tXinter\tXclsize\tXampF\tXres"<<std::endl;
  outfile<<"#\t\tYpitch\tYinter\tYclsize\tYampF\tYres"<<std::endl;

  TFile* f = new TFile(Form("hist_cl1cut_%s.root", detName.c_str()), "RECREATE");
  f->cd();
  h2strip->Write();
  h2gerber->Write();
  for(auto& it : histMap){
    outfile<<"testBench"<<"\t"<<it.first<<"\t"<<"1"<<std::endl;
    outfile<<"\t\t"<<it.second.pitchX<<"\t"<<it.second.interX<<"\t"
          <<it.second.clsizeX->GetMean()<<"\t"<<it.second.ampFracX->GetMean()<<"\t"<<0.<<std::endl;
    outfile<<"\t\t"<<it.second.pitchY<<"\t"<<it.second.interY<<"\t"
          <<it.second.clsizeY->GetMean()<<"\t"<<it.second.ampFracY->GetMean()<<"\t"<<0.<<std::endl; 
    it.second.save(f);
  }
  f->Close();
  outfile.close();

  // clusterSizeRegion(chain, detName, det);
  //clusterSizeLims(chain, detName, det, {80, 90}, {90, 100});

  return 0;
}

