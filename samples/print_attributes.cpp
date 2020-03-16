#include <cppgit2/repository.hpp>
#include <iostream>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {
    auto repo = repository::init(argv[1], false);

    repo.for_each_attribute(
        attribute::flag::file_then_index, ".gitattributes",
        [](const std::string &name, const std::string &value) {
          std::cout << name << ": " << value << std::endl;
        });
  } else {
    std::cout << "Usage: ./executable <repo_path>\n";
  }
}