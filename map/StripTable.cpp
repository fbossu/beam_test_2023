#include "StripTable.h"

StripTable::StripTable(std::string idetFile) : DetectorTable(idetFile) {
	this->buildTable();
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

float StripTable::interY(int sn, int snPerp){
	int GBch = this->toGB(sn, 'y');
	if(GBch<0) return -1;
	return this->getInter(GBch, this->toGB(snPerp, 'x'));
}