#include "TNtupleD.h"
#include "analysis.h"
#include "TF1.h"
#include "../../../CLAS12/style_sheet.h"


void plotResGeant(std::string fnamegeant, std::string fnameMM, std::string det, std::string layer){
    TFile* fgeant = new TFile(fnamegeant.c_str(), "READ");
    TFile* fmm = new TFile(fnameMM.c_str(), "READ");

    TH1F* hgeant = (TH1F*)fgeant->Get(layer.c_str());

    hgeant->SetLineColor(kBlue);
    hgeant->SetLineWidth(2);
    hgeant->SetMarkerStyle(20);
    hgeant->SetMarkerSize(0.5);
    hgeant->SetMarkerColor(kBlue);
    hgeant->GetXaxis()->SetTitle("residue [mm]");
    hgeant->GetYaxis()->SetTitle("counts");
    hgeant->GetYaxis()->SetTitleOffset(0.7);

    TNtupleD* nt = (TNtupleD*) fmm->Get("nt");
    std::cout<<nt->GetEntries()<<std::endl;

    TH1F* hMM = new TH1F("hMM", "hMM", 200, -2., 2.);
    hMM->SetLineColor(kBlack);
    hMM->SetLineWidth(2);
    hMM->SetMarkerStyle(20);
    hMM->SetMarkerSize(0.5);
    hMM->SetMarkerColor(kBlack);
    nt->Draw("yres>>hMM");

    hgeant->Scale(1./hgeant->Integral());
    hMM->Scale(1./hMM->Integral());

    TLegend* leg = new TLegend(0.65, 0.65, 0.85, 0.85);

    // fit each histogram with a gaussian and add mean and sigma to the legend
    // add the parameters on a different line in the legend
    TF1* fitG = new TF1("fitG", "gaus", -2., 2.);
    fitG->SetNpx(3000);
    fitG->SetLineColor(kBlue);
    hgeant->Fit(fitG, "R");
    leg->AddEntry(hgeant, "Geant4 msc", "l");
    leg->AddEntry(hgeant, Form("#mu=%.1f #sigma=%.2f", fitG->GetParameter(1), fitG->GetParameter(2)), "");
    
    TF1* fitM = new TF1("fitM", "gaus", -2., 2.);
    fitM->SetLineColor(kBlack);
    hMM->Fit(fitM, "R");
    leg->AddEntry(hMM, "data", "l");
    leg->AddEntry(hMM, Form("#mu=%.1f #sigma=%.2f", fitM->GetParameter(1), fitM->GetParameter(2)), "");

    TCanvas* c = new TCanvas("c", "c", 1000, 800);
    hgeant->SetTitle(Form("Detector %s", det.c_str()));
    hgeant->Draw("E");
    hMM->SetTitle(Form("Detector %s", det.c_str()));
    hMM->Draw("P same");
    leg->Draw();
    // fitG->Draw("same");
    fitM->Draw("same");
    gPad->SetLeftMargin(0.1);
    gPad->SetRightMargin(0.03);
    c->SaveAs(Form("overlayResidue%s.png", det.c_str()));
}

void plotRes(std::string fnameMM, std::string det){
    TFile* fmm = new TFile(fnameMM.c_str(), "READ");

    TNtupleD* nt = (TNtupleD*) fmm->Get("nt");
    std::cout<<nt->GetEntries()<<std::endl;

    TH1F* hMM = new TH1F("hMM", "hMM", 200, -2., 2.);
    hMM->SetLineColor(kBlack);
    hMM->SetLineWidth(2);
    hMM->SetMarkerStyle(20);
    hMM->SetMarkerSize(0.5);
    hMM->SetMarkerColor(kBlack);
    nt->Draw("yres>>hMM");

    TLegend* leg = new TLegend(0.62, 0.75, 0.87, 0.85);
    
    TF1* fitM = new TF1("fitM", "gaus", -2., 2.);
    fitM->SetLineColor(kRed);
    fitM->SetNpx(3000);
    hMM->Fit(fitM, "R");
    leg->AddEntry(hMM, "data", "l");
    leg->AddEntry(hMM, Form("#mu=%.1f #sigma=%.3f", fitM->GetParameter(1), fitM->GetParameter(2)), "");

    TCanvas* c = new TCanvas("c", "c", 800, 800);
    hMM->SetTitle(Form("Detector %s", det.c_str()));
    hMM->GetXaxis()->SetTitle("residue [mm]");
    hMM->Draw("E");
    leg->Draw();
    fitM->Draw("same");
    gPad->SetLeftMargin(0.07);
    gPad->SetRightMargin(0.03);
    c->SaveAs(Form("overlayResidue%s_onlyRes.png", det.c_str()));
}


void plotG4(std::string fnameG4, std::string layer, std::string det){
    TFile* fgeant = new TFile(fnameG4.c_str(), "READ");

    TH1F* hgeant = (TH1F*)fgeant->Get(layer.c_str());
    hgeant->SetLineColor(kBlue);
    hgeant->SetLineWidth(2);
    hgeant->SetMarkerStyle(20);
    hgeant->SetMarkerSize(0.5);
    hgeant->SetMarkerColor(kBlue);
    hgeant->GetXaxis()->SetTitle("residue [mm]");

    TLegend* leg = new TLegend(0.62, 0.75, 0.87, 0.85);
    
    TF1* fitG = new TF1("fitG", "gaus", -2., 2.);
    fitG->SetNpx(3000);
    fitG->SetLineColor(kRed);
    hgeant->Fit(fitG, "R");
    leg->AddEntry(hgeant, "Geant4 msc", "l");
    leg->AddEntry(hgeant, Form("#mu=%.1f #sigma=%.2f", fitG->GetParameter(1), fitG->GetParameter(2)), "");

    TCanvas* c = new TCanvas("c", "c", 800, 800);
    hgeant->SetTitle(Form("Detector %s", det.c_str()));
    hgeant->GetXaxis()->SetTitle("residue [mm]");
    hgeant->Draw("E");
    leg->Draw();
    fitG->Draw("same");
    gPad->SetLeftMargin(0.07);
    gPad->SetRightMargin(0.03);
    c->SaveAs(Form("overlayResidue%s_onlyG4.png", det.c_str()));
}


int main(int argc, char* argv[]) {
    defStyle();
    // TFile* fgeant = new TFile("../geant4BeamTest/plots/beamTestGeant4_p0.00_i0.00_t0.00_s0.00.root", "READ");
    // TFile* fmm = new TFile("stripFEU1_POS08_z6_residues_residueFile.root", "READ");
    // TFile* fmm = new TFile("interFEU1_POS13_z1_residues_residueFile.root", "READ");
    // TFile* fmm = new TFile("asaFEU2_POS05_z3_residues_residueFile.root", "READ");
    // TFile* fmm = new TFile("asaFEU4_POS05_z3_residues_residueFile.root", "READ");

    plotResGeant("../geant4BeamTest/plots/beamTestGeant4_p0.00_i0.00_t0.00_s0.00.root", "stripFEU1_POS08_z6_residues_residueFile.root", "D1", "Layer0/resYLayer0");
    plotResGeant("../geant4BeamTest/plots/beamTestGeant4_p0.00_i0.00_t0.00_s0.00.root", "interFEU1_POS13_z1_residues_residueFile.root", "D2", "Layer1/resYLayer1");
    plotResGeant("../geant4BeamTest/plots/beamTestGeant4_p0.00_i0.00_t0.00_s0.00.root", "asaFEU2_POS05_z3_residues_residueFile.root", "D3", "Layer2/resYLayer2");
    plotResGeant("../geant4BeamTest/plots/beamTestGeant4_p0.00_i0.00_t0.00_s0.00.root", "asaFEU4_POS05_z3_residues_residueFile.root", "D4", "Layer4/resYLayer4");

    // plotRes("stripFEU1_POS08_z6_residues_residueFile.root", "D1");
    // plotRes("interFEU1_POS13_z1_residues_residueFile.root", "D2");
    // plotRes("asaFEU2_POS05_z3_residues_residueFile.root", "D3");
    // plotRes("asaFEU4_POS05_z3_residues_residueFile.root", "D4");

    // plotG4("../geant4BeamTest/plots/beamTestGeant4_p0.00_i0.00_t0.00_s0.00.root", "Layer0/resYLayer0", "D1");
    // plotG4("../geant4BeamTest/plots/beamTestGeant4_p0.00_i0.00_t0.00_s0.00.root", "Layer1/resYLayer1", "D2");
    // plotG4("../geant4BeamTest/plots/beamTestGeant4_p0.00_i0.00_t0.00_s0.00.root", "Layer2/resYLayer2", "D3");
    // plotG4("../geant4BeamTest/plots/beamTestGeant4_p0.00_i0.00_t0.00_s0.00.root", "Layer4/resYLayer4", "D4");
    return 0;
}
