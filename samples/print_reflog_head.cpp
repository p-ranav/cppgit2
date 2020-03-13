#include <cppgit2/repository.hpp>

int main(int argc, char **argv) {
  if (argc == 2) {
    cppgit2::repository repo;
    repo.open(argv[1]);

    auto reflog = repo.read_reflog("HEAD");
    for (size_t i = 0; i < reflog.size(); ++i) {
      auto entry = reflog[i];
      std::cout << entry.new_oid().to_hex_string(8) << " HEAD@{" << i
                << "}: " << entry.message() << std::endl;
    }
  } else {
    std::cout << "Usage: ./executable <repo_path>\n";
  }
}