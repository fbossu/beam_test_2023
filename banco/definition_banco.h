#ifndef hitbanco_h
#define hitbanco_h

namespace banco {
  struct hit {
    uint16_t chipCol;
    uint16_t row;
    uint16_t ladderId;
    uint16_t chipId;
    uint64_t trgTime;
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

  struct track {
    float x0, y0;
    float mx, my;
    float ex0, ey0;
    float emx, emy;
    float chi2x;
    float chi2y;
  };
}

#endif
