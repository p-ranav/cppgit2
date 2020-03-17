#pragma once
#include <cppgit2/bitmask_operators.hpp>
#include <cppgit2/file_mode.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/ownership.hpp>
#include <cppgit2/strarray.hpp>
#include <cppgit2/tree.hpp>
#include <functional>
#include <git2.h>
#include <string>

namespace cppgit2 {

class index : public libgit2_api {
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
    uint32_t nanoseconds;
  };

  // File entry in index
  // Owned by libgit2
  class entry : public libgit2_api {
  public:
    // Default construct a file entry
    entry() { c_ptr_ = &default_; }

    // Construct from libgit2 C ptr
    // If owned by user, will be free'd in destructor
    entry(git_index_entry *c_ptr) : c_ptr_(c_ptr) {
      if (!c_ptr)
        c_ptr_ = &default_;
    }

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
      index::time result;
      result.seconds = c_ptr_->ctime.seconds;
      result.nanoseconds = c_ptr_->ctime.nanoseconds;
      return result;
    }

    index::time mtime() const {
      index::time result;
      result.seconds = c_ptr_->mtime.seconds;
      result.nanoseconds = c_ptr_->mtime.nanoseconds;
      return result;
    }

    uint32_t dev() const { return c_ptr_->dev; }

    uint32_t ino() const { return c_ptr_->ino; }

    uint32_t mode() const { return c_ptr_->mode; }

    void set_mode(file_mode mode) {
      c_ptr_->mode = static_cast<uint32_t>(mode);
    }

    uint32_t uid() const { return c_ptr_->uid; }

    uint32_t gid() const { return c_ptr_->gid; }

    uint32_t file_size() const { return c_ptr_->file_size; }

    oid id() const { return oid(&c_ptr_->id); }

    void set_id(const oid &id) { c_ptr_->id = *(id.c_ptr()); }

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

    void set_path(const std::string &path) { c_ptr_->path = path.c_str(); }

    // Return the stage number from a git index entry
    // This entry is calculated from the entry's flag attribute like this:
    // (entry->flags & GIT_INDEX_ENTRY_STAGEMASK) >> GIT_INDEX_ENTRY_STAGESHIFT
    int entry_stage() const { return git_index_entry_stage(c_ptr_); }

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

  // Capabilities of system that affect index actions.
  enum class capability {
    ignore_case = 1,
    no_filemode = 2,
    no_symlinks = 4,
    from_owner = -1,
  };

  // Flags for APIs that add files matching pathspec
  enum class add_option {
    default_ = 0,
    force = (1u << 0),
    disable_pathspec_match = (1u << 1),
    check_pathspec = (1u << 2),
  };

  // Git index stage states
  enum class stage {
    any = -1, // Some index APIs take a stage to match; pass this value to match
              // any entry matching the path regardless of stage.
    normal = 0,   // A normal staged file in the index.
    ancestor = 1, // The ancestor side of a conflict.
    ours = 2,     // The "ours" side of a conflict.
    theirs = 3,   // The "theirs" side of a conflict.
  };

  // Add or update an index entry from an in-memory struct
  void add_entry(const entry &source_entry);

  // Add or update index entries matching files in the working directory.
  void add_entries_that_match(
      const std::vector<std::string> &pathspec, add_option flags,
      std::function<int(const std::string &, const std::string &)> callback =
          {});

  // Add or update an index entry from a file on disk
  void add_entry_by_path(const std::string &path);

  // Add or update an index entry from a buffer in memory
  void add_entry_from_buffer(const entry &entry, const std::string &buffer);

  // Read index capabilities flags.
  capability capability_flags() const;

  // This checksum is the SHA-1 hash over the index file
  // (except the last 20 bytes which are the checksum itself).
  const oid checksum();

  // Clear the contents (all the entries) of an index object.
  void clear();

  // Add or update index entries to represent a conflict.
  // Any staged entries that exist at the given paths will be removed.
  void add_conflict_entry(const entry &ancestor_entry, const entry &our_entry,
                          const entry &their_entry);

  // Remove all conflicts in the index (entries with a stage greater than 0).
  void remove_all_conflicts();

  // Removes the index entries that represent a conflict of a single file.
  void remove_conflict_entries(const std::string &path);

  // Get the count of entries currently in the index
  size_t size() const;

  // Find the first position of any entries which point to given path in the Git
  // index.
  size_t find_first(const std::string &path);

  // Find the first position of any entries matching a prefix.
  // To find the first position of a path inside a given folder, suffix the
  // prefix with a '/'.
  size_t find_first_matching_prefix(const std::string &prefix);

  // Run operation for each entry in index
  void for_each(std::function<void(const entry &)> visitor);

  // Run operator for each conflict in index
  void for_each_conflict(
      std::function<void(const entry &, const entry &, const entry &)> visitor);

  // Get one of the entries in the index
  const entry operator[](size_t index);

  // Get one of the entries in the index
  // The entry is not modifiable and should not be freed. Because the
  // git_index_entry struct is a publicly defined struct, you should be able to
  // make your own permanent copy of the data if necessary.
  const entry entry_in_path(const std::string &path, stage stage);

  // Determine if the index contains entries representing file conflicts.
  bool has_conflicts() const;

  // Get the full path to the index file on disk.
  std::string path() const;

  // Update the contents of an existing index object in memory by reading from
  // the hard disk.
  //
  // If force is true, this performs a "hard" read that discards in-memory
  // changes and always reloads the on-disk index data. If there is no on-disk
  // version, the index will be cleared.
  void read(bool force);

  // Read a tree into the index file with stats
  void read_tree(const tree &tree);

  // Remove an entry from the index
  void remove_entry(const std::string &path, stage stage);

  // Remove all matching index entries.
  void remove_entries_that_match(
      const std::vector<std::string> &pathspec,
      std::function<int(const std::string &, const std::string &)> callback =
          {});

  // Remove an index entry corresponding to a file on disk
  void remove_entry_by_path(const std::string &path);

  // Remove all entries from the index under a given directory
  void remove_entries_in_directory(const std::string &dir, stage stage);

  // Set index capabilities flags.
  void set_index_capabilities(capability caps);

  // Set index on-disk version.
  void set_version(unsigned int version);

  // Update all index entries to match the working directory
  // This method will fail in bare index instances.
  //
  // @param pathspec: array of path patterns
  // @param callback: notification callback for each removed path (also gets
  // index of matching pathspec entry); return 0 to add, >0 to skip, < 0 to
  // abort scan.
  //
  // This scans the existing index entries and synchronizes them with the
  // working directory, deleting them if the corresponding working directory
  // file no longer exists otherwise updating the information (including adding
  // the latest version of file to the ODB if needed).
  //
  // If you provide a callback function, it will be invoked on each matching
  // item in the index immediately before it is updated (either refreshed or
  // removed depending on working directory state). Return 0 to proceed with
  // updating the item, > 0 to skip the item, and < 0 to abort the scan.
  void update_entries_that_match(
      const std::vector<std::string> &pathspec,
      std::function<int(const std::string &, const std::string &)> callback =
          {});

  // Get index on-disk version.
  // Valid return values are 2, 3, or 4. If 3 is returned, an index with version
  // 2 may be written instead, if the extension data in version 3 is not
  // necessary.
  unsigned int version() const;

  // Write an existing index object from memory back to disk using an atomic
  // file lock.
  void write();

  // Write the index as a tree
  // The index must not contain any file in conflict.
  oid write_tree();

  // Write the index as a tree to the given repository
  oid write_tree_to(const class repository &repo);

  // Create a new bare Git index object as a memory
  // representation of the Git index file in 'index_path',
  // without a repository to back it.
  static index open(const std::string &path);

  // Owner repository
  class repository owner() const;

  // Access libgit2 C ptr
  const git_index *c_ptr() const;

private:
  friend class pathspec;
  friend class rebase;
  friend class repository;
  git_index *c_ptr_;
  ownership owner_;
};
ENABLE_BITMASK_OPERATORS(index::entry::flag);
ENABLE_BITMASK_OPERATORS(index::entry::extended_flag);
ENABLE_BITMASK_OPERATORS(index::add_option);

} // namespace cppgit2