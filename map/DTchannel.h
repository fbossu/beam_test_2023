#ifndef DTCHANNEL_H
#define DTCHANNEL_H

#include "DetectorTable.h"

class DTchannel : public DetectorTable
{
public:
	DTchannel() = default;
	~DTchannel() = default;
	DTchannel(std::string idetFile, int dreamConnect0, int dreamConnect1, int dreamConnect2, int dreamConnect3);

	void setInversion(bool iC0, bool iC1, bool iC2, bool iC3);
	int toGB(int channel);  // convert real channel to gerber channnel
	bool isConnected(int channel);

private:
	std::vector<int> dreamConnect;
	std::vector<bool> inv = {false, false, false, false}; 
};

#endif