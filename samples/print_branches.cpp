#include <cppgit2/repository.hpp>
#include <iostream>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {
    repository repo(argv[1], false);

    std::cout << "Local branches:\n";
    repo.for_each_branch(
        [](const reference &ref) {
          std::cout << "* " << ref.name() << std::endl;
        },
        branch::branch_type::local);

    std::cout << "Remote branches:\n";
    repo.for_each_branch(
        [](const reference &ref) {
          std::cout << "* " << ref.name() << std::endl;
        },
        branch::branch_type::remote);

  } else {
    std::cout << "Usage: ./executable <repo_path>\n";
  }
}