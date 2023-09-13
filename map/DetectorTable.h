#ifndef DETECTORTABLE_H
#define DETECTORTABLE_H

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// the tables are indexed by gerber channel (GBch) that are defined as: connectorNb*64 + connectorCh

class DetectorTable
{
public:
	DetectorTable() = default;
	~DetectorTable() = default;
	DetectorTable(std::string idetFile);

	void buildTable();
	
	int getConnector(int GBch){return mapConnector[GBch];};
	char getAxis(int GBch){return mapAxis[GBch];};
	float getPitch(int GBch){return mapPitch[GBch];};
	float getInter(int GBch, int GBchPerp = -1);
	std::vector<int> getNeighbours(int GBch){return mapNgh[GBch];};
	double getPosX(int GBch){return mapPosX[GBch];};
	double getPosY(int GBch){return mapPosY[GBch];};

	// for debug purposes
	std::string printAll(int GBch);

protected:
	std::string detFile;

	std::map<int, int> mapConnector;
	std::map<int, char> mapAxis;
	std::map<int, float> mapPitch;
	std::map<int, std::vector<float>> mapInter;
	std::map<int, int> mapStripNb;
	std::map<int, std::vector<int>> mapNgh;
	std::map<int, double> mapPosX;
	std::map<int, double> mapPosY;
};

#endif