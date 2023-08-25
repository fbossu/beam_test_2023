all: clusterSizeRegion

clusterSizeRegion: clusterSizeRegion.cpp ../reco/definitions.h ../reco/linkdef.h
	rootcling -f ../reco/Dict.cxx ../reco/definitions.h ../reco/linkdef.h
	g++ clusterSizeRegion.cpp ../reco/Dict.cxx -o clusterSizeregion  $(shell root-config --cflags) $(shell root-config --libs )
