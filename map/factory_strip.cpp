#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

// all strip are counted from bottom to top in y direction and from left to right in x. Strips numbered from 0-127

int main(int argc, char const *argv[])
{
	//Output 
	std::ofstream outfile;
	outfile.open("strip_map.txt");
	outfile << "connector,conectorChannel,stripNb,axis,pitch(mm),interpitch(mm),neighbours(:separated)" << std::endl;

	double pitch = 0, inter = 0;
	char axis = 'n';
	std::vector<int> ngh;
	int cnt = 0, stripNb = 0;

	// Connector 0 (bottom left on gerber)
	for(int i=0; i<64; i++){
		ngh.clear();
		cnt = 0;
		pitch = 1.;  //mm
		inter = 0.75; //mm
		axis = 'y';
		stripNb = i;

	  	if(i!=0) ngh.push_back(i-1);
	  	if(i!=63) ngh.push_back(i+1);
		
		outfile<<cnt<<","<<i<<","<<stripNb<<","<<axis<<","<<pitch<<","<<inter<<",";
		
		for(int j=0; j<ngh.size(); j++){
			if(j!=ngh.size()-1) outfile<<ngh[j]<<":";
			else outfile<<ngh[j]<<std::endl;
		}
	}

	// Connector 1 (bottom right on gerber)
	for(int i=0; i<64; i++){
	  	ngh.clear();
	  	cnt = 1;
	  	axis = 'y';
	  	stripNb = 64 + i;

	  	if(i<32){
	  		pitch = 1.5;  
	  		inter = 1.12;
	  	}
	  	else{
	  		pitch = 0.5;  
	  		inter = 0.37;
	  	}
	  	if(i!=0 && i!=32) ngh.push_back(i-1);
	  	if(i!=31 && i!=63) ngh.push_back(i+1);
	  	
	  	outfile<<cnt<<","<<i<<","<<stripNb<<","<<axis<<","<<pitch<<","<<inter<<",";
	  	
	  	for(int j=0; j<ngh.size(); j++){
	  		if(j!=ngh.size()-1) outfile<<ngh[j]<<":";
	  		else outfile<<ngh[j]<<std::endl;
	  	}
	}


	// Connector 2 (right down on gerber)
	for(int i=0; i<64; i++){
	  	ngh.clear();
	  	cnt = 2;
	  	stripNb = i;
	  	axis = 'x';
	  	pitch = 1.;
	  	inter = 0.1;

	  	if(i!=0) ngh.push_back(i-1);
	  	if(i!=63) ngh.push_back(i+1);
	  	
	  	outfile<<cnt<<","<<i<<","<<stripNb<<","<<axis<<","<<pitch<<","<<inter<<",";
	  	
	  	for(int j=0; j<ngh.size(); j++){
	  		if(j!=ngh.size()-1) outfile<<ngh[j]<<":";
	  		else outfile<<ngh[j]<<std::endl;
	  	}
	}

	// Connector 3 (right up on gerber)
	for(int i=0; i<64; i++){
	  	ngh.clear();
	  	cnt = 3;
	  	axis = 'x';
	  	stripNb = 64 + i;

	  	if(i<32){
	  		pitch = 1.5;  
	  		inter = 0.1;
	  	}
	  	else{
	  		pitch = 0.5;  
	  		inter = 0.1;
	  	}
	  	if(i!=0 && i!=32) ngh.push_back(i-1);
	  	if(i!=31 && i!=63) ngh.push_back(i+1);
	  	
	  	outfile<<cnt<<","<<i<<","<<stripNb<<","<<axis<<","<<pitch<<","<<inter<<",";
	  	
	  	for(int j=0; j<ngh.size(); j++){
	  		if(j!=ngh.size()-1) outfile<<ngh[j]<<":";
	  		else outfile<<ngh[j]<<std::endl;
	  	}
	}

	outfile.close();

 	return 0;
}
