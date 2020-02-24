#pragma once
#include <cppgit2/exception.hpp>
#include <cppgit2/index.hpp>
#include <cppgit2/strarray.hpp>
#include <cppgit2/tree.hpp>
#include <git2.h>
#include <string>
#include <vector>

namespace cppgit2 {

class checkout {
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

    // Recursively checkout submodules if HEAD moved in super repo (NOT IMPLEMENTED)
    update_submodule_if_changed = (1u << 17),
  };

  // Checkout notification flags
  // Checkout will invoke an options notification callback (`notify_cb`) for
  // certain cases - you pick which ones via `notify_flags`:
  enum class notification_flag {
    none = 0,
    // invokes checkout on conflicting paths.
    conflict = (1u << 0),
    // notifies about "dirty" files, i.e. those that do not need an update but no longer match the
    // baseline.  Core git displays these files when checkout runs, but won't stop the checkout.
    dirty = (1u << 1),
    // sends notification for any file changed.
    updated = (1u << 2),
    // notifies about untracked files.
    untracked = (1u << 3),
    // notifies about ignored files.
    ignored = (1u << 4),
    notify_all = 0x0FFFFu
  };

  class options {
  public:
    // Initializes a git_checkout_options with default values. Equivalent to creating an instance
    // with GIT_CHECKOUT_OPTIONS_INIT.
    options();
    options(git_checkout_options *c_ptr);
    ~options();

    // Version
    unsigned int version() const;
    void set_version(unsigned int version);

    // Strategy
    checkout_strategy strategy() const;
    void set_strategy(checkout_strategy checkout_strategy);

    // Filters
    // don't apply filters like CRLF conversion
    void disable_filters() const;
    void enable_filters();
    bool are_filters_disabled() const;

    // Directory Mode
    // default is 0755
    unsigned int dir_mode() const;
    void set_dir_mode(unsigned int mode);

    // File Mode
    unsigned int file_mode() const;
    // default is 0644 or 0755 as dictated by blob
    void set_file_mode(unsigned int mode);

    // File open flags
    // default is O_CREAT | O_TRUNC | O_WRONLY
    int file_open_flags() const;
    void set_file_open_flags(int flags);

    // Notification flags
    // See checkout::notification_flags
    unsigned int notification_flags() const;
    void set_notification_flags(unsigned int flags);

    // Paths
    // A list of wildmatch patterns or paths. By default, all paths are processed. If you pass an
    // array of wildmatch patterns, those will be used to filter which paths should be taken into
    // account.
    std::vector<std::string> paths() const;
    void set_paths(const std::vector<std::string> &paths);

    // Baseline
    // The expected content of the working directory; defaults to HEAD. If the working directory
    // does not match this baseline information, that will produce a checkout conflict.
    tree baseline() const;
    void set_baseline(tree &baseline);

    // Baseline index
    // Like `baseline` above, though expressed as an index. This option overrides `baseline`.
    index baseline_index() const;
    void set_baseline_index(index &baseline_index);

    // Target directory
    // alternative checkout path to workdir
    std::string target_directory() const;
    void set_target_directory(const std::string &target_directory);

    // Ancestor label
    // the name of the common ancestor side of conflicts
    std::string ancestor_label() const;
    void set_ancestor_label(const std::string &ancestor_label);

    // "Our" label
    // the name of the "our" side of conflicts
    std::string our_label() const;
    void set_our_label(const std::string &our_label);

    // "Their" label
    // the name of the "their" side of conflicts
    std::string their_label() const;
    void set_their_label(const std::string &their_label);

    const git_checkout_options *c_ptr() const;

  private:
    git_checkout_options *c_ptr_;
    git_checkout_options default_options_;
  };
};

} // namespace cppgit2