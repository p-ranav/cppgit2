#include <cppgit2/repository.hpp>
#include <iostream>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {
    repository repo;
    repo.open(argv[1]);

    repo.for_each_reference(
        [](const reference &ref) { std::cout << ref.name() << std::endl; });

    // Use the following if you just want to iterate over reference names
    // repo.for_each_reference_name(
    //     [](const std::string &refname) { std::cout << refname << std::endl; });

    // Use the following if you want to print normalized reference names
    //repo.for_each_reference(
    //    [](const reference &ref) { 
    //  std::cout << reference::normalize_name(256, ref.name(), 
    //    reference::format::allow_onelevel) << std::endl; });

  } else {
    std::cout << "Usage: ./executable <repo_path>\n";
  }
}