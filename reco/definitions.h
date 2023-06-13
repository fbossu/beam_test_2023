#ifndef hit_h
#define hit_h

#pragma link C++ struct hit;
/*#pragma link C++ class std::vector<hit>;*/

struct hit {
  uint16_t channel;
  uint16_t maxamp;
  uint16_t samplemax;

  /*ClassDef(hit,1) */
};
#endif
