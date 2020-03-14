#pragma once
#include <cppgit2/bitmask_operators.hpp>
#include <cppgit2/diff.hpp>
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/ownership.hpp>
#include <functional>
#include <git2.h>

namespace cppgit2 {

class apply : public libgit2_api {
public:
  // Possible application locations for git_apply
  enum class location {
    // Apply the patch to the workdir, leaving the index untouched.
    // This is the equivalent of `git apply` with no location argument.
    workdir = 0,
    // Apply the patch to the index, leaving the working directory
    // untouched.  This is the equivalent of `git apply --cached
    index = 1,
    // Apply the patch to both the working directory and the index.
    // This is the equivalent of `git apply --index`.
    both = 2
  };

  class options : public libgit2_api {
  public:
    // Default construct apply::options
    options() {
      if (git_apply_options_init(&default_options_, GIT_APPLY_OPTIONS_VERSION))
        throw git_exception();
      c_ptr_ = &default_options_;
    }

    // Construct from libgit2 C ptr
    options(git_apply_options *c_ptr) : c_ptr_(c_ptr) {}

    // Version

    unsigned int version() const { return c_ptr_->version; }

    void set_version(unsigned int value) { c_ptr_->version = value; }

    // Flags controlling the behavior of git_apply
    enum class flag {
      // Don't actually make changes, just test that the patch applies.
      // This is the equivalent of `git apply --check`.
      check = (1 << 0)
    };

    flag flags() const { return static_cast<flag>(c_ptr_->flags); }

    void set_flags(flag flag) {
      c_ptr_->flags = static_cast<unsigned int>(flag);
    }

    // Access libgit2 C ptr
    const git_apply_options *c_ptr() const { return c_ptr_; }

  private:
    git_apply_options *c_ptr_;
    git_apply_options default_options_;
  };
};
ENABLE_BITMASK_OPERATORS(apply::options::flag);

}; // namespace cppgit2