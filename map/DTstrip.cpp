#include "DTstrip.h"

DTstrip::DTstrip(std::string idetFile) : DetectorTable(idetFile) {

}


int DTstrip::toGB(int channel){
	for (auto it = mapStripNb.begin(); it != mapStripNb.end(); ++it){
		if (it->second == channel) return it->first;
	}
	return -1;
}