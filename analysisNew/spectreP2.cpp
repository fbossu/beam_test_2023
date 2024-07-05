#include <iostream>
#include <string>
#include <TFile.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TH1.h>

#include "../reco/definitions.h"

void spectreP2(std::string fname, std::string run, TFile *fout){

    TFile *fin = new TFile(fname.c_str(), "READ");
    TTreeReader MM("events", fin);
    TTreeReaderValue< std::vector<hit> > hits( MM, "hits");
    
    double Edep_1 = 0, Edep_2 = 0, Edep_3 = 0; // define Edep per event for each P2 detector
    TH1F *hEdep_1 = new TH1F("hEdep_1", "Edep_1", 200, 0, 1000);
    TH1F *hEdep_2 = new TH1F("hEdep_2", "Edep_2", 200, 0, 1000);
    TH1F *hEdep_3 = new TH1F("hEdep_3", "Edep_3", 200, 0, 1000);
    
    while( MM.Next() ){
        Edep_1 = 0;
        Edep_2 = 0;
        Edep_3 = 0;
        for(auto& h : *hits){
            // separate per P2 detector, -256 is for the ZS amplitude offset
            if(h.channel/64 == 2 || h.channel/64 == 3) Edep_1 += h.maxamp - 256;
            if(h.channel/64 == 4 || h.channel/64 == 5) Edep_2 += h.maxamp - 256;
            if(h.channel/64 == 6 || h.channel/64 == 7) Edep_3 += h.maxamp - 256;
        }
        // fill if there was a hit on the detector
        if(Edep_1>0.1) hEdep_1->Fill(Edep_1);
        if(Edep_2>0.1) hEdep_2->Fill(Edep_2);
        if(Edep_3>0.1) hEdep_3->Fill(Edep_3);
    }
    fout->cd();
    fout->mkdir(run.c_str());
    fout->cd(run.c_str());
    hEdep_1->Write();
    hEdep_2->Write();
    hEdep_3->Write();
}

int main(int argc, char* argv[]){
    if(argc < 2){
        std::cerr << "Usage: " << argv[0] << " <input file>" << std::endl;
        return 1;
    }

    std::map<std::string, double> HVP2;
	HVP2["HVS01"] = 300; HVP2["HVS02"] = 350; HVP2["HVS03"] = 360; HVP2["HVS04"] = 370; HVP2["HVS04Again"] = 370;
	HVP2["HVS05"] = 380; HVP2["HVS06"] = 390; HVP2["HVS07"] = 400; HVP2["HVS08"] = 410; HVP2["HVS09"] = 410; 
	HVP2["HVS10"] = 410; HVP2["HVS11"] = 410; HVP2["HVS12"] = 410; HVP2["HVS13"] = 380; HVP2["HVS14"] = 360; HVP2["HVS15"] = 340;

    TFile *fout = new TFile("spectreP2.root", "RECREATE");
    
    for(int i=1; i<argc; i++){
        std::string fname = argv[i];
        std::string run = fname.substr(fname.find("HVS"), fname.find("_FEU5.root") - fname.find("HVS"));
        std::cout << "Processing run: " << run << std::endl;

        spectreP2(fname, run, fout);
    }
    fout->Close();

    return 0;
}