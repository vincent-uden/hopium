#include "System.h"

void writeToFile(std::string path, std::string content) {
  std::ofstream outfile;
  outfile.open(path);

  outfile << content;
  outfile.close();
}

std::string readFromFile(std::string path) {
  std::ifstream infile;
  infile.open(path);

  std::string content;
  infile >> content;
  infile.close();

  return content;
}

bool fileExists(std::string path) {
  std::ifstream infile;
  infile.open(path);

  return infile.good();
}
