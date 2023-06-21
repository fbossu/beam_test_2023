#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

class DetectorTable
{
public:
	DetectorTable() = default;
	~DetectorTable() = default;
	DetectorTable(std::string idetFile, int dreamConnect0, int dreamConnect1, int dreamConnect2, int dreamConnect3);
	
	void buildTable();
	int getConnector(int channel){return mapConnector[channel];}
	char getAxis(int channel){return mapAxis[channel];};
	float getPitch(int channel){return mapPitch[channel];};
	float getInter(int channel, int channelPerp = -1);   //{return mapInter[channel];};
	int getStripNb(int channel){return mapStripNb[channel];};
	std::vector<int> getNeighbours(int channel){return mapNgh[channel];};

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