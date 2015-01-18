#include <algorithm>
#include <cctype>
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
  this -> raw_text = p.text.substr(actual_from, actual_length);
  this -> text = std::string(raw_text);
}

std::string snap::Excerpt::get_raw_text() { return this -> raw_text; }

void snap::Excerpt::highlight_word(std::string w) {
  if (w.size() == 0 || w == "*" || w == "**") { return; }
  bool pre_wild_card = false; bool post_wild_card = false;
  if (w.front() == '*') { pre_wild_card = true; }
  if (w.back() == '*') { post_wild_card = true; }
  this -> search_strings.push_back(w);
  std::string w_trimmed = boost::algorithm::trim_copy_if(w, boost::algorithm::is_any_of("*"));
  std::transform(w_trimmed.begin(), w_trimmed.end(), w_trimmed.begin(), ::tolower);
  std::string lower_text(text);
  std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
  int offset = 0; int left_shift = 24; int right_shift = 7;
  int current_pos = 0;
  while ((current_pos = lower_text.find(w_trimmed, current_pos)) != -1) {
    bool highlight = true;
    if (current_pos != 0 && !pre_wild_card && !(isspace(lower_text[current_pos-1]) || ispunct(lower_text[current_pos-1]))) {
      highlight = false;
    }
    if (current_pos + w_trimmed.length() != lower_text.length() && 
        !post_wild_card && 
        !(isspace(lower_text[current_pos + w_trimmed.length()]) || ispunct(lower_text[current_pos + w_trimmed.length()]))) {
      highlight = false;
    }
    if (highlight) {
      (this -> text).insert(current_pos + offset, "<span style=\"color:red\">");
      offset += left_shift;
      (this -> text).insert(current_pos + w_trimmed.length() + offset, "</span>");
      offset += right_shift;
    }
    current_pos += w_trimmed.size();
  }
}

