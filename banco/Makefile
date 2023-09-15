all: decode_banco

decode_banco: decode_banco.cpp definition_banco.h linkdefBanco.h
	rootcling -f DictBanco.cxx definition_banco.h linkdefBanco.h
	g++ decode_banco.cpp DictBanco.cxx -o decode_banco $(shell root-config --cflags) $(shell root-config --libs )

# reco_det: reco_det.cpp definitions.h linkdef.h ../map/DetectorTable.o
# 	rootcling -f Dict.cxx definitions.h linkdef.h
# 	g++ reco_det.cpp Dict.cxx -o reco_det ../map/DetectorTable.o  $(shell root-config --cflags) $(shell root-config --libs )

# ../map/DetectorTable.o: ../map/DetectorTable.h ../map/DetectorTable.cpp
# 	g++ -c ../map/DetectorTable.cpp -o ../map/DetectorTable.o

# ../map/DetectorTable.o: ../map/DetectorTable.h ../map/DetectorTable.cpp
# 	g++ -c ../map/DetectorTable.cpp -o ../map/DetectorTable.o

clean:
	rm decode_banco
