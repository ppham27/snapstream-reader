#ifndef EXCERPT_H
#define EXCERPT_H

#include <string>
#include <vector>

#include "Program.h"

namespace snap {
  class Excerpt {
  public:
    std::string program_title;
    std::string date;
    std::vector<std::string> search_strings;
    std::string text;
    Excerpt(const snap::Program &p, int from, int to);
    void highlight_word(std::string w);
  };

  
}


#endif

