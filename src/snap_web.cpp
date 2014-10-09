#include <map>
#include <string>
#include <vector>

#include "snap.h"
#include "boost/algorithm/string.hpp"

namespace snap {
  namespace web {
    std::map<std::string, std::string> parse_query_string(std::string query_string) {
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
  }
}
