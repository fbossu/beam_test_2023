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

// list of subruns
std::vector<std::string> ParseJson::subRuns(){
    std::vector<std::string> subruns;
    for(auto it = strJson["sub_runs"].begin(); it != strJson["sub_runs"].end(); ++it){
        subruns.push_back(it.value()["sub_run_name"].get<std::string>());
    }
    return subruns;
}

// one file for each
std::vector<std::string> ParseJson::decodeFiles(std::string subRun){
    std::vector<std::string> files;
    std::string path = strJson["run_out_dir"].get<std::string>() + "/" + subRun + "/" + strJson["filtered_root_inner_dir"].get<std::string>() + "/";
    DIR* dir;
    struct dirent* entry;
    if ((dir = opendir(path.c_str())) == nullptr) {
        std::cerr << "Could not open directory: " << path << std::endl;
        return files;
    }
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_REG) {
            std::string filename = entry->d_name;
            if (filename.find("decoded") != std::string::npos && filename.find("_array_") == std::string::npos) {
                if (filename.find(printf("_0%d", this->feu())) != std::string::npos) {
                    files.push_back(path + filename);
                }
            }
        }
    }
    closedir(dir);
    return files;
}

std::string ParseJson::runName(){
    return strJson["run_name"].get<std::string>();
};

// reco is setup to have all connectors plugged into one FEU only
int ParseJson::feu(){
    return detInfo["dream_feus"]["x_1"][0].get<int>();
}
    
// the -1 is to convert fom Dylan's indexing to mine
int ParseJson::x1Dream(){
    return detInfo["dream_feus"]["x_1"][1].get<int>() - 1;
}

int ParseJson::x2Dream(){
    return detInfo["dream_feus"]["x_2"][1].get<int>() - 1;
}

int ParseJson::y1Dream(){
    return detInfo["dream_feus"]["y_1"][1].get<int>() - 1;
}

int ParseJson::y2Dream(){
    return detInfo["dream_feus"]["y_2"][1].get<int>() - 1;
}