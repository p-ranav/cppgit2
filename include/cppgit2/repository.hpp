#pragma once
#include <cppgit2/ownership.hpp>
#include <git2.h>
#include <string>

namespace cppgit2 {

class repository {
public:
  // Default construct a git repo
  repository();

  // Initialize git repository
  repository(const std::string &path, bool is_bare);

  // Free repository if owned by user
  ~repository();

  // Access to libgit2 C ptr
  const git_repository *c_ptr() const;

private:
  git_repository *c_ptr_;
  ownership owner_;
};

} // namespace cppgit2
