#include "DetectorTable.h"

// DetectorTable::DetectorTable(std::string idetFile, int dreamConnect0, int dreamConnect1, int dreamConnect2, int dreamConnect3){
// 	detFile = idetFile;
// 	dreamConnect.push_back(dreamConnect0);
// 	dreamConnect.push_back(dreamConnect1);
// 	dreamConnect.push_back(dreamConnect2);
// 	dreamConnect.push_back(dreamConnect3);
// 	this->buildTable();
// }

// void DetectorTable::setInversion(bool iC0, bool iC1, bool iC2, bool iC3){
// 	inv[0] = iC0;
// 	inv[1] = iC1;
// 	inv[2] = iC2;
// 	inv[3] = iC3;
// }

// bool DetectorTable::isConnected(int channel){
// 	int dream = channel/64;
// 	return ( std::find(dreamConnect.begin(), dreamConnect.end(), dream) != dreamConnect.end() );
// }

// int DetectorTable::toGB(int channel){
// 	if(!this->isConnected(channel)) return -1;
// 	int dreamNb = channel/64;
// 	int ch = channel%64;
// 	int cntNb = std::find(dreamConnect.begin(), dreamConnect.end(), dreamNb) - dreamConnect.begin();
// 	if(inv[cntNb]){
// 		return (cntNb+1)*64 - 1 - ch;
// 	}
// 	else{
// 		return cntNb*64 + ch;
// 	}
// }

DetectorTable::DetectorTable(std::string idetFile){
	detFile = idetFile;
}

void DetectorTable::buildTable(){

	std::cout<< "Building detector table "<<detFile<<std::endl;
	std::ifstream infile(detFile.c_str());
	if(infile.fail()){
		throw std::runtime_error("ERROR: map file not found");
	}

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

		int GBchannel = cnt*64 + ch;

		std::string n;
		std::istringstream sinter(finter);
		// std::cout<<"finter: "<<finter<<std::endl;
		while(std::getline(sinter, n, ':')) inter.push_back(std::stod(n));

		std::istringstream sngh(fngh);
		while(std::getline(sngh, n, ':')) ngh.push_back(cnt*64 + std::stoi(n));

		mapConnector[GBchannel] = cnt;
		mapStripNb[GBchannel] = stripNb;
		mapAxis[GBchannel] = axis;
		mapPitch[GBchannel] = pitch;
		mapInter[GBchannel] = inter;
		mapNgh[GBchannel] = ngh;
	}
}

float DetectorTable::getInter(int ch, int chPerp){
	int GBch = this->toGB(ch);
	int GBchPerp = this->toGB(chPerp);
	if(GBch<0 or GBchPerp<0) return -1;

	if(mapInter[GBch].size()>1 && this->getConnector(GBch) < 2){
		if(GBchPerp==-1 or this->getConnector(GBchPerp) < 2) throw std::runtime_error("ERROR: With detector inter_map.txt you need to specify the channel of the cluster in x in order to find the interstrip value of y strips (the interstrip changes along one vertical (y) strip)");
		else{
			if(this->getConnector(GBchPerp) == 3) return mapInter[GBch][1];  // the cluster is in the connector 3 horizontal region
			if(this->getConnector(GBchPerp) == 2) return mapInter[GBch][0];  // the cluster is in the connector 2 horizontal region
			else throw std::runtime_error("ERROR: In DetectorTable::getInter(int, int) the cluster x channel position doesn't match a dream connected to connectors 2 or 3 ");
		}
	}
	return (mapInter[GBch])[0];
}

int DetectorTable::getConnector(int channel){
	int GBch = this->toGB(channel);
	if(GBch<0) return -1;
	return mapConnector[GBch];
}

char DetectorTable::getAxis(int channel){
	int GBch = this->toGB(channel);
	if(GBch<0) return 'o';
	return mapAxis[GBch];
}

float DetectorTable::getPitch(int channel){
	int GBch = this->toGB(channel);
	if(GBch<0) return 0;
	return mapPitch[GBch];
}

int DetectorTable::getStripNb(int channel){
	int GBch = this->toGB(channel);
	if(GBch<0) return -1;
	return mapStripNb[GBch];
}

bool DetectorTable::isNeighbour(int ch1, int ch2){
	int GBch1 = this->toGB(ch1);
	int GBch2 = this->toGB(ch2);
	if(GBch1<0 or GBch2<0) return false;
	std::vector<int> ngh = mapNgh[GBch1];
	return std::find(ngh.begin(), ngh.end(), GBch2) != ngh.end();
}

bool DetectorTable::isEdge(int channel){
	int GBch = this->toGB(channel);
	if(GBch<0) return true;
	return mapNgh[GBch].size() < 2;
}

std::vector<int> DetectorTable::getNeighbours(int channel){
	int GBch = this->toGB(channel);
	if(GBch<0) return std::vector<int>();
	return mapNgh[GBch];
}

std::string DetectorTable::printAll(int channel){
	int GBch = this->toGB(channel);
	std::string out = "ch: " + std::to_string(channel) + " cnt: " + std::to_string(this->getConnector(channel)) + " cntChannel: " + 
		std::to_string(GBch) + " axis: " + this->getAxis(channel) +
		" pitch: " + std::to_string(this->getPitch(channel)) + " stripNb: " + std::to_string(this->getStripNb(channel));
	return out;
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