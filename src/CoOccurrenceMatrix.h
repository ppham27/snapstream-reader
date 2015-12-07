#ifndef COOCCURRENCE_MATRIX_H
#define COOCCURRENCE_MATRIX_H

#include <map>
#include <unordered_map>
#include <string>
#include <utility>
#include <tuple>
#include <vector>

#include "StringHasher.h"
#include "Expression.h"

namespace snap {  
  class CoOccurrenceMatrix {
  private:
    std::vector<snap::Expression> expressions;
    std::vector<std::string> patterns;
    snap::StringHasher hasher;
    int left_hash_width;
    int right_hash_width;
    // hash maps for term counts
    std::unordered_map<std::string, std::unordered_map<int, int>> left_hash_cnts;
    std::unordered_map<std::string, std::unordered_map<int, int>> right_hash_cnts;
    // hash maps for pair counts, ll, lr, rl, rr
    std::unordered_map<std::string, std::unordered_map<std::string, std::tuple<std::unordered_map<long long, int>, 
      std::unordered_map<long long, int>, 
      std::unordered_map<long long, int>, 
      std::unordered_map<long long, int>>>> pair_hash_cnts;    
    // contexts, programs, pairs
    std::map<std::string, std::map<std::string, std::tuple<int, int, int>>> cooccurrences;
    long long pair_hash(int l, int r);
  public:
    CoOccurrenceMatrix(const std::vector<snap::Expression> &expressions,
                       int M, int A, int left_hash_width, int right_hash_width);
    void add_program(const std::string &text, int distance);
    std::map<std::string, std::tuple<int, int, int>>& at(std::string a);    
  };
}

#endif
