#include "helper.h"

#include <iostream>

using namespace std;

string randomStringEng(int n) {
  string res;
  while (n--) {
    res += 'a' + rand() % 26;
  }
  return res;
}

string randomDNA(int n) {
  string res;
  const string dna = "ACGT";
  while (n--) {
    res += dna[rand() % 4];
  }
  return res;
}
