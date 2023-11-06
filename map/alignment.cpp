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
#include "../analysis/clusterSize.h"
 
using namespace ROOT::Math;
 

bool first = true;

/*
Parameters:
	0 zpos, position in z of the detector
	1 Tx, translation de the detector along the x axis
	2 Ty, translation de the detector along the y axis
	3 rot, rotation of the detector around the z axis
*/

// function Object to be minimized
// struct XstripChi2 {

// 	std::vector<banco::track> tracks;
// 	std::vector<cluster> Xcls;
// 	StripTable det;

//    	XstripChi2(StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Xcls) : 
//   	det(det), tracks(tracks), Xcls(Xcls) {}
 
//    	// calculate distance line-point
//    	double chi2(banco::track tr, cluster cl, const double *p) {
//    		double ytr = tr.y0 + p[0]*tr.my;
//    		double res = ytr - (det.posX(cl.stripCentroid)[1] + p[1]);
//    		double err2 = pow(tr.ey0,2) + pow(tr.my*tr.emy,2) + pow(det.pitchX(int(cl.stripCentroid))/sqrt(12),2);
//    		// std::cout<<res*res<< " " <<err2<<" "<<(res*res)/err2<<std::endl;
//    		return (res*res)/err2;
//    	}
 
//    // implementation of the function to be minimized
//    	double operator() (const double *par) {
//     	double sum = 0;
//       	for (int i  = 0; i < tracks.size(); ++i) {
//         	 sum += chi2(tracks[i], Xcls[i], par);
//       	}
//       	if (first) {
//         	 std::cout << "Total Initial chi2 = " << sum << std::endl;
//       	}
//       	first = false;
//       	return sum;
//    	}

// };

// struct YstripChi2 {

// 	std::vector<banco::track> tracks;
// 	std::vector<cluster> Ycls;
// 	StripTable det;

//    	YstripChi2(StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Ycls) : 
//   	det(det), tracks(tracks), Ycls(Ycls) {}
 
//    	// calculate distance line-point
//    	double chi2(banco::track tr, cluster cl, const double *p) {
//    		double xtr = tr.x0 + p[0]*tr.mx;
//    		double res = xtr - (det.posY(cl.stripCentroid)[0] + p[1]);
//    		double err2 = pow(tr.ex0,2) + pow(tr.mx*tr.emx,2) + pow(det.pitchY(int(cl.stripCentroid))/sqrt(12),2);
//    		// std::cout<<res*res<< " " <<err2<<" "<<(res*res)/err2<<std::endl;
//    		return (res*res)/err2;
//    	}
 
//    // implementation of the function to be minimized
//    	double operator() (const double *par) {
//     	double sum = 0;
//       	for (int i = 0; i < tracks.size(); ++i) {
//         	 sum += chi2(tracks[i], Ycls[i], par);
//       	}
//       	if (first) {
//         	 std::cout << "Total Initial chi2 = " << sum << std::endl;
//       	}
//       	first = false;
//       	return sum;
//    	}

// };


// std::string alignX(int pos, StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Xcls, double *pStart, bool fixZ=false){
// 	ROOT::Fit::Fitter fitter;
// 	first = true;

// 	// make the functor objet
// 	XstripChi2 schi2(det, tracks, Xcls);
// 	ROOT::Math::Functor fcn(schi2,2);

// 	// set the function and the initial parameter values
// 	// double pStart[2] = *pStart;
// 	fitter.SetFCN(fcn, pStart);
// 	// set step sizes different than default ones (0.3 times parameter values)
// 	fitter.Config().ParSettings(0).SetStepSize(1.);
// 	fitter.Config().ParSettings(1).SetStepSize(0.05);
// 	if(fixZ) fitter.Config().ParSettings(0).Fix();

// 	bool ok = fitter.FitFCN();
// 	if (!ok) {
// 	std::cout<< "Fit Failed" <<std::endl;
// 	return "";
// 	}

// 	const ROOT::Fit::FitResult & result = fitter.Result();

// 	std::cout << "Total final chi2 " << result.MinFcnValue() << std::endl;
// 	result.Print(std::cout);
// 	return "X,"+std::to_string(pos)+","+std::to_string(result.Parameter(0))+","+std::to_string(result.ParError(0))+","
// 	       +std::to_string(result.Parameter(1))+","+std::to_string(result.ParError(1));
// }

// std::string alignY(int pos, StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Ycls, double *pStart, bool fixZ=false){
// 	ROOT::Fit::Fitter fitter;
// 	first = true;

// 	// make the functor objet
// 	YstripChi2 schi2(det, tracks, Ycls);
// 	ROOT::Math::Functor fcn(schi2,2);

// 	// set the function and the initial parameter values
// 	// double pStart[2] = *pStart;
// 	fitter.SetFCN(fcn, pStart);
// 	// set step sizes different than default ones (0.3 times parameter values)
// 	fitter.Config().ParSettings(0).SetStepSize(1.);
// 	fitter.Config().ParSettings(1).SetStepSize(0.05);
// 	if(fixZ) fitter.Config().ParSettings(0).Fix();

// 	bool ok = fitter.FitFCN();
// 	if (!ok) {
// 	std::cout<< "Fit Failed" <<std::endl;
// 	return "";
// 	}

// 	const ROOT::Fit::FitResult & result = fitter.Result();

// 	std::cout << "Total final chi2 " << result.MinFcnValue() << std::endl;
// 	result.Print(std::cout);
// 	return "Y,"+std::to_string(pos)+","+std::to_string(result.Parameter(0))+","+std::to_string(result.ParError(0))+","
// 	       +std::to_string(result.Parameter(1))+","+std::to_string(result.ParError(1));
// }



struct funcChi2 {

	std::vector<banco::track> tracks;
	std::vector<cluster> Xcls;
	std::vector<cluster> Ycls;
	StripTable det;

   	funcChi2(StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Xcls, std::vector<cluster> Ycls) : 
  	det(det), tracks(tracks), Xcls(Xcls) ,Ycls(Ycls) {}
 
   	// calculate distance line-point
   	double chi2(banco::track tr, cluster clX, cluster clY, const double *p) {
   		double xtr = tr.x0 + p[0]*tr.mx;
		double ytr = tr.y0 + p[0]*tr.my;

		double clxpos = det.posY(clY.stripCentroid)[0];
		double clypos = det.posX(clX.stripCentroid)[1];

		double xpos = (clxpos + p[1]) * cos(p[3]) - (clypos + p[2])* sin(p[3]);
		double ypos = (clxpos + p[1]) * sin(p[3]) + (clypos + p[2])* cos(p[3]);
		
   		double res = sqrt( pow(xtr - xpos,2) + pow(ytr - ypos,2) );
   		double errx2 = pow(tr.ex0,2) + pow(tr.mx*tr.emx,2) + pow(det.pitchY(int(clY.stripCentroid))/sqrt(12),2);
		double erry2 = pow(tr.ey0,2) + pow(tr.my*tr.emy,2) + pow(det.pitchX(int(clX.stripCentroid))/sqrt(12),2);

		double err2 = pow(xtr - xpos,2)/(res*res) * errx2 + pow(ytr - ypos,2)/(res*res) * erry2;
   		// std::cout<<res*res<< " " <<err2<<" "<<(res*res)/err2<<std::endl;
   		return (res*res)/err2;
   	}
 
   // implementation of the function to be minimized
   	double operator() (const double *par) {
    	double sum = 0;
      	for (int i = 0; i < tracks.size(); ++i) {
        	 sum += chi2(tracks[i], Xcls[i], Ycls[i], par);
      	}
      	if (first) {
        	 std::cout << "Total Initial chi2 = " << sum << std::endl;
      	}
      	first = false;
      	return sum;
   	}

};


std::string align(int pos, StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Xcls, std::vector<cluster> Ycls, double *pStart, bool fixZ=false){
	ROOT::Fit::Fitter fitter;
	first = true;

	// make the functor objet
	funcChi2 schi2(det, tracks, Xcls, Ycls);
	ROOT::Math::Functor fcn(schi2,4);

	// set the function and the initial parameter values
	fitter.SetFCN(fcn, pStart);
	// set step sizes different than default ones (0.3 times parameter values)
	fitter.Config().ParSettings(0).SetStepSize(1.);
	fitter.Config().ParSettings(1).SetStepSize(0.01);
	fitter.Config().ParSettings(2).SetStepSize(0.01);
	fitter.Config().ParSettings(3).SetStepSize(M_PI/360.);
	fitter.Config().ParSettings(3).SetLimits(-20*M_PI/180., 20*M_PI/180.);

	if(fixZ) fitter.Config().ParSettings(0).Fix();

	bool ok = fitter.FitFCN();
	if (!ok) {
	std::cout<< "Fit Failed" <<std::endl;
	return "";
	}

	const ROOT::Fit::FitResult & result = fitter.Result();

	std::cout << "Total final chi2 " << result.MinFcnValue() << std::endl;
	result.Print(std::cout);
	return "X,"+std::to_string(pos)+","+std::to_string(result.Parameter(0))+","+std::to_string(result.ParError(0))+","
	       +std::to_string(result.Parameter(1))+","+std::to_string(result.ParError(1));
}



int main(int argc, char const *argv[])
{
	std::string basedir = argv[0];
	basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";

	// StripTable det(basedir+"../map/strip_map.txt");
	StripTable det(basedir+"../map/asa_map.txt");
	double zpos = -785.6, rot = 0.;

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
	std::vector<cluster> XclsFit, YclsFit;
	double initTx = 0., initTy = 0.;
	int nev = 0;

  	while( MM.Next() ){
    	bool isBanco = banco.Next();
    	if(!isBanco){
      		std::cout<<"WARNING: Missing banco event"<<std::endl; 
      		continue;
    	}
		for(auto tr : *tracks){
			if(tr.chi2x>1. or tr.chi2y>1.) continue;

			std::vector<cluster> clsX, clsY;

			std::copy_if (cls->begin(), cls->end(), std::back_inserter(clsX),
						[](const cluster& c){return c.axis=='x';} );
			std::copy_if (cls->begin(), cls->end(), std::back_inserter(clsY),
						[](const cluster& c){return c.axis=='y';} );
			if(clsX.size() == 0 || clsY.size() == 0) continue;

			auto maxX = *std::max_element(clsX.begin(), clsX.end(),
								[](const cluster& a,const cluster& b) { return a.size < b.size; });
			auto maxY = *std::max_element(clsY.begin(), clsY.end(),
								[](const cluster& a,const cluster& b) { return a.size < b.size; });
			
			tracksFit.push_back(tr);
			XclsFit.push_back(maxX);
			YclsFit.push_back(maxY);

			nev++;
			initTx = tr.x0 + zpos*tr.mx - det.posY(maxY.stripCentroid)[0];
			initTy = tr.y0 + zpos*tr.my - det.posX(maxX.stripCentroid)[1];
		}
	}
	if(banco.Next()) std::cout<<"WARNING: Missing MM event"<<std::endl;

	std::cout<<"Initial parameters: "<<zpos<<" "<<initTx/nev<<" "<<initTy/nev<<" "<<rot<<std::endl;
	double pStart[4] = {zpos, initTx/nev, initTy/nev, rot};
	std::string out = align(pos, det, tracksFit, XclsFit, YclsFit, pStart, true);
	std::cout<<out<<std::endl;

	return 0;
}