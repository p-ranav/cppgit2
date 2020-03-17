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

  // A rebase operation
  // Describes a single instruction/operation to be performed during the
  // rebase.
  class operation : libgit2_api {
  public:
    operation() : c_ptr_(nullptr) {}

    operation(git_rebase_operation *c_ptr) : c_ptr_(c_ptr) {}

    // Type of rebase operation in-progress after calling `git_rebase_next`.
    enum class operation_type {
      // The given commit is to be cherry-picked.  The client should commit
      // the changes and continue if there are no conflicts.
      pick = 0,
      // The given commit is to be cherry-picked, but the client should prompt
      // the user to provide an updated commit message.
      reword,
      // The given commit is to be cherry-picked, but the client should stop
      // to allow the user to edit the changes before committing them.
      edit,
      // The given commit is to be squashed into the previous commit. The
      // commit message will be merged with the previous message.
      squash,
      // The given commit is to be squashed into the previous commit. The
      // commit message from this commit will be discarded.
      fixup,
      // No commit will be cherry-picked.  The client should run the given
      // command and (if successful) continue.
      exec
    };

    // The type of rebase operation.
    operation_type type() const {
      return static_cast<operation_type>(c_ptr_->type);
    }

    // The commit ID being cherry-picked. This will be populated for
    // all operations except those of type `GIT_REBASE_OPERATION_EXEC`.
    oid id() const { return oid(&c_ptr_->id); }

    // The executable the user has requested be run. This will only
    // be populated for operations of type `GIT_REBASE_OPERATION_EXEC`.
    std::string exec() const {
      auto ret = c_ptr_->exec;
      return ret ? std::string(ret) : "";
    }

  private:
    friend class rebase;
    git_rebase_operation *c_ptr_;
  };

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

  // Performs the next rebase operation and returns the information about it. If
  // the operation is one that applies a patch (which is any operation except
  // GIT_REBASE_OPERATION_EXEC) then the patch will be applied and the index and
  // working directory will be updated with the changes. If there are conflicts,
  // you will need to address those before committing the changes.
  operation next();

  // Gets the onto id for merge rebases.
  oid onto_id();

  // Gets the onto ref name for merge rebases.
  std::string onto_refname();

  // Gets the rebase operation specified by the given index.
  operation operator[](size_t index);

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