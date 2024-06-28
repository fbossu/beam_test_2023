#include "../reco/definitions.h"

void spectreP2(){

    TTreeReader MM("events", _file0);
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
        hEdep_1->Fill(Edep_1);
        hEdep_2->Fill(Edep_2);
        hEdep_3->Fill(Edep_3);
    }

    TCanvas *c = new TCanvas("c", "c", 1200, 800);
    c->Divide(2, 2, 0.01, 0.01);
    c->cd(1);
    hEdep_1->Draw("hist");
    c->cd(2);
    hEdep_2->Draw("hist");
    c->cd(3);
    hEdep_3->Draw("hist");
    c->SaveAs("spectreP2.png");
}