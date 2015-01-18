#ifndef EXCERPT_H
#define EXCERPT_H

#include <string>
#include <vector>

#include "Program.h"

namespace snap {
  class Excerpt {
  private:
    std::string raw_text;
  public:
    std::string program_title;
    std::string date;
    std::vector<std::string> search_strings;
    std::string text;
    Excerpt(const snap::Program &p, int from, int to);
    void highlight_word(std::string w);
    std::string get_raw_text();
  };

  
}


#endif

