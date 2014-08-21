#include <string>
#include <sstream>
#include <ctime>
#include <iostream>
#include "boost/algorithm/string.hpp"
#include "Program.h"


snap::Program::Program(const std::string &program_text) {
  int header_break = program_text.find("========");
  std::string header = program_text.substr(0, header_break);
  read_header(header);
  header_break = program_text.find("\n", header_break);
  this -> text = program_text.substr(header_break + 1,
                                     program_text.length() - header_break);
  boost::algorithm::trim(this -> text);
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




