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

    std::string run;
    if(fnameMM.find("POS") != std::string::npos){
        run = fnameMM.substr(fnameMM.find("POS"), 5);
    }else if(fnameMM.find("HVS") != std::string::npos){
        run = fnameMM.substr(fnameMM.find("HVS"), 5);
    }
    else{
        std::cerr << "Invalid run name" << std::endl;
        return 1;
    }

    std::string alignName = basedir + "../map/alignFiles/" + detName + "_" + run + ".txt";
    StripTable det(mapName, alignName);
    
    auto posBeam = beamPosition(fnameBanco, det.getZpos(), true);
    std::cout<<det.getZpos()<<" x: "<<posBeam[0][0]<<" +- "<<posBeam[1][0]<<std::endl;
    std::cout<<det.getZpos()<<" y: "<<posBeam[0][1]<<" +- "<<posBeam[1][1]<<std::endl;

    TFile* fMM = TFile::Open(fnameMM.c_str(), "read");
    TFile* fbanco = TFile::Open(fnameBanco.c_str(), "read");

    TTreeReader MM("events", fMM);
    TTreeReader banco("events", fbanco);

    TTreeReaderValue< std::vector<cluster> > cls( MM, "clusters");
    TTreeReaderValue< std::vector<hit> > hits( MM, "hits");
    TTreeReaderValue< std::vector<banco::track> > tracks( banco, "tracks");

    double numX = 0, denX = 0;
    double numY = 0, denY = 0;
    double numXY = 0, denXY = 0;

    while( MM.Next() ){
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
        
        double xreftr = tr.x0 + tr.mx*det.getZpos();
        double yreftr = tr.y0 + tr.my*det.getZpos();
        if(abs(yreftr - posBeam[0][1]) < 1*posBeam[1][1] and abs(xreftr - posBeam[0][0]) < 1*posBeam[1][0]){
            denX++;
            denY++;
            denXY++;
        }else continue;

        if(maxX){
            std::vector<double> detPos = det.pos3D(maxX->stripCentroid, -1);
            double ytr = tr.y0 + tr.my*detPos[2];
            if(abs(detPos[1]-ytr) < 2) numX++;
        }
        if(maxY){
            std::vector<double> detPos = det.pos3D(-1, maxY->stripCentroid);
            double xtr = tr.x0 + tr.mx*detPos[2];
            if(abs(detPos[0]-xtr) < 2) numY++;
        }
        if(maxX && maxY){
            std::vector<double> detPos = det.pos3D(maxX->stripCentroid, maxY->stripCentroid);
            double xtr = tr.x0 + tr.mx*detPos[2];
            double ytr = tr.y0 + tr.my*detPos[2];
            if(abs(detPos[0]-xtr) < 2 and abs(detPos[1]-ytr) < 2) numXY++;
        }
    }

    std::cout<<"Efficiency X: "<<numX/denX<<std::endl;
    std::cout<<"Efficiency Y: "<<numY/denY<<std::endl;
    std::cout<<"Efficiency XY: "<<numXY/denXY<<std::endl;

    // in efficiency.txt file write run,numX,denX,numY,denY,numXY,denXY
    std::ofstream outfile;
    outfile.open("efficiency.txt");
    outfile << run << "," << numX << "," << denX << "," << numY << "," << denY << "," << numXY << "," << denXY << std::endl;
    outfile.close();

    fMM->Close();
    fbanco->Close();

    return 0;
}