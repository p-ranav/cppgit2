#pragma once
#include <cppgit2/exception.hpp>
#include <git2.h>
#include <string>

namespace cppgit2 {

class oid {
public:
  // Default constructor
  // Initializes libgit2
  oid();

  // Construct from string
  oid(const std::string &hex_string);

  // Construct from string with size `length`
  oid(const std::string &hex_string, size_t length);

  // Construct from C API
  oid(const git_oid *c_ptr);

  // Shuts down libgit2
  ~oid();

  // Compare two oid structions
  //
  // < 0 if oid sorts before rhs
  // 0 if oid matches rhs
  // > 0 if oid sorts after rhs
  int compare(const oid &rhs) const;

  // Compare the first `length` hexadecimal characters
  // (packets of 4 bits) of two oid structures
  int compare(const oid &rhs, size_t length) const;

  // Compare this oid to an hex formatted object id.
  int compare(const std::string &rhs) const;

  // Copy an oid from one structure to another
  oid copy() const;

  // Check if an oid is all zeros
  bool is_zero() const;

  // Compare two oid structures for equality
  bool operator==(const oid &rhs) const;

  // Check if an oid equals a hex formatted object id
  bool operator==(const std::string &rhs) const;

  // Format oid into hex format string
  std::string to_string(size_t n = GIT_OID_HEXSZ) const;

  // Format an oid into a loose-object path string
  //
  // Return string is "aa/...", where "aa" is the first two
  // digits of the oid and "..." is the remaining 38 digits.
  std::string to_path_string() const;

  // Access libgit2 C pointer
  git_oid *c_ptr();
  const git_oid *c_ptr() const;

private:
  git_oid c_struct_;
};

} // namespace cppgit2
