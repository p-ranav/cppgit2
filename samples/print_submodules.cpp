#include <cppgit2/repository.hpp>
#include <iostream>
#include <string>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {
    auto repo = repository::open(argv[1]);

    repo.for_each_submodule(
        [](const submodule &module, const std::string &path) {
          std::cout << module.name() << " " << module.url() << std::endl;
        });

  } else {
    std::cout << "Usage: ./executable <repo_path>\n";
  }
}