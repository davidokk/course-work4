#include "indexer.h"

int EngAlphIndexer::operator()(char c) const { return c - 'a'; }

int DNAIndexer::operator()(char c) const {
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
