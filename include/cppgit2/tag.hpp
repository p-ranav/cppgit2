#pragma once
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/object.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/ownership.hpp>
#include <cppgit2/signature.hpp>
#include <git2.h>

namespace cppgit2 {

class tag : public libgit2_api {
public:
  // Default construct a tag
  tag();

  // Construct from libgit2 C ptr
  // If owned by user, this will be free'd in destructor
  tag(git_tag *c_ptr, ownership owner = ownership::libgit2);

  // Cleanup tag ptr
  ~tag();

  // Create an in-memory copy of a tag
  tag copy() const;

  // Id of the tag
  oid id() const;

  // Tag mesage
  std::string message() const;

  // Tag name
  std::string name() const;

  // Recursively peel until a non-tag git_object is found
  object peel() const;

  // Get tagger (author) of this tag
  signature tagger() const;

  // Get the tagged object of this tag
  object target() const;

  // Get the OID of the tagged object
  oid target_id() const;

  // Get the type of a tag's tagged object
  object::object_type target_type() const;

  // Owner repository for this tag
  class repository owner() const;

  // Access to libgit2 C ptr
  git_tag *c_ptr();
  const git_tag *c_ptr() const;

private:
  friend class repository;
  git_tag *c_ptr_;
  ownership owner_;
};

} // namespace cppgit2
