#include <algorithm>
#include <set>
#include <unordered_set>

#include "CoOccurrenceMatrix.h"

#include "snap.h"

#include <iostream>

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
    cooccurrences[it0 -> raw_expression] = std::map<std::string, std::tuple<int, int, int>>();
    for (auto it1(it0); it1 != expressions.end(); ++it1) {
      cooccurrences[it0 -> raw_expression][it1 -> raw_expression] = std::make_tuple(0, 0, 0);
    }
  }
  // initialize hash counts
  for (snap::Expression e : expressions) {
    left_hash_cnts[e.raw_expression] = std::unordered_map<int, int>();
    right_hash_cnts[e.raw_expression] = std::unordered_map<int, int>();
    pair_hash_cnts[e.raw_expression] = std::unordered_map<std::string, std::tuple<std::unordered_map<long long, int>, 
                                                                                 std::unordered_map<long long, int>, 
                                                                                 std::unordered_map<long long, int>, 
                                                                                 std::unordered_map<long long, int>>>();
    for (snap::Expression f : expressions) {
      if (e.raw_expression != f.raw_expression) {
        pair_hash_cnts[e.raw_expression][f.raw_expression] = make_tuple(std::unordered_map<long long, int>(), 
                                                                       std::unordered_map<long long, int>(),
                                                                       std::unordered_map<long long, int>(),
                                                                       std::unordered_map<long long, int>());
      }
    }
  }

}

long long snap::CoOccurrenceMatrix::pair_hash(int l, int r) {
  long long hash = l;
  hash <<= 31;
  return hash + r;
}

std::map<std::string, std::tuple<int, int, int>>& snap::CoOccurrenceMatrix::at(std::string a) {
  return cooccurrences.at(a);
}

// (A,B,Al,Ar,Bl,Br,p) == (C,D,Cl,Cr,Dl,Dr,q) <==> A == C && B == D && p
// == q && (Al == Cl || Ar == Cr) && (Bl == Dl || Br == Dr) ==>
// (A,B,Al,Ar,Bl,Br,p) != (C,D,Cl,Cr,Dl,Dr,q) <==> A != C || B != D || p
// != q || (Al != Cl && Ar != Cr) || (Bl != Dl && Br != Dr)
// p and q refer to orientation

// For every oriented match, we store 4 hashes (Al,Bl), (Al,Br), (Ar,Bl),
// and (Ar,Br).  Upon a new cooccurrence (C,D,Cl,Cr,Dl,Dr,q), we'll check
// that there are no matches with any of the 4 pairs.

void snap::CoOccurrenceMatrix::add_program(const std::string &text, int distance) {
  hasher.load_text(text);
  // get match positions
  std::map<std::string, std::vector<int>> raw_match_positions = snap::find(patterns, text);
  std::map<std::string, std::vector<int>> match_positions = snap::evaluate_expressions(expressions, raw_match_positions);
  // put all the positions in one vector
  std::vector<std::pair<int, std::string>> positions;
  for (std::pair<std::string, std::vector<int>> position : match_positions) {
    for (int idx : position.second) positions.emplace_back(idx, position.first);
  }
  std::sort(positions.begin(), positions.end());

  std::unordered_set<std::string> program_added_pairs;
  std::unordered_set<std::string> context_added_pairs;
  for (auto it0 = positions.begin(); it0 != positions.end(); ++it0) {
    for (auto it1(it0); it1 >= positions.begin() && (it0 -> first) - (it1 -> first) <= distance; --it1) {
      if ((it0 -> first) == (it1 -> first)) {        
        // if points to the same match
        ++std::get<2>(cooccurrences[it0 -> second][it1 -> second]);
        // program match cnt
        if (program_added_pairs.count(it0 -> second) == 0) {
          ++std::get<1>(cooccurrences[it0 -> second][it1 -> second]);
          program_added_pairs.insert(it0 -> second);
        }
        // context cnt
        int left_hash = hasher.hash(it0 -> first - left_hash_width, it0 -> first);
        int right_hash = hasher.hash(it0 -> first, it0 -> first + right_hash_width);
        int left_hash_cnt = left_hash_cnts[it0 -> second][left_hash]++;
        int right_hash_cnt = right_hash_cnts[it0 -> second][right_hash]++;
        if (left_hash_cnt == 0 && right_hash_cnt == 0 && context_added_pairs.count(it0 -> second)) {
          ++std::get<0>(cooccurrences[it0 -> second][it1 -> second]);
          context_added_pairs.insert(it0 -> second);
        }
      } else if ((it0 -> second) != (it1 -> second)) {
        std::string expressionA, expressionB;
        std::string paired_string_hash;
        if ((it0 -> second) < (it1 -> second)) {
          expressionA = it0 -> second; 
          expressionB = it1 -> second;
          paired_string_hash = expressionA + "|" + expressionB;
        } else {                // (it0 -> second) > (it1 -> second)
          expressionA = it1 -> second; 
          expressionB = it0 -> second;
          paired_string_hash = expressionB + "|" + expressionA;
        } 
        // total count
        ++std::get<2>(cooccurrences[expressionA][expressionB]);
        // program match count
        if (program_added_pairs.count(paired_string_hash) == 0) {
          ++std::get<1>(cooccurrences[expressionA][expressionB]);
          program_added_pairs.insert(paired_string_hash);
        }
        // context match
        // left 1, left 0
        long long ll_hash = pair_hash(hasher.hash(it1 -> first - left_hash_width, it1 -> first),
                                      hasher.hash(it0 -> first - left_hash_width, it0 -> first));
        int ll_hash_cnt = std::get<0>(pair_hash_cnts[it1 -> second][it0 -> second])[ll_hash]++;
        // left 1, right 0
        long long lr_hash = pair_hash(hasher.hash(it1 -> first - left_hash_width, it1 -> first),
                                      hasher.hash(it0 -> first, it0 -> first + right_hash_width));
        int lr_hash_cnt = std::get<1>(pair_hash_cnts[it1 -> second][it0 -> second])[lr_hash]++;
        // right 1, left 0
        long long rl_hash = pair_hash(hasher.hash(it1 -> first, it1 -> first + right_hash_width),
                                      hasher.hash(it0 -> first - left_hash_width, it0 -> first));
        int rl_hash_cnt = std::get<2>(pair_hash_cnts[it1 -> second][it0 -> second])[rl_hash]++;
        // right 1, right 0
        long long rr_hash = pair_hash(hasher.hash(it1 -> first, it1 -> first + right_hash_width),
                                      hasher.hash(it0 -> first, it0 -> first + right_hash_width));
        int rr_hash_cnt = std::get<3>(pair_hash_cnts[it1 -> second][it0 -> second])[rr_hash]++;
        if (ll_hash_cnt == 0 && lr_hash_cnt == 0 &&
            rl_hash_cnt == 0 && rr_hash_cnt == 0 &&
            context_added_pairs.count(paired_string_hash) == 0) {
          ++std::get<0>(cooccurrences[expressionA][expressionB]);
          context_added_pairs.insert(paired_string_hash);
        }        
      }
    }
  }
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
