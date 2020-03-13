#pragma once
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/ownership.hpp>
#include <git2.h>

namespace cppgit2 {

class odb : public libgit2_api {
public:
  odb();
  odb(git_odb *c_ptr, ownership owner = ownership::libgit2);
  ~odb();

  // Access libgit2 C ptr
  const git_odb *c_ptr() const;

private:
  friend class repository;
  git_odb *c_ptr_;
  ownership owner_;
};

} // namespace cppgit2
