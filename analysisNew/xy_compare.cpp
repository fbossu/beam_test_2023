#include <iostream>
#include <string>
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TLatex.h"
#include "TLine.h"

#include "../reco/definitions.h"
#include "../banco/definition_banco.h"
#include "../map/StripTable.h"
#include "analysis.h"

double median(std::vector<double> &v, double q=0.5){
    int n = v.size()*q;
    std::nth_element(v.begin(), v.begin()+n, v.end());
    double med = v[n];
    if(!(v.size() & 1)) { //If the set size is even
        auto max_it = std::max_element(v.begin(), v.begin()+n);
        med = (*max_it + med) / 2.0;
    }
    return med;    
}

double mean(std::vector<double> &v){
    double sum = std::accumulate(v.begin(), v.end(), 0.0);
    double mean = sum / v.size();
    return mean;
}

std::vector<double> beamPos(std::string fBanco, double z){
    TFile* filebanco = TFile::Open(fBanco.c_str(), "read");
    if (!filebanco) {
        std::cerr << "Error: could not open input file " << fBanco << std::endl;
        return {0,0,0,0,0};
    }
    TTreeReader readerBanco("events", filebanco);
    TTreeReaderValue< std::vector<banco::track> > tracks( readerBanco, "tracks");
    double meanx = 0, meany = 0;
    double count = 0;
    while (readerBanco.Next()) {
        if(tracks->size() == 0) continue;

        auto tr = *std::min_element(tracks->begin(), tracks->end(),
                       [](const banco::track& a,const banco::track& b) { return a.chi2x+a.chi2y < b.chi2x+b.chi2y; });
        meanx += tr.x0 + z*tr.mx;
        meanx += tr.y0 + z*tr.my;
        count++;
    }
    filebanco->Close();
    return {meanx/count, meany/count};
}

// std::vector<double> xy_compare(std::string fBanco, std::string fname, StripTable det, int zone, std::string graphName){

//     TH1F* h1[5];
//     TH1F* h2[5];
//     std::vector<std::string> labels = {"1/1", "1/2", "2/1", "2/2", "other"};
//     for(int i=0; i<5; i++){
//         h1[i] = new TH1F("h1", Form("Ratio xAmp/(xAmp+yAmp) cluster size %s X/Y",labels[i].c_str()), 200, 0., 1);
//         h1[i]->GetXaxis()->SetTitle("amplitude ratio");
//         h1[i]->SetLineColor(kBlue);
//         h2[i] = new TH1F("h2", Form("Ratio yAmp/(xAmp+yAmp) cluster size %s X/Y",labels[i].c_str()), 200, 0., 1);
//         h2[i]->GetXaxis()->SetTitle("amplitude ratio");
//         h2[i]->SetLineColor(kRed);
//     }

//     TH2F *h2clsize = new TH2F("h2clsize", "cluster size X vs Y", 8,-0.5,7.5, 8,-0.5,7.5);
//     h2clsize->SetXTitle("cluster size y direction strips");
//     h2clsize->SetYTitle("cluster size x direction strips");

//     TH2F *ampXY = new TH2F("ampXY", "Amplitude X vs Y", 700, 0, 7500, 700, 0, 7500.);
//     ampXY->SetXTitle("amplitude Y direction strips [ADC]");
//     ampXY->SetYTitle("amplitude X direction strips [ADC]");

//     TFile* file = TFile::Open(fname.c_str(), "read");
//     if (!file) {
//         std::cerr << "Error: could not open input file " << fname << std::endl;
//         return {0,0,0,0,0};
//     }

//     TTreeReader reader("events", file);
//     TTreeReaderValue<std::vector<cluster>> cls(reader, "clusters");
//     TTreeReaderValue<std::vector<hit>> hits(reader, "hits");

//     TFile* filebanco = TFile::Open(fBanco.c_str(), "read");
//     if (!filebanco) {
//         std::cerr << "Error: could not open input file " << fBanco << std::endl;
//         return {0,0,0,0,0};
//     }
//     TTreeReader readerBanco("events", filebanco);
//     TTreeReaderValue< std::vector<banco::track> > tracks(readerBanco, "tracks");

//     // std::vector<double> beamAvg = beamPos(fBanco, det.getZpos());

//     int nX=0, nY=0;
//     double gainNum=0, gainDen=0;
//     std::vector<double> Xclsize, Yclsize;
//     std::vector<double> XampF, YampF;
//     while (reader.Next()) {

//         std::shared_ptr<cluster> maxX = maxSizeClX(*cls);
//         std::shared_ptr<cluster> maxY = maxSizeClY(*cls);
//         bool isBanco = readerBanco.Next();
//         if(!isBanco){
//             std::cout<<"WARNING: Missing banco event"<<std::endl;
//             continue;
//         }
//         if(tracks->size() == 0) continue;
//         auto tr = *std::min_element(tracks->begin(), tracks->end(),
//                        [](const banco::track& a,const banco::track& b) { return a.chi2x+a.chi2y < b.chi2x+b.chi2y; });

//         int ampX=0, ampY=0;
//         if(maxX) {
//             nX++;
//             std::vector<double> detPos = det.pos3D(maxX->stripCentroid, 64);
//             double res = detPos[1] - tr.y0 - tr.my*detPos[2];
//             if(abs(res)<5.){
//                 ampX = totMaxAmp(&(*hits), maxX->id);
//                 Xclsize.push_back(maxX->size);
//             }
//             else maxX = nullptr;
//         }
//         if(maxY) {
//             nY++;
//             std::vector<double> detPos = det.pos3D(64, maxY->stripCentroid);
//             double res = detPos[0] - tr.x0 - tr.mx*detPos[2];
//             if(abs(res)<5.){
//                 ampY = totMaxAmp(&(*hits), maxY->id);
//                 Yclsize.push_back(maxY->size);
//             }
//             else maxY = nullptr;
//         }

//         if(maxX && maxY){
//             if(det.zone(maxX->stripCentroid, maxY->stripCentroid) != zone) continue;
//             std::vector<double> detPos = det.pos3D(maxX->stripCentroid, maxY->stripCentroid);
            
//             // double res = sqrt(pow(detPos[1] - tr.y0 - tr.my*detPos[2], 2) + pow(detPos[0] - tr.x0 - tr.mx*detPos[2], 2));
//             // if(abs(res) > 5.) continue;
            
//             double resX = detPos[0] - tr.x0 - tr.mx*detPos[2];
//             double resY = detPos[1] - tr.y0 - tr.my*detPos[2];
//             if(abs(resX) > 5. && abs(resY) > 5.) continue;
            
//             ampX = totMaxAmp(&(*hits), maxX->id);
//             ampY = totMaxAmp(&(*hits), maxY->id);
//             gainNum += ampX;
//             gainNum += ampY;
//             gainDen ++;
//             double Xfrac = (double) ampX/(ampX+ampY);
//             double Yfrac = (double) ampY/(ampX+ampY);
//             XampF.push_back(Xfrac);
//             YampF.push_back(Yfrac);
//             h2clsize->Fill(maxY->size, maxX->size);
//             ampXY->Fill(ampY, ampX);
//             if (maxX->size == 1 && maxY->size == 1)      {h1[0]->Fill(Xfrac); h2[0]->Fill(Yfrac);}
//             else if (maxX->size == 1 && maxY->size == 2) {h1[1]->Fill(Xfrac); h2[1]->Fill(Yfrac);}
//             else if (maxX->size == 2 && maxY->size == 1) {h1[2]->Fill(Xfrac); h2[2]->Fill(Yfrac);}
//             else if (maxX->size == 2 && maxY->size == 2) {h1[3]->Fill(Xfrac); h2[3]->Fill(Yfrac);}
//             else {h1[4]->Fill(Xfrac); h2[4]->Fill(Yfrac);}
//         }
//     }
//     file->Close();
    
//     double eff = (double) nX/nY;

//     TCanvas* c1 = new TCanvas("c1", "c1", 1600, 1200);
//     c1->Divide(3,2);
//     c1->cd(1);
//     h2clsize->SetStats(0);
//     h2clsize->Draw("colz");
//     gPad->SetLogz();
//     TLatex* tex = new TLatex();
//     tex->SetTextSize(0.03);
//     tex->DrawLatexNDC(0., 0., Form("corr factor: %.2f", h2clsize->GetCorrelationFactor()));
//     for(int i=0; i<5; i++){
//         c1->cd(i+2);
//         h1[i]->Draw();
//         h2[i]->Draw("same");
//     }
//     c1->cd(0);
//     // std::cout<<"cc"<<zone<<std::endl;
//     // std::cout<<det.pitchXzone(zone)<<"cc"<<std::endl;
//     TLegend* leg = new TLegend(0.8, 0.2, 0.95, 0.3);
//     leg->AddEntry(h1[0], Form("x pitch: %.2f mm", det.pitchXzone(zone)), "l");
//     leg->AddEntry(h2[0], Form("y pitch: %.2f mm", det.pitchYzone(zone)), "l");
//     leg->AddEntry("", Form("efficiency nX/nY: %.2f", eff), "");
//     leg->Draw();
//     c1->SaveAs(graphName.c_str());

//     TCanvas* c2 = new TCanvas("c2", "c2", 1600, 1200);
//     ampXY->SetStats(0);
//     ampXY->Draw("colz");
//     gPad->SetLogz();

//     TLine* line = new TLine(5, 5, 7000, 7000);
//     line->SetLineColor(kRed);
//     line->SetLineWidth(3);
//     ampXY->Draw("colz");
//     line->Draw("same");

//     TLatex* tex2 = new TLatex();
//     // tex2->SetTextFont(43);
//     // tex2->SetTextSize(22);
//     tex2->DrawLatexNDC(0.8, 0.25, Form("x pitch: %.2f mm", det.pitchXzone(zone)));
//     tex2->DrawLatexNDC(0.8, 0.2, Form("y pitch: %.2f mm", det.pitchYzone(zone)));
//     c2->SaveAs(Form("%s_ampXY.png", graphName.substr(0,graphName.size()-5).c_str()));

//     return {gainNum/gainDen, mean(Xclsize), mean(Yclsize), mean(XampF), mean(YampF)};
// }


std::vector<double> xy_compare(std::string fBanco, std::string fname, StripTable det, int zone, std::string graphName){

    TH1F* h1[5];
    TH1F* h2[5];
    std::vector<std::string> labels = {"1/1", "1/2", "2/1", "2/2", "other"};
    for(int i=0; i<5; i++){
        h1[i] = new TH1F("h1", Form("Ratio xAmp/(xAmp+yAmp) cluster size %s X/Y",labels[i].c_str()), 200, 0., 1);
        h1[i]->GetXaxis()->SetTitle("amplitude ratio");
        h1[i]->SetLineColor(kBlue);
        h2[i] = new TH1F("h2", Form("Ratio yAmp/(xAmp+yAmp) cluster size %s X/Y",labels[i].c_str()), 200, 0., 1);
        h2[i]->GetXaxis()->SetTitle("amplitude ratio");
        h2[i]->SetLineColor(kRed);
    }

    TH2F *h2clsize = new TH2F("h2clsize", "cluster size X vs Y", 8,-0.5,7.5, 8,-0.5,7.5);
    h2clsize->SetXTitle("cluster size y direction strips");
    h2clsize->SetYTitle("cluster size x direction strips");

    TH2F *ampXY = new TH2F("ampXY", "Amplitude X vs Y", 700, 0, 7500, 700, 0, 7500.);
    ampXY->SetXTitle("amplitude Y direction strips [ADC]");
    ampXY->SetYTitle("amplitude X direction strips [ADC]");

    TFile* file = TFile::Open(fname.c_str(), "read");
    if (!file) {
        std::cerr << "Error: could not open input file " << fname << std::endl;
        return {0,0,0,0,0};
    }

    TTreeReader reader("events", file);
    TTreeReaderValue<std::vector<cluster>> cls(reader, "clusters");
    TTreeReaderValue<std::vector<hit>> hits(reader, "hits");

    TFile* filebanco = TFile::Open(fBanco.c_str(), "read");
    if (!filebanco) {
        std::cerr << "Error: could not open input file " << fBanco << std::endl;
        return {0,0,0,0,0};
    }
    TTreeReader readerBanco("events", filebanco);
    TTreeReaderValue< std::vector<banco::track> > tracks(readerBanco, "tracks");

    // std::vector<double> beamAvg = beamPos(fBanco, det.getZpos());

    int nX=0, nY=0;
    double gainNum=0, gainDen=0;
    std::vector<double> Xclsize, Yclsize;
    std::vector<double> XampF, YampF;
    while (reader.Next()) {

        std::shared_ptr<cluster> maxX = maxSizeClX(*cls);
        std::shared_ptr<cluster> maxY = maxSizeClY(*cls);
        bool isBanco = readerBanco.Next();
        if(!isBanco){
            std::cout<<"WARNING: Missing banco event"<<std::endl;
            continue;
        }
        if(tracks->size() == 0) continue;
        auto tr = *std::min_element(tracks->begin(), tracks->end(),
                       [](const banco::track& a,const banco::track& b) { return a.chi2x+a.chi2y < b.chi2x+b.chi2y; });

        int ampX=0, ampY=0;
        if(maxX==nullptr || maxY==nullptr || det.zone(maxX->stripCentroid, maxY->stripCentroid) != zone) continue;
        std::vector<double> detPos = det.pos3D(maxX->stripCentroid, maxY->stripCentroid);
        
        double resX = detPos[0] - tr.x0 - tr.mx*detPos[2];
        double resY = detPos[1] - tr.y0 - tr.my*detPos[2];
        if(abs(resX) > 5. && abs(resY) > 5.) continue;
        
        double yGerber = det.posX(maxX->stripCentroid)[1];
        double xGerber = det.posY(maxY->stripCentroid)[0];
        if(xGerber<-75) continue; // POS11
        // if(yGerber>43) continue; // POS06
        // if(yGerber<18) continue; // asaFEU2
        // if(yGerber > 15 || xGerber > -75) continue; // asaFEU4 POS02
        // if(yGerber<100) continue; // stripFEU1 5mm x region
        
        ampX = totMaxAmp(&(*hits), maxX->id);
        Xclsize.push_back(maxX->size);
        
        ampY = totMaxAmp(&(*hits), maxY->id);
        Yclsize.push_back(maxY->size);

        gainNum += ampX;
        gainNum += ampY;
        gainDen ++;
        double Xfrac = (double) ampX/(ampX+ampY);
        double Yfrac = (double) ampY/(ampX+ampY);
        XampF.push_back(Xfrac);
        YampF.push_back(Yfrac);
        h2clsize->Fill(maxY->size, maxX->size);
        ampXY->Fill(ampY, ampX);
        if (maxX->size == 1 && maxY->size == 1)      {h1[0]->Fill(Xfrac); h2[0]->Fill(Yfrac);}
        else if (maxX->size == 1 && maxY->size == 2) {h1[1]->Fill(Xfrac); h2[1]->Fill(Yfrac);}
        else if (maxX->size == 2 && maxY->size == 1) {h1[2]->Fill(Xfrac); h2[2]->Fill(Yfrac);}
        else if (maxX->size == 2 && maxY->size == 2) {h1[3]->Fill(Xfrac); h2[3]->Fill(Yfrac);}
        else {h1[4]->Fill(Xfrac); h2[4]->Fill(Yfrac);}
    }
    file->Close();
    
    double eff = (double) nX/nY;

    TCanvas* c1 = new TCanvas("c1", "c1", 1600, 1200);
    c1->Divide(3,2);
    c1->cd(1);
    h2clsize->SetStats(0);
    h2clsize->Draw("colz");
    gPad->SetLogz();
    TLatex* tex = new TLatex();
    tex->SetTextSize(0.03);
    tex->DrawLatexNDC(0., 0., Form("corr factor: %.2f", h2clsize->GetCorrelationFactor()));
    for(int i=0; i<5; i++){
        c1->cd(i+2);
        h1[i]->Draw();
        h2[i]->Draw("same");
    }
    c1->cd(0);
    // std::cout<<"cc"<<zone<<std::endl;
    // std::cout<<det.pitchXzone(zone)<<"cc"<<std::endl;
    TLegend* leg = new TLegend(0.8, 0.2, 0.95, 0.3);
    leg->AddEntry(h1[0], Form("x pitch: %.2f mm", det.pitchXzone(zone)), "l");
    leg->AddEntry(h2[0], Form("y pitch: %.2f mm", det.pitchYzone(zone)), "l");
    leg->AddEntry("", Form("efficiency nX/nY: %.2f", eff), "");
    leg->Draw();
    c1->SaveAs(graphName.c_str());

    TCanvas* c2 = new TCanvas("c2", "c2", 1600, 1200);
    ampXY->SetStats(0);
    ampXY->Draw("colz");
    gPad->SetLogz();

    TLine* line = new TLine(5, 5, 7000, 7000);
    line->SetLineColor(kRed);
    line->SetLineWidth(3);
    ampXY->Draw("colz");
    line->Draw("same");

    TLatex* tex2 = new TLatex();
    // tex2->SetTextFont(43);
    // tex2->SetTextSize(22);
    tex2->DrawLatexNDC(0.8, 0.25, Form("x pitch: %.2f mm", det.pitchXzone(zone)));
    tex2->DrawLatexNDC(0.8, 0.2, Form("y pitch: %.2f mm", det.pitchYzone(zone)));
    c2->SaveAs(Form("%s_ampXY.png", graphName.substr(0,graphName.size()-5).c_str()));

    return {gainNum/gainDen, mean(Xclsize), mean(Yclsize), mean(XampF), mean(YampF)};
}


std::vector<double> xy_compareNoBanco(std::string fname, StripTable det, int zone, std::string graphName){

    TH1F* h1[5];
    TH1F* h2[5];
    std::vector<std::string> labels = {"1/1", "1/2", "2/1", "2/2", "other"};
    for(int i=0; i<5; i++){
        h1[i] = new TH1F("h1", Form("Ratio xAmp/(xAmp+yAmp) cluster size %s X/Y",labels[i].c_str()), 200, 0., 1);
        h1[i]->GetXaxis()->SetTitle("amplitude ratio");
        h1[i]->SetLineColor(kBlue);
        h2[i] = new TH1F("h2", Form("Ratio yAmp/(xAmp+yAmp) cluster size %s X/Y",labels[i].c_str()), 200, 0., 1);
        h2[i]->GetXaxis()->SetTitle("amplitude ratio");
        h2[i]->SetLineColor(kRed);
    }

    TH2F *h2clsize = new TH2F("h2clsize", "cluster size X vs Y", 8,-0.5,7.5, 8,-0.5,7.5);
    h2clsize->SetXTitle("cluster size y direction strips");
    h2clsize->SetYTitle("cluster size x direction strips");

    TH2F *ampXY = new TH2F("ampXY", "Amplitude X vs Y", 1000, 0, 3000, 1000, 0, 3000.);
    ampXY->SetXTitle("amplitude Y direction strips [ADC]");
    ampXY->SetYTitle("amplitude X direction strips [ADC]");

    TFile* file = TFile::Open(fname.c_str(), "read");
    if (!file) {
        std::cerr << "Error: could not open input file " << fname << std::endl;
        return {0,0,0,0,0};
    }

    TTreeReader reader("events", file);
    TTreeReaderValue<std::vector<cluster>> cls(reader, "clusters");
    TTreeReaderValue<std::vector<hit>> hits(reader, "hits");


    int nX=0, nY=0;
    double gainNum=0, gainDen=0;
    std::vector<double> Xclsize, Yclsize;
    std::vector<double> XampF, YampF;
    while (reader.Next()) {

        std::shared_ptr<cluster> maxX = maxSizeClX(*cls);
        std::shared_ptr<cluster> maxY = maxSizeClY(*cls);

        int ampX=0, ampY=0;

        if(maxX && maxY){
            if(det.zone(maxX->stripCentroid, maxY->stripCentroid) != zone && zone > -1) continue;
            std::vector<double> detPos = det.pos3D(maxX->stripCentroid, maxY->stripCentroid);
            // if(detPos[0]>-76) continue;
            ampX = totMaxAmp(&(*hits), maxX->id);
            Xclsize.push_back(maxX->size);
            ampY = totMaxAmp(&(*hits), maxY->id);
            Yclsize.push_back(maxY->size);

            gainNum += ampX;
            gainNum += ampY;
            gainDen ++;
            double Xfrac = (double) ampX/(ampX+ampY);
            double Yfrac = (double) ampY/(ampX+ampY);
            XampF.push_back(Xfrac);
            YampF.push_back(Yfrac);
            h2clsize->Fill(maxY->size, maxX->size);
            ampXY->Fill(ampY, ampX);
            if (maxX->size == 1 && maxY->size == 1)      {h1[0]->Fill(Xfrac); h2[0]->Fill(Yfrac);}
            else if (maxX->size == 1 && maxY->size == 2) {h1[1]->Fill(Xfrac); h2[1]->Fill(Yfrac);}
            else if (maxX->size == 2 && maxY->size == 1) {h1[2]->Fill(Xfrac); h2[2]->Fill(Yfrac);}
            else if (maxX->size == 2 && maxY->size == 2) {h1[3]->Fill(Xfrac); h2[3]->Fill(Yfrac);}
            else {h1[4]->Fill(Xfrac); h2[4]->Fill(Yfrac);}
        }
    }
    file->Close();
    
    double eff = (double) nX/nY;

    TCanvas* c1 = new TCanvas("c1", "c1", 1600, 1200);
    c1->Divide(3,2);
    c1->cd(1);
    h2clsize->SetStats(0);
    h2clsize->Draw("colz");
    gPad->SetLogz();
    TLatex* tex = new TLatex();
    tex->SetTextSize(0.03);
    tex->DrawLatexNDC(0., 0., Form("corr factor: %.2f", h2clsize->GetCorrelationFactor()));
    for(int i=0; i<5; i++){
        c1->cd(i+2);
        h1[i]->Draw();
        h2[i]->Draw("same");
    }
    c1->cd(0);
    // std::cout<<"cc"<<zone<<std::endl;
    // std::cout<<det.pitchXzone(zone)<<"cc"<<std::endl;
    TLegend* leg = new TLegend(0.8, 0.2, 0.95, 0.3);
    leg->AddEntry(h1[0], Form("x pitch: %.2f mm", det.pitchXzone(zone)), "l");
    leg->AddEntry(h2[0], Form("y pitch: %.2f mm", det.pitchYzone(zone)), "l");
    leg->AddEntry("", Form("efficiency nX/nY: %.2f", eff), "");
    leg->Draw();
    c1->SaveAs(graphName.c_str());

    TCanvas* c2 = new TCanvas("c2", "c2", 1600, 1200);
    ampXY->SetStats(0);
    ampXY->Draw("colz");
    gPad->SetLogz();

    TLine* line = new TLine(5, 5, 7000, 7000);
    line->SetLineColor(kRed);
    line->SetLineWidth(3);
    ampXY->Draw("colz");
    line->Draw("same");

    TLatex* tex2 = new TLatex();
    // tex2->SetTextFont(43);
    // tex2->SetTextSize(22);
    tex2->DrawLatexNDC(0.8, 0.25, Form("x pitch: %.2f mm", det.pitchXzone(zone)));
    tex2->DrawLatexNDC(0.8, 0.2, Form("y pitch: %.2f mm", det.pitchYzone(zone)));
    c2->SaveAs(Form("%s_ampXY.png", graphName.substr(0,graphName.size()-5).c_str()));

    return {gainNum/gainDen, mean(Xclsize), mean(Yclsize), mean(XampF), mean(YampF)};
}

// int main(int argc, char* argv[]) {
//     if (argc < 3) {
//         std::cerr << "Usage: " << argv[0] << "<detname> <input_file.root>" << std::endl;
//         return 1;
//     }

//     std::string basedir = argv[0];
//     basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";
//     std::cout << basedir << std::endl;


//     std::map<int, int> zoneRuns;
//     StripTable det;
    
//     std::string detName = argv[1];

//     if (detName.find("strip") != std::string::npos){
//         zoneRuns = { {0,16}, {1,14}, {3,11}, {4,13}, {6,8}, {7,6}};
//         det = StripTable(basedir+"../map/strip_map.txt");
//     }
//     else if (detName.find("inter") != std::string::npos){
//         zoneRuns = { {1,13} };
//         det = StripTable(basedir+"../map/inter_map.txt");
//     }
//     else if (detName.find("asa") != std::string::npos){
//         zoneRuns = { {0,8}, {0,11}, {1,14}, {2,2}, {3,5}};
//         det = StripTable(basedir+"../map/asa_map.txt");
//     }
//     else {
//         std::cerr << "Error: detector name not recognized" << std::endl;
//         return 1;
//     }

//     std::ofstream outfile;
//     outfile.open(Form("%s_xycompare.txt", detName.c_str()));
//     outfile<<"#run\tzone\tgain"<<std::endl;
//     outfile<<"#\t\tXpitch\tXinter\tXclsize\tXampF"<<std::endl;
//     outfile<<"#\t\tpitch\tYinter\tYclsize\tYampF"<<std::endl;

//     for (int i = 2; i < argc; i++) {
//         std::string arg = argv[i];
//         std::cout<<arg<<std::endl;
//         if (arg.substr(arg.size() - 5) != ".root") {
//             std::cout<<arg<<" is invalid"<<std::endl;
//         } else {
//             std::string fname = arg;
//             int pos;
//             if(fname.find("POS")!=std::string::npos) pos = std::stoi( fname.substr(fname.find("POS")+3, 2) );
//             else if(fname.find("HVS")!=std::string::npos) pos = std::stoi( fname.substr(fname.find("HVS")+3, 2) );
//             else{
//                 std::cout<<fname<<" is invalid"<<std::endl;
//                 continue;
//             }
//             auto it = std::find_if(zoneRuns.begin(), zoneRuns.end(), [pos](const auto& it) {return it.second == pos; });
//             if(it != zoneRuns.end()){
//                 std::vector<double> xyout = xy_compare(fname, det, it->first, Form("%s_POS%d_z%d_xy_maxamp.png", detName.c_str(), pos, it->first));
//                 clusterSizeFile(fname, detName, det, it->first);
//                 outfile<<"POS"<<it->second<<"\t"<<it->first<<"\t"<<xyout[0]<<std::endl;
//                 outfile<<"\t\t"<<det.pitchXzone(it->first)<<"\t"<<det.interXzone(it->first)<<"\t"<<xyout[1]<<"\t"<<xyout[3]<<std::endl;
//                 outfile<<"\t\t"<<det.pitchYzone(it->first)<<"\t"<<det.interYzone(it->first)<<"\t"<<xyout[2]<<"\t"<<xyout[4]<<std::endl;   
//             }
//         }
//     }
//     outfile.close();
//     return 0;
// }
