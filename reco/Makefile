all: 
	rootcling -f Dict.cxx definitions.h linkdef.h
	g++ reco.cpp Dict.cxx -o reco $(shell root-config --cflags) $(shell root-config --libs )

clean:
	rm reco Dict*
