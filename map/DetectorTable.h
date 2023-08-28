#ifndef DETECTORTABLE_H
#define DETECTORTABLE_H

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// the maps are indexed with the gerber (GB) channel number which are defined as: connectorNb*64 + connectorCh
// here channel refers to real channels if using DTchannel or strips if using DTstrip

class DetectorTable
{
public:
	DetectorTable() = default;
	~DetectorTable() = default;
	DetectorTable(std::string idetFile);
	// DetectorTable(std::string idetFile, int dreamConnect0, int dreamConnect1, int dreamConnect2, int dreamConnect3);

	// void setInversion(bool iC0, bool iC1, bool iC2, bool iC3);

	virtual int toGB(int channel) = 0;  		// convert real channel or strip to gerber channel, <0 if invalid

	bool isNeighbour(int ch1, int ch2);
	bool isEdge(int channel);
	void buildTable();
	
	int getConnector(int channel);
	char getAxis(int channel);
	float getPitch(int channel);
	float getInter(int channel, int channelPerp = -1);   //{return mapInter[channel];};
	int getStripNb(int channel);
	std::vector<int> getNeighbours(int channel);

	// for debug purposes
	std::string printAll(int channel);

protected:
	std::string detFile;
	// std::vector<int> dreamConnect;
	// std::vector<bool> inv = {false, false, false, false}; 

	std::map<int, int> mapConnector;
	std::map<int, char> mapAxis;
	std::map<int, float> mapPitch;
	std::map<int, std::vector<float>> mapInter;
	std::map<int, int> mapStripNb;
	std::map<int, std::vector<int>> mapNgh;
};

#endif