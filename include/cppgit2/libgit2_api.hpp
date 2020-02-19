#pragma once
#include <cppgit2/exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <git2.h>
#include <iostream>
#include <tuple>

namespace cppgit2 {

class libgit2_api {
public:
  libgit2_api() { git_libgit2_init(); }

  ~libgit2_api() { git_libgit2_shutdown(); }

  std::tuple<int, int, int> version() const {
    int major, minor, revision;
    if (git_libgit2_version(&major, &minor, &revision))
      throw exception();
    return {major, minor, revision};
  }
};

} // namespace cppgit2