#pragma once
#include <cppgit2/data_buffer.hpp>
#include <cppgit2/ownership.hpp>
#include <cppgit2/strarray.hpp>
#include <git2.h>
#include <string>
#include <utility>

namespace cppgit2 {

class worktree {
public:
  // Default construct a worktree
  worktree();

  // Construct from libgit2 C ptr
  // If owned by user, will be free'd in the destructor
  worktree(git_worktree *c_ptr, ownership owner = ownership::libgit2);

  // Free worktree if owned by user
  ~worktree();

  // If true, the string holds the reason
  std::pair<bool, std::string> is_locked() const;

  //
  // TODO: CONSIDER ADDING AN OPTIONS CLASS
  // AND MERGE is_prunable() & prune() METHODS
  // 

  // Is the worktree prunable with the given options?
  // A worktree is not prunable in the following scenarios:
  //   - the worktree is linking to a valid on-disk worktree. The valid member
  //   will cause this check to be ignored.
  //   - the worktree is locked. The locked flag will cause this check to be
  //   ignored.
  bool is_prunable(unsigned int version, uint32_t flags) const;

  // Uses git_worktree_prune_options_init to initialize prune options
  bool is_prunable() const;

  // Lock worktree if not already locked
  //
  // Lock a worktree, optionally specifying a reason why the linked working tree
  // is being locked.
  void lock(const std::string &reason = "");

  // Retrieve the name of the worktree
  std::string name() const;

  // Retrieve the filesystem path for the worktree
  std::string path() const;

  // Prune working tree
  //
  // Prune the working tree, that is remove the git data structures on disk. The
  // repository will only be pruned of worktree.is_prunable succeeds.
  void prune(unsigned int version, uint32_t flags);

  // Prune working tree
  //
  // Uses git_worktree_prune_options_init to initialize prune options
  void prune();

  // Unlock a locked worktree
  void unlock();

  // Check if worktree is valid
  //
  // A valid worktree requires both the git data structures inside the linked
  // parent repository and the linked working copy to be present.
  bool is_valid();

  // Similar to is_valid but throws exception
  // if worktree is invalid
  bool validate();

  // Access libgit2 C ptr
  const git_worktree *c_ptr() const;

private:
  git_worktree *c_ptr_;
  ownership owner_;
};

} // namespace cppgit2