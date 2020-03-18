#include <cppgit2/repository.hpp>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {
    auto repo = repository::open(argv[1]);
    std::cout 
      << repo.branch_remote_name("refs/remotes/origin/master") // prints "origin"
      << std::endl;
  } else {
    std::cout << "Usage: ./executable <repo_path>\n";
  }
}