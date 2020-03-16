#include <cppgit2/repository.hpp>
#include <iostream>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {
    auto repo = repository::open(argv[1]);

    repo.for_each_tag([&repo](const std::string &name, const oid &id) {
      std::cout << "[" << id.to_hex_string(8) << "] " << name << std::endl;
    });

  } else {
    std::cout << "Usage: ./executable <repo_path>\n";
  }
}