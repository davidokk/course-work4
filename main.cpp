#include <deque>
#include <fstream>
#include <iostream>
#include <vector>

#include "helper.h"
#include "indexer.h"
#include "profile.h"
#include "suffix_tree.h"
#include "tests.h"
#include "benchmarks.h"

using namespace std;

int main() {
  // RunTests();
  // GraphicCompareNodeType();

  // статистика по вершинам 
  // SuffixTree<vector<int>> sf(randomIntVec<100>(1'000'000));
  // // SuffixTree<string> sf(randomStringEng(1'000'000));
  // ofstream fout("graphics/stats1000.txt");
  // printContainer(fout, sf.GetStats().Childs());

  // исследование скорости от размера алфавита 
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
