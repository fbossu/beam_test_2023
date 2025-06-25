#include <iostream>
#include <string>
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "../reco/definitions.h"
#include "../banco/definition_banco.h"
#include "../map/StripTable.h"
#include "TLatex.h"

#include "analysis.h"

void defStyle(){
  // myStyle = (TStyle*)gStyle->Clone(); // copy the default style
  // myStyle = gROOT->GetStyle("Default");
  // TStyle* myStyle = new TStyle("Plain","Default Style");
    gStyle->SetName("myStyle");
    gStyle->SetTextFont(43);
    gStyle->SetTextSize(25);

    // Set the font and size for all axis labels
    gStyle->SetLabelFont(43, "XYZ"); // Set the font to Helvetica for the labels of the x-axis, y-axis, and z-axis
    gStyle->SetLabelSize(25, "XYZ"); // Set the font size for the labels of the x-axis, y-axis, and z-axis

    // Set the font and size for all axis titles
    gStyle->SetTitleFont(43, "XYZ"); // Set the font to Helvetica for the titles of the x-axis, y-axis, and z-axis
    gStyle->SetTitleSize(25, "XYZ"); // Set the font size for the titles of the x-axis, y-axis, and z-axis

    gStyle->SetTitleFont(43,"T"); // Set the font to Helvetica for the titles of the x-axis, y-axis, and z-axis
    gStyle->SetTitleSize(25,"T"); // Set the font size for the titles of the x-axis, y-axis, and z-axis

  // gROOT->SetStyle("myStyle");
  // gROOT->ForceStyle();
    // gStyle->SetPalette(kTemperatureMap);
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(0);
    gStyle->SetPadTopMargin(0.07);
    gStyle->SetPadBottomMargin(0.12);
    gStyle->SetPadLeftMargin(0.12);
    gStyle->SetPadRightMargin(0.12);

    gStyle->SetLineWidth(2);
    gStyle->SetFrameLineWidth(2);
    // gStyle->SetFuncWidth(2);
    gStyle->SetHistLineWidth(1);
}

int main(int argc, char* argv[]) {
    defStyle();
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << "<detname> <MMfile.root> ..." << std::endl;
        return 1;
    }

    std::string detName = argv[1];
    std::string fnameMM = argv[2];
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

    std::vector<double> xyout = xy_compareNoBanco(fnameMM, det, 2, "interCosmic_z2_xy_maxamp.png");

    std::cout << "clSize X: " << xyout[1] << ", clSize Y: " << xyout[2] << std::endl;
    std::cout << "ampFrac X: " << xyout[3] << ", ampFrac Y: " << xyout[4] << std::endl;
    
    return 0;
}