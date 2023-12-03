#pragma once

template <typename T>
struct DefaultIndexer {
  T operator()(const T& c) const { return c; }
};

template <int AlphSize> 
struct IntSequenceIndxer {
  int operator()(int c) const { return c; }
  constexpr static int size = AlphSize;
};

struct EngAlphIndexer {
  int operator()(char c) const;
  constexpr static int size = 26;
};

struct DNAIndexer {
  int operator()(char c) const;
  constexpr static int size = 4;
};