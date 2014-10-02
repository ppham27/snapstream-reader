#include <cstdlib>
#include <iostream>
#include <string>

#include "snap.h"

const std::string prefix = "Data/";
const std::string suffix = "-Combined.txt";

int main() {
  std::cout << "Content-type: text/plain; charset=iso-8859-1\n" << std::endl;
  
  std::cout << "Hello, world" << std::endl;  
  char *query_string = getenv("QUERY_STRING");
  std::cout << query_string << std::endl;
  return 0;
}
