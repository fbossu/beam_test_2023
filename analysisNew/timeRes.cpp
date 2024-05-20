#include <iostream>
#include <string>
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "../reco/definitions.h"
#include "../banco/definition_banco.h"
#include "../map/StripTable.h"
#include "analysis.h"
#include "TLatex.h"

int main(int argc, char* argv[]) {
    
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << "<detector name> <input file>" << std::endl;
        return 1;
    }
    std::string fname = argv[2];
    std::string detName = argv[1];

    TFile* file = TFile::Open(fname.c_str(), "read");
    if (!file) {
        std::cerr << "Error: could not open input file " << fname << std::endl;
        return 1;
    }

    TTreeReader reader("events", file);
    TTreeReaderValue<std::vector<cluster>> cls(reader, "clusters");
    TTreeReaderValue<std::vector<hit>> hits(reader, "hits");

    TH1F *h_timeofmaxX = new TH1F("h_timeofmaxX", "Time of max X", 100, 0, 10);
    TH1F *h_timeofmaxY = new TH1F("h_timeofmaxY", "Time of max Y", 100, 0, 10);
    
    while (reader.Next()) {
        
        std::shared_ptr<cluster> clX = maxSizeClX(*cls);
        std::shared_ptr<cluster> clY = maxSizeClY(*cls);
        if(clX){
            auto hX = getHits(*hits, clX->id);
            h_timeofmaxX->Fill(hX[0].timeofmax);
        }
        if(clY){
            auto hY = getHits(*hits, clY->id);
            h_timeofmaxY->Fill(hY[0].timeofmax);
        }
    }

    TCanvas *c = new TCanvas("c", "c", 1600, 1200);
    c->Divide(2, 1);
    c->cd(1);
    h_timeofmaxX->Draw();
    c->cd(2);
    h_timeofmaxY->Draw();
    c->SaveAs((detName + "_timeofmax.png").c_str());
    return 0;
}