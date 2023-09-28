#include <string>
#include <numeric>

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TTreeReader.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TGraphErrors.h"
#include "TLatex.h"

#include "../reco/definitions.h"
#include "../banco/definition_banco.h"
#include "../map/StripTable.h"
#include "clusterSize.h"


void residu(std::string fnameBanco, std::string fnameMM, StripTable det, double zpos, std::string graphname = "residue.png"){

  TFile *fbanco = TFile::Open(fnameBanco.c_str(), "read");
  TFile *fMM = TFile::Open(fnameMM.c_str(), "read");

  TTreeReader MM("events", fMM);
  TTreeReader banco("events", fbanco);

  TTreeReaderValue< std::vector<cluster> > cls( MM, "clusters");
  TTreeReaderValue< std::vector<banco::track> > tracks( banco, "tracks");

  TH1F* hx = new TH1F("hx", "residu X strips (track - (centroid - meanPosition))", 200, -20, 20.);
  hx->GetXaxis()->SetTitle("residue on y axis (mm)");
  TH1F* hy = new TH1F("hy", "residu Y strips (track - (centroid - meanPosition))", 200, -20, 20.);
  hy->GetXaxis()->SetTitle("residue on x axis (mm)");

  std::vector<float> Xstrip, Ystrip;
  std::vector<float> xtrack, ytrack;


  while( MM.Next() ){
    bool isBanco = banco.Next();
    if(!isBanco){
      std::cout<<"WARNING: Missing banco event"<<std::endl; 
      continue;
    }

    for(auto tr : *tracks){
      double xdet = tr.x0 + zpos*tr.mx;
      double ydet = tr.y0 + zpos*tr.my;
      for(auto cl : *cls){
        if(cl.axis == 'x'){
          Xstrip.push_back(cl.stripCentroid);
          ytrack.push_back(ydet);
        }
        if(cl.axis == 'y'){
          Ystrip.push_back(cl.stripCentroid);
          xtrack.push_back(xdet);
        }
      }
    }
  }

  if(banco.Next()) std::cout<<"WARNING: Missing MM event"<<std::endl;

  float avgXstrip = std::accumulate(Xstrip.begin(), Xstrip.end(), decltype(Xstrip)::value_type(0)) / Xstrip.size();
  float avgYstrip = std::accumulate(Ystrip.begin(), Ystrip.end(), decltype(Ystrip)::value_type(0)) / Ystrip.size();
  
  double xbeam = det.posY(avgYstrip)[0];
  double ybeam = det.posX(avgXstrip)[1];

  for( int i=0; i<Xstrip.size(); i++){
    hx->Fill(ytrack[i] - (det.posX(Xstrip[i])[1] - ybeam));
  }

  for( int i=0; i<Ystrip.size(); i++){
    hy->Fill(xtrack[i] - (det.posY(Ystrip[i])[0] - xbeam));
  }
  
  TCanvas *c = new TCanvas("c", "c", 1600,1000);
  TLatex latex;
  latex.SetTextSize(0.03);
  std::string label;

  c->Divide(2,1);
  c->cd(1);
  hx->Draw();
  hx->Draw("same");
  label = "pitch: "+ std::to_string(det.pitchX(avgXstrip)).substr(0, 5);
  latex.DrawLatexNDC(0.7, 0.5, (label).c_str());

  label = "inter: "+ std::to_string(det.interX(avgXstrip)).substr(0, 5);
  latex.DrawLatexNDC(0.7, 0.45, (label).c_str());

  c->cd(2);
  hy->SetLineColor(kRed);
  hy->Draw();
  hy->Draw("same");
  
  label = "pitch: "+ std::to_string(det.pitchY(avgYstrip)).substr(0, 5);
  latex.DrawLatexNDC(0.7, 0.5, (label).c_str());

  label = "inter: "+ std::to_string(det.interY(avgYstrip, avgXstrip)).substr(0, 5);
  latex.DrawLatexNDC(0.7, 0.45, (label).c_str());

  c->Print(graphname.c_str(), "png");
}


int main(int argc, char const *argv[])
{

  std::string basedir = argv[0];
  basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";
  std::cout << basedir << std::endl;

  StripTable det(basedir+"../map/strip_map.txt");

  std::string fnameBanco =  argv[1];
  std::string fnameMM =  argv[2];

  int pos = std::stoi( fnameMM.substr(fnameMM.find("POS")+3, fnameMM.find("POS")+5) );
  std::cout<<fnameMM<<" pos: "<<pos<<std::endl;
  std::string graphname = "residue_POS"+std::to_string(pos)+".png";

  residu(fnameBanco, fnameMM, det, -305.2, graphname);
}

