#ifndef PROGRAM_H
#define PROGRAM_H

#include <string>
#include <regex>

namespace snap {
  class Program {
  public:
    std::string title;
    std::string description;
    std::string channel;    
    std::string aired_date;
    std::string recorded_date;
    std::string raw_text;
    std::string text;
    std::string lower_text;
    bool operator==(const snap::Program &other) const;
    bool operator!=(const snap::Program &other) const;
    Program(const std::string &program_text);    
  private:
    std::regex time_regex = std::regex("\n\n\\[[0-9]{1,2}:[0-9]{2}:[0-9]{2} (AM|PM)\\]\n\n");
    void read_header(const std::string &header);
    std::string read_date(const std::string &date);
  };  
}


#endif

