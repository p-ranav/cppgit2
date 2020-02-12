#pragma once
#include <cppgit2/config.hpp>
#include <cppgit2/data_buffer.hpp>
#include <cppgit2/exception.hpp>
#include <cppgit2/index.hpp>
#include <cppgit2/object.hpp>
#include <cppgit2/oid.hpp>
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

  // Check if a repository is bare
  bool is_bare() const;

  // Check if a repository is empty
  // If true, an empty repository has just been initialized and
  // contains no references apart from HEAD, which must be pointing
  // to the unborn master branch.
  bool is_empty() const;

  // Determine if the repository was a shallow clone
  bool is_shallow() const;

  // Check if a repository is a linked work tree
  bool is_worktree() const;

  // Path to the .git folder for normal repos
  // Path to the repo itself for bare repos
  std::string path() const;

  // Path to the shared common dir for this repo
  std::string commondir() const;

  // Config file for this repo
  // If no config file is set, the default config is returned
  cppgit2::config config() const;

  // Get a snapshot of this repo's config
  cppgit2::config config_snapshot() const;

  // Detach the HEAD
  void detach_head();

  // Look for a git repository and return path
  // Method will auto-detect if a repo is bare (if there is a repo)
  static std::string discover_path(const std::string &start_path,
                                   bool across_fs,
                                   const std::string &ceiling_dirs);

  static std::string discover_path(const std::string &start_path);

  // Currently active namespace for this repo
  std::string namespace_() const;

  // Calculate hash of file using repo filtering rules
  oid hashfile(const std::string &path, object::object_type type,
               const std::string &as_path);

  // hashfile overload that uses as_path = path
  oid hashfile(const std::string &path, object::object_type type);

  // Check if repo HEAD is detached
  // A repository's HEAD is detached when it points directly to a commit instead
  // of a branch.
  bool is_head_detached() const;

  // Check if a worktree's HEAD is detached
  // A worktree's HEAD is detached when it points directly to a commit instead
  // of a branch.
  bool is_head_detached_for_worktree(const std::string &path);

  // Check if the current branch is unborn
  // An unborn branch is one named from HEAD but which doesn't exist in the refs
  // namespace, because it doesn't have any commit to point to.
  bool is_head_unborn() const;

  // Retrieve configured identity to use for reflogs
  std::pair<std::string, std::string> identity() const;

  // Get the index file for this repo
  // If a custom index has not been set, the default index for the repository
  // will be returned (the one located in .git/index).
  cppgit2::index index() const;

  // Access to libgit2 C ptr
  const git_repository *c_ptr() const;

private:
  git_repository *c_ptr_;
};

} // namespace cppgit2
