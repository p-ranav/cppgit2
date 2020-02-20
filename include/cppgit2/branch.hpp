#pragma once
#include <cppgit2/libgit2_api.hpp>
#include <git2.h>

namespace cppgit2 {

class branch : public libgit2_api {
public:
  enum class branch_type { local = 1, remote = 2, all = local | remote };
};

} // namespace cppgit2