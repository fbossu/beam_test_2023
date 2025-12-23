ROOT = $(shell root-config --cflags) $(shell root-config --libs) -lGenVector
DT = ../map/DetectorTable.o ../map/StripTable.o

export ROOTSYS := $(ROOTSYS):$(PWD)/../reco
$(info ROOT_INCLUDE_PATH is $(ROOT_INCLUDE_PATH))

#all: inter strip asa DetectorTable.o DreamTable.o StripTable.o alignment checkTrlRot paramAlign
all: inter strip asa DetectorTable.o DreamTable.o StripTable.o

asa: factory_asa.cpp
	g++ factory_asa.cpp -o factory_asa

strip: factory_strip.cpp
	g++ factory_strip.cpp -o factory_strip

inter: factory_inter.cpp
	g++ factory_inter.cpp -o factory_inter

DetectorTable.o: DetectorTable.cpp DetectorTable.h
	g++ -c DetectorTable.cpp

DreamTable.o: DreamTable.cpp DreamTable.h DetectorTable.cpp DetectorTable.h
	g++ -c DreamTable.cpp

StripTable.o: StripTable.cpp StripTable.h DetectorTable.cpp DetectorTable.h
	g++ -c StripTable.cpp $(ROOT)

alignment: alignment.cpp ../analysis/clusterSize.o StripTable.o DreamTable.o DetectorTable.o
	g++ alignment.cpp ../reco/Dict.cxx ../banco/DictBanco.cxx -o alignment ../analysis/clusterSize.o $(DT) $(ROOT)

checkTrlRot: checkTrlRot.cpp ../analysis/clusterSize.cpp
	g++ checkTrlRot.cpp ../reco/Dict.cxx ../banco/DictBanco.cxx -o checkTrlRot ../analysis/clusterSize.o $(DT) $(ROOT)

paramAlign: paramAlign.cpp ../analysis/clusterSize.cpp
	g++ paramAlign.cpp ../reco/Dict.cxx ../banco/DictBanco.cxx -o paramAlign ../analysis/clusterSize.o $(DT) $(ROOT)
