#pragma once
#include <git2.h>
#include <string>

namespace cppgit2 {

class repository {
public:
  // Initialize git repository
  repository(const std::string &path, bool is_bare);

  ~repository();

  // Access to libgit2 C ptr
  const git_repository *c_ptr() const;

private:
  git_repository *c_ptr_;
};

} // namespace cppgit2
