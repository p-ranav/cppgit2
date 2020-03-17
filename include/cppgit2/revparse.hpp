#pragma once
#include <cppgit2/bitmask_operators.hpp>
#include <git2.h>

namespace cppgit2 {

class revparse {
public:
  // Revparse flags.  These indicate the intended behavior of the spec passed to
  // git_revparse
  enum class mode {
    // The spec targeted a single object.
    single = 1 << 0,

    // The spec targeted a range of commits.
    range = 1 << 1,

    // The spec used the '...' operator, which invokes special semantics.
    merge_base = 1 << 2,
  };
};
ENABLE_BITMASK_OPERATORS(revparse::mode);

} // namespace cppgit2