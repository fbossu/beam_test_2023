
void defStyle(){
  // myStyle = (TStyle*)gStyle->Clone(); // copy the default style
  // myStyle = gROOT->GetStyle("Default");
  // TStyle* myStyle = new TStyle("Plain","Default Style");
    gStyle->SetName("myStyle");
    gStyle->SetTextFont(43);
    gStyle->SetTextSize(25);

// Set the font and size for all axis labels
    /*TGaxis::SetMaxDigits(3);*/
    gStyle->SetLabelFont(42, "XYZ"); // Set the font to Helvetica for the labels of the x-axis, y-axis, and z-axis
    gStyle->SetLabelSize(0.045, "XYZ"); // Set the font size for the labels of the x-axis, y-axis, and z-axis

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
//    gROOT->ForceStyle();
}
