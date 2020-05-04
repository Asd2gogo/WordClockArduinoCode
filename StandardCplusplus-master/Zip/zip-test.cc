#include <cstdio>
#include <list>
#include <vector>

#include "zip.h"

int main() {
  std::vector<int> one{{1, 11}};
  auto two = [] { return std::vector<short>{{2, 22}}; };
  const std::list<float> three{{3, 33}};
  std::vector<int> four{{4, 44}};
  for (auto a : zip(one, two(), three, four)) {
    std::printf("%d %d %f %d\n", std::get<0>(a), std::get<short&>(a),
                std::get<const float&>(a), std::get<3>(a));
  }
}