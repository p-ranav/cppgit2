#pragma once
#include <cppgit2/bitmask_operators.hpp>
#include <cppgit2/blob.hpp>
#include <cppgit2/data_buffer.hpp>
#include <cppgit2/diff.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/ownership.hpp>
#include <cppgit2/strarray.hpp>
#include <git2.h>
#include <string>
#include <utility>
#include <vector>

namespace cppgit2 {

class diff : public libgit2_api {
public:
  diff();
  diff(git_diff *c_ptr, ownership owner = ownership::libgit2);
  ~diff();

  class delta : public libgit2_api {
  public:
    delta() {}
    delta(const git_diff_delta *c_ptr) {
      if (c_ptr)
        c_struct_ = *c_ptr;
    }
    ~delta() {}

    // Flags for the delta object and the file objects on each side.
    enum class flag {
      binary = (1u << 0),     // file(s) treated as binary data
      not_binary = (1u << 1), // file(s) treated as text data
      valid_id = (1u << 2),   // `id` value is known correct
      exists = (1u << 3),     // file exists at this side of the delta
    };

    // What type of change is described by this delta?
    enum class type {
      unmodified = 0, // no change
      added = 1,      // entry does not exist in old version
      deleted = 2,    // entry does not exist in new version
      modified = 3,   // entry content changed between old and new
      renamed = 4,    // entry was renamed between old and new
      copied = 5,     // entry was copied from another old entry
      ignored = 6,    // entry is ignored item in workdir
      untracked = 7,  // entry is untracked item in workdir
      typechange = 8, // type of entry changed between old and new
      unreadable = 9, // entry is unreadable
      conflicted = 10 // entry in the index is conflicted
    };

    class file : public libgit2_api {
    public:
      // The `id` is the `git_oid` of the item.
      oid id() const { return oid(c_struct_.id.id); }

      // `path` is relative to the working directory of the repository.
      std::string path() const {
        auto result_c = c_struct_.path;
        if (result_c)
          return std::string(result_c);
        else
          return "";
      }

      // `size` is the size of the entry in bytes.
      uint64_t size() const { return c_struct_.size; }

      // `flags` is a combination of the diff::flag types
      uint32_t flags() const { return c_struct_.flags; }

      // `mode` is, roughly, the stat() `st_mode` value for the item.
      uint16_t mode() const { return c_struct_.mode; }

      // The `id_abbrev` represents the known length of the `id` field, when
      // converted to a hex string. It is generally `GIT_OID_HEXSZ`, unless this
      // delta was created from reading a patch file, in which case it may be
      // abbreviated to something reasonable, like 7 characters.
      uint16_t abbreviated_id() const { return c_struct_.id_abbrev; }

    private:
      friend diff;
      git_diff_file c_struct_;
    };

    type status() const { return static_cast<type>(c_struct_.status); }

    uint32_t flags() const { return c_struct_.flags; }

    uint16_t similarity() const { return c_struct_.similarity; }

    uint16_t nfiles() const { return c_struct_.nfiles; }

    file old_file() const {
      file result;
      result.c_struct_ = c_struct_.old_file;
      return result;
    }

    file new_file() const {
      file result;
      result.c_struct_ = c_struct_.new_file;
      return result;
    }

  private:
    friend diff;
    git_diff_delta c_struct_;
  };

  class options : public libgit2_api {
  public:
    options() {
      auto ret =
          git_diff_init_options(&default_options_, GIT_DIFF_OPTIONS_VERSION);
      c_ptr_ = &default_options_;
      if (ret != 0)
        throw git_exception();
    }

    options(git_diff_options *c_ptr) : c_ptr_(c_ptr) {}

    // Flags for diff options.  A combination of these flags can be passed
    // in via the `flags` value in the `git_diff_options`
    enum class flag {
      // Normal diff, the default
      normal = 0,

      // Options controlling which files will be in the diff

      // Reverse the sides of the diff
      reverse = (1u << 0),
      // Include ignored files in the diff
      include_ignored = (1u << 1),
      // Even with `include_ignored`, an entire ignored directory
      // will be marked with only a single entry in the diff; this flag
      // adds all files under the directory as IGNORED entries, too.
      recurse_ignored_dirs = (1u << 2),
      // Include untracked files in the diff
      include_untracked = (1u << 3),
      // Even with `include_untracked`, an entire untracked
      // directory will be marked with only a single entry in the diff
      // (a la what core Git does in `git status`); this flag adds *all*
      // files under untracked directories as UNTRACKED entries, too.
      recurse_untracked_dirs = (1u << 4),
      // Include unmodified files in the diff
      include_unmodified = (1u << 5),
      // Normally, a type change between files will be converted into a
      // DELETED record for the old and an ADDED record for the new; this
      // options enabled the generation of TYPECHANGE delta records.
      include_typechange = (1u << 6),
      // Even with `include_typechange`, blob->tree changes still
      // generally show as a DELETED blob.  This flag tries to correctly
      // label blob->tree transitions as TYPECHANGE records with new_file's
      // mode set to tree.  Note: the tree SHA will not be available.
      include_typechange_trees = (1u << 7),
      // Ignore file mode changes
      ignore_filemode = (1u << 8),
      // Treat all submodules as unmodified
      ignore_submodules = (1u << 9),
      // Use case insensitive filename comparisons
      ignore_case = (1u << 10),
      // May be combined with `ignore_case` to specify that a file
      // that has changed case will be returned as an add/delete pair.
      include_casechange = (1u << 11),
      // If the pathspec is set in the diff options, this flags indicates
      // that the paths will be treated as literal paths instead of
      // fnmatch patterns.  Each path in the list must either be a full
      // path to a file or a directory.  (A trailing slash indicates that
      // the path will _only_ match a directory).  If a directory is
      // specified, all children will be included.
      disable_pathspec_match = (1u << 12),
      // Disable updating of the `binary` flag in delta records.  This is
      // useful when iterating over a diff if you don't need hunk and data
      // callbacks and want to avoid having to load file completely.
      skip_binary_check = (1u << 13),
      // When diff finds an untracked directory, to match the behavior of
      // core Git, it scans the contents for IGNORED and UNTRACKED files.
      // If *all// contents are IGNORED, then the directory is IGNORED; if
      // any contents are not IGNORED, then the directory is UNTRACKED.
      // This is extra work that may not matter in many cases.  This flag
      // turns off that scan and immediately labels an untracked directory
      // as UNTRACKED (changing the behavior to not match core Git).
      enable_fast_untracked_dirs = (1u << 14),
      // When diff finds a file in the working directory with stat
      // information different from the index, but the OID ends up being the
      // same, write the correct stat information into the index.  Note:
      // without this flag, diff will always leave the index untouched.
      update_index = (1u << 15),
      // Include unreadable files in the diff
      include_unreadable = (1u << 16),
      // Include unreadable files in the diff
      include_unreadable_as_untracked = (1u << 17),

      // Options controlling how output will be generated

      // Use a heuristic that takes indentation and whitespace into account
      // which generally can produce better diffs when dealing with ambiguous
      // diff hunks.
      indent_heuristic = (1u << 18),
      // Treat all files as text, disabling binary attributes & detection
      force_text = (1u << 20),
      // Treat all files as binary, disabling text diffs
      force_binary = (1u << 21),
      // Ignore all whitespace
      ignore_whitespace = (1u << 22),
      // Ignore changes in amount of whitespace
      ignore_whitespace_change = (1u << 23),
      // Ignore whitespace at end of line
      ignore_whitespace_eol = (1u << 24),
      // When generating patch text, include the content of untracked
      // files.  This automatically turns on GIT_DIFF_INCLUDE_UNTRACKED but
      // it does not turn on GIT_DIFF_RECURSE_UNTRACKED_DIRS.  Add that
      // flag if you want the content of every single UNTRACKED file.
      show_untracked_content = (1u << 25),
      // When generating output, include the names of unmodified files if
      // they are included in the git_diff.  Normally these are skipped in
      // the formats that list files (e.g. name-only, name-status, raw).
      // Even with this, these will not be included in patch format.
      show_unmodified = (1u << 26),
      // Use the "patience diff" algorithm
      patience = (1u << 28),
      // Take extra time to find minimal diff
      minimal = (1u << 29),
      // Include the necessary deflate / delta information so that `git-apply`
      // can apply given diff information to binary files.
      show_binary = (1u << 30),
    };

    unsigned int version() const { return c_ptr_->version; }

    void set_version(unsigned int value) { c_ptr_->version = value; }

    flag flags() const { return static_cast<flag>(c_ptr_->flags); }

    void set_flags(flag value) { c_ptr_->flags = static_cast<uint32_t>(value); }

    std::vector<std::string> pathspec() const {
      return strarray(&c_ptr_->pathspec).to_vector();
    }

    void set_pathspec(const std::vector<std::string> &value) {
      c_ptr_->pathspec = *(strarray(value).c_ptr());
    }

    uint32_t context_lines() const { return c_ptr_->context_lines; }

    void set_context_lines(uint32_t value) { c_ptr_->context_lines = value; }

    uint32_t interhunk_lines() const { return c_ptr_->interhunk_lines; }

    void set_interhunk_lines(uint32_t value) {
      c_ptr_->interhunk_lines = value;
    }

    uint16_t id_abbrev() const { return c_ptr_->id_abbrev; }

    void set_id_abbrev(uint16_t value) { c_ptr_->id_abbrev = value; }

    uint64_t max_size() const { return c_ptr_->max_size; }

    void set_max_size(uint64_t value) { c_ptr_->max_size = value; }

    std::string old_prefix() const {
      if (c_ptr_->old_prefix)
        return std::string(c_ptr_->old_prefix);
      else
        return "";
    }

    void set_old_prefix(const std::string &value) {
      c_ptr_->old_prefix = value.c_str();
    }

    std::string new_prefix() const {
      if (c_ptr_->new_prefix)
        return std::string(c_ptr_->new_prefix);
      else
        return "";
    }

    void set_new_prefix(const std::string &value) {
      c_ptr_->new_prefix = value.c_str();
    }

    const git_diff_options *c_ptr() const { return c_ptr_; }

  private:
    friend diff;
    git_diff_options *c_ptr_;
    git_diff_options default_options_;
  };

  // Directly run a diff on two blobs
  // Each pair is {Blob object, Treat as if it had this filename}
  static delta compare_files(const std::pair<blob, std::string> &old_file,
                             const std::pair<blob, std::string> &new_file,
                             diff::options options = diff::options());

  // Diff delta for an entry in the diff list
  const delta operator[](size_t index) const;

  // Check if deltas are sorted case sensitively or insensitively
  bool is_sorted_case_sensitive() const;

  // Merge one diff into another
  // This merges items from the `from` list into this list. The resulting diff
  // will have all items that appear in either list.
  void merge(const diff &from);

  // Query how many diff records are there in a diff
  size_t size() const;

  // Query how many diff deltas are there in a diff filtered by type.
  size_t size(delta::type type) const;

  // Look up the single character abbreviation for a delta status code.
  // When you run git diff --name-status it uses single letter codes in the
  // output such as 'A' for added, 'D' for deleted, 'M' for modified, etc.
  char status_char(delta::type status) const;

  // Possible output formats for diff
  enum class format {
    patch = 1u,        // full git diff
    patch_header = 2u, // just the file headers of patch
    raw = 3u,          // like git diff --raw
    name_only = 4u,    // like git diff --name-only
    name_status = 5u,  // like git diff --name-status
    patch_id = 6u      // git diff as used by git patch-id
  };

  // Produce the complete formatted text output from a diff into a buffer.
  std::string to_string(diff::format format_type) const;

  // Access libgit2 C ptr
  const git_diff *c_ptr() const;

private:
  friend class patch;
  friend class pathspec;
  friend class repository;
  git_diff *c_ptr_;
  ownership owner_;
};
ENABLE_BITMASK_OPERATORS(diff::delta::flag);
ENABLE_BITMASK_OPERATORS(diff::options::flag);

} // namespace cppgit2