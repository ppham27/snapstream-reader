#ifndef STRING_HASHER_H
#define STRING_HASHER_H

#include <string>
#include <vector>

namespace snap {
  class StringHasher {
  public:
    StringHasher(const std::string &text,
                 int M, int A);
    const std::string text;
    const int M;
    const int A;
  private:
    std::vector<int> H;    
  };
}

#endif


