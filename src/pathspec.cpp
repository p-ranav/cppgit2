#include <cppgit2/pathspec.hpp>
using namespace cppgit2;

pathspec::pathspec() : c_ptr_(nullptr), owner_(ownership::libgit2) {}

pathspec::pathspec(git_pathspec * c_ptr, ownership owner) 
  : c_ptr_(c_ptr), owner_(owner) {}

pathspec::~pathspec() {
  if (c_ptr_ && owner_ == ownership::user)
    git_pathspec_free(c_ptr_);
}

pathspec pathspec::compile(const strarray& paths) {
  pathspec result;
  if (git_pathspec_new(&result.c_ptr_, paths.c_ptr()))
    throw git_exception();
  return result;
}

pathspec::match_list pathspec::match_diff(const diff &diff, pathspec::flag flags) {
  git_pathspec_match_list ** result_c = (git_pathspec_match_list **)malloc(sizeof(git_pathspec_match_list *));
  if (git_pathspec_match_diff(result_c, diff.c_ptr_, static_cast<uint32_t>(flags), c_ptr_))
    throw git_exception();
  return pathspec::match_list(*result_c);
}

pathspec::match_list pathspec::match_index(const index &index, pathspec::flag flags) {
  git_pathspec_match_list ** result_c = (git_pathspec_match_list **)malloc(sizeof(git_pathspec_match_list *));
  if (git_pathspec_match_index(result_c, index.c_ptr_, static_cast<uint32_t>(flags), c_ptr_))
    throw git_exception();
  return pathspec::match_list(*result_c);
}

pathspec::match_list pathspec::match_tree(const tree &tree, pathspec::flag flags) {
  git_pathspec_match_list ** result_c = (git_pathspec_match_list **)malloc(sizeof(git_pathspec_match_list *));
  if (git_pathspec_match_tree(result_c, tree.c_ptr_, static_cast<uint32_t>(flags), c_ptr_))
    throw git_exception();
  return pathspec::match_list(*result_c);
}

bool pathspec::matches_path(pathspec::flag flags, const std::string &path) const {
  return git_pathspec_matches_path(c_ptr_, static_cast<uint32_t>(flags), path.c_str());
}

const git_pathspec * pathspec::c_ptr() const {
  return c_ptr_;
}