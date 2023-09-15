#include "TFile.h"
#include "TFile.h"
#include "TTree.h"
#include "TInterpreter.h"
#include "definition_banco.h"

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <iomanip>


void recoBanco(std::string fnameIn, std::string fnameOut){

	TFile* fin = TFile::Open(fnameIn.c_str(),"read");
	TTreeReader reader("events",fin);
	TTreeReaderValue<std::vector<hitBanco>> hits(reader, "hits");

	std::vector<clusterBanco> cl;
}

int main(int argc, char const *argv[])
{
	/* code */
	return 0;
}