#include "StripTable.h"

StripTable::StripTable(std::string idetFile) : DetectorTable(idetFile) {
	this->buildTable();
	if(idetFile.find("asa") != std::string::npos){
		zonePitch = { {2.f, 2.f}, {1.5f, 1.5f}, {1.f, 1.f}, {0.8f, 0.8f} };
		zoneInter = { {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f}, {0.f, 0.f} };
	}
	if(idetFile.find("inter") != std::string::npos){
		zonePitch = { {1.f, 1.f}, {1.f, 1.f}, {1.f, 1.f}, {1.f, 1.f} };
		zoneInter = { {0.1f, 0.75f}, {0.1f, 0.5f}, {0.1f, 0.9f}, {0.1f, 0.67f} };
	}
	if(idetFile.find("strip") != std::string::npos){
		zonePitch = { {0.5f, 1.f}, {0.5f, 1.5f}, {0.5f, 0.5f}, {1.5f,1.f}, {1.5f, 1.5f}, {1.5f, 0.5f}, {1.f, 1.f}, {1.f, 1.5f}, {1.f, 0.5f} };
		zoneInter = { {0.1f, 0.75f}, {0.1f, 1.12f}, {0.1f, 0.37f}, {0.1f,0.75f}, {0.1f, 1.12f}, {0.1f, 0.37f}, {0.1f, 0.75f}, {0.1f, 1.12f}, {0.1f, 0.37f} };
	}
}

#include <stdexcept>

StripTable::StripTable(std::string idetFile, std::string alignFile) : StripTable(idetFile) {
	
	std::cout<<"Reading alignment file "<<alignFile<<std::endl;
	std::ifstream file(alignFile);
	
	if (!file.is_open()) {
		throw std::runtime_error("Error: alignment file " + alignFile + " does not exist or cannot be opened.");
	}

	std::string line;

	while (std::getline(file, line)) {
		if (line[0] == '#') {
			continue;
		}
		std::istringstream iss(line);
		if (!(iss >> run >> zpos >> Tx >> Ty >> rot)) {
			break;
		}
		std::getline(file, line);
		if (line[0] == '#') {
			std::getline(file, line);
		}
		std::istringstream iss2(line);
		if (!(iss2 >> run >> ezpos >> eTx >> eTy >> erot)) {
			break;
		}
	}
	std::cout<<zpos<<" "<<Tx<<" "<<Ty<<" "<<rot<<std::endl;
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
	// std::vector<double> v = { this->getPosx(GBchmin) + (sn - snmin)*abs(this->getPosx(GBchmax) - this->getPosx(GBchmin)),
							  // this->getPosy(GBchmin) + (sn - snmin)*abs(this->getPosy(GBchmax) - this->getPosy(GBchmin)) };
	std::vector<double> v = { this->getPosx(GBchmin) + (sn - snmin)*(this->getPosx(GBchmax) - this->getPosx(GBchmin)),
							  this->getPosy(GBchmin) + (sn - snmin)*(this->getPosy(GBchmax) - this->getPosy(GBchmin)) };


	std::vector<double> vAlign = { (v[0] + Tx) * cos(rot) - (v[1] + Ty)* sin(rot),
								   (v[0] + Tx) * sin(rot) + (v[1] + Ty)* cos(rot)  };

	return vAlign;
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

