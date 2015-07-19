#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "snap.h"

const std::string output_path = "../tmp/";
const int CHUNK_SIZE = 100;


int main() {
  snap::web::print_header();
  int content_length = atoi(getenv("CONTENT_LENGTH"));
  std::string content_type = std::string(getenv("CONTENT_TYPE"));
  char *input = new char[CHUNK_SIZE];
  int bytes_read = 0;
  std::ostringstream input_stream;
  while (fgets(input, CHUNK_SIZE, stdin) != NULL) {
    int next_chunk_size = fmin(CHUNK_SIZE, content_length - bytes_read);    
    input_stream << input;
  } 
  delete[] input;
  std::string input_string = input_stream.str();
  
  std::map<std::string, std::string> files = snap::web::parse_multiform_data(content_type, input_string);
  std::string json = snap::web::matrix_to_json(files["matrix_file"]);
  srand(time(NULL));
  std::string random_id = std::to_string(rand());
  std::string filename = random_id + ".json";
  std::ofstream out_file(output_path + filename, std::ios::out);
  out_file << json;
  out_file.close();
    
  // too lazy to send a proper redirect request, so just insert some javascript instead    
  snap::web::redirect("../visualize.html?filename=tmp%2F" + filename + "&title=" + snap::web::encode_uri(files["graph_title"]));
  snap::web::close_html();

  return 0;
}
