all: inter strip asa DetectorTable

asa: factory_asa.cpp
	g++ factory_asa.cpp -o factory_asa

strip: factory_strip.cpp
	g++ factory_strip.cpp -o factory_strip

inter: factory_inter.cpp
	g++ factory_inter.cpp -o factory_inter

DetectorTable: DetectorTable.cpp DetectorTable.h
	g++ DetectorTable.cpp -o DetectorTable
