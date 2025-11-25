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
#include "TMultiGraph.h"
#include "TLatex.h"
#include "TProfile.h"

#include "../reco/definitions.h"
#include "../banco/definition_banco.h"
#include "../map/StripTable.h"
#include "analysis.h"


void plot_efficiency(std::string fname, std::string detName){

    std::map<std::string, std::vector<double>> HVS;
    HVS["asaFEU4"] = { 300., 350., 360., 370., 380., 390., 400., 410., 430., 440., 450, 460., 410, 410, 390.};
    HVS["stripFEU1"] = { 300., 350., 360., 370., 380., 390., 400., 410., 410., 410., 410, 410, 380, 360, 340.};


    std::ifstream infile(fname);
    std::string line;
    std::string run, numstr, denstr;
    double num, den;
    std::vector<std::string> names = {"X", "Y", "XY"};
    int colors[3] = {kBlue, kRed, kBlack};

    TGraphErrors* gr[3];
    for(int i=0; i<3; i++){
        gr[i] = new TGraphErrors();
        gr[i]->SetMarkerStyle(20+i);
        gr[i]->SetMarkerSize(1.5);
        gr[i]->SetMarkerColor(colors[i]);
        gr[i]->SetName(names[i].c_str());
    }

    while (std::getline(infile, line)) {
        if (line[0] == '#') continue;
        std::istringstream iss(line);
        std::getline(iss, run, ',');
        for(int i=0; i<3; i++){
            std::getline(iss, numstr, ',');    num = std::stod(numstr);
            std::getline(iss, denstr, ',');    den = std::stod(denstr);
            gr[i]->SetPoint(gr[i]->GetN(), HVS[detName][std::stoi(run.substr(3,2))-1], num/den);
            std::cout<<num/den<<" ";
        }
        std::cout<<std::endl;
    }

    TCanvas* c1 = new TCanvas("c1", "c1", 1600, 1200);
    TMultiGraph* mg = new TMultiGraph();
    mg->SetTitle(Form("HVS Efficiency %s", detName.c_str()));
    for(int i=0; i<3; i++){
        mg->Add(gr[i], "p");
    }
    mg->Draw("a");
    mg->GetXaxis()->SetTitle("R tension [V]");
    mg->GetYaxis()->SetTitle("Efficiency");
    // mg->GetYaxis()->SetRangeUser(0.5, 1.1);
    c1->BuildLegend();
    c1->SetGrid();
    c1->Modified();
    c1->Update();
    c1->SaveAs(Form("efficiency_%s.png", detName.c_str()));
}


// Return { {x,y}, {sigmax,sigmay} } of the banco tracks at the detector zpos
std::vector<std::vector<double>> beamPosition(std::string fname, double zpos, bool plot=false){
    
    TFile* file = TFile::Open(fname.c_str(), "read");
    if (!file) {
        std::cerr << "Error: could not open input file " << fname << std::endl;
        return {{0,0},{0,0}};
    }

    TTreeReader reader("events", file);
    TTreeReaderValue<std::vector<banco::track>> tracks(reader, "tracks");

    TH1F *hx = new TH1F("hx", "x position", 100, 2, 12);
    TH1F *hy = new TH1F("hy", "y position", 100, 1, 11); 

    while(reader.Next()){
        if(tracks->size() == 0) continue;

        auto tr = *std::min_element(tracks->begin(), tracks->end(),
                        [](const banco::track& a,const banco::track& b) { return a.chi2x+a.chi2y < b.chi2x+b.chi2y; });
        if(tr.chi2x>1 or tr.chi2y>1) continue;
        hx->Fill(tr.x0 + tr.mx*zpos);
        hy->Fill(tr.y0 + tr.my*zpos);
    }

    TF1* fitX = new TF1("fitFuncX", "gaus", 2, 12);
    hx->Fit(fitX, "R");

    TF1* fitY = new TF1("fitFuncY", "gaus", 1, 11);
    hy->Fit(fitY, "R");

    if(plot){
        TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
        c1->Divide(2,1);
        c1->cd(1);
        hx->Draw();
        c1->cd(2);
        hy->Draw();
        c1->SaveAs("beamPosition.png");
    }
    file->Close();
    return {{fitX->GetParameter(1), fitY->GetParameter(1)}, {fitX->GetParameter(2), fitY->GetParameter(2)}};
}


int main(int argc, char* argv[]){

    std::string basedir = argv[0];
    basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";

    if(argc < 3 and argc > 5){
        std::cerr << "compute efficiency " << argv[0] << " <detName> <banco.root> <mm.root> <banco y>" << std::endl;
        std::cerr << "plot " << argv[0] << " <detName> <efficiency.txt>" << std::endl;
        return 1;
    }

    if(argc == 3){
        std::string detName = argv[1];
        std::string fname = argv[2];
        plot_efficiency(fname, detName);
        return 0;
    }

    std::string detName = argv[1];
    std::string fnameBanco = argv[2];
    std::string fnameMM = argv[3];

    float bancoY = 0.;
    if( argc == 5 ) bancoY = std::atof( argv[4] );

    std::string mapName;
    if (detName.find("asa") != std::string::npos) {
        mapName = basedir + "../map/" + "asa_map.txt";
    } else if (detName.find("strip") != std::string::npos) {
        mapName = basedir + "../map/" + "strip_map.txt";
    } else if (detName.find("inter") != std::string::npos) {
        mapName = basedir + "../map/" + "inter_map.txt";
    } else if (detName.find("rd5") != std::string::npos) {
        mapName = basedir + "../map/" + "rd542_map.txt";
    } else {
        std::cerr << "Invalid detector name" << std::endl;
        return 1;
    }

    std::string run, alignName;
    if(fnameMM.find("POS") != std::string::npos){
        run = fnameMM.substr(fnameMM.find("POS"), 5);
        alignName = basedir + "../map/alignFiles/" + detName + "_" + run + ".txt";
    }else if(fnameMM.find("HVS") != std::string::npos){
        run = fnameMM.substr(fnameMM.find("HVS"), 5);
        int nb = std::stoi(run.substr(3,2));
        if(nb<13) alignName = basedir + "../map/alignFiles/" + detName + "_" + "HVS" + ".txt";
        else alignName = basedir + "../map/alignFiles/"+ detName + "_" + "HVS2" + ".txt";
    }
    else if ( detName.find("rd5") != std::string::npos ) {
      alignName = "alignFiles/" + detName + ".txt";
    }
    else{
        std::cerr << "Invalid run name" << std::endl;
        return 1;
    }

    StripTable det(mapName, alignName);
    std::cout<<"Detector "<<detName<<" at "<<det.getZpos()<<std::endl;

    TFile* fMM = TFile::Open(fnameMM.c_str(), "read");
    TFile* fbanco = TFile::Open(fnameBanco.c_str(), "read");

    TTreeReader MM("events", fMM);
    TTreeReader banco("events", fbanco);

    TTreeReaderValue< std::vector<cluster> > cls( MM, "clusters");
    TTreeReaderValue< std::vector<hit> > hits( MM, "hits");
    TTreeReaderValue< std::vector<banco::track> > tracks( banco, "tracks");

    TTreeReaderValue< unsigned long > banco_evId( banco, "eventId");
    TTreeReaderValue< unsigned long > MM_evId( MM, "eventId");

    double numX = 0, denX = 0;
    double numY = 0, denY = 0;
    double numXY = 0, denXY = 0;

    double dtol = 2.5;
    double sigma = 5;
 
    TFile *fout = TFile::Open( ("eff_"+ detName + ".root").c_str(),"recreate");
    TH2F *hNumX  = new TH2F("hNumX","efficiency X",   100, -30, 210, 100, -30, 210); 
    TH2F *hNumY  = new TH2F("hNumY","efficiency Y",   100, -30, 210, 100, -30, 210); 
    TH2F *hNumXY = new TH2F("hNumXY","efficiency XY", 100, -30, 210, 100, -30, 210); 
    hNumX->SetDirectory(fout);
    hNumY->SetDirectory(fout);
    hNumXY->SetDirectory(fout);

    TH2F *hDenXY = new TH2F("hDenXY","efficiency XY", 100, -30, 210, 100, -30, 210); 
    hDenXY->SetDirectory(fout);


    while( MM.Next() ){
      bool isBanco = banco.Next();
      if( *MM_evId != *banco_evId ){
        std::cout << "WARNING: files with different numbers of triggers. MM: " << *MM_evId << "; banco: " << *banco_evId << std::endl;
        break;
      }
      if(!isBanco){
          std::cout<<"WARNING: Missing banco event"<<std::endl;
          continue;
      }

      // skip events without tracks 
      if(tracks->size() == 0 or cls->size() == 0) continue;

      // get the best track
      auto tr = *std::min_element(tracks->begin(), tracks->end(),
                      [](const banco::track& a,const banco::track& b) { return a.chi2x+a.chi2y < b.chi2x+b.chi2y; });
      if(tr.chi2x>3 or tr.chi2y>20) continue;

      // find the clusters with the largest size. TODO is this needed? how to check on cluster quality?
      auto maxX = maxSizeClX(*cls);
      auto maxY = maxSizeClY(*cls);
      
      double xreftr = tr.x0 + tr.mx*det.getZpos();
      double yreftr = tr.y0 + tr.my*det.getZpos() + bancoY; // taking in consideration the shift of banco

      // at this point here we have a good track;
      denX++;
      denY++;
      denXY++;

      hDenXY->Fill( xreftr, yreftr );

      if(maxX){
          std::vector<double> detPos = det.pos3D( maxX->stripCentroid,-1);
          if(abs(detPos[1]-yreftr) < dtol){
            numX++;
            hNumX->Fill( xreftr, yreftr );
          }
      }
      if(maxY){
          std::vector<double> detPos = det.pos3D(-1, maxY->stripCentroid);
          if(abs(detPos[0]-xreftr) < dtol){
            numY++;
            hNumY->Fill( xreftr, yreftr );
          }
      }
      if(maxX && maxY){
          std::vector<double> detPos = det.pos3D(maxX->stripCentroid, maxY->stripCentroid);
          double xtr = tr.x0 + tr.mx*detPos[2];
          double ytr = tr.y0 + tr.my*detPos[2];
          if(abs(detPos[0]-xreftr) < dtol and abs(detPos[1]-yreftr) < dtol){
            numXY++;
            hNumXY->Fill( xreftr, yreftr );
          }
      }
    }

    fMM->Close();
    fbanco->Close();

    std::cout<<"\nEfficiency X: "<<numX/denX<<" den:"<<denXY<<std::endl;
    std::cout<<"Efficiency Y: "<<numY/denY<<" den:"<<denXY<<std::endl;
    std::cout<<"Efficiency XY: "<<numXY/denXY<<" den:"<<denXY<<std::endl;

    TH2F *hEffX  = (TH2F*)hNumX->Clone("hEffX"); 
    TH2F *hEffY  = (TH2F*)hNumY->Clone("hEffY"); 
    TH2F *hEffXY  = (TH2F*)hNumXY->Clone("hEffXY"); 
    hEffX->SetDirectory(fout);
    hEffY->SetDirectory(fout);
    hEffXY->SetDirectory(fout);
    hEffX->Divide( hDenXY );
    hEffY->Divide( hDenXY );
    hEffXY->Divide( hDenXY );
   
    fout->Write();
    fout->Close();
    // // in efficiency.txt file write run,numX,denX,numY,denY,numXY,denXY
    std::ofstream outfile;
    outfile.open( detName+"_efficiency.txt");
    outfile << run << "," << numX << "," << denX << "," << numY << "," << denY << "," << numXY << "," << denXY << std::endl;
    outfile.close();
    return 0;
}
