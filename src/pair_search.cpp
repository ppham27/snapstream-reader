#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <set>
#include <string>

#include "boost/algorithm/string.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

#include "snap.h"

const std::string prefix = "Data/";
const std::string outputPath = "/tmp/";
const std::string suffix = "-Combined.txt";
const int max_input_size = 1000000;

int main() {
  clock_t start_time = std::clock();
  
  snap::web::print_header();

  // get user input
  int content_length = atoi(getenv("CONTENT_LENGTH"));
  char *input = new char[content_length+1];
  fgets(input, content_length+1, stdin);
  std::string query_string(input);
  delete[] input;
  std::map<std::string, std::string> arguments = snap::web::parse_query_string(query_string);

  // turn inputs into C++ types
  int distance = stoi(arguments["distance"]);
  boost::gregorian::date current_date, from_date, to_date;
  try {
    current_date = snap::date::string_to_date(arguments["from-date"]);
    from_date = snap::date::string_to_date(arguments["from-date"]);
    to_date = snap::date::string_to_date(arguments["to-date"]);
  } catch (snap::date::InvalidDateException &e) {
    std::cout << "<span class=\"error\">" << e.what() << "</span>" << std::endl;
    exit(-1);
  }
  std::vector<std::string> file_list = snap::io::generate_file_names(from_date, to_date, prefix, suffix);
  
  // process search strings
  std::vector<std::string> search_strings;
  arguments["search-strings"] = snap::web::sanitize_string(arguments["search-strings"]);
  boost::split(search_strings, arguments["search-strings"], boost::is_any_of("\n"));
  // remove empty strings  
  auto search_string_iterator = search_strings.begin();
  while (search_string_iterator != search_strings.end()) {
    if (std::all_of(search_string_iterator -> begin() , search_string_iterator -> end(), ::isspace)) {
      search_string_iterator = search_strings.erase(search_string_iterator);
    } else {
      boost::algorithm::trim(*search_string_iterator);
      ++search_string_iterator;
    }
  }
  if (search_strings.size() == 0) {
    std::cout << "<span class=\"error\">" << "Error: There are no search strings." << "</span>" << std::endl;
    exit(-1);
  }
  std::sort(search_strings.begin(), search_strings.end());
  
  std::vector<snap::Expression> expressions;
  std::set<std::string> pattern_set;
  for (auto it = search_strings.begin(); it != search_strings.end(); ++it) {
    try {
      expressions.emplace_back(*it);
    } catch(snap::ExpressionSyntaxError &e) {
      const char *error_msg = e.what();
      std::cout << "<span class=\"error\">" << error_msg << "</span>" << std::endl;
      delete[] error_msg;
      exit(-1);
    }
    pattern_set.insert(expressions.back().patterns.begin(), expressions.back().patterns.end());
  }
  std::vector<std::string> patterns;
  patterns.insert(patterns.end(), pattern_set.begin(), pattern_set.end());
  
  // print output for user to verify
  std::cout << "<p>" << std::endl;
  std::cout << "Search strings:" << "<br/>" << std::endl;
  for (auto it = search_strings.begin(); it != search_strings.end(); ++it) {    
    std::cout << *it << "<br/>" << std::endl;
  }
  std::cout << "Distance: " << arguments["distance"] << "<br/>" << std::endl;
  std::cout << "From (inclusive): " << arguments["from-date"] << "<br/>" << std::endl;
  std::cout << "To (exclusive): " << arguments["to-date"] << "<br/>" << std::endl;
  std::cout << "</p>" << std::endl;

  // initiate result matrix
  std::map<std::string, std::map<std::string, std::pair<int, int>>> results;
  for (auto it0 = expressions.begin(); it0 != expressions.end(); ++it0) {
    results[it0 -> raw_expression] = std::map<std::string, std::pair<int, int>>();
    for (auto it1(it0); it1 != expressions.end(); ++it1) {
      results[it0 -> raw_expression][it1 -> raw_expression] = std::make_pair(0, 0);
    }
  }
  int total_programs_cnt = 0;
  int selected_programs_cnt = 0;

  // run through dates
  std::vector<std::string> corrupt_files;
  for (auto it = file_list.begin(); it != file_list.end(); ++it) {
    boost::gregorian::date::ymd_type ymd = current_date.year_month_day();
    std::cout << "<span>Processing " << ymd.month << " "
              << ymd.day << "," << ymd.year
              << "...</span></br>" << std::endl;
    if (snap::io::file_exists(*it)) {
      std::vector<snap::Program> programs;
      try {
        programs = snap::io::parse_programs(*it);
      } catch (snap::io::CorruptFileException &e) {
        programs.clear();
        current_date += boost::gregorian::date_duration(1);
        corrupt_files.push_back(*it);
        continue;
      }
      total_programs_cnt += programs.size();
      for (auto p = programs.begin(); p != programs.end(); ++p) {
        ++selected_programs_cnt;
        std::map<std::string, std::vector<int>> raw_match_positions = snap::find(patterns, p -> lower_text);
        std::map<std::string, std::vector<int>> match_positions = evaluate_expressions(expressions, raw_match_positions);
        std::map<std::string, std::map<std::string, int>> cooccurences = snap::pair(match_positions, distance);
        for (auto it0 = cooccurences.begin(); it0 != cooccurences.end(); ++it0) {
          for (auto it1 = (it0 -> second).begin(); it1 != (it0 -> second).end(); ++it1) {
            if (it1 -> second > 0) {
              ++results[it0 -> first][it1 -> first].first;
              results[it0 -> first][it1 -> first].second += it1 -> second;
            }
          }
        }
      }
    }
    current_date += boost::gregorian::date_duration(1);
  }
  snap::web::print_corrupt_files(corrupt_files);

  // print results
  std::cout << "<pre>" << std::endl;
  std::cout << "Selected Programs Count: " << selected_programs_cnt << std::endl;
  std::cout << "Total Programs Count: " << total_programs_cnt << std::endl;
  std::cout << "search_string_1\tsearch_string_2\tmatching_programs_cnt\ttotal_matches_cnt" << std::endl;
  for (auto it0 = results.begin(); it0 != results.end(); ++it0) {
    for (auto it1 = (it0 -> second).begin(); it1 != (it0 -> second).end(); ++it1) {
      std::cout << it0 -> first << '\t' << it1 -> first << '\t' 
                << (it1 -> second).first << '\t' << (it1 -> second).second << std::endl;      
    }
  }

  std::cout << std::endl << "Matching Programs" << std::endl;
  snap::web::print_matrix(results, 
                          [](std::pair<int, int> x) -> int { return x.first; },
                          std::cout, true, '\t');

  std::cout << std::endl << "Total Matches" << std::endl;
  snap::web::print_matrix(results, 
                          [](std::pair<int, int> x) -> int { return x.second; },
                          std::cout, true, '\t');
  std::cout << "</pre>" << std::endl;

  srand(time(NULL));
  std::string randomId = std::to_string(rand());
  std::string outputMatrixFilePath = outputPath + snap::date::date_to_string(from_date) + "_matrix_" + randomId + ".csv";
  std::ofstream outputMatrixFile(".." + outputMatrixFilePath, std::ios::out);
  snap::web::print_matrix(results, 
                          [](std::pair<int, int> x) -> int { return x.first; },
                          outputMatrixFile, false, ',');
  outputMatrixFile.close();
  std::string outputKeyFilePath = outputPath + snap::date::date_to_string(from_date) + "_keys_" + randomId + ".csv";
  std::ofstream outputKeyFile(".." + outputKeyFilePath, std::ios::out);
  for (auto it = results.begin(); it != results.end(); ++it) outputKeyFile << it -> first << '\n';
  outputKeyFile.close();

  std::cout << snap::web::create_link(outputMatrixFilePath, "Matching programs matrix") << "<br/>" << std::endl;
  std::cout << snap::web::create_link(outputKeyFilePath, "Matrix row and column names") << "<br/>" << std::endl;
  double duration = (std::clock() - start_time) / (double) CLOCKS_PER_SEC;
  std::cout << "<br/><span>Time taken (seconds): " << duration << "</span><br/>" << std::endl;
  snap::web::close_html();
  return 0;
}
