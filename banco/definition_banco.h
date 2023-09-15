#ifndef hitbanco_h
#define hitbanco_h

#pragma link C++ struct hitBanco;
#pragma link C++ struct clusterBanco;
// /*#pragma link C++ class std::vector<hit>;*/

#include <vector>

// pixId = row*1e4 + col
// col = col + (chipId − 4) × 1024
// chipCol local chip column

struct hitBanco {
  uint16_t chipCol;
  uint16_t row;
  uint16_t ladderId;
  uint16_t chipId;
  int col;
  int pixId;
  int clusterId;
};

struct clusterBanco {
  float    colCentroid;
  float    rowCentroid;
  uint16_t size;
  uint16_t id;
  uint16_t ladderId;
};

#endif