#include <cppgit2/repository.hpp>
#include <iostream>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {
    repository repo(argv[1], true);
  } else {
    std::cout << "Usage: ./executable <repo_path>\n";
  }
}