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

int main(int argc, char const *argv[]){

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

    StripTable det(mapName, alignName);

    TFile *f = new TFile(fnameMM.c_str());
    TTreeReader reader("events", f);

    TTreeReaderValue<std::vector<hit>> hits(reader, "hits");
    TTreeReaderValue<std::vector<cluster>> cls(reader, "clusters");

    TH1I *hX = new TH1I("hX", "MaxAmplitude on the next to leading X strip", 301, 255.5, 555.5);
    TH1I *hY = new TH1I("hY", "MaxAmplitude on the next to leading Y strip", 301, 255.5, 555.5);

    while(reader.Next()){
        auto maxX = maxSizeClX(*cls);
        auto maxY = maxSizeClY(*cls);
    
        if(maxX && maxY){
            auto hitsX = getHits(*hits, maxX->id);
            auto hitsY = getHits(*hits, maxY->id);
            if(hitsX.size() < 2 || hitsY.size() < 2) continue;
            hX->Fill(hitsX[1].maxamp);
            hY->Fill(hitsY[1].maxamp);
        }
    }

    TCanvas *c = new TCanvas("c", "c", 1600, 1200);
    c->Divide(2, 1);
    c->cd(1);
    hX->Draw();
    c->cd(2);
    hY->Draw();
    c->SaveAs("testAmp_2.png");

    return 0;
}