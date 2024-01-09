#include <string>
#include <numeric>
#include <cmath>

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"
#include "TNtupleD.h"
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


double Xpitch, Xinter, Ypitch, Yinter;

std::vector<THStack*> waveforms(std::string fnameMM, int& entry, int clsizeX=0, int clsizeY=0){

    TFile* fMM = TFile::Open(fnameMM.c_str(), "read");;

    TTreeReader MM("events", fMM);

    TTreeReaderValue< std::vector<cluster> > cls( MM, "clusters");
    TTreeReaderValue< std::vector<hit> > hits( MM, "hits");

    double stX = 0;
    double stY = 0;
    int n = 0;
    std::vector<int> color = {kBlue, kRed, kViolet, kBlack};

    TH1F* hx[4];
    TH1F* hy[4];

    for(int i=0; i<4; i++){
        hx[i] = new TH1F(("hx"+std::to_string(i)).c_str(), ("waveform strip nb "+std::to_string(i+1)).c_str(), 17, -0.5, 16.0);
        hx[i]->GetXaxis()->SetTitle("time sample");
        hx[i]->GetYaxis()->SetTitle("ADC");
        hx[i]->SetLineColor(color[i]);
        hy[i] = new TH1F(("hy"+std::to_string(i)).c_str(), ("waveform strip nb "+std::to_string(i+1)).c_str(), 17, -0.5, 16.0);
        hy[i]->GetXaxis()->SetTitle("time sample");
        hy[i]->GetYaxis()->SetTitle("ADC");
        hy[i]->SetLineColor(color[i]);
    }

    MM.SetEntry(entry-1);
    bool eventFound = false;

    while(!eventFound){
        entry++;
        if(!MM.Next()) break;
        auto maxX = maxSizeClX(*cls);
        auto maxY = maxSizeClY(*cls);

        if(!maxX || !maxY) continue;
        if(clsizeX!=0 && maxX->size!=clsizeX) continue;
        if(clsizeY!=0 && maxY->size!=clsizeY) continue;
        eventFound = true;

        // n++; stX += maxX->stripCentroid; stY += maxY->stripCentroid;
        auto hitsX = getHits(*hits, maxX->id);
        auto hitsY = getHits(*hits, maxY->id);
        for(int i=0; i<maxX->size && i<4; i++){
            std::cout<<"X "<<i<<" maxsample "<<hitsX[i].samplemax<<" maxamp "<<hitsX[i].maxamp<<std::endl;
            for(int j=0; j<hitsX[i].samples.size(); j++) hx[i]->SetBinContent(j, hitsX[i].samples[j]);
        }
        for(int i=0; i<maxY->size && i<4; i++){
            std::cout<<"Y "<<i<<" maxsample "<<hitsY[i].samplemax<<" maxamp "<<hitsY[i].maxamp<<std::endl;
            for(int j=0; j<hitsY[i].samples.size(); j++) hy[i]->SetBinContent(j, hitsY[i].samples[j]);
        }
    }

    // Xpitch = det.pitchX(stX/n);
    // Xinter = det.interX(stX/n);
    // Ypitch = det.pitchY(stY/n);
    // Yinter = det.interY(stY/n, stX/n);

    THStack* hsx = new THStack("hsx", "");
    THStack* hsy = new THStack("hsy", "");
    for(int i=0; i<4; i++){
        hsx->Add(hx[i]);
        hsy->Add(hy[i]);
    }
    return {hsx, hsy};
}

void plotWaveform(std::string fname, std::string graphname, int entry, int clsizeX=0, int clsizeY=0){

    TCanvas *c = new TCanvas("c", "c", 1600,1000);
    gStyle->SetOptStat(0);
    TLatex latex;
    latex.SetTextFont(43);
    latex.SetTextSize(18);
    std::string label;

    c->Divide(4,2, 0.001, 0.001);
    for(int i=0; i<8; i++){
        auto hs = waveforms(fname, entry, clsizeX, clsizeY);
        c->cd(i+1);
        hs[0]->Draw("nostack");
        hs[0]->GetXaxis()->SetTitle("time sample");
        hs[0]->GetYaxis()->SetTitle("ADC");
        c->cd(i+1+4);
        hs[1]->Draw("nostack");
        hs[1]->GetXaxis()->SetTitle("time sample");
        hs[1]->GetYaxis()->SetTitle("ADC");
    }
    c->Print(graphname.c_str(), "png");
}


int main(int argc, char const *argv[])
{

  std::string basedir = argv[0];
  basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";
  
  if(argc != 3){
    std::cerr << "Usage: " << argv[0] << " <detName> <mm.root>" << std::endl;
    return 1;
  }

  std::string detName = argv[1];
  std::string fnameMM = argv[2];
  
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

//   StripTable det(mapName, alignName);
  // StripTable det(mapName);
  std::string graphname = "waveform_"+run+"_"+detName+"_32_cuts.png";
  
  plotWaveform(fnameMM,graphname, 1000, 3, 2);

  return 0;
}