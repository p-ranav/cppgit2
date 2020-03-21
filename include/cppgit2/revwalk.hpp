#pragma once
#include <cppgit2/bitmask_operators.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/ownership.hpp>
#include <functional>
#include <git2.h>

namespace cppgit2 {

class revwalk : public libgit2_api {
public:
  // Default construct a revwalk
  revwalk();

  // Construct from libgit2 C ptr
  revwalk(git_revwalk *c_ptr, ownership owner);

  // Cleanup revwalk
  ~revwalk();

  // Adds, changes or removes a callback function to hide a commit and its
  // parents.
  //
  // This is a callback function that user can provide to hide a commit
  // and its parents. If the callback function returns non-zero value, then this
  // commit and its parents will be hidden.
  void add_hide_callback(std::function<int(const oid &)> callback);

  // Returns true if there are no more commits left to walk
  bool done() const;

  // Mark a commit (and its ancestors) uninteresting for the output.
  // The given id must belong to a committish on the walked repository.
  void hide(const oid &commit_id);

  // Hide matching references.
  void hide_glob(const std::string &glob);

  // Hide the repository's HEAD
  void hide_head();

  // Hide the OID pointed to by a reference
  void hide_reference(const std::string &ref);

  // Get the next commit from the revision walk.
  // The initial call to this method is not blocking when iterating through a
  // repo with a time-sorting mode.
  //
  // Iterating with Topological or inverted modes makes the initial call
  // blocking to preprocess the commit list, but this block should be mostly
  // unnoticeable on most repositories (topological preprocessing times at 0.3s
  // on the git.git repo).
  //
  // The revision walker is reset when the walk is over.
  oid next();

  // Add a new root for the traversal
  // The pushed commit will be marked as one of the roots from which to start
  // the walk. This commit may not be walked if it or a child is hidden.
  void push(const oid &id);

  // Push matching references
  void push_glob(const std::string &glob);

  // Push the repository's HEAD
  void push_head();

  // Push and hide the respective endpoints of the given range.
  void push_range(const std::string &range);

  // Push the OID pointed to by a reference
  void push_reference(const std::string &ref);

  // Return the repository on which this walker is operating.
  class repository repository();

  // Reset the revision walker for reuse.
  // This will clear all the pushed and hidden commits,
  // and leave the walker in a blank state
  void reset();

  // Flags to specify the sorting which a revwalk should perform.
  enum class sort {
    none = 0,
    topological = (1 << 0),
    time = (1 << 1),
    reverse = (1 << 2)
  };

  // Change the sorting mode when iterating through the repository's contents.
  void set_sorting_mode(revwalk::sort sorting_mode);

  // Simplify the history by first-parent
  void simplify_first_parent();

  // Access libgit2 C ptr
  const git_revwalk *c_ptr() const;

private:
  friend class pack_builder;
  friend class repository;
  bool done_;
  ownership owner_;
  git_revwalk *c_ptr_;
};
ENABLE_BITMASK_OPERATORS(revwalk::sort);

} // namespace cppgit2