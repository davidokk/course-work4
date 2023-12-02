#include <concepts>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <ranges>
#include <unordered_map>

#include "helper.h"
#include "indexer.h"
#include "profile.h"
#include "test_runner.h"
#include "tests.h"

using namespace std;

template <typename T>
concept RandomAccessContainer =
    std::ranges::random_access_range<T> &&  // требуем произвольный доступ к
                                            // элементам
    std::equality_comparable<typename T::value_type>;  // требуем сравнение
                                                       // элементов
// ???? требовать хешфункцию или оператор меньше ????

// вершины хранятся в массиве
template <RandomAccessContainer T = string,
          typename NodeType = unordered_map<typename T::value_type, int>,
          typename Indexer = DefaultIndexer<typename T::value_type>>
class SuffixTree {
 public:
  SuffixTree(T str) : s(str), t(1), ptr(0, 0) { Build(); }

  SuffixTree(T str, Indexer indexer) : s(str), t(1), ptr(0, 0) { Build(); }

  int Find(T substr) {
    State st(0, 0);
    int i = 0;
    while (i < substr.size()) {
      int v = t[st.v].get(substr[i]);
      if (v == -1) return -1;
      while (i < substr.size() && st.pos < t[v].len()) {
        if (substr[i] != s[t[v].l + st.pos]) return -1;
        ++st.pos;
        ++i;
      }
      if (i == substr.size()) return t[v].l + st.pos - substr.size();
      st = State(v, 0);
    }
    return 0;
  }

 private:
  void Build() {
    t.reserve(s.size() * 2);
    for (int i = 0; i < s.size(); i++) {
      TreeExtend(i);
    }
  }

  // для вершины храним данные о ребре входящем в нее из предка
  // полуинтервал [l, r) - границы подстроки
  // par - номер вершины родителя
  // link - суффиксная ссылка
  struct Node {
    int l, r, par, link;
    NodeType next;
    Indexer indexer;

    Node(int l = 0, int r = 0, int par = -1) : l(l), r(r), par(par), link(-1) {
      // no effect for map
      for (int i = 0; i < next.size(); i++) next[i] = 0;
    }
    int len() { return r - l; }
    int& get(typename T::value_type c) {
      auto idx = indexer(c);
      if (next[idx] == 0) next[idx] = -1;
      return next[idx];
    }
  };

  // v - номер вершины в массиве t
  // pos - сколько символов отступили от начала вершины
  struct State {
    int v, pos;
    State(int v, int pos) : v(v), pos(pos) {}
  };

  State Go(State st, int l, int r) {
    while (l < r)
      if (st.pos == t[st.v].len()) {
        st = State(t[st.v].get(s[l]), 0);
        if (st.v == -1) return st;
      } else {
        if (s[t[st.v].l + st.pos] != s[l]) return State(-1, -1);
        if (r - l < t[st.v].len() - st.pos) return State(st.v, st.pos + r - l);
        l += t[st.v].len() - st.pos;
        st.pos = t[st.v].len();
      }
    return st;
  }

  int Split(State st) {
    // если в конце вершины возвращаем ее же
    if (st.pos == t[st.v].len()) return st.v;
    // если в начале вершины то возвращаем родителя
    if (st.pos == 0) return t[st.v].par;

    // если в середине - разделяем и возвращаем новую вершнину
    Node v = t[st.v];
    t.emplace_back(v.l, v.l + st.pos, v.par);
    t[v.par].get(s[v.l]) = t.size() - 1;
    t.back().get(s[v.l + st.pos]) = st.v;
    t[st.v].par = t.size() - 1;
    t[st.v].l += st.pos;
    return t.size() - 1;
  }

  int GetLink(int v) {
    if (t[v].link != -1) return t[v].link;
    if (t[v].par == -1) return 0;
    // переходим по суффиксной ссылке родителя и спускаемся
    // если родетель корень - отрезаем один символ
    int to = GetLink(t[v].par);
    return t[v].link = Split(
               Go(State(to, t[to].len()), t[v].l + (t[v].par == 0), t[v].r));
  }

  // Дополняем текущее дерево символом s[pos]
  void TreeExtend(int pos) {
    while (true) {
      // проверяем есть ли переход по символу s[pos]
      // если есть то переходим и завершаем итерацию
      State nptr = Go(ptr, pos, pos + 1);
      if (nptr.v != -1) {
        ptr = nptr;
        return;
      }

      // делим вершину и присоединяем лист
      int mid = Split(ptr);
      t.emplace_back(pos, s.size(), mid);
      t[mid].get(s[pos]) = t.size() - 1;

      // переходим по суффиксной ссылке к суффиксу меньшей длинны
      // повторяем цикл
      ptr.v = GetLink(mid);
      ptr.pos = t[ptr.v].len();

      // ????????
      if (!mid) break;
    }
  }

  T s;
  vector<Node> t;
  State ptr;
};

using ArrSufTreeWithEngAlph =
    SuffixTree<string, array<int, 26>, EnglishAlphabetIndexer>;

using ArrSufTreeWithDNA = SuffixTree<string, array<int, 4>, DNAIndexer>;


void BenchMarks() {
  const int stringSize = 5'000'000;
  const int queryCnt = 100'000;
  const int queryStringSize = 1000;

  auto run_queries = [=]<typename Tree>(Tree& sf) {
    for (int i = 0; i < queryCnt; i++) {
      sf.Find(randomStringEng(rand() % queryStringSize));
    }
  };

  string stringToTest = randomStringEng(stringSize);

  {
    LOG_DURATION("map")
    SuffixTree<string, map<char, int>> sf(stringToTest);
    run_queries(sf);
  }

  {
    LOG_DURATION("unordered_map")
    SuffixTree<string, unordered_map<char, int>> sf(stringToTest);
    run_queries(sf);
  }

  {
    LOG_DURATION("array")
    ArrSufTreeWithEngAlph sf(stringToTest);
    run_queries(sf);
  }
}

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

    auto hash = GraphicRun<SuffixTree<string>>(step, N, randomDNA);
    fout << vectorToPythonString("hash_x", hash.first) << endl;
    fout << vectorToPythonString("hash_y", hash.second) << endl;

  //   auto tree = GraphicRun<SuffixTree<string, map<char, int>>>(step, N);
  //   fout << vectorToPythonString("tree_x", tree.first) << endl;
  //   fout << vectorToPythonString("tree_y", tree.second) << endl;

//   auto arr = GraphicRun<ArrSufTreeWithDNA>(step, N, randomDNA);
//   fout << vectorToPythonString("arr_x", arr.first) << endl;
//   fout << vectorToPythonString("arr_y", arr.second) << endl;
}

int main() {
  RunTests();
  BenchMarks();
  Graphics();
}
