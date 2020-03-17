#pragma once
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/ownership.hpp>
#include <git2.h>
#include <string>

namespace cppgit2 {

class refdb : public libgit2_api {
public:
  // Default constructor
  // Initializes libgit2
  refdb() : owner_(ownership::libgit2), c_ptr_(nullptr) {}

  // Construct from C API
  refdb(git_refdb *c_ptr, ownership owner = ownership::libgit2)
      : c_ptr_(c_ptr), owner_(owner) {}

  // Free if owned by user
  ~refdb() {
    if (c_ptr_ && owner_ == ownership::user)
      git_refdb_free(c_ptr_);
  }

  // Suggests that the given refdb compress or optimize its references.
  // This mechanism is implementation specific. For on-disk reference
  // databases, for example, this may pack all loose references.
  void compress() {
    if (git_refdb_compress(c_ptr_))
      throw git_exception();
  }

private:
  friend class repository;
  ownership owner_;
  git_refdb *c_ptr_;
};

} // namespace cppgit2
