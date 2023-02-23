#include <iostream>
#include <vector>
#include <numeric>
#include <functional>
#include <algorithm>
#include "Rnumgen.h"

using namespace std;


rnumgen::rnumgen(int seedvalue, vector<int> &v) {
  srand(seedvalue);
  F.resize(v.size());
  partial_sum(v.begin(), v.end(), F.begin());
  transform(F.begin(), F.end(), F.begin(),
  bind2nd(divides<float>(), F.back()));
} 
int rnumgen::rand() {
  const double randmax = RAND_MAX+1.0;
  const double p = (double)std::rand()/randmax;
  return upper_bound(F.begin(), F.end(), p) - F.begin();
}