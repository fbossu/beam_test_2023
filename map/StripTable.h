#ifndef STRIPTABLE_H
#define STRIPTABLE_H

#include "DetectorTable.h"

// can read DetectorTable from the strip number and axis
// zones are defined as areas in the detector with different pitch/inter combinations. Numbered From left to right, top to botom.

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

	std::vector<double> pos(int sn, char axis);
	std::vector<double> posX(int sn){return this->pos(sn, 'x');};
	std::vector<double> posY(int sn){return this->pos(sn, 'y');};

	int zone(int snx, int sny);
	int nbZone(){return zonePitch.size();};
	std::string zoneLabel(int z);

private:
	std::vector<std::vector<float>> zonePitch;
	std::vector<std::vector<float>> zoneInter;
};
#endif