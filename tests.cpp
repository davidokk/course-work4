#include "test_runner.h"
#include "suffix_tree.h"
#include "indexer.h"
#include "helper.h"

#include <numeric>

template <typename SufTree>
void SimpleTest() {
  SufTree sf("abciwoegjwe");

  ASSERT_EQUAL(sf.Find(""), 0);
  ASSERT_EQUAL(sf.Find("e"), 6);
  ASSERT_EQUAL(sf.Find("iw"), 3);
  ASSERT_EQUAL(sf.Find("abc"), 0);
  ASSERT_EQUAL(sf.Find("gjw"), 7);
  ASSERT_EQUAL(sf.Find("gjwe"), 7);
  ASSERT_EQUAL(sf.Find("abciwoegjwe"), 0);

  ASSERT_EQUAL(sf.Find("y"), -1);
  ASSERT_EQUAL(sf.Find("abcv"), -1);
  ASSERT_EQUAL(sf.Find("woegjj"), -1);
  ASSERT_EQUAL(sf.Find("abciwoegjwee"), -1);
  ASSERT_EQUAL(sf.Find("babciwoegjwe"), -1);
  ASSERT_EQUAL(sf.Find("babciwoegjwe"), -1);
  ASSERT_EQUAL(sf.Find("babciwoegjwe"), -1);
  ASSERT_EQUAL(sf.Find("bbciwoegjwe"), -1);
}

void SimpleTestHash() { SimpleTest<SuffixTree<string>>(); }

void SimpleTestArray() { SimpleTest<ArrSufTreeWithEngAlph>(); }

void UnicodeStringTest() {
  SuffixTree<wstring> sf(L"😂😃😆");

  ASSERT_EQUAL(sf.Find(L"😃"), 1);
  ASSERT_EQUAL(sf.Find(L"😂😃"), 0);
  ASSERT_EQUAL(sf.Find(L"😂😃😈"), -1);
}

void SequenceTest() {
  SuffixTree<vector<int>> sf(vector<int>{1, 2, 3, -2, 4});

  ASSERT_EQUAL(sf.Find(vector<int>{2, 3}), 1);
  ASSERT_EQUAL(sf.Find(vector<int>{2, 4}), -1);
  ASSERT_EQUAL(sf.Find(vector<int>{1, 2, 3, -2, 4, 10}), -1);
  ASSERT_EQUAL(sf.Find(vector<int>{-1535}), -1);
  ASSERT_EQUAL(sf.Find(vector<int>{3, -2}), 2);

  vector<int> big(10000);
  iota(big.begin(), big.end(), 0);

  sf = SuffixTree<vector<int>>(big);
  ASSERT_EQUAL(sf.Find(vector<int>{764, 765}), 764);
  ASSERT_EQUAL(sf.Find(vector<int>{501, 502, 503, 504}), 501);
  ASSERT_EQUAL(sf.Find(vector<int>{9999}), 9999);
  ASSERT_EQUAL(sf.Find(vector<int>{325, 326, 324, 327}), -1);
  ASSERT_EQUAL(sf.Find(vector<int>{9999, 10000}), -1);
}

template <typename SufTree>
void StressTest() {
  const int n = 1000;
  auto str = randomStringEng(n);
  SufTree sf(str);

  for (int i = 0; i < n; i++) {
    for (int j = i; j < n; j++) {
      auto sub = str.substr(i, j - i + 1);
      ASSERT_EQUAL(sf.Find(sub), str.find(sub))
    }
  }

  for (int i = 0; i < n; i++) {
    auto sub = randomStringEng(rand() % (n + 100));
    ASSERT_EQUAL(sf.Find(sub), str.find(sub))
  }
}

void StressTestHash() {
  StressTest<SuffixTree<string>>();
  StressTest<SuffixTree<string, unordered_map<char, int>>>();
}

void StressTestTree() { StressTest<SuffixTree<string, map<char, int>>>(); }

void StressTestArray() { StressTest<ArrSufTreeWithEngAlph>(); }

void StressTestVector() { StressTest<VecSufTreeWithEngAlph>(); }

void StressTestList() { StressTest<ListSufTreeWithEngAlph>(); }

void IndexerTest() {
  ArrSufTreeWithDNA sf(randomDNA(1000));
  VecSufTreeWithEngAlph sf1(randomStringEng(1000));
  SuffixTree<string> sf2(randomDNA(1000));
  ArrSufTreeWithIntSeq<100> sf3(randomIntVec<100>(1000));
  VecSufTreeWithIntSeq<100> sf4(randomIntVec<100>(1000));
}

void RunTests() {
  cerr << "Run tests..." << endl;
  TestRunner tr;
  RUN_TEST(tr, SimpleTestHash);
  RUN_TEST(tr, SimpleTestArray);
  RUN_TEST(tr, UnicodeStringTest);
  RUN_TEST(tr, SequenceTest);
  RUN_TEST(tr, StressTestHash);
  RUN_TEST(tr, StressTestTree);
  RUN_TEST(tr, StressTestArray);
  RUN_TEST(tr, StressTestVector);
  RUN_TEST(tr, StressTestList);
  RUN_TEST(tr, IndexerTest);
}