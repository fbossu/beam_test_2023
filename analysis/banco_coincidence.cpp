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

int main(int argc, char const *argv[])
{

  std::string basedir = argv[0];
  basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";
  std::cout << basedir << std::endl;

  // StripTable det(basedir+"../map/strip_map.txt");
  StripTable det(basedir+"../map/asa_map.txt");

  std::string fnameBanco =  argv[1];
  std::string fnameMM =  argv[2];

  int pos = std::stoi( fnameMM.substr(fnameMM.find("POS")+3, fnameMM.find("POS")+5) );

  // std::string graphname = "residue_POS"+std::to_string(pos)+"_stripFEU1.png";
  std::string graphname = "coincidence_POS"+std::to_string(pos)+"_asaFEU4.png";

  // double zpos = -305.6;
  double zpos = -785.6;

  // z pos on murwell strip: -305.6
  // z pos of asa strip: -785.6
  // residu(fnameBanco, fnameMM, det, -305.2, graphname);

  TH2F *h2c = new TH2F("h2c", "Map banco with MM coincidence", 1500, 8, 22, 1500, -2, 12);
  h2c->SetXTitle("x (mm)");
  h2c->SetYTitle("y (mm)");
  
  TFile *fbanco = TFile::Open(fnameBanco.c_str(), "read");
  TFile *fMM = TFile::Open(fnameMM.c_str(), "read");

  TTreeReader MM("events", fMM);
  TTreeReader banco("events", fbanco);

  TTreeReaderValue< std::vector<cluster> > cls( MM, "clusters");
  TTreeReaderValue< std::vector<banco::track> > tracks( banco, "tracks");

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
      bool isXcl = false, isYcl = false;
      
      for(auto cl : *cls){
        if(cl.axis == 'x'){
          // if(abs(det.posX(cl.stripCentroid)[1] - ydet) < 2.) 
          isXcl = true;
        }
        if(cl.axis == 'y'){
          // if(abs(det.posY(cl.stripCentroid)[0] - xdet) < 2.) 
          isYcl = true;
        }
      }
      if(isXcl and isYcl) h2c->Fill(xdet, ydet);
    }
  }

  if(banco.Next()) std::cout<<"WARNING: Missing MM event"<<std::endl;

  TCanvas *c3 = new TCanvas("c3", "c3", 1000,1000);
  // h2c->SetStats(0);
  h2c->Draw("colz");
  gPad->SetLogz();
  c3->Print(graphname.c_str(), "png");

  fbanco->Close();
  fMM->Close();
}

