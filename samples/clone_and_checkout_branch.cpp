#include <cppgit2/repository.hpp>
#include <iostream>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 4) {
    clone::options options;
    options.set_checkout_branch_name(argv[3]);
    auto repo = repository::clone(argv[1], argv[2], options);
  } else {
    std::cout
        << "Usage: ./executable <repo_url> <path> <checkout_branch_name>\n";
  }
}