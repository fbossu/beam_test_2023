#ifndef hit_h
#define hit_h

#pragma link C++ struct hit;
#pragma link C++ struct cluster;
/*#pragma link C++ class std::vector<hit>;*/

#include <vector>

struct hit {
  uint16_t channel;
  uint16_t maxamp;
  uint16_t samplemax;
  float    inflex;
  uint16_t clusterId;
  uint16_t strip;
  char axis;
  std::vector<uint16_t> samples;
};

struct cluster {
  float    centroid;
  uint16_t size;
  uint16_t id;
  char     axis;
  float    stripCentroid;
};
#endif
