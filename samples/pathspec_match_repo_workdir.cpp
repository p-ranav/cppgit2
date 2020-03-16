#include <cppgit2/repository.hpp>
using namespace cppgit2;

int main(int argc, char **argv) {
  if (argc == 3) {
    auto repo = repository::open(argv[1]);

    auto pathspec = pathspec::compile(std::vector<std::string>{argv[2]});
    auto match_list = pathspec.match_workdir(repo, pathspec::flag::default_);
    std::cout << "Matched " << match_list.size() << " files\n";
    for (size_t i = 0; i < match_list.size(); ++i) {
      std::cout << "* " << match_list.entry(i) << std::endl;
    }
  } else {
    std::cout << "Usage: ./executable <repo_path> <pathspec>\n";
  }
}

// Example output:
// $ ./samples/pathspec_match_repo_workdir ../. "include/cppgit2/*.hpp"

/*
Matched 41 files
* include/cppgit2/annotated_commit.hpp
* include/cppgit2/apply.hpp
* include/cppgit2/attribute.hpp
* include/cppgit2/bitmask_operators.hpp
* include/cppgit2/blame.hpp
* include/cppgit2/blob.hpp
* include/cppgit2/branch.hpp
* include/cppgit2/checkout.hpp
* include/cppgit2/commit.hpp
* include/cppgit2/config.hpp
* include/cppgit2/data_buffer.hpp
* include/cppgit2/diff.hpp
* include/cppgit2/file_mode.hpp
* include/cppgit2/git_exception.hpp
* include/cppgit2/index.hpp
* include/cppgit2/libgit2_api.hpp
* include/cppgit2/note.hpp
* include/cppgit2/object.hpp
* include/cppgit2/odb.hpp
* include/cppgit2/oid.hpp
* include/cppgit2/ownership.hpp
* include/cppgit2/pack_builder.hpp
* include/cppgit2/pathspec.hpp
* include/cppgit2/proxy.hpp
* include/cppgit2/push.hpp
* include/cppgit2/reference.hpp
* include/cppgit2/reflog.hpp
* include/cppgit2/repository.hpp
* include/cppgit2/reset.hpp
* include/cppgit2/revision.hpp
* include/cppgit2/signature.hpp
* include/cppgit2/stash.hpp
* include/cppgit2/status.hpp
* include/cppgit2/strarray.hpp
* include/cppgit2/submodule.hpp
* include/cppgit2/tag.hpp
* include/cppgit2/time.hpp
* include/cppgit2/transaction.hpp
* include/cppgit2/tree.hpp
* include/cppgit2/tree_builder.hpp
* include/cppgit2/worktree.hpp
*/