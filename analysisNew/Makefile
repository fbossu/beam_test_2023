all: clusterSize.o xy_compare.o residu.o analysisMain timeRes overlayResidue spectreP2 hitmap

ROOT = $(shell root-config --cflags) $(shell root-config --libs ) -lGenVector
DT = ../map/DetectorTable.o ../map/StripTable.o clusterSize.o

clusterSize.o: clusterSize.cpp analysis.h
	rootcling -f ../reco/Dict.cxx ../reco/definitions.h ../reco/linkdef.h
	g++ -c clusterSize.cpp $(ROOT)

residu.o: analysis.h residu.cpp ../map/DetectorTable.o ../map/StripTable.o
	g++ -c residu.cpp ../reco/Dict.cxx ../banco/DictBanco.cxx $(ROOT)

xy_compare.o: analysis.h xy_compare.cpp clusterSize.cpp
	g++ -c xy_compare.cpp ../reco/Dict.cxx ../banco/DictBanco.cxx $(ROOT)

analysisMain: analysisMain.cpp analysis.h residu.o xy_compare.o clusterSize.o
	g++ -o analysisMain analysisMain.cpp residu.o xy_compare.o ../reco/Dict.cxx ../banco/DictBanco.cxx $(DT) $(ROOT)

timeRes: timeRes.cpp residu.o xy_compare.o clusterSize.o
	g++ -o timeRes timeRes.cpp residu.o xy_compare.o ../reco/Dict.cxx ../banco/DictBanco.cxx $(DT) $(ROOT)

overlayResidue: overlayResidue.cpp analysis.h
	g++ -o overlayResidue overlayResidue.cpp $(DT) $(ROOT)

spectreP2: spectreP2.cpp ../reco/definitions.h
	g++ -o spectreP2 spectreP2.cpp ../reco/Dict.cxx $(ROOT)

hitmap: hitmap.cpp analysis.h
	g++ -o hitmap hitmap.cpp ../reco/Dict.cxx $(DT) $(ROOT)