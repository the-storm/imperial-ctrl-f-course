#include <iostream>
#include <fstream>
#include "libflac.hpp"

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0] << " input.flac" << std::endl;
    return 1;
  }

  std::ifstream fin(argv[1], std::ios::in|std::ios::binary);
  if (!fin) {
    std::cout << "Cannot open file: " << argv[1] << std::endl;
    return 1;
  }

  FlacFile ff(fin);
  if (!ff.Parse()) {
    std::cout << "Cannot parse file: " << ff.Reason() << std::endl;
    return 1;
  }

  ff.Dump();
  return 0;
}
