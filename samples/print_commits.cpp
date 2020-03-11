#include <cppgit2/repository.hpp>
#include <iostream>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {
    repository repo;
    repo.open(argv[1]);

    repo.for_each_commit([](const commit &c) {
      std::cout << c.id().to_hex_string(8) << " [" << c.committer().name() << "]"
                << " " << c.summary() << std::endl;
    });

  } else {
    std::cout << "Usage: ./executable <repo_path>\n";
  }
}