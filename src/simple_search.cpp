#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>
#include <random> 
#include <chrono> 
#include <sys/stat.h>

#include "boost/algorithm/string.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

#include "snap.h"

const std::string prefix = "Data/";
const std::string suffix = "-Combined.txt";

bool file_exists(const std::string &file_name) {
  struct stat buffer;
  return stat(file_name.c_str(), &buffer) == 0;
}

int main() {
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::cout << "Content-type: text/html; charset=iso-8859-1\n" << std::endl;
  std::cout << "<html>\n<head>\n<title>Snapstream Searcher</title>\n<meta charset=\"utf-8\">\n<link rel=\"stylesheet\" type=\"text/css\" href=\"/style.css\">\n</head>\n<body>" << std::endl;
      
  std::string query_string(getenv("QUERY_STRING"));
  std::cout << "<p>" << std::endl;  
  std::map<std::string, std::string> arguments = snap::parse_query_string(query_string);
  std::cout << "Search string: " << arguments["search-string"] << "<br/>" << std::endl;
  std::cout << "From (inclusive): " << arguments["from-date"] << "<br/>" << std::endl;
  std::cout << "To (exclusive): " << arguments["to-date"] << "<br/>" << std::endl;
  std::cout << "</p>" << std::endl;  
 
  std::string search_string = boost::algorithm::trim_copy(arguments["search-string"]);
  boost::gregorian::date current_date = snap::string_to_date(arguments["from-date"]);
  boost::gregorian::date from_date = snap::string_to_date(arguments["from-date"]);
  boost::gregorian::date to_date = snap::string_to_date(arguments["to-date"]);
  std::vector<std::string> file_list = snap::generate_file_names(from_date, to_date, prefix, suffix);
  std::vector<snap::Excerpt> excerpts;
  std::cout << "<pre>" << std::endl;
  std::cout << "dt\tmatching_programs_cnt\ttotal_matches_cnt\tselected_programs_cnt\ttotal_programs_cnt" << std::endl;
  for (auto it = file_list.begin();
       it != file_list.end();
       ++it) {
    if (file_exists(*it)) {      
      std::vector<snap::Program> programs = snap::parse_programs(*it);
      std::cout << snap::date_to_string(current_date);
      int matching_programs = 0;
      int total_matches = 0;
      for (auto p = programs.begin();
           p != programs.end();
           ++p) {
        std::map<std::string, std::vector<int>> match_positions;
        if (std::any_of(search_string.begin(), search_string.end(), ::isupper)) {
          match_positions = snap::find(search_string, p -> text);
        } else {
          match_positions = snap::find(search_string, p -> lower_text);
        }
        if (match_positions[search_string].size() > 0) {
          ++matching_programs;
          total_matches += match_positions[search_string].size();
          for (auto it = match_positions[search_string].begin(); it != match_positions[search_string].end(); ++it) {
            excerpts.emplace_back(*p, *it-150, *it+150);
            excerpts.back().highlight_word(search_string);
          }
        }
        match_positions.clear();
      }
      std::cout << '\t' << matching_programs;
      std::cout << '\t' << total_matches;
      std::cout << '\t' << programs.size();
      std::cout << '\t' << programs.size() << std::endl;
      programs.clear();
    }
    current_date += boost::gregorian::date_duration(1);
  }
  std::cout << "</pre>" << std::endl;
  // print excerpts
  std::shuffle(excerpts.begin(), excerpts.end(), std::default_random_engine(seed));
  std::cout << "<div>" << std::endl;
  for (int i = 0; i < std::min((int) excerpts.size(), 100); ++i) {
    std::cout << "<div>" << std::endl;
    std::cout << "<h4>" << "Program title: " << excerpts[i].program_title << "</h4>";
    std::cout << "<span>Date: " << excerpts[i].date << "</span></br>";
    std::cout << "Search strings: ";
    for (auto it = excerpts[i].search_strings.begin();
         it != excerpts[i].search_strings.end();
         ++it) {
      std::cout << *it << ", ";
    }
    std::cout << "<br/>" << excerpts[i].text << "<br/>";    
    std::cout << "</div>" << std::endl;
  }
  std::cout << "</div>" << std::endl;
  std::cout << "</body>\n</html>" << std::endl;  
  return 0;
}
