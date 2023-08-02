all: hitmap amplitude

hitmap: hitmap.cpp ../reco/definitions.h ../reco/linkdef.h
	rootcling -f ../reco/Dict.cxx ../reco/definitions.h ../reco/linkdef.h
	g++ hitmap.cpp ../reco/Dict.cxx -o hitmap  $(shell root-config --cflags) $(shell root-config --libs )

amplitude: amplitude.cpp ../reco/definitions.h ../reco/linkdef.h
	rootcling -f ../reco/Dict.cxx ../reco/definitions.h ../reco/linkdef.h
	g++ amplitude.cpp ../reco/Dict.cxx -o amplitude  $(shell root-config --cflags) $(shell root-config --libs )
