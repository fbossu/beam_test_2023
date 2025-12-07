ROOT=$(shell root-config --cflags) $(shell root-config --libs ) -lGenVector
TESTBEAM="/home/fbossu/Data/EIC/TestBeam/code/"

all: res

res: res.cpp
	g++ -o res res.cpp $(TESTBEAM)/analysisNew/*.o $(TESTBEAM)/map/*.o -I$(TESTBEAM) $(ROOT)
an: analysis.cpp hits.cpp
	g++ -c eff.cpp -o eff_cpp.o $(TESTBEAM)/map/*.o -I$(TESTBEAM) $(ROOT)
	g++ -c hits.cpp -o hits_cpp.o $(TESTBEAM)/map/*.o -I$(TESTBEAM) $(ROOT)
	g++ -o an analysis.cpp hits_cpp.o eff_cpp.o utils.cpp  $(TESTBEAM)/analysisNew/*.o  $(TESTBEAM)/map/*.o -I$(TESTBEAM) $(ROOT)
#g++ -o an analysis.cpp hits.cpp eff.cpp  $(TESTBEAM)/analysisNew/*.o $(TESTBEAM)/map/*.o -I$(TESTBEAM) $(ROOT)
#eff:
#g++ -o eff eff.cpp $(TESTBEAM)/analysisNew/*.o $(TESTBEAM)/map/*.o -I$(TESTBEAM) $(ROOT)
