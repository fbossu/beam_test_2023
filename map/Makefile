all: inter strip asa DetectorTable.o DTchannel.o DTstrip.o

asa: factory_asa.cpp
	g++ factory_asa.cpp -o factory_asa

strip: factory_strip.cpp
	g++ factory_strip.cpp -o factory_strip

inter: factory_inter.cpp
	g++ factory_inter.cpp -o factory_inter

DetectorTable.o: DetectorTable.cpp DetectorTable.h
	g++ -c DetectorTable.cpp

DTchannel.o: DTchannel.cpp DTchannel.h DetectorTable.cpp DetectorTable.h
	g++ -c DTchannel.cpp

DTstrip.o: DTstrip.cpp DTstrip.h DetectorTable.cpp DetectorTable.h
	g++ -c DTstrip.cpp