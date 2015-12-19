#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>


#include "boost/algorithm/string.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

#include "snap.h"
#include "StringHasher.h"

const std::string prefix = "Data/";
const std::string output_path = "../tmp/";
const std::string suffix = "-Combined.txt";
const int max_input_size = 1000000;

// hashing parameters
const int A = 3;
const int M = 65071;
const int LEFT_HASH_WIDTH = 15;
const int RIGHT_HASH_WIDTH = 25;

void output_matrix_file(const std::vector<std::vector<std::string>> &search_results, 
                 const std::vector<std::string> &search_strings,
                 std::string id, std::string name) {
  std::string output_file_path = output_path + search_results.front().front() + "_" + name + "_" + id + ".csv";
  std::ofstream output_file(output_file_path);
  output_file << "dt,";
  std::copy(search_strings.begin(), search_strings.end(), std::ostream_iterator<std::string>(output_file, ","));
  output_file << "selected_programs_cnt";
  for (std::vector<std::string> daily_result : search_results) {
    output_file << '\n';
    std::copy(daily_result.begin(), daily_result.end() - 1, std::ostream_iterator<std::string>(output_file, ","));
    output_file << daily_result.back();
  }
  output_file.close();
  std::cout << "<p>";
  std::cout << snap::web::create_link(output_file_path, "Output " + name + " File", name + "_matrix");
  std::cout << "</p>" << std::endl;
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

  // process user input
  std::map<std::string, std::string> arguments = snap::web::parse_query_string(query_string);
  int num_excerpts = stoi(arguments["num-excerpts"]);
  int excerpt_size = stoi(arguments["excerpt-size"]);
  // dates
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
  std::cout << "From (inclusive): <span id=\"from-date\">" << arguments["from-date"] << "</span><br/>" << std::endl;
  std::cout << "To (inclusive): <span id=\"to-date\">" << arguments["to-date"] << "</span><br/>" << std::endl;
  std::cout << "Number of Excerpts: " << arguments["num-excerpts"] << "<br/>" << std::endl;
  std::cout << "Excerpt Size: " << arguments["excerpt-size"] << "<br/>" << std::endl;
  std::cout << "</p>" << std::endl;

  // variables to store results of loop
  std::vector<std::vector<std::string>> search_results;
  std::vector<std::vector<std::string>> search_results_programs;
  std::vector<std::vector<std::string>> search_results_total_matches;
  int total_programs_cnt = 0;
  int selected_programs_cnt = 0;
  std::vector<std::string> corrupt_files;
  std::vector<std::string> missing_files;
  std::vector<snap::Excerpt> excerpts;

  // print table header
  std::cout << "<table><thead><tr><th>dt</th>";
  for (auto it = search_strings.begin(); it != search_strings.end(); ++it) {
    std::cout << "<th>" << (*it) + " Contexts" << "</th>";
  }
  std::cout << "<th>selected_programs_cnt</th></tr></thead><tbody>" << std::endl;
  snap::StringHasher hasher("", M, A);
  std::unordered_map<std::string, std::unordered_map<int, int>> total_left_word_hashes;
  std::unordered_map<std::string, std::unordered_map<int, int>> total_right_word_hashes;
  std::map<std::string, std::tuple<int, int, int>> match_counts;
  for (auto it = file_list.begin();
       it != file_list.end();
       ++it) {
    boost::gregorian::date current_date = snap::date::string_to_date((*it).substr(prefix.length(), 10));
    if (snap::io::file_exists(*it)) {
      std::vector<snap::Program> programs;
      try {
        programs = snap::io::parse_programs(*it);
      } catch (snap::io::CorruptFileException &e) {
        programs.clear();
        corrupt_files.push_back(*it);
        continue;
      }
      search_results.push_back(std::vector<std::string>{snap::date::date_to_string(current_date)});
      search_results_programs.push_back(std::vector<std::string>{snap::date::date_to_string(current_date)});
      search_results_total_matches.push_back(std::vector<std::string>{snap::date::date_to_string(current_date)});
      std::unordered_map<std::string, std::unordered_map<int, int>> daily_left_word_hashes;
      std::unordered_map<std::string, std::unordered_map<int, int>> daily_right_word_hashes;
      std::cout << "<tr><td>" << snap::date::date_to_string(current_date) << "</td>";      
      total_programs_cnt += programs.size();
      int daily_selected_programs_cnt = 0;
      std::map<std::string, std::tuple<int, int, int>> daily_match_counts;
      for (auto p = programs.begin(); p != programs.end(); ++p) {
        ++selected_programs_cnt;
        ++daily_selected_programs_cnt;
        hasher.load_text(p -> lower_text);
        std::map<std::string, std::vector<int>> raw_match_positions = snap::find(patterns, p -> lower_text);
        std::map<std::string, std::vector<int>> match_positions = snap::evaluate_expressions(expressions, raw_match_positions);
        for (auto ss = search_strings.begin(); ss != search_strings.end(); ++ss) {
          if (match_positions[*ss].size() > 0) {
            bool total_context_added = false;
            bool context_added = false;
            ++std::get<1>(daily_match_counts[*ss]);
            ++std::get<1>(match_counts[*ss]);
            std::get<2>(daily_match_counts[*ss]) += match_positions[*ss].size();
            std::get<2>(match_counts[*ss]) += match_positions[*ss].size();            
            for (auto it = match_positions[*ss].begin(); it != match_positions[*ss].end(); ++it) {
              int left_word_hash = hasher.hash(*it - LEFT_HASH_WIDTH, *it);
              int right_word_hash = hasher.hash(*it, *it + RIGHT_HASH_WIDTH);
              int daily_left_hash_cnt = daily_left_word_hashes[*ss][left_word_hash]++;
              int daily_right_hash_cnt = daily_right_word_hashes[*ss][right_word_hash]++;
              int total_left_hash_cnt = total_left_word_hashes[*ss][left_word_hash]++;
              int total_right_hash_cnt = total_right_word_hashes[*ss][right_word_hash]++;
              if (daily_left_hash_cnt == 0 && daily_right_hash_cnt == 0) {
                if (!context_added) {
                  ++std::get<0>(daily_match_counts[*ss]);
                  context_added = true;
                }
                if (total_left_hash_cnt == 0 && total_right_hash_cnt == 0) {
                  if (!total_context_added) {
                    ++std::get<0>(match_counts[*ss]);
                    total_context_added = true;
                  }
                  excerpts.emplace_back(*p, *it - excerpt_size, *it + excerpt_size);
                  std::vector<std::string> search_string_patterns = expressions[ss - search_strings.begin()].patterns;
                  for (auto pattern = search_string_patterns.begin(); pattern != search_string_patterns.end(); ++pattern) {
                    excerpts.back().highlight_word(*pattern);
                  }
                }
              } 
            }
          }
        }
      }
      for (auto ss = search_strings.begin(); ss != search_strings.end(); ++ss) {
        search_results.back().push_back(std::to_string(std::get<0>(daily_match_counts[*ss])));
        search_results_programs.back().push_back(std::to_string(std::get<1>(daily_match_counts[*ss])));
        search_results_total_matches.back().push_back(std::to_string(std::get<2>(daily_match_counts[*ss])));
        std::cout << "<td>" << std::get<0>(daily_match_counts[*ss]) << "</td>";
      }
      search_results.back().push_back(std::to_string(daily_selected_programs_cnt));
      search_results_programs.back().push_back(std::to_string(daily_selected_programs_cnt));
      search_results_total_matches.back().push_back(std::to_string(daily_selected_programs_cnt));
      std::cout << "<td>" << daily_selected_programs_cnt << "</td>";
      std::cout << "</tr>" << std::endl;      
      programs.clear();
    } else {
      missing_files.push_back(*it);
    }
  }
  // print out total line
  std::cout << "<tr>" << std::endl;
  search_results.emplace_back();
  search_results_programs.emplace_back();
  search_results_total_matches.emplace_back();
  std::cout << "<td><strong>Grand Total:</strong></td>" << std::endl;
  search_results.back().push_back("Grand Total:");
  search_results_programs.back().push_back("Grand Total:");
  search_results_total_matches.back().push_back("Grand Total:");
  for (std::string ss : search_strings) {
    std::cout << "<td>" << std::get<0>(match_counts[ss]) << "</td>" << std::endl;
    search_results.back().push_back(std::to_string(std::get<0>(match_counts[ss])));
    search_results_programs.back().push_back(std::to_string(std::get<1>(match_counts[ss])));
    search_results_total_matches.back().push_back(std::to_string(std::get<2>(match_counts[ss])));
  }
  std::cout << "<td>" << total_programs_cnt << "</td>" << std::endl;
  search_results.back().push_back(std::to_string(total_programs_cnt));
  std::cout << "</tr>" << std::endl;

  std::cout << "</tbody></table>" << std::endl;
  std::cout << "<div>";
  std::cout << "<br/>" << std::endl;
  snap::web::print_missing_files(missing_files);
  std::cout << "<br/>" << std::endl;
  snap::web::print_corrupt_files(corrupt_files);
  std::cout << "</div>" << std::endl;

  snap::web::print_excerpts(excerpts, num_excerpts, true);

  // output file
  srand(time(NULL));
  std::string random_id = std::to_string(rand());  
  output_matrix_file(search_results, search_strings, random_id, "contexts");
  output_matrix_file(search_results_programs, search_strings, random_id, "programs");
  output_matrix_file(search_results_total_matches, search_strings, random_id, "total_matches"); 
  // all data in long form
  std::string output_file_name = search_results.front().front() + "_all_" + random_id + ".csv";
  std::string output_file_path = output_path + output_file_name;
  std::ofstream output_file(output_file_path);
  output_file << "Date,Term,Contexts,Programs,Total Matches";
  for (int i = 0; i < search_results.size() - 1; ++i) { // skip total line
    for (int j = 0; j < search_strings.size(); ++j) {
      output_file << '\n';
      output_file << search_results[i].front() << ',' << search_strings[j] << ','
                  << search_results[i][j + 1] << ',' // j + 1 skips date column
                  << search_results_programs[i][j + 1] << ','
                  << search_results_total_matches[i][j + 1];
    }
  }
  output_file.close();
  std::cout << "<p>";
  std::cout << snap::web::create_link(output_file_path, "Output Long File", "long-data");
  std::cout << "</p>" << std::endl;
  
  double duration = (std::clock() - start_time) / (double) CLOCKS_PER_SEC;
  std::cout << "<br/><span>Time taken (seconds): " << duration << "</span><br/>" << std::endl;
  snap::web::close_html();  
  return 0;
}
