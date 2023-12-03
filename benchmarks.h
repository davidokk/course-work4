#pragma once

#include <fstream>
#include <iostream>

#include "suffix_tree.h"

using namespace std;

template <typename Tree, typename StrGen>
int BuildTreeBenchmark(int size, StrGen gen, int attempts = 10) {
  int sum = 0;
  for (int i = 0; i < attempts; i++) {
    int time = 0;
    {
      LOG_DURATION(&time)
      Tree sf(gen(size));
    }
    sum += time;
  }
  return sum / attempts;
}

template <typename Tree, typename StrGen>
pair<vector<int>, vector<int>> GraphicTimeN(int step, int N, StrGen gen) {
  vector<int> n, time;
  for (int size = step; size <= N; size += step) {
    n.push_back(size);
    time.push_back(BuildTreeBenchmark<Tree>(size, gen));
  }
  return make_pair(n, time);
}

void GraphicCompareNodeType() {
  const int step = 100'000;
  const int N = 3'000'000;

  cerr << "GraphicCompareNodeType\n";
  ofstream out("graphics/compare_node_type.txt");

  {
    LOG_DURATION("hash")
    auto hash = GraphicTimeN<SuffixTree<string>>(step, N, randomStringEng);
    out << vecToString(hash.first) << "\n";
    out << vecToString(hash.second) << "\n";
  }

  {
    LOG_DURATION("tree")
    auto tree = GraphicTimeN<SuffixTree<string, map<char, int>>>(
        step, N, randomStringEng);
    out << vecToString(tree.first) << "\n";
    out << vecToString(tree.second) << "\n";
  }

  {
    LOG_DURATION("vector")
    auto vec = GraphicTimeN<VecSufTreeWithEngAlph>(step, N, randomStringEng);
    out << vecToString(vec.first) << "\n";
    out << vecToString(vec.second) << "\n";
  }

  {
    LOG_DURATION("array")
    auto arr = GraphicTimeN<ArrSufTreeWithEngAlph>(step, N, randomStringEng);
    out << vecToString(arr.first) << "\n";
    out << vecToString(arr.second);
  }
}