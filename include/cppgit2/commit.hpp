#pragma once
#include <cppgit2/data_buffer.hpp>
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/ownership.hpp>
#include <cppgit2/signature.hpp>
#include <cppgit2/time.hpp>
#include <cppgit2/tree.hpp>
#include <git2.h>
#include <string>

namespace cppgit2 {

class commit : public libgit2_api {
public:
  commit();
  commit(git_commit *c_ptr, ownership owner = ownership::libgit2);
  ~commit();

  // Amend an existing commit by replacing only non-NULL values
  void amend(const oid &id, const std::string &update_ref,
             const signature &author, const signature &committer,
             const std::string &message_encoding, const std::string &message,
             const tree &tree);

  // Author of this commit
  signature author() const;

  // Long "body" of the commit message
  std::string body() const;

  // Committer of this commit
  signature committer() const;

  // Duplicate this commit object
  commit copy() const;

  // Get an arbitrary header field
  std::string operator[](const std::string &field) const;

  // SHA-1 hash of this commit
  oid id() const;

  // Full message of this commit
  std::string message() const;

  // Get the encoding for the message of a commit,
  // as a string representing a standard encoding name
  std::string message_encoding() const;

  // Full raw message of this commit
  std::string message_raw() const;

  // Get the commit ob`ject that  is the < nth gen ancestopr
  // of the named commit object.
  //
  // Returned commit is owned by the caller.
  // Passing 0 returns another instance of the base commit itself.
  commit ancestor(unsigned long n) const;

  // n is the position of the parent (from 0 to `parentcount`)
  commit parent(unsigned int n) const;

  // SHA-1 hash of the parent commit
  // n is the position of the parent (from 0 to `parentcount`)
  oid parent_id(unsigned int n) const;

  // Number of parents of this commit
  unsigned int parent_count() const;

  // Full raw text of this commit's header
  std::string raw_header() const;

  // Get the "short" summary of the git commit message
  std::string summary() const;

  // Commit time (i.e., committer time) of a commit
  epoch_time_seconds time() const;

  // Commit timezone offset (i.e., committer's preferred timezone)
  offset_minutes time_offset() const;

  // Get the tree pointed by this commit
  cppgit2::tree tree() const;

  // SHA-1 has of the tree pointed to by this commit
  // No attempts are made to fetch an object from the ODB
  oid tree_id() const;

  // Get owner repository
  class repository owner() const;

  // Access libgit2 C ptr
  const git_commit *c_ptr() const;

private:
  friend class repository;
  git_commit *c_ptr_;
  ownership owner_;
};

} // namespace cppgit2
