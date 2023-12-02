#pragma once

#include <chrono>
#include <iostream>
#include <string>

using namespace std;
using namespace std::chrono;

class LogDuration {
 public:
  explicit LogDuration(const string& msg = "")
      : message(msg + ": "), start(steady_clock::now()) {}

  explicit LogDuration(int* saveTo)
      : start(steady_clock::now()), save(saveTo) {}

  ~LogDuration() {
    auto finish = steady_clock::now();
    auto dur = finish - start;

    auto ms = duration_cast<milliseconds>(dur).count();

    if (save != nullptr) {
      *save = ms;
    } else {
      cerr << message << ms << " ms" << endl;
    }
  }

 private:
  string message;
  steady_clock::time_point start;
  int* save = nullptr;
};

#define UNIQ_ID_IMPL(lineno) _a_local_var_##lineno
#define UNIQ_ID(lineno) UNIQ_ID_IMPL(lineno)

#define LOG_DURATION(message) LogDuration UNIQ_ID(__LINE__)(message);