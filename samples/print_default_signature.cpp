#include <cppgit2/repository.hpp>
#include <iostream>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {
    auto repo = repository::open(argv[1]);

    auto default_signature = repo.default_signature();
    std::cout << "Name: " << default_signature.name()
              << "\nEmail: " << default_signature.email() << std::endl;

  } else {
    std::cout << "Usage: ./executable <repo_path>\n";
  }
}