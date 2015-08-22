#include <iostream>

#include "snap.h"

const std::string prefix = "Data/";
const std::string output_path = "../tmp/";
const std::string suffix = "-Combined.txt";
const int max_input_size = 1000000;

std::map<std::string, int> get_word_count(const std::vector<std::string> &file_list) {
  std::map<std::string, int> word_count;
  // run through dates
  std::vector<std::string> missing_files;
  std::vector<std::string> corrupt_files;
  for (auto it = file_list.begin(); it != file_list.end(); ++it) {
    boost::gregorian::date current_date = snap::date::string_to_date((*it).substr(prefix.length(), 10));
    if (snap::io::file_exists(*it)) {      
      std::vector<snap::Program> programs;
      try {
        programs = snap::io::parse_programs(*it);
        boost::gregorian::date::ymd_type ymd = current_date.year_month_day();
        std::cout << "<span>Processing " << ymd.month << " "
                  << ymd.day << ", " << ymd.year
                  << "...</span></br>" << std::endl;
        for (snap::Program program : programs) {
          std::map<std::string, int> program_word_count = snap::word::count_words(snap::word::tokenize(program.lower_text));
          for (std::pair<std::string, int> count : program_word_count) word_count[count.first] += count.second;
        }
      } catch (snap::io::CorruptFileException &e) {
        programs.clear();
        corrupt_files.push_back(*it);
        continue;
      }
      programs.clear();
    } else {
      missing_files.push_back(*it);
    }
  }
  std::cout << "<div>";
  std::cout << "<br/>" << std::endl;
  snap::web::print_missing_files(missing_files);
  std::cout << "<br/>" << std::endl;
  snap::web::print_corrupt_files(corrupt_files);
  std::cout << "</div>" << std::endl;
  return word_count;
}

int main(int argc, char *argv[]) {
  clock_t start_time = std::clock();  
  snap::web::print_header();

  int content_length = atoi(getenv("CONTENT_LENGTH"));
  char *input = new char[content_length+1];
  fgets(input, content_length+1, stdin);
  std::string query_string(input);
  delete[] input;
  std::map<std::string, std::string> arguments = snap::web::parse_query_string(query_string);

  // turn inputs into C++ types
  int min_count = stoi(arguments["min-occurences"]);
  double percent_increase = stof(arguments["percent-increase"]);
  boost::gregorian::date from_date_a, to_date_a, from_date_b, to_date_b;
  try {
    from_date_a = snap::date::string_to_date(arguments["from-date-a"]);
    to_date_a = snap::date::string_to_date(arguments["to-date-a"]);
    from_date_b = snap::date::string_to_date(arguments["from-date-b"]);
    to_date_b = snap::date::string_to_date(arguments["to-date-b"]);
  } catch (snap::date::InvalidDateException &e) {
    std::cout << "<span class=\"error\">" << e.what() << "</span>" << std::endl;
    exit(-1);
  }
  std::vector<std::string> file_list_a = snap::io::generate_file_names(from_date_a, to_date_a, prefix, suffix);
  std::vector<std::string> file_list_b = snap::io::generate_file_names(from_date_b, to_date_b, prefix, suffix);
  std::map<std::string, int> word_count_a = get_word_count(file_list_a);
  std::map<std::string, int> word_count_b = get_word_count(file_list_b);
  std::map<std::string, std::pair<int, int>> hot_list = snap::word::compare_word_counts(word_count_a, word_count_b, min_count, percent_increase);

  std::cout << "<table><thead><tr><th>Word</th><th>Occurrences in interval A</th><th>Occurrences in interval B</th>";
  std::cout << "</tr></thead><tbody>" << std::endl;
  for (std::pair<std::string, std::pair<int, int>> hot_word : hot_list) {
    std::cout << "<tr>" << std::endl;
    std::cout << "<td>" << hot_word.first << "</td>" << std::endl;
    std::cout << "<td>" << hot_word.second.first << "</td>" << std::endl;
    std::cout << "<td>" << hot_word.second.second << "</td>" << std::endl;
    std::cout << "</tr>" << std::endl;
  }    
  std::cout << "</tbody></table>" << std::endl;
    
  double duration = (std::clock() - start_time) / (double) CLOCKS_PER_SEC;
  std::cout << "<br/><span>Time taken (seconds): " << duration << "</span><br/>" << std::endl;
  snap::web::close_html();
  return 0;
}
