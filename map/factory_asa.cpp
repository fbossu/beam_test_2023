#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <fstream>

// all strip are counted from top to botom in y direction and from left to right in x. Counted from 0.64 for each of the 4 active regions

int main(int argc, char const *argv[])
{
	//Output 
	std::ofstream outfile;
	outfile.open("ASA_map.txt");
	outfile << "connector,conectorChannel,stripNb,axis,pitch(mm),interpitch(mm),neighbours(:separated)" << std::endl;

	double pitch = 0, inter = 0;
	char axis = 'n';
	std::vector<int> ngh;
	int cnt = 0, stripNb = 0;

	// Connector 0 (bottom left on gerber)
	for(int i=0; i<64; i++){
		ngh.clear();
		cnt = 0;
		pitch = 2.;  //mm
		inter = 0;

		if(i<32){
			axis = 'x';
			stripNb = i;
		}
		else{
			axis = 'y';
			stripNb = i - 32;
		}
	  	if(i!=0 && i!=32) ngh.push_back(i-1);
	  	if(i!=31 && i!=63) ngh.push_back(i+1);
		
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
	  	pitch = 1.;  
	  	inter = 0;

	  	if(i<32){
	  		axis = 'y';
	  		stripNb = i;
	  	}
	  	else{
	  		axis = 'x';
	  		stripNb = 63 - i;
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
	  	pitch = 0.8;  
	  	inter = 0;

	  	if(i<32){
	  		axis = 'x';
	  		stripNb = 31-i;
	  	}
	  	else{
	  		axis = 'y';
	  		stripNb = 63-i;
	  	}
	  	if(i!=0 && i!=32) ngh.push_back(i-1);
	  	if(i!=31 && i!=63) ngh.push_back(i+1);
	  	
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
	  	pitch = 1.5;  
	  	inter = 0;

	  	if(i<32){
	  		axis = 'x';
	  		stripNb = 31-i;
	  	}
	  	else{
	  		axis = 'y';
	  		stripNb = 63-i;
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