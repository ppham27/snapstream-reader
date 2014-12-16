#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstdio>
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

  // display user input
  std::cout << "<p>" << std::endl;  
  std::map<std::string, std::string> arguments = snap::web::parse_query_string(query_string);
  std::cout << "Search string 1: " << arguments["search-string-1"] << "<br/>" << std::endl;
  std::cout << "Search string 2: " << arguments["search-string-2"] << "<br/>" << std::endl;
  std::cout << "Distance: " << arguments["distance"] << "<br/>" << std::endl;
  std::cout << "From (inclusive): " << arguments["from-date"] << "<br/>" << std::endl;
  std::cout << "To (exclusive): " << arguments["to-date"] << "<br/>" << std::endl;
  std::cout << "Number of Excerpts: " << arguments["num-excerpts"] << "<br/>" << std::endl;
  std::cout << "</p>" << std::endl;

  // process user input
  std::string search_string01 = boost::algorithm::trim_copy(arguments["search-string-1"]);
  std::string search_string02 = boost::algorithm::trim_copy(arguments["search-string-2"]);
  int distance = stoi(arguments["distance"]);
  boost::gregorian::date current_date = snap::date::string_to_date(arguments["from-date"]);
  boost::gregorian::date from_date = snap::date::string_to_date(arguments["from-date"]);
  boost::gregorian::date to_date = snap::date::string_to_date(arguments["to-date"]);
  int num_excerpts = stoi(arguments["num-excerpts"]);
  std::vector<std::string> file_list = snap::io::generate_file_names(from_date, to_date, prefix, suffix);

    // begin to iteratively process files
  std::vector<snap::Excerpt> excerpts;
  std::cout << "<pre>" << std::endl;
  std::cout << "dt\tmatching_programs_cnt\ttotal_matches_cnt1\ttotal_matches_cnt2\tselected_programs_cnt\ttotal_programs_cnt" << std::endl;
  for (auto it = file_list.begin();
       it != file_list.end();
       ++it) {
    if (snap::io::file_exists(*it)) {      
      std::vector<snap::Program> programs = snap::io::parse_programs(*it);
      std::cout << snap::date::date_to_string(current_date);
      int matching_programs = 0;
      int total_matches01 = 0; int total_matches02 = 0;
      for (auto p = programs.begin(); p != programs.end(); ++p) {
        std::map<std::string, std::vector<int>> match_positions;
        if (std::any_of(search_string01.begin(), search_string01.end(), ::isupper) ||
            std::any_of(search_string02.begin(), search_string02.end(), ::isupper)) {
          match_positions = snap::near(search_string01, search_string02,
                                       distance, p -> text);
        } else {
          match_positions = snap::near(search_string01, search_string02,
                                       distance, p -> lower_text);
        }
        if (match_positions[search_string01].size() > 0 || match_positions[search_string02].size() > 0) {
          ++matching_programs;
          total_matches01 += match_positions[search_string01].size();
          total_matches02 += match_positions[search_string02].size();
          for (auto it = match_positions[search_string01].begin(); it != match_positions[search_string01].end(); ++it) {
            excerpts.emplace_back(*p, *it-distance-excerpt_size, *it+distance+excerpt_size);
            excerpts.back().highlight_word(search_string01); excerpts.back().highlight_word(search_string02);
          }
        }
        match_positions.clear();
      }
      std::cout << '\t' << matching_programs;
      std::cout << '\t' << total_matches01;
      std::cout << '\t' << total_matches02;
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
