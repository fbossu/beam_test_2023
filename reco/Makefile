all: reco reco_det

reco: reco.cpp definitions.h linkdef.h ../map/DetectorTable.o
	rootcling -f Dict.cxx definitions.h linkdef.h
	g++ reco.cpp Dict.cxx -o reco ../map/DetectorTable.o  $(shell root-config --cflags) $(shell root-config --libs )

reco_det: reco_det.cpp definitions.h linkdef.h ../map/DetectorTable.o
	rootcling -f Dict.cxx definitions.h linkdef.h
	g++ reco_det.cpp Dict.cxx -o reco_det ../map/DetectorTable.o  $(shell root-config --cflags) $(shell root-config --libs )

../map/DetectorTable.o: ../map/DetectorTable.h ../map/DetectorTable.cpp
	g++ -c ../map/DetectorTable.cpp -o ../map/DetectorTable.o

clean:
	rm reco Dict*
