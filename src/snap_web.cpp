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
  }
}
