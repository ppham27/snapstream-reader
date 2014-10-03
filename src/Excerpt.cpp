#include <algorithm>
#include <string>
#include <vector>

#include "boost/algorithm/string.hpp"

#include "Program.h"
#include "Excerpt.h"



snap::Excerpt::Excerpt(const snap::Program &p, int from, int to) {
  this -> search_strings = std::vector<std::string>(0);
  this -> date = p.recorded_date;
  this -> program_title = p.title;
  int actual_from = std::max(0, from);
  int actual_length = std::min(to - from, (int) p.text.length() - actual_from);
  this -> text = p.text.substr(actual_from, actual_length);
}

void snap::Excerpt::highlight_word(std::string w) {
  this -> search_strings.push_back(w);
  std::string w_trimmed = boost::algorithm::trim_copy_if(w, boost::algorithm::is_any_of("*"));
  boost::ireplace_all(this -> text, w_trimmed, "<span style=\"color:red\">"+w_trimmed+"</span>");
}

