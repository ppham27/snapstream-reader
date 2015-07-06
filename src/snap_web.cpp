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
#include "boost/format.hpp"

namespace snap {
  namespace web {

    std::string sanitize_string(std::string s) {
      boost::replace_all(s, "%0D%0A", "\n");
      boost::replace_all(s, "%21", "!");
      boost::replace_all(s, "%22", "\"");
      boost::replace_all(s, "%26", "&");
      boost::replace_all(s, "%2F", "/");
      boost::replace_all(s, "%40", "@");
      boost::replace_all(s, "%2B", "+");
      boost::replace_all(s, "%7B", "{");
      boost::replace_all(s, "%7D", "}");
      boost::replace_all(s, "%2C", ",");
      boost::replace_all(s, "%27", "'");
      boost::replace_all(s, "%28", "(");
      boost::replace_all(s, "%29", ")");
      boost::replace_all(s, "%3A", ":");
      boost::replace_all(s, "%3B", ";");
      boost::replace_all(s, "%3C", "<");
      boost::replace_all(s, "%3E", ">");
      boost::replace_all(s, "%3F", "?");
      boost::replace_all(s, "%5B", "[");
      boost::replace_all(s, "%5D", "]");
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

    std::map<std::string, std::string> parse_multiform_data(const std::string &content_type,
                                                            const std::string &body) {
      // first get the boundary
      std::map<std::string, std::string> kv;
      std::string boundary = content_type.substr(content_type.find("boundary="));
      boundary = boundary.substr(9);
      if (boundary.find(';') != -1) boundary = boundary.substr(0, boundary.find(';'));
      int cursor = body.find(boundary) + boundary.length();
      while (cursor < body.length()) {
        int nextBoundaryIdx = body.find(boundary, cursor);
        while (body[nextBoundaryIdx - 1] == '-') --nextBoundaryIdx;
        std::string part = body.substr(cursor, nextBoundaryIdx - cursor);
        int partCursor = part.find("name=\"") + 6;
        std::string name = part.substr(partCursor, part.find('"', partCursor)-partCursor);
        for (int i = 0; i < 3; ++i) partCursor = part.find('\n', partCursor) + 1;
        std::string file = part.substr(partCursor, part.length() - partCursor - 2);
        kv[name] = file;
        cursor = body.find(boundary, cursor) + boundary.length();
        if (body.find(boundary, cursor) == -1) break;
      }
      return kv;
    }

    std::string matrix_to_json(std::string matrix) {
      // matrix comes in CSV form
      std::vector<std::string> rows;
      boost::split(rows, matrix, boost::is_any_of("\r\n"));
      // remove blank rows?
      auto it = rows.begin();
      while (it != rows.end()) {
        boost::algorithm::trim(*it);
        if (*it == "") {
          it = rows.erase(it);
        } else {
          ++it;        
        }
      }
      int N = rows.size();      // number of countries
      std::vector<std::vector<std::string>> rowsSplit(N);
      // item info, for countries, tuple is symbol, name, size
      std::ostringstream json;
      json << "{\"nodes\":[";
      for (int i = 0; i < N; ++i) {
        boost::split(rowsSplit[i], rows[i], boost::is_any_of(","));
        json << boost::format("{\"symbol\":\"%s\",\"name\":\"%s\",\"size\":{\"1\":%f}}") % rowsSplit[i][0] % rowsSplit[i][1] % rowsSplit[i][2];
        if (i < N - 1) {
          json << ',';
        } else {
          json << "],\"times\":[{\"name\":\"1\",\"key\":\"1\"}],\"links\":[";
        }
      }
      for (int i = 0; i < N; ++i) {
        json << '[';
        for (int j = 0; j < N; ++j) {
          int k = 1;
          if (i == j) k = -1;
          json << boost::format("{\"1\":%f,\"k\":%d}") % rowsSplit[i][j + 3] % k;
          if (j < N - 1) {
            json << ',';
          }
        }
        if (i < N - 1) {
          json << "],";
        } else {
          json << "]]}";
        }
      }      
      return json.str();
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

    void redirect(const std::string &href) {
      std::cout << "<script charset=\"utf-8\">"
                << "window.location.href='"
                << href << "'"
                << "</script>" << std::endl;      
    }

    void print_excerpts(std::vector<snap::Excerpt> &excerpts,
                        int n, bool random) {
      if (random) { 
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(excerpts.begin(), excerpts.end(), std::default_random_engine(seed));
      }
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

    void print_matrix(std::map<std::string, std::map<std::string, std::pair<int, int>>> &results,
                      std::function<int(std::pair<int, int>)> getter,
                      std::ostream &outputStream, bool header, char sep) {
      std::vector<std::string> keys;
      for (auto it = results.begin(); it != results.end(); ++it) { keys.push_back(it -> first); }

      // print matching programs
      if (header) {
        outputStream << sep;
        for (auto it = keys.begin(); it != keys.end(); ++it) {
          if (it != keys.end() - 1) {
            outputStream << *it << sep;
          } else {
            outputStream << *it << std::endl;
          }
        }
      }
      for (auto it0 = keys.begin(); it0 != keys.end(); ++it0) {
        if(header) outputStream << *it0 << sep;
        for (auto it1 = keys.begin(); it1 != keys.end(); ++it1) {
          int stat = *it1 <= *it0 ? getter(results[*it1][*it0]) : getter(results[*it0][*it1]);
          if (it1 != keys.end() - 1) {
            outputStream << stat << sep;
          } else {
            outputStream << stat << std::endl;
          }          
        }
      }
    }

    void print_missing_files(const std::vector<std::string> &missing_files) {
      if (missing_files.size() > 0) {
        std::cout << "<span class=\"error\">Warning: ";
        for (auto it = missing_files.begin(); it != missing_files.end() - 1; ++it) {
          std::cout << *it << ", ";
        }
        if (missing_files.size() == 1) {
          std::cout << missing_files.back() << " is missing.</span>" << std::endl;
        } else {
          std::cout << " and " << missing_files.back() << " are missing.</span>" << std::endl;
        }
      }
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

    std::string create_link(std::string href, std::string text) {
      return "<a href=\"" + href + "\">" + text + "</a>";
    }
  }
}
