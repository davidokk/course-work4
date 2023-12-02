#pragma once

#include <sstream>

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

template <typename T>
string vectorToPythonString(string name, const vector<T>& v) {
  ostringstream ss;
  ss << name << " = [";

  bool first = true;
  for (const T& i : v) {
    if (!first) {
      ss << ",";
    }
    first = false;
    ss << i;
  }

  ss << "]";
  return ss.str();
}