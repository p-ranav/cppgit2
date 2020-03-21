#pragma once
#include <cppgit2/bitmask_operators.hpp>
#include <cppgit2/blob.hpp>
#include <cppgit2/data_buffer.hpp>
#include <cppgit2/diff.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/ownership.hpp>
#include <cppgit2/signature.hpp>
#include <cppgit2/strarray.hpp>
#include <functional>
#include <git2.h>
#include <string>
#include <utility>
#include <vector>

namespace cppgit2 {

class diff : public libgit2_api {
public:
  // Default construct a diff object
  diff();

  // Construct from libgit2 C ptr
  diff(git_diff *c_ptr, ownership owner = ownership::libgit2);

  // Read the contents of a git patch file into a git_diff object.
  // The diff object produced is similar to the one that would be produced if
  // you actually produced it computationally by comparing two trees, however
  // there may be subtle differences. For example, a patch file likely contains
  // abbreviated object IDs, so the object IDs in a git_diff_delta produced by
  // this function will also be abbreviated.
  diff(const std::string &buffer);

  // Cleanup diff
  // Free if owned by user
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

  // This is an opaque structure which is allocated by `git_diff_get_stats`. You
  // are responsible for releasing the object memory when done, using the
  // `git_diff_stats_free()` function.
  //
  // Owned by user
  class stats : public libgit2_api {
  public:
    stats(git_diff_stats *c_ptr) : c_ptr_(c_ptr) {}

    ~stats() {
      if (c_ptr_)
        git_diff_stats_free(c_ptr_);
    }

    // Get the total number of deletions in a diff
    size_t deletions() const { return git_diff_stats_deletions(c_ptr_); }

    // Get the total number of files changed in a diff
    size_t files_changed() const {
      return git_diff_stats_files_changed(c_ptr_);
    }

    // Get the total number of insertions in a diff
    size_t insertions() const { return git_diff_stats_insertions(c_ptr_); }

    // Formatting options for diff stats
    enum class format {
      // No stats
      none = 0,
      // Full statistics, equivalent of `--stat`
      full = (1u << 0),
      // Short statistics, equivalent of `--shortstat`
      short_ = (1u << 1),
      // Number statistics, equivalent of `--numstat`
      number = (1u << 2),
      // Extended header information such as creations, renames and mode
      // changes, equivalent of `--summary`
      include_summary = (1u << 3)
    };

    // Print diff statistics to a git_buf.
    data_buffer to_buffer(format format, size_t width) {
      data_buffer result;
      if (git_diff_stats_to_buf(result.c_ptr(), c_ptr_,
                                static_cast<git_diff_stats_format_t>(format),
                                width))
        throw git_exception();
      return result;
    }

  private:
    friend diff;
    git_diff_stats *c_ptr_;
  };

  // Accumulate diff statistics for all patches.
  stats diff_stats() const;

  // Formatting options for diff e-mail generation
  enum class format_email_flag {
    // Normal patch, the default
    none = 0,
    // Don't insert "[PATCH]" in the subject header
    exclude_subject_patch_marker = (1 << 0)
  };

  // Options for controlling the formatting of the generated e-mail.
  class format_email_options : public libgit2_api {
  public:
    format_email_options() : c_ptr_(nullptr) {
      auto ret = git_diff_format_email_init_options(
          &default_options_, GIT_DIFF_FORMAT_EMAIL_OPTIONS_VERSION);
      c_ptr_ = &default_options_;
      if (ret != 0)
        throw git_exception();
    }

    format_email_options(git_diff_format_email_options *c_ptr)
        : c_ptr_(c_ptr) {}

    // Version
    unsigned int version() const { return c_ptr_->version; }
    void set_version(unsigned int version) { c_ptr_->version = version; }

    // Flags
    format_email_flag flags() const {
      return static_cast<format_email_flag>(c_ptr_->flags);
    }
    void set_flags(format_email_flag flags) {
      c_ptr_->flags = static_cast<uint32_t>(flags);
    }

    // This patch number
    size_t patch_no() const { return c_ptr_->patch_no; }
    void set_patch_no(size_t value) { c_ptr_->patch_no = value; }

    // Total number of patches in this series
    size_t total_patches() const { return c_ptr_->total_patches; }
    void set_total_patches(size_t value) { c_ptr_->total_patches = value; }

    // id to use for the commit
    oid id() const { return oid(c_ptr_->id); }
    void set_id(const oid &id) { c_ptr_->id = id.c_ptr(); }

    // Summary of the change
    std::string summary() const {
      return c_ptr_->summary ? std::string(c_ptr_->summary) : "";
    }
    void set_summary(const std::string &value) {
      c_ptr_->summary = value.c_str();
    }

    // Commit message's body
    std::string body() const {
      return c_ptr_->body ? std::string(c_ptr_->body) : "";
    }
    void set_body(const std::string &value) { c_ptr_->body = value.c_str(); }

    // Author of the change
    signature author() const { return signature(c_ptr_->author); }
    void set_author(const signature &sig) { c_ptr_->author = sig.c_ptr(); }

    // Access libgit2 C ptr
    const git_diff_format_email_options *c_ptr() const { return c_ptr_; }

  private:
    git_diff_format_email_options *c_ptr_;
    git_diff_format_email_options default_options_;
  };

  // Create an e-mail ready patch from a diff.
  data_buffer
  format_email(const format_email_options &options = format_email_options());

  // When producing a binary diff, the binary data returned will be
  // either the deflated full ("literal") contents of the file, or
  // the deflated binary delta between the two sides (whichever is
  // smaller).
  enum class binary_data_type {
    none,    // There is no binary delta.
    literal, // The binary data is the literal contents of the file.
    delta,   // The binary data is the delta from one side to the other.
  };

  // The contents of one of the files in a binary diff.
  class binary_file : public libgit2_api {
  public:
    binary_file(const git_diff_binary_file *c_ptr) : c_struct_(*c_ptr) {}

    // The type of binary data for this file
    binary_data_type type() const {
      return static_cast<binary_data_type>(c_struct_.type);
    }

    // The binary data, deflated.
    const char *data() const { return c_struct_.data; }

    // The length of the binary data.
    size_t data_length() const { return c_struct_.datalen; }

    // The length of the binary data after inflation.
    size_t inflated_length() const { return c_struct_.inflatedlen; }

    // Access libgit2 C ptr
    const git_diff_binary_file *c_ptr() const { return &c_struct_; }

  private:
    git_diff_binary_file c_struct_;
  };

  // Structure describing the binary contents of a diff.
  //
  // A `binary` file / delta is a file (or pair) for which no text diffs
  // should be generated. A diff can contain delta entries that are
  // binary, but no diff content will be output for those files. There is
  // a base heuristic for binary detection and you can further tune the
  // behavior with git attributes or diff flags and option settings.
  class binary : public libgit2_api {
  public:
    binary(const git_diff_binary *c_ptr) : c_struct_(*c_ptr) {}

    // Whether there is data in this binary structure or not.
    //
    // If this is `true`, then this was produced and included binary content.
    // If this is `false` then this was generated knowing only that a binary
    // file changed but without providing the data, probably from a patch
    // that said `Binary files a/file.txt and b/file.txt differ`.
    bool contains_data() const { return c_struct_.contains_data; }

    // The contents of the old file.
    binary_file old_file() const { return binary_file(&c_struct_.old_file); }

    // The contents of the new file.
    binary_file new_file() const { return binary_file(&c_struct_.new_file); }

    // Access libgit2 C ptr
    const git_diff_binary *c_ptr() const { return &c_struct_; }

  private:
    git_diff_binary c_struct_;
  };

  // Structure describing a line (or data span) of a diff.
  //
  // A line is a range of characters inside a hunk. It could be a context line
  // (i.e. in both old and new versions), an added line (i.e. only in the new
  // version), or a removed line (i.e. only in the old version). Unfortunately,
  // we don't know anything about the encoding of data in the file being diffed,
  // so we cannot tell you much about the line content. Line data will not be
  // NUL-byte terminated, however, because it will be just a span of bytes
  // inside the larger file.
  class line : public libgit2_api {
  public:
    line(const git_diff_line *c_ptr) : c_struct_(*c_ptr) {}

    // A git_diff_line_t value
    char origin() const { return c_struct_.origin; }

    // Line number in old file or -1 for added line
    int old_lineno() const { return c_struct_.old_lineno; }

    // Line number in new file or -1 for deleted line
    int new_lineno() const { return c_struct_.new_lineno; }

    // Number of newline characters in content
    int num_lines() const { return c_struct_.num_lines; }

    // Number of bytes of data
    size_t content_length() const { return c_struct_.content_len; }

    // Offset in the original file to the content
    git_off_t content_offset() const { return c_struct_.content_offset; }

    // Pointer to diff text, not NUL-byte terminated
    const char *content() const { return c_struct_.content; }

    // Access libgit2 C ptr
    const git_diff_line *c_ptr() const { return &c_struct_; }

  private:
    git_diff_line c_struct_;
  };

  // Structure describing a hunk of a diff.
  //
  // A hunk is a span of modified lines in a delta along with some stable
  // surrounding context. You can configure the amount of context and other
  // properties of how hunks are generated. Each hunk also comes with a header
  // that described where it starts and ends in both the old and new versions in
  // the delta.
  class hunk : public libgit2_api {
  public:
    hunk(const git_diff_hunk *c_ptr) : c_struct_(*c_ptr) {}

    // Starting line number in old_file
    int old_start() const { return c_struct_.old_start; }

    // Number of lines in old_file
    int old_lines() const { return c_struct_.old_lines; }

    // Starting line number in new_file
    int new_start() const { return c_struct_.new_start; }

    // Number of lines in new_file
    int new_lines() const { return c_struct_.new_lines; }

    // Number of bytes in header text
    size_t header_length() const { return c_struct_.header_len; }

    // Header text, NUL-byte terminated
    char const *header() const { return &(c_struct_.header[0]); }

    // Access libgit2 C ptr
    const git_diff_hunk *c_ptr() const { return &c_struct_; }

  private:
    git_diff_hunk c_struct_;
  };

  // Loop over all deltas in a diff issuing callbacks.
  //
  // @param file_callback: Callback function to make per file in the diff.
  // @param binary_callback: Optional callback to make for binary files.
  // @param hunk_callback: Optional callback to make per hunk of text diff. This
  // callback is called to describe a range of lines in the diff. It will not be
  // issued for binary files.
  // @param line_callback: Optional callback to make per line of diff text. This
  // same callback will be made for context lines, added, and removed lines, and
  // even for a deleted trailing newline.
  //
  // This will iterate through all of the files described in a diff. You should
  // provide a file callback to learn about each file.
  //
  // The "hunk" and "line" callbacks are optional, and the text diff of the
  // files will only be calculated if they are not NULL. Of course, these
  // callbacks will not be invoked for binary files on the diff or for files
  // whose only changed is a file mode change.
  void for_each(std::function<void(const diff::delta &, float)> file_callback,
                std::function<void(const diff::delta &, const diff::binary &)>
                    binary_callback = {},
                std::function<void(const diff::delta &, const diff::hunk &)>
                    hunk_callback = {},
                std::function<void(const diff::delta &, const diff::hunk &,
                                   const diff::line &)>
                    line_callback = {});

  // Iterate over a diff generating formatted text output.
  void print(diff::format format,
             std::function<void(const diff::delta &, const diff::hunk &,
                                const diff::line &)>
                 line_callback);

  // Directly run a diff between a blob and a buffer.
  static void diff_blob_to_buffer(
      const blob &old_blob, const std::string &old_as_path,
      const char *new_buffer, size_t new_buffer_length,
      const std::string &new_as_path,
      const diff::options &options = diff::options(nullptr),
      std::function<void(const diff::delta &, float)> file_callback = {},
      std::function<void(const diff::delta &, const diff::binary &)>
          binary_callback = {},
      std::function<void(const diff::delta &, const diff::hunk &)>
          hunk_callback = {},
      std::function<void(const diff::delta &, const diff::hunk &,
                         const diff::line &)>
          line_callback = {});

  // Directly run a diff between two buffers.
  //
  // Even more than with git_diff_blobs, comparing two buffer lacks context, so
  // the git_diff_file parameters to the callbacks will be faked a la the rules
  // for git_diff_blobs().
  static void diff_between_buffers(
      const void *old_buffer, size_t old_buffer_length,
      const std::string &old_as_path, const void *new_buffer,
      size_t new_buffer_length, const std::string &new_as_path,
      const diff::options &options = diff::options(nullptr),
      std::function<void(const diff::delta &, float)> file_callback = {},
      std::function<void(const diff::delta &, const diff::binary &)>
          binary_callback = {},
      std::function<void(const diff::delta &, const diff::hunk &)>
          hunk_callback = {},
      std::function<void(const diff::delta &, const diff::hunk &,
                         const diff::line &)>
          line_callback = {});

  // Flags to control the behavior of diff rename/copy detection.
  enum class find_flag {
    // obey `diff.renames`. overridden by any other git_diff_find_... flag.
    by_config = 0,

    // look for renames? (`--find-renames`)
    renames = (1u << 0),

    // consider old side of modified for renames? (`--break-rewrites=n`) */
    renames_from_rewrites = (1u << 1),

    // look for copies? (a la `--find-copies`). */
    copies = (1u << 2),

    // consider unmodified as copy sources? (`--find-copies-harder`).
    //
    // for this to work correctly, use git_diff_include_unmodified when
    // the initial `git_diff` is being generated.
    copies_from_unmodified = (1u << 3),

    // mark significant rewrites for split (`--break-rewrites=/m`) */
    rewrites = (1u << 4),
    // actually split large rewrites into delete/add pairs */
    break_rewrites = (1u << 5),
    // mark rewrites for split and break into delete/add pairs */
    and_break_rewrites = (rewrites | break_rewrites),

    // find renames/copies for untracked items in working directory.
    //
    // for this to work correctly, use git_diff_include_untracked when the
    // initial `git_diff` is being generated (and obviously the diff must
    // be against the working directory for this to make sense).
    for_untracked = (1u << 6),

    // turn on all finding features. */
    all = (0x0ff),

    // measure similarity ignoring leading whitespace (default) */
    ignore_leading_whitespace = 0,
    // measure similarity ignoring all whitespace */
    ignore_whitespace = (1u << 12),
    // measure similarity including all data */
    dont_ignore_whitespace = (1u << 13),
    // measure similarity only by comparing shas (fast and cheap) */
    exact_match_only = (1u << 14),

    // do not break rewrites unless they contribute to a rename.
    //
    // normally, git_diff_find_and_break_rewrites will measure the self-
    // similarity of modified files and split the ones that have changed a
    // lot into a delete / add pair.  then the sides of that pair will be
    // considered candidates for rename and copy detection.
    //
    // if you add this flag in and the split pair is *not* used for an
    // actual rename or copy, then the modified record will be restored to
    // a regular modified record instead of being split.
    break_rewrites_for_renames_only = (1u << 15),

    // remove any unmodified deltas after find_similar is done.
    //
    // using git_diff_find_copies_from_unmodified to emulate the
    // --find-copies-harder behavior requires building a diff with the
    // git_diff_include_unmodified flag.  if you do not want unmodified
    // records in the final result, pass this flag to have them removed.
    remove_unmodified = (1u << 16),
  };

  // Control behavior of rename and copy detection
  // These options mostly mimic parameters that can be passed to git-diff.
  class find_options : public libgit2_api {
  public:
    find_options() : c_ptr_(nullptr) {
      auto ret = git_diff_find_init_options(&default_options_,
                                            GIT_DIFF_FIND_OPTIONS_VERSION);
      c_ptr_ = &default_options_;
      if (ret != 0)
        throw git_exception();
    }

    find_options(git_diff_find_options *c_ptr) : c_ptr_(c_ptr) {}

    // Version
    unsigned int version() const { return c_ptr_->version; }
    void set_version(unsigned int version) { c_ptr_->version = version; }

    // Flags
    // Combination of git_diff_find_t values (default GIT_DIFF_FIND_BY_CONFIG).
    // NOTE: if you don't explicitly set this, `diff.renames` could be set
    // to false, resulting in `git_diff_find_similar` doing nothing.
    find_flag flags() const { return static_cast<find_flag>(c_ptr_->flags); }
    void set_flags(find_flag value) {
      c_ptr_->flags = static_cast<uint32_t>(value);
    }

    // Threshold above which similar files will be considered renames.
    // This is equivalent to the -M option. Defaults to 50.
    uint16_t rename_threshold() const { return c_ptr_->rename_threshold; }
    void set_rename_threshold(uint16_t value) {
      c_ptr_->rename_threshold = value;
    }

    // Threshold below which similar files will be eligible to be a rename
    // source. This is equivalent to the first part of the -B option. Defaults
    // to 50.
    uint16_t rename_from_rewrite_threshold() const {
      return c_ptr_->rename_from_rewrite_threshold;
    }
    void set_rename_from_rewrite_threshold(uint16_t value) {
      c_ptr_->rename_from_rewrite_threshold = value;
    }

    // Threshold above which similar files will be considered copies.
    // This is equivalent to the -C option. Defaults to 50.
    uint16_t copy_threshold() const { return c_ptr_->copy_threshold; }
    void set_copy_threshold(uint16_t value) { c_ptr_->copy_threshold = value; }

    // Threshold below which similar files will be split into a delete/add pair.
    // This is equivalent to the last part of the -B option. Defaults to 60.
    uint16_t break_rewrite_threshold() const {
      return c_ptr_->break_rewrite_threshold;
    }
    void set_break_rewrite_threshold(uint16_t value) {
      c_ptr_->break_rewrite_threshold = value;
    }

    // Maximum number of matches to consider for a particular file.
    //
    // This is a little different from the `-l` option from Git because we
    // will still process up to this many matches before abandoning the search.
    // Defaults to 200.
    size_t rename_limit() const { return c_ptr_->rename_limit; }
    void set_rename_limit(size_t value) { c_ptr_->rename_limit = value; }

    // TODO: Add diff_simiarity_metric

    // Access libgit2 C ptr
    const git_diff_find_options *c_ptr() const { return c_ptr_; }

  private:
    git_diff_find_options *c_ptr_;
    git_diff_find_options default_options_;
  };

  // Transform a diff marking file renames, copies, etc.
  //
  // This modifies a diff in place, replacing old entries that look like renames
  // or copies with new entries reflecting those changes. This also will, if
  // requested, break modified files into add/remove pairs if the amount of
  // change is above a threshold.
  void find_similar(const find_options &options = find_options());

  // Patch ID options structure
  class patchid_options : public libgit2_api {
  public:
    patchid_options() : c_ptr_(nullptr) {
      auto ret = git_diff_patchid_init_options(
          &default_options_, GIT_DIFF_PATCHID_OPTIONS_VERSION);
      c_ptr_ = &default_options_;
      if (ret != 0)
        throw git_exception();
    }

    patchid_options(git_diff_patchid_options *c_ptr) : c_ptr_(c_ptr) {}

    // Version
    unsigned int version() const { return c_ptr_->version; }
    void set_version(unsigned int version) { c_ptr_->version = version; }

    // Access libgit2 C ptr
    const git_diff_patchid_options *c_ptr() const { return c_ptr_; }

  private:
    friend diff;
    git_diff_patchid_options *c_ptr_;
    git_diff_patchid_options default_options_;
  };

  // Calculate the patch ID for the given patch.
  oid patchid(const patchid_options &options = patchid_options());

private:
  friend class patch;
  friend class pathspec;
  friend class repository;
  git_diff *c_ptr_;
  ownership owner_;
};
ENABLE_BITMASK_OPERATORS(diff::delta::flag);
ENABLE_BITMASK_OPERATORS(diff::options::flag);
ENABLE_BITMASK_OPERATORS(diff::stats::format);
ENABLE_BITMASK_OPERATORS(diff::format_email_flag);
ENABLE_BITMASK_OPERATORS(diff::find_flag);

} // namespace cppgit2