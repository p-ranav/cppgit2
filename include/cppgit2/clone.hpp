#pragma once
#include <cppgit2/checkout.hpp>
#include <cppgit2/fetch.hpp>
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <git2.h>
#include <string>

namespace cppgit2 {

class clone : public libgit2_api {
public:
  class options : public libgit2_api {
  public:
    options() : c_ptr_(nullptr) {
      auto ret =
          git_clone_init_options(&default_options_, GIT_CLONE_OPTIONS_VERSION);
      c_ptr_ = &default_options_;
      if (ret != 0)
        throw git_exception();
    }

    options(git_clone_options *c_ptr) : c_ptr_(c_ptr) {}

    // Version
    unsigned int version() const { return c_ptr_->version; }
    void set_version(unsigned int version) { c_ptr_->version = version; }

    // Checkout options
    checkout::options checkout_options() const {
      return checkout::options(&c_ptr_->checkout_opts);
    }
    void set_checkout_options(const checkout::options &checkout_options) {
      c_ptr_->checkout_opts = *(checkout_options.c_ptr());
    }

    // Fetch options
    fetch::options fetch_options() const {
      return fetch::options(&c_ptr_->fetch_opts);
    }
    void set_fetch_options(const fetch::options &fetch_options) {
      c_ptr_->fetch_opts = *(fetch_options.c_ptr());
    }

    // Bare?
    bool is_bare() const { return c_ptr_->bare; }
    void set_bare(bool value) { c_ptr_->bare = value; }

    // Options for bypassing the git-aware transport on clone. Bypassing
    // it means that instead of a fetch, libgit2 will copy the object
    // database directory instead of figuring out what it needs, which is
    // faster. If possible, it will hardlink the files to save space.
    enum class local {
      // Auto-detect (default), libgit2 will bypass the git-aware
      // transport for local paths, but use a normal fetch for
      // `file://` urls.
      auto_,

      // Bypass the git-aware transport even for a `file://` url.
      local,

      // Do no bypass the git-aware transport
      no_local,

      // Bypass the git-aware transport, but do not try to use hardlinks
      local_no_links
    };

    // Local
    // Whether to use a fetch or copy the object database.
    local local_option() const {
      return static_cast<clone::options::local>(c_ptr_->local);
    }
    void set_local_option(local option) {
      c_ptr_->local = static_cast<git_clone_local_t>(option);
    }

    // Checkout branch name
    std::string checkout_branch_name() const {
      auto ret = c_ptr_->checkout_branch;
      return ret ? std::string(ret) : "";
    }
    void set_checkout_branch_name(const std::string &checkout_branch_name) {
      c_ptr_->checkout_branch = checkout_branch_name.c_str();
    }

    // Access libgit2 C ptr
    const git_clone_options *c_ptr() const { return c_ptr_; }

  private:
    git_clone_options *c_ptr_;
    git_clone_options default_options_;
  };
};

} // namespace cppgit2