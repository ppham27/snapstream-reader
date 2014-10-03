#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <sys/stat.h>

#include "boost/date_time/gregorian/gregorian.hpp"

#include "snap.h"

int main() {
  std::vector<snap::Program> tmp = snap::parse_programs("Data/2014-07-01-Combined.txt");
  
  return 0;
}
