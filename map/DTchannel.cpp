#include "DTchannel.h"

DTchannel::DTchannel(std::string idetFile, int dreamConnect0, int dreamConnect1, int dreamConnect2, int dreamConnect3) : DetectorTable(idetFile) {
	dreamConnect.push_back(dreamConnect0);
	dreamConnect.push_back(dreamConnect1);
	dreamConnect.push_back(dreamConnect2);
	dreamConnect.push_back(dreamConnect3);
	this->buildTable();
}

void DTchannel::setInversion(bool iC0, bool iC1, bool iC2, bool iC3){
	inv[0] = iC0;
	inv[1] = iC1;
	inv[2] = iC2;
	inv[3] = iC3;
}

bool DTchannel::isConnected(int channel){
	int dream = channel/64;
	return ( std::find(dreamConnect.begin(), dreamConnect.end(), dream) != dreamConnect.end() );
}

int DTchannel::toGB(int channel){
	int dreamNb = channel/64;
	if(std::find(dreamConnect.begin(), dreamConnect.end(), dreamNb) == dreamConnect.end()) return -1;

	int ch = channel%64;
	int cntNb = std::find(dreamConnect.begin(), dreamConnect.end(), dreamNb) - dreamConnect.begin();
	if(inv[cntNb]){
		return (cntNb+1)*64 - 1 - ch;
	}
	else{
		return cntNb*64 + ch;
	}
}

bool DTchannel::isNeighbour(int ch1, int ch2){
	int GBch = this->toGB(ch);
	if(GBch<0) return false;
	std::vector<int> ngh = mapNgh[ch1];
	return std::find(ngh.begin(), ngh.end(), ch2) != ngh.end();
}

bool DTchannel::isEdge(int ch){
	int GBch = this->toGB(ch);
	if(GBch<0) return false;
	return mapNgh[GBch].size() < 2;
}

int DTchannel::stripNb(int ch){
	int GBch = this->toGB(ch);
	if(GBch<0) return -1;
	return mapStripNb[GBch];
}


char DTchannel::getAxis(int ch){
	int GBch = this->toGB(ch);
	if(GBch<0) return 'o';
	return mapAxis[GBch];
}