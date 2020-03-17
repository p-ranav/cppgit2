#pragma once
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/object.hpp>
#include <cppgit2/revparse.hpp>
#include <git2.h>

namespace cppgit2 {

// Git Revision Spec: output of a `git_revparse` operation
// Contents are owned by the user
class revspec : public libgit2_api {
public:
  // Default construct a revspec
  revspec() : c_ptr_(nullptr) {}

  // Construct from libgit2 C ptr
  revspec(git_revspec *c_ptr) : c_ptr_(c_ptr) {
    from_ = object(c_ptr_->from, ownership::user);
    to_ = object(c_ptr_->to, ownership::user);
  }

  // The left element of the revspec; must be freed by the user
  object from() const { return from_; }

  // The right element of the revspec; must be freed by the user
  object to() const { return to_; }

  revparse::mode flags() const {
    return static_cast<revparse::mode>(c_ptr_->flags);
  }

  const git_revspec *c_ptr() const { return c_ptr_; }

private:
  friend class repository;
  object from_;
  object to_;
  git_revspec *c_ptr_;
};

} // namespace cppgit2