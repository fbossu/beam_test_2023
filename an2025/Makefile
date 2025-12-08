ifndef BEAMTESTCODE
$(error BEAMTESTCODE is not set, it must point to base directory of the code repository)
endif



ROOT=$(shell root-config --cflags) $(shell root-config --libs ) -lGenVector

DICT=$(BEAMTESTCODE)/reco/Dict.cxx $(BEAMTESTCODE)/banco/DictBanco.cxx

all: hits eff res an

hits: hits.cpp
	g++ -c hits.cpp -o hits_cpp.o -I$(BEAMTESTCODE) $(ROOT)
eff: eff.cpp
	g++ -c eff.cpp -o eff_cpp.o -I$(BEAMTESTCODE) $(ROOT)
res: res.cpp
	g++ -c res.cpp -o res_cpp.o -I$(BEAMTESTCODE) $(ROOT)

an: analysis.cpp hits_cpp.o eff_cpp.o res_cpp.o
	g++ -o an analysis.cpp hits_cpp.o eff_cpp.o res_cpp.o utils.cpp $(DICT) $(BEAMTESTCODE)/map/*.o  -I$(BEAMTESTCODE) $(ROOT)
	ln -s $(BEAMTESTCODE)/reco/Dict_rdict.pcm
	ln -s $(BEAMTESTCODE)/banco/DictBanco_rdict.pcm

clean:
	rm *.o an
	unlink Dict_rdict.pcm
	unlink DictBanco_rdict.pcm
