#pragma once
#include <cppgit2/annotated_commit.hpp>
#include <cppgit2/apply.hpp>
#include <cppgit2/attribute.hpp>
#include <cppgit2/blob.hpp>
#include <cppgit2/config.hpp>
#include <cppgit2/data_buffer.hpp>
#include <cppgit2/exception.hpp>
#include <cppgit2/index.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/object.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/reference.hpp>
#include <git2.h>
#include <string>

namespace cppgit2 {

// Owned by user
class repository : public libgit2_api {
public:
  // Default construct a git repo
  repository(git_repository *c_ptr = nullptr);

  // Initialize git repository
  repository(const std::string &path, bool is_bare);

  // Free git_repository ptr
  ~repository();

  // Open a git repository
  // Auto-detects if `path` is normal or bare repo and fails if neither
  void open(const std::string &path);

  // Fast open for bare repositories
  // Useful when hosting repos and need to access them effeciently
  void open_bare(const std::string &path);

  // Check if a repository is bare
  bool is_bare() const;

  // Check if a repository is empty
  // If true, an empty repository has just been initialized and
  // contains no references apart from HEAD, which must be pointing
  // to the unborn master branch.
  bool is_empty() const;

  // Determine if the repository was a shallow clone
  bool is_shallow() const;

  // Check if a repository is a linked work tree
  bool is_worktree() const;

  // Path to the .git folder for normal repos
  // Path to the repo itself for bare repos
  std::string path() const;

  // Path to the shared common dir for this repo
  std::string commondir() const;

  // Config file for this repo
  // If no config file is set, the default config is returned
  cppgit2::config config() const;

  // Get a snapshot of this repo's config
  cppgit2::config config_snapshot() const;

  // Detach the HEAD
  void detach_head();

  // Look for a git repository and return path
  // Method will auto-detect if a repo is bare (if there is a repo)
  static std::string discover_path(const std::string &start_path,
                                   bool across_fs,
                                   const std::string &ceiling_dirs);

  static std::string discover_path(const std::string &start_path);

  // Currently active namespace for this repo
  std::string namespace_() const;

  // Calculate hash of file using repo filtering rules
  oid hashfile(const std::string &path, object::object_type type,
               const std::string &as_path);

  // hashfile overload that uses as_path = path
  oid hashfile(const std::string &path, object::object_type type);

  // Check if repo HEAD is detached
  // A repository's HEAD is detached when it points directly to a commit instead
  // of a branch.
  bool is_head_detached() const;

  // Check if a worktree's HEAD is detached
  // A worktree's HEAD is detached when it points directly to a commit instead
  // of a branch.
  bool is_head_detached_for_worktree(const std::string &path);

  // Check if the current branch is unborn
  // An unborn branch is one named from HEAD but which doesn't exist in the refs
  // namespace, because it doesn't have any commit to point to.
  bool is_head_unborn() const;

  // Retrieve configured identity to use for reflogs
  std::pair<std::string, std::string> identity() const;

  // Get the index file for this repo
  // If a custom index has not been set, the default index for the repository
  // will be returned (the one located in .git/index).
  cppgit2::index index() const;

  enum class item {
    gitdir,
    workdir,
    commondir,
    index,
    objects,
    refs,
    packed_refs,
    remotes,
    config,
    info,
    hooks,
    logs,
    modules,
    worktree
  };

  // Get the location of a specific repository file or directory
  std::string path(item item) const;

  // Retrieve git's prepared message
  //
  // Operations such as git revert/cherry-pick/merge with the -n option stop
  // just short of creating a commit with the changes and save their prepared
  // message in .git/MERGE_MSG so the next git-commit execution can present it
  // to the user for them to amend if they wish.
  //
  // Use this function to get the contents of this file. Don't forget to remove
  // the file after you create the commit.
  std::string message() const;

  // Remove the message that the above message() call retrieves.
  void remove_message();

  // Make the repository HEAD point to the specified reference.
  void set_head(const std::string &refname);

  // Make the repository HEAD directly point to the Commit.
  void set_head_detached(const oid &commitish);

  // Set the identity to be used for writing reflogs
  void set_identity(const std::string &name, const std::string &email);

  // Unset the identity used for writing reflogs
  // When unset, the identity will be taken from the repository's configuration.
  void unset_identity();

  // Sets the active namespace for this Git Repository
  // This should not include the refs folder, e.g. to namespace all
  // references under `refs/namespaces/foo/`, use `foo` as the namespace.
  void set_namespace(const std::string &nmspace);

  // Set the path to the working directory for this repository
  // The working directory doesn't need to be the same one that contains the
  // .git folder for this repository.
  void set_workdir(const std::string &workdir, bool update_gitlink);

  // Remove all the metadata associated with an ongoing command like merge,
  // revert, cherry-pick, etc. For example: MERGE_HEAD, MERGE_MSG, etc.
  void cleanup_state();

  enum class repository_state {
    unknown,                 // maps to git_repository_state() of -1
    none,                    // GIT_REPOSITORY_STATE_NONE
    rebase_interactive,      // GIT_REPOSITORY_STATE_REBASE_INTERACTIVE
    rebase_merge,            // GIT_REPOSITORY_STATE_REBASE_MERGE
    rebase,                  // GIT_REPOSITORY_STATE_REBASE
    apply_mailbox,           // GIT_REPOSITORY_STATE_APPLY_MAILBOX
    apply_mailbox_or_rebase, // GIT_REPOSITORY_STATE_APPLY_MAILBOX_OR_REBASE
    merge,                   // GIT_REPOSITORY_STATE_MERGE
    revert,                  // GIT_REPOSITORY_STATE_REVERT
    revert_sequence,         // GIT_REPOSITORY_STATE_REVERT_SEQUENCE
    cherrypick,              // GIT_REPOSITORY_STATE_CHERRYPICK
    cherrypick_sequence,     // GIT_REPOSITORY_STATE_CHERRYPICK_SEQUENCE
    bisect,                  // GIT_REPOSITORY_STATE_BISECT
  };

  // Determines the status of a git repository - ie, whether an operation
  // (merge, cherry-pick, etc) is in progress.
  repository_state state() const;

  // Get the path of the working directory for this repository
  std::string workdir() const;

  // Access to libgit2 C ptr
  const git_repository *c_ptr() const;

  /*
   * Annotated Commit API
   * See git_annotated_commit_* functions
   */

  // Creates an annotated_commit from the given fetch head data
  annotated_commit create_annotated_commit(const std::string &branch_name,
                                           const std::string &remote_url,
                                           const oid &id);

  // Create annotated commit from revspec
  // (the extended sha syntax string to use to lookup the commit)
  annotated_commit create_annotated_commit(const std::string &revspec);

  // Creates an annotated_commit from the given reference (
  // (used to lookup the git_annotated_commit)
  annotated_commit create_annotated_commit(const reference &ref);

  // Lookup annotated_commit from the given commit id
  annotated_commit lookup_annotated_commit(const oid &id);

  /*
   * Apply API
   * See git_apply_* functions
   */

  // Apply a git_diff to the given repository, making changes directly in the
  // working directory, the index, or both.
  void apply_diff(const diff &diff, apply::location location,
                  const apply::options &options = apply::options());

  // Apply a git_diff to a git_tree, and return the resulting image as an index.
  cppgit2::index apply_diff(const tree &preimage, const diff &diff,
                            const apply::options &options = apply::options());

  /*
   * Attributes API
   * See git_attr_* functions
   */

  // Add a macro definition.
  // Macros will automatically be loaded from the top level .gitattributes file
  // of the repository (plus the build-in "binary" macro). This function allows
  // you to add others. For example, to add the default macro, you would call:
  //     repo.add_attributes_macro("binary", "-diff -crlf");
  void add_attributes_macro(const std::string &name, const std::string &values);

  // Flush the gitattributes cache.
  //
  // Call this if you have reason to believe that the attributes files on disk
  // no longer match the cached contents of memory. This will cause the
  // attributes files to be reloaded the next time that an attribute access
  // function is called.
  void flush_attributes_cache();

  // Loop over all the git attributes for a path.
  void for_each_attribute(
      attribute::flag flags, const std::string &path,
      std::function<void(const std::string &, const std::string &)> visitor);

  // Look up the value of one git attribute for path.
  std::string lookup_attribute(attribute::flag flags, const std::string &path,
                               const std::string &name);

  // Look up a list of git attributes for path.
  // Use this if you have a known list of attributes that you want to look up in
  // a single call. This is somewhat more efficient than calling
  // lookup_attribute() multiple times.
  //
  // Example usage:
  //   repo.get_multiple_attributes(repo, attribute::flag::file_then_index,
  //      "my/fun/file.c", { "crlf", "diff", "foo" });
  std::vector<std::string>
  lookup_multiple_attributes(attribute::flag flags, const std::string &path,
                             const std::vector<std::string> &names);

  /*
   * BLOB API
   * See git_blob_* functions
   */

  // Write an in-memory buffer to the ODB as a blob
  oid create_blob_from_buffer(const std::string &buffer);

  // Read a file from the filesystem and write its content to the Object
  // Database as a loose blob
  oid create_blob_from_disk(const std::string &path);
  // Read a file from the working folder of a repository
  // and write it to the Object Database as a loose blob
  oid create_blob_from_workdir(const std::string &relative_path);

  // Lookup a blob object from a repository.
  blob lookup_blob(const oid &id) const;

  // Lookup a blob object from a repository,
  // given a prefix of its identifier (short id).
  blob lookup_blob(const oid &id, size_t len) const;

private:
  friend class tree_builder;
  git_repository *c_ptr_;
};

} // namespace cppgit2
