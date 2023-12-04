#pragma once

#include <fstream>
#include <iostream>

#include "suffix_tree.h"
#include "profile.h"

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
void GraphicTimeN(ostream& out, int step, int N, StrGen gen) {
  vector<int> n, time;
  for (int size = step; size <= N; size += step) {
    n.push_back(size);
    time.push_back(BuildTreeBenchmark<Tree>(size, gen));
  }
  out << vecToString(n) << '\n';
  out << vecToString(time) << '\n';
}

void GraphicCompareNodeType() {
  const int step = 100'000;
  const int N = 3'000'000;

  cerr << "GraphicCompareNodeType" << endl;
  ofstream out("graphics/compare_node_type.txt");

  {
    LOG_DURATION("hash")
    GraphicTimeN<SuffixTree<string>>(out, step, N, randomStringEng);
  }

  {
    LOG_DURATION("tree")
    GraphicTimeN<SuffixTree<string, map<char, int>>>(out, step, N,
                                                     randomStringEng);
  }

  {
    LOG_DURATION("vector")
    GraphicTimeN<VecSufTreeWithEngAlph>(out, step, N, randomStringEng);
  }

  {
    LOG_DURATION("array")
    GraphicTimeN<ArrSufTreeWithEngAlph>(out, step, N, randomStringEng);
  }
}

void GraphicCompareAlphSize() {
  const int step = 100'000;
  const int N = 3'000'000;

  cerr << "GraphicCompareAlphSize" << endl;
  ofstream out("graphics/compare_alph_size.txt");

#define HASH(n)                                                           \
  {                                                                       \
    LOG_DURATION(string("hash") + #n)                                     \
    GraphicTimeN<SuffixTree<vector<int>>>(out, step, N, randomIntVec<n>); \
  }

#define TREE(n)                                                            \
  {                                                                        \
    LOG_DURATION(string("tree") + #n)                                      \
    GraphicTimeN<SuffixTree<vector<int>, map<int, int>>>(out, step, N,     \
                                                         randomIntVec<n>); \
  }

#define VECTOR(n)                                                         \
  {                                                                       \
    LOG_DURATION(string("vector") + #n)                                   \
    GraphicTimeN<VecSufTreeWithIntSeq<n>>(out, step, N, randomIntVec<n>); \
  }

#define ARRAY(n)                                                          \
  {                                                                       \
    LOG_DURATION(string("array") + #n)                                    \
    GraphicTimeN<ArrSufTreeWithIntSeq<n>>(out, step, N, randomIntVec<n>); \
  }

#define TEST(size) \
  HASH(size)       \
  TREE(size)       \
  VECTOR(size)     \
  ARRAY(size)

  TEST(2)
  TEST(5)
  TEST(10)
  TEST(50)
  TEST(100)
  TEST(1000)
}

void GraphicStatsChilds() {
  cerr << "GraphicStatsChilds" << endl;
  ofstream out("graphics/childs.txt");

  for (auto size : {1000, 10'000'000}) {
    SuffixTree<vector<int>> sf(randomIntVec<26>(size));
    out << vecToString(sf.GetStats().Childs()) << "\n";
  }

  SuffixTree<vector<int>> sf(randomIntVec<1000>(10'000'000));
  out << vecToString(sf.GetStats().Childs()) << "\n";
}

void GraphicStatsChildsDepth() {
  cerr << "GraphicStatsChildsDepth" << endl;
  ofstream out("graphics/childs_depth.txt");

  const int N = 10'000'000;
  {
    SuffixTree<vector<int>> sf(randomIntVec<26>(N));
    auto stats = sf.GetStats();
    auto avg = stats.DepthsAvg();
    auto cnt = stats.DepthsCount();
    out << vecToString(cnt.first) << "\n";
    out << vecToString(cnt.second) << "\n";
    out << vecToString(avg.second) << "\n";
  }

  {
    SuffixTree<vector<int>> sf(randomIntVec<5>(N));
    auto stats = sf.GetStats();
    auto avg = stats.DepthsAvg();
    auto cnt = stats.DepthsCount();
    out << vecToString(cnt.first) << "\n";
    out << vecToString(cnt.second) << "\n";
    out << vecToString(avg.second) << "\n";
  }
}

void RunGraphics() {
  cerr << "Run benchmarks for graphics... It can take a while" << endl;
  GraphicCompareNodeType();
  GraphicCompareAlphSize();
  GraphicStatsChilds();
  GraphicStatsChildsDepth();
}