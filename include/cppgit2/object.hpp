#pragma once
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/ownership.hpp>
#include <git2.h>

namespace cppgit2 {

class object : public libgit2_api {
public:
  // Default construct a git object
  object();

  // Construct from libgit2 C ptr
  object(git_object *c_ptr, ownership owner_ = ownership::libgit2);

  // Free git object if owned by user
  ~object();

  // SHA1 hash of this object
  oid id() const;

  // Short abbreviated OID string of this object
  std::string short_id() const;

  // Clone this object
  object copy() const;

  // Basic type (loose or packed) of any git object
  enum class object_type {
    any = -2,
    invalid = -1,
    commit = 1,
    tree = 2,
    blob = 3,
    tag = 4,
    ofs_delta = 6,
    ref_delta = 7
  };

  // object tyoe to string
  static std::string object_type_to_string(object_type type);

  using object_size = git_object_size_t;

  // Recursively peel until an object of the specified type is met
  object peel_until(object_type target);

  // Type of this git object
  object_type type() const;

  // Type of this git object - as string
  std::string type_string() const;

  // Convert a string object type to object_type
  static object_type type_from_string(const std::string &type_string);

  // Convert an object type to string representation
  static std::string string_from_type(object_type type);

  // Determine if the given type is a valid loose object type
  static bool is_type_loose(object_type type);

  // Get owner repository
  class repository owner() const;

  // Cast to blob
  // Throws git_exception if object is not a blob
  class blob as_blob();

  // Cast to commit
  // Throws git_exception if object is not a commit
  class commit as_commit();

  // Cast to tree
  // Throws git_exception if object is not a tree
  class tree as_tree();

  // Cast to tag
  // Throws git_exception if object is not a tag
  class tag as_tag();

  // Access libgit2 C ptr
  const git_object *c_ptr() const { return c_ptr_; }

private:
  friend class reference;
  friend class repository;
  friend class revspec;
  friend class tag;
  ownership owner_;
  git_object *c_ptr_;
};

} // namespace cppgit2
