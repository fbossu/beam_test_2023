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

double getRes(StripTable &det, std::vector<banco::track> &tracks, std::vector<cluster> &Xcls, std::vector<cluster> &Ycls, const double* p, std::string plot = ""){
	std::vector<double> res;
	double res_avg = 0.; int nres = 0;
	det.setTransform(p[0], p[1], p[2], p[3], p[4], p[5]);
	for(int j=0; j<tracks.size(); j++){
		std::vector<double> posdet = det.pos3D(Xcls[j].stripCentroid, Ycls[j].stripCentroid);
		double xtr = tracks[j].x0 + posdet[2]*tracks[j].mx;
		double ytr = tracks[j].y0 + posdet[2]*tracks[j].my;
		res.push_back(sqrt( pow(xtr - posdet[0],2) + pow(ytr - posdet[1],2) ));
		res_avg += sqrt( pow(xtr - posdet[0],2) + pow(ytr - posdet[1],2) );
		nres++;
	}
	TH1F* hres = new TH1F("hres", "", 1000, res_avg/nres-2., res_avg/nres+2);
	for(int j=0; j<nres; j++){
		hres->Fill(res[j]);
	}
	TF1* fitFunc = new TF1("fitFunc", "gaus", -2, 2);
	hres->Fit(fitFunc, "R");

	if(plot!=""){
		TCanvas* c1 = new TCanvas("c1", "c1", 1600, 1200);
		hres->Draw();
		fitFunc->Draw("same");
		c1->Print(plot.c_str(), ".png");
	}

	return fitFunc->GetParameter(2);
}

double zAlign(std::string graphName, StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Xcls, std::vector<cluster> Ycls, const double* p){
	
	TGraph* grSigma = new TGraph();
	for(double z=p[0]-100; z<p[0]+100; z+=2){
		double pRes[6] = {z, p[1], p[2], p[2], p[4], p[5]};
		double sigma = getRes(det, tracks, Xcls, Ycls, pRes);
		grSigma->SetPoint(grSigma->GetN(), z, sigma);
	}

	TF1* fitFunc = new TF1("fitFunc", "pol2", p[0]-100, p[0]+100);
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

	double pRes[6] = {zout, p[1], p[2], p[2], p[4], p[5]};
	double sigma = getRes(det, tracks, Xcls, Ycls, pRes, "minZ_"+graphName);

	return zout;
}
	
double yAlign(std::string graphName, StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Xcls, std::vector<cluster> Ycls, const double* p){
	
	TGraph* grSigma = new TGraph();
	for(double yRot=p[4]-0.15; yRot<p[4]+0.15; yRot+=0.005){
		double pRes[6] = {p[0], p[1], p[2], p[2], yRot, p[5]};
		double sigma = getRes(det, tracks, Xcls, Ycls, pRes);
		grSigma->SetPoint(grSigma->GetN(), yRot, sigma);
	}

	TF1* fitFunc = new TF1("fitFunc", "pol2", p[4]-0.15, p[4]+0.15);
	fitFunc->SetParameters(0.1, 0.1, 0.1);
	grSigma->Fit(fitFunc, "R");
	double yRotOut = -fitFunc->GetParameter(1)/(2*fitFunc->GetParameter(2));	
	
	TCanvas* c1 = new TCanvas("c1", "c1", 1600, 1200);
	grSigma->SetMarkerStyle(20);
	grSigma->SetMarkerSize(0.8);
	grSigma->SetMarkerColor(kBlue);
	grSigma->SetTitle("yRot alignment");
	grSigma->GetXaxis()->SetTitle("yRot position [rad]");
	grSigma->GetYaxis()->SetTitle("sigma [mm]");
	grSigma->GetYaxis()->SetRangeUser(0.09, 0.3);
	grSigma->Draw("AP");

	TLatex* latex = new TLatex();
	latex->SetTextSize(0.03);
	latex->SetTextColor(kRed);
	latex->DrawLatexNDC(0.2, 0.2, Form("yRot = %.2f", yRotOut));
	gStyle->SetOptFit(1111);
	
	c1->SaveAs(graphName.c_str());

	double pRes[6] = {p[0], p[1], p[2], p[2], yRotOut, p[5]};
	double sigma = getRes(det, tracks, Xcls, Ycls, pRes, "minyRot_"+graphName);

	return yRotOut;
}


double xAlign(std::string graphName, StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Xcls, std::vector<cluster> Ycls, const double* p){
	TGraph* grSigma = new TGraph();
	
	for(double xRot=p[5]-0.15; xRot<p[5]+0.15; xRot+=0.005){
		double pRes[6] = {p[0], p[1], p[2], p[2], p[4], xRot};
		double sigma = getRes(det, tracks, Xcls, Ycls, pRes);
		grSigma->SetPoint(grSigma->GetN(), xRot, sigma);
	}

	TF1* fitFunc = new TF1("fitFunc", "pol2", p[5]-0.15, p[5]+0.15);
	fitFunc->SetParameters(0.1, 0.1, 0.1);
	grSigma->Fit(fitFunc, "R");
	double xRotOut = -fitFunc->GetParameter(1)/(2*fitFunc->GetParameter(2));	
	
	TCanvas* c1 = new TCanvas("c1", "c1", 1600, 1200);
	grSigma->SetMarkerStyle(20);
	grSigma->SetMarkerSize(0.8);
	grSigma->SetMarkerColor(kBlue);
	grSigma->SetTitle("xRot alignment");
	grSigma->GetXaxis()->SetTitle("xRot position [rad]");
	grSigma->GetYaxis()->SetTitle("sigma [mm]");
	grSigma->GetYaxis()->SetRangeUser(0.09, 0.3);
	grSigma->Draw("AP");

	TLatex* latex = new TLatex();
	latex->SetTextSize(0.03);
	latex->SetTextColor(kRed);
	latex->DrawLatexNDC(0.2, 0.2, Form("xRot = %.2f", xRotOut));
	gStyle->SetOptFit(1111);
	
	c1->SaveAs(graphName.c_str());

	double pRes[6] = {p[0], p[1], p[2], p[2], p[4], xRotOut};
	double sigma = getRes(det, tracks, Xcls, Ycls, pRes, "minxRot_"+graphName);

	return xRotOut;
}



struct funcChi2 {

	std::vector<banco::track> tracks;
	std::vector<cluster> Xcls;
	std::vector<cluster> Ycls;
	StripTable det;

	funcChi2(StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Xcls, std::vector<cluster> Ycls) : 
	det(det), tracks(tracks), Xcls(Xcls) ,Ycls(Ycls) {}
	
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
		// double errx2 = pow(tr.ex0,2) + pow(tr.mx*tr.emx,2) + pow(det.pitchY(int(clY.stripCentroid))/sqrt(12),2);
		// double erry2 = pow(tr.ey0,2) + pow(tr.my*tr.emy,2) + pow(det.pitchX(int(clX.stripCentroid))/sqrt(12),2);

		// double err2 = pow(xtr - pr.X(),2)/(res*res) * errx2 + pow(ytr - pr.Y(),2)/(res*res) * erry2;
		// std::cout<<res*res<< " " <<err2<<" "<<(res*res)/err2<<std::endl;
		// return (res*res)/err2;
		return (res*res)/0.001;
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


const double* align(std::string pos, StripTable det, std::vector<banco::track> tracks, std::vector<cluster> Xcls, std::vector<cluster> Ycls, double *pStart, bool fixVars=false){
	
	first = true;
	const char * minName = "Minuit2";
	const char * algoName = "Migrad";

	// create minimizer giving a name and a name (optionally) for the specific
	// algorithm
	// possible choices are:
	//     minName                  algoName
	// Minuit /Minuit2             Migrad, Simplex,Combined,Scan  (default is Migrad)
	//  Minuit2                     Fumili2
	//  Fumili
	//  GSLMultiMin                ConjugateFR, ConjugatePR, BFGS,
	//                              BFGS2, SteepestDescent
	//  GSLMultiFit
	//   GSLSimAn
	//   Genetic
   	ROOT::Math::Minimizer* minimum = ROOT::Math::Factory::CreateMinimizer(minName, algoName);

	if (!minimum) {
		std::cerr << "Error: cannot create minimizer \"" << minName
				<< "\". Maybe the required library was not built?" << std::endl;
		// return "";
	}

	// set tolerance , etc...
	minimum->SetMaxFunctionCalls(1000000); // for Minuit/Minuit2
	minimum->SetMaxIterations(10000);  // for GSL
	minimum->SetTolerance(0.001);
	// minimum->SetTolerance(100);
	minimum->SetPrintLevel(2);
 

	// make the functor objet
	funcChi2 schi2(det, tracks, Xcls, Ycls);
	ROOT::Math::Functor fcn(schi2, 6);
	minimum->SetFunction(fcn);

	// Set the free variables to be minimized !
	double step[6] = {0.05, 0.05, 0.05, 0.1*M_PI/180., 0.1*M_PI/180., 0.1*M_PI/180.};
	minimum->SetVariable(0,"zpos", pStart[0], step[0]);
   	minimum->SetVariable(1,"Tx", pStart[1], step[1]);
   	minimum->SetVariable(2,"Ty", pStart[2], step[2]);
	minimum->SetVariable(3,"rotZ", pStart[3], step[3]);
	minimum->SetVariable(4,"rotY", pStart[4], step[4]);
	minimum->SetVariable(5,"rotX", pStart[5], step[5]);
	minimum->FixVariable(0);
	minimum->FixVariable(4);
	minimum->FixVariable(5);
	if(fixVars){
		minimum->FixVariable(0);
		minimum->FixVariable(3);
		minimum->FixVariable(4);
		minimum->FixVariable(5);
	}

	double pLow[6] = {pStart[0]-5., pStart[1]-100., pStart[2]-100., pStart[3]-5.*M_PI/180., pStart[4]-5.*M_PI/180., pStart[5]-5.*M_PI/180.};
	double pUp[6]  = {pStart[0]+5., pStart[1]+100., pStart[2]+100., pStart[3]+5.*M_PI/180., pStart[4]+5.*M_PI/180., pStart[5]+5.*M_PI/180.};
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

	return param;
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
	double zpos = 0., rotZ = 0., rotY = -0.15, rotX = 0.088;

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

	if(nev < 10000) {
		std::cerr << "Error: Not enough events: " << nev << std::endl;
		return 1;
	}
	std::cout<<"Number of events: "<<nev<<std::endl;

	std::cout<<"Initial parameters: "<<zpos<<" "<<initTx/nev<<" "<<initTy/nev<<" "<<rotX<<std::endl;

	double pStart[6] = {zpos, initTx/nev, initTy/nev, rotZ, rotY, rotX};
	const double* pEnd = align(run, det, tracksFit, XclsFit, YclsFit, pStart, true);
	double zout = zAlign(Form("zAlign_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pEnd);

	double pStart2[6] = {zout, pEnd[1], pEnd[2], pEnd[3], pEnd[4], pEnd[5]};
	double rotYout = yAlign(Form("yAlign_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pStart2);
	double rotXout = xAlign(Form("xAlign_%s_%s.png", detName.c_str(), run.c_str()), det, tracksFit, XclsFit, YclsFit, pStart2);
	// const double* pEnd2 = align(run, det, tracksFit, XclsFit, YclsFit, pStart2, false, false);


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