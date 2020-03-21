#include <cppgit2/repository.hpp>
#include <iostream>
#include <string>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 2) {
    auto repo = repository::open(argv[1]);
    auto index = repo.index();

    auto diff = repo.create_diff_index_to_workdir(index);

    diff.for_each(
        // File Callback
        [](const diff::delta &delta, float progress) {
            auto status = delta.status();

            switch(status) {
            case diff::delta::type::added:
                std::cout << "Added " << delta.new_file().path() << std::endl;
                break;
            case diff::delta::type::deleted:
                std::cout << "Deleted " << delta.new_file().path() << std::endl;
                break;
            case diff::delta::type::modified:
                std::cout << "Modified " << delta.new_file().path() << std::endl;
                break;
            case diff::delta::type::renamed:
                std::cout << "Renamed " << delta.old_file().path() << " -> " << 
                delta.new_file().path() << std::endl;
                break;
            case diff::delta::type::copied:
                std::cout << "Copied " << delta.new_file().path() << std::endl;
                break;
            case diff::delta::type::untracked:
                std::cout << "Untracked " << delta.new_file().path() << std::endl;
                break;
            default:
                break;
            }
        }
        // TODO: add the 3 other callbacks - binary_callback, hunk_callback, line_callback
        // and prepare a nice diff view
    );

  } else {
    std::cout << "Usage: ./executable <repo_path>\n";
  }
}