#include <cppgit2/repository.hpp>
#include <iostream>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 3) {
    repository repo(argv[1], false);
    auto blame = repo.blame_file(argv[2]);
    auto hunk_count = blame.hunk_count();
    std::cout << "Blame Hunk count: " << hunk_count << std::endl;
    std::cout << "Blame Hunk by index:" << std::endl;
    for (size_t i = 0; i < hunk_count; ++i) {
      auto hunk = blame.hunk_by_index(i);
      auto original_commit_id = hunk.orig_commit_id();
      auto original_signature = hunk.orig_signature();
      std::cout << "[" << i << "] " << original_commit_id.to_hex_string() << " "
                << hunk.boundary() << " " << original_signature.name() << " "
                << original_signature.email() << std::endl;
    }
  } else {
    std::cout
        << "Usage: ./executable <repo_path> <file_path_to_get_blame_for>\n";
  }
}
