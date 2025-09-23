#include "parse_json.h"

ParseJson::ParseJson(std::string jsonPath, std::string detName): jsonPath(jsonPath), detName(detName) {
    std::ifstream jFile(jsonPath);
    strJson = json::parse(jFile);
    jFile.close();

    json dets = strJson["detectors"];
    for (auto it = dets.begin(); it != dets.end(); ++it){
        if(it.value()["name"] == detName){
            detInfo = it.value();
        }
    }
}

std::string ParseJson::decodePath(){
    return strJson["decoded_root_inner_dir"].get<std::string>();
};

std::string ParseJson::runName(){
    return strJson["run_name"].get<std::string>();
};

// the -1 is to convert fom Dylan's indexing to mine
int ParseJson::x1Feu(){
    return detInfo["dream_feus"]["x_1"].get<int>() - 1;
};

int ParseJson::x2Feu(){
    return detInfo["dream_feus"]["x_2"].get<int>() - 1;
};

int ParseJson::y1Feu(){
    return detInfo["dream_feus"]["y_1"].get<int>() - 1;
};

int ParseJson::y2Feu(){
    return detInfo["dream_feus"]["y_2"].get<int>() - 1;
};