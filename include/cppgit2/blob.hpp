#pragma once
#include <cppgit2/data_buffer.hpp>
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/oid.hpp>

namespace cppgit2 {

using blob_size = git_off_t;

// Always owned by user
// Free'd in destructor
class blob : public libgit2_api {
public:
  // Default construct a blob
  blob();

  // Construct a blob from libgit2 C ptr
  // Calls git_blob_dup to create a duplicate
  blob(const git_blob *c_ptr);

  // Free blob with git_blob_free
  ~blob();

  // Owner repository
  class repository owner() const;

  // Create an in-memory copy of a blob
  blob copy() const;

  // SHA1 hash for this blob
  oid id() const;

  // Determine if the blob content is binary or not
  bool is_binary() const;

  // Get read-only buffer with raw contents of this blob
  const void *raw_contents() const;

  // Get size in bytes of the contents of this blob
  blob_size raw_size() const;

  // Access libgit2 C ptr
  const git_blob *c_ptr() const;

private:
  friend class repository;
  git_blob *c_ptr_;
};

} // namespace cppgit2
