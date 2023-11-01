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

// Residue relative to the beam position
void residueRel(StripTable det, std::vector<float> Xstrip, std::vector<float> Ystrip, std::vector<float> xtrack, std::vector<float> ytrack, std::string graphname = "residue.png"){

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


// Absolute residue
void residueAbs(StripTable det, std::vector<float> xdet, std::vector<float> ydet, std::vector<float> xtrack, std::vector<float> ytrack, std::string graphname = "residue.png"){

  std::vector<float> xresidue, yresidue;

  for(int i=0; i<xdet.size(); i++){
    xresidue.push_back(xtrack[i] - xdet[i]);
  }

  for(int i=0; i<ydet.size(); i++){
    yresidue.push_back(ytrack[i] - ydet[i]);
  }

  float avgxresidue = std::accumulate(xresidue.begin(), xresidue.end(), decltype(xresidue)::value_type(0)) / xresidue.size();
  float avgyresidue = std::accumulate(yresidue.begin(), yresidue.end(), decltype(yresidue)::value_type(0)) / yresidue.size();
  
  float avgxdet = std::accumulate(xdet.begin(), xdet.end(), decltype(xdet)::value_type(0)) / xdet.size();
  float avgydet = std::accumulate(ydet.begin(), ydet.end(), decltype(ydet)::value_type(0)) / ydet.size();

  TH1F* hx = new TH1F("hx", "residu X strips (track - centroid)", 200, avgyresidue-8, avgyresidue+8);
  hx->GetXaxis()->SetTitle("residue on y axis (mm)");
  TH1F* hy = new TH1F("hy", "residu Y strips (track - centroid)", 200, avgxresidue-8, avgxresidue+8);
  hy->GetXaxis()->SetTitle("residue on x axis (mm)");

  TH1F* hbsx = new TH1F("hbsx", "Beam spot X strips", 300, -29, 129);
  hbsx->GetXaxis()->SetTitle("cluter position on y axis (mm)");
  TH1F* hbsy = new TH1F("hbsy", "Beam spot Y strips", 300, -129, 29);
  hbsy->GetXaxis()->SetTitle("cluter position on x axis (mm)");

  TH1F* hbsx_tracks = new TH1F("hbsx_tracks", "Beam spot tracks", 300, -30, 138);
  hbsx_tracks->GetXaxis()->SetTitle("cluter position on x axis (mm)");
  TH1F* hbsy_tracks = new TH1F("hbsy_tracks", "Beam spot tracks", 300, -30, 30);
  hbsy_tracks->GetXaxis()->SetTitle("cluter position on y axis (mm)");

  TH2F* h2x = new TH2F("h2x", "residu X strips vs y pos", 100,avgydet-10,avgydet+10, 200, avgyresidue-8, avgyresidue+8);
  h2x->GetXaxis()->SetTitle("position y axis (mm)");
  h2x->GetYaxis()->SetTitle("residue (mm)");

  TH2F* h2y = new TH2F("h2y", "residu Y strips vs x pos", 100,avgxdet-10,avgxdet+10, 200, avgxresidue-8, avgxresidue+8);
  h2y->GetXaxis()->SetTitle("position x axis (mm)");
  h2y->GetYaxis()->SetTitle("residue (mm)");

  for( int i=0; i<ydet.size(); i++){
    hx->Fill(yresidue[i]);
    h2x->Fill( ydet[i], yresidue[i]);
    hbsx->Fill(ydet[i]);
    hbsx_tracks->Fill(xtrack[i]);
  }

  for( int i=0; i<xdet.size(); i++){
    hy->Fill(xresidue[i]);
    h2y->Fill(xdet[i], xresidue[i]);
    hbsy->Fill(xdet[i]);
    hbsy_tracks->Fill(ytrack[i]);
  }
  
  TCanvas *c = new TCanvas("c", "c", 1600,1000);
  TLatex latex;
  latex.SetTextSize(0.025);
  std::string label;

  c->Divide(2,2);
  c->cd(1);
  hx->Draw();
  // label = "pitch: "+ std::to_string(det.pitchX(avgydet)).substr(0, 5);
  // latex.DrawLatexNDC(0.7, 0.5, (label).c_str());

  // label = "inter: "+ std::to_string(det.interX(avgydet)).substr(0, 5);
  // latex.DrawLatexNDC(0.7, 0.47, (label).c_str());

  c->cd(2);
  hy->SetLineColor(kRed);
  hy->Draw();
  
  // label = "pitch: "+ std::to_string(det.pitchY(avgxdet)).substr(0, 5);
  // latex.DrawLatexNDC(0.7, 0.5, (label).c_str());

  // label = "inter: "+ std::to_string(det.interY(avgxdet, avgydet)).substr(0, 5);
  // latex.DrawLatexNDC(0.7, 0.47, (label).c_str());

  c->cd(3);
  h2x->Draw("colz");
  gPad->SetLogz();

  c->cd(4);
  h2y->Draw("colz");
  gPad->SetLogz();

  c->Print(graphname.c_str(), "png");

  TCanvas *c1 = new TCanvas("c1", "c1", 1600,1000);

  c1->Print(("BS"+graphname).c_str(), "png");
  c1->Divide(2,2);
  c1->cd(1);
  hbsx->Draw();

  c1->cd(2);
  // hbsy->SetLineColor(kRed);
  hbsy->Draw();

  c1->cd(3);
  hbsx_tracks->SetLineColor(kRed);
  hbsx_tracks->Draw();

  c1->cd(4);
  hbsy_tracks->SetLineColor(kRed);
  hbsy_tracks->Draw();

  c1->Print(("BS"+graphname).c_str(), "png");
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
  // std::string graphname = "residue_POS"+std::to_string(pos)+"_stripFEU1.png";
  std::string graphname = "residue_POS"+std::to_string(pos)+"_asaFEU4.png";

  double zpos = -785.6, Ty = -93.6933, Tx = 80.169; // POS16
  // double zpos = -305.2, Ty = -7.4934, Tx = 29.08; //POS05
  // double zpos = -785.6, Ty = -61.5856, Tx = 24.817; // POS13
  // double zpos = -305.6, Ty = -63.382, Tx = 28.377; //POS13
  
  // z pos on murwell strip: -305.2
  // z pos of asa strip: -785.6
  // residu(fnameBanco, fnameMM, det, -305.2, graphname);
  
  TFile *fbanco = TFile::Open(fnameBanco.c_str(), "read");
  TFile *fMM = TFile::Open(fnameMM.c_str(), "read");

  TTreeReader MM("events", fMM);
  TTreeReader banco("events", fbanco);

  TTreeReaderValue< std::vector<cluster> > cls( MM, "clusters");
  TTreeReaderValue< std::vector<banco::track> > tracks( banco, "tracks");

  std::vector<float> xdet, ydet;
  std::vector<float> xtrack, ytrack;

  while( MM.Next() ){
    bool isBanco = banco.Next();
    if(!isBanco){
      std::cout<<"WARNING: Missing banco event"<<std::endl; 
      continue;
    }

    for(auto tr : *tracks){
      if(tr.chi2x>1. or tr.chi2y>1.) continue;

      double xdetTrack = tr.x0 + zpos*tr.mx;
      double ydetTrack = tr.y0 + zpos*tr.my;
      std::vector<cluster> clsX, clsY;

      std::copy_if (cls->begin(), cls->end(), std::back_inserter(clsX),
                  [](const cluster& c){return c.axis=='x';} );
      std::copy_if (cls->begin(), cls->end(), std::back_inserter(clsY),
                  [](const cluster& c){return c.axis=='y';} );
      if(clsX.size() == 0 || clsY.size() == 0) continue;

      auto maxX = *std::max_element(clsX.begin(), clsX.end(),
                         [](const cluster& a,const cluster& b) { return a.size < b.size; });
      auto maxY = *std::max_element(clsY.begin(), clsY.end(),
                         [](const cluster& a,const cluster& b) { return a.size < b.size; });

      ydet.push_back(det.posX(maxX.stripCentroid)[1] + Ty);
      ytrack.push_back(ydetTrack);
      
      xdet.push_back(det.posY(maxY.stripCentroid)[0] + Tx);
      xtrack.push_back(xdetTrack);
    }
  }

  if(banco.Next()) std::cout<<"WARNING: Missing MM event"<<std::endl;

  // residueRel(det, xdet, ydet, xtrack, ytrack, "rel_"+graphname);
  residueAbs(det, xdet, ydet, xtrack, ytrack, "abs_"+graphname);
}

