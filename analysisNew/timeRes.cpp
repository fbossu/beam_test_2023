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
#include "TCanvas.h"
#include "TF1.h"

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
    TTreeReaderValue<uint16_t> ftst(reader, "ftst");

    TH1F *h_timeofmaxX = new TH1F("h_timeofmaxX", "tdiff X", 80, 0., 500);
    TH1F *h_timeofmaxY = new TH1F("h_timeofmaxY", "tdiff Y", 80, 0., 500);
    
    while (reader.Next()) {
        
        std::shared_ptr<cluster> clX = maxSizeClX(*cls);
        std::shared_ptr<cluster> clY = maxSizeClY(*cls);
        if(clX){
            auto hX = getHits(*hits, clX->id);
            if(hX[0].maxamp>400) h_timeofmaxX->Fill(hX[0].timeofmax*50+*ftst*10);
        }
        if(clY){
            auto hY = getHits(*hits, clY->id);
            if(hY[0].maxamp>400) h_timeofmaxY->Fill(hY[0].timeofmax*50+*ftst*10);
        }
        if(!clX && !clY){
            std::vector<hit> hX, hY;
            for(auto h : *hits){
                if(h.axis == 'x') hX.push_back(h);
                else hY.push_back(h);
                
                if(hX.size() > 1){
                    std::sort (hX.begin(), hX.end(),
                        [](const hit& a, const hit& b) {return a.maxamp > b.maxamp;});
                    if(hX[0].maxamp>400) h_timeofmaxX->Fill(hX[0].timeofmax*50+*ftst*10);
                }
                if(hY.size() > 1){
                    std::sort (hY.begin(), hY.end(),
                        [](const hit& a, const hit& b) {return a.maxamp > b.maxamp;});
                    if(hY[0].maxamp>400) h_timeofmaxY->Fill(hY[0].timeofmax*50+*ftst*10);
                }
            }
        }
    }

    // fit each histogram with a gaussian centered around 4
    TF1 *f = new TF1("f", "gaus", 100, 350);
    f->SetParameter(1, 250);

    gStyle->SetOptFit(1111);
    TCanvas *c = new TCanvas("c", "c", 1600, 1200);
    c->Divide(2, 1);
    c->cd(1);
    h_timeofmaxX->Fit(f, "R");
    h_timeofmaxX->Draw();
    c->cd(2);
    h_timeofmaxY->Fit(f, "R");
    h_timeofmaxY->Draw();
    c->SaveAs((detName + "_timeofmax_ftst.png").c_str());
    return 0;
}