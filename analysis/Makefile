all: clusterSize.o clusterSizeAsa clusterSizeStrip clusterSizeInter correlation residu banco_coincidence

ROOT = $(shell root-config --cflags) $(shell root-config --libs )
DT = ../map/DetectorTable.o ../map/StripTable.o clusterSize.o

clusterSize.o: clusterSize.cpp clusterSize.h
	rootcling -f ../reco/Dict.cxx ../reco/definitions.h ../reco/linkdef.h
	g++ -c clusterSize.cpp $(ROOT)

clusterSizeAsa: clusterSizeAsa.cpp ../reco/definitions.h ../reco/linkdef.h $(DT)
	rootcling -f ../reco/Dict.cxx ../reco/definitions.h ../reco/linkdef.h
	g++ clusterSizeAsa.cpp ../reco/Dict.cxx -o clusterSizeAsa $(DT) $(ROOT)

clusterSizeStrip: clusterSizeStrip.cpp ../reco/definitions.h ../reco/linkdef.h $(DT)
	rootcling -f ../reco/Dict.cxx ../reco/definitions.h ../reco/linkdef.h
	g++ clusterSizeStrip.cpp ../reco/Dict.cxx -o clusterSizeStrip $(DT) $(ROOT)


clusterSizeInter: clusterSizeInter.cpp ../reco/definitions.h ../reco/linkdef.h $(DT)
	rootcling -f ../reco/Dict.cxx ../reco/definitions.h ../reco/linkdef.h
	g++ clusterSizeInter.cpp ../reco/Dict.cxx -o clusterSizeInter $(DT) $(ROOT)


correlation: correlation.cpp
# 	rootcling -f ../reco/Dict.cxx ../reco/definitions.h ../reco/linkdef.h
	g++ correlation.cpp ../reco/Dict.cxx -o corelation $(DT) $(ROOT)

residu: residu.cpp
# 	rootcling -f ../reco/Dict.cxx ../reco/definitions.h ../reco/linkdef.h
	g++ residu.cpp ../reco/Dict.cxx ../banco/DictBanco.cxx -o residu $(DT) $(ROOT)

banco_coincidence: banco_coincidence.cpp
	g++ banco_coincidence.cpp ../reco/Dict.cxx ../banco/DictBanco.cxx -o banco_coincidence $(DT) $(ROOT)