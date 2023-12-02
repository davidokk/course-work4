#pragma once

template <typename T>
struct DefaultIndexer {
  T operator()(const T& t) { return t; };
};

struct EnglishAlphabetIndexer {
  int operator()(char c) const { return c - 'a'; }
};

struct DNAIndexer {
  int operator()(char c) const {
    switch (c) {
      case 'A':
        return 0;
      case 'C':
        return 1;
      case 'T':
        return 2;
      default:
        return 3;
    }
  }
};