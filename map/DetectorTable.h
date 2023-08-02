#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// the maps are indexed with the gerber (GB) channel number which are defined as: connectorNb*64 + connectorCh 

class DetectorTable
{
public:
	DetectorTable() = default;
	~DetectorTable() = default;
	DetectorTable(std::string idetFile, int dreamConnect0, int dreamConnect1, int dreamConnect2, int dreamConnect3);

	void setInversion(bool iC0, bool iC1, bool iC2, bool iC3);
	int toGB(int channel);  // convert real channel to gerber channnel

	bool isConnected(int channel);
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
	std::string getAll(int channel);

private:
	std::string detFile;
	std::vector<int> dreamConnect;
	std::vector<bool> inv = {false, false, false, false}; 

	std::map<int, int> mapConnector;
	std::map<int, char> mapAxis;
	std::map<int, float> mapPitch;
	std::map<int, std::vector<float>> mapInter;
	std::map<int, int> mapStripNb;
	std::map<int, std::vector<int>> mapNgh;
};