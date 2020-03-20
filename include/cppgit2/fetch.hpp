#pragma once
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/proxy.hpp>
#include <cppgit2/strarray.hpp>
#include <git2.h>
#include <string>
#include <vector>

namespace cppgit2 {

class fetch : public libgit2_api {
public:
  class options : public libgit2_api {
  public:
    options() : c_ptr_(nullptr) {
      auto ret =
          git_fetch_init_options(&default_options_, GIT_FETCH_OPTIONS_VERSION);
      c_ptr_ = &default_options_;
      if (ret != 0)
        throw git_exception();
    }

    options(git_fetch_options *c_ptr) : c_ptr_(c_ptr) {}

    // Version
    unsigned int version() const { return c_ptr_->version; }
    void set_version(unsigned int version) { c_ptr_->version = version; }

    // TODO: Add callbacks to use for this fetch operation

    // Acceptable prune settings when fetching
    enum class prune {
      // Use the setting from the configuration
      unspecified,

      // Force pruning on
      prune,

      // Force pruning off
      no_prune
    };

    // Prune - Whether to perform a prune after the fetch
    prune prune_option() const {
      return static_cast<fetch::options::prune>(c_ptr_->prune);
    }
    void set_prune_option(prune option) {
      c_ptr_->prune = static_cast<git_fetch_prune_t>(option);
    }

    // Update fetchhead
    // Whether to write the results to FETCH_HEAD. Defaults to on. Leave this
    // default in order to behave like git.
    bool update_fetchhead() const { return c_ptr_->update_fetchhead; }
    void set_update_fetchhead(bool value) { c_ptr_->update_fetchhead = value; }

    // Automatic tag following option
    // Lets us select the --tags option to use.
    enum class autotag {
      // Use the setting from the configuration
      unspecified = 0,
      // Ask the server for tags pointing to objects we're already downloading.
      auto_,
      // Don't ask for any tags beyond the refspecs.
      none,
      // Ask for the all the tags.
      all
    };

    // Download tags
    // Determines how to behave regarding tags on the remote, such as
    // auto-downloading tags for objects we're downloading or downloading all of
    // them. The default is to auto-follow tags.
    autotag download_tags_option() const {
      return static_cast<autotag>(c_ptr_->download_tags);
    }
    void set_download_tags_option(autotag download_tags) {
      c_ptr_->download_tags =
          static_cast<git_remote_autotag_option_t>(download_tags);
    }

    // Proxy options
    proxy::options proxy_options() const {
      return proxy::options(&c_ptr_->proxy_opts);
    }
    void set_proxy_options(const proxy::options &options) {
      c_ptr_->proxy_opts = *(options.c_ptr());
    }

    // Custom headers
    strarray custom_headers() const {
      return strarray(&c_ptr_->custom_headers);
    }
    void set_custom_headers(const std::vector<std::string> &headers) {
      c_ptr_->custom_headers = *(strarray(headers).c_ptr());
    }

    // Access libgit2 C ptr
    const git_fetch_options *c_ptr() const { return c_ptr_; }

  private:
    git_fetch_options *c_ptr_;
    git_fetch_options default_options_;
  };
};

} // namespace cppgit2