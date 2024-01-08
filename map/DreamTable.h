#ifndef DREAMTABLE_H
#define DREAMTABLE_H

#include "DetectorTable.h"

// channel refers to real dream channel

class DreamTable : protected DetectorTable
{
public:
	DreamTable() = default;
	~DreamTable() = default;
	DreamTable(std::string idetFile, int dreamConnect0, int dreamConnect1, int dreamConnect2, int dreamConnect3);

	void setInversion(bool iC0, bool iC1, bool iC2, bool iC3);
	void killChannel(int ch){offChannel.push_back(ch);};
	int toGB(int ch);  										// convert real channel to gerber channel
	bool isConnected(int ch);
	bool isNeighbour(int ch1, int ch2);
	bool isEdge(int ch);
	char axis(int ch);
	int stripNb(int ch);

private:
	std::vector<int> dreamConnect;
	std::vector<<int> offChannel;
	std::vector<bool> inv = {false, false, false, false}; 
};

#endif