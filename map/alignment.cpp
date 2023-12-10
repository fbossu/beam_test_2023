#include <TMath.h>
#include <TGraph2D.h>
#include <TRandom2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TF2.h>
#include <TH1.h>
#include <TLatex.h>
#include <TGraph.h>
#include <Math/Functor.h>
#include <Math/Minimizer.h>
#include <Math/MinimizerOptions.h>
#include <Minuit2/Minuit2Minimizer.h>
#include <Math/Factory.h>
#include <TPolyLine3D.h>
#include <Fit/Fitter.h>
#include "TTreeReader.h"
#include "TFile.h"
 
#include <Math/Transform3D.h>
#include <Math/Translation3D.h>
#include <Math/Rotation3D.h>
#include <Math/Vector3D.h>
#include <Math/RotationZYX.h>
#include <Math/PositionVector3D.h>
#include <Math/Point3D.h>

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
	3 rotZ, rotation of the detector around the z axis
	4 rotY, rotation of the detector around the y axis
	5 rotX, rotation of the detector around the x axis
*/

struct funcChi2 {

	std::vector<banco::track> tracks;
	std::vector<cluster> Xcls;
	std::vector<cluster> Ycls;
	std::vector<double> resVect;
	StripTable det;
	bool stdOpt = false;

	funcChi2(StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Xcls, std::vector<cluster> Ycls, bool stdOpt=false) : 
	det(det), tracks(tracks), Xcls(Xcls) ,Ycls(Ycls), stdOpt(stdOpt) {
		resVect.resize(tracks.size());
	}
	
	// calculate distance line-point
	double chi2(banco::track tr, cluster clX, cluster clY, const double *p) {

		double clxpos = det.posY(clY.stripCentroid)[0];
		double clypos = det.posX(clX.stripCentroid)[1];

		Rotation3D rot(RotationZYX(p[3], p[4], p[5])); // rotation around z, y, x
		Translation3D trl(p[1], p[2], p[0]);

		Transform3D trans(rot, trl);

		ROOT::Math::XYZPoint pdet(clxpos, clypos, 0.);
		// std::cout<<"det "<<pdet<<std::endl;
		ROOT::Math::XYZPoint pr = trans(pdet);
		// std::cout<<"r "<<pr<<std::endl;

		// XYZVector pr = trans * XYZVector(clxpos, clypos, 0.);

		// double xpos = (clxpos + p[1]) * cos(p[3]) - (clypos + p[2])* sin(p[3]);
		// double ypos = (clxpos + p[1]) * sin(p[3]) + (clypos + p[2])* cos(p[3]);

		double xtr = tr.x0 + pr.Z()*tr.mx;
		double ytr = tr.y0 + pr.Z()*tr.my;

		double res = sqrt( pow(xtr - pr.X(),2) + pow(ytr - pr.Y(),2) );
		// double res = ytr - pr.Y();
		// double errx2 = pow(tr.ex0,2) + pow(tr.mx*tr.emx,2) + pow(det.pitchY(int(clY.stripCentroid))/sqrt(12),2);
		// double erry2 = pow(tr.ey0,2) + pow(tr.my*tr.emy,2) + pow(det.pitchX(int(clX.stripCentroid))/sqrt(12),2);

		// double err2 = pow(xtr - pr.X(),2)/(res*res) * errx2 + pow(ytr - pr.Y(),2)/(res*res) * erry2;
		// std::cout<<res*res<< " " <<err2<<" "<<(res*res)/err2<<std::endl;
		// return (res*res)/err2;
		return res;
   	}
 
	double operator() (const double *par) {
		
		if(!stdOpt){
			double sum = 0;
			for (int i = 0; i < tracks.size(); ++i) {
				sum += pow( chi2(tracks[i], Xcls[i], Ycls[i], par), 2 )/0.001;
			}
			if (first) {
				std::cout << "Total Initial chi2 = " << sum << std::endl;
			}
			first = false;
			return sum;
		}
		else{
			for (int i = 0; i < tracks.size(); ++i) {
        		resVect[i] = chi2(tracks[i], Xcls[i], Ycls[i], par);
			}

			double sum = std::accumulate(resVect.begin(), resVect.end(), 0.0);
			double mean = sum / resVect.size();
			std::vector<double> diff(resVect.size());
			std::transform(resVect.begin(), resVect.end(), diff.begin(), [mean](double x) { return x - mean; });
			double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
			double stdev = std::sqrt(sq_sum / resVect.size());

			if (first) {
				std::cout << "Total Initial chi2 = " << sum << std::endl;
			}
			first = false;
			// std::cout<<"stdev "<<stdev<<std::endl;
			return stdev;
		}

   	}

};


struct funcChi2XY {

	std::vector<banco::track> tracks;
	std::vector<cluster> Xcls;
	std::vector<cluster> Ycls;
	std::vector<double> resVect;
	StripTable det;
	char axis;
	bool stdOpt = false;

	funcChi2XY(StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Xcls, std::vector<cluster> Ycls, char axis, bool stdOpt=false) : 
	det(det), tracks(tracks), Xcls(Xcls) ,Ycls(Ycls), stdOpt(stdOpt), axis(axis) {
		resVect.resize(tracks.size());
	}
	
	// calculate distance line-point
	double chi2(banco::track tr, cluster clX, cluster clY, const double *p) {

		double clxpos = det.posY(clY.stripCentroid)[0];
		double clypos = det.posX(clX.stripCentroid)[1];

		Rotation3D rot(RotationZYX(p[3], p[4], p[5])); // rotation around z, y, x
		Translation3D trl(p[1], p[2], p[0]);

		Transform3D trans(rot, trl);

		ROOT::Math::XYZPoint pdet(clxpos, clypos, 0.);
		ROOT::Math::XYZPoint pr = trans(pdet);

		double xtr = tr.x0 + pr.Z()*tr.mx;
		double ytr = tr.y0 + pr.Z()*tr.my;

		double res = 0;
		if(axis == 'y') res = xtr - pr.X();
		else if(axis == 'x') res = ytr - pr.Y();

		return res;
   	}
 
	double operator() (const double *par) {
		
		if(!stdOpt){
			double sum = 0;
			for (int i = 0; i < tracks.size(); ++i) {
				sum += pow( chi2(tracks[i], Xcls[i], Ycls[i], par), 2 )/0.001;
			}
			if (first) {
				std::cout << "Total Initial chi2 = " << sum << std::endl;
			}
			first = false;
			return sum;
		}
		else{
			for (int i = 0; i < tracks.size(); ++i) {
        		resVect[i] = chi2(tracks[i], Xcls[i], Ycls[i], par);
			}

			double sum = std::accumulate(resVect.begin(), resVect.end(), 0.0);
			double mean = sum / resVect.size();
			std::vector<double> diff(resVect.size());
			std::transform(resVect.begin(), resVect.end(), diff.begin(), [mean](double x) { return x - mean; });
			double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
			double stdev = std::sqrt(sq_sum / resVect.size());

			if (first) {
				std::cout << "Total Initial chi2 = " << sum << std::endl;
			}
			first = false;
			// std::cout<<"stdev "<<stdev<<std::endl;
			return stdev;
		}

   	}
};


double* align(std::string pos, StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Xcls, std::vector<cluster> Ycls, double *pStart, bool fixVars=false, bool stdOpt=false){
	
	first = true;
	const char * minName = "Minuit2";
	const char * algoName = "Migrad";

   	ROOT::Math::Minimizer* minimum = ROOT::Math::Factory::CreateMinimizer(minName, algoName);

	if (!minimum) {
		std::cerr << "Error: cannot create minimizer \"" << minName
				<< "\". Maybe the required library was not built?" << std::endl;
		// return "";
	}

	// set tolerance , etc...
	minimum->SetMaxFunctionCalls(1000000); // for Minuit/Minuit2
	minimum->SetMaxIterations(10000);  // for GSL
	minimum->SetTolerance(1e-6);
	// minimum->SetTolerance(100);
	minimum->SetPrintLevel(2);
 

	// make the functor objet
	funcChi2 schi2(det, tracks, Xcls, Ycls, stdOpt);
	ROOT::Math::Functor fcn(schi2, 6);
	minimum->SetFunction(fcn);

	// Set the free variables to be minimized !
	double step[6] = {0.05, 0.05, 0.05, 0.01*M_PI/180., 0.01*M_PI/180., 0.01*M_PI/180.};
	minimum->SetVariable(0,"zpos", pStart[0], step[0]);
   	minimum->SetVariable(1,"Tx", pStart[1], step[1]);
   	minimum->SetVariable(2,"Ty", pStart[2], step[2]);
	minimum->SetVariable(3,"rotZ", pStart[3], step[3]);
	minimum->SetVariable(4,"rotY", pStart[4], step[4]);
	minimum->SetVariable(5,"rotX", pStart[5], step[5]);
	if(fixVars){
		minimum->FixVariable(0);
		minimum->FixVariable(3);
		minimum->FixVariable(4);
		minimum->FixVariable(5);
	}
	// if(stdOpt){
	// 	minimum->FixVariable(1);
	// 	minimum->FixVariable(2);
	// 	minimum->FixVariable(3);
	// }

	double pLow[6] = {pStart[0]-100., pStart[1]-100., pStart[2]-100., pStart[3]-30.*M_PI/180., pStart[4]-30.*M_PI/180., pStart[5]-30.*M_PI/180.};
	double pUp[6]  = {pStart[0]+100., pStart[1]+100., pStart[2]+100., pStart[3]+30.*M_PI/180., pStart[4]+30.*M_PI/180., pStart[5]+30.*M_PI/180.};
	for(int i=0; i<6; i++){
		// minimum->SetVariableLimits(i, pLow[i], pUp[i]);
	}

	minimum->Minimize();
	const double* param = minimum->X();
	
   for(int i=0; i<6; i++){
		if(param[i]-pLow[i] < 1e-3){
			std::cout<<"Parameter "<<i<<"has reached lower limit"<<std::endl;
		}
		if(pUp[i]-param[i] < 1e-3){
			std::cout<<"Parameter "<<i<<"has reached upper limit"<<std::endl;
		}
    }

	double *pOut = new double[6];
	for(int i=0; i<6; i++){
		pOut[i] = param[i];
	}
	return pOut;
}


double* alignXY(std::string pos, StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Xcls, std::vector<cluster> Ycls, double *pStart, char axis){
	
	first = true;
	const char * minName = "Minuit2";
	const char * algoName = "Migrad";

   	ROOT::Math::Minimizer* minimum = ROOT::Math::Factory::CreateMinimizer(minName, algoName);

	if (!minimum) {
		std::cerr << "Error: cannot create minimizer \"" << minName
				<< "\". Maybe the required library was not built?" << std::endl;
		// return "";
	}

	// set tolerance , etc...
	minimum->SetMaxFunctionCalls(1000000); // for Minuit/Minuit2
	minimum->SetMaxIterations(10000);  // for GSL
	minimum->SetTolerance(0.0001);
	// minimum->SetTolerance(100);
	minimum->SetPrintLevel(2);
 
	// make the functor objet
	funcChi2XY schi2(det, tracks, Xcls, Ycls, axis);
	ROOT::Math::Functor fcn(schi2, 6);
	minimum->SetFunction(fcn);

	// Set the free variables to be minimized !
	double step[6] = {0.05, 0.05, 0.05, 0.005*M_PI/180., 0.005*M_PI/180., 0.005*M_PI/180.};
	minimum->SetVariable(0,"zpos", pStart[0], step[0]);
   	minimum->SetVariable(1,"Tx", pStart[1], step[1]);
   	minimum->SetVariable(2,"Ty", pStart[2], step[2]);
	minimum->SetVariable(3,"rotZ", pStart[3], step[3]);
	minimum->SetVariable(4,"rotY", pStart[4], step[4]);
	minimum->SetVariable(5,"rotX", pStart[5], step[5]);

	minimum->FixVariable(1);
	minimum->FixVariable(2);
	minimum->FixVariable(3);
	if(axis == 'x'){
		minimum->FixVariable(4);
	}else if(axis == 'y'){
		minimum->FixVariable(5);
	}

	double pLow[6] = {pStart[0]-100., pStart[1]-100., pStart[2]-100., pStart[3]-30.*M_PI/180., pStart[4]-30.*M_PI/180., pStart[5]-30.*M_PI/180.};
	double pUp[6]  = {pStart[0]+100., pStart[1]+100., pStart[2]+100., pStart[3]+30.*M_PI/180., pStart[4]+30.*M_PI/180., pStart[5]+30.*M_PI/180.};
	for(int i=0; i<6; i++){
		minimum->SetVariableLimits(i, pLow[i], pUp[i]);
	}

	minimum->Minimize();
	const double* param = minimum->X();
	
   for(int i=0; i<6; i++){
		if(param[i]-pLow[i] < 1e-3){
			std::cout<<"Parameter "<<i<<"has reached lower limit"<<std::endl;
		}
		if(pUp[i]-param[i] < 1e-3){
			std::cout<<"Parameter "<<i<<"has reached upper limit"<<std::endl;
		}
    }

	double *pOut = new double[6];
	for(int i=0; i<6; i++){
		pOut[i] = param[i];
	}
	return pOut;
}






double getXmin(TGraph* gr){
	double ymin = 1e9;
	double xmin = 1e9;
	for(int i=0; i<gr->GetN(); i++){
		double x, y;
		gr->GetPoint(i, x, y);
		if(y<ymin){
			ymin = y;
			xmin = x;
		}
	}
	return xmin;
}

double* get2DMin(TGraph2D* gr){
	double zmin = 1e9;
	double xmin = 1e9;
	double ymin = 1e9;
	for(int i=0; i<gr->GetN(); i++){
		double x, y, z;
		gr->GetPoint(i, x, y, z);
		if(z<zmin){
			zmin = z;
			xmin = x;
			ymin = y;
		}
	}
	double* out = new double[2];
	out[0] = xmin;
	out[1] = ymin;
	out[2] = zmin;
	return out;
}


double getRes(StripTable &det, std::vector<banco::track> &tracks, std::vector<cluster> &Xcls, std::vector<cluster> &Ycls, const double* p, char axis = 'o', std::string plot = ""){
	std::vector<double> resX(tracks.size());
	std::vector<double> resY(tracks.size());
	double resX_avg = 0.;
	double resY_avg = 0.;
	std::cout<<"p "<<p[0]<<" "<<p[1]<<" "<<p[2]<<" "<<p[3]<<" "<<p[4]<<" "<<p[5]<<std::endl;
	det.setTransform(p[0], p[1], p[2], p[3], p[4], p[5]);
	
	for(int j=0; j<tracks.size(); j++){
		std::vector<double> posdet = det.pos3D(Xcls[j].stripCentroid, Ycls[j].stripCentroid);
		double xtr = tracks[j].x0 + posdet[2]*tracks[j].mx;
		double ytr = tracks[j].y0 + posdet[2]*tracks[j].my;
		resX[j] = ytr - posdet[1];
		resY[j] = xtr - posdet[0];

		resX_avg += resX[j];
		resY_avg += resY[j];
	}
	TH1F* hresX; TH1F* hresY;
	TF1* fitX; TF1* fitY;
	if(axis != 'y'){
		hresX = new TH1F("hresX", "", 1000, resX_avg/resX.size()-2., resX_avg/resX.size()+2);
		for(int j=0; j<resX.size(); j++){
			hresX->Fill(resX[j]);
		}
		fitX = new TF1("fitX", "gaus", resX_avg/resX.size()-2., resX_avg/resX.size()+2);
		fitX->SetParameters(100, resX_avg/resX.size(), 0.1);
		hresX->Fit(fitX, "R");
	}
	if(axis != 'x'){
		hresY = new TH1F("hresY", "", 1000, resY_avg/resY.size()-2., resY_avg/resY.size()+2);
		for(int j=0; j<resY.size(); j++){
			hresY->Fill(resY[j]);
		}
		fitY = new TF1("fitY", "gaus", resY_avg/resY.size()-2., resY_avg/resY.size()+2);
		fitY->SetParameters(100, resY_avg/resY.size(), 0.1);
		hresY->Fit(fitY, "R");
	}

	if(plot!=""){
		TCanvas* c1 = new TCanvas("c1", "c1", 1600, 1200);
		if(axis == 'x') hresX->Draw();
		else if(axis == 'y') hresY->Draw();
		else{
			hresX->Draw();
			hresY->SetLineColor(kRed);
			hresY->Draw("same");
		}
		c1->Print(plot.c_str(), ".png");
	}

	if(axis == 'x') return fitX->GetParameter(2);
	else if(axis == 'y') return fitY->GetParameter(2);
	else return sqrt( pow(fitX->GetParameter(2),2) + pow(fitY->GetParameter(2),2));
}


void zRotAlign(std::string graphName, StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Xcls, std::vector<cluster> Ycls, const double* p, char axis = 'o'){
	
	TGraph2D* grSigma = new TGraph2D();
	double rotStart;
	if(axis == 'x') rotStart = p[5];
	else rotStart = p[4];
	funcChi2 schi2(det, tracks, Xcls, Ycls);

	for(double z=p[0]-80; z<p[0]+120; z+=10){
	// for(double rotX=rotStart-0.4; rotX<rotStart+0.4; rotX+=0.05){
		for(double rot=rotStart-0.4; rot<rotStart+0.4; rot+=0.05){
			double pRes[6] = {z, p[1], p[2], p[3], p[4], p[5]};
			if(axis == 'x') pRes[5] = rot;
			else pRes[4] = rot;
			// double sigma = getRes(det, tracks, Xcls, Ycls, pRes);
			// grSigma->SetPoint(grSigma->GetN(), z, rot, sigma);
			grSigma->SetPoint(grSigma->GetN(), z, rot, schi2(pRes));
		}
	}
	double* min = get2DMin(grSigma);
	// TF1* fitFunc = new TF1("fitFunc", "pol2", xmin-40, xmin+40);
	// fitFunc->SetParLimits(2, 0., 100);
	// grSigma->Fit(fitFunc, "R");
	// double zout = -fitFunc->GetParameter(1)/(2*fitFunc->GetParameter(2));	
	
	TCanvas* c1 = new TCanvas("c1", "c1", 1600, 1200);
	grSigma->SetMarkerStyle(20);
	grSigma->SetMarkerSize(0.8);
	grSigma->SetMarkerColor(kBlue);
	grSigma->SetTitle("z alignment");
	grSigma->GetXaxis()->SetTitle("z position [mm]");
	grSigma->GetYaxis()->SetTitle("rot [rad]");
	grSigma->GetZaxis()->SetTitle("sigma [mm]");
	grSigma->Draw("surf1");

	TLatex* latex = new TLatex();
	latex->SetTextSize(0.03);
	latex->SetTextColor(kRed);
	latex->DrawLatexNDC(0.2, 0.22, Form("zmin = %.2f", min[0]));
	latex->DrawLatexNDC(0.2, 0.19, Form("%cRotmin = %.2f", axis, min[1]));
	gStyle->SetOptFit(1111);
	
	c1->SaveAs(graphName.c_str());

	// double pRes[6] = {zout, p[1], p[2], p[2], p[4], p[5]};
	// double sigma = getRes(det, tracks, Xcls, Ycls, pRes, axis, "minZ_"+graphName);

	// return zout;
}

double zAlign(std::string graphName, StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Xcls, std::vector<cluster> Ycls, const double* p, char axis = 'o'){
	
	TGraph* grSigma = new TGraph();
	for(double z=p[0]-100; z<p[0]+140; z+=3){
		std::cout<<"z "<<z<<std::endl;
		double pRes[6] = {z, p[1], p[2], p[3], p[4], p[5]};
		double sigma = getRes(det, tracks, Xcls, Ycls, pRes, axis);
		grSigma->SetPoint(grSigma->GetN(), z, sigma);
	}
	double xmin = getXmin(grSigma);
	TF1* fitFunc = new TF1("fitFunc", "pol2", xmin-40, xmin+40);
	fitFunc->SetParLimits(2, 0., 100);
	grSigma->Fit(fitFunc, "R");
	double zout = -fitFunc->GetParameter(1)/(2*fitFunc->GetParameter(2));	
	
	TCanvas* c1 = new TCanvas("c1", "c1", 1600, 1200);
	grSigma->SetMarkerStyle(20);
	grSigma->SetMarkerSize(0.8);
	grSigma->SetMarkerColor(kBlue);
	grSigma->SetTitle("z alignment");
	grSigma->GetXaxis()->SetTitle("z position [mm]");
	grSigma->GetYaxis()->SetTitle("sigma [mm]");
	grSigma->Draw("AP");

	TLatex* latex = new TLatex();
	latex->SetTextSize(0.03);
	latex->SetTextColor(kRed);
	latex->DrawLatexNDC(0.2, 0.2, Form("zpos = %.2f", zout));
	gStyle->SetOptFit(1111);
	
	c1->SaveAs(graphName.c_str());

	double pRes[6] = {zout, p[1], p[2], p[3], p[4], p[5]};
	double sigma = getRes(det, tracks, Xcls, Ycls, pRes, axis, "minZ_"+graphName);

	return zout;
}
	
double yAlign(std::string graphName, StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Xcls, std::vector<cluster> Ycls, const double* p, char axis = 'o'){
	
	TGraph* grSigma = new TGraph();
	for(double yRot=p[4]-0.4; yRot<p[4]+0.4; yRot+=0.005){
		double pRes[6] = {p[0], p[1], p[2], p[3], yRot, p[5]};
		double sigma = getRes(det, tracks, Xcls, Ycls, pRes, axis);
		grSigma->SetPoint(grSigma->GetN(), yRot, sigma);
	}
	double xmin = getXmin(grSigma);
	std::cout<<"Y xmin "<<xmin<<std::endl;

	TF1* fitFunc = new TF1("fitFunc", "pol2", xmin-0.1, xmin+0.1);
	fitFunc->SetParameters(0.1, 0.1, 0.1);
	fitFunc->SetParLimits(2, 0., 100);

	grSigma->Fit(fitFunc, "R");
	double yRotOut = -fitFunc->GetParameter(1)/(2*fitFunc->GetParameter(2));	
	
	TCanvas* c1 = new TCanvas("c1", "c1", 1600, 1200);
	grSigma->SetMarkerStyle(20);
	grSigma->SetMarkerSize(0.8);
	grSigma->SetMarkerColor(kBlue);
	grSigma->SetTitle("yRot alignment");
	grSigma->GetXaxis()->SetTitle("yRot position [rad]");
	grSigma->GetYaxis()->SetTitle("sigma [mm]");
	// grSigma->GetYaxis()->SetRangeUser(0.09, 0.3);
	grSigma->Draw("AP");

	TLatex* latex = new TLatex();
	latex->SetTextSize(0.03);
	latex->SetTextColor(kRed);
	latex->DrawLatexNDC(0.2, 0.2, Form("yRot = %.2f", yRotOut));
	gStyle->SetOptFit(1111);
	
	c1->SaveAs(graphName.c_str());

	double pRes[6] = {p[0], p[1], p[2], p[3], xmin, p[5]};
	double sigma = getRes(det, tracks, Xcls, Ycls, pRes, axis, "minyRot_"+graphName);

	return yRotOut;
}


double xAlign(std::string graphName, StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Xcls, std::vector<cluster> Ycls, const double* p, char axis = 'o'){
	TGraph* grSigma = new TGraph();
	
	for(double xRot=p[5]-0.4; xRot<p[5]+0.4; xRot+=0.005){
		double pRes[6] = {p[0], p[1], p[2], p[3], p[4], xRot};
		double sigma = getRes(det, tracks, Xcls, Ycls, pRes, axis);
		grSigma->SetPoint(grSigma->GetN(), xRot, sigma);
	}

	double xmin = getXmin(grSigma);
	std::cout<<"X xmin "<<xmin<<std::endl;

	TF1* fitFunc = new TF1("fitFunc", "pol2", xmin-0.1, xmin+0.1);
	fitFunc->SetParameters(0.1, 0.1, 0.1);
	fitFunc->SetParLimits(2, 0., 100);
	grSigma->Fit(fitFunc, "R");
	double xRotOut = -fitFunc->GetParameter(1)/(2*fitFunc->GetParameter(2));	
	
	TCanvas* c1 = new TCanvas("c1", "c1", 1600, 1200);
	grSigma->SetMarkerStyle(20);
	grSigma->SetMarkerSize(0.8);
	grSigma->SetMarkerColor(kBlue);
	grSigma->SetTitle("xRot alignment");
	grSigma->GetXaxis()->SetTitle("xRot position [rad]");
	grSigma->GetYaxis()->SetTitle("sigma [mm]");
	// grSigma->GetYaxis()->SetRangeUser(0.09, 0.3);
	grSigma->Draw("AP");

	TLatex* latex = new TLatex();
	latex->SetTextSize(0.03);
	latex->SetTextColor(kRed);
	latex->DrawLatexNDC(0.2, 0.2, Form("xRot = %.2f", xRotOut));
	gStyle->SetOptFit(1111);
	
	c1->SaveAs(graphName.c_str());

	double pRes[6] = {p[0], p[1], p[2], p[3], p[4], xmin};
	double sigma = getRes(det, tracks, Xcls, Ycls, pRes, axis, "minxRot_"+graphName);

	return xRotOut;
}



void globalMinima(StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Xcls, std::vector<cluster> Ycls, double *p){
	
	funcChi2 schi2(det, tracks, Xcls, Ycls);
	double zmin, rotXmin, rotYmin, rotZmin;
	double distMin = 1e9;

	for(double z=p[0]-50; z<p[0]+50; z+=10){
		std::cout<<"z "<<z<<std::endl;
		for(double rotX=p[5]-0.3; rotX<p[5]+0.3; rotX+=0.1){
			for(double rotY=p[4]-0.3; rotY<p[4]+0.3; rotY+=0.1){
				for(double rotZ=p[3]-0.3; rotZ<p[3]+0.3; rotZ+=0.1){
					double pRes[6] = {z, p[1], p[2], rotZ, rotY, rotX};
					double dist = schi2(pRes);
					if(dist < distMin){
						distMin = dist;
						zmin = z;
						rotXmin = rotX;
						rotYmin = rotY;
						rotZmin = rotZ;
					}
				}
			}
		}
	}
	std::cout<<"zmin "<<zmin<<" rotXmin "<<rotXmin<<" rotYmin "<<rotYmin<<" rotZmin "<<rotZmin<<std::endl;
}



int main(int argc, char const *argv[])
{
	if (argc < 4) {
		std::cerr << "Usage: " << argv[0] << "<detname> <bancoFile.root> <MMFile.root>" << std::endl;
		return 1;
	}

	std::string basedir = argv[0];
	basedir = basedir.substr(0, basedir.find_last_of("/")) + "/";
	std::string detName = argv[1];
	std::string fnameBanco =  argv[2];
	std::string fnameMM =  argv[3];

	std::string mapName;
	// double zpos = 0., rotZ = 0., rotY = -0.15, rotX = 0.088;
	double zpos = 0., rotZ = 0., rotY = 0.0, rotX = 0.0;

	if (detName == "asaFEU4") {
		mapName = "asa_map.txt";
		zpos = -785.6;
	} else if (detName == "asaFEU2") {
		mapName = "asa_map.txt";
		zpos = -545.6;
	} else if (detName == "stripFEU1") {
		mapName = "strip_map.txt";
		zpos = -305.6;
	} else if (detName == "interFEU1") {
		mapName = "inter_map.txt";
		zpos = -425.6;
	} else {
		std::cerr << "Invalid detector name" << std::endl;
		return 1;
	}
	StripTable det(basedir+"../map/"+mapName);
	
	std::string run = fnameMM.substr(fnameMM.find("POS"), 5);
	int pos = std::stoi( fnameMM.substr(fnameMM.find("POS")+3, 2) );
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
		if(tracks->size() == 0 or cls->size() == 0) continue;

    	auto tr = *std::min_element(tracks->begin(), tracks->end(),
                       [](const banco::track& a,const banco::track& b) { return a.chi2x+a.chi2y < b.chi2x+b.chi2y; });
    	
		if(tr.chi2x>1. or tr.chi2y>1.) continue;
    	
		auto maxX = maxSizeClX(*cls);
    	auto maxY = maxSizeClY(*cls);
		if(!maxX or !maxY) continue;
		// if(maxX->size < 2 or maxY->size < 2) continue;
		
		tracksFit.push_back(tr);
		XclsFit.push_back(*maxX);
		YclsFit.push_back(*maxY);

		nev++;
		initTx += tr.x0 + zpos*tr.mx - det.posY(maxY->stripCentroid)[0];
		initTy += tr.y0 + zpos*tr.my - det.posX(maxX->stripCentroid)[1];
		if(nev>100000) break;
	}
	if(banco.Next()) std::cout<<"WARNING: Missing MM event"<<std::endl;

	// Removing noise, only keeping events whithin 1cm of the average beam position
	double Xavg = std::accumulate(XclsFit.begin(), XclsFit.end(), 0.0,
		[&](double sum, const cluster& cls) { return sum + det.posX(cls.stripCentroid)[1]; })/nev;
	double Yavg = std::accumulate(YclsFit.begin(), YclsFit.end(), 0.0,
		[&](double sum, const cluster& cls) { return sum + det.posY(cls.stripCentroid)[0]; })/nev;
	
	for(int i=0; i<tracksFit.size(); i++){
		double dist = sqrt( pow(Yavg - det.posY(YclsFit[i].stripCentroid)[0], 2) + 
				  pow(Xavg - det.posX(XclsFit[i].stripCentroid)[1], 2) );
		std::cout<<"dist "<<dist<<std::endl;
		if( dist> 10. ){
			tracksFit.erase(tracksFit.begin()+i);
			XclsFit.erase(XclsFit.begin()+i);
			YclsFit.erase(YclsFit.begin()+i);
			i--;
		}
	}

	if(tracksFit.size() < 10000) {
		std::cerr << "Error: Not enough events: " << tracksFit.size() << std::endl;
		return 1;
	}
	std::cout<<"Number of events: "<<nev<<std::endl;
	std::cout<<"After noise is removed: "<<tracksFit.size()<<std::endl;

	double pStart[6] = {zpos, initTx/nev, initTy/nev, rotZ, rotY, rotX};
	// double z0 = zAlign(Form("zAlign_z0_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pStart);
	// pStart[0] = -305.6;
	// pStart[3] = -M_PI/2.;
	// pStart[4] = M_PI;
	// pStart[5] = 0.;
	std::cout<<"Initial parameters: "<<pStart[0]<<" "<<pStart[1]<<" "<<pStart[2]<<" "<<pStart[3]<<" "<<pStart[4]<<" "<<pStart[5]<<std::endl;

	double* pTrl = align(run, det, tracksFit, XclsFit, YclsFit, pStart, true);
	// globalMinima(det, tracksFit, XclsFit, YclsFit, pTrl);
	// double* pEnd = align(run, det, tracksFit, XclsFit, YclsFit, pTrl, false, true);

	double zout    = zAlign(Form("zAlign_trlz0_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pTrl);
	double rotYout = yAlign(Form("yAlign_trlz0_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pTrl);
	double rotXout = xAlign(Form("xAlign_trlz0_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pTrl);
	// std::cout<<"Final Trl: "<<pTrl[0]<<" "<<pTrl[1]<<" "<<pTrl[2]<<" "<<rotYout<<" "<<rotXout<<std::endl;
	std::cout<<"Final Trl: "<<pTrl[0]<<" "<<pTrl[1]<<" "<<pTrl[2]<<" "<<pTrl[3]<<" "<<pTrl[4]<<" "<<pTrl[4]<<std::endl;
	// std::cout<<getRes(det, tracksFit, XclsFit, YclsFit, pTrl, 'x', "test_"+detName+"_"+run+".png")<<std::endl;
	
	zRotAlign(Form("zRotXAlign_funcchi2_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pTrl, 'x');
	zRotAlign(Form("zRotYAlign_funcchi2_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pTrl, 'y');


	double Xzout    = zAlign(Form("zAlign_resx_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pTrl, 'x');
	double XrotYout = yAlign(Form("yAlign_resx_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pTrl, 'x');
	double XrotXout = xAlign(Form("xAlign_resx_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pTrl, 'x');

	double Yzout    = zAlign(Form("zAlign_resy_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pTrl, 'y');
	double YrotYout = yAlign(Form("yAlign_resy_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pTrl, 'y');
	double YrotXout = xAlign(Form("xAlign_resy_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pTrl, 'y');


	// double pStart2[6] = {zout, pTrl[1], pTrl[2], pTrl[3], rotYout, rotXout};
	// double* pEnd = align(run, det, tracksFit, XclsFit, YclsFit, pStart2, false);
	// std::cout<<"Final: "<<pEnd[0]<<" "<<pEnd[1]<<" "<<pEnd[2]<<" "<<pEnd[3]<<" "<<pEnd[4]<<" "<<pEnd[5]<<std::endl;
	
	// zout    = zAlign(Form("zAlign_end_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pEnd);
	// rotYout = yAlign(Form("yAlign_end_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pEnd);
	// rotXout = xAlign(Form("xAlign_end_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pEnd);

	// zRotAlign(Form("zRotXAlign_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pTrl, 'x');
	// zRotAlign(Form("zRotYAlign_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pTrl, 'y');

	// double* pX = alignXY(run, det, tracksFit, XclsFit, YclsFit, pTrl, 'x');
	// std::cout<<"X "<<pX[0]<<" "<<pX[1]<<" "<<pX[2]<<" "<<pX[3]<<" "<<pX[4]<<" "<<pX[5]<<std::endl;
	// double* pY = alignXY(run, det, tracksFit, XclsFit, YclsFit, pTrl, 'y');
	// std::cout<<"Y "<<pY[0]<<" "<<pY[1]<<" "<<pY[2]<<" "<<pY[3]<<" "<<pY[4]<<" "<<pY[5]<<std::endl;

	// double Xzout    = zAlign(Form("zAlign_resx_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pX, 'x');
	// double XrotYout = yAlign(Form("yAlign_resx_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pX, 'x');
	// double XrotXout = xAlign(Form("xAlign_resx_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pX, 'x');

	// double Yzout    = zAlign(Form("zAlign_resy_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pY, 'y');
	// double YrotYout = yAlign(Form("yAlign_resy_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pY, 'y');
	// double YrotXout = xAlign(Form("xAlign_resy_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pY, 'y');

	// std::cout<<"X "<<pX[0]<<" "<<pX[1]<<" "<<pX[2]<<" "<<pX[3]<<" "<<pX[4]<<" "<<pX[5]<<std::endl;	
	// std::cout<<"Y "<<pY[0]<<" "<<pY[1]<<" "<<pY[2]<<" "<<pY[3]<<" "<<pY[4]<<" "<<pY[5]<<std::endl;
	
	// double pStart2[6] = {(Xzout+Yzout)/2., pTrl[1], pTrl[2], pTrl[3], YrotYout, XrotXout};
	// double* pEnd2 = align(run, det, tracksFit, XclsFit, YclsFit, pStart2, false);

	// double zout    = zAlign(Form("zAlign_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pEnd2);
	// double rotYout = yAlign(Form("yAlign_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pEnd2);
	// double rotXout = xAlign(Form("xAlign_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pEnd2);

	// const double* pEnd2 = align(run, det, tracksFit, XclsFit, YclsFit, pStart2, false, true);

	// double pStart[6] = {zout, initTx/nev, initTy/nev, rotZ, rotY, rotX};


	// std::ofstream outfile("alignFiles/"+ detName + "_" + run + ".txt");
	// // std::ofstream outfile("test.txt");
	// outfile << "# POS zpos Tx Ty rot(z y x)\n";
	// outfile << Form("%s %f %f %f %f %f %f", run.c_str(), pEnd2[0], pEnd2[1], pEnd2[2], pEnd2[3], pEnd2[4], pEnd2[5]) << std::endl;
	// outfile.close();

	// std::string out = "# POS zpos Tx Ty rot(x y x)\n# POS ezpos eTx eTy erot\n";
	// out += Form("%s %f %f %f %f \n", pos.c_str(), result.Parameter(0), result.Parameter(1), result.Parameter(2), result.Parameter(3));
	// out += Form("%s %f %f %f %f \n", pos.c_str(), result.ParError(0), result.ParError(1), result.ParError(2), result.ParError(3));
	// out += Form("%s %f %f %f %f %f %f\n", pos.c_str(), result.Parameter(0), result.Parameter(1), result.Parameter(2), result.Parameter(3), result.Parameter(4), result.Parameter(5));
	// out += Form("%s %f %f %f %f %f %f\n", pos.c_str(), result.ParError(0), result.ParError(1), result.ParError(2), result.ParError(3), result.ParError(4), result.ParError(5));
	// return out;

	return 0;
}