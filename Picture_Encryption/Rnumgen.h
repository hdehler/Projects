#ifndef __RNUMGEN_H__
#define __RNUMGEN_H__
#include <iostream>
#include <vector>

using namespace std;

class rnumgen {
  public:
    rnumgen(int seedvalue, std::vector<int> &v);
    int rand();

  private:
    vector<float> F;
};

#endif
