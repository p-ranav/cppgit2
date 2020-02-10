#pragma once
#include <cppgit2/oid.hpp>
#include <cppgit2/strarray.hpp>
#include <cppgit2/tree.hpp>
#include <cppgit2/ownership.hpp>
#include <functional>
#include <git2.h>
#include <string>

namespace cppgit2 {

class index {
public:
  // Default construct in-memory index object
  // Cannot be read/written to the filesystem
  // May be used to perform in-memory index operations
  index();

  // Construct from libgit2 C ptr
  // If owned by user, will be free'd in the destructor
  index(git_index * c_ptr, ownership owner = ownership::libgit2);

  // Create a new bare index object as a memory representation
  // of the Git index file in `index_path`, without a repo to back it
  index(const std::string &path);

  // Free index object if owned by user
  ~index();

  // Time structure used in a git index entry
  struct time {
    int32_t seconds;
    int32_t nanoseconds;
  };

private:
  git_index * c_ptr_;
  ownership owner_;
};

}