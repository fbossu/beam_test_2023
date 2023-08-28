#include "DreamTable.h"

DreamTable::DreamTable(std::string idetFile, int dreamConnect0, int dreamConnect1, int dreamConnect2, int dreamConnect3) : DetectorTable(idetFile) {
	dreamConnect.push_back(dreamConnect0);
	dreamConnect.push_back(dreamConnect1);
	dreamConnect.push_back(dreamConnect2);
	dreamConnect.push_back(dreamConnect3);
	this->buildTable();
}

void DreamTable::setInversion(bool iC0, bool iC1, bool iC2, bool iC3){
	inv[0] = iC0;
	inv[1] = iC1;
	inv[2] = iC2;
	inv[3] = iC3;
}

bool DreamTable::isConnected(int ch){
	int dream = ch/64;
	return ( std::find(dreamConnect.begin(), dreamConnect.end(), dream) != dreamConnect.end() );
}

int DreamTable::toGB(int ch){
	if(!this->isConnected(ch)) return -1;
	int dreamNb = ch/64;
	int dreamch = ch%64;
	int cntNb = std::find(dreamConnect.begin(), dreamConnect.end(), dreamNb) - dreamConnect.begin();
	if(inv[cntNb]){
		return (cntNb+1)*64 - 1 - dreamch;
	}
	else{
		return cntNb*64 + dreamch;
	}
}

bool DreamTable::isNeighbour(int ch1, int ch2){
	int GBch1 = this->toGB(ch1);
	int GBch2 = this->toGB(ch2);
	if(GBch1<0 or GBch2<0) return false;
	std::vector<int> ngh = mapNgh[GBch1];
	return std::find(ngh.begin(), ngh.end(), GBch2) != ngh.end();
}

bool DreamTable::isEdge(int ch){
	int GBch = this->toGB(ch);
	if(GBch<0) return false;
	return mapNgh[GBch].size() < 2;
}


char DreamTable::axis(int ch){
	int GBch = this->toGB(ch);
	if(GBch<0) return 'o';
	return mapAxis[GBch];
}