#pragma once

#include <concepts>
#include <forward_list>
#include <ranges>
#include <unordered_map>
#include <vector>

#include "helper.h"
#include "indexer.h"
#include "stats.h"

using namespace std;

template <typename T>
concept String =
    std::ranges::random_access_range<T> &&
    std::equality_comparable<typename T::value_type> && requires(T t) {
      { std::begin(t) } -> std::same_as<decltype(std::begin(t))>;
      { std::end(t) } -> std::same_as<decltype(std::end(t))>;
    };

// NodeType is one of map, unordered_map, vector, array
template <String T = string,
          typename NodeType = unordered_map<typename T::value_type, int>,
          typename Indexer = DefaultIndexer<typename T::value_type>>
class SuffixTree {
 public:
  SuffixTree(T str) : s(str), t(1), ptr(0, 0) { Build(); }

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

  // enable only for unordered_map NodeType
  Stats GetStats() {
    Stats st;
    if constexpr (std::is_same_v<NodeType, unordered_map<ElType, int>>) {
      StatsDfs(0, 0, st);
    }
    return st;
  }

 private:
  using ElType = typename T::value_type;

  void StatsDfs(int idx, int depth, Stats& st) {
    auto node = t[idx];
    st.Add(depth, static_cast<int>(node.next.size()));
    for (const auto& [ch, to] : node.next) {
      StatsDfs(to, depth + 1, st);
    }
  }

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
      if constexpr (std::is_same_v<NodeType, array<int, Indexer::size>>) {
        for (int i = 0; i < next.size(); i++) next[i] = 0;
      } else if constexpr (std::is_same_v<NodeType, vector<int>>) {
        next.resize(Indexer::size);
      }
    }

    int len() { return r - l; }

    int& get(ElType c) {
      if constexpr (std::is_same_v<NodeType, forward_list<pair<ElType, int>>>) {
        if (next.empty() || c < next.begin()->first) {
          next.emplace_front(c, -1);
          return next.begin()->second;
        }
        auto prev = next.begin();
        auto it = std::next(next.begin());
        while (it != next.end()) {
          if (it->first > c) break;
          prev = it;
          ++it;
        }
        if (prev->first != c) {
          prev = next.emplace_after(prev, c, -1);
        }
        return prev->second;
      } else {
        auto idx = indexer(c);
        if (next[idx] == 0) next[idx] = -1;
        return next[idx];
      }
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
    if (st.pos == t[st.v].len()) return st.v;
    if (st.pos == 0) return t[st.v].par;

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

      if (!mid) break;
    }
  }

  T s;
  vector<Node> t;
  State ptr;
};

using ArrSufTreeWithEngAlph =
    SuffixTree<string, array<int, EngAlphIndexer::size>, EngAlphIndexer>;

using ArrSufTreeWithDNA =
    SuffixTree<string, array<int, DNAIndexer::size>, DNAIndexer>;

template <int AlphSize>
using ArrSufTreeWithIntSeq =
    SuffixTree<vector<int>, array<int, IntSequenceIndxer<AlphSize>::size>,
               IntSequenceIndxer<AlphSize>>;

template <int AlphSize>
using VecSufTreeWithIntSeq =
    SuffixTree<vector<int>, vector<int>, IntSequenceIndxer<AlphSize>>;

using VecSufTreeWithEngAlph = SuffixTree<string, vector<int>, EngAlphIndexer>;

using ListSufTreeWithEngAlph =
    SuffixTree<string, forward_list<pair<char, int>>>;
using ListSufTreeWithIntSeq =
    SuffixTree<vector<int>, forward_list<pair<int, int>>>;

using MapSufTreeWithEngAlph = SuffixTree<string, map<char, int>>;
using MapSufTreeWithIntSeq = SuffixTree<vector<int>, map<int, int>>;
