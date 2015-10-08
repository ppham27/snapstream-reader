#include <algorithm>
#include <climits>
#include <cmath>

#include "StringHasher.h"

const int MAX_M = 1000000;

snap::StringHasher::StringHasher(const std::string &text,
                                 unsigned int M, unsigned int A) : M(M), A(A) { 
  load_text(text);
  if (M <= MAX_M) {           // precompute modular inverses
    modular_inverses.resize(M - 1);
    if (M > sqrt(UINT_MAX)) {
      long long ALL = A;
      for (int i = M - 2; i >= 0; --i) {
        modular_inverses[i] = ALL;
        ALL *= A;
        ALL %= M;
      }      
    } else {
      unsigned int ALL = A;
      for (int i = M - 2; i >= 0; --i) {
        modular_inverses[i] = ALL;
        ALL *= A;
        ALL %= M;
      }      
    }
  }
}

void snap::StringHasher::load_text(const std::string &text) {
  if (text.size() > 0) {
    N = text.size();    
    if (N > H.size()) H.resize(N);
    // build cumulative hash
    H[0] = text[0];    
    if (M > sqrt(UINT_MAX)) {
      long long ALL = A;
      for (int i = 1; i < text.size(); ++i) {
        long long newHash = H[i-1];  
        newHash += ALL*text[i];
        newHash %= M;
        ALL *= A;
        ALL %= M;
        H[i] = newHash;
      }
    } else {
      unsigned int ALL = A;
      for (int i = 1; i < text.size(); ++i) {
        unsigned int newHash = H[i-1];  
        newHash += ALL*text[i];
        newHash %= M;
        ALL *= A;
        ALL %= M;
        H[i] = newHash;
      }
    }
  } else {
    N = 0;
  }
}

int snap::StringHasher::hash(int i, int j) {
  i = std::max(i, 0); j = std::min(j, N-1); // make sure we don't exceed bounds of text
  if (M > sqrt(UINT_MAX)) {
    long long subHash = H[j];    
    if (i > 0) subHash -= H[i-1];
    // this is ci*A^i + ... + cj*A^j
    if (subHash < 0) subHash += M;
    // we need to multiply through by A^(-i)
    // by Fermat's little theorem, A^(M-1) = 1
    // ==> A^(i)*A^(M-1-i) = 1 ==> A^(-i) = A^(M-1-i)
    i %= M - 1;
    if (M <= MAX_M) {
      subHash *= modular_inverses[i];
    } else {
      subHash *= mod_exp(A, M - 1 - i, M);
    }
    subHash %= M;
    return (int) subHash;    
  } else {
    unsigned int subHash = H[j];    
    if (i > 0) {
      if (subHash < H[i-1]) subHash += M;
      subHash -= H[i-1];
    }
    // this is ci*A^i + ... + cj*A^j
    // we need to multiply through by A^(-i)
    // by Fermat's little theorem, A^(M-1) = 1
    // ==> A^(i)*A^(M-1-i) = 1 ==> A^(-i) = A^(M-1-i)
    i %= M - 1;
    if (M <= MAX_M) {
      subHash *= modular_inverses[i];
    } else {
      subHash *= mod_exp(A, M - 1 - i, M);
    }
    subHash %= M;
    return (int) subHash;
  }
}

int snap::StringHasher::mod_exp(unsigned int A, unsigned int B, unsigned int M) {
  // A^B mod M, undefined behaviour for -B
  long long C = 1LL;
  long long ALL = A;
  while (B > 0) {
    if ((B & 1) == 1) {
      C *= ALL;
      C %= M;
    }
    B >>= 1;
    ALL *= ALL;
    ALL %= M;
  }
  return (int) C;
}



