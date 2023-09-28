#include <string>

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

  TH1F* hx = new TH1F("hx", "residu X strips (track - centroid)", 200, -90, -40.);
  hx->GetXaxis()->SetTitle("residue on y axis (mm)");
  TH1F* hy = new TH1F("hy", "residu Y strips (track - centroid)", 200, 180, 230.);
  hy->GetXaxis()->SetTitle("residue on x axis (mm)");

  TH1F *hxstrip = new TH1F("hxstrip", "", 200,0,128);
  TH1F *hystrip = new TH1F("hystrip", "", 200,0,128);

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
          hx->Fill(ydet - det.posX(cl.stripCentroid)[1]);
          hxstrip->Fill(cl.stripCentroid);
        }
        if(cl.axis == 'y'){
          hy->Fill(xdet - det.posY(cl.stripCentroid)[0]);
          hystrip->Fill(cl.stripCentroid);
        }
      }
    }
  }

  if(banco.Next()) std::cout<<"WARNING: Missing MM event"<<std::endl;
  
  TCanvas *c = new TCanvas("c", "c", 1600,1000);
  TLatex latex;
  latex.SetTextSize(0.03);
  std::string label;

  c->Divide(2,1);
  c->cd(1);
  hx->Draw();
  hx->Draw("same");
  label = "pitch: "+ std::to_string(det.pitchX(hxstrip->GetMean())).substr(0, 5);
  latex.DrawLatexNDC(0.7, 0.5,(label).c_str());

  label = "inter: "+ std::to_string(det.interX(hxstrip->GetMean())).substr(0, 5);
  latex.DrawLatexNDC(0.7, 0.45,(label).c_str());

  c->cd(2);
  hy->SetLineColor(kRed);
  hy->Draw();
  hy->Draw("same");
  
  label = "pitch: "+ std::to_string(det.pitchY(hystrip->GetMean())).substr(0, 5);
  latex.DrawLatexNDC(0.7, 0.5,(label).c_str());

  label = "inter: "+ std::to_string(det.interY(hystrip->GetMean(), hxstrip->GetMean())).substr(0, 5);
  latex.DrawLatexNDC(0.7, 0.45,(label).c_str());

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

