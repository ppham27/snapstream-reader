#ifndef EXCERPT_H
#define EXCERPT_H

#include <string>
#include <vector>

#include "Program.h"

namespace snap {
  class Excerpt {
  public:
    std::string program_title;
    std::vector<std::string> search_strings;
    std::string text;
    Excerpt(const snap::Program &p, int from, int to);
  };  
}


#endif

