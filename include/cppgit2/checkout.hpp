#pragma once
#include <cppgit2/bitmask_operators.hpp>
#include <cppgit2/git_exception.hpp>
#include <cppgit2/index.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/strarray.hpp>
#include <cppgit2/tree.hpp>
#include <git2.h>
#include <string>
#include <vector>

namespace cppgit2 {

class checkout : public libgit2_api {
public:
  enum class checkout_strategy {
    // Default is a dry run, no actual updates
    none = 0,

    // Allow safe updates that cannot overwrite uncommitted data.
    // If the uncommitted changes don't conflict with the checked out files,
    // the checkout will still proceed, leaving the changes intact.
    //
    //  Mutually exclusive with strategy::force.
    // strategy::force takes precedence over strategy::safe.
    safe = (1u << 0),

    // Allow all updates to force working directory to look like index.
    //
    // Mutually exclusive with strategy::safe.
    // strategy::force takes precedence over strategy::safe.
    force = (1u << 1),

    // Allow checkout to recreate missing files
    recreate_missing = (1u << 2),

    // Allow checkout to make safe updates even if conflicts are found
    allow_conflicts = (1u << 4),

    // Remove untracked files not in index (that are not ignored)
    remove_untracked = (1u << 5),

    // Remove ignored files not in index
    remove_ignored = (1u << 6),

    // Only update existing files, don't create new ones
    update_only = (1u << 7),

    // Normally checkout updates index entries as it goes; this stops that.
    // Implies strategy::dont_write_index
    dont_update_index = (1u << 8),

    // Don't refresh index/config/etc before doing checkout
    no_refresh = (1u << 9),

    // Allow checkout to skip unmerged files
    skip_unmerged = (1u << 10),

    // For unmerged files, checkout stage 2 from index
    use_ours = (1u << 11),

    // For unmerged files, checkout stage 3 from index
    use_theirs = (1u << 12),

    // Treat pathspec as simple list of exact match file paths
    disable_pathspec_match = (1u << 13),

    // Ignore directories in use, they will be left empty
    skip_locked_directories = (1u << 18),

    // Don't overwrite ignored files that exist in the checkout target
    dont_overwrite_ignored = (1u << 19),

    // Write normal merge files for conflicts
    conflict_style_merge = (1u << 20),

    // Include common ancestor data in diff3 format files for conflicts
    conflict_style_diff3 = (1u << 21),

    // Don't overwrite existing files or folders
    dont_remove_existing = (1u << 22),

    // Normally checkout writes the index upon completion; this prevents that.
    dont_write_index = (1u << 23),

    // THE FOLLOWING OPTIONS ARE NOT YET IMPLEMENTED
    // as of date: 2020.01.06

    // Recursively checkout submodules with same options (NOT IMPLEMENTED)
    update_submodules = (1u << 16),

    // Recursively checkout submodules if HEAD moved in super repo (NOT
    // IMPLEMENTED)
    update_submodule_if_changed = (1u << 17),
  };

  // Checkout notification flags
  // Checkout will invoke an options notification callback (`notify_cb`) for
  // certain cases - you pick which ones via `notify_flags`:
  enum class notification_flag {
    none = 0,
    // invokes checkout on conflicting paths.
    conflict = (1u << 0),
    // notifies about "dirty" files, i.e. those that do not need an update but
    // no longer match the baseline.  Core git displays these files when
    // checkout runs, but won't stop the checkout.
    dirty = (1u << 1),
    // sends notification for any file changed.
    updated = (1u << 2),
    // notifies about untracked files.
    untracked = (1u << 3),
    // notifies about ignored files.
    ignored = (1u << 4),
    notify_all = 0x0FFFFu
  };

  class options : public libgit2_api {
  public:
    options() {
      auto ret = git_checkout_init_options(&default_options_,
                                           GIT_CHECKOUT_OPTIONS_VERSION);
      c_ptr_ = &default_options_;
      if (ret != 0)
        throw git_exception();
    }

    options(git_checkout_options *c_ptr) : c_ptr_(c_ptr) {}

    // Version
    unsigned int version() const { return c_ptr_->version; }
    void set_version(unsigned int version) { c_ptr_->version = version; }

    // Strategy
    checkout_strategy strategy() const {
      return static_cast<checkout_strategy>(c_ptr_->checkout_strategy);
    }
    void set_strategy(checkout_strategy checkout_strategy) {
      c_ptr_->checkout_strategy = static_cast<unsigned int>(checkout_strategy);
    }

    // Filters
    // don't apply filters like CRLF conversion
    void disable_filters() const { c_ptr_->disable_filters = true; }
    void enable_filters() { c_ptr_->disable_filters = false; }
    bool are_filters_disabled() const { return c_ptr_->disable_filters; }

    // Directory Mode
    // default is 0755
    unsigned int dir_mode() const { return c_ptr_->dir_mode; }
    void set_dir_mode(unsigned int mode) { c_ptr_->dir_mode = mode; }

    // File Mode
    unsigned int file_mode() const { return c_ptr_->file_mode; }
    // default is 0644 or 0755 as dictated by blob
    void set_file_mode(unsigned int mode) { c_ptr_->file_mode = mode; }

    // File open flags
    // default is O_CREAT | O_TRUNC | O_WRONLY
    int file_open_flags() const { return c_ptr_->file_open_flags; }
    void set_file_open_flags(int flags) { c_ptr_->file_open_flags = flags; }

    // Notification flags
    // See checkout::notification_flags
    unsigned int notification_flags() const { return c_ptr_->notify_flags; }
    void set_notification_flags(unsigned int flags) {
      c_ptr_->notify_flags = flags;
    }

    // Paths
    // A list of wildmatch patterns or paths. By default, all paths are
    // processed. If you pass an array of wildmatch patterns, those will be used
    // to filter which paths should be taken into account.
    std::vector<std::string> paths() const {
      return strarray(&c_ptr_->paths).to_vector();
    }
    void set_paths(const std::vector<std::string> &paths) {
      c_ptr_->paths = *(strarray(paths).c_ptr());
    }

    // Baseline
    // The expected content of the working directory; defaults to HEAD. If the
    // working directory does not match this baseline information, that will
    // produce a checkout conflict.
    tree baseline() const { return tree(c_ptr_->baseline); }
    void set_baseline(const tree &baseline) {
      c_ptr_->baseline = const_cast<git_tree *>(baseline.c_ptr());
    }

    // Baseline index
    // Like `baseline` above, though expressed as an index. This option
    // overrides `baseline`.
    index baseline_index() const { return index(c_ptr_->baseline_index); }
    void set_baseline_index(const index &baseline_index) {
      c_ptr_->baseline_index = const_cast<git_index *>(baseline_index.c_ptr());
    }

    // Target directory
    // alternative checkout path to workdir
    std::string target_directory() const {
      auto ret = c_ptr_->target_directory;
      if (ret)
        return std::string(ret);
      else
        return "";
    }
    void set_target_directory(const std::string &target_directory) {
      c_ptr_->target_directory = target_directory.c_str();
    }

    // Ancestor label
    // the name of the common ancestor side of conflicts
    std::string ancestor_label() const {
      auto ret = c_ptr_->ancestor_label;
      if (ret)
        return std::string(ret);
      else
        return "";
    }
    void set_ancestor_label(const std::string &ancestor_label) {
      c_ptr_->ancestor_label = ancestor_label.c_str();
    }

    // "Our" label
    // the name of the "our" side of conflicts
    std::string our_label() const {
      auto ret = c_ptr_->our_label;
      if (ret)
        return std::string(ret);
      else
        return "";
    }
    void set_our_label(const std::string &our_label) {
      c_ptr_->our_label = our_label.c_str();
    }

    // "Their" label
    // the name of the "their" side of conflicts
    std::string their_label() const {
      auto ret = c_ptr_->their_label;
      if (ret)
        return std::string(ret);
      else
        return "";
    }
    void set_their_label(const std::string &their_label) {
      c_ptr_->their_label = their_label.c_str();
    }

    const git_checkout_options *c_ptr() const { return c_ptr_; }

  private:
    git_checkout_options *c_ptr_;
    git_checkout_options default_options_;
  };
};
ENABLE_BITMASK_OPERATORS(checkout::notification_flag);
ENABLE_BITMASK_OPERATORS(checkout::checkout_strategy);

} // namespace cppgit2