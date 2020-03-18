#include <cppgit2/repository.hpp>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {
    // Discover repository path by walking up from argv[1]
    // When a repository is discovered, its .git directory is returned
    // as the result
    std::cout 
      << repository::discover_path(argv[1], true, "/")
      << std::endl;
  } else {
    std::cout << "Usage: ./executable <repo_path>\n";
  }
}