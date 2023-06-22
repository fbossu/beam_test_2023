all: inter pitch asa DetectorTable

asa: factory_asa.cpp
	g++ factory_asa.cpp -o factory_asa

pitch: factory_pitch.cpp
	g++ factory_pitch.cpp -o factory_pitch

inter: factory_inter.cpp
	g++ factory_inter.cpp -o factory_inter

DetectorTable: DetectorTable.cpp DetectorTable.h
	g++ DetectorTable.cpp -o DetectorTable
