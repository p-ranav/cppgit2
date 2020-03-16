#include <cppgit2/repository.hpp>
#include <iostream>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 3) {
    auto repo = repository::clone(argv[1], argv[2]);
  } else {
    std::cout << "Usage: ./executable <repo_url> <path>\n";
  }
}