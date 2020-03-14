#include <cppgit2/repository.hpp>
#include <iostream>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {
    repository repo;
    repo.open(argv[1]);

    // Fetch, iterate over and print tags
    for (const auto &tag : repo.tags())
      std::cout << tag << std::endl;

  } else {
    std::cout << "Usage: ./executable <repo_path>\n";
  }
}