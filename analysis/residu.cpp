#include <string>
#include <numeric>
#include <cmath>

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"
#include "TH3.h"
#include "TNtupleD.h"
#include "TCanvas.h"
#include "TTreeReader.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TLegend.h"
#include "TGraphErrors.h"
#include "TLatex.h"
#include "TProfile.h"

#include "../reco/definitions.h"
#include "../banco/definition_banco.h"
#include "../map/StripTable.h"
#include "clusterSize.h"

// TStyle* myStyle;
void defStyle(){
  // myStyle = (TStyle*)gStyle->Clone(); // copy the default style
  // myStyle = gROOT->GetStyle("Default");
  // TStyle* myStyle = new TStyle("Plain","Default Style");
  gStyle->SetName("myStyle");
  gStyle->SetTextFont(43);
  gStyle->SetTextSize(22);

  // Set the font and size for all axis labels
  gStyle->SetLabelFont(43, "XYZ"); // Set the font to Helvetica for the labels of the x-axis, y-axis, and z-axis
  gStyle->SetLabelSize(22, "XYZ"); // Set the font size for the labels of the x-axis, y-axis, and z-axis

  // Set the font and size for all axis titles
  gStyle->SetTitleFont(43, "XYZ"); // Set the font to Helvetica for the titles of the x-axis, y-axis, and z-axis
  gStyle->SetTitleSize(22, "XYZ"); // Set the font size for the titles of the x-axis, y-axis, and z-axis

  gStyle->SetTitleFont(43,"T"); // Set the font to Helvetica for the titles of the x-axis, y-axis, and z-axis
  gStyle->SetTitleSize(22,"T"); // Set the font size for the titles of the x-axis, y-axis, and z-axis

  // gROOT->SetStyle("myStyle");
  // gROOT->ForceStyle();
}


// Residue relative to the beam position
// void residueRel(StripTable det, std::vector<float> Xstrip, std::vector<float> Ystrip, std::vector<float> xtrack, std::vector<float> ytrack, std::string graphname = "residue.png"){

//   TH1F* hx = new TH1F("hx", "residu X strips ( (track-beamPositionBanco) - (centroid-beamPositionMM) )", 200, -15, 15.);
//   hx->GetXaxis()->SetTitle("residue on y axis (mm)");
//   TH1F* hy = new TH1F("hy", "residu Y strips ( (track-beamPositionBanco) - (centroid-beamPositionMM) )", 200, -15, 15.);
//   hy->GetXaxis()->SetTitle("residue on x axis (mm)");

//   TH2F* h2x = new TH2F("h2x", "residu X strips vs y pos", 100,-29,129, 200, -15, 15.);
//   h2x->GetXaxis()->SetTitle("position y axis (mm)");
//   h2x->GetYaxis()->SetTitle("residue (mm)");

//   TH2F* h2y = new TH2F("h2y", "residu Y strips vs x pos", 100,-129,29, 200, -15, 15.);
//   h2y->GetXaxis()->SetTitle("position x axis (mm)");
//   h2y->GetYaxis()->SetTitle("residue (mm)");

//   float avgXstrip = std::accumulate(Xstrip.begin(), Xstrip.end(), decltype(Xstrip)::value_type(0)) / Xstrip.size();
//   float avgYstrip = std::accumulate(Ystrip.begin(), Ystrip.end(), decltype(Ystrip)::value_type(0)) / Ystrip.size();

//   float xtrackBeam = std::accumulate(xtrack.begin(), xtrack.end(), decltype(xtrack)::value_type(0)) / xtrack.size();
//   float ytrackBeam = std::accumulate(ytrack.begin(), ytrack.end(), decltype(ytrack)::value_type(0)) / ytrack.size();
  
//   double xbeam = det.posY(avgYstrip)[0];
//   double ybeam = det.posX(avgXstrip)[1];

//   for( int i=0; i<Xstrip.size(); i++){
//     hx->Fill((ytrack[i] - ytrackBeam) - (det.posX(Xstrip[i])[1] - ybeam));
//     h2x->Fill( det.posX(Xstrip[i])[1], (ytrack[i] - ytrackBeam) - (det.posX(Xstrip[i])[1] - ybeam));
//   }

//   for( int i=0; i<Ystrip.size(); i++){
//     hy->Fill((xtrack[i] - xtrackBeam) - (det.posY(Ystrip[i])[0] - xbeam));
//     h2y->Fill(det.posY(Ystrip[i])[0], (xtrack[i] - xtrackBeam) - (det.posY(Ystrip[i])[0] - xbeam));
//   }
  
//   TCanvas *c = new TCanvas("c", "c", 1600,1000);
//   TLatex latex;
//   latex.SetTextSize(0.025);
//   std::string label;

//   c->Divide(2,2);
//   c->cd(1);
//   hx->Draw();
//   hx->Draw("same");
//   label = "pitch: "+ std::to_string(det.pitchX(avgXstrip)).substr(0, 5);
//   latex.DrawLatexNDC(0.7, 0.5, (label).c_str());

//   label = "inter: "+ std::to_string(det.interX(avgXstrip)).substr(0, 5);
//   latex.DrawLatexNDC(0.7, 0.47, (label).c_str());

//   label = "MM <ybeam> =  "+ std::to_string(ybeam).substr(0, 5);
//   latex.DrawLatexNDC(0.7, 0.44, (label).c_str());

//   label = "Banco <ybeam> =  "+ std::to_string(ytrackBeam).substr(0, 5);
//   latex.DrawLatexNDC(0.7, 0.41, (label).c_str());

//   c->cd(2);
//   hy->SetLineColor(kRed);
//   hy->Draw();
//   hy->Draw("same");
  
//   label = "pitch: "+ std::to_string(det.pitchY(avgYstrip)).substr(0, 5);
//   latex.DrawLatexNDC(0.7, 0.5, (label).c_str());

//   label = "inter: "+ std::to_string(det.interY(avgYstrip, avgXstrip)).substr(0, 5);
//   latex.DrawLatexNDC(0.7, 0.47, (label).c_str());

//   label = "MM <xbeam> =  "+ std::to_string(xbeam).substr(0, 5);
//   latex.DrawLatexNDC(0.7, 0.44, (label).c_str());

//   label = "Banco <xbeam> =  "+ std::to_string(xtrackBeam).substr(0, 5);
//   latex.DrawLatexNDC(0.7, 0.41, (label).c_str());

//   c->cd(3);
//   h2x->Draw("colz");

//   c->cd(4);
//   h2y->Draw("colz");

//   c->Print(graphname.c_str(), "png");
// }


// // Absolute residue
// void residueAbs(StripTable det, std::vector<float> xdet, std::vector<float> ydet, std::vector<float> xtrack, std::vector<float> ytrack, std::string graphname = "residue.png"){

//   std::vector<float> xresidue, yresidue;

//   for(int i=0; i<xdet.size(); i++){
//     xresidue.push_back(xtrack[i] - xdet[i]);
//   }

//   for(int i=0; i<ydet.size(); i++){
//     yresidue.push_back(ytrack[i] - ydet[i]);
//   }

//   float avgxresidue = std::accumulate(xresidue.begin(), xresidue.end(), decltype(xresidue)::value_type(0)) / xresidue.size();
//   float avgyresidue = std::accumulate(yresidue.begin(), yresidue.end(), decltype(yresidue)::value_type(0)) / yresidue.size();
  
//   float avgxdet = std::accumulate(xdet.begin(), xdet.end(), decltype(xdet)::value_type(0)) / xdet.size();
//   float avgydet = std::accumulate(ydet.begin(), ydet.end(), decltype(ydet)::value_type(0)) / ydet.size();

//   TH1F* hx = new TH1F("hx", "residu X strips (track - centroid)", 200, -4, 4);
//   hx->GetXaxis()->SetTitle("residue on y axis (mm)");
//   TH1F* hy = new TH1F("hy", "residu Y strips (track - centroid)", 200, -4, 4);
//   hy->GetXaxis()->SetTitle("residue on x axis (mm)");

//   TH1F* hbsx = new TH1F("hbsx", "Beam spot X strips", 300, -29, 129);
//   hbsx->GetXaxis()->SetTitle("cluter position on y axis (mm)");
//   TH1F* hbsy = new TH1F("hbsy", "Beam spot Y strips", 300, -129, 29);
//   hbsy->GetXaxis()->SetTitle("cluter position on x axis (mm)");

//   TH1F* hbsx_tracks = new TH1F("hbsx_tracks", "Beam spot tracks", 300, -30, 138);
//   hbsx_tracks->GetXaxis()->SetTitle("cluter position on x axis (mm)");
//   TH1F* hbsy_tracks = new TH1F("hbsy_tracks", "Beam spot tracks", 300, -30, 30);
//   hbsy_tracks->GetXaxis()->SetTitle("cluter position on y axis (mm)");

//   TH2F* h2x = new TH2F("h2x", "residu X strips vs y pos", 100,avgydet-4,avgydet+4, 200, -4, 4);
//   h2x->GetXaxis()->SetTitle("position y axis (mm)");
//   h2x->GetYaxis()->SetTitle("residue (mm)");

//   TH2F* h2y = new TH2F("h2y", "residu Y strips vs x pos", 100,avgxdet-4,avgxdet+4, 200, -4, 4);
//   h2y->GetXaxis()->SetTitle("position x axis (mm)");
//   h2y->GetYaxis()->SetTitle("residue (mm)");

//   for( int i=0; i<ydet.size(); i++){
//     hx->Fill(yresidue[i]);
//     h2x->Fill( ydet[i], yresidue[i]);
//     hbsx->Fill(ydet[i]);
//     hbsx_tracks->Fill(xtrack[i]);
//   }

//   for( int i=0; i<xdet.size(); i++){
//     hy->Fill(xresidue[i]);
//     h2y->Fill(xdet[i], xresidue[i]);
//     hbsy->Fill(xdet[i]);
//     hbsy_tracks->Fill(ytrack[i]);
//   }
  
//   TCanvas *c = new TCanvas("c", "c", 1600,1000);
//   TLatex latex;
//   latex.SetTextSize(0.025);
//   std::string label;

//   c->Divide(2,2);
//   c->cd(1);
//   hx->Draw();
//   // label = "pitch: "+ std::to_string(det.pitchX(avgydet)).substr(0, 5);
//   // latex.DrawLatexNDC(0.7, 0.5, (label).c_str());

//   // label = "inter: "+ std::to_string(det.interX(avgydet)).substr(0, 5);
//   // latex.DrawLatexNDC(0.7, 0.47, (label).c_str());

//   c->cd(2);
//   hy->SetLineColor(kRed);
//   hy->Draw();
  
//   // label = "pitch: "+ std::to_string(det.pitchY(avgxdet)).substr(0, 5);
//   // latex.DrawLatexNDC(0.7, 0.5, (label).c_str());

//   // label = "inter: "+ std::to_string(det.interY(avgxdet, avgydet)).substr(0, 5);
//   // latex.DrawLatexNDC(0.7, 0.47, (label).c_str());

//   c->cd(3);
//   h2x->Draw("colz");
//   gPad->SetLogz();

//   c->cd(4);
//   h2y->Draw("colz");
//   gPad->SetLogz();

//   c->Print(graphname.c_str(), "png");

//   TCanvas *c1 = new TCanvas("c1", "c1", 1600,1000);

//   c1->Print(("BS"+graphname).c_str(), "png");
//   c1->Divide(2,2);
//   c1->cd(1);
//   hbsx->Draw();

//   c1->cd(2);
//   // hbsy->SetLineColor(kRed);
//   hbsy->Draw();

//   c1->cd(3);
//   hbsx_tracks->SetLineColor(kRed);
//   hbsx_tracks->Draw();

//   c1->cd(4);
//   hbsy_tracks->SetLineColor(kRed);
//   hbsy_tracks->Draw();

//   c1->Print(("BS"+graphname).c_str(), "png");
// }

double getMean(TNtupleD* nt, const char* columnName, std::vector<double> lim = {}) {
  double sum = 0.;
  int count = 0;
  double col = 0.;

  nt->ResetBranchAddresses();
  nt->SetBranchAddress(columnName, &col);

  for (int i = 0; i < nt->GetEntries(); i++) {
    nt->GetEntry(i);
    if(lim.size() == 2 and (col < lim[0] or col > lim[1])) continue;
    sum += col;
    count++;
  }
  return sum / count;
}

double getStdDev(TNtupleD* nt, const char* columnName, std::vector<double> lim = {}) {
  double mean = getMean(nt, columnName, lim);
  // std::cout<<"mean: "<<mean<<std::endl;
  double sumDiff = 0.;
  double col = 0.;
  int count = 0;

  nt->ResetBranchAddresses();
  nt->SetBranchAddress(columnName, &col);

  for(int i = 0; i < nt->GetEntries(); i++) {
    nt->GetEntry(i);
    if(lim.size() == 2 and (col < lim[0] or col > lim[1])) continue;
    // if(col - mean < 10){
      sumDiff += pow(col - mean, 2);
      count++;
    // }
  }

  return sqrt(sumDiff / count);
}


std::vector<double> rotation(double posx, double posy, double posz){

  ROOT::Math::Rotation3D rot(ROOT::Math::RotationZYX(0., 0.7, 0.)); // rotation around z, y, x
  ROOT::Math::Translation3D trl(0., 0., posz);
  ROOT::Math::Transform3D trans = ROOT::Math::Transform3D(rot, trl);

  ROOT::Math::XYZPoint pdet(posx, posy, 0.);
	ROOT::Math::XYZPoint pr = trans(pdet);
	return {pr.x(), pr.y(), pr.z()};
}


double Xpitch, Xinter, Ypitch, Yinter;

void residue(TFile* res, std::string fnameBanco, std::string fnameMM, StripTable det){

  TNtupleD *nt = new TNtupleD("nt", "nt", "xtrack:ytrack:xdet:ydet:xres:yres:Xclsize:Yclsize:Xmaxamp:Ymaxamp:stX:stY:stresX:stresY:chX:chY:chresX:chresY");
  nt->SetDirectory(res);

  TFile* fMM = TFile::Open(fnameMM.c_str(), "read");
  TFile* fbanco = TFile::Open(fnameBanco.c_str(), "read");

  TTreeReader MM("events", fMM);
  TTreeReader banco("events", fbanco);

  TTreeReaderValue< std::vector<cluster> > cls( MM, "clusters");
  TTreeReaderValue< std::vector<hit> > hits( MM, "hits");
  TTreeReaderValue< std::vector<banco::track> > tracks( banco, "tracks");

  double stX = 0;
  double stY = 0;
  int n = 0;

  double avgxdet = 0;

  while( MM.Next() ){
    // if(n==1000){
      // std::cout<<"n: "<<n<<std::endl;
      // n++;
      // continue;
    // }
    bool isBanco = banco.Next();
    if(!isBanco){
      std::cout<<"WARNING: Missing banco event"<<std::endl;
      continue;
    }
    if(tracks->size() == 0 or cls->size() == 0) continue;

    auto tr = *std::min_element(tracks->begin(), tracks->end(),
                       [](const banco::track& a,const banco::track& b) { return a.chi2x+a.chi2y < b.chi2x+b.chi2y; });
    if(tr.chi2x>1 or tr.chi2y>1) continue;
    auto maxX = maxSizeClX(*cls);
    auto maxY = maxSizeClY(*cls);
    
    if(maxX && maxY){
      // if(maxY->size != 3) continue;
      n++; stX += maxX->stripCentroid; stY += maxY->stripCentroid;
      auto hitsX = getHits(*hits, maxX->id);
      auto hitsY = getHits(*hits, maxY->id);
      if(hitsX.size() == 0 or hitsY.size() == 0){
              std::cout<<"aaaaaaaahhhhhhhhhhhhhhhhhhhh"<<std::endl;
        continue;
      }

      std::vector<double> detPos = det.pos3D(maxX->stripCentroid, maxY->stripCentroid);
      // detPos = rotation(detPos[0], detPos[1], detPos[2]);
      double xdet = detPos[0];
      double ydet = detPos[1];
      // std::cout<<"xdet: "<<xdet<<" ydet: "<<ydet<<" zdet: "<<detPos[2]<<std::endl;

      double xtrack = tr.x0 + detPos[2]*tr.mx;
      double ytrack = tr.y0 + detPos[2]*tr.my;
      
      for(int i=0; i<hitsY.size(); i++){
        if(hitsY[i].strip==64) std::cout<<"hitY: "<<hitsY[i].strip<<" "<<hitsY[i].channel<<std::endl;
      }

      // if(hitsX[0].maxamp < 400 or hitsY[0].maxamp < 400) continue;

      // double Xth = 0, Yth = 0;
      // double Xamp = 0, Yamp = 0;
      // for(int i=0; i<hitsX.size(); i++){
      //   if(hitsX[i].channel<392 || hitsX[i].channel>444) continue;
      //   Xth  += (hitsX[i].maxamp-256)*hitsX[i].strip;
      //   Xamp += (hitsX[i].maxamp-256);
      // }

      // for(int i=0; i<hitsY.size(); i++){
      //   if(hitsY[i].channel<392 || hitsX[i].channel>444) continue;
      //   Yth  += (hitsY[i].maxamp-256)*hitsY[i].strip;
      //   Yamp += (hitsY[i].maxamp-256);
      // }

      // if(Xamp<1 || Yamp<1) continue;
      // std::vector<double> detPosTh = det.pos3D(Xth/Xamp, Yth/Yamp);
      // double xdet = detPosTh[0];
      // double ydet = detPosTh[1];

      // double xtrack = tr.x0 + detPosTh[2]*tr.mx;
      // double ytrack = tr.y0 + detPosTh[2]*tr.my;
      avgxdet += xdet;

      double data[18] = {xtrack, ytrack, xdet, ydet, xtrack-xdet, ytrack-ydet, maxX->size, maxY->size, hitsX[0].maxamp, hitsY[0].maxamp, 
          maxX->stripCentroid, maxY->stripCentroid, ytrack-maxX->stripCentroid, xtrack-maxY->stripCentroid, maxX->centroid, maxY->centroid, ytrack-maxX->centroid, xtrack-maxY->centroid};

      nt->Fill(data);
    }
  }
  if(banco.Next()) std::cout<<"WARNING: Missing MM event"<<std::endl;
  Xpitch = det.pitchX(stX/n);
  Xinter = det.interX(stX/n);
  Ypitch = det.pitchY(stY/n);
  Yinter = det.interY(stY/n, stX/n);
  std::cout<<"avgxdet: "<<avgxdet/n<<std::endl;

  res->Write();
  delete nt;
}


void plotResidue(TFile* res, std::string graphname){
  
  // defStyle();

  TNtupleD* nt = (TNtupleD*) res->Get("nt");

  double meanxdet = getMean(nt, "xdet",{-200, 200});
  double meanydet = getMean(nt, "ydet",{-200, 200});
  double stdx = getStdDev(nt, "xres",{-50, 50});
  double stdy = getStdDev(nt, "yres",{-50, 50});

  double meanresx = getMean(nt, "xres",{-200, 200});
  double meanresy = getMean(nt, "yres",{-200, 200});
  double meanstX = getMean(nt, "stX",{-50, 50});
  double meanstY = getMean(nt, "stY",{-50, 50});

  double avg_std = (stdx+stdy)/2.;

  std::cout<<"meanxdet: "<<meanxdet<<" stdx: "<<stdx<<std::endl;

  TH1F* hx = new TH1F("hx", "residu X strips (track - centroid)", 300, meanresy-1.5*avg_std, meanresy+1.5*avg_std);
  hx->GetXaxis()->SetTitle("residue on y axis (mm)");
  TH1F* hy = new TH1F("hy", "residu Y strips (track - centroid)", 300, meanresx-1.5*avg_std, meanresx+1.5*avg_std);
  hy->GetXaxis()->SetTitle("residue on x axis (mm)");
  nt->Draw("yres>>hx");
  nt->Draw("xres>>hy");

  // Fit hx with a Gaussian function
  TF1* fitFuncX = new TF1("fitFuncX", "gaus", meanresy-2.*avg_std, meanresy+2.*avg_std);
  fitFuncX->SetParameters(0, stdy);
  hx->Fit(fitFuncX, "R");

  // Fit hy with a Gaussian function
  TF1* fitFuncY = new TF1("fitFuncY", "gaus", meanresx-2.*avg_std, meanresx+2.*avg_std);
  fitFuncY->SetParameters(0, stdx);
  hy->Fit(fitFuncY, "R");

  TH2F* h2x = new TH2F("h2x", "residu X strips vs y pos", 300, meanydet-4, meanydet+4, 300, meanresy-1.5*avg_std, meanresy+1.5*avg_std);
  h2x->GetXaxis()->SetTitle("position y axis (mm)");
  h2x->GetYaxis()->SetTitle("residue (mm)");

  TH2F* h2y = new TH2F("h2y", "residu Y strips vs x pos", 300, meanxdet-4, meanxdet+4, 300, meanresx-1.5*avg_std, meanresx+1.5*avg_std);
  h2y->GetXaxis()->SetTitle("position x axis (mm)");
  h2y->GetYaxis()->SetTitle("residue (mm)");
  nt->Draw("yres:ydet>>h2x");
  nt->Draw("xres:xdet>>h2y");

  TProfile* prx = new TProfile("prx", "residu X strips vs y pos", 300, meanydet-4, meanydet+4, meanresy-1.5*avg_std, meanresy+1.5*avg_std);
  TProfile* pry = new TProfile("pry", "residu Y strips vs x pos", 300, meanxdet-4, meanxdet+4, meanresx-1.5*avg_std, meanresx+1.5*avg_std);
  nt->Draw("yres:ydet>>prx");
  nt->Draw("xres:xdet>>pry");

  // gStyle->SetTextFont(43); // Set the font to Helvetica
  // gStyle->SetTextSize(20); // Set the font size to 0.05

  TCanvas *c = new TCanvas("c", "c", 1600,1000);
  gStyle->SetOptStat(0);
  TLatex latex;
  // latex.SetTextFont(43);
  // latex.SetTextSize(18);
  std::string label;

  c->Divide(2,2);
  c->cd(1);
  hx->Draw();
  // gPad->SetLogy();
  label = "pitch: " + std::to_string(Xpitch).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.8, (label).c_str());

  label = "inter: " + std::to_string(Xinter).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.76, (label).c_str());

  label = "#mu_{X}: " + std::to_string(fitFuncX->GetParameter(1)).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.72, (label).c_str());

  label = "#sigma_{X}: " + std::to_string(fitFuncX->GetParameter(2)).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.68, (label).c_str());

  c->cd(2);
  hy->Draw();
  // gPad->SetLogy();
  label = "pitch: " + std::to_string(Ypitch).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.8, (label).c_str());

  label = "inter: " + std::to_string(Yinter).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.76, (label).c_str());

  label = "#mu_{Y}: " + std::to_string(fitFuncY->GetParameter(1)).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.72, (label).c_str());

  label = "#sigma_{Y}: " + std::to_string(fitFuncY->GetParameter(2)).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.68, (label).c_str());
  
  c->cd(3);
  TF1 *fpol1 = new TF1("pol1", "pol1", meanydet-1.5, meanydet+1.5);
  prx->Fit(fpol1, "R");
  h2x->Draw("colz");
  prx->Draw("same");
  gPad->SetLogz();
  label = "slope="+ std::to_string(fpol1->GetParameter(1)).substr(0, 5);
  latex.DrawLatexNDC(0.15, 0.8, (label).c_str());

  c->cd(4);
  TF1 *fpoly = new TF1("poly", "pol1", meanxdet-1.5, meanxdet+1.5);
  pry->Fit(fpoly, "R");
  h2y->Draw("colz");
  pry->Draw("same");
  gPad->SetLogz();
  label = "slope="+ std::to_string(fpoly->GetParameter(1)).substr(0, 5);
  latex.DrawLatexNDC(0.15, 0.8, (label).c_str());

  c->Print(graphname.c_str(), "png");
}

void res3Dplot(TFile* res, std::string graphname){
  
  // gROOT->SetStyle("myStyle");
  // gROOT->ForceStyle();

  TNtupleD* nt = (TNtupleD*) res->Get("nt");

  double meanxdet = getMean(nt, "xdet",{-200, 200});
  double meanydet = getMean(nt, "ydet",{-200, 200});
  double stdx = getStdDev(nt, "xres",{-100, 100});
  double stdy = getStdDev(nt, "yres",{-100, 100});

  double meanresx = getMean(nt, "xres",{-200, 200});
  double meanresy = getMean(nt, "yres",{-200, 200});
  double meanstX = getMean(nt, "stX",{-200, 200});
  double meanstY = getMean(nt, "stY",{-200, 200});

  TH2F* h2xres = new TH2F("h2x", "2D Y strip residu distribution", 300, meanxdet-3, meanxdet+3, 300, meanydet-3, meanydet+3);
  h2xres->GetXaxis()->SetTitle("position x axis (mm)");
  h2xres->GetYaxis()->SetTitle("position y axis (mm)");
  h2xres->GetZaxis()->SetTitle("residue (mm)");
  h2xres->GetZaxis()->SetRangeUser(-1,1);

  TH2F* h2yres = new TH2F("h2y", "2D X strip residu distribution", 300, meanxdet-3, meanxdet+3, 300, meanydet-3, meanydet+3);
  h2yres->GetXaxis()->SetTitle("position x axis (mm)");
  h2yres->GetYaxis()->SetTitle("position y axis (mm)");
  h2yres->GetZaxis()->SetTitle("residue (mm)");
  h2yres->GetZaxis()->SetRangeUser(-1,1);

  double xdet, ydet, xres, yres;

  nt->ResetBranchAddresses();
  nt->SetBranchAddress("xdet", &xdet);
  nt->SetBranchAddress("ydet", &ydet);
  nt->SetBranchAddress("xres", &xres);
  nt->SetBranchAddress("yres", &yres);

  for(int i = 0; i < nt->GetEntries(); i++) {
    nt->GetEntry(i);
    h2xres->SetBinContent(h2xres->FindBin(xdet, ydet), xres);
    h2yres->SetBinContent(h2yres->FindBin(xdet, ydet), yres);
  }

  TCanvas *c = new TCanvas("c", "c", 1600,1000);
  gStyle->SetOptStat(0);
  gStyle->SetPalette(kTemperatureMap);
  c->Divide(2,1);
  c->cd(1);
  h2yres->Draw("colz");
  c->cd(2);
  h2xres->Draw("colz");
  c->Print(graphname.c_str(), "png");
  delete c;

  std::cout<<"stdx: "<<stdx<<" stdy: "<<stdy<<std::endl;
  std::cout<<"squared sum of stdx and stdy: "<<sqrt(stdx*stdx + stdy*stdy)<<std::endl;
}

void plotResidueClsize(TFile* res, std::string graphname){

  // gROOT->SetStyle("myStyle");
  // gROOT->ForceStyle();
  
  TNtupleD* nt = (TNtupleD*) res->Get("nt");
  int N = 4;

  double meanxdet = getMean(nt, "xdet",{-200, 200});
  double meanydet = getMean(nt, "ydet",{-200, 200});
  double stdx = getStdDev(nt, "xres",{-20, 20});
  double stdy = getStdDev(nt, "yres",{-20, 20});

  double meanresx = getMean(nt, "xres",{-200, 200});
  double meanresy = getMean(nt, "yres",{-200, 200});
  double meanstX = getMean(nt, "stX",{-200, 200});
  double meanstY = getMean(nt, "stY",{-200, 200});
  
  double meanchX = getMean(nt, "chX",{-200, 200});
  double meanchY = getMean(nt, "chY",{-200, 200});

  std::cout<<"meanxdet: "<<meanxdet<<" stdx: "<<stdx<<std::endl;
  TH1F* hx[3];
  TH1F* hy[3];
  TH2F* h2x[3];
  TH2F* h2y[3];

  TF1* fitFuncX[3];
  TF1* fitFuncY[3];

  THStack* hsx = new THStack("hsx", "residu X strips (track - centroid)");
  THStack* hsy = new THStack("hsy", "residu Y strips (track - centroid)");

  // gStyle->SetTextFont(43); // Set the font to Helvetica
  // gStyle->SetTextSize(20); // Set the font size to 0.05

  std::vector<int> color = {kBlue, kRed, kViolet, kBlack};

  for(int i=0; i<N; i++){
    hx[i] = new TH1F(Form("hx_%d",i), "residu X strips (track - centroid)", 300, meanresy-1.5*stdy, meanresy+1.5*stdy);
    hx[i]->GetXaxis()->SetTitle("residue on y axis (mm)");
    hx[i]->SetLineColor(color[i]);
    hy[i] = new TH1F(Form("hy_%d",i), "residu Y strips (track - centroid)", 300, meanresx-1.5*stdx, meanresx+1.5*stdx);
    hy[i]->GetXaxis()->SetTitle("residue on x axis (mm)");
    hy[i]->SetLineColor(color[i]);
    nt->Draw(Form("yres>>hx_%d",i), Form("Xclsize==%d",i+1));
    nt->Draw(Form("xres>>hy_%d",i), Form("Yclsize==%d",i+1));

    // Fit hx with a Gaussian function
    fitFuncX[i] = new TF1(Form("fitfuncX_%d",i), "gaus", meanresy-1.5*stdy, meanresy+1.5*stdy);
    fitFuncX[i]->SetParameters(0, stdy);
    fitFuncX[i]->SetLineColor(color[i]);
    hx[i]->Fit(fitFuncX[i], "R");
    hsx->Add(hx[i]);

    // Fit hy with a Gaussian function
    fitFuncY[i] = new TF1(Form("fitfuncY_%d",i), "gaus", meanresx-1.5*stdx, meanresx+1.5*stdx);
    fitFuncY[i]->SetParameters(0, stdx);
    fitFuncY[i]->SetLineColor(color[i]);
    hy[i]->Fit(fitFuncY[i], "R");
    hsy->Add(hy[i]);

    h2x[i] = new TH2F(Form("h2x_%d",i), "residu X strips vs y pos", 300, meanydet-3, meanydet+3, 200, meanresy-1.5*stdy, meanresy+1.5*stdy);
    h2x[i]->GetXaxis()->SetTitle("position y axis (mm)");
    h2x[i]->GetYaxis()->SetTitle("residue (mm)");
    h2x[i]->SetMarkerColor(color[i]);

    h2y[i] = new TH2F(Form("h2y_%d",i), "residu Y strips vs x pos", 300, meanxdet-3, meanxdet+3, 200, meanresx-1.5*stdx, meanresx+1.5*stdx);
    h2y[i]->GetXaxis()->SetTitle("position x axis (mm)");
    h2y[i]->GetYaxis()->SetTitle("residue (mm)");
    h2y[i]->SetMarkerColor(color[i]);

    nt->Draw(Form("yres:ydet>>h2x_%d",i), Form("Xclsize==%d",i+1));
    nt->Draw(Form("xres:xdet>>h2y_%d", i),Form("Yclsize==%d",i+1));

    // h2x[i] = new TH2F(Form("h2x_%d",i), "residu X strips vs y pos", 300, meanchX-6, meanchX+6, 200, meanresy-1.5*stdy, meanresy+1.5*stdy);
    // h2x[i]->GetXaxis()->SetTitle("channel centroid y axis (mm)");
    // h2x[i]->GetYaxis()->SetTitle("residue (mm)");
    // h2x[i]->SetMarkerColor(color[i]);

    // h2y[i] = new TH2F(Form("h2y_%d",i), "residu Y strips vs x pos", 300, meanchY-6, meanchY+6, 200, meanresx-1.5*stdx, meanresx+1.5*stdx);
    // h2y[i]->GetXaxis()->SetTitle("channel centroid x axis (mm)");
    // h2y[i]->GetYaxis()->SetTitle("residue (mm)");
    // h2y[i]->SetMarkerColor(color[i]);

    // nt->Draw(Form("yres:chX>>h2x_%d",i), Form("Xclsize==%d",i+1));
    // nt->Draw(Form("xres:chY>>h2y_%d", i),Form("Yclsize==%d",i+1));
  }

  TCanvas *c = new TCanvas("c", "c", 1600,1000);
  gStyle->SetOptStat(0);
  TLatex latex;
  // latex.SetTextFont(43);
  // latex.SetTextSize(18);
  std::string label;

  c->Divide(2,2);
  c->cd(1);
  hsx->Draw("nostack");
  label = "pitch: " + std::to_string(Xpitch).substr(0, 5);
  latex.DrawLatexNDC(0.65, 0.8, (label).c_str());

  label = "inter: " + std::to_string(Xinter).substr(0, 5);
  latex.DrawLatexNDC(0.65, 0.76, (label).c_str());

  for(int i=0; i<N; i++){
    latex.SetTextColor(color[i]);
    label = Form("size%d: #mu_{X}=%.3f; #sigma_{X}=%.3f", i+1, fitFuncX[i]->GetParameter(1), fitFuncX[i]->GetParameter(2));
    latex.DrawLatexNDC(0.65, 0.72-i*0.04, (label).c_str());
    latex.SetTextColor(kBlack);
  }

  c->cd(2);
  hsy->Draw("nostack");

  label = "pitch: " + std::to_string(Ypitch).substr(0, 5);
  latex.DrawLatexNDC(0.65, 0.8, (label).c_str());

  label = "inter: " + std::to_string(Yinter).substr(0, 5);
  latex.DrawLatexNDC(0.65, 0.76, (label).c_str());

  for(int i=0; i<N; i++){
    latex.SetTextColor(color[i]);
    label = Form("size%d: #mu_{Y}=%.3f; #sigma_{Y}=%.3f", i+1, fitFuncY[i]->GetParameter(1), fitFuncY[i]->GetParameter(2));
    latex.DrawLatexNDC(0.65, 0.72-i*0.04, (label).c_str());
    latex.SetTextColor(kBlack);
  }

  c->cd(3);
  h2x[1]->Draw("");
  h2x[2]->Draw("same");
  h2x[0]->Draw("same");
  h2x[3]->Draw("same");
  gPad->SetLogz();

  c->cd(4);
  h2y[1]->Draw("");
  h2y[2]->Draw("same");
  h2y[0]->Draw("same");
  h2y[3]->Draw("same");
  gPad->SetLogz();

  c->Print(graphname.c_str(), "png");
}

void plotResidueChannel(TFile* res, std::string graphname){
  
  // defStyle();

  TNtupleD* nt = (TNtupleD*) res->Get("nt");

  double meanxdet = getMean(nt, "chY");
  double meanydet = getMean(nt, "chX");
  double stdx = getStdDev(nt, "chresY");
  double stdy = getStdDev(nt, "chresX");

  double meanresx = getMean(nt, "chresY");
  double meanresy = getMean(nt, "chresX");
  double meanstX = getMean(nt, "stX");
  double meanstY = getMean(nt, "stY");

  double avg_std = (stdx+stdy)/2.;

  std::cout<<"meanxdet: "<<meanxdet<<" stdx: "<<stdx<<std::endl;

  TH1F* hx = new TH1F("hx", "residu X strips (track - centroid)", 300, meanresy-1.5*avg_std, meanresy+1.5*avg_std);
  hx->GetXaxis()->SetTitle("residue on y axis (channel nb)");
  TH1F* hy = new TH1F("hy", "residu Y strips (track - centroid)", 300, meanresx-1.5*avg_std, meanresx+1.5*avg_std);
  hy->GetXaxis()->SetTitle("residue on x axis (channel nb)");
  nt->Draw("chresX>>hx");
  nt->Draw("chresY>>hy");

  // Fit hx with a Gaussian function
  TF1* fitFuncX = new TF1("fitFuncX", "gaus", meanresy-2.*avg_std, meanresy+2.*avg_std);
  fitFuncX->SetParameters(0, stdy);
  hx->Fit(fitFuncX, "R");

  // Fit hy with a Gaussian function
  TF1* fitFuncY = new TF1("fitFuncY", "gaus", meanresx-2.*avg_std, meanresx+2.*avg_std);
  fitFuncY->SetParameters(0, stdx);
  hy->Fit(fitFuncY, "R");

  TH2F* h2x = new TH2F("h2x", "residu X strips vs y pos", 300, meanydet-4, meanydet+4, 300, meanresy-1.5*avg_std, meanresy+1.5*avg_std);
  h2x->GetXaxis()->SetTitle("position y axis (channel nb)");
  h2x->GetYaxis()->SetTitle("residue (channel nb)");

  TH2F* h2y = new TH2F("h2y", "residu Y strips vs x pos", 300, meanxdet-4, meanxdet+4, 300, meanresx-1.5*avg_std, meanresx+1.5*avg_std);
  h2y->GetXaxis()->SetTitle("position x axis (channel nb)");
  h2y->GetYaxis()->SetTitle("residue (channel nb)");
  nt->Draw("chresX:chX>>h2x");
  nt->Draw("chresY:chY>>h2y");

  TProfile* prx = new TProfile("prx", "residu X strips vs y pos", 300, meanydet-4, meanydet+4, meanresy-1.5*avg_std, meanresy+1.5*avg_std);
  TProfile* pry = new TProfile("pry", "residu Y strips vs x pos", 300, meanxdet-4, meanxdet+4, meanresx-1.5*avg_std, meanresx+1.5*avg_std);
  nt->Draw("chresX:chX>>prx");
  nt->Draw("chresY:chY>>pry");

  // gStyle->SetTextFont(43); // Set the font to Helvetica
  // gStyle->SetTextSize(20); // Set the font size to 0.05

  TCanvas *c = new TCanvas("c", "c", 1600,1000);
  gStyle->SetOptStat(0);
  TLatex latex;
  // latex.SetTextFont(43);
  // latex.SetTextSize(18);
  std::string label;

  c->Divide(2,2);
  c->cd(1);
  hx->Draw();
  // gPad->SetLogy();
  label = "pitch: " + std::to_string(Xpitch).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.8, (label).c_str());

  label = "inter: " + std::to_string(Xinter).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.76, (label).c_str());

  label = "#mu_{X}: " + std::to_string(fitFuncX->GetParameter(1)).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.72, (label).c_str());

  label = "#sigma_{X}: " + std::to_string(fitFuncX->GetParameter(2)).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.68, (label).c_str());

  c->cd(2);
  hy->Draw();
  // gPad->SetLogy();
  label = "pitch: " + std::to_string(Ypitch).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.8, (label).c_str());

  label = "inter: " + std::to_string(Yinter).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.76, (label).c_str());

  label = "#mu_{Y}: " + std::to_string(fitFuncY->GetParameter(1)).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.72, (label).c_str());

  label = "#sigma_{Y}: " + std::to_string(fitFuncY->GetParameter(2)).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.68, (label).c_str());
  
  c->cd(3);
  TF1 *fpol1 = new TF1("pol1", "pol1", meanydet-1.5, meanydet+1.5);
  prx->Fit(fpol1, "R");
  h2x->Draw("colz");
  prx->Draw("same");
  gPad->SetLogz();
  label = "slope="+ std::to_string(fpol1->GetParameter(1)).substr(0, 5);
  latex.DrawLatexNDC(0.15, 0.8, (label).c_str());

  c->cd(4);
  TF1 *fpoly = new TF1("poly", "pol1", meanxdet-1.5, meanxdet+1.5);
  pry->Fit(fpoly, "R");
  h2y->Draw("colz");
  pry->Draw("same");
  gPad->SetLogz();
  label = "slope="+ std::to_string(fpoly->GetParameter(1)).substr(0, 5);
  latex.DrawLatexNDC(0.15, 0.8, (label).c_str());

  c->Print(graphname.c_str(), "png");
}

void plotResidueSt(TFile* res, std::string graphname){
  
  // defStyle();

  TNtupleD* nt = (TNtupleD*) res->Get("nt");

  double meanxdet = getMean(nt, "stY");
  double meanydet = getMean(nt, "stX");
  double stdx = getStdDev(nt, "stresY");
  double stdy = getStdDev(nt, "stresX");

  double meanresx = getMean(nt, "stresY");
  double meanresy = getMean(nt, "stresX");
  double meanstX = getMean(nt, "stX");
  double meanstY = getMean(nt, "stY");

  double avg_std = (stdx+stdy)/2.;

  std::cout<<"meanxdet: "<<meanxdet<<" stdx: "<<stdx<<std::endl;

  TH1F* hx = new TH1F("hx", "residu X strips (track - centroid)", 300, meanresy-1.5*avg_std, meanresy+1.5*avg_std);
  hx->GetXaxis()->SetTitle("residue on y axis (strip nb)");
  TH1F* hy = new TH1F("hy", "residu Y strips (track - centroid)", 300, meanresx-1.5*avg_std, meanresx+1.5*avg_std);
  hy->GetXaxis()->SetTitle("residue on x axis (strip nb)");
  nt->Draw("stresX>>hx");
  nt->Draw("stresY>>hy");

  // Fit hx with a Gaussian function
  TF1* fitFuncX = new TF1("fitFuncX", "gaus", meanresy-2.*avg_std, meanresy+2.*avg_std);
  fitFuncX->SetParameters(0, stdy);
  hx->Fit(fitFuncX, "R");

  // Fit hy with a Gaussian function
  TF1* fitFuncY = new TF1("fitFuncY", "gaus", meanresx-2.*avg_std, meanresx+2.*avg_std);
  fitFuncY->SetParameters(0, stdx);
  hy->Fit(fitFuncY, "R");

  TH2F* h2x = new TH2F("h2x", "residu X strips vs y pos", 300, meanydet-4, meanydet+4, 300, meanresy-1.5*avg_std, meanresy+1.5*avg_std);
  h2x->GetXaxis()->SetTitle("position y axis (strip nb)");
  h2x->GetYaxis()->SetTitle("residue");

  TH2F* h2y = new TH2F("h2y", "residu Y strips vs x pos", 300, meanxdet-4, meanxdet+4, 300, meanresx-1.5*avg_std, meanresx+1.5*avg_std);
  h2y->GetXaxis()->SetTitle("position x axis (strip nb)");
  h2y->GetYaxis()->SetTitle("residue");
  nt->Draw("stresX:stX>>h2x");
  nt->Draw("stresY:stY>>h2y");

  TProfile* prx = new TProfile("prx", "residu X strips vs y pos", 300, meanydet-4, meanydet+4, meanresy-1.5*avg_std, meanresy+1.5*avg_std);
  TProfile* pry = new TProfile("pry", "residu Y strips vs x pos", 300, meanxdet-4, meanxdet+4, meanresx-1.5*avg_std, meanresx+1.5*avg_std);
  nt->Draw("stresX:stX>>prx");
  nt->Draw("stresY:stY>>pry");

  // gStyle->SetTextFont(43); // Set the font to Helvetica
  // gStyle->SetTextSize(20); // Set the font size to 0.05

  TCanvas *c = new TCanvas("c", "c", 1600,1000);
  gStyle->SetOptStat(0);
  TLatex latex;
  // latex.SetTextFont(43);
  // latex.SetTextSize(18);
  std::string label;

  c->Divide(2,2);
  c->cd(1);
  hx->Draw();
  // gPad->SetLogy();
  label = "pitch: " + std::to_string(Xpitch).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.8, (label).c_str());

  label = "inter: " + std::to_string(Xinter).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.76, (label).c_str());

  label = "#mu_{X}: " + std::to_string(fitFuncX->GetParameter(1)).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.72, (label).c_str());

  label = "#sigma_{X}: " + std::to_string(fitFuncX->GetParameter(2)).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.68, (label).c_str());

  c->cd(2);
  hy->Draw();
  // gPad->SetLogy();
  label = "pitch: " + std::to_string(Ypitch).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.8, (label).c_str());

  label = "inter: " + std::to_string(Yinter).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.76, (label).c_str());

  label = "#mu_{Y}: " + std::to_string(fitFuncY->GetParameter(1)).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.72, (label).c_str());

  label = "#sigma_{Y}: " + std::to_string(fitFuncY->GetParameter(2)).substr(0, 5);
  latex.DrawLatexNDC(0.75, 0.68, (label).c_str());
  
  c->cd(3);
  TF1 *fpol1 = new TF1("pol1", "pol1", meanydet-1.5, meanydet+1.5);
  prx->Fit(fpol1, "R");
  h2x->Draw("colz");
  prx->Draw("same");
  gPad->SetLogz();
  label = "slope="+ std::to_string(fpol1->GetParameter(1)).substr(0, 5);
  latex.DrawLatexNDC(0.15, 0.8, (label).c_str());

  c->cd(4);
  TF1 *fpoly = new TF1("poly", "pol1", meanxdet-1.5, meanxdet+1.5);
  pry->Fit(fpoly, "R");
  h2y->Draw("colz");
  pry->Draw("same");
  gPad->SetLogz();
  label = "slope="+ std::to_string(fpoly->GetParameter(1)).substr(0, 5);
  latex.DrawLatexNDC(0.15, 0.8, (label).c_str());

  c->Print(graphname.c_str(), "png");
}

int main(int argc, char const *argv[])
{

  std::string basedir = argv[0];
  basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";
  
  if(argc != 4){
    std::cerr << "Usage: " << argv[0] << " <detName> <banco.root> <mm.root>" << std::endl;
    return 1;
  }

  std::string detName = argv[1];
  std::string fnameBanco = argv[2];
  std::string fnameMM = argv[3];
  
  std::string mapName;
  if (detName.find("asa") != std::string::npos) {
    mapName = basedir + "../map/" + "asa_map.txt";
  } else if (detName.find("strip") != std::string::npos) {
    mapName = basedir + "../map/" + "strip_map.txt";
  } else if (detName.find("inter") != std::string::npos) {
    mapName = basedir + "../map/" + "inter_map.txt";
  } else {
    std::cerr << "Invalid detector name" << std::endl;
    return 1;
  }

  std::string run = fnameMM.substr(fnameMM.find("POS"), 5);
  std::string alignName = basedir + "../map/alignFiles/" + detName + "_" + run + ".txt";

  StripTable det(mapName, alignName);
  // StripTable det(mapName);
  std::string graphname = "residue_"+run+"_"+detName+""+"_test.png";
  std::string graphnamech = "residue_"+run+"_"+detName+""+"_channel_test.png";
  std::string graphnamest = "residue_"+run+"_"+detName+""+"_strip_test.png";
  std::string graphnameCl = "residue_"+run+"_"+detName+"_clsize_test"+".png";
  std::string graphname3D = "residue_"+run+"_"+detName+"_3D_test"+".png";
  
  std::string resfname = "residue_"+run+"_"+detName+"_residue"+".root";
  TFile* res = new TFile((resfname).c_str(), "recreate");
  std::cout<<"residue file: "<<resfname<<std::endl;
  residue(res, fnameBanco, fnameMM, det);
  std::cout<<"residue file: "<<resfname<<std::endl;
  
  // TFile* res = new TFile((resfname).c_str(), "open");
  defStyle();
  plotResidueSt(res, graphnamest);
  plotResidueChannel(res, graphnamech);
  plotResidue(res, graphname);
  plotResidueClsize(res, graphnameCl);
  res3Dplot(res, graphname3D);
  return 0;
}

//   double zpos = det.getZpos();

//   // double zpos = -785.6, Ty = -93.6933, Tx = 80.169; // POS16
//   // double zpos = -785.6, Tx = 23.8601, Ty = -10.7433, rot = -0.00634606; // POS05

//   // double zpos = -305.2, Ty = -94.365, Tx = 83.231; // POS16
//   // double zpos = -305.2, Ty = -10.518, Tx = 29.079;   //POS05
//   // double zpos = -785.6, Ty = -61.5856, Tx = 24.817; // POS13
//   // double zpos = -305.6, Ty = -63.382, Tx = 28.377; //POS13
  
//   // z pos on murwell strip: -305.2
//   // z pos of asa strip: -785.6
//   // residu(fnameBanco, fnameMM, det, -305.2, graphname);
  
//   TFile *fbanco = TFile::Open(fnameBanco.c_str(), "read");
//   TFile *fMM = TFile::Open(fnameMM.c_str(), "read");

//   TTreeReader MM("events", fMM);
//   TTreeReader banco("events", fbanco);

//   TTreeReaderValue< std::vector<cluster> > cls( MM, "clusters");
//   TTreeReaderValue< std::vector<banco::track> > tracks( banco, "tracks");

//   std::vector<float> xdet, ydet;
//   std::vector<float> xtrack, ytrack;

//   while( MM.Next() ){
//     bool isBanco = banco.Next();
//     if(!isBanco){
//       std::cout<<"WARNING: Missing banco event"<<std::endl; 
//       continue;
//     }

//     for(auto tr : *tracks){
//       if(tr.chi2x>1. or tr.chi2y>1.) continue;

//       double xdetTrack = tr.x0 + zpos*tr.mx;
//       double ydetTrack = tr.y0 + zpos*tr.my;
//       std::vector<cluster> clsX, clsY;

//       std::copy_if (cls->begin(), cls->end(), std::back_inserter(clsX),
//                   [](const cluster& c){return c.axis=='x';} );
//       std::copy_if (cls->begin(), cls->end(), std::back_inserter(clsY),
//                   [](const cluster& c){return c.axis=='y';} );
//       if(clsX.size() == 0 || clsY.size() == 0) continue;

//       auto maxX = *std::max_element(clsX.begin(), clsX.end(),
//                          [](const cluster& a,const cluster& b) { return a.size < b.size; });
//       auto maxY = *std::max_element(clsY.begin(), clsY.end(),
//                          [](const cluster& a,const cluster& b) { return a.size < b.size; });
      
//       // double clxpos = det.posY(maxY.stripCentroid)[0];
// 		  // double clypos = det.posX(maxX.stripCentroid)[1];
      
//       if(maxX.size < 2 or maxY.size < 2) continue;
		  
//       double xpos = det.posY(maxY.stripCentroid)[0];
// 		  double ypos = det.posX(maxX.stripCentroid)[1];

//       ydet.push_back(ypos);
//       ytrack.push_back(ydetTrack);
      
//       xdet.push_back(xpos );
//       xtrack.push_back(xdetTrack);
//     }
//   }
//   if(xtrack.size() < 10000) {
// 		std::cerr << "Error: Not enough events" << std::endl;
// 		return 1;
// 	}

//   if(banco.Next()) std::cout<<"WARNING: Missing MM event"<<std::endl;

//   // residueRel(det, xdet, ydet, xtrack, ytrack, "rel_"+graphname);
//   residueAbs(det, xdet, ydet, xtrack, ytrack, "abs_"+graphname);
// }

