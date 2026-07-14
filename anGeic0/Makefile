ifndef BEAMTESTCODE
$(error BEAMTESTCODE is not set, it must point to base directory of the code repository)
endif


all: hitmap

hitmap: hitmap.cpp
	g++ -o hitmap hitmap.cpp $(BEAMTESTCODE)/an2025/utils.cpp  $(BEAMTESTCODE)/reco/Dict.cxx $(BEAMTESTCODE)/map/*.o -I $(BEAMTESTCODE)/map -I $(BEAMTESTCODE) $(shell root-config --cflags) $(shell root-config --libs ) -lGenVector
	ln -f -s $(BEAMTESTCODE)/reco/Dict_rdict.pcm
