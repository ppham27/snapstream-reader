#include <string>
#include <vector>

#include "Program.h"
#include "Excerpt.h"

snap::Excerpt::Excerpt(const snap::Program &p, int from, int to) {
  this -> search_strings = std::vector<std::string>(0);
  this -> program_title = p.title;
  this -> text = p.text;
}

