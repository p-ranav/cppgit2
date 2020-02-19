#pragma once
#include <git2.h>
#include <tuple>
#include <cppgit2/exception.hpp>

namespace cppgit2 {

class libgit2_api {
public:
  libgit2_api() {
    git_libgit2_init();
  }

  ~libgit2_api() {
    git_libgit2_shutdown();
  }

  std::tuple<int, int, int> version() const {
    int major, minor, revision;
    if (git_libgit2_version(&major, &minor, &revision))
      throw exception();
    return {major, minor, revision};
  }

};

}