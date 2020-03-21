#pragma once
#include <cppgit2/git_exception.hpp>
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
      throw git_exception();
    return std::tuple<int, int, int>{major, minor, revision};
  }
};

} // namespace cppgit2