all: clusterSize.o clusterSizeAsa clusterSizeStrip clusterSizeInter correlation residu banco_coincidence xy_compare deltaT_samplemax

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
	g++ correlation.cpp ../reco/Dict.cxx -o correlation $(DT) $(ROOT)

residu: residu.cpp ../map/DetectorTable.o ../map/StripTable.o
# 	rootcling -f ../reco/Dict.cxx ../reco/definitions.h ../reco/linkdef.h
	g++ residu.cpp ../reco/Dict.cxx ../banco/DictBanco.cxx -o residu $(DT) $(ROOT)

banco_coincidence: banco_coincidence.cpp
	g++ banco_coincidence.cpp ../reco/Dict.cxx ../banco/DictBanco.cxx -o banco_coincidence $(DT) $(ROOT)

xy_compare: xy_compare.cpp clusterSize.cpp
	g++ xy_compare.cpp ../reco/Dict.cxx ../banco/DictBanco.cxx -o xy_compare $(DT) $(ROOT)

deltaT_samplemax: deltaT_samplemax.cpp clusterSize.cpp
	g++ deltaT_samplemax.cpp ../reco/Dict.cxx ../banco/DictBanco.cxx -o deltaT_samplemax $(DT) $(ROOT)