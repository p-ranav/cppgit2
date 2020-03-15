#pragma once
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/ownership.hpp>
#include <cppgit2/signature.hpp>
#include <git2.h>
#include <string>

namespace cppgit2 {

class note : public libgit2_api {
public:
  note();
  note(git_note *c_ptr, ownership owner = ownership::libgit2);
  ~note();

  // Get the note author
  signature author() const;

  // Get the note committer
  signature committer() const;

  // Get the note object's id
  oid id() const;

  // Get the note message
  std::string message() const;

  // Access libgit2 C ptr
  const git_note *c_ptr() const;

private:
  friend class repository;
  git_note *c_ptr_;
  ownership owner_;
};

} // namespace cppgit2