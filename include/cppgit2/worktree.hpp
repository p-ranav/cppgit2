#pragma once
#include <cppgit2/bitmask_operators.hpp>
#include <cppgit2/data_buffer.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/ownership.hpp>
#include <cppgit2/reference.hpp>
#include <cppgit2/strarray.hpp>
#include <git2.h>
#include <string>
#include <utility>

namespace cppgit2 {

class worktree : public libgit2_api {
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

  class add_options : public libgit2_api {
  public:
    add_options() : c_ptr_(nullptr) {
      auto ret = git_worktree_add_init_options(
          &default_options_, GIT_WORKTREE_ADD_OPTIONS_VERSION);
      c_ptr_ = &default_options_;
      if (ret != 0)
        throw git_exception();
    }

    add_options(git_worktree_add_options *c_ptr) : c_ptr_(c_ptr) {}

    // Version
    unsigned int version() const { return c_ptr_->version; }
    void set_version(unsigned int version) { c_ptr_->version = version; }

    // lock newly created worktree
    bool lock() const { return c_ptr_->lock; }
    void set_lock(bool value) { c_ptr_->lock = value; }

    // reference to use for the new worktree HEAD
    cppgit2::reference reference() const {
      return cppgit2::reference(c_ptr_->ref);
    }
    void set_reference(const cppgit2::reference &ref) {
      c_ptr_->ref = const_cast<git_reference *>(ref.c_ptr());
    }

    // Access libgit2 C ptr
    const git_worktree_add_options *c_ptr() const { return c_ptr_; }

  private:
    git_worktree_add_options *c_ptr_;
    git_worktree_add_options default_options_;
  };

  // Flags which can be passed to git_worktree_prune to alter its
  // behavior.
  enum class prune_type {
    // Prune working tree even if working tree is valid
    valid = 1u << 0,
    // Prune working tree even if it is locked
    locked = 1u << 1,
    // Prune checked out working tree
    tree = 1u << 2,
  };

  // Worktree prune options structure
  class prune_options : public libgit2_api {
    prune_options() : c_ptr_(nullptr) {
      auto ret = git_worktree_prune_init_options(
          &default_options_, GIT_WORKTREE_PRUNE_OPTIONS_VERSION);
      c_ptr_ = &default_options_;
      if (ret != 0)
        throw git_exception();
    }

    prune_options(git_worktree_prune_options *c_ptr) : c_ptr_(c_ptr) {}

    // Version
    unsigned int version() const { return c_ptr_->version; }
    void set_version(unsigned int version) { c_ptr_->version = version; }

    // Flags
    prune_type flags() const { return static_cast<prune_type>(c_ptr_->flags); }
    void set_flags(prune_type flags) {
      c_ptr_->flags = static_cast<uint32_t>(flags);
    }

    // Access libgit2 C ptr
    const git_worktree_prune_options *c_ptr() const { return c_ptr_; }

  private:
    git_worktree_prune_options *c_ptr_;
    git_worktree_prune_options default_options_;
  };

  // Access libgit2 C ptr
  const git_worktree *c_ptr() const;

private:
  friend class repository;
  git_worktree *c_ptr_;
  ownership owner_;
};
ENABLE_BITMASK_OPERATORS(worktree::prune_type);

} // namespace cppgit2