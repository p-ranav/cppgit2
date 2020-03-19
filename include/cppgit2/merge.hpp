#pragma once
#include <cppgit2/bitmask_operators.hpp>
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <git2.h>
#include <string>

namespace cppgit2 {

class merge : public libgit2_api {
public:
  class file : public libgit2_api {
  public:
    // Merge file favor options for `git_merge_options` instruct the file-level
    // merging functionality how to deal with conflicting regions of the files.
    enum class favor {
      // When a region of a file is changed in both branches, a conflict
      // will be recorded in the index so that `git_checkout` can produce
      // a merge file with conflict markers in the working directory.
      // This is the default.
      normal = 0,

      // When a region of a file is changed in both branches, the file
      // created in the index will contain the "ours" side of any conflicting
      // region.  The index will not record a conflict.
      ours = 1,

      // When a region of a file is changed in both branches, the file
      // created in the index will contain the "theirs" side of any conflicting
      // region.  The index will not record a conflict.
      theirs = 2,

      // When a region of a file is changed in both branches, the file
      // created in the index will contain each unique line from each side,
      // which has the result of combining both files.  The index will not
      // record a conflict.
      union_ = 3
    };

    // File merging flags
    enum class flag {
      // Defaults
      default_ = 0,

      // Create standard conflicted merge files
      style_merge = (1 << 0),

      // Create diff3-style files
      style_diff3 = (1 << 1),

      // Condense non-alphanumeric regions for simplified diff file
      simplify_alnum = (1 << 2),

      // Ignore all whitespace
      ignore_whitespace = (1 << 3),

      // Ignore changes in amount of whitespace
      ignore_whitespace_change = (1 << 4),

      // Ignore whitespace at end of line
      ignore_whitespace_eol = (1 << 5),

      // Use the "patience diff" algorithm
      diff_patience = (1 << 6),

      // Take extra time to find minimal diff
      diff_minimal = (1 << 7)
    };

    // The file inputs to `git_merge_file`. Callers should populate the
    // `git_merge_file_input` structure with descriptions of the files in each
    // side of the conflict for use in producing the merge file.
    class input : public libgit2_api {
    public:
      // Default construct a merge_file_input struct
      // Initializes a git_merge_file_input with default values. Equivalent to
      // creating an instance with GIT_MERGE_FILE_INPUT_INIT.
      input() {
        if (git_merge_file_input_init(&c_struct_, GIT_MERGE_FILE_INPUT_VERSION))
          throw git_exception();
      }

      // Construct from libgit2 C ptr
      input(git_merge_file_input *c_ptr) : c_struct_(*c_ptr) {}

      // Version
      unsigned int version() const { return c_struct_.version; }
      void set_version(unsigned int value) { c_struct_.version = value; }

      // Contents of the file.
      const char *ptr() const { return c_struct_.ptr; }
      void set_ptr(const char *value) { c_struct_.ptr = value; }

      // Size of the contents pointed to in `ptr`.
      size_t size() const { return c_struct_.size; }
      void set_size(size_t value) { c_struct_.size = value; }

      // File name of the conflicted file, or "" to not merge the path.
      std::string path() const {
        return c_struct_.path ? std::string(c_struct_.path) : "";
      }
      void set_path(const std::string &value) {
        c_struct_.path = value.c_str();
      }

      // File mode of the conflicted file, or `0` to not merge the mode.
      unsigned int mode() const { return c_struct_.mode; }
      void set_mode(unsigned int value) { c_struct_.mode = value; }

      // Access libgit2 C ptr
      const git_merge_file_input *c_ptr() const { return &c_struct_; }

    private:
      git_merge_file_input c_struct_;
    };

    // Information about file-level merging
    // Always owned by user
    class result : public libgit2_api {
    public:
      result(git_merge_file_result *c_ptr) : c_ptr_(c_ptr) {}

      ~result() {
        if (c_ptr_)
          git_merge_file_result_free(c_ptr_);
      }

      // True if the output was automerged, false if the output contains
      // conflict markers.
      bool automergable() const { return c_ptr_->automergeable; }

      // The path that the resultant merge file should use, or empty if a
      // filename conflict would occur.
      std::string path() const {
        return c_ptr_->path ? std::string(c_ptr_->path) : "";
      }

      // The mode that the resultant merge file should use.
      unsigned int mode() const { return c_ptr_->mode; }

      // The contents of the merge
      const char *ptr() const { return c_ptr_->ptr; }

      // The length of the merge contents.
      size_t size() const { return c_ptr_->len; }

      // Access libgit2 C ptr
      const git_merge_file_result *c_ptr() const { return c_ptr_; }

    private:
      git_merge_file_result *c_ptr_;
    };

    // Options for merging a file
    class options : public libgit2_api {
    public:
      options() : c_ptr_(nullptr) {
        auto ret = git_merge_file_init_options(&default_options_,
                                               GIT_MERGE_FILE_OPTIONS_VERSION);
        c_ptr_ = &default_options_;
        if (ret != 0)
          throw git_exception();
      }

      options(git_merge_file_options *c_ptr) : c_ptr_(c_ptr) {}

      // Version
      unsigned int version() const { return c_ptr_->version; }
      void set_version(unsigned int version) { c_ptr_->version = version; }

      // Ancestor Label
      // Label for the ancestor file side of the conflict which will be
      // prepended to labels in diff3-format merge files.
      std::string ancestor_label() const {
        return c_ptr_->ancestor_label ? std::string(c_ptr_->ancestor_label)
                                      : "";
      }
      void set_ancestor_label(const std::string &value) {
        c_ptr_->ancestor_label = value.c_str();
      }

      // Our Label
      // Label for our file side of the conflict which will be prepended to
      // labels in merge files.
      std::string our_label() const {
        return c_ptr_->our_label ? std::string(c_ptr_->our_label) : "";
      }
      void set_our_label(const std::string &value) {
        c_ptr_->our_label = value.c_str();
      }

      // Their Label
      // Label for their file side of the conflict which will be prepended to
      // labels in merge files.
      std::string their_label() const {
        return c_ptr_->their_label ? std::string(c_ptr_->their_label) : "";
      }
      void set_their_label(const std::string &value) {
        c_ptr_->their_label = value.c_str();
      }

      // File favor
      merge::file::favor file_favor() const {
        return static_cast<merge::file::favor>(c_ptr_->favor);
      }
      void set_file_favor(merge::file::favor value) {
        c_ptr_->favor = static_cast<git_merge_file_favor_t>(value);
      }

      // Flags
      merge::file::flag flags() const {
        return static_cast<merge::file::flag>(c_ptr_->flags);
      }
      void set_flags(merge::file::flag flags) {
        c_ptr_->flags = static_cast<uint32_t>(flags);
      }

      // Marker size
      // The size of conflict markers (eg, " < < < < < < < ").
      // Default is GIT_MERGE_CONFLICT_MARKER_SIZE.
      unsigned short marker_size() const { return c_ptr_->marker_size; }
      void set_marker_size(unsigned short value) {
        c_ptr_->marker_size = value;
      }

      // Access libgit2 C ptr
      const git_merge_file_options *c_ptr() const { return c_ptr_; }

    private:
      git_merge_file_options *c_ptr_;
      git_merge_file_options default_options_;
    };
  };

  class options : public libgit2_api {
  public:
    options() : c_ptr_(nullptr) {
      auto ret =
          git_merge_init_options(&default_options_, GIT_MERGE_OPTIONS_VERSION);
      c_ptr_ = &default_options_;
      if (ret != 0)
        throw git_exception();
    }

    options(git_merge_options *c_ptr) : c_ptr_(c_ptr) {}

    // Version
    unsigned int version() const { return c_ptr_->version; }
    void set_version(unsigned int version) { c_ptr_->version = version; }

    // Flags for `git_merge` options.  A combination of these flags can be
    // passed in via the `flags` value in the `git_merge_options`.
    enum class flag {
      // Detect renames that occur between the common ancestor and the "ours"
      // side or the common ancestor and the "theirs" side.  This will enable
      // the ability to merge between a modified and renamed file.
      find_renames = (1 << 0),

      // If a conflict occurs, exit immediately instead of attempting to
      // continue resolving conflicts.  The merge operation will fail with
      // GIT_EMERGECONFLICT and no index will be returned.
      fail_on_conflict = (1 << 1),

      // Do not write the REUC extension on the generated index
      skip_reuc = (1 << 2),

      // If the commits being merged have multiple merge bases, do not build
      // a recursive merge base (by merging the multiple merge bases),
      // instead simply use the first base.  This flag provides a similar
      // merge base to `git-merge-resolve`.
      no_recursive = (1 << 3)
    };

    // Flags
    // See `merge::options::flag` above
    flag flags() const {
      return static_cast<merge::options::flag>(c_ptr_->flags);
    }

    void set_flags(flag flags) {
      c_ptr_->flags = static_cast<git_merge_flag_t>(flags);
    }

    // Rename threshold
    // Similarity to consider a file renamed (default 50). If
    // `GIT_MERGE_FIND_RENAMES` is enabled, added files will be compared with
    // deleted files to determine their similarity. Files that are more similar
    // than the rename threshold (percentage-wise) will be treated as a rename.
    unsigned int rename_threshold() const { return c_ptr_->rename_threshold; }
    void set_rename_threshold(unsigned int rename_threshold) {
      c_ptr_->rename_threshold = rename_threshold;
    }

    // Target Limit
    // Maximum similarity sources to examine for renames (default 200). If the
    // number of rename candidates (add / delete pairs) is greater than this
    // value, inexact rename detection is aborted. This setting overrides the
    // `merge.renameLimit` configuration value.
    unsigned int target_limit() const { return c_ptr_->target_limit; }
    void set_target_limit(unsigned int target_limit) {
      c_ptr_->target_limit = target_limit;
    }

    // Not including interface for:
    //   git_diff_similarity_metric // metric

    // Recursion Limit
    // Maximum number of times to merge common ancestors to build a virtual
    // merge base when faced with criss-cross merges. When this limit is
    // reached, the next ancestor will simply be used instead of attempting to
    // merge it. The default is unlimited.
    unsigned int recursion_limit() const { return c_ptr_->recursion_limit; }
    void set_recursion_limit(unsigned int recursion_limit) {
      c_ptr_->recursion_limit = recursion_limit;
    }

    // Default driver
    std::string default_driver() const {
      auto ret = c_ptr_->default_driver;
      return ret ? std::string(ret) : "";
    }

    void set_default_driver(const std::string &default_driver) {
      c_ptr_->default_driver =
          default_driver == "" ? nullptr : default_driver.c_str();
    }

    // File Favor
    merge::file::favor file_favor() const {
      return static_cast<merge::file::favor>(c_ptr_->file_favor);
    }

    void set_file_favor(merge::file::favor file_favor) {
      c_ptr_->file_favor = static_cast<git_merge_file_favor_t>(file_favor);
    }

    // File Flags
    // See `merge::options::file_flag` above
    merge::file::flag file_flags() const {
      return static_cast<merge::file::flag>(c_ptr_->file_flags);
    }
    void set_file_flags(merge::file::flag file_flags) {
      c_ptr_->file_flags = static_cast<git_merge_file_flag_t>(file_flags);
    }

    // Access libgit2 C ptr
    const git_merge_options *c_ptr() const { return c_ptr_; }

  private:
    git_merge_options *c_ptr_;
    git_merge_options default_options_;
  };

  // The results of `git_merge_analysis` indicate the merge opportunities.
  enum class analysis_result {
    // No merge is possible. (Unused.)
    none,
    // A "normal" merge; both HEAD and the given merge input have diverged from
    // their common ancestor. The divergent commits must be merged.
    normal,
    // All given merge inputs are reachable from HEAD, meaning the repository is
    // up-to-date and no merge needs to be performed.
    up_to_date,
    // The given merge input is a fast-forward from HEAD and no merge needs to
    // be performed. Instead, the client can check out the given merge input.
    fastforward,
    // The HEAD of the current repository is "unborn" and does not point to a
    // valid commit. No merge can be performed, but the caller may wish to
    // simply set HEAD to the target commit(s).
    unborn
  };

  // The user's stated preference for merges.
  enum class preference {
    // No configuration was found that suggests a preferred behavior for merge.
    none,
    // There is a merge.ff=false configuration setting, suggesting that the user
    // does not want to allow a fast-forward merge.
    no_fastforward,
    // There is a merge.ff=only configuration setting, suggesting that the user
    // only wants fast-forward merges.
    fastforward_only
  };

  // Merge two files as they exist in the in-memory data structures, using the
  // given common ancestor as the baseline, producing a git_merge_file_result
  // that reflects the merge result. The git_merge_file_result must be freed
  // with git_merge_file_result_free.
  static file::result
  merge_files(const file::input &ancestor, const file::input &ours,
              const file::input &theirs,
              const file::options &options = file::options());
};
ENABLE_BITMASK_OPERATORS(merge::options::flag);
ENABLE_BITMASK_OPERATORS(merge::file::flag);

} // namespace cppgit2