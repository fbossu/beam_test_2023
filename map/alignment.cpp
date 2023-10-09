#include <TMath.h>
#include <TGraph2D.h>
#include <TRandom2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TF2.h>
#include <TH1.h>
#include <Math/Functor.h>
#include <TPolyLine3D.h>
#include <Math/Vector3D.h>
#include <Fit/Fitter.h>
#include "TTreeReader.h"
#include "TFile.h"

#include "../reco/definitions.h"
#include "../banco/definition_banco.h"
#include "../map/StripTable.h"
 
using namespace ROOT::Math;
 

bool first = true;

/*
Parameters:
	0 zpos, position in z of the detector
	1 Ty, translation de the detector along the y axis
*/

// function Object to be minimized
struct XstripChi2 {

	std::vector<banco::track> tracks;
	std::vector<cluster> Xcls;
	StripTable det;

   	XstripChi2(StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Xcls) : 
  	det(det), tracks(tracks), Xcls(Xcls) {}
 
   	// calculate distance line-point
   	double chi2(banco::track tr, cluster cl, const double *p) {
   		double ytr = tr.y0 + p[0]*tr.my;
   		double res = ytr - (det.posX(cl.stripCentroid)[1] - p[1]);
   		double err2 = pow(tr.ey0,2) + pow(tr.my*tr.emy,2) + pow(det.pitchX(int(cl.stripCentroid))/sqrt(12),2);
   		// std::cout<<res*res<< " " <<err2<<" "<<(res*res)/err2<<std::endl;
   		return (res*res)/err2;
   	}
 
   // implementation of the function to be minimized
   	double operator() (const double *par) {
    	double sum = 0;
      	for (int i  = 0; i < tracks.size(); ++i) {
        	 sum += chi2(tracks[i], Xcls[i], par);
      	}
      	if (first) {
        	 std::cout << "Total Initial chi2 = " << sum << std::endl;
      	}
      	first = false;
      	return sum;
   	}

};

int main(int argc, char const *argv[])
{
	std::string basedir = argv[0];
	basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";

	StripTable det(basedir+"../map/strip_map.txt");
	// StripTable det(basedir+"../map/asa_map.txt");
	// double zpos = -305.6, Ty = 94;
	double zpos = -305.6, Ty = 10;
  	// double zpos = -785.6, Ty = -94;

	std::string fnameBanco =  argv[1];
	std::string fnameMM =  argv[2];

	int pos = std::stoi( fnameMM.substr(fnameMM.find("POS")+3, fnameMM.find("POS")+5) );
	std::cout << "Position: " << pos << std::endl;

	TFile *fbanco = TFile::Open(fnameBanco.c_str(), "read");
	TFile *fMM = TFile::Open(fnameMM.c_str(), "read");

	TTreeReader MM("events", fMM);
	TTreeReader banco("events", fbanco);

	TTreeReaderValue< std::vector<cluster> > cls( MM, "clusters");
	TTreeReaderValue< std::vector<banco::track> > tracks( banco, "tracks");

	std::vector<banco::track> tracksFit;
	std::vector<cluster> XclsFit;

	while( MM.Next() ){
		bool isBanco = banco.Next();
		if(!isBanco){
			std::cout<<"WARNING: Missing banco event"<<std::endl; 
			continue;
		}
		banco::track tr;
		cluster Xcl; Xcl.stripCentroid=-1;
		int maxSizeX = 0;

		for( auto c : *cls ){
			if( c.axis == 'x' ){
				if(c.size>maxSizeX) {
				  Xcl = c;
				  maxSizeX = c.size;
				}
			}
		}
		if(Xcl.stripCentroid > 0){
			for(auto tr : *tracks){
				tracksFit.push_back(tr);
				XclsFit.push_back(Xcl);
			}
		}
	}

	if(banco.Next()) std::cout<<"WARNING: Missing MM event"<<std::endl;

	ROOT::Fit::Fitter fitter;

	// make the functor objet
	XstripChi2 schi2(det, tracksFit, XclsFit);
	ROOT::Math::Functor fcn(schi2,2);

	// set the function and the initial parameter values
	double pStart[2] = {zpos,Ty};
	fitter.SetFCN(fcn, pStart);
	// set step sizes different than default ones (0.3 times parameter values)
	fitter.Config().ParSettings(0).SetStepSize(1.);
	fitter.Config().ParSettings(1).SetStepSize(0.05);

	bool ok = fitter.FitFCN();
	if (!ok) {
	std::cout<< "Fit Failed" <<std::endl;
	return 1;
	}

	const ROOT::Fit::FitResult & result = fitter.Result();

	std::cout << "Total final chi2 " << result.MinFcnValue() << std::endl;
	result.Print(std::cout);

	return 0;
}