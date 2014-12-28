#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <exception>
#include <iostream>
#include <string>

#include "boost/algorithm/string.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

#include "snap.h"

const std::string prefix = "Data/";
const std::string suffix = "-Combined.txt";
const int max_input_size = 1000000;
const int excerpt_size = 400;

int main() {
  clock_t start_time = std::clock();
  
  snap::web::print_header();

  // get user input
  int content_length = atoi(getenv("CONTENT_LENGTH"));
  char *input = new char[content_length+1];
  fgets(input, content_length+1, stdin);
  std::string query_string(input);
  delete[] input;

  // process user input
  std::map<std::string, std::string> arguments = snap::web::parse_query_string(query_string);
  std::string search_string = snap::web::sanitize_string(boost::algorithm::trim_copy(arguments["search-string"]));
  boost::gregorian::date current_date, from_date, to_date;
  try {
    current_date = snap::date::string_to_date(arguments["from-date"]);
    std::cout << current_date << std::endl;
    from_date = snap::date::string_to_date(arguments["from-date"]);
    to_date = snap::date::string_to_date(arguments["to-date"]);
  } catch (snap::date::InvalidDateException &e) {
    std::cout << "<span class=\"error\">" << e.what() << "</span>" << std::endl;
    exit(-1);
  }
  int num_excerpts = stoi(arguments["num-excerpts"]);
  std::vector<std::string> file_list = snap::io::generate_file_names(from_date, to_date, prefix, suffix);
  std::vector<snap::Expression> expressions;
  expressions.emplace_back(search_string);

  // display user input
  std::cout << "<p>" << std::endl;    
  std::cout << "Search string: " << search_string << "<br/>" << std::endl;
  std::cout << "From (inclusive): " << arguments["from-date"] << "<br/>" << std::endl;
  std::cout << "To (exclusive): " << arguments["to-date"] << "<br/>" << std::endl;
  std::cout << "Number of Excerpts: " << arguments["num-excerpts"] << "<br/>" << std::endl;
  std::cout << "</p>" << std::endl;  

  // begin to iteratively process files
  std::vector<snap::Excerpt> excerpts;
  std::cout << "<pre>" << std::endl;
  std::cout << "dt\tmatching_programs_cnt\ttotal_matches_cnt\tselected_programs_cnt\ttotal_programs_cnt" << std::endl;
  for (auto it = file_list.begin();
       it != file_list.end();
       ++it) {
    if (snap::io::file_exists(*it)) {      
      std::vector<snap::Program> programs = snap::io::parse_programs(*it);
      std::cout << snap::date::date_to_string(current_date);
      int matching_programs = 0;
      int total_matches = 0;
      for (auto p = programs.begin(); p != programs.end(); ++p) {
        std::map<std::string, std::vector<int>> raw_match_positions = snap::find(expressions.back().patterns, p -> lower_text);
        std::map<std::string, std::vector<int>> match_positions = snap::evaluate_expressions(expressions, raw_match_positions);
        if (match_positions[search_string].size() > 0) {
          ++matching_programs;
          total_matches += match_positions[search_string].size();
          for (auto it = match_positions[search_string].begin(); it != match_positions[search_string].end(); ++it) {
            excerpts.emplace_back(*p, *it-excerpt_size, *it+excerpt_size);
            for (auto pattern = expressions.back().patterns.begin(); pattern != expressions.back().patterns.end(); ++pattern) {
              excerpts.back().highlight_word(*pattern);
            }
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
  snap::web::print_excerpts(excerpts, num_excerpts);

  double duration = (std::clock() - start_time) / (double) CLOCKS_PER_SEC;
  std::cout << "<br/><span>Time taken (seconds): " << duration << "</span><br/>" << std::endl;
  
  snap::web::close_html();
  return 0;
}
