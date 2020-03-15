#pragma once
#include <cppgit2/bitmask_operators.hpp>
#include <cppgit2/diff.hpp>
#include <cppgit2/index.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/ownership.hpp>
#include <git2.h>
#include <vector>

namespace cppgit2 {

class pathspec : public libgit2_api {
public:
  // Default construct a pathspec object
  pathspec();

  // Construct from libgit2 C ptr
  pathspec(git_pathspec *c_ptr, ownership owner);

  // Cleanup pathspec if needed
  ~pathspec();

  enum class flag {
    default_ = 0,
    ignore_case = (1u << 0),
    use_case = (1u << 1),
    no_glob = (1u << 2),
    no_match_error = (1u << 3),
    find_failures = (1u << 4),
    failures_only = (1u << 5),
  };

  class match_list {
  public:
    match_list() : c_ptr_(nullptr), owner_(ownership::libgit2) {}
    match_list(git_pathspec_match_list *c_ptr,
               ownership owner = ownership::libgit2)
        : c_ptr_(c_ptr), owner_(owner) {}

    ~match_list() {
      if (c_ptr_ && owner_ == ownership::user)
        git_pathspec_match_list_free(c_ptr_);
    }

    // Get a matching diff delta by position.
    diff::delta diff_entry(size_t pos) const {
      return diff::delta(git_pathspec_match_list_diff_entry(c_ptr_, pos));
    }

    // Get a matching filename by position.
    std::string entry(size_t pos) const {
      auto ret = git_pathspec_match_list_entry(c_ptr_, pos);
      if (ret)
        return std::string(ret);
      return "";
    }

    // Get an original pathspec string that had no matches.
    std::string failed_entry(size_t pos) const {
      auto ret = git_pathspec_match_list_failed_entry(c_ptr_, pos);
      if (ret)
        return std::string(ret);
      return "";
    }

    // Get the number of items in a match list.
    size_t size() const { return git_pathspec_match_list_entrycount(c_ptr_); }

    // Get the number of pathspec items that did not match.
    size_t failed_entrycount() const {
      return git_pathspec_match_list_failed_entrycount(c_ptr_);
    }

    // Acecss libgit2 C ptr
    const git_pathspec_match_list *c_ptr() const { return c_ptr_; }

  private:
    ownership owner_;
    git_pathspec_match_list *c_ptr_;
  };

  // Compile a pathspec
  static pathspec compile(const strarray &paths);

  // Match a pathspec against files in a diff list.
  match_list match_diff(const diff &diff, flag flags);

  // Match a pathspec against entries in an index.
  match_list match_index(const index &index, flag flags);

  // Match a pathspec against files in a tree.
  match_list match_tree(const tree &tree, flag flags);

  // Match a pathspec against the working directory of a repository.
  match_list match_workdir(const class repository &repo, flag flags);

  // Try to match a path against a pathspec
  // Returns true if path matches spec, false if it does not
  bool matches_path(flag flags, const std::string &path) const;

  // Access libgit2 C ptr
  const git_pathspec *c_ptr() const;

private:
  ownership owner_;
  git_pathspec *c_ptr_;
};
ENABLE_BITMASK_OPERATORS(pathspec::flag);

} // namespace cppgit2