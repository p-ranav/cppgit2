#include <cppgit2/repository.hpp>
#include <iostream>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 3) {
    
    auto repo = repository::open(argv[1]);
    auto module = repo.lookup_submodule(argv[2]);
    std::cout << module.name() << " from " << module.url() << std::endl;

  } else {
    std::cout << "Usage: ./executable <repo_path> <submodule_name_or_path>\n";
  }
}