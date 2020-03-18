#pragma once
#include <cppgit2/checkout.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/merge.hpp>
#include <git2.h>

namespace cppgit2 {

class cherrypick : public libgit2_api {
public:
  class options : public libgit2_api {
  public:
    options() : c_ptr_(nullptr) {
      auto ret = git_cherrypick_init_options(&default_options_,
                                             GIT_CHERRYPICK_OPTIONS_VERSION);
      c_ptr_ = &default_options_;
      if (ret != 0)
        throw git_exception();
    }

    options(git_cherrypick_options *c_ptr) : c_ptr_(c_ptr) {}

    // Version
    unsigned int version() const { return c_ptr_->version; }
    void set_version(unsigned int version) { c_ptr_->version = version; }

    // Mainline
    // For merge commits, the "mainline" is treated as the parent.
    unsigned int mainline() const { return c_ptr_->mainline; }
    void set_mainline(unsigned int mainline) { c_ptr_->mainline = mainline; }

    // Checkout options
    checkout::options checkout_options() const {
      return checkout::options(&c_ptr_->checkout_opts);
    }
    void set_checkout_options(const checkout::options &checkout_options) {
      c_ptr_->checkout_opts = *(checkout_options.c_ptr());
    }

    // Merge options
    merge::options merge_options() const {
      return merge::options(&c_ptr_->merge_opts);
    }
    void set_merge_options(const merge::options &merge_options) {
      c_ptr_->merge_opts = *(merge_options.c_ptr());
    }

    const git_cherrypick_options *c_ptr() const { return c_ptr_; }

  private:
    git_cherrypick_options *c_ptr_;
    git_cherrypick_options default_options_;
  };
};

} // namespace cppgit2