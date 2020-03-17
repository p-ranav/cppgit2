#pragma once
#include <git2.h>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/ownership.hpp>

namespace cppgit2 {

class rebase : public libgit2_api {
public:
  // Default construct a rebase object
  rebase();

  // Construct from libgit2 C ptr
  rebase(git_rebase * c_ptr, ownership owner = ownership::libgit2);

  // Free rebase object if owned by user
  ~rebase();

  

private:
  friend class repository;
  ownership owner_;
  git_rebase * c_ptr_;
};

}