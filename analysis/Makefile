all: clusterSizeAsa clusterSizeStrip

clusterSizeAsa: clusterSizeAsa.cpp ../reco/definitions.h ../reco/linkdef.h
	rootcling -f ../reco/Dict.cxx ../reco/definitions.h ../reco/linkdef.h
	g++ clusterSizeAsa.cpp ../reco/Dict.cxx -o clusterSizeAsa  $(shell root-config --cflags) $(shell root-config --libs )

clusterSizeStrip: clusterSizeStrip.cpp ../reco/definitions.h ../reco/linkdef.h
	rootcling -f ../reco/Dict.cxx ../reco/definitions.h ../reco/linkdef.h
	 g++ clusterSizeStrip.cpp ../reco/Dict.cxx -o clusterSizeStrip  $(shell root-config --cflags) $(shell root-config --libs )


