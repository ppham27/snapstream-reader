#include <algorithm>
#include <cctype>
#include <chrono> 
#include <cstdlib>
#include <cstdio>
#include <exception>
#include <iostream>
#include <fstream>
#include <random> 
#include <string>
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
const int A = 900015709;
const int M = 1000000007;
const int HASH_WIDTH = 50;

void print_column_headers() {
  std::cout << "mt_cxt = matching_contexts_cnt" << '\n';
  std::cout << "mt_prg = matching_programs_cnt" << '\n';
  std::cout << "tot_mt = total_matches_cnt" << '\n';
  std::cout << "sel_prg = selected_programs_cnt" << '\n';
  std::cout << "tot_prg = total_programs_cnt" << '\n';
  std::cout << "dt        \tmt_cxt\tmt_prg\ttot_mt\tsel_prg\ttot_prg" << std::endl;
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
  std::string search_string = snap::web::decode_uri(boost::algorithm::trim_copy(arguments["search-string"]));
  boost::gregorian::date from_date, to_date;
  try {
    from_date = snap::date::string_to_date(arguments["from-date"]);
    to_date = snap::date::string_to_date(arguments["to-date"]);
  } catch (snap::date::InvalidDateException &e) {
    std::cout << "<span class=\"error\">" << e.what() << "</span>" << std::endl;
    exit(-1);
  }
  int num_excerpts = stoi(arguments["num-excerpts"]);
  int excerpt_size = stoi(arguments["excerpt-size"]);
  bool random = arguments["random"] == "on";
  
  std::vector<std::string> file_list = snap::io::generate_file_names(from_date, to_date, prefix, suffix);
  if (random) {
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(file_list.begin(), file_list.end(), std::default_random_engine(seed));
  }
  std::vector<snap::Expression> expressions;
  try {
    expressions.emplace_back(search_string);
  } catch(snap::ExpressionSyntaxError &e) {
    const char *error_msg = e.what();
    std::cout << "<span class=\"error\">" << error_msg << "</span>" << std::endl;
    delete[] error_msg;
    exit(-1);    
  }

  // display user input
  std::cout << "<p>" << std::endl;    
  std::cout << "Search string: " << search_string << "<br/>" << std::endl;
  std::cout << "From (inclusive): " << arguments["from-date"] << "<br/>" << std::endl;
  std::cout << "To (inclusive): " << arguments["to-date"] << "<br/>" << std::endl;
  std::cout << "Number of Excerpts: " << arguments["num-excerpts"] << "<br/>" << std::endl;
  std::cout << "Excerpt Size: " << arguments["excerpt-size"] << "<br/>" << std::endl;
  std::cout << "</p>" << std::endl;  

  // begin to iteratively process files
  int matching_contexts_sum = 0;
  int matching_programs_sum = 0;
  int total_matches_sum = 0;
  int selected_programs_sum = 0;
  int total_programs_sum = 0;
  std::vector<std::vector<std::string>> search_results;
  std::vector<std::string> corrupt_files;
  std::vector<std::string> missing_files;
  std::vector<snap::Excerpt> excerpts;
  if (!random) {
    std::cout << "<pre>" << std::endl;
    print_column_headers();
  }
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
      int matching_contexts = 0; // context is distinct hash AND distinct program
      int matching_programs = 0;
      int total_matches = 0;      
      if (random) {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(programs.begin(), programs.end(), std::default_random_engine(seed));
      }
      std::unordered_map<int, int> match_hashes;
      for (auto p = programs.begin(); p != programs.end(); ++p) {
        std::map<std::string, std::vector<int>> raw_match_positions = snap::find(expressions.back().patterns, p -> lower_text);
        std::map<std::string, std::vector<int>> match_positions = snap::evaluate_expressions(expressions, raw_match_positions);
        snap::StringHasher hasher(p -> text, M, A);
        if (match_positions[search_string].size() > 0) {          
          ++matching_programs;
          total_matches += match_positions[search_string].size();
          bool context_added = false;
          for (auto it = match_positions[search_string].begin(); it != match_positions[search_string].end(); ++it) {
            int match_hash = hasher.hash(*it - HASH_WIDTH, *it + HASH_WIDTH);
            int hash_cnt = match_hashes[match_hash]++;
            if (hash_cnt == 0) {               
              // only add if new hash is encountered
              if (!context_added) { // only add once per program 
                context_added = true;
                ++matching_contexts;
              }              
              // only add excerpts with new hashes
              excerpts.emplace_back(*p, *it-excerpt_size, *it+excerpt_size);
              for (auto pattern = expressions.back().patterns.begin(); pattern != expressions.back().patterns.end(); ++pattern) {
                excerpts.back().highlight_word(*pattern);
              }
              if (random && excerpts.size() <= num_excerpts) {
                snap::web::print_excerpt(excerpts.back());
              }
            }
          }
        }
        match_positions.clear();
      }
      matching_contexts_sum += matching_contexts;
      matching_programs_sum += matching_programs;
      total_matches_sum += total_matches;
      selected_programs_sum += programs.size();
      total_programs_sum += programs.size();
      search_results.push_back(std::vector<std::string>());
      search_results.back().push_back(snap::date::date_to_string(current_date));
      search_results.back().push_back(std::to_string(matching_contexts));
      search_results.back().push_back(std::to_string(matching_programs));
      search_results.back().push_back(std::to_string(total_matches));      
      search_results.back().push_back(std::to_string(programs.size()));
      search_results.back().push_back(std::to_string(programs.size()));
      if (!random) {
        std::copy(search_results.back().begin(), search_results.back().end() - 1, std::ostream_iterator<std::string>(std::cout, "\t"));
        std::cout << search_results.back().back() << std::endl;
      }
      programs.clear();
    } else {
      missing_files.push_back(*it);
    }
  }  
  sort(search_results.begin(), search_results.end(),
       [](std::vector<std::string> a, std::vector<std::string> b) -> bool { return a.front() < b.front(); });
  if (random) {
    std::cout << "<pre>" << std::endl;
    print_column_headers();
    for (auto it = search_results.begin(); it != search_results.end(); ++it) {
      std::copy(it -> begin(), it -> end() - 1, std::ostream_iterator<std::string>(std::cout, "\t"));
      std::cout << it -> back() << std::endl;
    }
  }
  std::cout << "Grand Total:";
  std::cout << '\t' << matching_contexts_sum;
  std::cout << '\t' << matching_programs_sum;
  std::cout << '\t' << total_matches_sum;
  std::cout << '\t' << selected_programs_sum;
  std::cout << '\t' << total_programs_sum << std::endl;
  std::cout << "</pre>" << std::endl;  

  // output file
  srand(time(NULL));
  std::string random_id = std::to_string(rand());
  std::string output_file_name = output_path + search_results.front().front() + "_" + random_id + ".csv";
  std::ofstream output_file(output_file_name);
  output_file << "dt,matching_contexts_cnt,matching_programs_cnt,total_matches_cnt,selected_programs_cnt,total_programs_cnt" << std::endl;
  for (auto it = search_results.begin(); it != search_results.end(); ++it) {
    std::copy(it -> begin(), it -> end() - 1, std::ostream_iterator<std::string>(output_file, ","));
    output_file << it -> back() << '\n';
  }
  output_file.close();
  
  std::cout << "<a href=\"" + output_file_name + "\">Output CSV</a><br/>" << std::endl;
  
  std::cout << "<div>";
  std::cout << "<br/>" << std::endl;
  snap::web::print_missing_files(missing_files);
  std::cout << "<br/>" << std::endl;
  snap::web::print_corrupt_files(corrupt_files);
  std::cout << "</div>" << std::endl;

  if (!random) snap::web::print_excerpts(excerpts, num_excerpts, true);

  double duration = (std::clock() - start_time) / (double) CLOCKS_PER_SEC;
  std::cout << "<br/><span>Time taken (seconds): " << duration << "</span><br/>" << std::endl;
  
  snap::web::close_html();
  return 0;
}
