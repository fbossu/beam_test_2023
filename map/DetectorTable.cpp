#include "DetectorTable.h"

DetectorTable::DetectorTable(std::string idetFile, int dreamConnect0, int dreamConnect1, int dreamConnect2, int dreamConnect3){
	detFile = idetFile;
	if(dreamConnect0 > 0) dreamConnect.push_back(dreamConnect0);
	if(dreamConnect1 > 0) dreamConnect.push_back(dreamConnect1);
	if(dreamConnect2 > 0) dreamConnect.push_back(dreamConnect2);
	if(dreamConnect3 > 0) dreamConnect.push_back(dreamConnect3);
	this->buildTable();
}


void DetectorTable::buildTable(){

	std::ifstream infile(detFile.c_str());

	std::string line, fcnt, fch, fstripNb, faxis, fpitch, finter, fngh;
	// float inter = 0;
	float pitch = 0;
	char axis = 'n';
	std::vector<int> ngh;
	std::vector<float> inter;
	int cnt = 0, ch=0, stripNb = 0;

	std::getline(infile, line);

	while(std::getline(infile, line)){
		ngh.clear(); inter.clear();
		std::istringstream fline(line);
		std::getline(fline, fcnt, ',');
		std::getline(fline, fch, ',');
		std::getline(fline, fstripNb, ',');
		std::getline(fline, faxis, ',');
		std::getline(fline, fpitch, ',');
		std::getline(fline, finter, ',');
		std::getline(fline, fngh, ',');

		cnt = std::stoi(fcnt);
		ch = std::stoi(fch);
		stripNb = std::stoi(fstripNb);
		axis = faxis[0];
		pitch = std::stod(fpitch);
		// inter = std::stod(finter);

		std::string n;
		std::istringstream sinter(finter);
		// std::cout<<"finter: "<<finter<<std::endl;
		while(std::getline(sinter, n, ':')) inter.push_back(std::stod(n));

		std::istringstream sngh(fngh);
		while(std::getline(sngh, n, ':')) ngh.push_back(dreamConnect[cnt]*64 + std::stoi(n));

		int dreamChannel = dreamConnect[cnt]*64 + ch;
		mapConnector[dreamChannel] = cnt;
		mapStripNb[dreamChannel] = stripNb;
		mapAxis[dreamChannel] = axis;
		mapPitch[dreamChannel] = pitch;
		mapInter[dreamChannel] = inter;
		mapNgh[dreamChannel] = ngh;
	}
}

float DetectorTable::getInter(int channel, int channelPerp){
	if(detFile == "inter_map.txt" && this->getConnector(channel) < 2){
		if(channelPerp==-1 or this->getConnector(channelPerp) < 2) throw std::runtime_error("ERROR: With detector inter_map.txt you need to specify the channel of the cluster in x in order to find the interstrip value of y strips (the interstrip changes along one vertical (y) strip)");
		else{
			if(this->getConnector(channelPerp) == 3) return mapInter[channel][0];  // the cluster is in the connector 3 horizontal region
			if(this->getConnector(channelPerp) == 2) return mapInter[channel][1];  // the cluster is in the connector 2 horizontal region
			else throw std::runtime_error("ERROR: In DetectorTable::getInter(int, int) the cluster x channel position doesn't match a dream connected to connectors 2 or 3 ");
		}
	}
	return (mapInter[channel])[0];
}


// int main(int argc, char const *argv[])
// {
// 	DetectorTable det = DetectorTable("inter_map.txt", 4, 5, 6, 7);

// 	for(int i=4*64; i<8*64; i++){
// 		// std::cout<<"DreamCh: "<<i<<" Connector: "<<det.getConnector(i)<<" Axis: "<<det.getAxis(i)<<" StripNb "<<det.getStripNb(i)<<" Pitch: "<<det.getPitch(i)<< " DreamCh_Neighbours: ";
// 		// for(int j=0; j<det.getNeighbours(i).size(); j++) std::cout << det.getNeighbours(i)[j] <<" ";
// 		// std::cout<<std::endl;

// 		std::cout<<"DreamCh: "<<i<<" Connector: "<<det.getConnector(i)<<" Axis: "<<det.getAxis(i)<<" StripNb "<<det.getStripNb(i)<<" Pitch: "<<det.getPitch(i);
// 		std::cout<< " Inter: "<<det.getInter(i,435)<<std::endl;
// 	}
// 	return 0;
// }