#include <string>
#include <vector>
#include <fstream>
#include <sys/stat.h>

#include <iostream>

#include "boost/algorithm/string.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

#include "snap.h"
#include "Program.h"

namespace snap {
  namespace io {
    
    bool file_exists(const std::string &file_name) {
      struct stat buffer;
      return stat(file_name.c_str(), &buffer) == 0;
    }
    
    std::vector<snap::Program> parse_programs(const std::string &file_name) {
      std::ifstream ifs(file_name, std::ifstream::in);    
      return parse_programs(ifs);
    }
  
    std::vector<snap::Program> parse_programs(std::istream &input) {
      std::vector<snap::Program> prog_vector;
      const int read_size = 1000000;
      char *program_text = new char[read_size];
      int cnt = 0;
      while (!input.eof()) {
        if (input.tellg() == -1) {
          throw snap::io::CorruptFileException();
        }
        input.getline(program_text, read_size, -65);
        input.getline(program_text, read_size, -61);
        std::string program_string(program_text);
        prog_vector.emplace_back(program_string);
      }
      delete[] program_text;
      return prog_vector;
    }
    std::vector<std::string> generate_file_names(boost::gregorian::date from,
                                                 boost::gregorian::date to,
                                                 std::string prefix,
                                                 std::string suffix) {
      std::vector<std::string> file_names;
      while (from <= to) {
        file_names.push_back(prefix + snap::date::date_to_string(from) + suffix);
        from += boost::gregorian::date_duration(1);
      }
      return file_names;
    }

    std::map<std::string, std::tuple<std::string, std::string, std::string>> read_dictionary(std::istream &input) {
      std::map<std::string, std::tuple<std::string, std::string, std::string>> dict;
      std::string line;
      while (!input.eof()) {
        std::getline(input, line);
        if (line.find(',') != -1) {
          std::vector<std::string> split_line;
          int current_idx = 0;
          int next_idx = line.find(',', current_idx);
          while (next_idx != -1) {
            split_line.push_back(line.substr(current_idx, next_idx - current_idx));
            current_idx = next_idx + 1;
            next_idx = line.find(',', current_idx);
          }
          split_line.push_back(line.substr(current_idx));
          dict[split_line[1]] = std::tuple<std::string, std::string, std::string>(split_line[2], split_line[0], split_line[3]);
        }
      }            
      return dict;
    }
  }  
}
