#include <cppgit2/repository.hpp>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {
    auto repo = repository::open(argv[1]);
    auto revwalk = repo.create_revwalk();
    revwalk.push_head();
    while (!revwalk.done()) {
      auto id = revwalk.next();
      auto commit = repo.lookup_commit(id);

      std::cout << "[" << id.to_hex_string(8) << "] " << commit.summary()
                << std::endl;
    }
  } else {
    std::cout << "Usage: ./executable <repo_path>\n";
  }
}