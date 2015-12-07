#ifndef SNAP_H
#define SNAP_H

#include <functional>
#include <map>
#include <exception>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "boost/date_time/gregorian/gregorian.hpp"

#include "Program.h"
#include "Excerpt.h"
#include "Expression.h"

namespace snap {
  std::map<std::string, std::vector<int>> find(const std::vector<std::string> &patterns,
                                               const std::string &s);
  std::map<std::string, std::vector<int>> find(const std::string &pattern,
                                               const std::string &s);
  std::map<std::string, std::vector<int>> near(const snap::Expression &pattern1,
                                               const snap::Expression &pattern2,
                                               int distance,
                                               const std::string &s);

  std::map<std::string, std::map<std::string, int>> pair(const std::map<std::string, std::vector<int>> &match_positions,
                                                         int distance);

  std::vector<int> andv(const std::vector<int> &a, const std::vector<int> &b);
  std::vector<int> notandv(const std::vector<int> &a, const std::vector<int> &b);
  std::vector<int> orv(const std::vector<int> &a, const std::vector<int> &b);
  std::vector<int> nearv(const std::vector<int> &a, const std::vector<int> &b, int distance);
  std::vector<int> notnearv(const std::vector<int> &a, const std::vector<int> &b, int distance);  

  std::vector<int> evaluate_expression(const snap::Expression &e, const std::map<std::string, std::vector<int>> &locations);
  std::map<std::string, std::vector<int>> evaluate_expressions(const std::vector<snap::Expression> &expressions,
                                                               const std::map<std::string, std::vector<int>> &locations);
  
  namespace web {
    std::map<std::string, std::string> parse_query_string(const std::string &query_string);
    std::map<std::string, std::string> parse_multiform_data(const std::string &content_type,
                                                            const std::string &body);
    std::string matrix_to_json(std::string matrix);
    void print_header();
    void close_html();
    void redirect(const std::string &href);
    void print_excerpts(std::vector<snap::Excerpt> &excerpts, int n, bool random = false);
    void print_excerpt(const snap::Excerpt &e);
    void print_missing_files(const std::vector<std::string> &missing_files);
    void print_corrupt_files(const std::vector<std::string> &corrupt_files);
    void print_matrix(std::map<std::string, std::map<std::string, std::tuple<int, int, int>>> &results,
                      std::function<int(std::tuple<int, int, int>)> getter,
                      std::ostream &outputStream, bool header = true, char sep = '\t');
    std::string encode_uri(const std::string &s);
    std::string decode_uri(std::string s);
    std::string create_link(std::string href, std::string text);
    std::string create_link(std::string href, std::string text, std::string id);
  }
  
  namespace io {
    bool file_exists(const std::string &file_name);
    std::vector<snap::Program> parse_programs(const std::string &file_name);
    std::vector<snap::Program> parse_programs(std::istream &input);
    std::vector<std::string> generate_file_names(boost::gregorian::date from,
                                                 boost::gregorian::date to,
                                                 std::string prefix,
                                                 std::string suffix);
    std::map<std::string, std::pair<std::string, std::string>> read_dictionary(std::istream &input);
    class CorruptFileException: public std::runtime_error {
    public:
      CorruptFileException() : std::runtime_error("File is corrupt.") {}
      virtual const char* what() const noexcept {
        std::string error_message(std::runtime_error::what());
        return error_message.c_str();
      }
    };
  }
  
  namespace date {
    boost::gregorian::date string_to_date(std::string d);
    std::string date_to_string(boost::gregorian::date d);
    class InvalidDateException: public std::runtime_error {
    private:
      std::string d;
    public:
      InvalidDateException(std::string d) : std::runtime_error(" is not a valid date"), d(d) {}      
      virtual const char* what() const noexcept {
        std::string error_message(std::runtime_error::what());
        error_message = d + error_message;
        return error_message.c_str();
      }
    };
  }

  namespace word {
    std::vector<std::vector<std::pair<std::string, int>>> tokenize(const std::string &text);
    std::map<std::string, int> count_words(const std::vector<std::vector<std::pair<std::string, int>>> &phrases);
    std::map<std::string, std::pair<int, int>> compare_word_counts(const std::map<std::string, int> &a, 
                                                                   const std::map<std::string, int> &b,
                                                                   int min_count,
                                                                   double min_percent_increase);
  }  
}

#endif

