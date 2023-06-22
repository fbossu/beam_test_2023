#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

class DetectorTable
{
public:
	DetectorTable() = default;
	~DetectorTable() = default;
	DetectorTable(std::string idetFile, int dreamConnect0, int dreamConnect1, int dreamConnect2, int dreamConnect3);

	bool isConnected(int channel);
	void buildTable();
	
	int getConnector(int channel);
	char getAxis(int channel);
	float getPitch(int channel);
	float getInter(int channel, int channelPerp = -1);   //{return mapInter[channel];};
	int getStripNb(int channel);
	std::vector<int> getNeighbours(int channel);

private:
	std::string detFile;
	std::vector<int> dreamConnect;

	std::map<int, int> mapConnector;
	std::map<int, char> mapAxis;
	std::map<int, float> mapPitch;
	std::map<int, std::vector<float>> mapInter;
	std::map<int, int> mapStripNb;
	std::map<int, std::vector<int>> mapNgh;
};