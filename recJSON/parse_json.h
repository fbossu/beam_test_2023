#ifndef PARSE_JSON_H
#define PARSE_JSON_H

#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class ParseJson {
    public:
        ParseJson()=default;
        ~ParseJson()=default;

        ParseJson(std::string jsonPath, std::string detName);
        std::string decodePath();
        // std::string recoPath(); could be added to json
        // std::string mapPath(); could be added to json
        std::string runName();
        int x1Feu();
        int x2Feu();
        int y1Feu();
        int y2Feu();

    private:
        std::string jsonPath, detName;
        json strJson, detInfo;
};

#endif