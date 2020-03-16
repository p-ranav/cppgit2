#pragma once
#include <cppgit2/data_buffer.hpp>
#include <cppgit2/diff.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/ownership.hpp>
#include <git2.h>
#include <string>

namespace cppgit2 {

class patch : public libgit2_api {
public:
  // Default construct a patch object
  patch();

  // Construct from libgit2 C ptr
  patch(git_refspec *c_ptr, ownership owner = ownership::libgit2);

  // Cleanup patch object
  ~patch();

  // Get the delta associated with a patch.
  // This delta points to internal data, owned by libgit2
  diff::delta delta() const;

  // Access to libgit2 C ptr
  const git_patch *c_ptr() const;

private:
  ownership owner_;
  git_patch *c_ptr_;
};

} // namespace cppgit2