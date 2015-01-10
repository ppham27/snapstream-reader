#include <algorithm>
#include <cctype>
#include <chrono> 
#include <iostream>
#include <map>
#include <random> 
#include <string>
#include <vector>

#include "snap.h"
#include "boost/algorithm/string.hpp"

namespace snap {
  namespace web {

    std::string sanitize_string(std::string s) {
      boost::replace_all(s, "%0D%0A", "\n");
      boost::replace_all(s, "%21", "!");
      boost::replace_all(s, "%26", "&");
      boost::replace_all(s, "%40", "@");
      boost::replace_all(s, "%2B", "+");
      boost::replace_all(s, "%7B", "{");
      boost::replace_all(s, "%7D", "}");
      boost::replace_all(s, "%2C", ",");
      boost::replace_all(s, "%27", "'");
      boost::replace_all(s, "%28", "(");
      boost::replace_all(s, "%29", ")");
      std::transform(s.begin(), s.end(), s.begin(), ::tolower);
      return s;
    }
    
    std::map<std::string, std::string> parse_query_string(const std::string &query_string) {
      std::map<std::string, std::string> kv;
      std::vector<std::string> entries;
      boost::split(entries, query_string, boost::is_any_of("=&"));
      auto it = entries.begin();
      for (; it != entries.end(); ++it) {
        boost::replace_all(*(it + 1),"+"," ");
        kv[*it] = *(it + 1); ++it;
      }
      return kv;
    }

    void print_header() {
      std::cout << R"ZZZ(Content-type: text/html; charset=iso-8859-1

<html>
<head>
<title>Snapstream Searcher</title>
<meta charset="utf-8">
<style>
body {
font-family: Helvetica;
}
.error {
color: DarkRed;
}
</style>
</head>
<body>)ZZZ"<< std::endl;
    }

    void close_html() {
      std::cout << "</body>\n</html>" << std::endl; 
    }

    void print_excerpts(std::vector<snap::Excerpt> &excerpts,
                        int n) {
      unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
      std::shuffle(excerpts.begin(), excerpts.end(), std::default_random_engine(seed));
      std::cout << "<div>" << std::endl;
      for (int i = 0; i < std::min((int) excerpts.size(), n); ++i) {
        snap::web::print_excerpt(excerpts[i]);
      }
      std::cout << "</div>" << std::endl;
    }

    void print_excerpt(const snap::Excerpt &e) {
      std::cout << "<div>" << std::endl;
      std::cout << "<h4>" << "Program title: " << e.program_title << "</h4>";
      std::cout << "<span>Date: " << e.date << "</span></br>";
      std::cout << "Search strings: ";
      for (auto it = e.search_strings.begin();
           it != e.search_strings.end();
           ++it) {
        std::cout << *it << ", ";
      }
      std::cout << "<br/>" << e.text << "<br/>";    
      std::cout << "</div>" << std::endl;  
    }

    void print_matrix(std::map<std::string, std::map<std::string, std::pair<int, int>>> &results) {
      std::vector<std::string> keys;
      for (auto it = results.begin(); it != results.end(); ++it) { keys.push_back(it -> first); }

      // print matching programs
      std::cout << std::endl;      
      std::cout << "matching_programs" << '\t';
      for (auto it = keys.begin(); it != keys.end(); ++it) {
        if (it != keys.end() - 1) {
          std::cout << *it << '\t';
        } else {
          std::cout << *it << std::endl;
        }
      }
      for (auto it0 = keys.begin(); it0 != keys.end(); ++it0) {
        std::cout << *it0 << '\t';
        for (auto it1 = keys.begin(); it1 != keys.end(); ++it1) {
          int stat = *it1 <= *it0 ? results[*it1][*it0].first : results[*it0][*it1].first;
          if (it1 != keys.end() - 1) {
            std::cout << stat << '\t';
          } else {
            std::cout << stat << std::endl;
          }          
        }
      }
      std::cout << std::endl;

      // print total matches
      std::cout << std::endl;      
      std::cout << "total_matches" << '\t';
      for (auto it = keys.begin(); it != keys.end(); ++it) {
        if (it != keys.end() - 1) {
          std::cout << *it << '\t';
        } else {
          std::cout << *it << std::endl;
        }
      }
      for (auto it0 = keys.begin(); it0 != keys.end(); ++it0) {
        std::cout << *it0 << '\t';
        for (auto it1 = keys.begin(); it1 != keys.end(); ++it1) {
          int stat = *it1 <= *it0 ? results[*it1][*it0].second : results[*it0][*it1].second;
          if (it1 != keys.end() - 1) {
            std::cout << stat << '\t';
          } else {
            std::cout << stat << std::endl;
          }          
        }
      }
      std::cout << std::endl; 
    }

    void print_corrupt_files(const std::vector<std::string> &corrupt_files) {
      if (corrupt_files.size() > 0) {
        std::cout << "<span class=\"error\">Warning: ";
        for (auto it = corrupt_files.begin(); it != corrupt_files.end() - 1; ++it) {
          std::cout << *it << ", ";
        }
        if (corrupt_files.size() == 1) {
          std::cout << corrupt_files.back() << " is corrupt.</span>" << std::endl;
        } else {
          std::cout << " and " << corrupt_files.back() << " are corrupt.</span>" << std::endl;
        }
      }
    }
  }
}
