#include <cppgit2/repository.hpp>
#include <iostream>
#include <string>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 3) {
    repository repo;
    repo.open(argv[1]);

    std::cout << "Should ignore file " << argv[2] << "? ";
    std::cout << std::boolalpha << repo.should_ignore(argv[2]) << std::endl;

    // Example output:

    // $ ./samples/check_ignore_rules ../. .gitignore
    // Should ignore file .gitignore? false

    // $ ./samples/check_ignore_rules ../. main.cpp
    // Should ignore file main.cpp? false

    // $ ./samples/check_ignore_rules ../. build
    // Should ignore file build? true

  } else {
    std::cout << "Usage: ./executable <repo_path> <file_path>\n";
  }
}