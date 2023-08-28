#ifndef DTSTRIP_H
#define DTSTRIP_H

#include "DetectorTable.h"

// here channel refers to the strip number

class DTstrip : protected DetectorTable
{
public:
	DTstrip() = default;
	~DTstrip() = default;
	DTstrip(std::string idetFile);

	int toGB(int ch, char axis);  					// convert strip to gerber channnel
	float pitchX(int ch);
	float pitchX(int ch);
	std::vector<float> getInter(int ch){return mapInter[ch];};

};
#endif