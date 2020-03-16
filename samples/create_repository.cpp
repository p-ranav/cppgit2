#include <cppgit2/repository.hpp>
#include <iostream>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {
    auto repo = repository::init(argv[1], false);
  } else {
    std::cout << "Usage: ./executable <repo_path>\n";
  }
}