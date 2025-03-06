#include <string>
#include <numeric>

#include "TFile.h"
#include "TChain.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TTreeReader.h"
#include "TStyle.h"
#include "TLegend.h"
#include "TGraphErrors.h"
#include "TLatex.h"

#include "../reco/definitions.h"
#include "../banco/definition_banco.h"
#include "../map/StripTable.h"
#include "clusterSize.h"

int main(int argc, char const *argv[])
{

	std::map<std::string, double> HVasa;
	HV["HVS01"] = 300; HV["HVS02"] = 350; HV["HVS03"] = 360; HV["HVS04"] = 370; HV["HVS04Again"] = 380;
	HV["HVS05"] = 380; HV["HVS06"] = 390; HV["HVS07"] = 400; HV["HVS08"] = 410; HV["HVS09"] = 430; 
	HV["HVS10"] = 440; HV["HVS11"] = 450;

	std::map<std::string, double> HVstrip;
	HV["HVS01"] = 300; HV["HVS02"] = 350; HV["HVS03"] = 360; HV["HVS04"] = 370; HV["HVS04Again"] = 380;
	HV["HVS05"] = 380; HV["HVS06"] = 390; HV["HVS07"] = 400; HV["HVS08"] = 410; HV["HVS09"] = 410; 
	HV["HVS10"] = 410; HV["HVS11"] = 410;

	std::map<std::string, double> HVinter;
	HV["HVS01"] = 300; HV["HVS02"] = 350; HV["HVS03"] = 360; HV["HVS04"] = 370; HV["HVS04Again"] = 380;
	HV["HVS05"] = 380; HV["HVS06"] = 390; HV["HVS07"] = 400; HV["HVS08"] = 410; HV["HVS09"] = 420; 
	HV["HVS10"] = 430; HV["HVS11"] = 430;

	std::map<std::string, double> HVP2;
	HV["HVS01"] = 300; HV["HVS02"] = 350; HV["HVS03"] = 360; HV["HVS04"] = 370; HV["HVS04Again"] = 370;
	HV["HVS05"] = 380; HV["HVS06"] = 390; HV["HVS07"] = 400; HV["HVS08"] = 410; HV["HVS09"] = 410; 
	HV["HVS10"] = 410; HV["HVS11"] = 410;

	/*
	*/
	return 0;
}