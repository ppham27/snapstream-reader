#include <algorithm>

#include "StringHasher.h"

snap::StringHasher::StringHasher(const std::string &text,
                                 int M, int A) : text(text), M(M), A(A) { 
  if (text.size() > 0) {
    H.push_back(text[0]);
    // build cumulative hash
    long long ALL = A;
    for (int i = 1; i < text.size(); ++i) {
      long long newHash = H.back();  
      newHash += ALL*text[i];
      newHash %= M;
      ALL *= A;
      ALL %= M;
      H.push_back(newHash);
    }
  }
  N = text.size();
}


int snap::StringHasher::hash(int i, int j) {
  i = std::max(i, 0); j = std::min(j, N-1); // make sure we don't exceed bounds of text
  long long subHash = H[j];
  if (i > 0) subHash -= H[i-1];
  // this is ci*A^i + ... + cj*A^j
  if (subHash < 0) subHash += M;
  // we need to multiply through by A^(-i)
  // by Fermat's little theorem, A^(M-1) = 1
  // ==> A^(i)*A^(M-1-i) = 1 ==> A^(-i) = A^(M-1-i)
  subHash *= mod_exp(A, M - 1 - i, M);
  subHash %= M;
  return (int) subHash;
}

int snap::StringHasher::mod_exp(int A, int B, int M) {
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



