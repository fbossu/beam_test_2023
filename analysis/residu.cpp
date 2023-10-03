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

  TH1F* hx = new TH1F("hx", "residu X strips ( (track-beamPositionBanco) - (centroid-beamPositionMM) )", 200, -15, 15.);
  hx->GetXaxis()->SetTitle("residue on y axis (mm)");
  TH1F* hy = new TH1F("hy", "residu Y strips ( (track-beamPositionBanco) - (centroid-beamPositionMM) )", 200, -15, 15.);
  hy->GetXaxis()->SetTitle("residue on x axis (mm)");

  TH2F* h2x = new TH2F("h2x", "residu X strips vs y pos", 100,-29,129, 200, -15, 15.);
  h2x->GetXaxis()->SetTitle("position y axis (mm)");
  h2x->GetYaxis()->SetTitle("residue (mm)");

  TH2F* h2y = new TH2F("h2y", "residu Y strips vs x pos", 100,-129,29, 200, -15, 15.);
  h2y->GetXaxis()->SetTitle("position x axis (mm)");
  h2y->GetYaxis()->SetTitle("residue (mm)");

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

  float xtrackBeam = std::accumulate(xtrack.begin(), xtrack.end(), decltype(xtrack)::value_type(0)) / xtrack.size();
  float ytrackBeam = std::accumulate(ytrack.begin(), ytrack.end(), decltype(ytrack)::value_type(0)) / ytrack.size();
  
  double xbeam = det.posY(avgYstrip)[0];
  double ybeam = det.posX(avgXstrip)[1];

  for( int i=0; i<Xstrip.size(); i++){
    hx->Fill((ytrack[i] - ytrackBeam) - (det.posX(Xstrip[i])[1] - ybeam));
    h2x->Fill( det.posX(Xstrip[i])[1], (ytrack[i] - ytrackBeam) - (det.posX(Xstrip[i])[1] - ybeam));
  }

  for( int i=0; i<Ystrip.size(); i++){
    hy->Fill((xtrack[i] - xtrackBeam) - (det.posY(Ystrip[i])[0] - xbeam));
    h2y->Fill(det.posY(Ystrip[i])[0], (xtrack[i] - xtrackBeam) - (det.posY(Ystrip[i])[0] - xbeam));
  }
  
  TCanvas *c = new TCanvas("c", "c", 1600,1000);
  TLatex latex;
  latex.SetTextSize(0.025);
  std::string label;

  c->Divide(2,2);
  c->cd(1);
  hx->Draw();
  hx->Draw("same");
  label = "pitch: "+ std::to_string(det.pitchX(avgXstrip)).substr(0, 5);
  latex.DrawLatexNDC(0.7, 0.5, (label).c_str());

  label = "inter: "+ std::to_string(det.interX(avgXstrip)).substr(0, 5);
  latex.DrawLatexNDC(0.7, 0.47, (label).c_str());

  label = "MM <ybeam> =  "+ std::to_string(ybeam).substr(0, 5);
  latex.DrawLatexNDC(0.7, 0.44, (label).c_str());

  label = "Banco <ybeam> =  "+ std::to_string(ytrackBeam).substr(0, 5);
  latex.DrawLatexNDC(0.7, 0.41, (label).c_str());

  c->cd(2);
  hy->SetLineColor(kRed);
  hy->Draw();
  hy->Draw("same");
  
  label = "pitch: "+ std::to_string(det.pitchY(avgYstrip)).substr(0, 5);
  latex.DrawLatexNDC(0.7, 0.5, (label).c_str());

  label = "inter: "+ std::to_string(det.interY(avgYstrip, avgXstrip)).substr(0, 5);
  latex.DrawLatexNDC(0.7, 0.47, (label).c_str());

  label = "MM <xbeam> =  "+ std::to_string(xbeam).substr(0, 5);
  latex.DrawLatexNDC(0.7, 0.44, (label).c_str());

  label = "Banco <xbeam> =  "+ std::to_string(xtrackBeam).substr(0, 5);
  latex.DrawLatexNDC(0.7, 0.41, (label).c_str());

  c->cd(3);
  h2x->Draw("colz");

  c->cd(4);
  h2y->Draw("colz");

  c->Print(graphname.c_str(), "png");
}


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
  std::cout<<fnameMM<<" pos: "<<pos<<std::endl;
  // std::string graphname = "residue_POS"+std::to_string(pos)+"_stripFEU1_flipedxy.png";
  std::string graphname = "residue_POS"+std::to_string(pos)+"_asaFEU4_flipedxy.png";

  // z pos on murwell strip: -305.2
  // z pos of asa strip: -785.6
  // residu(fnameBanco, fnameMM, det, -305.2, graphname);
  residu(fnameBanco, fnameMM, det, -785.6, graphname);
}

