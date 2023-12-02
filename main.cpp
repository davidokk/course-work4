#include <iostream>
#include <map>
#include <concepts>
#include <ranges>
#include <list>
#include <unordered_map>
#include <numeric>

#include "test_runner.h"
#include "helper.h"
#include "profile.h"

using namespace std;

template <typename T>
concept RandomAccessContainer =
std::ranges::random_access_range<T> &&            // требуем произвольный доступ к элементам
std::equality_comparable<typename T::value_type>; // требуем сравнение элементов
// ???? требовать хешфункцию или оператор меньше ????

template <typename T>
struct DefaultIndexer {
    T operator()(const T& t) { return t; };
};

// вершины хранятся в массиве
template <
    RandomAccessContainer T = string,
    typename NodeType = unordered_map<typename T::value_type, int>,
    typename Indexer = DefaultIndexer<typename T::value_type>
>
class SuffixTree {
public:
    SuffixTree(T str) : s(str), t(1), ptr(0, 0) {
        Build();
    }

    SuffixTree(T str, Indexer indexer) : s(str), t(1), ptr(0, 0) {
        Build();
    }

    int Find(T substr) {
        State st(0, 0);
        int i = 0;
        while (i < substr.size()) {
            int v = t[st.v].get(substr[i]);
            if (v == -1)
                return -1;
            while (i < substr.size() && st.pos < t[v].len()) {
                if (substr[i] != s[t[v].l + st.pos])
                    return -1;
                ++st.pos;
                ++i;
            }
            if (i == substr.size())
                return t[v].l + st.pos - substr.size();
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
            for (int i = 0; i < next.size(); i++) 
                next[i] = 0;
        }
        int len() { return r - l; }
        int& get(typename T::value_type c) {
            auto idx = indexer(c);
            if (next[idx] == 0)
                next[idx] = -1;
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
                if (st.v == -1)
                    return st;
            }
            else {
                if (s[t[st.v].l + st.pos] != s[l])
                    return State(-1, -1);
                if (r - l < t[st.v].len() - st.pos)
                    return State(st.v, st.pos + r - l);
                l += t[st.v].len() - st.pos;
                st.pos = t[st.v].len();
            }
        return st;
    }

    int Split(State st) {
        // если в конце вершины возвращаем ее же
        if (st.pos == t[st.v].len())
            return st.v;
        // если в начале вершины то возвращаем родителя
        if (st.pos == 0)
            return t[st.v].par;

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
        if (t[v].link != -1)
            return t[v].link;
        if (t[v].par == -1)
            return 0;
        // переходим по суффиксной ссылке родителя и спускаемся
        // если родетель корень - отрезаем один символ
        int to = GetLink(t[v].par);
        return t[v].link = Split(Go(State(to, t[to].len()), t[v].l + (t[v].par == 0), t[v].r));
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
            if (!mid)
                break;
        }
    }

    T s;
    vector<Node> t;
    State ptr;
    // Indexer indexer;
};

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

void SimpleTestHash() {
    SimpleTest<SuffixTree<string>>();
}

void SimpleTestArray() {
    auto lambda = [](char c) {
        return c - 'a';
    };
    SimpleTest<SuffixTree<string, array<int, 26>, decltype(lambda)>>();
}

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

template<typename SufTree>
void StressTest() {
    const int n = 1000;
    auto str = randomString(n);
    SufTree sf(str);

    for (int i = 0; i < n; i++) {
        for (int j = i; j < n; j++) {
            auto sub = str.substr(i, j - i + 1);
            ASSERT_EQUAL(sf.Find(sub), str.find(sub))
        } 
    }

    for (int i = 0; i < n; i++) {
        auto sub = randomString(rand() % (n + 100));
        ASSERT_EQUAL(sf.Find(sub), str.find(sub))
    }
}

void StressTestHash() {
    StressTest<SuffixTree<string>>();
    StressTest<SuffixTree<string, unordered_map<char, int>>>(); // the same but should test
}

void StressTestTree() {
    StressTest<SuffixTree<string, map<char, int>>>();
}

void StressTestArray() {
    auto lambda = [](char c) {
        return int(c - 'a');
    };
    StressTest<SuffixTree<string, array<int, 26>, decltype(lambda)>>();
}

void BenchMarks() {
    const int stringSize = 1000000;
    // const int queryCnt = 100000;

    {
        LOG_DURATION("map")
        SuffixTree<string, map<char, int>> sf(randomString(stringSize));
        // for (int i = 0; i < queryCnt; i++) {
        //     sf.Find(randomString(rand() % stringSize));
        // }
    }

    {
        LOG_DURATION("unordered_map")
        SuffixTree<string, unordered_map<char, int>> sf(randomString(stringSize));
        // for (int i = 0; i < queryCnt; i++) {
        //     sf.Find(randomString(rand() % stringSize));
        // }
    }

    {
        LOG_DURATION("array")
        auto lambda = [](char c) {
            return int(c - 'a');
        };
        SuffixTree<string, array<int, 26>, decltype(lambda)> sf(randomString(stringSize));
        // for (int i = 0; i < queryCnt; i++) {
        //     sf.Find(randomString(rand() % stringSize));
        // }
    }
}


int main() {

    TestRunner tr;
    RUN_TEST(tr, SimpleTestHash);
    RUN_TEST(tr, SimpleTestArray);
    RUN_TEST(tr, UnicodeStringTest);
    RUN_TEST(tr, SequenceTest);
    RUN_TEST(tr, StressTestHash);
    RUN_TEST(tr, StressTestTree);
    RUN_TEST(tr, StressTestArray);

    BenchMarks();
}
