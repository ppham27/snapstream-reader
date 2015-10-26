#include <algorithm>
#include <cctype>
#include <string>
#include <sstream>
#include <ctime>
#include <regex>
#include "boost/algorithm/string.hpp"
#include "Program.h"

#include <iostream>

const std::regex time_regex = std::regex("\\[[0-9]{1,2}:[0-9]{2}:[0-9]{2} (AM|PM)\\]");

snap::Program::Program(const std::string &program_text) {
  int header_break = program_text.find("========");
  std::string header = program_text.substr(0, header_break);
  read_header(header);
  header_break = program_text.find("\n", header_break);
  this -> raw_text = program_text.substr(header_break + 1,
                                         program_text.length() - header_break);
  boost::algorithm::trim(this -> raw_text);
  this -> text = strip_timestamps(this -> raw_text);  
  this -> lower_text = std::string(this -> text);
  std::transform(this -> lower_text.begin(), this -> lower_text.end(),
                 this -> lower_text.begin(), ::tolower);
}

std::string snap::Program::strip_timestamps(const std::string &text) {
  std::string stripped_text; stripped_text.reserve(text.size());
  int current = 0;
  int left = text.find("[", current);
  while (left != -1) {
    int right = text.find("]", current);
    if (right == -1) break;
    int width = right - left;
    if (11 <= width && width <= 12
        && std::regex_match(text.substr(left, width+1), time_regex)) {
      // found a timestamp, they are preceded by 2 new lines and proceeded by 2 new lines
      if (left >= 2) left -= 2;
      stripped_text += text.substr(current, left - current); // add all texts before the timestamp
      if (right < text.size() - 2) right += 2;
    } else {
      // not a timestamp so add everything
      stripped_text += text.substr(current, right - current + 1);
    }
    current = right + 1;
    left = text.find("[", current);
  }
  stripped_text += text.substr(current); // add the rest of the text
  return stripped_text;
}

void snap::Program::read_header(const std::string &header) {
  std::istringstream is(header);
  std::string line;
  while (getline(is, line)) {
    if (line.substr(0,7) == "Title: ") {
      this -> title = line.substr(7, line.length() - 7);
    } else if (line.substr(0, 13) == "Description: ") {
      this -> description = line.substr(13, line.length() - 13);
    } else if (line.substr(0, 9) == "Channel: ") {
      this -> channel = line.substr(9, line.length() - 9);
    } else if (line.substr(0, 13) == "Recorded On: ") {
      this -> recorded_date = read_date(line.substr(13, line.length() - 13));
    } else if (line.substr(0, 19) == "Original Air Date: ") {
      this -> aired_date = read_date(line.substr(19, line.length() - 19));
    }
  }  
}

std::string snap::Program::read_date(const std::string &date) {
  struct tm tm_date;
  strptime(date.c_str(), "%m/%d/%Y", &tm_date);
  char *new_date = new char[11];
  std::strftime(new_date, 11, "%F", &tm_date);
  std::string new_date_str(new_date);
  delete[] new_date;
  return new_date_str;
}

bool snap::Program::operator==(const snap::Program &other) const {
  if (this -> title == other.title &&
      this -> description == other.description &&
      this -> channel == other.channel &&
      this -> aired_date == other.aired_date &&
      this -> recorded_date == other.recorded_date &&
      this -> text == other.text) {
    return true;
  }
  return false;
}

bool snap::Program::operator!=(const snap::Program &other) const {
  return !(*this == other);
}


