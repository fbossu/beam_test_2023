#include <iostream>
#include <string>
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "../reco/definitions.h"
#include "../banco/definition_banco.h"
#include "../map/StripTable.h"
#include "clusterSize.h"


TCanvas* xy_compare(std::string fname, StripTable det, int zone){

    TH1F* h1[5];
    std::vector<std::string> labels = {"1/1", "1/2", "2/1", "2/2", "other"};
    for(int i=0; i<5; i++){
        h1[i] = new TH1F("h1", Form("Ratio xAmp/yAmp cluster size %s Y/X",labels[i].c_str()), 200, 0., 3);
        h1[i]->GetXaxis()->SetTitle("xAmp/yAmp");
    }

    TFile* file = TFile::Open(fname.c_str(), "read");
    if (!file) {
        std::cerr << "Error: could not open input file " << fname << std::endl;
        return nullptr;
    }

    TTreeReader reader("events", file);
    TTreeReaderValue<std::vector<cluster>> cls(reader, "clusters");
    TTreeReaderValue<std::vector<hit>> hits(reader, "hits");

    int nX=0, nY=0;
    while (reader.Next()) {
        std::shared_ptr<cluster> maxX = maxSizeClX(*cls);
        std::shared_ptr<cluster> maxY = maxSizeClY(*cls);
        int ampX=0, ampY=0;
        if(maxX) {nX++; ampX = totAmp(*hits, maxX->id);}
        if(maxY) {nY++; ampY = totAmp(*hits, maxY->id);}
        if(maxX && maxY){
            if (maxY->size == 1 && maxX->size == 1) h1[0]->Fill((float) ampX/ampY);
            else if (maxY->size == 1 && maxX->size == 2) h1[1]->Fill((float) ampX/ampY);
            else if (maxY->size == 2 && maxX->size == 1) h1[2]->Fill((float) ampX/ampY);
            else if (maxY->size == 2 && maxX->size == 2) h1[3]->Fill((float) ampX/ampY);
            else h1[4]->Fill((float) ampX/ampY);
        }
    }
    file->Close();

    double eff = (double) nX/nY;

    TCanvas* c1 = new TCanvas("c1", "c1", 1600, 1200);
    c1->Divide(3,3);
    for(int i=0; i<5; i++){
        c1->cd(i+1);
        h1[i]->Draw();
    }
    c1->cd(0);
    TLegend* leg = new TLegend(0.8, 0.2, 0.95, 0.3);
    leg->AddEntry("", Form("x pitch: %.2f mm", det.pitchXzone(zone)), "");
    leg->AddEntry("", Form("y pitch: %.2f mm", det.pitchYzone(zone)), "");
    leg->AddEntry("", Form("efficiency nX/nY: %.2f", eff), "");
    leg->Draw();

    return c1;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << "<detname> <input_file.root>" << std::endl;
        return 1;
    }

    std::string basedir = argv[0];
    basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";
    std::cout << basedir << std::endl;


    std::map<int, int> zoneRuns;
    StripTable det;
    
    std::string detName = argv[1];

    if (detName.find("strip") != std::string::npos){
        zoneRuns = { {0,16}, {1,14}, {3,11}, {4,13}, {6,8}, {7,6}};
        det = StripTable(basedir+"../map/strip_map.txt");
    }
    else if (detName.find("asa") != std::string::npos){
        zoneRuns = { {0,16}, {1,14}, {2,2}, {3,5}};
        det = StripTable(basedir+"../map/asa_map.txt");
    }
    else {
        std::cerr << "Error: detector name not recognized" << std::endl;
        return 1;
    }

    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        if (arg.substr(arg.size() - 5) != ".root") {
            std::cout<<arg<<" is invalid"<<std::endl;
        } else {
            std::string fname = arg;
            int pos = std::stoi( fname.substr(fname.find("POS")+3, fname.find("POS")+5) );
            auto it = std::find_if(zoneRuns.begin(), zoneRuns.end(), [pos](const auto& it) {return it.second == pos; });
            if(it != zoneRuns.end()){
                TCanvas* c = xy_compare(fname, det, it->first);
                c->SaveAs(Form("%s_POS%d_z%d_xy.png", detName.c_str(), pos, it->first));
                delete c;
            }
        }
    }

    return 0;
}
