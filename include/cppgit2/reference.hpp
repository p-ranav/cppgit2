#pragma once
#include <cppgit2/bitmask_operators.hpp>
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/object.hpp>
#include <cppgit2/ownership.hpp>
#include <git2.h>
#include <string>

namespace cppgit2 {

class reference : public libgit2_api {
public:
  reference();
  reference(git_reference *c_ptr, ownership owner = ownership::libgit2);
  ~reference();

  // Basic type of any Git reference
  enum class reference_type {
    invalid = 0,  // Invalid reference
    direct = 1,   // A reference that points at an object id
    symbolic = 2, // A reference that points at another reference
    all = direct | symbolic
  };

  // Compare two references.
  // 0 if the same, else a stable but meaningless ordering.
  int compare(const reference &rhs) const;

  // Delete an existing reference.
  static void delete_reference(reference &ref);

  // Create a duplicate of an existing reference
  reference copy() const;

  // Check if this reference ...
  bool is_branch() const;
  bool is_note() const;
  bool is_remote() const;
  bool is_tag() const;

  // Check if a reference name is well-formed.
  // Valid reference names must follow one of two patterns:
  // 1. Top-level names must contain only capital letters and underscores, and
  // must begin and end
  //    with a letter. (e.g. "HEAD", "ORIG_HEAD").
  // 2. Names prefixed with "refs/" can be almost
  //    anything. You must avoid the characters '~', '^', ':', '\', '?', '[',
  //    and '*', and the sequences ".." and "@{" which have special meaning to
  //    revparse.
  static bool is_valid_name(const std::string &refname);

  // Full name of this reference
  std::string name() const;

  // Normalization options for reference lookup
  enum class format {
    normal = 0u,
    allow_onelevel = (1u << 0),
    refspec_pattern = (1u << 1),
    refspec_shorthand = (1u << 2)
  };

  // Normalize reference name and check validity.
  // This will normalize the reference name by removing any leading
  // slash '/' characters and collapsing runs of adjacent
  // slashes between name components into a single slash.
  static std::string normalize_name(size_t length, const std::string &name,
                                    reference::format flags);

  // This will transform the reference name into a name "human-readable"
  // version. If no shortname is appropriate, it will return the full name.
  std::string shorthand_name() const;

  // Get the repository where a reference resides
  class repository owner() const;

  // Recursively peel reference until object of the specified type is found.
  object peel_until(object::object_type type);

  // Rename an existing reference.
  // This method works for both direct and symbolic references.
  // IMPORTANT: The user needs to write a proper reflog entry
  // if the reflog is enabled for the repository. We only rename the reflog if
  // it exists.
  reference rename(const std::string &new_name, bool force,
                   const std::string &log_message);

  // Resolve a sym reference to a direct reference
  // If a direct reference is passed as an argument, a copy of that reference is
  // returned. This copy must be manually freed too.
  reference resolve();

  // Conditionally create a new reference with the same name as
  // the given reference but a different OID target.
  // The reference must be a direct reference, otherwise this will fail.
  reference set_target(const oid &id, const std::string &log_message);

  // Create a new reference with the same name as the given reference
  // but a different symbolic target.
  // The reference must be a symbolic reference, otherwise this will fail.
  reference set_symbolic_target(const std::string &target,
                                const std::string &log_message);

  // Get the OID pointed by a direct reference
  // Only available if the reference is direct (i.e. an object id reference, not
  // a symbolic one).
  //
  // To find the OID of a symbolic ref, call reference.resolve() and then this
  // function (or maybe use reference.name_to_id() to directly resolve a
  // reference name all the way through to an OID).
  oid target() const;

  // Get full name to the reference pointed to by a symbolic reference.
  std::string symbolic_target() const;

  // Return the peeled OID target of this reference
  oid peeled_target() const;

  // Get the type of a reference
  reference_type type() const;

  // Access libgit2 C ptr
  const git_reference *c_ptr() const;

private:
  friend class repository;
  git_reference *c_ptr_;
  ownership owner_;
};
ENABLE_BITMASK_OPERATORS(reference::format);

} // namespace cppgit2