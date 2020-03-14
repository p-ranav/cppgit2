#include <cppgit2/repository.hpp>
#include <iostream>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {
    repository repo;
    repo.open(argv[1]);

    repo.for_each_stash(
        [](size_t index, const std::string &message, const oid &id) {
          std::cout << index << " [" << id.to_hex_string(8) << "] " << message
                    << std::endl;
        });

  } else {
    std::cout << "Usage: ./executable <repo_path>\n";
  }
}