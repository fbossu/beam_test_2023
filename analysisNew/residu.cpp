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
#include "analysis.h"


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

void residue(std::string resName, std::string fnameBanco, std::string fnameMM, StripTable det){

  TFile* res = new TFile((resName).c_str(), "recreate");

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
    if(n>3000) break;
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
      
      // for(int i=0; i<hitsY.size(); i++){
      //   // if(hitsY[i].strip==64) std::cout<<"hitY: "<<hitsY[i].strip<<" "<<hitsY[i].channel<<std::endl;
      // }
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
  res->Close();
}


std::vector<double> plotResidue(std::string resName, std::string graphname, double angleX, double angleY){
  
  // defStyle();
  
  TFile* res = new TFile((resName).c_str(), "open");
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

  if(angleX != 0) {
    avg_std = avg_std*1.5;
    meanresy += 1.5;
    meanresx += 1.5;
  }

  std::cout<<"meanxdet: "<<meanxdet<<" stdx: "<<stdx<<std::endl;

  TH1F* hx = new TH1F("hx", "Residue X strips (track - centroid)", 50, meanresy-1.5*avg_std, meanresy+1.5*avg_std);
  hx->GetXaxis()->SetTitle("residue on y axis (mm)");
  TH1F* hy = new TH1F("hy", "Residue Y strips (track - centroid)", 50, meanresx-1.5*avg_std, meanresx+1.5*avg_std);
  hy->GetXaxis()->SetTitle("residue on x axis (mm)");
  // nt->Draw("yres>>hx");
  // nt->Draw("xres>>hy");
  nt->Draw(Form("cos(%f)*yres+sin(%f)*ydet>>hx", angleX, angleX));
  nt->Draw(Form("cos(%f)*xres+sin(%f)*xdet>>hy", angleY, angleY));

  // Fit hx with a Gaussian function
  TF1* fitFuncX = new TF1("fitFuncX", "gaus", meanresy-2.*avg_std, meanresy+2.*avg_std);
  fitFuncX->SetParameters(0, stdy);
  hx->Fit(fitFuncX, "R");

  // Fit hy with a Gaussian function
  TF1* fitFuncY = new TF1("fitFuncY", "gaus", meanresx-2.*avg_std, meanresx+2.*avg_std);
  fitFuncY->SetParameters(0, stdx);
  hy->Fit(fitFuncY, "R");

  TH2F* h2x = new TH2F("h2x", "Residue X strips vs y pos", 300, meanydet-4, meanydet+4, 300, meanresy-1.5*avg_std, meanresy+1.5*avg_std);
  h2x->GetXaxis()->SetTitle("position y axis (mm)");
  h2x->GetYaxis()->SetTitle("residue (mm)");

  TH2F* h2y = new TH2F("h2y", "Residue Y strips vs x pos", 300, meanxdet-4, meanxdet+4, 300, meanresx-1.5*avg_std, meanresx+1.5*avg_std);
  h2y->GetXaxis()->SetTitle("position x axis (mm)");
  h2y->GetYaxis()->SetTitle("residue (mm)");
  // nt->Draw("yres:ydet>>h2x");
  // nt->Draw("xres:xdet>>h2y");
  nt->Draw(Form("cos(%f)*yres+sin(%f)*ydet:-sin(%f)*yres+cos(%f)*ydet>>h2x",angleX,angleX,angleX,angleX));
  nt->Draw(Form("cos(%f)*xres+sin(%f)*xdet:-sin(%f)*xres+cos(%f)*xdet>>h2y",angleY,angleY,angleY,angleY));

  TProfile* prx = new TProfile("prx", "residu X strips vs y pos", 300, meanydet-4, meanydet+4, meanresy-1.5*avg_std, meanresy+1.5*avg_std);
  TProfile* pry = new TProfile("pry", "residu Y strips vs x pos", 300, meanxdet-4, meanxdet+4, meanresx-1.5*avg_std, meanresx+1.5*avg_std);
  // nt->Draw("yres:ydet>>prx");
  // nt->Draw("xres:xdet>>pry");
  nt->Draw(Form("cos(%f)*yres+sin(%f)*ydet:-sin(%f)*yres+cos(%f)*ydet>>prx",angleX,angleX,angleX,angleX));
  nt->Draw(Form("cos(%f)*xres+sin(%f)*xdet:-sin(%f)*xres+cos(%f)*xdet>>pry",angleY,angleY,angleY,angleY));

  // gStyle->SetTextFont(43); // Set the font to Helvetica
  // gStyle->SetTextSize(20); // Set the font size to 0.05

  TCanvas *c = new TCanvas("c", "c", 1600,1200);
  // gStyle->SetOptStat(0);
  TLatex latex;
  // latex.SetTextFont(43);
  // latex.SetTextSize(18);
  std::string label;

  c->Divide(2,2);
  c->cd(1);
  hx->Draw();
  // gPad->SetLogy();
  label = "pitch: " + std::to_string(Xpitch).substr(0, 5);
  latex.DrawLatexNDC(0.72, 0.8, (label).c_str());

  label = "inter: " + std::to_string(Xinter).substr(0, 5);
  latex.DrawLatexNDC(0.72, 0.76, (label).c_str());
  label = "#mu_{X}: " + std::to_string(fitFuncX->GetParameter(1)).substr(0, 5);
  latex.DrawLatexNDC(0.72, 0.72, (label).c_str());

  label = "#sigma_{X}: " + std::to_string(fitFuncX->GetParameter(2)).substr(0, 5) + "#pm " + std::to_string(fitFuncX->GetParError(2)).substr(0, 6);
  latex.DrawLatexNDC(0.72, 0.68, (label).c_str());

  c->cd(2);
  hy->Draw();
  // gPad->SetLogy();
  label = "pitch: " + std::to_string(Ypitch).substr(0, 5);
  latex.DrawLatexNDC(0.72, 0.8, (label).c_str());

  label = "inter: " + std::to_string(Yinter).substr(0, 5);
  latex.DrawLatexNDC(0.72, 0.76, (label).c_str());

  label = "#mu_{Y}: " + std::to_string(fitFuncY->GetParameter(1)).substr(0, 5);
  latex.DrawLatexNDC(0.72, 0.72, (label).c_str());

  label = "#sigma_{Y}: " + std::to_string(fitFuncY->GetParameter(2)).substr(0, 5)+ "#pm " + std::to_string(fitFuncY->GetParError(2)).substr(0, 6);
  latex.DrawLatexNDC(0.72, 0.68, (label).c_str());
  
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
  res->Close();


  double angleXout = atan(fpol1->GetParameter(1));
  double angleYout = atan(fpoly->GetParameter(1));

  return { fitFuncX->GetParameter(2), fitFuncY->GetParameter(2), angleXout, angleYout };
}



void plotResidueBanco(std::string resName, std::string graphname){
  
  // defStyle();
  
  TFile* res = new TFile((resName).c_str(), "open");
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

  TH1F* hx = new TH1F("hx", "Residue X strips (track - centroid)", 300, meanresy-1.5*avg_std, meanresy+1.5*avg_std);
  hx->GetXaxis()->SetTitle("residue on y axis (mm)");
  TH1F* hy = new TH1F("hy", "Residue Y strips (track - centroid)", 300, meanresx-1.5*avg_std, meanresx+1.5*avg_std);
  hy->GetXaxis()->SetTitle("residue on x axis (mm)");
  // nt->Draw("yres>>hx");
  // nt->Draw("xres>>hy");
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

  TH2F* h2x = new TH2F("h2x", "Residue X strips vs y pos", 300, meanydet-4, meanydet+4, 300, meanresy-1.5*avg_std, meanresy+1.5*avg_std);
  h2x->GetXaxis()->SetTitle("position y axis (mm)");
  h2x->GetYaxis()->SetTitle("residue (mm)");

  TH2F* h2y = new TH2F("h2y", "Residue Y strips vs x pos", 300, meanxdet-4, meanxdet+4, 300, meanresx-1.5*avg_std, meanresx+1.5*avg_std);
  h2y->GetXaxis()->SetTitle("position x axis (mm)");
  h2y->GetYaxis()->SetTitle("residue (mm)");
  // nt->Draw("yres:ydet>>h2x");
  // nt->Draw("xres:xdet>>h2y");
  nt->Draw("yres:ytrack>>h2x");
  nt->Draw("xres:xtrack>>h2y");

  TProfile* prx = new TProfile("prx", "residu X strips vs y pos", 300, meanydet-4, meanydet+4, meanresy-1.5*avg_std, meanresy+1.5*avg_std);
  TProfile* pry = new TProfile("pry", "residu Y strips vs x pos", 300, meanxdet-4, meanxdet+4, meanresx-1.5*avg_std, meanresx+1.5*avg_std);
  // nt->Draw("yres:ydet>>prx");
  // nt->Draw("xres:xdet>>pry");
  nt->Draw("yres:ytrack>>prx");
  nt->Draw("xres:xtrack>>pry");

  // gStyle->SetTextFont(43); // Set the font to Helvetica
  // gStyle->SetTextSize(20); // Set the font size to 0.05

  TCanvas *c = new TCanvas("c", "c", 1600,1200);
  // gStyle->SetOptStat(0);
  TLatex latex;
  // latex.SetTextFont(43);
  // latex.SetTextSize(18);
  std::string label;

  c->Divide(2,2);
  c->cd(1);
  hx->Draw();
  // gPad->SetLogy();
  label = "pitch: " + std::to_string(Xpitch).substr(0, 5);
  latex.DrawLatexNDC(0.72, 0.8, (label).c_str());

  label = "inter: " + std::to_string(Xinter).substr(0, 5);
  latex.DrawLatexNDC(0.72, 0.76, (label).c_str());
  label = "#mu_{X}: " + std::to_string(fitFuncX->GetParameter(1)).substr(0, 5);
  latex.DrawLatexNDC(0.72, 0.72, (label).c_str());

  label = "#sigma_{X}: " + std::to_string(fitFuncX->GetParameter(2)).substr(0, 5);
  latex.DrawLatexNDC(0.72, 0.68, (label).c_str());

  c->cd(2);
  hy->Draw();
  // gPad->SetLogy();
  label = "pitch: " + std::to_string(Ypitch).substr(0, 5);
  latex.DrawLatexNDC(0.72, 0.8, (label).c_str());

  label = "inter: " + std::to_string(Yinter).substr(0, 5);
  latex.DrawLatexNDC(0.72, 0.76, (label).c_str());

  label = "#mu_{Y}: " + std::to_string(fitFuncY->GetParameter(1)).substr(0, 5);
  latex.DrawLatexNDC(0.72, 0.72, (label).c_str());

  label = "#sigma_{Y}: " + std::to_string(fitFuncY->GetParameter(2)).substr(0, 5);
  latex.DrawLatexNDC(0.72, 0.68, (label).c_str());
  
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
  res->Close();
}




void res3Dplot(std::string resName, std::string graphname){
  
  // gROOT->SetStyle("myStyle");
  // gROOT->ForceStyle();
  TFile* res = new TFile((resName).c_str(), "open");
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

  TCanvas *c = new TCanvas("c", "c", 1600,1200);
  // gStyle->SetOptStat(0);
  // gStyle->SetPalette(kTemperatureMap);
  c->Divide(2,1);
  c->cd(1);
  h2yres->Draw("colz");
  c->cd(2);
  h2xres->Draw("colz");
  c->Print(graphname.c_str(), "png");
  delete c;

  std::cout<<"stdx: "<<stdx<<" stdy: "<<stdy<<std::endl;
  std::cout<<"squared sum of stdx and stdy: "<<sqrt(stdx*stdx + stdy*stdy)<<std::endl;
  res->Close();
}

void plotResidueClsize(std::string resName, std::string graphname){

  // gROOT->SetStyle("myStyle");
  // gROOT->ForceStyle();
  
  TFile* res = new TFile((resName).c_str(), "open");
  TNtupleD* nt = (TNtupleD*) res->Get("nt");
  int N = 3;

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

  std::vector<int> color = {kBlue, kRed, kViolet, kOrange-3};

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

  TCanvas *c = new TCanvas("c", "c", 1600,1200);
  // gStyle->SetOptStat(0);
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
    label = Form("size%d: #sigma_{X}=%.3f", i+1, fitFuncX[i]->GetParameter(2));
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
    label = Form("size%d: #sigma_{Y}=%.3f", i+1, fitFuncY[i]->GetParameter(2));
    latex.DrawLatexNDC(0.65, 0.72-i*0.04, (label).c_str());
    latex.SetTextColor(kBlack);
  }

  c->cd(3);
  h2x[1]->Draw("");
  h2x[2]->Draw("same");
  h2x[0]->Draw("same");
  // h2x[3]->Draw("same");
  gPad->SetLogz();

  c->cd(4);
  h2y[1]->Draw("");
  h2y[2]->Draw("same");
  h2y[0]->Draw("same");
  // h2y[3]->Draw("same");
  gPad->SetLogz();

  c->Print(graphname.c_str(), "png");
  res->Close();
}

void plotResidueChannel(std::string resName, std::string graphname){
  
  // defStyle();
  TFile* res = new TFile((resName).c_str(), "open");
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

  TCanvas *c = new TCanvas("c", "c", 1600,1200);
  // gStyle->SetOptStat(0);
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
  res->Close();
}

void plotResidueSt(std::string resName, std::string graphname){
  
  // defStyle();

  TFile* res = new TFile((resName).c_str(), "open");
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

  TCanvas *c = new TCanvas("c", "c", 1600,1200);
  // gStyle->SetOptStat(0);
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
  res->Close();
}


std::vector<double> ResiduePlotAll(StripTable det, std::string fnameBanco, std::string fnameMM, std::string prefix){
  std::string graphname      = prefix+".png";
  std::string graphnameRot   = prefix+"_rot.png";
  std::string graphnamech    = prefix+"_channel.png";
  std::string graphnamest    = prefix+"_strip.png";
  std::string graphnameCl    = prefix+"_clsize"+".png";
  std::string graphname3D    = prefix+"_3D"+".png";
  std::string graphnameBanco = prefix+"_Banco"+".png";

  std::string resName = prefix+"_residueFile.root";

  residue(resName, fnameBanco, fnameMM, det);
  std::vector<double> vect = plotResidue(resName, graphname);
  // std::vector<double> vectRot = plotResidue(resName, graphnameRot, -vect[2], -vect[3]);
  plotResidueBanco(resName, graphnameBanco);
  // plotResidueSt(resName, graphnamest);
  // plotResidueChannel(resName, graphnamech);
  plotResidueClsize(resName, graphnameCl);
  // res3Dplot(resName, graphname3D);
  return vect;
}