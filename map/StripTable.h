#ifndef STRIPTABLE_H
#define STRIPTABLE_H

#include "DetectorTable.h"

// can read DetectorTable from the strip number and axis

class StripTable : protected DetectorTable
{
public:
	StripTable() = default;
	~StripTable() = default;
	StripTable(std::string idetFile);

	int toGB(int sn, char axis);  		// convert strip to gerber channnel

	float pitch(int sn, char axis);
	float pitchX(int sn){return this->pitch(sn, 'x');};
	float pitchY(int sn){return this->pitch(sn, 'y');};

	float interX(int sn);
	float interY(int sn, int snPerp);

};
#endif