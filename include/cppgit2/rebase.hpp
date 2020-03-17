#pragma once
#include <cppgit2/index.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/ownership.hpp>
#include <cppgit2/signature.hpp>
#include <git2.h>
#include <string>

namespace cppgit2 {

class rebase : public libgit2_api {
public:
  // Default construct a rebase object
  rebase();

  // Construct from libgit2 C ptr
  rebase(git_rebase *c_ptr, ownership owner = ownership::libgit2);

  // Free rebase object if owned by user
  ~rebase();

  // Aborts a rebase that is currently in progress, resetting the
  // repository and working directory to their state before rebase began.
  void abort();

  // Commits the current patch. You must have resolved any conflicts that
  // were introduced during the patch application from the
  // `git_rebase_next` invocation.
  oid commit(const signature &author, const signature &committer,
             const std::string &message_encoding, const std::string &message);

  // Finishes a rebase that is currently in progress once all patches
  // have been applied.
  void finish(const signature &sig);

  // Gets the index produced by the last operation, which is the result of
  // git_rebase_next and which will be committed by the next invocation of
  // git_rebase_commit. This is useful for resolving conflicts in an in-memory
  // rebase before committing them. You must call git_index_free when you are
  // finished with this.
  cppgit2::index index();

  // Gets the onto id for merge rebases.
  oid onto_id();

  // Gets the onto ref name for merge rebases.
  std::string onto_refname();

  // Gets the index of the rebase operation that is currently being applied. If
  // the first operation has not yet been applied (because you have called init
  // but not yet next) then this returns GIT_REBASE_NO_OPERATION.
  size_t current_operation();

  // Gets the count of rebase operations that are to be applied.
  size_t size();

  // Gets the original HEAD id for merge rebases.
  oid original_head_id();

  // Gets the original HEAD ref name for merge rebases.
  std::string original_head_name();

private:
  friend class repository;
  ownership owner_;
  git_rebase *c_ptr_;
};

} // namespace cppgit2