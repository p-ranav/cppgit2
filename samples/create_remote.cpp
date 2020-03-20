#include <cppgit2/diff.hpp>
#include <cppgit2/repository.hpp>
#include <iostream>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {

    // Create new repo
    auto repo = repository::init(argv[1], false);

    // Create remote
    auto remote =
        repo.create_remote("origin", "https://github.com/p-ranav/test");

  } else {
    std::cout << "Usage: ./executable <new_repo_path>\n";
  }
}