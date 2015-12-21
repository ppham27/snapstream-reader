#ifndef DISTANCE_H
#define DISTANCE_H

/*
 * These functions work on a symmetric matrix where large M[i,j] indicates high correlation to
 * produce a new matrix of distances.
 * 
 * The inputs will be of type std::map<std::string, std::map<std::string, int>> 
 * The map will have N keys. The first key will have value to a map of N keys.
 * The second key will have value to a map of N - 1 keys
 * Thus, there are 1 + 2 + ... + N = N(N+1)/2 entries
 */

#include <map>
#include <string>
#include <tuple>

namespace distance {

  std::map<std::string, std::map<std::string, double>> int_matrix_to_double_matrix(const std::map<std::string, std::map<std::string, int>> &M);

  // make a new matrix by filtering out top entries using the diagonal
  // param 1 is the original matrix
  // param 2 is the number of countries
  std::map<std::string, std::map<std::string, double>> filter_top(const std::map<std::string, std::map<std::string, double>> &M, int n);

  // calculate a correlation coefficient where given M return A
  // Let S be the sum of M
  // R and C be row and column sum
  // A[i][j] = S*M[i][j]/(R[i]*C[j])
  std::map<std::string, std::map<std::string, double>> correlate_sum(const std::map<std::string, std::map<std::string, double>> &M);
  
  // get size by taking to the power of occurrences which are along the diagonal
  std::map<std::string, double> size_pow(const std::map<std::string, std::map<std::string, double>> &M, double p);
  
  // calculate distance using 1/(1+M[i,j])
  std::map<std::string, std::map<std::string, double>> distance_inv(const std::map<std::string, std::map<std::string, double>> &M,
                                                                    double d);

  // convert to csv before converting to json
  std::string size_distance_to_csv(const std::map<std::string, double> &size,
                                   const std::map<std::string, std::map<std::string, double>> &distance);
  std::string size_distance_to_csv(const std::map<std::string, double> &sizes,
                                   const std::map<std::string, std::map<std::string, double>> &distance,
                                   const std::map<std::string, std::tuple<std::string, std::string, std::string>> &dict);
}



#endif
