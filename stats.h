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

  vector<pair<int, double>> DepthsAvg() const {
    return DepthAggregation<double>(avg<int>);
  }

  vector<pair<int, int>> DepthsMedian() const {
    return DepthAggregation<int>(median<int>);
  }

 private:
  template <typename ResType, typename AggFunc>
  vector<pair<int, ResType>> DepthAggregation(AggFunc agg) const {
    vector<pair<int, ResType>> res; 
    for (const auto& [depth, values] : depthMap) {
      res.emplace_back(depth, agg(values));
    }
    return res; 
  }

  vector<int> childs;
  map<int, vector<int>> depthMap;
};