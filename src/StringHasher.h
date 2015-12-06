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
    const unsigned int M;
    const unsigned int A;
    StringHasher(unsigned int M, unsigned int A);
    StringHasher(const std::string &text,
                 unsigned int M, unsigned int A);
    int hash(int i, int j);     /* get hash between range i and j inclusive */
    void load_text(const std::string &text);
  private:
    std::vector<unsigned int> H;    
    int N;    
    std::vector<unsigned int> modular_inverses;
    int mod_exp(unsigned int A, unsigned int B, unsigned int M);
  };
}

#endif


