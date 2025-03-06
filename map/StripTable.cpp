#include "StripTable.h"

StripTable::StripTable(std::string idetFile) : DetectorTable(idetFile) {
	this->buildTable();
	if(idetFile.find("asa") != std::string::npos){
		zonePitch = { {2.f, 2.f}, {1.5f, 1.5f}, {1.f, 1.f}, {0.8f, 0.8f} };
		zoneInter = { {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f} };
	}
	else if(idetFile.find("inter") != std::string::npos){
		zonePitch = { {1.f, 1.f}, {1.f, 1.f}, {1.f, 1.f}, {1.f, 1.f} };
		zoneInter = { {0.1f, 0.75f}, {0.1f, 0.5f}, {0.1f, 0.9f}, {0.1f, 0.67f} };
	}
	else if(idetFile.find("strip") != std::string::npos){
		zonePitch = { {0.5f, 1.f}, {0.5f, 1.5f}, {0.5f, 0.5f}, {1.5f,1.f}, {1.5f, 1.5f}, {1.5f, 0.5f}, {1.f, 1.f}, {1.f, 1.5f}, {1.f, 0.5f} };
		zoneInter = { {0.1f, 0.75f}, {0.1f, 1.12f}, {0.1f, 0.37f}, {0.1f,0.75f}, {0.1f, 1.12f}, {0.1f, 0.37f}, {0.1f, 0.75f}, {0.1f, 1.12f}, {0.1f, 0.37f} };
	}
	else{
		throw std::runtime_error("Error: zones not recognized");
	}
}

StripTable::StripTable(std::string idetFile, std::string alignFile) : StripTable(idetFile) {
	bool isOk = this->SetAlignFile(alignFile);
	if(!isOk) throw std::runtime_error("Error: alignment file not found");
}


bool StripTable::SetAlignFile(std::string alignFile) {
	
	std::cout<<"Reading alignment file "<<alignFile<<std::endl;
	std::ifstream file(alignFile);
	
	if (!file.is_open()) {
		std::cout<<"Error: alignment file not found"<<std::endl;
		return false;
	}

	std::string line;
	double zpos,Tx, Ty, rotX, rotY, rotZ;

	while (std::getline(file, line)) {
		if (line[0] == '#') {
			continue;
		}
		std::istringstream iss(line);
		if (!(iss >> run >> zpos >> Tx >> Ty >> rotZ >> rotY >> rotX)) {
			break;
		}
		std::getline(file, line);
		if (line[0] == '#') {
			std::getline(file, line);
		}
		// std::istringstream iss2(line);
		// if (!(iss2 >> run >> ezpos >> eTx >> eTy >> erot)) {
		// 	break;
		// }
	}
	// std::cout<<zpos<<" "<<Tx<<" "<<Ty<<" "<<rot<<std::endl;
	this->setTransform(zpos, Tx, Ty, rotZ, rotY, rotX);
	return true;
}

void StripTable::setTransform(double zpos, double Tx, double Ty, double rotZ, double rotY, double rotX){
	ROOT::Math::Rotation3D rot(ROOT::Math::RotationZYX(rotZ, rotY, rotX)); // rotation around z, y, x
	ROOT::Math::Translation3D trl(Tx, Ty, zpos);
	trans = ROOT::Math::Transform3D(rot, trl);
	printf("Alignment transformation: zpos = %f, Tx = %f, Ty = %f, rotZ = %f, rotY = %f, rotX = %f\n", zpos, Tx, Ty, rotZ, rotY, rotX);
}

int StripTable::toGB(int sn, char axis){
	for (auto it = mapStripNb.begin(); it != mapStripNb.end(); ++it){
		if(it->second == sn and mapAxis[it->first] == axis) return it->first;
	}
	return -1;
}

float StripTable::pitch(int sn, char axis){
	int GBch = this->toGB(sn, axis);
	if(GBch<0) return -1;
	return mapPitch[GBch];
}

float StripTable::interX(int sn){
	int GBch = this->toGB(sn, 'x');
	if(GBch<0) return -1;
	return this->getInter(GBch);
}

std::vector<double> StripTable::pos(double sn, char axis){
	int snmin = int(sn);
	int snmax = snmin + 1;
	int GBchmin = this->toGB(snmin, axis);	
	int GBchmax = this->toGB(snmax, axis);
	if(GBchmin<0 or GBchmax<0) return {-9999., -9999.};
	// std::cout<<axis<<" PosX "<<this->getPosx(GBchmin)<<" "<<this->getPosx(GBchmax)<<std::endl;
	// std::cout<<axis<<" PosY "<<this->getPosy(GBchmin)<<" "<<this->getPosy(GBchmax)<<std::endl;
	// std::vector<double> v = { this->getPosx(GBchmin) + (sn - snmin)*abs(this->getPosx(GBchmax) - this->getPosx(GBchmin)),
							  // this->getPosy(GBchmin) + (sn - snmin)*abs(this->getPosy(GBchmax) - this->getPosy(GBchmin)) };
	std::vector<double> v = { this->getPosx(GBchmin) + (sn - snmin)*(this->getPosx(GBchmax) - this->getPosx(GBchmin)),
							  this->getPosy(GBchmin) + (sn - snmin)*(this->getPosy(GBchmax) - this->getPosy(GBchmin)) };
	// std::cout<<axis<<" vxy "<<v[0]<<" "<<v[1]<<std::endl;

	// a translation form the 0,0 origin of the gerber axis to the center of the active region of the detector
	// v[0] = v[0] + 50.;
	// v[1] = v[1] - 50.;

	// std::vector<double> vAlign = { (v[0] + Tx) * cos(rot) - (v[1] + Ty)* sin(rot),
								//    (v[0] + Tx) * sin(rot) + (v[1] + Ty)* cos(rot)  };
	
	// ROOT::Math::XYZPoint pdet(v[0], v[1], 0.);
		// std::cout<<"det "<<pdet<<std::endl;
	// ROOT::Math::XYZPoint pr = trans(pdet);
	// std::cout<<"r "<<pr<<std::endl;

	return v;
	// return {pr.x(), pr.y()};
}

std::vector<double> StripTable::pos3D(double snx, double sny){
	double xpos, ypos;
	if(sny<0) xpos = -50.;
	else xpos = this->pos(sny, 'y')[0];
	if(snx<0) ypos = 50.;
	else ypos = this->pos(snx, 'x')[1];

	ROOT::Math::XYZPoint pdet(xpos, ypos, 0.);
	ROOT::Math::XYZPoint pr = trans(pdet);
	return {pr.x(), pr.y(), pr.z()};
}

float StripTable::interY(int sn, int snPerp){
	int GBch = this->toGB(sn, 'y');
	if(GBch<0) return -1;
	return this->getInter(GBch, this->toGB(snPerp, 'x'));
}

int  StripTable::zone(int snx, int sny){
	float pitchx = this->pitchX(snx);
	float pitchy = this->pitchY(sny);

	float interx = this->interX(snx);
	float intery = this->interY(sny, snx);
	
	for(int i=0; i<zonePitch.size(); i++){
		if( zonePitch[i][0] == pitchx and zonePitch[i][1] == pitchy and zoneInter[i][0] == interx and zoneInter[i][1] == intery){
			return i;
		}
	}
	return -1;
}

std::string StripTable::zoneLabel(int z){
	std::string label = "pitch "+std::to_string(zonePitch[z][1]).substr(0,3)+"/"+std::to_string(zonePitch[z][0]).substr(0,3);
	label += " inter "+std::to_string(zoneInter[z][1]).substr(0,4)+"/"+std::to_string(zoneInter[z][0]).substr(0,4);
	label += " (y/x mm)";
	return label;
}

float StripTable::pitchXzone(int zone) {
    return zonePitch[zone][0];
}

float StripTable::pitchYzone(int zone) {
    return zonePitch[zone][1];
}

float StripTable::interXzone(int zone) {
    return zoneInter[zone][0];
}

float StripTable::interYzone(int zone) {
    return zoneInter[zone][1];
}

double StripTable::getZpos(){
	ROOT::Math::XYZPoint pdet(-50., 50., 0);
	ROOT::Math::XYZPoint pr = trans(pdet);
	return pr.z();
}