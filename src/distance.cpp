#include "distance.h"

#include <algorithm>
#include <cmath>
#include <vector>
#include <queue>
#include <sstream>

#include <iostream>


namespace distance {
  std::map<std::string, std::map<std::string, double>> int_matrix_to_double_matrix(const std::map<std::string, std::map<std::string, int>> &M) {
    std::map<std::string, std::map<std::string, double>> newM; 
    for (auto it = M.begin(); it != M.end(); ++it) {
      newM[it -> first] = std::map<std::string, double>();
      for (auto jt = (it -> second).begin(); jt != (it -> second).end(); ++jt) {
        newM[it->first][jt->first] = M.at(it -> first).at(jt -> first);
      }
    }    
    return newM;
  }
  
  std::map<std::string, std::map<std::string, double>> filter_top(const std::map<std::string, std::map<std::string, double>> &M, 
                                                                  int n) {

    // use a priority queue, if N is the number of keys and we're filter for the top n
    // then we're O(N*logN) in time and O(n) in space since we only need to keep the top n keys    
    // found so far in the priority queue
    auto comp = [](std::pair<std::string, double> a, std::pair<std::string, double> b) -> bool { return b.second < a.second; };
    std::priority_queue<std::pair<std::string, double>, std::vector<std::pair<std::string, double>>, decltype(comp)> top_n_queue(comp);
     
    for (auto it = M.begin(); it != M.end(); ++it) {
      if (top_n_queue.size() < n) {
        top_n_queue.emplace(it -> first, M.at(it -> first).at(it -> first));
      } else if (top_n_queue.top().second < M.at(it -> first).at(it -> first)) {
        top_n_queue.pop();
        top_n_queue.emplace(it -> first, M.at(it -> first).at(it -> first));
      }
    }

    std::vector<std::string> top_keys;
    while (!top_n_queue.empty()) {
      top_keys.push_back(top_n_queue.top().first);
      top_n_queue.pop(); 
    }

    std::map<std::string, std::map<std::string, double>> newM;
    std::sort(top_keys.begin(), top_keys.end());     
    for (auto it = top_keys.begin(); it != top_keys.end(); ++it) {
      newM[*it] = std::map<std::string, double>();
      for (auto jt(it); jt != top_keys.end(); ++jt) {
        newM[*it][*jt] = M.at(*it).at(*jt);
      }
    }    
    return newM;
  }

  std::map<std::string, std::map<std::string, double>> correlate_sum(const std::map<std::string, std::map<std::string, double>> &M) {
    std::map<std::string, std::map<std::string, double>> A;
    std::map<std::string, double> R;
    std::map<std::string, double> C;
    double S;    
    for (auto it = M.begin(); it != M.end(); ++it) {
      for (auto jt = (it -> second).begin(); jt != (it -> second).end(); ++jt) {
        double value = M.at(it -> first).at(jt -> first);
        if (it -> first == jt -> first) {
          S += value;
          R[it -> first] += value;
          C[jt -> first] += value;
        } else {
          S += 2*value;
          R[it -> first] += value;
          R[jt -> first] += value;
          C[it -> first] += value;
          C[jt -> first] += value;          
        }
      }
    }
    for (auto it = M.begin(); it != M.end(); ++it) {
      A[it->first] = std::map<std::string, double>();
      for (auto jt = (it -> second).begin(); jt != (it -> second).end(); ++jt) {
        A[it->first][jt->first] = S*M.at(it->first).at(jt->first)/R[it->first]/C[jt->first];
      }
    }    
    return A;
  }

  std::map<std::string, double> size_pow(const std::map<std::string, std::map<std::string, double>> &M, double p) {
    std::map<std::string, double> sizes;
    for (auto it = M.begin(); it != M.end(); ++it) {
      sizes[it->first] = pow(M.at(it -> first).at(it -> first), p);      
    }
    return sizes;
  }


  std::map<std::string, std::map<std::string, double>> distance_inv(const std::map<std::string, std::map<std::string, double>> &M,
                                                                    double d) {
    std::map<std::string, std::map<std::string, double>> newM;
    for (auto it = M.begin(); it != M.end(); ++it) {
      newM[it -> first] = std::map<std::string, double>();
      auto jt(it);
      newM[it -> first][jt -> first] = 0;
      ++jt;
      for (; jt != M.end(); ++jt) {
        newM[it -> first][jt -> first] = 1.0/(d + M.at(it -> first).at(jt -> first));
      }
    }
    return newM;      
  }

  std::string size_distance_to_csv(const std::map<std::string, double> &sizes,
                                   const std::map<std::string, std::map<std::string, double>> &distance) {
    std::ostringstream out;
    for (auto it = sizes.begin(); it != sizes.end(); ++it) {
      if (it != sizes.begin()) out << '\n';
      std::string symbol = it -> first;
      std::string name = it -> first;
      double size = it -> second;
      out << symbol << ',' << name << ',' << size;
      for (auto jt = sizes.begin(); jt != sizes.end(); ++jt) {
        if (it -> first <= jt -> first) {
          out << ',' << distance.at(it -> first).at(jt -> first);
        } else {
          out << ',' << distance.at(jt -> first).at(it -> first);
        }
      }
    }
    return out.str();
  }

  std::string size_distance_to_csv(const std::map<std::string, double> &sizes,
                                   const std::map<std::string, std::map<std::string, double>> &distance,
                                   const std::map<std::string, std::pair<std::string, std::string>> &dict) {
    std::ostringstream out;
    for (auto it = sizes.begin(); it != sizes.end(); ++it) {
      if (it != sizes.begin()) out << '\n';
      std::string symbol, name;
      if (dict.count(it -> first)) {
        symbol = dict.at(it -> first).first;
        name = dict.at(it -> first).second;
      } else {
        symbol = it -> first;
        name = it -> first;
      }
      double size = it -> second;
      out << symbol << ',' << name << ',' << size;
      for (auto jt = sizes.begin(); jt != sizes.end(); ++jt) {
        if (it -> first <= jt -> first) {
          out << ',' << distance.at(it -> first).at(jt -> first);
        } else {
          out << ',' << distance.at(jt -> first).at(it -> first);
        }
      }
    }
    return out.str();
  }
}
