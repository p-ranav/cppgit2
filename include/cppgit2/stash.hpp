#pragma once
#include <cppgit2/bitmask_operators.hpp>
#include <cppgit2/checkout.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/ownership.hpp>
#include <cppgit2/strarray.hpp>
#include <cppgit2/tree.hpp>
#include <git2.h>

namespace cppgit2 {

class stash : public libgit2_api {
public:
  class apply : public libgit2_api {
  public:
    // Stash application flags.
    enum class flag { default_ = 0, reinstate_index = (1 << 0) };

    // Stash apply progression states
    enum class progress {
      none = 0,
      // loading the stashed data from the object database.
      loading_stash,
      // the stored index is being analyzed.
      analyze_index,
      // the modified files are being analyzed.
      analyze_modified,
      // the untracked and ignored files are being analyzed.
      analyze_untracked,
      // the untracked files are being written to disk.
      checkout_untracked,
      // the modified files are being written to disk.
      checkout_modified,
      // the stash was applied successfully.
      done
    };

    class options : public libgit2_api {
    public:
      options() {
        auto ret = git_stash_apply_init_options(
            &default_options_, GIT_STASH_APPLY_OPTIONS_VERSION);
        c_ptr_ = &default_options_;
        if (ret != 0)
          throw git_exception();
      }

      options(git_stash_apply_options *c_ptr) : c_ptr_(c_ptr) {}

      // Version
      unsigned int version() const { return c_ptr_->version; }
      void set_version(unsigned int value) { c_ptr_->version = value; }

      // Flags
      flag flags() const { return static_cast<flag>(c_ptr_->flags); }
      void set_flags(flag value) {
        c_ptr_->flags = static_cast<uint32_t>(value);
      }

      // Checkout options
      checkout::options checkout_options() const {
        return checkout::options(&c_ptr_->checkout_options);
      }
      void set_checkout_options(const checkout::options &options) {
        c_ptr_->checkout_options = *(options.c_ptr());
      }

      // Access libgit2 C ptr
      const git_stash_apply_options *c_ptr() const { return c_ptr_; }

    private:
      friend stash;
      git_stash_apply_options *c_ptr_;
      git_stash_apply_options default_options_;
    };
  };

private:
};
ENABLE_BITMASK_OPERATORS(stash::apply::flag);

} // namespace cppgit2