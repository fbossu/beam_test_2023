ROOT = $(shell root-config --cflags) $(shell root-config --libs) -lEG -lRooFitCore -lGenVector
#JSON = -I /home/samy/software/json/include/

all: reco_det

parse_json.o: parse_json.cpp parse_json.h
	g++ -c parse_json.cpp $(JSON)

reco_det: reco_det.cpp parse_json.o ../reco/definitions.h ../reco/linkdef.h ../map/DetectorTable.o ../map/DreamTable.o
	g++ reco_det.cpp ../reco/Dict.cxx -o reco_det parse_json.o ../map/DetectorTable.o ../map/DreamTable.o $(ROOT) $(JSON)
