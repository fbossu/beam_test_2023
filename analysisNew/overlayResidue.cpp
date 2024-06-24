#include "TNtupleD.h"
#include "analysis.h"
#include "../../../CLAS12/style_sheet.h"

int main(int argc, char* argv[]) {
    defStyle();
    TFile* fgeant = new TFile("../geant4BeamTest/plots/beamTestGeant4_p0.00_i0.00_t0.00_s0.00.root", "READ");
    TFile* fmm = new TFile("asaFEU2_POS05_z3_residues_residueFile.root", "READ");

    TH1F* hgeant = (TH1F*)fgeant->Get("Layer2/resXLayer2");
    hgeant->SetLineColor(kRed);
    hgeant->SetLineWidth(2);
    hgeant->GetXaxis()->SetTitle("residue [mm]");

    TNtupleD* nt = (TNtupleD*) fmm->Get("nt");
    std::cout<<nt->GetEntries()<<std::endl;

    TH1F* hMM = new TH1F("hMM", "hMM", 200, -2., 2.);
    hMM->SetLineColor(kBlue);
    hMM->SetLineWidth(2);
    nt->Draw("xres>>hMM");

    hgeant->Scale(1./hgeant->Integral(), "width");
    hMM->Scale(1./hMM->Integral(), "width");

    TLegend* leg = new TLegend(0.62, 0.75, 0.85, 0.85);
    leg->AddEntry(hgeant, "Geant4 msc", "l");
    leg->AddEntry(hMM, "data", "l");

    TCanvas* c = new TCanvas("c", "c", 1000, 800);
    hgeant->SetTitle("Detector D3");
    hgeant->Draw("hist");
    hMM->Draw("hist same");
    leg->Draw();
    c->SaveAs("overlayResidueD3.png");
    return 0;
}