#pragma once
#include <cppgit2/exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/oid.hpp>
#include <git2.h>

namespace cppgit2 {

class object : public libgit2_api {
public:
  // Default construct a git object
  object();

  // Construct from libgit2 C ptr
  object(const git_object *c_ptr);

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

  git_object *c_ptr() { return c_ptr_; }

  const git_object *c_ptr() const { return c_ptr_; }

  git_object **c_ptr_ptr() { return &c_ptr_; }

private:
  friend class reference;
  git_object *c_ptr_;
};

} // namespace cppgit2
