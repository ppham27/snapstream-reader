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
#include "distance.h"

const std::string prefix = "Data/";
const std::string output_path = "../tmp/";
const std::string suffix = "-Combined.txt";
const int max_input_size = 1000000;

void output_visualization(std::map<std::string, std::map<std::string, std::pair<int, int>>> &results, 
                          int topFilter, 
                          std::string uid, std::string dt) {
  // first turn the results to just program matches
  std::map<std::string, std::map<std::string, int>> program_matches;
  for (auto it = results.begin(); it != results.end(); ++it) {
    program_matches[it->first] = std::map<std::string, int>();
    for (auto jt = (it -> second).begin(); jt != (it -> second).end(); ++jt) {
      program_matches[it->first][jt -> first] = (jt -> second).first;
    }
  }

  std::map<std::string, std::map<std::string, double>> program_matches_double = distance::int_matrix_to_double_matrix(program_matches);
  
  std::map<std::string, std::map<std::string, double>> filtered_program_matches = distance::filter_top(program_matches_double, topFilter);

  std::map<std::string, double> sizes = distance::size_pow(filtered_program_matches, 1.0/3);
  
  filtered_program_matches = distance::correlate_sum(filtered_program_matches);

  std::map<std::string, std::map<std::string, double>> distances = distance::distance_inv(filtered_program_matches, 0.1);
  
  std::string csv;
  if (snap::io::file_exists("dictionary.csv")) {
    std::ifstream dict_file("dictionary.csv");
    csv = distance::size_distance_to_csv(sizes, distances, snap::io::read_dictionary(dict_file));
  } else {
    csv = distance::size_distance_to_csv(sizes, distances);
  }

  // std::cout << "<pre>" << csv << "</pre>" << std::endl;
  std::string visualization_file_path = output_path + dt + "_visualization_" + uid + ".csv";
  std::ofstream visualization_file(visualization_file_path, std::ios::out);
  visualization_file << csv;
  visualization_file.close();
  std::cout << snap::web::create_link(visualization_file_path, "Visualization File") << "<br/>" << std::endl;

  std::string json = snap::web::matrix_to_json(csv);
  std::string json_file_name = dt + "_visualization_" + uid + ".json";
  std::string visualization_json_path = output_path + json_file_name;
  std::ofstream visualization_json(visualization_json_path, std::ios::out);
  visualization_json << json;
  visualization_json.close(); 
  std::string title = dt + "%20" + std::to_string(topFilter) + "%20" + snap::web::encode_uri((sizes.begin() -> first).substr(0, 25));
  std::cout << "<br>"
            << "<a id=\"visualization-link\" href=\"../visualize.html?filename=tmp%2F" + json_file_name + "&title=" + title + "#spring-embed\">" 
            << "Visualization"
            << "</a><br>"
            << std::endl;
}

void remove_zero_keys(std::map<std::string, std::map<std::string, std::pair<int, int>>> &results) {
  std::vector<std::string> zero_keys;
  for (auto it0 = results.begin(); it0 != results.end(); ++it0) {
    bool all_zero = true;
    for (auto it1 = results.begin(); it1 != results.end(); ++it1) {
      std::string key0 = it0 -> first;
      std::string key1 = it1 -> first;
      if (key0 <= key1 && results[key0][key1].first != 0) {
        all_zero = false;
        break;
      } else if(results[key1][key0].first != 0) {
        all_zero = false;
        break;
      }
    }
    if (all_zero) {
      zero_keys.push_back(it0 -> first);
    }
  }
  for (std::string zero_key : zero_keys) {
    for (auto it = results.rbegin(); (it -> first) > zero_key; ++it) {
      (it -> second).erase(zero_key);
    }
    results.erase(zero_key);
  }
}

void output_files(std::map<std::string, std::map<std::string, std::pair<int, int>>> &results, 
                  std::string uid, std::string prefix, std::string description) {
  boost::algorithm::trim(description);
  std::string file_description = "";
  if (description != "") {
    file_description = description + "_"; 
    boost::replace_all(file_description, " ", "_");
  }
  std::string outputMatrixFilePath = output_path + prefix + "_matrix_" + file_description + uid + ".csv";
  std::ofstream outputMatrixFile(outputMatrixFilePath, std::ios::out);
  snap::web::print_matrix(results, 
                          [](std::pair<int, int> x) -> int { return x.first; },
                          outputMatrixFile, false, ',');
  outputMatrixFile.close();
  std::string outputMatrixWithHeadersFilePath = output_path + prefix + "_matrix_with_headers_" + file_description + uid + ".csv";
  std::ofstream outputMatrixWithHeadersFile(outputMatrixWithHeadersFilePath, std::ios::out);
  snap::web::print_matrix(results, 
                          [](std::pair<int, int> x) -> int { return x.first; },
                          outputMatrixWithHeadersFile, true, ',');
  outputMatrixWithHeadersFile.close();
  std::string outputKeyFilePath = output_path + prefix + "_keys_" + file_description + uid + ".csv";
  std::ofstream outputKeyFile(outputKeyFilePath, std::ios::out);
  for (auto it = results.begin(); it != results.end(); ++it) outputKeyFile << it -> first << '\n';
  outputKeyFile.close();

  std::cout << snap::web::create_link(outputMatrixFilePath, "Matching programs matrix (numbers only) " + description) << "<br/>" << std::endl;
  std::cout << snap::web::create_link(outputMatrixWithHeadersFilePath, "Matching programs matrix (with headers) " + description, file_description + "matrix") << "<br/>" << std::endl;
  std::cout << snap::web::create_link(outputKeyFilePath, "Matrix row and column names " + description) << "<br/>" << std::endl;  
}

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
  int topFilter = stoi(arguments["top-filter"]);
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
  arguments["search-strings"] = snap::web::decode_uri(arguments["search-strings"]);
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
  std::cout << "From (inclusive): <span id=\"from-date\">" << arguments["from-date"] << "</span><br/>" << std::endl;
  std::cout << "To (inclusive): <span id=\"to-date\">" << arguments["to-date"] << "</span><br/>" << std::endl;
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
  std::vector<std::string> missing_files;
  std::vector<std::string> corrupt_files;
  for (auto it = file_list.begin(); it != file_list.end(); ++it) {
    boost::gregorian::date::ymd_type ymd = current_date.year_month_day();
    std::cout << "<span>Processing " << ymd.month << " "
              << ymd.day << ", " << ymd.year
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
        std::map<std::string, std::vector<int>> match_positions = snap::evaluate_expressions(expressions, raw_match_positions);
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
    } else {
      missing_files.push_back(*it);
    }
    current_date += boost::gregorian::date_duration(1);
  }
  std::cout << "<div>";
  std::cout << "<br/>" << std::endl;
  snap::web::print_missing_files(missing_files);
  std::cout << "<br/>" << std::endl;
  snap::web::print_corrupt_files(corrupt_files);
  std::cout << "</div>" << std::endl;

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

  std::cout << "\nMatching Programs\n" << std::endl;
  snap::web::print_matrix(results, 
                          [](std::pair<int, int> x) -> int { return x.first; },
                          std::cout, true, '\t');

  std::cout << "\nTotal Matches\n" << std::endl;
  snap::web::print_matrix(results, 
                          [](std::pair<int, int> x) -> int { return x.second; },
                          std::cout, true, '\t');
  std::cout << "</pre>" << std::endl;


  std::cout << "<h3>Output Files</h3>" << std::endl;
  srand(time(NULL));
  std::string random_id = std::to_string(rand());

  output_files(results, random_id, snap::date::date_to_string(from_date), "");
  remove_zero_keys(results);
  output_files(results, random_id, snap::date::date_to_string(from_date), "no zeroes");

  // output for visualization
  output_visualization(results, topFilter, random_id, snap::date::date_to_string(from_date));
 
  double duration = (std::clock() - start_time) / (double) CLOCKS_PER_SEC;
  std::cout << "<br/><span>Time taken (seconds): " << duration << "</span><br/>" << std::endl;
  snap::web::close_html();
  return 0;
}
