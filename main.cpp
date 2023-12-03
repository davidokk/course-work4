#include <deque>
#include <fstream>
#include <iostream>
#include <vector>

#include "helper.h"
#include "indexer.h"
#include "profile.h"
#include "suffix_tree.h"
#include "test_runner.h"
#include "tests.h"

using namespace std;

template <typename Tree, typename StrGen>
pair<vector<int>, vector<int>> GraphicRun(int step, int N, StrGen gen) {
  vector<int> ns, ms;
  for (int size = step; size <= N; size += step) {
    int time;
    {
      LOG_DURATION(&time);
      Tree sf(gen(size));
    }
    ns.push_back(size);
    ms.push_back(time);
  }
  return make_pair(ns, ms);
}

void Graphics() {
  const int step = 100'000;
  const int N = 2'000'000;

  ofstream fout("graphics.txt");

  //   auto hash = GraphicRun<SuffixTree<string>>(step, N, randomDNA);
  //   fout << vectorToPythonString("hash_x", hash.first) << endl;
  //   fout << vectorToPythonString("hash_y", hash.second) << endl;

  //   auto tree = GraphicRun<SuffixTree<string, map<char, int>>>(step, N);
  //   fout << vectorToPythonString("tree_x", tree.first) << endl;
  //   fout << vectorToPythonString("tree_y", tree.second) << endl;

  //   auto arr = GraphicRun<ArrSufTreeWithDNA>(step, N, randomDNA);
  //   fout << vectorToPythonString("arr_x", arr.first) << endl;
  //   fout << vectorToPythonString("arr_y", arr.second) << endl;

  //   auto vec = GraphicRun<VecSufTreeWithEngAlph>(step, N, randomStringEng);
  //   fout << vectorToPythonString("vec_x", arr.first) << endl;
  //   fout << vectorToPythonString("vec_y", arr.second) << endl;
}

template <typename Tree, typename StrGen>
int BuildTreeBenchmark(int size, StrGen gen, int attempts = 1) {
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

int main() {
  RunTests();
  //   Graphics();

  // SuffixTree<vector<int>> sf(randomIntVec<100>(1'000'000));
  // // SuffixTree<string> sf(randomStringEng(1'000'000));

  // ofstream fout("graphics/stats1000.txt");
  // printContainer(fout, sf.GetStats().Childs());

  // cout << BuildTreeBenchmark<SuffixTree<string>>(5000000, randomStringEng) <<
  // endl; cout << BuildTreeBenchmark<SuffixTree<string>>(5000000, randomDNA) <<
  // endl;

  // cout << BuildTreeBenchmark<VecSufTreeWithIntSeq<2>>(
  //             5000000, randomIntVec<2>)
  //      << endl;
  // cout << BuildTreeBenchmark<VecSufTreeWithIntSeq<10>>(
  //             5000000, randomIntVec<10>)
  //      << endl;
  // cout << BuildTreeBenchmark<VecSufTreeWithIntSeq<50>>(
  //             5000000, randomIntVec<50>)
  //      << endl;
  // cout << BuildTreeBenchmark<VecSufTreeWithIntSeq<100>>(
  //             5000000, randomIntVec<100>)
  //      << endl;
  // cout << BuildTreeBenchmark<VecSufTreeWithIntSeq<1000>>(
  //             5000000, randomIntVec<1000>)
  //      << endl;

  // cout << BuildTreeBenchmark<SuffixTree<vector<int>>>(5000000, randomIntVec<2>)
  //      << endl;
  // cout << BuildTreeBenchmark<SuffixTree<vector<int>>>(5000000, randomIntVec<10>)
  //      << endl;
  // cout << BuildTreeBenchmark<SuffixTree<vector<int>>>(5000000, randomIntVec<50>)
  //      << endl;
  // cout << BuildTreeBenchmark<SuffixTree<vector<int>>>(5000000,
  //                                                     randomIntVec<100>)
  //      << endl;
  // cout << BuildTreeBenchmark<SuffixTree<vector<int>>>(5000000,
  //                                                     randomIntVec<1000>)
  //      << endl;

}
