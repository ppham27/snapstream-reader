#include <cmath>
#include <iostream>
#include <fstream>

#include "snap.h"

const int CHUNK_SIZE = 100;

int main() {
  snap::web::print_header();
  int content_length = atoi(getenv("CONTENT_LENGTH"));
  std::string content_type = std::string(getenv("CONTENT_TYPE"));
  char *input = new char[CHUNK_SIZE];
  int bytes_read = 0;
  std::ofstream out("../tmp/tmp.txt", std::ios::out);
  while (fgets(input, CHUNK_SIZE, stdin) != NULL) {
    int next_chunk_size = fmin(CHUNK_SIZE, content_length - bytes_read);    
    std::cout << input;
    out << input;
  } 
  std::cout << std::flush;
  // too lazy to send a proper redirect request, so just insert some javascript instead
  // snap::web::redirect("../index.html");
  snap::web::close_html();
  out.close();

  return 0;
}
