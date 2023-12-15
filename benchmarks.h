#pragma once

#include <mach/mach.h>
#include <mach/task.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <map>
#include <vector>

#include "profile.h"
#include "suffix_tree.h"

using namespace std;

template <typename Tree, typename StrGen>
int BuildTreeBenchmark(int size, StrGen gen, int attempts = 5) {
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
void GraphicTimeN(ostream& out, int step, int N, StrGen gen, int attempts = 5) {
  vector<int> n, time;
  for (int size = step; size <= N; size += step) {
    n.push_back(size);
    time.push_back(BuildTreeBenchmark<Tree>(size, gen, attempts));
  }
  out << vecToString(n) << '\n';
  out << vecToString(time) << '\n';
}

void GraphicCompareNodeType() {
  const int step = 100'000;
  const int N = 3'000'000;
  const int attempts = 10;

  cerr << "GraphicCompareNodeType" << endl;
  ofstream out("graphics/compare_node_type.txt");

  {
    LOG_DURATION("hash")
    GraphicTimeN<SuffixTree<string>>(out, step, N, randomStringEng, attempts);
  }

  {
    LOG_DURATION("tree")
    GraphicTimeN<MapSufTreeWithEngAlph>(out, step, N, randomStringEng,
                                        attempts);
  }

  {
    LOG_DURATION("list")
    GraphicTimeN<ListSufTreeWithEngAlph>(out, step, N, randomStringEng,
                                         attempts);
  }

  {
    LOG_DURATION("array")
    GraphicTimeN<ArrSufTreeWithEngAlph>(out, step, N, randomStringEng,
                                        attempts);
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

#define TREE(n)                                                        \
  {                                                                    \
    LOG_DURATION(string("tree") + #n)                                  \
    GraphicTimeN<MapSufTreeWithIntSeq>(out, step, N, randomIntVec<n>); \
  }

#define LIST(n)                                                         \
  {                                                                     \
    LOG_DURATION(string("list") + #n)                                   \
    GraphicTimeN<ListSufTreeWithIntSeq>(out, step, N, randomIntVec<n>); \
  }

#define ARRAY(n)                                                          \
  {                                                                       \
    LOG_DURATION(string("array") + #n)                                    \
    GraphicTimeN<ArrSufTreeWithIntSeq<n>>(out, step, N, randomIntVec<n>); \
  }

#define TEST(size) \
  HASH(size)       \
  TREE(size)       \
  LIST(size)       \
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

  SuffixTree<vector<int>> sf(randomIntVec<100>(10'000'000));
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

void GraphicsMapAndHash() {
  cerr << "GraphicsMapAndHash" << endl;
  ofstream out("graphics/map_and_hash.txt");

  const int N = 3'000'000;
  const int step = 100'000;
  constexpr int alphsize = 10'000;

  {
    LOG_DURATION("hash")
    GraphicTimeN<SuffixTree<vector<int>>>(out, step, N, randomIntVec<alphsize>);
  }
  {
    LOG_DURATION("tree")
    GraphicTimeN<MapSufTreeWithIntSeq>(out, step, N, randomIntVec<alphsize>);
  }
}

void RunGraphics() {
  cerr << "Run benchmarks for graphics... It can take a while" << endl;
  GraphicCompareNodeType();
  GraphicCompareAlphSize();
  GraphicsMapAndHash();
  GraphicStatsChilds();
  GraphicStatsChildsDepth();
}

// works on macos
double getMemoryUsage() {
  mach_task_basic_info_data_t info;
  mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;

  if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info,
                &infoCount) != KERN_SUCCESS) {
    return 0;  // Unable to retrieve information
  }

  return 1.0 * info.resident_size / 1024 / 1024;  // Convert bytes to megabytes
}

void MemoryUsage() {
  const int N = 10'000'000;
  const int attempts = 3;

  cerr << "MemoryUsage" << endl;
  ofstream out("graphics/memory_usage.txt");

  map<string, vector<double>> m;

#define MEM(TreeType, struct, n)         \
  {                                      \
    double sum = 0;                      \
    LOG_DURATION(string(struct) + #n)    \
    for (int i = 0; i < attempts; i++) { \
      TreeType sf(randomIntVec<n>(N));   \
      usleep(5000);                      \
      sum += getMemoryUsage();           \
    }                                    \
    m[struct].push_back(sum / attempts); \
  }

#define RUN(TreeType, struct) \
  MEM(TreeType, struct, 2)    \
  MEM(TreeType, struct, 5)    \
  MEM(TreeType, struct, 10)   \
  MEM(TreeType, struct, 50)   \
  MEM(TreeType, struct, 100)  \
  MEM(TreeType, struct, 1000)

#define RUN_ARR(TreeType, struct) \
  MEM(TreeType<2>, struct, 2)     \
  MEM(TreeType<5>, struct, 5)     \
  MEM(TreeType<10>, struct, 10)   \
  MEM(TreeType<50>, struct, 50)   \
  MEM(TreeType<100>, struct, 100) \
  MEM(TreeType<1000>, struct, 1000)

  RUN(SuffixTree<vector<int>>, "hash")
  RUN(MapSufTreeWithIntSeq, "tree")
  RUN(ListSufTreeWithIntSeq, "list")
  RUN_ARR(ArrSufTreeWithIntSeq, "array")

  out << vecToString(vector<int>{2, 5, 10, 50, 100, 1000}) << "\n";
  out << vecToString(m["hash"]) << "\n";
  out << vecToString(m["tree"]) << "\n";
  out << vecToString(m["list"]) << "\n";
  out << vecToString(m["array"]) << "\n";
}
