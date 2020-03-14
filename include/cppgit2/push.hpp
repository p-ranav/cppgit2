#pragma once
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/proxy.hpp>
#include <cppgit2/strarray.hpp>
#include <git2.h>

namespace cppgit2 {

class push : public libgit2_api {
public:
  class options : public libgit2_api {
  public:
    options() {
      auto ret =
          git_push_init_options(&default_options_, GIT_PUSH_OPTIONS_VERSION);
      c_ptr_ = &default_options_;
      if (ret != 0)
        throw git_exception();
    }

    options(git_push_options *c_ptr) : c_ptr_(c_ptr) {}

    // Version
    unsigned int version() const { return c_ptr_->version; }
    void set_version(unsigned int value) { c_ptr_->version = value; }

    // Parallelism
    // If the transport being used to push to the remote
    // requires the creation of a pack file,
    // this controls the number of worker threads used by the packbuilder
    // when creating that pack file to be sent to the remote.
    // If set to 0, the packbuilder will auto-detect the number
    // of threads to create. The default value is 1.
    unsigned int pb_parallelism() const { return c_ptr_->pb_parallelism; }
    void set_pb_parallelism(unsigned int value) {
      c_ptr_->pb_parallelism = value;
    }

    // Proxy options to use, by default no proxy is used.
    proxy::options proxy_options() const {
      return proxy::options(&c_ptr_->proxy_opts);
    }
    void set_proxy_options(const proxy::options &options) {
      c_ptr_->proxy_opts = *(options.c_ptr());
    }

    // Extra headers for this push operation
    strarray custom_headers() const {
      return strarray(&c_ptr_->custom_headers);
    }
    void set_custom_headers(const strarray &headers) {
      c_ptr_->custom_headers = *(headers.c_ptr());
    }

    // Access libgit2 C ptr
    const git_push_options *c_ptr() const { return c_ptr_; }

  private:
    friend push;
    git_push_options *c_ptr_;
    git_push_options default_options_;
  };
};

} // namespace cppgit2