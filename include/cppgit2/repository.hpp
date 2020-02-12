#pragma once
#include <cppgit2/exception.hpp>
#include <git2.h>
#include <string>

namespace cppgit2 {

// Owned by user
class repository {
public:
  // Default construct a git repo
  repository();

  // Initialize git repository
  repository(const std::string &path, bool is_bare);

  // Free git_repository ptr
  ~repository();

  // Open a git repository
  // Auto-detects if `path` is normal or bare repo and fails if neither
  void open(const std::string &path);

  // Fast open for bare repositories
  // Useful when hosting repos and need to access them effeciently
  void open_bare(const std::string &path);

  // Path to the .git folder for normal repos
  // Path to the repo itself for bare repos
  std::string path() const;

  // Access to libgit2 C ptr
  const git_repository *c_ptr() const;

private:
  git_repository *c_ptr_;
};

} // namespace cppgit2
