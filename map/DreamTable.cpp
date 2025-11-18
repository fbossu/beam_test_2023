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

void DreamTable::setFlip(bool iC0, bool iC1, bool iC2, bool iC3){
  // a flip is when a connector is returned, but the cable stayed with the same orientation
	flip[0] = iC0;
	flip[1] = iC1;
	flip[2] = iC2;
	flip[3] = iC3;
}

bool DreamTable::isConnected(int ch){
	int dream = ch/64;
	return ( std::find(dreamConnect.begin(), dreamConnect.end(), dream) != dreamConnect.end() && std::find(offChannel.begin(), offChannel.end(), ch) == offChannel.end());
}

int DreamTable::toGB(int ch){
	if(!this->isConnected(ch)) return -1;
	int dreamNb = ch/64;
	int dreamch = ch%64;
	int cntNb = std::find(dreamConnect.begin(), dreamConnect.end(), dreamNb) - dreamConnect.begin();
	if(flip[cntNb]){ // exchange pair and upair channels
    dreamch = (dreamch%2==0) ? dreamch+1 : dreamch-1;
  }
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


int DreamTable::stripNb(int ch){
	int GBch = this->toGB(ch);
	if(GBch<0) return -1;
	return mapStripNb[GBch];
}

char DreamTable::axis(int ch){
	int GBch = this->toGB(ch);
	if(GBch<0) return 'o';
	return mapAxis[GBch];
}

int DreamTable::getConnectors(){
  int c=0;
  for( int i=0;i<dreamConnect.size();i++){
    int p=1;
    for( int j=0; j < i; j++ ) p *= 10;
    c += dreamConnect.at( dreamConnect.size() - 1 - i) * p;
  }
  return c;
}

