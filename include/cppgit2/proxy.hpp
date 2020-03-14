#pragma once
#include <cppgit2/libgit2_api.hpp>
#include <git2.h>

namespace cppgit2 {

class proxy : public libgit2_api {
public:
  // The type of proxy to use.
  enum class proxy_type {
    none,     // Do not attempt to connect through a proxy
    auto_,    // Try to auto-detect the proxy from the git configuration.
    specified // Connect via the URL given in the options
  };

  class options : public libgit2_api {
  public:
    options() {
      auto ret =
          git_proxy_init_options(&default_options_, GIT_PROXY_OPTIONS_VERSION);
      c_ptr_ = &default_options_;
      if (ret != 0)
        throw git_exception();
    }

    options(git_proxy_options *c_ptr) : c_ptr_(c_ptr) {}

    // Version
    unsigned int version() const { return c_ptr_->version; }
    void set_version(unsigned int value) { c_ptr_->version = value; }

    // The type of proxy to use, by URL, auto-detect.
    proxy::proxy_type proxy_type() const {
      return static_cast<proxy::proxy_type>(c_ptr_->type);
    }
    void set_proxy_type(proxy::proxy_type type) {
      c_ptr_->type = static_cast<git_proxy_t>(type);
    }

    // URL
    std::string url() const {
      if (c_ptr_->url)
        return std::string(c_ptr_->url);
      return "";
    }
    void set_url(const std::string &url) { c_ptr_->url = url.c_str(); }

    // Access libgit2 C ptr
    const git_proxy_options *c_ptr() const { return c_ptr_; }

  private:
    friend proxy;
    git_proxy_options *c_ptr_;
    git_proxy_options default_options_;
  };
};

} // namespace cppgit2