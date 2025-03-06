#include <iostream>
#include <string>
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "../reco/definitions.h"
#include "../banco/definition_banco.h"
#include "../map/StripTable.h"
#include "clusterSize.h"
#include "TLatex.h"


int plots_samplemax(std::string fname, std::string detName, StripTable det, int zone){
    
    TFile* file = TFile::Open(fname.c_str(), "read");
    if (!file) {
        std::cerr << "Error: could not open input file " << fname << std::endl;
        return 1;
    }

    TTreeReader reader("events", file);
    TTreeReaderValue<std::vector<cluster>> cls(reader, "clusters");
    TTreeReaderValue<std::vector<hit>> hits(reader, "hits");
    TTreeReaderValue<uint64_t> delta_timestamp(reader, "delta_timestamp");

    TH2F* h2 = new TH2F("h2", "sampleMax vs deltatimestamp", 200, 0, 50000, 50, -0.5, 16.5);
    h2->GetXaxis()->SetTitle("delta timestamp");
    h2->GetYaxis()->SetTitle("sampleMax");

    TH1F *hclX[6];
    TH1F *hclcenterX[6];
    TH1F *hclY[6];
    TH1F *hclcenterY[6];
    for(int i=0; i<6; i++){
        std::string label = "hcl"+std::to_string(i);
        std::string title = "clusters size "+std::to_string(i+1);
        hclX[i] = new TH1F(("X "+label).c_str(), ("X "+title).c_str(), 100, -0.5, 16.5);
        hclX[i]->SetXTitle("sample max");

        hclcenterX[i] = new TH1F(("X "+label+"center").c_str(), ("X "+title).c_str(), 100, -0.5, 16.5);
        hclcenterX[i]->SetXTitle("sample max");

        hclY[i] = new TH1F(("Y "+label).c_str(), ("Y "+title).c_str(), 100, -0.5, 16.5);
        hclY[i]->SetXTitle("sample max");

        hclcenterY[i] = new TH1F(("Y "+label+"center").c_str(), ("Y "+title).c_str(), 100, -0.5, 16.5);
        hclcenterY[i]->SetXTitle("sample max");
    }

    TH1F *clsizeX = new TH1F("clsizeX", "cluster size", 12,-0.5,11.5);
    TH1F *clsizeY = new TH1F("clsizeY", "cluster size", 12,-0.5,11.5);

    double avgX = 0; int nX = 0;
    double avgY = 0; int nY = 0;

    while (reader.Next()) {
        
        std::shared_ptr<cluster> clX = maxSizeClX(*cls);
        std::shared_ptr<cluster> clY = maxSizeClY(*cls);
        if(clX){
            clsizeX->Fill(clX->size);
            if(clX->size > 6) continue;
            auto hX = getHits(*hits, clX->id);
            avgX += clX->stripCentroid; nX++;
            hclcenterX[clX->size-1]->Fill(hX[0].timeofmax);
            for(auto h : hX) hclX[clX->size-1]->Fill(h.timeofmax);   
        }
        if(clY){
            clsizeY->Fill(clY->size);
            if(clY->size > 6) continue;
            auto hY = getHits(*hits, clY->id);
            avgY += clY->stripCentroid; nY++;
            hclcenterY[clY->size-1]->Fill(hY[0].timeofmax);
            for(auto h : hY) hclY[clY->size-1]->Fill(h.timeofmax);   
        }
    }

    float pitchX = det.pitchX(avgX/nX);
    float pitchY = det.pitchY(avgY/nY);

    TCanvas* c1 = new TCanvas("c1", "c1", 1600, 1000);
    h2->Draw("colz");
    // gPad->SetLogz();
    c1->SaveAs(Form("%s_sampleMax_deltaT_ref%d.png", detName.c_str(), zone));

    TCanvas* c2 = new TCanvas("c2", "c2", 1600, 1000);
    c2->Divide(3,2);
    for(int i=0; i<6; i++){
        c2->cd(i+1);
        hclcenterX[i]->SetLineColor(kBlue);
        hclcenterX[i]->Draw();

        hclX[i]->SetLineColor(kRed);
        hclX[i]->Scale(1./(i+1));
        hclX[i]->Draw("HIST same");
    }
    c2->cd(0);
    TLegend* legX = new TLegend(0.85, 0.7, 1, 0.8);
    legX->AddEntry(hclX[0], "#splitline{samplemax all hits}{scaled by 1/clsize}", "l");
    legX->AddEntry(hclcenterX[0], "samplemax of center strip", "l");
    legX->AddEntry("", Form("X pitch = %.2f mm", pitchX), "");
    legX->SetTextFont(43);
    legX->SetTextSize(15);
    legX->Draw();
    c2->SaveAs(Form("%s_timeofmax_clusterSizeX_ref%d.png", detName.c_str(), zone));

    TCanvas* c2y = new TCanvas("c2y", "c2y", 1600, 1000);
    c2y->Divide(3,2);
    for(int i=0; i<6; i++){
        c2y->cd(i+1);
        hclcenterY[i]->SetLineColor(kBlue);
        hclcenterY[i]->Draw();

        hclY[i]->SetLineColor(kRed);
        hclY[i]->Scale(1./(i+1));
        hclY[i]->Draw("HIST same");
    }
    c2y->cd(0);
    TLegend* legY = new TLegend(0.85, 0.7, 1, 0.8);
    legY->AddEntry(hclY[0], "#splitline{samplemax all hits}{scaled by 1/clsize}", "l");
    legY->AddEntry(hclcenterY[0], "samplemax of center strip", "l");
    legY->AddEntry("", Form("Y pitch = %.2f mm", pitchY), "");
    legY->SetTextFont(43);
    legY->SetTextSize(15);
    legY->Draw();
    c2y->SaveAs(Form("%s_timeofmax_clusterSizeY_ref%d.png", detName.c_str(), zone));


    TCanvas* c3 = new TCanvas("c3", "c3", 1600, 1000);
    clsizeX->SetLineColor(kBlue);
    clsizeX->Draw();
    clsizeY->SetLineColor(kRed);
    clsizeY->Draw("same");

    TLegend* leg2 = new TLegend(0.85, 0.6, 1, 0.7);
    leg2->AddEntry(clsizeX, "cluster size X", "l");
    leg2->AddEntry(clsizeY, "cluster size Y", "l");
    leg2->AddEntry("", Form("X pitch = %.2f mm", pitchX), "");
    leg2->AddEntry("", Form("Y pitch = %.2f mm", pitchY), "");
    leg2->SetTextFont(43);
    leg2->SetTextSize(18);
    leg2->Draw();
    c3->SaveAs(Form("%s_clusterSize_ref%d.png", detName.c_str(), zone));

    return 0;
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
        zoneRuns = { {0,11}, {1,14}, {2,2}, {3,5}};
        det = StripTable(basedir+"../map/asa_map.txt");
    }
    else if (detName.find("inter") != std::string::npos){
        zoneRuns = { {1,13} };
        det = StripTable(basedir+"../map/inter_map.txt");
    }
    else {
        std::cerr << "Error: detector name not recognized" << std::endl;
        return 1;
    }

    for (int i = 2; i < argc; i++) {
        std::string arg = argv[i];
        std::cout<<arg<<std::endl;
        if (arg.substr(arg.size() - 5) != ".root") {
            std::cout<<arg<<" is invalid"<<std::endl;
        } else {
            std::string fname = arg;
            int pos = std::stoi( fname.substr(fname.find("POS")+3, 2) );
            auto it = std::find_if(zoneRuns.begin(), zoneRuns.end(), [pos](const auto& it) {return it.second == pos; });
            if(it != zoneRuns.end()){
                plots_samplemax(fname, detName, det, it->first);
            }
        }
    }

    return 0;
}