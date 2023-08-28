all: clusterSizeAsa clusterSizeStrip

ROOT = $(shell root-config --cflags) $(shell root-config --libs )
DT = ../map/DetectorTable.o ../map/DTstrip.o

clusterSizeAsa: clusterSizeAsa.cpp ../reco/definitions.h ../reco/linkdef.h $(DT)
	rootcling -f ../reco/Dict.cxx ../reco/definitions.h ../reco/linkdef.h
	g++ clusterSizeAsa.cpp ../reco/Dict.cxx -o clusterSizeAsa $(ROOT) $(DT)

clusterSizeStrip: clusterSizeStrip.cpp ../reco/definitions.h ../reco/linkdef.h $(DT)
	rootcling -f ../reco/Dict.cxx ../reco/definitions.h ../reco/linkdef.h
	 g++ clusterSizeStrip.cpp ../reco/Dict.cxx -o clusterSizeStrip  $(ROOT) $(DT)


