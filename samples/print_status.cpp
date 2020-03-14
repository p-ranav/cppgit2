#include <cppgit2/repository.hpp>
#include <iostream>
#include <string>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {
    repository repo;
    repo.open(argv[1]);

    repo.for_each_status([](const std::string& path, status::status_type status_flags) {
      if ((status_flags & status::status_type::index_modified) != 
        status::status_type::index_modified) {
        if ((status_flags & status::status_type::ignored) !=
          status::status_type::ignored) {
          std::cout << "modified: " << path << std::endl;
        } else {
          std::cout << "ignored: " << path << std::endl;
        }
      }
    });

  } else {
    std::cout << "Usage: ./executable <repo_path>\n";
  }
}