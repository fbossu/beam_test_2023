#ifndef STRIPTABLE_H
#define STRIPTABLE_H

#include "DetectorTable.h"
#include "TLegend.h"
#include <Math/Transform3D.h>
#include <Math/Translation3D.h>
#include <Math/Rotation3D.h>
#include <Math/Vector3D.h>
#include <Math/RotationZYX.h>

#include <Math/Point3D.h>

// can read DetectorTable from the strip number and axis
// zones are defined as areas in the detector with different pitch/inter combinations. Numbered From left to right, top to botom.

class StripTable : protected DetectorTable
{
public:
	StripTable() = default;
	~StripTable() = default;
	StripTable(std::string idetFile);
	StripTable(std::string idetFile, std::string alignFile);

	void setTransform(double zpos, double Tx, double Ty, double rotZ, double rotY, double rotX);
	int toGB(int sn, char axis);  		// convert strip to gerber channnel

	float pitch(int sn, char axis);
	float pitchX(int sn){return this->pitch(sn, 'x');};
	float pitchY(int sn){return this->pitch(sn, 'y');};

	float interX(int sn);
	float interY(int sn, int snPerp);

	std::vector<double> pos(double sn, char axis);
	std::vector<double> posX(double sn){return this->pos(sn, 'x');};
	std::vector<double> posY(double sn){return this->pos(sn, 'y');};
	std::vector<double> pos3D(double snx, double sny);

	int zone(int snx, int sny);
	int nbZone(){return zonePitch.size();};
	std::string zoneLabel(int z);
	float pitchXzone(int z);
	float pitchYzone(int z);

	std::string getRun(){return run;};


private:
	std::vector<std::vector<float>> zonePitch;
	std::vector<std::vector<float>> zoneInter;
	std::string run;
	ROOT::Math::Transform3D trans;
};
#endif