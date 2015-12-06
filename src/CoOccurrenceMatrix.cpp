#include <set>

#include "CoOccurrenceMatrix.h"



#include <iostream>
#include <algorithm>
#include <iterator>
#include <climits>

snap::CoOccurrenceMatrix::CoOccurrenceMatrix(const std::vector<snap::Expression> &expressions,
                                             int M, int A,
                                             int left_hash_width, int right_hash_width) : expressions(expressions),
                                                                                          hasher(M, A), 
                                                                                          left_hash_width(left_hash_width), 
                                                                                          right_hash_width(right_hash_width) { 
  // initial patterns for find command
  std::set<std::string> pattern_set;
  for (snap::Expression e : expressions) pattern_set.insert(e.patterns.begin(), e.patterns.end());
  patterns.insert(patterns.end(), pattern_set.begin(), pattern_set.end());
  // initial result matrix
  for (auto it0 = expressions.begin(); it0 != expressions.end(); ++it0) {
    results[it0 -> raw_expression] = std::map<std::string, std::tuple<int, int, int>>();
    for (auto it1(it0); it1 != expressions.end(); ++it1) {
      results[it0 -> raw_expression][it1 -> raw_expression] = std::make_tuple(0, 0, 0);
    }
  }
}

long long snap::CoOccurrenceMatrix::pair_hash(int l, int r) {
  long long hash = l;
  hash <<= 31;
  return hash + r;
}

void snap::CoOccurrenceMatrix::test() {
  long long hash = pair_hash(INT_MAX,INT_MAX);
  long long tmp = hash;
  std::vector<int> h;
  while (tmp > 0) { 
    h.push_back(tmp & 1);
    tmp >>= 1;
  }
  std::reverse(h.begin(), h.end());
  std::copy(h.begin(), h.end(), std::ostream_iterator<int>(std::cout));
  std::cout << std::endl;
  std::copy(patterns.begin(), patterns.end(), std::ostream_iterator<std::string>(std::cout,"|"));
  std::cout << std::endl;
  for (snap::Expression e : expressions) {
    std::cout << e.raw_expression << std::endl;
  }
}
