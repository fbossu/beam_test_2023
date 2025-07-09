#include <iostream>
#include <string>
#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "../reco/definitions.h"
#include "../banco/definition_banco.h"
#include "../map/StripTable.h"
#include "TLatex.h"
#include "TROOT.h"
#include "TGaxis.h"

#include "analysis.h"

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

std::map<int, std::vector<std::string>> parseFiles(std::map<int, std::string> zoneRuns, int argc, char* argv[]){
    std::map<int, std::string> filesBanco, filesMM;
    std::string fname, run;
    for (int i = 2; i < argc; i++) {
        fname = argv[i];
        if(fname.find("POS")!=std::string::npos) run = fname.substr(fname.find("POS"), 5);
        else if(fname.find("HVS")!=std::string::npos) run = "HVS";
        else{
            std::cout<<fname<<" is invalid"<<std::endl;
            continue;
        }
        auto it = std::find_if(zoneRuns.begin(), zoneRuns.end(), [run](const auto& it) {return it.second == run; });
        if(it != zoneRuns.end()){
            if(fname.find("FEU")!=std::string::npos) filesMM[it->first] = fname;
            else if(fname.find("banco")!=std::string::npos) filesBanco[it->first] = fname;
            else std::cout<<fname<<" is invalid"<<std::endl;
        }
    }
    std::map<int, std::vector<std::string>> files;
    for(auto it = filesBanco.begin(); it != filesBanco.end(); it++){
        if(filesMM.find(it->first) == filesMM.end()){
            std::cout<<"Missing MM file for zone "<<it->first<<std::endl;
            continue;
        }
        files[it->first].push_back(it->second);
        files[it->first].push_back(filesMM[it->first]);
    }
    return files;
}


int main(int argc, char* argv[]) {
    defStyle();

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << "<detname> <bancofile.root> <MMfile.root> ..." << std::endl;
        return 1;
    }

    std::string basedir = argv[0];
    basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";
    std::cout << basedir << std::endl;


    std::map<int, std::string> zoneRuns;
    std::string detMap;
    
    std::string detName = argv[1];

    if (detName.find("strip") != std::string::npos){
        zoneRuns = { {0,"POS16"}, {1,"POS14"}, {3,"POS11"}, {4,"POS13"}, {6,"POS08"}, {7,"POS06"}};
        detMap = "../map/strip_map.txt";
    }
    else if (detName.find("inter") != std::string::npos){
        zoneRuns = { {1,"POS13"}, {0,"POS11"}, {3,"POS06"}, {2,"POS08"} };
        detMap = "../map/inter_map.txt";
    }
    else if (detName.find("asa") != std::string::npos){
        zoneRuns = { {0,"POS11"}, {1,"POS13"}, {2,"POS02"}, {3,"POS05"}};
        detMap = "../map/asa_map.txt";
    }
    else {
        std::cerr << "Error: detector name not recognized" << std::endl;
        return 1;
    }

    std::map<int, std::vector<std::string>> files = parseFiles(zoneRuns, argc, argv);

    std::ofstream outfile;
    outfile.open(Form("%s_table_res_timeres.txt", detName.c_str()));
    outfile<<"#run\tzone\tgain"<<std::endl;
    outfile<<"#\t\tXpitch\tXinter\tXclsize\tXampF\tXres"<<std::endl;
    outfile<<"#\t\tYpitch\tYinter\tYclsize\tYampF\tYres"<<std::endl;

    StripTable det(detMap);

    for(auto it = files.begin(); it != files.end(); it++){
        std::string alignName = basedir + "../map/alignFiles/" + detName + "_" + zoneRuns[it->first] + ".txt";
        if(!det.SetAlignFile(alignName)) continue;
        std::cout<<"Zone "<<it->first<<" fMM "<<it->second[1]<<" fBanco "<<it->second[0]<<std::endl;

        // std::vector<double> xyout = xy_compare(it->second[0], it->second[1], det, it->first, Form("%s_%s_z%d_xy_maxamp.png", detName.c_str(), (zoneRuns[it->first]).c_str(), it->first));
        // std::vector<double> resXY = ResiduePlotAll(det, it->second[0], it->second[1], Form("%s_%s_z%d_residues", detName.c_str(), (zoneRuns[it->first]).c_str(), it->first));
        // clusterSizeFile(it->second[1], detName, det, it->first);
        // outfile<<it->second[0].substr(it->second[0].find("POS"), 5)<<"\t"<<it->first<<"\t"<<xyout[0]<<std::endl;
        // outfile<<"\t\t"<<det.pitchXzone(it->first)<<"\t"<<det.interXzone(it->first)<<"\t"<<xyout[1]<<"\t"<<xyout[3]<<"\t"<<resXY[0]<<std::endl;
        // outfile<<"\t\t"<<det.pitchYzone(it->first)<<"\t"<<det.interYzone(it->first)<<"\t"<<xyout[2]<<"\t"<<xyout[4]<<"\t"<<resXY[1]<<std::endl;   
        
        std::vector<double> tRes = timeRes(it->second[0], it->second[1], det, it->first, Form("%s_%s_z%d_timeRes.png", detName.c_str(), (zoneRuns[it->first]).c_str(), it->first));
        outfile<<it->second[0].substr(it->second[0].find("POS"), 5)<<"\t"<<it->first<<std::endl;
        outfile<<"\t\t"<<det.pitchXzone(it->first)<<"\t"<<det.interXzone(it->first)<<"\t"<<tRes[0]<<std::endl;
        outfile<<"\t\t"<<det.pitchYzone(it->first)<<"\t"<<det.interYzone(it->first)<<"\t"<<tRes[1]<<std::endl;   
    }

    // outfile.close();
    return 0;
}
