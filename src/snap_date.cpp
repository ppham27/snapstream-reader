#include <cmath>
#include <cstring>
#include <string>
#include <exception>

#include "boost/algorithm/string.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"


#include "snap.h"

namespace snap {
  namespace date {
    std::string pad_number(int n, int p) {
      int digits = log10(n) + 1;
      std::string nstr = std::to_string(n);
      while (digits < p) {
        nstr = '0' + nstr; ++digits;
      }
      return nstr;
    }
  
    std::string date_to_string(boost::gregorian::date d) {
      boost::gregorian::date::ymd_type ymd = d.year_month_day();
      return pad_number(ymd.year, 4) + '-' + pad_number(ymd.month, 2) + '-' + pad_number(ymd.day, 2);
    }

    boost::gregorian::date string_to_date(std::string d) {
      if (d.size() == 0) { throw InvalidDateException("{empty string}"); }      
      char *d_c_str = new char[d.length() + 1];
      std::strcpy(d_c_str, d.c_str());
      char *tok = strtok(d_c_str,"-");
      int year = atoi(tok);
      if (year == 0) { delete[] d_c_str; throw InvalidDateException(d); }      
      tok = strtok(NULL,"-");
      int month = atoi(tok);
      if (month == 0) { delete[] d_c_str; throw InvalidDateException(d); }      
      tok = strtok(NULL,"-");
      int day = atoi(tok);
      if (day == 0) { delete[] d_c_str; throw InvalidDateException(d); }      
      delete[] d_c_str;
      return boost::gregorian::date(year, month, day);    
    }
  }
}
