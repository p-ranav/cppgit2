#pragma once
#include <cppgit2/oid.hpp>
#include <cppgit2/ownership.hpp>
#include <cppgit2/strarray.hpp>
#include <cppgit2/tree.hpp>
#include <functional>
#include <git2.h>
#include <string>

namespace cppgit2 {

class index {
public:
  // Default construct in-memory index object
  // Cannot be read/written to the filesystem
  // May be used to perform in-memory index operations
  index();

  // Construct from libgit2 C ptr
  // If owned by user, will be free'd in the destructor
  index(git_index *c_ptr, ownership owner = ownership::libgit2);

  // Create a new bare index object as a memory representation
  // of the Git index file in `index_path`, without a repo to back it
  index(const std::string &path);

  // Free index object if owned by user
  ~index();

  // Time structure used in a git index entry
  struct time {
    int32_t seconds;
    int32_t nanoseconds;
  };

  // File entry in index
  // Owned by libgit2
  class entry {
  public:
    // Default construct a file entry
    entry() : c_ptr_(&default_) { git_libgit2_init(); }

    // Construct from libgit2 C ptr
    // If owned by user, will be free'd in destructor
    entry(git_index_entry *c_ptr) : c_ptr_(c_ptr) {
      if (!c_ptr)
        c_ptr_ = &default_;
      git_libgit2_init();
    }

    // Cleanup file entry
    ~entry() { git_libgit2_shutdown(); }

    // Flags for index entries
    enum class flag {
      extended = (0x4000),
      valid = (0x8000),
    };

    // Bitmasks for on-disk fields of `git_index_entry`'s `flags_extended`
    //
    // In memory, the `flags_extended` fields are divided into two parts: the
    // fields that are read from and written to disk, and other fields that
    // in-memory only and used by libgit2.  Only the flags in
    // `GIT_INDEX_ENTRY_EXTENDED_FLAGS` will get saved on-disk.
    //
    // Thee first three bitmasks match the three fields in the
    // `git_index_entry` `flags_extended` value that belong on disk.  You
    // can use them to interpret the data in the `flags_extended`.
    //
    // The rest of the bitmasks match the other fields in the `git_index_entry`
    // `flags_extended` value that are only used in-memory by libgit2.
    // You can use them to interpret the data in the `flags_extended`.
    enum class extended_flag {
      intent_to_add = (1 << 13),
      skip_worktree = (1 << 14),
      extended_flags = (intent_to_add | skip_worktree),
      uptodate = (1 << 2),
    };

    index::time ctime() const {
      return {.seconds = c_ptr_->ctime.seconds,
              .nanoseconds = c_ptr_->ctime.nanoseconds};
    }

    index::time mtime() const {
      return {.seconds = c_ptr_->mtime.seconds,
              .nanoseconds = c_ptr_->mtime.nanoseconds};
    }

    uint32_t dev() const { return c_ptr_->dev; }

    uint32_t ino() const { return c_ptr_->ino; }

    uint32_t mode() const { return c_ptr_->mode; }

    uint32_t uid() const { return c_ptr_->uid; }

    uint32_t gid() const { return c_ptr_->gid; }

    uint32_t file_size() const { return c_ptr_->file_size; }

    oid id() const { return oid(&c_ptr_->id); }

    flag flags() const { return static_cast<flag>(c_ptr_->flags); }

    extended_flag extended_flags() const {
      return static_cast<extended_flag>(c_ptr_->flags_extended);
    }

    std::string path() const {
      if (c_ptr_->path)
        return std::string(c_ptr_->path);
      else
        return "";
    }

    // Return the stage number from a git index entry
    // This entry is calculated from the entry's flag attribute like this:
    // (entry->flags & GIT_INDEX_ENTRY_STAGEMASK) >> GIT_INDEX_ENTRY_STAGESHIFT
    int get_entry_stage() const { return git_index_entry_stage(c_ptr_); }

    // Return whether the given index entry is a conflict (has a high stage
    // entry). This is simply shorthand for get_entry_stage() > 0.
    bool is_conflict() const { return git_index_entry_is_conflict(c_ptr_); }

    // Access libgit2 C ptr
    const git_index_entry *c_ptr() const { return c_ptr_; }

  private:
    friend class index;
    git_index_entry *c_ptr_;
    git_index_entry default_;
  };

private:
  git_index *c_ptr_;
  ownership owner_;
};

} // namespace cppgit2