#ifndef hitbanco_h
#define hitbanco_h

namespace banco {
  struct hit {
    uint16_t chipCol;
    uint16_t row;
    uint16_t ladderId;
    uint16_t chipId;
    int col;
    int pixId;
    int clusterId;
  };

  struct cluster {
    float    colCentroid;
    float    rowCentroid;
    uint16_t size;
    uint16_t id;
    uint16_t ladderId;
  };
}

#endif
