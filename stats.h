#pragma once

#include <map>
#include <vector>

using namespace std;

template <std::integral T>
double avg(const std::vector<T>& v) {
  return 1.0 * accumulate(v.begin(), v.end(), 0) / v.size();
}

template <std::integral T>
T median(const std::vector<T>& v) {
  std::vector<T> copy = v;
  size_t n = v.size() / 2;
  nth_element(copy.begin(), copy.begin() + n, copy.end());
  return copy[n];
}

class Stats {
 public:
  void Add(int depth, int childCnt) {
    childs.push_back(childCnt);
    depthMap[depth].push_back(childCnt);
  }

  vector<int> Childs() const { return childs; }

  pair<vector<int>, vector<double>> DepthsAvg() const {
    return DepthAggregation<double>(avg<int>);
  }

  pair<vector<int>, vector<int>> DepthsMedian() const {
    return DepthAggregation<int>(median<int>);
  }

  pair<vector<int>, vector<int>> DepthsCount() const {
    return DepthAggregation<int>([](const vector<int>& v) { return v.size(); });
  }

 private:
  template <typename ResType, typename AggFunc>
  pair<vector<int>, vector<ResType>> DepthAggregation(AggFunc agg) const {
    vector<int> d;
    vector<ResType> res;
    for (const auto& [depth, values] : depthMap) {
      d.push_back(depth);
      res.push_back(agg(values));
    }
    return {d, res};
  }

  vector<int> childs;
  map<int, vector<int>> depthMap;
};