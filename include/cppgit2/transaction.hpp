#pragma once
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/ownership.hpp>
#include <cppgit2/reflog.hpp>
#include <git2.h>
#include <string>

namespace cppgit2 {

class transaction : public libgit2_api {
public:
  // Default construct a transaction
  transaction();

  // Construct from libgit2 C ptr
  // If owned by user, free'd in the destructor
  transaction(git_transaction *c_ptr, ownership owner = ownership::libgit2);

  // Free transaction if owned by user
  ~transaction();

  // Commit changes from this transaction
  void commit();

  // Lock the specified reference
  // This is the first step to updating a reference
  void lock_reference(const std::string &refname);

  // Remove a reference
  void remove_reference(const std::string &refname);

  // Set the specified reference's reflog
  void set_reflog(const std::string &refname, const reflog &reflog);

  // Set the symbolic target of a reference
  void set_symbolic_target(const std::string &refname,
                           const std::string &target,
                           const signature &signature,
                           const std::string &message);

  // Set the target of a reference
  void set_target(const std::string &refname, const oid &target,
                  const signature &signature, const std::string &message);

  // Access the libgit2 C ptr
  git_transaction *c_ptr();
  const git_transaction *c_ptr() const;

private:
  friend class repository;
  git_transaction *c_ptr_;
  ownership owner_;
};

} // namespace cppgit2
