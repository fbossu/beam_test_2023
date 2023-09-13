#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

// all strip are counted from top to botom in y direction and from left to right in x. Counted from 0-127 over all 4 active region 
// 0-31 in x and y is the pad connected to connector 0
// 32-63 in x and y is the pad connected to connector 1
// 64-95 in x and y is the pad connected to connector 2
// 96-127 in x and y is the pad connected to connector 3

int main(int argc, char const *argv[])
{
	//Output 
	std::ofstream outfile;
	outfile.open("asa_map.txt");
	outfile << "connector,conectorChannel,stripNb,axis,pitch(mm),interpitch(mm),neighbours(:separated),xGerber,yGerber" << std::endl;

	double pitch = 0, inter = 0;
	char axis = 'n';
	std::vector<int> ngh;
	int cnt = 0, stripNb = 0;
	double posX = 0, posY = 0;

	// Connector 0 (bottom left on gerber)
	for(int i=0; i<64; i++){
		ngh.clear();
		cnt = 0;
		pitch = 2.;  //mm
		inter = 0;

		if(i<32){
			axis = 'x';
			stripNb = 31 - i;
			posX = -113.15;
			posY = 50.15 + pitch*stripNb;
		}
		else{
			axis = 'y';
			stripNb = i - 32;
			posX = -112.15 + pitch*stripNb;
			posY = 51.15;
		}
	  	if(i!=0 && i!=32) ngh.push_back(i-1);
	  	if(i!=31 && i!=63) ngh.push_back(i+1);
		
		outfile<<cnt<<","<<i<<","<<stripNb<<","<<axis<<","<<pitch<<","<<inter<<",";
		
		for(int j=0; j<ngh.size(); j++){
			if(j!=ngh.size()-1) outfile<<ngh[j]<<":";
			else outfile<<ngh[j]<<",";
		}
		outfile<<posX<<","<<posY<<std::endl;
	}

	// Connector 1 (bottom right on gerber)
	for(int i=0; i<64; i++){
	  	ngh.clear();
	  	cnt = 1;
	  	pitch = 1.;  
	  	inter = 0;

	  	if(i<32){
	  		axis = 'y';
	  		stripNb = 32 + i;
	  		posX = -97.565 + pitch*i;
			posY = 2.819;
	  	}
	  	else{
	  		axis = 'x';
	  		stripNb = i;
	  		posX = -98.065;
			posY = 2.319 + pitch*(stripNb-32);
	  	}
	  	if(i!=0 && i!=32) ngh.push_back(i-1);
	  	if(i!=31 && i!=63) ngh.push_back(i+1);
	  	
	  	outfile<<cnt<<","<<i<<","<<stripNb<<","<<axis<<","<<pitch<<","<<inter<<",";
	  	
	  	for(int j=0; j<ngh.size(); j++){
	  		if(j!=ngh.size()-1) outfile<<ngh[j]<<":";
	  		else outfile<<ngh[j]<<",";
	  	}
	  	outfile<<posX<<","<<posY<<std::endl;
	}


	// Connector 2 (right down on gerber)
	for(int i=0; i<64; i++){
	  	ngh.clear();
	  	cnt = 2;
	  	pitch = 0.8;  
	  	inter = 0;

	  	if(i<32){
	  		axis = 'x';
	  		// stripNb = 31-i;
	  		stripNb = 64 + i;
	  		posX = -30.65;
			posY = 5.45 + pitch*i;
	  	}
	  	else{
	  		axis = 'y';
	  		// stripNb = 63-i;
	  		stripNb = 127-i;
	  		posX = -30.25 + pitch*(stripNb-64);
			posY = 5.851;
	  	}
	  	if(i!=0 && i!=32) ngh.push_back(i-1);
	  	if(i!=31 && i!=63) ngh.push_back(i+1);
	  	
	  	outfile<<cnt<<","<<i<<","<<stripNb<<","<<axis<<","<<pitch<<","<<inter<<",";
	  	
	  	for(int j=0; j<ngh.size(); j++){
	  		if(j!=ngh.size()-1) outfile<<ngh[j]<<":";
	  		else outfile<<ngh[j]<<",";
	  	}
	  	outfile<<posX<<","<<posY<<std::endl;
	}

	// Connector 3 (right up on gerber)
	for(int i=0; i<64; i++){
	  	ngh.clear();
	  	cnt = 3;
	  	pitch = 1.5;  
	  	inter = 0;

	  	if(i<32){
	  		axis = 'x';
	  		// stripNb = 31-i;
	  		stripNb = 96+i;
	  		posX = -41.805;
			posY = 58.394 + pitch*i;
	  	}
	  	else{
	  		axis = 'y';
	  		// stripNb = 63-i;
	  		stripNb = 159-i;
	  		posX = -41.055 + pitch*(stripNb-96);
			posY = 59.144;
	  	}
	  	if(i!=0 && i!=32) ngh.push_back(i-1);
	  	if(i!=31 && i!=63) ngh.push_back(i+1);
	  	
	  	outfile<<cnt<<","<<i<<","<<stripNb<<","<<axis<<","<<pitch<<","<<inter<<",";
	  	
	  	for(int j=0; j<ngh.size(); j++){
	  		if(j!=ngh.size()-1) outfile<<ngh[j]<<":";
	  		else outfile<<ngh[j]<<",";
	  	}
	  	outfile<<posX<<","<<posY<<std::endl;
	}

	outfile.close();

 	return 0;
}