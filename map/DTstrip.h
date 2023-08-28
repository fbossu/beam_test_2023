#ifndef DTSTRIP_H
#define DTSTRIP_H

#include "DetectorTable.h"

class DTstrip : public DetectorTable
{
public:
	DTstrip() = default;
	~DTstrip() = default;
	DTstrip(std::string idetFile);

	int toGB(int channel);  // convert real channel to gerber channnel
};
#endif