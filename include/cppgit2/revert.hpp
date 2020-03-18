#pragma once
#include <cppgit2/checkout.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/merge.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/ownership.hpp>
#include <git2.h>

namespace cppgit2 {

class revert : public libgit2_api {
public:
  class options : public libgit2_api {
  public:
    options() {
      auto ret = git_revert_init_options(&default_options_,
                                         GIT_REVERT_OPTIONS_VERSION);
      c_ptr_ = &default_options_;
      if (ret != 0)
        throw git_exception();
    }

    options(git_revert_options *c_ptr) : c_ptr_(c_ptr) {}

    // Version
    unsigned int version() const { return c_ptr_->version; }
    void set_version(unsigned int value) { c_ptr_->version = value; }

    // For merge commits, the "mainline" is treated as the parent.
    unsigned int mainline() const { return c_ptr_->mainline; }
    void set_mainline(unsigned int value) { c_ptr_->mainline = value; }

    // Options for the merging
    merge::options merge_options() const {
      return merge::options(&c_ptr_->merge_opts);
    }
    void set_merge_options(const merge::options &options) {
      c_ptr_->merge_opts = *(options.c_ptr());
    }

    // Options for the checkout
    checkout::options checkout_options() const {
      return checkout::options(&c_ptr_->checkout_opts);
    }
    void set_checkout_options(const checkout::options &options) {
      c_ptr_->checkout_opts = *(options.c_ptr());
    }

    const git_revert_options *c_ptr() const { return c_ptr_; }

  private:
    friend revert;
    git_revert_options *c_ptr_;
    git_revert_options default_options_;
  };
};

} // namespace cppgit2