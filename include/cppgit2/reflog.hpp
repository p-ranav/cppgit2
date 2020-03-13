#pragma once
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/ownership.hpp>
#include <cppgit2/signature.hpp>
#include <git2.h>

namespace cppgit2 {

class reflog : public libgit2_api {
public:
  // Default construct a reflog
  reflog();

  // Construct reflog from libgit2 C ptr
  // If owned by user, will be free'd in the destructor
  // using git_reflog_free
  reflog(git_reflog *c_ptr, ownership owner = ownership::libgit2);

  // Free reflog if owned by user
  ~reflog();

  // Entry in this reflog
  class entry : public libgit2_api {
  public:
    entry(const git_reflog_entry *c_ptr) : c_ptr_(c_ptr) {}

    signature committer() {
      return signature(git_reflog_entry_committer(c_ptr_));
    }

    std::string message() {
      auto ret = git_reflog_entry_message(c_ptr_);
      if (ret)
        return std::string(ret);
      else
        return "";
    }

    oid new_oid() { return oid(git_reflog_entry_id_new(c_ptr_)); }

    oid old_oid() { return oid(git_reflog_entry_id_old(c_ptr_)); }

    const git_reflog_entry *c_ptr() { return c_ptr_; }

  private:
    const git_reflog_entry *c_ptr_;
  };

  // Remove entry from reflog by index
  // To ensure there's no gap in the log history,
  // set rewrite_previous_entry to true
  void remove(size_t index, bool rewrite_previous_entry);

  // Get the number of log entries in this reflog
  size_t size() const;

  // Add a new entry to the in-memory reflog
  void append(const oid &id, const signature &committer,
              const std::string &message = "");

  // Lookup an entry by index
  // Requesting reflog[0] will return the most recently created entry
  entry operator[](size_t index) const;

  // Write an existing in-memory reflog object back to disk
  // Uses atomic file lock
  void write_to_disk();

  // Access libgit2 C ptr
  const git_reflog *c_ptr() const;

private:
  friend class repository;
  git_reflog *c_ptr_;
  ownership owner_;
};

} // namespace cppgit2
