#include <cppgit2/repository.hpp>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 3) {
    auto repo = repository::open(argv[1]);
    auto sm = repo.lookup_submodule(argv[2]);
    std::cout << sm.name() << " from " << sm.url() << std::endl;
    std::cout << "Opening submodule as repo\n";

    auto submodule_repo = sm.open_repository();
    std::cout << "Path: " << submodule_repo.path() << std::endl;

    std::cout << "References\n:";
    submodule_repo.for_each_reference(
        [](const reference &ref) { std::cout << ref.name() << std::endl; });

  } else {
    // Example usage: ./samples/repository_open_submodule ../. "ext/libgit2"
    std::cout << "Usage: ./executable <repo_path> <submodule_name_or_path>\n";
    std::cout
        << "Example usage: ▶ ./samples/open_submodule ../. \"ext/libgit2\""
        << std::endl;
  }
}