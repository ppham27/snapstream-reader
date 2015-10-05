#ifndef STRING_HASHER_H
#define STRING_HASHER_H

#include <string>
#include <vector>

/**
 * Hash using Robin Karp algorithm
 * c0 + A*c1 + A^2*c2 + ... + A^n*cn mod M
 */

namespace snap {
  class StringHasher {
  public:
    const std::string text;
    const int M;
    const int A;
    StringHasher(const std::string &text,
                 int M, int A);
    int hash(int i, int j);     /* get hash between range i and j inclusive */
    
  private:
    std::vector<int> H;    
    int N;
    int mod_exp(int A, int B, int M);
  };
}

#endif


