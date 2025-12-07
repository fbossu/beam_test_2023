ifndef BEAMTESTCODE
$(error BEAMTESTCODE is not set, it must point to base directory of the code repository)
endif



ROOT=$(shell root-config --cflags) $(shell root-config --libs ) -lGenVector

DICT="$(BEAMTESTCODE)/reco/Dict.cxx $(BEAMTESTCODE)/banco/DictBanco.cxx" 

all: an

an: analysis.cpp hits.cpp
	g++ -c eff.cpp -o eff_cpp.o -I$(BEAMTESTCODE) $(ROOT)
	g++ -c hits.cpp -o hits_cpp.o -I$(BEAMTESTCODE) $(ROOT)
	g++ -o an analysis.cpp hits_cpp.o eff_cpp.o utils.cpp $(BEAMTESTCODE)/map/*.o -I$(BEAMTESTCODE) $(ROOT)
	ln -s $(BEAMTESTCODE)/reco/Dict_rdict.pcm
	ln -s $(BEAMTESTCODE)/banco/DictBanco_rdict.pcm

clean:
	rm *.o an
	unlink Dict_rdict.pcm
	unlink DictBanco_rdict.pcm
