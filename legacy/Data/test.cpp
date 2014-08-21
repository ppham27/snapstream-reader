#include <iostream>
#include <fstream>
#include <sstream>

int main() {
  std::ifstream myfile;
  myfile.open("2012-10-23-Combined.txt", std::ios::in);
  std::string line;
  std::cout << line.capacity() << std::endl;
  while (getline(myfile, line)) {    
    if (line.find("ï»¿") != -1) { break; }
  }
  std::cout << line << std::endl;
  myfile.close();
  return 0;
}
