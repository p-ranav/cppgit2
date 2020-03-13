#pragma once
#include <cppgit2/file_mode.hpp>
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/ownership.hpp>
#include <git2.h>

namespace cppgit2 {

class odb : public libgit2_api {
public:
  // Default construct an odb object
  // Owned by user. Calls git_odb_new
  odb();

  // Construct from libgit2 C ptr
  odb(git_odb *c_ptr, ownership owner = ownership::libgit2);

  // Cleanup odb object if owned by user
  ~odb();

  class backend: public libgit2_api {
  public:
    backend() : c_ptr_(nullptr) {}
    backend(git_odb_backend *c_ptr) : c_ptr_(c_ptr) {}

  private:
    friend class odb;
    git_odb_backend * c_ptr_;
  };

  // Lookup an ODB backend object by index
  backend operator[](size_t index) const;

  // Create a backend for loose objects
  static backend create_backend_for_loose_objects(const std::string &objects_dir, 
    int compression_level, bool do_fsync, unsigned int dir_mode, file_mode mode);

  // Create a backend out of a single packfile
  // index_file is the path to the packfile's .idx file
  static backend create_backend_for_one_packfile(const std::string &index_file);

  // Create a backend for the packfiles.
  // objects_dir is the Git repository's objects directory
  static backend create_backend_for_packfiles(const std::string &objects_dir);

  // Access libgit2 C ptr
  const git_odb *c_ptr() const;

private:
  friend class repository;
  git_odb *c_ptr_;
  ownership owner_;
};

} // namespace cppgit2
