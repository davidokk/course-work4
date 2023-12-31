#pragma once

#include <concepts>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

using namespace std;

string randomStringEng(int n);
string randomDNA(int n);

template <int AlphSize> 
vector<int> randomIntVec(int n) {
  vector<int> res;
  while (n--) {
    res.push_back(rand() % AlphSize);
  }
  return res;
}

template<typename T> 
string vecToString(const vector<T>& v) {
  ostringstream out;
  out << "[";
  bool first = true;
  for (const auto& i : v) {
    if (!first) {
      out << ",";
    }
    out << i;
    first = false;
  }
  out << "]";
  return out.str();
}
