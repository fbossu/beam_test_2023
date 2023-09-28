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


void residu(std::string fnameBanco, std::string fnameMM, StripTable det, double zpos){

  TFile *fbanco = TFile::Open(fnameBanco.c_str(), "read");
  TFile *fMM = TFile::Open(fnameMM.c_str(), "read");

  TTreeReader MM("events", fMM);
  TTreeReader banco("events", fbanco);

  TTreeReaderValue< std::vector<cluster> > cls( MM, "clusters");
  TTreeReaderValue< std::vector<banco::track> > tracks( banco, "tracks");

  TH1F* hx = new TH1F("hx", ("residu X strips (track - centroid)"+fnameDet).c_str(), 100, -50, 50.);
  hx->GetXaxis()->SetTitle("residue on y axis (mm)");
  TH1F* hy = new TH1F("hy", ("residu Y strips (track - centroid)"+fnameDet).c_str(), 100, -50, 50.);
  hy->GetXaxis()->SetTitle("residue on x axis (mm)");

  while( MM.Next() ){
    bool isBanco = banco.Next();
    if(!isBanco){
      std::cout<<"WARNING: Missing banco event"<<std::endl; 
      continue;
    }
    std::cout<<"cc"<<std::endl;
    for(auto tr : *tracks){
      double xdet = tr.x0 + zpos*tr.mx;
      double ydet = tr.y0 + zpos*tr.my;
      std::cout<<tr.x0<<" "<<tr.y0<<std::endl;
      for(auto cl : *cls){
        if(cl.axis == 'x') hx->Fill(ydet - det.posX(cl.stripCentroid)[1]);
        if(cl.axis == 'y') hy->Fill(xdet - det.posY(cl.stripCentroid)[0]);
      }
    }
  }

  if(banco.Next()) std::cout<<"WARNING: Missing MM event"<<std::endl;
  
  TCanvas *c = new TCanvas("c", "c", 1600,1000);
  c->Divide(2,1);
  c->cd(1);
  hx->Draw();
  hx->Draw("same");

  c->cd(2);
  hy->SetLineColor(kRed);
  hy->Draw();
  hy->Draw("same");
  c->Print("residue.png", "png");
}


int main(int argc, char const *argv[])
{

  std::string basedir = argv[0];
  basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";
  std::cout << basedir << std::endl;

  StripTable det(basedir+"../map/strip_map.txt");

  std::string fnameBanco =  argv[1];
  std::string fnameMM =  argv[2];

  residu(fnameBanco, fnameMM, det, -305.2);
}

