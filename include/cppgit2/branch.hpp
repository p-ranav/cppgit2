#pragma once
#include <git2.h>
#include <cppgit2/libgit2_api.hpp>

namespace cppgit2 {

class branch: public libgit2_api {
public:
  enum class branch_type {
    local = 1,
    remote = 2,
    all = local | remote
  };
};

}