#include <cppgit2/repository.hpp>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {
    std::cout 
      << repository::discover_path(argv[1], true, "/")
      << std::endl;
  } else {
    std::cout << "Usage: ./executable <repo_path>\n";
  }
}