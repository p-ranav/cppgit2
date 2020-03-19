#pragma once
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <git2.h>
#include <string>

namespace cppgit2 {

class oid : public libgit2_api {
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

  // Construct from raw bytes
  oid(const unsigned char *raw);

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
  std::string to_hex_string(size_t n = GIT_OID_HEXSZ) const;

  // Format an oid into a loose-object path string
  //
  // Return string is "aa/...", where "aa" is the first two
  // digits of the oid and "..." is the remaining 38 digits.
  std::string to_path_string() const;

  // Access libgit2 C pointer
  git_oid *c_ptr();
  const git_oid *c_ptr() const;

  // The OID shortener is used to process a list of OIDs
  // in text form and return the shortest length that
  // would uniquely identify all of them.
  //
  // e.g. look at the result of git log --abbrev.
  class shortener : public libgit2_api {
  public:
    // The minimal length for all identifiers,
    // which will be used even if shorter
    // OIDs would still be unique.
    explicit shortener(size_t min_length = 0) {
      c_ptr_ = git_oid_shorten_new(min_length);
    }

    ~shortener() {
      if (c_ptr_)
        git_oid_shorten_free(c_ptr_);
    }

    // Add a new OID to set of shortened OIDs
    // Returns the minimal length to uniquely identify
    // all the OIDs in the set.
    //
    // For performance reasons, there is a hard-limit of
    // how many OIDs can be added to a single set
    // (around ~32000, assuming a mostly randomized distribution)
    int add(const std::string &text_id) {
      auto result = git_oid_shorten_add(c_ptr_, text_id.c_str());
      if (result < 0)
        throw git_exception();
      return result;
    }

  private:
    git_oid_shorten *c_ptr_;
  };

private:
  friend class repository;
  git_oid c_struct_;
};

} // namespace cppgit2
