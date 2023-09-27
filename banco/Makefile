all: decode_banco reco_banco

decode_banco: decode_banco.cpp definition_banco.h linkdefBanco.h
	rootcling -f DictBanco.cxx definition_banco.h linkdefBanco.h
	g++ decode_banco.cpp DictBanco.cxx -o decode_banco $(shell root-config --cflags) $(shell root-config --libs )

reco_banco: reco_banco.cpp definition_banco.h linkdefBanco.h ladder.h ladder.cpp
	rootcling -f DictBanco.cxx definition_banco.h linkdefBanco.h
	g++ reco_banco.cpp DictBanco.cxx ladder.cpp -o reco_banco $(shell root-config --cflags) $(shell root-config --libs ) -lGenVector

clean:
	rm decode_banco Dict*

