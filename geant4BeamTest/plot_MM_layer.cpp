#include <string>

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TF1.h"
#include "TF2.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TTreeReader.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TGraphErrors.h"
#include "TLatex.h"
#include "TFitResult.h"
#include "TProfile.h"

#include <Math/Transform3D.h>
#include <Math/Translation3D.h>
#include <Math/Rotation3D.h>
#include <Math/Vector3D.h>
#include <Math/RotationZYX.h>
#include <Math/Point3D.h>
#include <random>

struct track{
  double x0 = -1;
  double y0 = -1;
  double mx = -1;
  double my = -1;
  double chi2x = -1;
  double chi2y = -1;
  double clsize = -1;
};

struct cluster{
  double x = -999;
  double y = -999;
  double z = -999;
  double edep = -1;
  double clsizex = -999;
  double clsizey = -999;
  double chargex = -999;
  double chargey = -999;
};

double checkOverlap(double R, double Xc, double Yc,
                         double Xr, double Yr, double hx, double hy){
 
    // Find the nearest point on the rectangle to the center of the circle
    double Xn = std::max(Xr-hx/2., std::min(Xc, Xr+hy/2.));
    double Yn = std::max(Yr-hy/2., std::min(Yc, Yr+hy/2.));
    return R - sqrt(pow(Xn-Xc,2) + pow(Yn-Yc,2));
} 

track trackingBanco(const std::vector<std::vector<double>>& ladderPos, const std::vector<double>& ladderEdep, bool smear=true){

  TGraphErrors gry;
  TGraphErrors grx;
  double pitchy = 0.02688; // mm
  double pitchx = 0.02924; // mm
  std::vector<std::vector<double>> closePix(9, std::vector<double>(2, 0.));
  track trk;

  int index = -1;
  for( auto pos : ladderPos ){
    index++;
    if( pos[0]<-900 ) return trk;
    double x = 0, y = 0;
    if(smear){
      double diff = 0.0015*ladderEdep[index];
      double offset = 0.002; // dead edge
      int cX = std::floor(pos[0]/pitchx);
      int cY = std::floor(pos[1]/pitchy);
      for(int i=0; i<3; i++){
        for(int j=0; j<3; j++){
          closePix[i*3+j][0] = (cX-1+i)*pitchx + pitchx/2.;
          closePix[i*3+j][1] = (cY-1+j)*pitchy + pitchy/2.;
        }
      }

      int clsize = 0;
      for(int i=0; i<closePix.size(); i++){
        int xindex = i/3-1;
        int yindex = i%3-1;
        double radiusOverlap = checkOverlap(diff/2., pos[0], pos[1], closePix[i][0], closePix[i][1], pitchx, pitchy);
        if( radiusOverlap > 0 ){
          if( xindex!=0 && yindex!=0 && radiusOverlap < offset ) continue;
          x += closePix[i][0];
          y += closePix[i][1];
          clsize++;
        }
      }
      trk.clsize = clsize;
      x /= clsize;
      y /= clsize;
    }
    else{
      x = pos[0];
      y = pos[1];
    }
    grx.SetPoint(grx.GetN(),pos[2], x);
    gry.SetPoint(gry.GetN(),pos[2], y);
    grx.SetPointError(grx.GetN()-1,0,0.028/sqrt(12));
    gry.SetPointError(gry.GetN()-1,0,0.028/sqrt(12));
  }

  auto ptrx = grx.Fit("pol1","Q0S");
  auto ptry = gry.Fit("pol1","Q0S");

  if( (int)ptrx==0 && (int)ptry==0){
    trk.x0 = ptrx->Parameter(0);
    trk.mx = ptrx->Parameter(1);
    trk.chi2x = ptrx->Chi2()/ptrx->Ndf();

    trk.y0 = ptry->Parameter(0);
    trk.my = ptry->Parameter(1);
    trk.chi2y = ptry->Chi2()/ptry->Ndf();
  }
  return trk;
}


cluster MMclusterStrip(const std::vector<double>& pos, double edep, double pitch, double interRatio, double threshold, double sigma){

    double inter = pitch - pitch*interRatio;
    double minLim = -50.;
    double maxLim =  50.;

    cluster cl;
    double x = 0., y = 0., z = 0.;
    double clsizex = 0., clsizey = 0.;
    double chargex = 0., chargey = 0.;
    
    TF2* f = new TF2("f","[0]*TMath::Gaus(x,[1],[2],true)*TMath::Gaus(y,[3],[4],true)", minLim, maxLim, minLim, maxLim);
    f->SetParameters(edep, pos[0], sigma, pos[1], sigma);

    for(int i=0; i<(maxLim-minLim)/pitch; i++){
      double intx = f->Integral(minLim+i*pitch, minLim+(i+1)*pitch, minLim, maxLim, 1e-5);
      double inty = f->Integral(minLim, maxLim, minLim+i*pitch+inter/2., minLim+(i+1)*pitch-inter/2., 1e-5);
      if(intx>threshold){
        x += (minLim+(i+0.5)*pitch)*intx;
        chargex += intx;
        clsizex++;
      }
      if(inty>threshold){
        y += (minLim+(i+0.5)*pitch)*inty;
        chargey += inty;
        clsizey++;
      }
    }

    if(clsizex>0.) cl.x = x/chargex;
    if(clsizey>0.) cl.y = y/chargey;
    cl.z = pos[2];
    cl.edep = edep;
    cl.clsizex = clsizex;
    cl.clsizey = clsizey;
    cl.chargex = chargex;
    cl.chargey = chargey;
    // std::cout<<cl.x<<" "<<cl.y<<" "<<cl.z<<" "<<cl.edep<<" "<<cl.clsizex<<" "<<cl.clsizey<<" "<<cl.chargex<<" "<<cl.chargey<<std::endl;
    delete f;
    return cl;
}


void plotTracks(std::string fnameBanco){

    TFile *fbanco = TFile::Open(fnameBanco.c_str(), "read");
    TTreeReader reader("eic", fbanco);
    TTreeReaderValue<std::vector<double>> Lpos0( reader, "Lpos0");
    TTreeReaderValue<std::vector<double>> Lpos1( reader, "Lpos1");
    TTreeReaderValue<std::vector<double>> Lpos2( reader, "Lpos2");
    TTreeReaderValue<std::vector<double>> Lpos3( reader, "Lpos3");
    TTreeReaderValue<double> Ledep0( reader, "Ledep0");
    TTreeReaderValue<double> Ledep1( reader, "Ledep1");
    TTreeReaderValue<double> Ledep2( reader, "Ledep2");
    TTreeReaderValue<double> Ledep3( reader, "Ledep3");
    
    TH2F* h2tr = new TH2F("h2tr", "banco", 200, -2.5, 2.5, 200, -2.5, 2.5);
    h2tr->SetXTitle("x (mm)");
    h2tr->SetYTitle("y (mm)");

    TH1F* h1trx = new TH1F("h1trx", "x0", 200, -2, 2);
    h1trx->SetXTitle("x (mm)");
    TH1F* h1try = new TH1F("h1try", "y0", 200, -2, 2);
    h1try->SetXTitle("y (mm)");

    TH1F* h1trmx = new TH1F("h1trmx", "mx", 200, -0.005, 0.005);
    h1trmx->SetXTitle("mx");
    TH1F* h1trmy = new TH1F("h1trmy", "my", 200, -0.005, 0.005);
    h1trmy->SetXTitle("my");

    TH1F* h1trchi2x = new TH1F("h1trchi2x", "chi2x", 100, 0., 5.);
    h1trchi2x->SetXTitle("chi2x");
    TH1F* h1trchi2y = new TH1F("h1trchi2y", "chi2y", 100, 0., 5.);
    h1trchi2y->SetXTitle("chi2y");
    TH1F* h1clsize = new TH1F("h1clsize", "clsize", 11, -0.5, 10.5);
    h1trchi2y->SetXTitle("clsize");

    double z = 0;
    // double z= 305.6;

    while(reader.Next()){
      track trk = trackingBanco({*Lpos0, *Lpos1, *Lpos2, *Lpos3}, {*Ledep0, *Ledep1, *Ledep2, *Ledep3});
      if( trk.chi2x < 0. || trk.chi2y< 0. ) continue;
      // if(*chi2x>1. or *chi2y>1) continue;
      h2tr->Fill(trk.x0 + z*trk.mx, trk.y0 + z*trk.my);
      h1trx->Fill(trk.x0 + z*trk.mx);
      h1try->Fill(trk.y0 + z*trk.my);
      h1trmx->Fill(trk.mx);
      h1trmy->Fill(trk.my);
      h1trchi2x->Fill(trk.chi2x);
      h1trchi2y->Fill(trk.chi2y);
      h1clsize->Fill(trk.clsize);
    }
  
    TCanvas *c2 = new TCanvas("c2", "c2", 1000,1000);
    h2tr->Draw("colz");
    // gPad->SetLogz();
    c2->Print("tracks_ladder0.png", "png");

    TLatex latex;
    latex.SetTextSize(0.035);
    std::string label;

    TCanvas *c3 = new TCanvas("c3", "c3", 1600,1000);
    c3->Divide(2,2);
    c3->cd(1);
    h1trx->Fit("gaus");
    h1trx->Draw();
    label = "#sigma_{x}: "+ std::to_string(h1trx->GetFunction("gaus")->GetParameter(2)).substr(0, 5)+"mm";
    latex.DrawLatexNDC(0.14, 0.85, (label).c_str());

    c3->cd(2);
    h1try->Fit("gaus");
    h1try->Draw();
    label = "#sigma_{y}: "+ std::to_string(h1try->GetFunction("gaus")->GetParameter(2)).substr(0, 5)+"mm";
    latex.DrawLatexNDC(0.14, 0.85, (label).c_str());

    c3->cd(3);
    h1trmx->Fit("gaus");
    h1trmx->Draw();
    label = "#sigma_{mx}: "+ std::to_string(h1trmx->GetFunction("gaus")->GetParameter(2)).substr(0, 5);
    latex.DrawLatexNDC(0.14, 0.85, (label).c_str());

    c3->cd(4);
    h1trmy->Fit("gaus");
    h1trmy->Draw();
    label = "#sigma_{my}: "+ std::to_string(h1trmy->GetFunction("gaus")->GetParameter(2)).substr(0, 5);
    latex.DrawLatexNDC(0.14, 0.85, (label).c_str());
    c3->Print("tracksXY_ladder0.png", "png");

    TCanvas *c4 = new TCanvas("c4", "c4", 1600,1000);
    c4->Divide(2,2);
    c4->cd(1);
    h1trchi2x->Draw();
    c4->cd(2);
    h1trchi2y->Draw();
    c4->cd(3);
    h1clsize->Draw();
    c4->Print("tracksChi2.png", "png");

    fbanco->Close();
}

ROOT::Math::XYZPoint rotation(double posx, double posy, double posz){

  ROOT::Math::Rotation3D rot(ROOT::Math::RotationZYX(-0.05, 0.0, 0.)); // rotation around z, y, x
  ROOT::Math::Translation3D trl(0., 0., posz);
  ROOT::Math::Transform3D trans = ROOT::Math::Transform3D(rot, trl);

  ROOT::Math::XYZPoint pdet(posx, posy, 0.);
	ROOT::Math::XYZPoint pr = trans(pdet);
	return pr;
}


void plot_all(std::string fname, double pitch, double inter, double thRatio, double sigma){

  TFile *fin = TFile::Open(fname.c_str(), "read");
  TTree* tree = (TTree*)fin->Get("eic");
  TH1F* hedp = new TH1F("hedep", "hedep", 300, 0., 5.);
  tree->Draw("MMedep0>>hedep");

  double threshold = 0.;
  double prob = 0.5;
  hedp->GetQuantiles(1, &threshold, &prob);

  std::cout<<"pitch="<<pitch<<", inter="<<inter<<"*pitch, medianEdep="<<threshold;
  threshold *= thRatio;
  std::cout<<", threshold="<<threshold<<std::endl;
  delete tree, hedp;
  
  TTreeReader reader("eic", fin);
  TTreeReaderValue<std::vector<double>> Lpos0( reader, "Lpos0");
  TTreeReaderValue<std::vector<double>> Lpos1( reader, "Lpos1");
  TTreeReaderValue<std::vector<double>> Lpos2( reader, "Lpos2");
  TTreeReaderValue<std::vector<double>> Lpos3( reader, "Lpos3");
  TTreeReaderValue<double> Ledep0( reader, "Ledep0");
  TTreeReaderValue<double> Ledep1( reader, "Ledep1");
  TTreeReaderValue<double> Ledep2( reader, "Ledep2");
  TTreeReaderValue<double> Ledep3( reader, "Ledep3");  
  TTreeReaderValue<std::vector<double>> MMpos0( reader, "MMpos0");
  TTreeReaderValue<std::vector<double>> MMpos1( reader, "MMpos1");
  TTreeReaderValue<std::vector<double>> MMpos2( reader, "MMpos2");
  TTreeReaderValue<std::vector<double>> MMpos3( reader, "MMpos3");
  TTreeReaderValue<std::vector<double>> MMpos4( reader, "MMpos4");
  TTreeReaderValue<double> MMedep0( reader, "MMedep0");
  TTreeReaderValue<double> MMedep1( reader, "MMedep1");
  TTreeReaderValue<double> MMedep2( reader, "MMedep2");
  TTreeReaderValue<double> MMedep3( reader, "MMedep3");
  TTreeReaderValue<double> MMedep4( reader, "MMedep4");


  auto start = (fname.find("/")==std::string::npos)?0 : fname.find("/")+1;
  std::string prefix = "plots/" + fname.substr( start, fname.find(".root")-start) + Form("_p%.2f_i%.2f_t%.2f_s%.2f", pitch, inter, thRatio, sigma);
  std::string graphname = prefix + ".png";
  std::string graphnameX = prefix + "_X.png";
  std::string graphnameY = prefix + "_Y.png";
  
  std::string graphnameResX = prefix + "_resX.png";
  std::string graphnameResY = prefix + "_resY.png";

  std::string graphnameResPosX = prefix + "_ResPosX.png";
  std::string graphnameResPosY = prefix + "_ResPosY.png";

  TH2F* h[5];
  TH2F* hresposX[5];
  TH2F* hresposY[5];

  TH1F* hx[5];
  TH1F* hy[5];
  TH1F* hresX[5];
  TH1F* hresY[5];
  TH1F* h1clsizex[5];
  TH1F* h1clsizey[5];

  double zavg[5] = {0, 0, 0, 0, 0};
  double sigmaX[5] = {0, 0, 0, 0, 0};

  for( int i=0; i<5; i++){
    std::string label = "Layer"+std::to_string(i);
    double resW = 2.;
    h[i] = new TH2F(("h"+label).c_str(), label.c_str(), 150, -3, 3, 150, -3, 3);
    h[i]->SetXTitle("x position [mm]");
    h[i]->SetYTitle("y position [mm]");

    hresposX[i] = new TH2F(("hresposX"+label).c_str(), label.c_str(), 300, -4, 4, 300, -resW, resW);
    hresposX[i]->SetXTitle("x position [mm]");
    hresposX[i]->SetYTitle("residue [mm]");

    hresposY[i] = new TH2F(("hresposY"+label).c_str(), label.c_str(), 300, -4, 4, 300, -resW, resW);
    hresposY[i]->SetXTitle("y position [mm]");
    hresposY[i]->SetYTitle("residue [mm]");

    hx[i] = new TH1F(("hx"+label).c_str(), label.c_str(), 1000, -5, 5);
    hx[i]->SetXTitle("x position [mm]");
    hy[i] = new TH1F(("hy"+label).c_str(), label.c_str(), 1000, -5, 5);
    hy[i]->SetXTitle("y position [mm]");

    hresX[i] = new TH1F(("resX"+label).c_str(), ("Residue track - centroid "+label).c_str(), 200, -resW, resW);
    hresX[i]->SetXTitle("x position [mm]");
    hresY[i] = new TH1F(("resY"+label).c_str(), ("Residue track - centroid "+label).c_str(), 200, -resW, resW);
    hresY[i]->SetXTitle("y position [mm]");

    h1clsizex[i] = new TH1F(("h1clsizex"+label).c_str(), label.c_str(), 11, -0.5, 10.5);
    h1clsizex[i]->SetXTitle("clsize");
    h1clsizey[i] = new TH1F(("h1clsizey"+label).c_str(), label.c_str(), 11, -0.5, 10.5);
    h1clsizey[i]->SetXTitle("clsize");
  }

  while(reader.Next()){
    std::vector<std::vector<double>> MMpos = {*MMpos0, *MMpos1, *MMpos2, *MMpos3, *MMpos4};
    std::vector<double> MMedep = {*MMedep0, *MMedep1, *MMedep2, *MMedep3, *MMedep4};

    track trk = trackingBanco({*Lpos0, *Lpos1, *Lpos2, *Lpos3}, {*Ledep0, *Ledep1, *Ledep2, *Ledep3});
    if( trk.chi2x < 0. || trk.chi2y < 0. ) continue;
    if( trk.chi2x > 1. || trk.chi2y > 1. ) continue;
    
    for( int i=0; i<5; i++){

        if(MMpos[i][0]<-900 || MMpos[i][1]<-900) continue;
        cluster cl = MMclusterStrip(MMpos[i], MMedep[i], pitch, inter, threshold, sigma);

        h[i]->Fill(cl.x, cl.y);
        hx[i]->Fill(cl.x);
        hy[i]->Fill(cl.y);
        zavg[i] += cl.z;

        h1clsizex[i]->Fill(cl.clsizex);
        h1clsizey[i]->Fill(cl.clsizey);

        double xres = (trk.x0 + cl.z * trk.mx) - cl.x;
        double yres = (trk.y0 + cl.z * trk.my) - cl.y;

        hresX[i]->Fill(xres);
        hresY[i]->Fill(yres);

        hresposX[i]->Fill(cl.x, xres);
        hresposY[i]->Fill(cl.y, yres);
        
        // hresposX[i]->Fill((*x0 + MMpos[i][j+2] * *mx), xres);
        // hresposY[i]->Fill((*y0 + MMpos[i][j+2] * *my), yres);
      }
    }

  TLatex latex;
  latex.SetTextFont(43);
  latex.SetTextSize(25);

  std::string label;

  TCanvas *c2 = new TCanvas("c2", "c2", 1600,1200);
  c2->Divide(3,2, 0.01, 0.01);
  for(int i=0; i<5; i++){
    c2->cd(i+1);
    hx[i]->Fit("gaus");
    hx[i]->Draw();

    label = "z="+ std::to_string(zavg[i]/h[i]->GetEntries()).substr(0, 5)+" mm";
    latex.DrawLatexNDC(0.15, 0.8, (label).c_str());
  }
  c2->cd(6);
  h1clsizex[0]->Draw();
  c2->Print(graphnameX.c_str(), "png");

  TCanvas *c2Y = new TCanvas("c2Y", "c2Y", 1600,1200);
  c2Y->Divide(3,2, 0.01, 0.01);
  for(int i=0; i<5; i++){
    c2Y->cd(i+1);
    hy[i]->Fit("gaus");
    hy[i]->Draw();

    label = "z="+ std::to_string(zavg[i]/h[i]->GetEntries()).substr(0, 5)+" mm";
    latex.DrawLatexNDC(0.15, 0.8, (label).c_str());
  }
  c2Y->cd(6);
  h1clsizey[0]->Draw();
  c2Y->Print(graphnameY.c_str(), "png");

  TCanvas *c1 = new TCanvas("c1", "c1", 1600,1200);
  c1->Divide(3,2, 0.01, 0.01);
  for(int i=0; i<5; i++){
    c1->cd(i+1);
    h[i]->Draw("colz");
    gPad->SetLogz();

    label = "z="+ std::to_string(zavg[i]/h[i]->GetEntries()).substr(0, 5)+" mm";
    latex.DrawLatexNDC(0.15, 0.8, (label).c_str());

    label = "#sigma_{x}="+ std::to_string(hx[i]->GetFunction("gaus")->GetParameter(2)).substr(0, 5)+" mm";
    latex.DrawLatexNDC(0.15, 0.75, (label).c_str());
  }
  c1->Print(graphname.c_str(), "png");

  TCanvas *c3 = new TCanvas("c3", "c3", 1600,1200);
  c3->Divide(3,2, 0.01, 0.01);
  for(int i=0; i<5; i++){
    c3->cd(i+1);
    hresX[i]->Fit("gaus");
    hresX[i]->Draw();

    label = "#sigma_{x}="+ std::to_string(hresX[i]->GetFunction("gaus")->GetParameter(2)).substr(0, 5)+" mm";
    latex.DrawLatexNDC(0.15, 0.8, (label).c_str());
  }
  c3->Print(graphnameResX.c_str(), "png");

  TCanvas *c4 = new TCanvas("c4", "c4", 1600,1200);
  c4->Divide(3,2, 0.01, 0.01);
  for(int i=0; i<5; i++){
    c4->cd(i+1);
    hresY[i]->Fit("gaus");
    hresY[i]->Draw();

    label = "#sigma_{y}="+ std::to_string(hresY[i]->GetFunction("gaus")->GetParameter(2)).substr(0, 5)+" mm";
    latex.DrawLatexNDC(0.15, 0.8, (label).c_str());
  }
  c4->Print(graphnameResY.c_str(), "png");

  TCanvas *c5 = new TCanvas("c5", "c5", 1600,1200);
  c5->Divide(3,2, 0.01, 0.01);
  for(int i=0; i<5; i++){
    c5->cd(i+1);
    TF1 *fpol1 = new TF1("pol1", "pol1", -2, 2);
    TProfile *hprof = hresposX[i]->ProfileX();
    hprof->Fit(fpol1, "R");
    hresposX[i]->Draw("colz");
    // hprof->Draw("same");
    gPad->SetLogz();

    label = "slope="+ std::to_string(fpol1->GetParameter(1)).substr(0, 5);
    latex.DrawLatexNDC(0.15, 0.8, (label).c_str());
  }
  c5->Print(graphnameResPosX.c_str(), "png");

  TCanvas *c6 = new TCanvas("c6", "c6", 1600,1200);
  c6->Divide(3,2, 0.01, 0.01);
  for(int i=0; i<5; i++){
    c6->cd(i+1);
    TF1 *fpol1 = new TF1("pol1", "pol1", -2, 2);
    TProfile *hprof = hresposY[i]->ProfileX();
    hprof->Fit(fpol1, "R");
    hresposY[i]->Draw("colz");
    // hprof->Draw("same");
    gPad->SetLogz();

    label = "slope="+ std::to_string(fpol1->GetParameter(1)).substr(0, 5);
    latex.DrawLatexNDC(0.15, 0.8, (label).c_str());
  }
  c6->Print(graphnameResPosY.c_str(), "png");

  for(int l=0; l<5; l++){
    std::string graphnameLayer = prefix + "_Layer"+std::to_string(l)+".png";
    
    TCanvas *c7 = new TCanvas("c7", "c7", 1600,1200);
    c7->Divide(2,2, 0.01, 0.01);
    c7->cd(1);
    hresX[l]->Fit("gaus");
    hresX[l]->Draw();
    label = "#sigma_{x}="+ std::to_string(hresX[l]->GetFunction("gaus")->GetParameter(2)).substr(0, 5)+" mm";
    latex.DrawLatexNDC(0.15, 0.8, (label).c_str());

    c7->cd(2);
    hresY[l]->Fit("gaus");
    hresY[l]->Draw();
    label = "#sigma_{y}="+ std::to_string(hresY[l]->GetFunction("gaus")->GetParameter(2)).substr(0, 5)+" mm";
    latex.DrawLatexNDC(0.15, 0.8, (label).c_str());

    c7->cd(3);
    TF1 *fpol1 = new TF1("pol1", "pol1", -2, 2);
    TProfile *hprof = hresposX[l]->ProfileX();
    hprof->Fit(fpol1, "R");
    hresposX[l]->Draw("colz");
    hprof->Draw("same");
    gPad->SetLogz();
    label = "slope="+ std::to_string(fpol1->GetParameter(1)).substr(0, 5);
    latex.DrawLatexNDC(0.15, 0.8, (label).c_str());

    c7->cd(4);
    TF1 *fpol2 = new TF1("pol2", "pol1", -2, 2);
    TProfile *hprof2 = hresposY[l]->ProfileX();
    hprof2->Fit(fpol2, "R");
    hresposY[l]->Draw("colz");
    hprof2->Draw("same");
    gPad->SetLogz();
    label = "slope="+ std::to_string(fpol2->GetParameter(1)).substr(0, 5);
    latex.DrawLatexNDC(0.15, 0.8, (label).c_str());

    c7->Print(graphnameLayer.c_str(), "png");
    delete c7;
  }
  fin->Close();
  delete c1, c2, c3, c4, c5, c6;
  delete fin;
  // delete h[5], hresposX[5], hresposY[5], hx[5], hy[5], hresX[5], hresY[5], h1clsizex[5], h1clsizey[5], hresposX[5], hresposY[5];
}

void defStyle(){
  // myStyle = (TStyle*)gStyle->Clone(); // copy the default style
  // myStyle = gROOT->GetStyle("Default");
  // TStyle* myStyle = new TStyle("Plain","Default Style");
    gStyle->SetName("myStyle");
    gStyle->SetTextFont(43);
    gStyle->SetTextSize(25);

    // Set the font and size for all axis labels
    gStyle->SetLabelFont(43, "XYZ"); // Set the font to Helvetica for the labels of the x-axis, y-axis, and z-axis
    gStyle->SetLabelSize(25, "XYZ"); // Set the font size for the labels of the x-axis, y-axis, and z-axis

    // Set the font and size for all axis titles
    gStyle->SetTitleFont(43, "XYZ"); // Set the font to Helvetica for the titles of the x-axis, y-axis, and z-axis
    gStyle->SetTitleSize(25, "XYZ"); // Set the font size for the titles of the x-axis, y-axis, and z-axis

    gStyle->SetTitleFont(43,"T"); // Set the font to Helvetica for the titles of the x-axis, y-axis, and z-axis
    gStyle->SetTitleSize(25,"T"); // Set the font size for the titles of the x-axis, y-axis, and z-axis

  // gROOT->SetStyle("myStyle");
  // gROOT->ForceStyle();
    // gStyle->SetPalette(kTemperatureMap);
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);
    gStyle->SetPadTopMargin(0.07);
    gStyle->SetPadBottomMargin(0.12);
    gStyle->SetPadLeftMargin(0.12);
    gStyle->SetPadRightMargin(0.12);

    gStyle->SetLineWidth(2);
    gStyle->SetFrameLineWidth(2);
    // gStyle->SetFuncWidth(2);
    gStyle->SetHistLineWidth(1);
}


int main(int argc, char const *argv[])
{ 
  defStyle();
  std::string fname = argv[1];
  std::vector<double> pitch = {0.5, 1., 1.5};       // mm
  std::vector<double> inter =  {0.25, 0.5};         // fraction of pitch
  std::vector<double> thRatio = {0.1, 0.2};
  std::vector<double> sigma = {0.2, 0.300, 0.4}; // mm

  for( auto p : pitch){
    for( auto i : inter){
      for( auto t : thRatio){
        for( auto s : sigma){
          std::cout<<"pitch="<<p<<", inter="<<i<<", thRatio="<<t<<", sigma="<<s<<std::endl;
          plot_all(fname, p, i, t, s);
        }
      }
    }
  }

  return 0;
}