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
#include "TProfile.h"
#include "TGraphErrors.h"
#include "TGaxis.h"
#include "TLatex.h"
#include "TROOT.h"

void defStyle(){
  // myStyle = (TStyle*)gStyle->Clone(); // copy the default style
  // myStyle = gROOT->GetStyle("Default");
  // TStyle* myStyle = new TStyle("Plain","Default Style");
    gStyle->SetName("myStyle");
    gStyle->SetTextFont(43);
    gStyle->SetTextSize(25);

// Set the font and size for all axis labels
    TGaxis::SetMaxDigits(3);
    gStyle->SetLabelFont(42, "XYZ"); // Set the font to Helvetica for the labels of the x-axis, y-axis, and z-axis
    gStyle->SetLabelSize(0.05, "XYZ"); // Set the font size for the labels of the x-axis, y-axis, and z-axis

    // Set the font and size for all axis titles
    gStyle->SetTitleFont(42, "XYZ"); // Set the font to Helvetica for the titles of the x-axis, y-axis, and z-axis
    gStyle->SetTitleSize(0.06, "XYZ"); // Set the font size for the titles of the x-axis, y-axis, and z-axis
    gStyle->SetTitleOffset(0.8, "X"); // Set the offset of the x-axis title
    gStyle->SetTitleOffset(1., "Y"); // Set the offset of the y-axis title

    gStyle->SetTitleH(0.07);
	gStyle->SetTitleW(1.);
    gStyle->SetTitleX(0.53);
    gStyle->SetTitleY(0.98);

  // gROOT->SetStyle("myStyle");
  // gROOT->ForceStyle();
    // gStyle->SetPalette(kTemperatureMap);
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);
    gStyle->SetPadTopMargin(0.09);
    gStyle->SetPadBottomMargin(0.12);
    gStyle->SetPadLeftMargin(0.12);
    gStyle->SetPadRightMargin(0.12);

    gStyle->SetLineWidth(2);
    gStyle->SetFrameLineWidth(2);
    // gStyle->SetFuncWidth(2);
    gStyle->SetHistLineWidth(2);
    gROOT->ForceStyle();
}

int main(int argc, char* argv[]) {
    
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << "<detector name> <banco file> <input file>" << std::endl;
        return 1;
    }
    defStyle();

    std::string basedir = argv[0];
    std::string detName = argv[1];
    std::string fnameBanco = argv[2];
    std::string fname = argv[3];

    TFile* file = TFile::Open(fname.c_str(), "read");
    TFile* fileBanco = TFile::Open(fnameBanco.c_str(), "read");
    if (!file) {
        std::cerr << "Error: could not open input file " << fname << std::endl;
        return 1;
    }

    std::string detMap;
    if (detName.find("strip") != std::string::npos){
        detMap = "../map/strip_map.txt";
    }
    else if (detName.find("inter") != std::string::npos){
        detMap = "../map/inter_map.txt";
    }
    else if (detName.find("asa") != std::string::npos){
        detMap = "../map/asa_map.txt";
    }
    else {
        std::cerr << "Error: detector name not recognized" << std::endl;
        return 1;
    }

    StripTable det(detMap);
    std::string run;
    if(fname.find("POS")!=std::string::npos) run = fname.substr(fname.find("POS"), 5);
    std::string alignName = basedir + "../map/alignFiles/" + detName + "_" + run + ".txt";
    basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";
    det.SetAlignFile(alignName);

    TTreeReader reader("events", file);
    TTreeReaderValue<std::vector<cluster>> cls(reader, "clusters");
    TTreeReaderValue<std::vector<hit>> hits(reader, "hits");
    TTreeReaderValue<uint16_t> ftst(reader, "ftst");

    TTreeReader readerBanco("events", fileBanco);
    TTreeReaderValue< std::vector<banco::track> > tracks(readerBanco, "tracks");

    TH1F *h_timeofmaxX = new TH1F("h_timeofmaxX", "timeofmax X", 80, 0., 500);
    TH1F *h_timeofmaxY = new TH1F("h_timeofmaxY", "timeofmax Y", 80, 0., 500);
    
    while (reader.Next()) {
        bool isBanco = readerBanco.Next();
        if(!isBanco){
            std::cout<<"WARNING: Missing banco event"<<std::endl;
            continue;
        }
        if(tracks->size() == 0) continue;
        auto tr = *std::min_element(tracks->begin(), tracks->end(),
                       [](const banco::track& a,const banco::track& b) { return a.chi2x+a.chi2y < b.chi2x+b.chi2y; });

        std::shared_ptr<cluster> clX = maxSizeClX(*cls);
        std::shared_ptr<cluster> clY = maxSizeClY(*cls);
        double corr = *ftst*10.;
        if(clX && clY){
            std::vector<double> detPos = det.pos3D(clX->stripCentroid, clY->stripCentroid);
            double resX = detPos[0] - tr.x0 - tr.mx*detPos[2];
            double resY = detPos[1] - tr.y0 - tr.my*detPos[2];
            if(abs(resX) > 5. && abs(resY) > 5.) continue;

            auto hX = getHits(&(*hits), clX->id);
            if(hX[0].maxamp>400) h_timeofmaxX->Fill(hX[0].timeofmax*50+corr);
            auto hY = getHits(&(*hits), clY->id);
            if(hY[0].maxamp>400) h_timeofmaxY->Fill(hY[0].timeofmax*50+corr);
        }
        // if(!clX && !clY){
        //     std::vector<hit> hX, hY;
        //     for(auto h : *hits){
        //         if(h.axis == 'x') hX.push_back(h);
        //         else hY.push_back(h);
                
        //         if(hX.size() > 1){
        //             std::sort (hX.begin(), hX.end(),
        //                 [](const hit& a, const hit& b) {return a.maxamp > b.maxamp;});
        //             if(hX[0].maxamp>400) h_timeofmaxX->Fill(hX[0].timeofmax*50+corr);
        //         }
        //         if(hY.size() > 1){
        //             std::sort (hY.begin(), hY.end(),
        //                 [](const hit& a, const hit& b) {return a.maxamp > b.maxamp;});
        //             if(hY[0].maxamp>400) h_timeofmaxY->Fill(hY[0].timeofmax*50+corr);
        //         }
        //     }
        // }
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
    c->SaveAs((detName + "_timeofmaxCorr.png").c_str());


    // plot the relative time diff between P2 detectors on FEU5
//     if(detName.find("P2") == std::string::npos) return 0;

//     reader.Restart();
//     TH1F *hdiff = new TH1F("hdiff", "Time of max difference between P2_1 and P2_2", 80, -130, 130);
//     TH1F *htmax = new TH1F("htmax", "Time of max P2_1", 100, 125, 350);
//     TH1F *htmaxCorr = new TH1F("htmaxCorr", "Time of max P2_1 ftst corrected", 100, 125, 350);
//     TH2F *h2 = new TH2F("h2", "Time of max P2_1 vs maxamp", 70, 0, 500, 100, 170, 320);
    
//     while (reader.Next()) {
//         std::vector<hit> hP2_1, hP2_2;
//         double corr = *ftst*10.;
        
//         for(auto h : *hits){
//             if(h.channel/64 == 2 || h.channel/64 == 3) hP2_1.push_back(h);
//             if(h.channel/64 == 4 || h.channel/64 == 5) hP2_2.push_back(h);
//         }
//         if(hP2_1.size() < 1 || hP2_2.size() < 1) continue;
//         auto maxP2_1 = std::max_element(hP2_1.begin(), hP2_1.end(), 
//             [](const hit& a, const hit& b) {return a.maxamp < b.maxamp;});
//         auto maxP2_2 = std::max_element(hP2_2.begin(), hP2_2.end(), 
//             [](const hit& a, const hit& b) {return a.maxamp < b.maxamp;});
        
//         hdiff->Fill((maxP2_1->timeofmax - maxP2_2->timeofmax)*50);
//         htmax->Fill(maxP2_1->timeofmax*50);
//         htmaxCorr->Fill(maxP2_1->timeofmax*50 + corr);
//         h2->Fill(maxP2_1->maxamp-256, maxP2_1->timeofmax*50+corr);
//     }

//     TCanvas *c2 = new TCanvas("c2", "c2", 1400, 1200);
//     TF1 *f2 = new TF1("f2", "gaus", -100, 100);
//     hdiff->Fit(f2, "R");
//     hdiff->Draw();
//     hdiff->GetXaxis()->SetTitle("Time of max difference [ns]");
//     c2->SaveAs("P2_23_timeofmax_diff.png");

//     TCanvas *c3 = new TCanvas("c3", "c3", 1400, 1200);
//     htmax->Draw();
//     htmax->Fit(f, "R");
//     htmax->GetXaxis()->SetTitle("Time of max [ns]");
//     c3->SaveAs("P2_1_timeofmax.png");

//     TCanvas *c4 = new TCanvas("c4", "c4", 1400, 1200);
//     htmaxCorr->Draw();
//     htmaxCorr->Fit(f, "R");
//     htmaxCorr->GetXaxis()->SetTitle("Time of max ftst corrected [ns]");
//     c4->SaveAs("P2_1_timeofmaxCorr.png");

//     TCanvas *c5 = new TCanvas("c5", "c5", 1400, 1200);
//     h2->Draw("colz");
//     // gPad->SetLogz();
//     h2->GetXaxis()->SetTitle("Max amplitude [ADC]");
//     h2->GetYaxis()->SetTitle("Time of max ftst corrected [ns]");
//     c5->SaveAs("P2_1_timeofmaxCorr_vs_maxAmp.png");

//     // fit each x bin of the 2D histogram with a gaussian and plot the sigma as a function of x
//     TGraphErrors *gr = new TGraphErrors();
//     for(int i=1; i<=h2->GetNbinsX(); i++){
//         TH1D *h = h2->ProjectionY("h", i, i);
//         TF1 *f = new TF1("f", "gaus", 150, 300);
//         f->SetParameter(1, 250);
//         h->Fit(f, "R");
//         gr->SetPoint(gr->GetN(), h2->GetXaxis()->GetBinCenter(i), f->GetParameter(2));
//         gr->SetPointError(gr->GetN()-1, 0, f->GetParError(2));
//     }
//     TCanvas *c6 = new TCanvas("c6", "c6", 1400, 1200);
//     gr->Draw("AP");
//     gr->SetMarkerStyle(20);
//     gr->GetXaxis()->SetTitle("Max amplitude [ADC]");
//     gr->GetYaxis()->SetTitle("Sigma time of max corrected [ns]");
//     c6->SaveAs("P2_1_timeofmaxCorr_vs_maxAmp_sigma.png");
        
    return 0;
}