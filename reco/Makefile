ROOT = $(shell root-config --cflags) $(shell root-config --libs) -lEG -lGenVector
JSON = -I /home/fbossu/software/json/include/

all: dict reco_det

dict: Dict.cxx definitions.h linkdef.h
	rootcling -f Dict.cxx definitions.h linkdef.h

parse_json.o: parse_json.cpp parse_json.h
	g++ -c parse_json.cpp $(JSON)

reco_det: dict reco_det.cpp parse_json.o ../reco/definitions.h ../reco/linkdef.h ../map/DetectorTable.o ../map/DreamTable.o
	g++ reco_det.cpp Dict.cxx -o reco_det parse_json.o ../map/DetectorTable.o ../map/DreamTable.o $(ROOT) $(JSON)
clean:
	rm reco_det


#reco: reco.cpp definitions.h linkdef.h ../map/DetectorTable.o ../map/DreamTable.o
	#g++ reco.cpp Dict.cxx -o reco ../map/DetectorTable.o ../map/DreamTable.o $(shell root-config --cflags) $(shell root-config --libs )
