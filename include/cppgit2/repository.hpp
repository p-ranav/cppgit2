#pragma once
#include <cppgit2/annotated_commit.hpp>
#include <cppgit2/apply.hpp>
#include <cppgit2/attribute.hpp>
#include <cppgit2/bitmask_operators.hpp>
#include <cppgit2/blame.hpp>
#include <cppgit2/blob.hpp>
#include <cppgit2/branch.hpp>
#include <cppgit2/checkout.hpp>
#include <cppgit2/cherrypick.hpp>
#include <cppgit2/clone.hpp>
#include <cppgit2/commit.hpp>
#include <cppgit2/config.hpp>
#include <cppgit2/data_buffer.hpp>
#include <cppgit2/fetch.hpp>
#include <cppgit2/git_exception.hpp>
#include <cppgit2/index.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/note.hpp>
#include <cppgit2/object.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/pack_builder.hpp>
#include <cppgit2/pathspec.hpp>
#include <cppgit2/rebase.hpp>
#include <cppgit2/refdb.hpp>
#include <cppgit2/reference.hpp>
#include <cppgit2/remote.hpp>
#include <cppgit2/reset.hpp>
#include <cppgit2/revert.hpp>
#include <cppgit2/revision.hpp>
#include <cppgit2/revparse.hpp>
#include <cppgit2/revspec.hpp>
#include <cppgit2/revwalk.hpp>
#include <cppgit2/stash.hpp>
#include <cppgit2/status.hpp>
#include <cppgit2/submodule.hpp>
#include <cppgit2/tag.hpp>
#include <cppgit2/tree_builder.hpp>
#include <cppgit2/worktree.hpp>
#include <functional>
#include <git2.h>
#include <string>
#include <utility>

namespace cppgit2 {

// Owned by user
class repository : public libgit2_api {
public:
  // Default construct a git repo
  repository(git_repository *c_ptr = nullptr);

  // Free git_repository ptr
  ~repository();

  // Initialize git repository
  static repository init(const std::string &path, bool is_bare);

  // Option flags for `git_repository_init_ext`
  enum class init_flag {
    bare = (1u << 0),
    no_reinit = (1u << 1),
    no_dotgit_dir = (1u << 2),
    mkdir = (1u << 3),
    mkpath = (1u << 4),
    external_template = (1u << 5),
    relative_gitlink = (1u << 6),
  };

  // mode options for `git_repository_init_ext`
  enum class init_mode {
    shared_umask = 0,
    shared_group = 0002775,
    shared_all = 0002777,
  };

  class init_options : public libgit2_api {
  public:
    init_options() : c_ptr_(nullptr) {
      auto ret = git_repository_init_options_init(
          &default_options_, GIT_REPOSITORY_INIT_OPTIONS_VERSION);
      c_ptr_ = &default_options_;
      if (ret != 0)
        throw git_exception();
    }

    init_options(git_repository_init_options *c_ptr) : c_ptr_(c_ptr) {}

    // Version
    unsigned int version() const { return c_ptr_->version; }
    void set_version(unsigned int version) { c_ptr_->version = version; }

    // Init flags
    init_flag flags() const { return static_cast<init_flag>(c_ptr_->flags); }
    void set_flags(init_flag flags) {
      c_ptr_->flags = static_cast<uint32_t>(flags);
    }

    // Init mode
    init_mode mode() const { return static_cast<init_mode>(c_ptr_->mode); }
    void set_mode(init_mode mode) {
      c_ptr_->mode = static_cast<uint32_t>(mode);
    }

    // Workdir path
    std::string workdir_path() const {
      return c_ptr_->workdir_path ? std::string(c_ptr_->workdir_path) : "";
    }
    void set_workdir_path(const std::string &value) {
      c_ptr_->workdir_path = value.c_str();
    }

    // Description
    std::string description() const {
      return c_ptr_->description ? std::string(c_ptr_->description) : "";
    }
    void set_description(const std::string &value) {
      c_ptr_->description = value.c_str();
    }

    // Template path
    std::string template_path() const {
      return c_ptr_->template_path ? std::string(c_ptr_->template_path) : "";
    }
    void set_template_path(const std::string &value) {
      c_ptr_->template_path = value.c_str();
    }

    // Initial head
    std::string initial_head() const {
      return c_ptr_->initial_head ? std::string(c_ptr_->initial_head) : "";
    }
    void set_initial_head(const std::string &value) {
      c_ptr_->initial_head = value.c_str();
    }

    // Origin URL
    std::string origin_url() const {
      return c_ptr_->origin_url ? std::string(c_ptr_->origin_url) : "";
    }
    void set_origin_url(const std::string &value) {
      c_ptr_->origin_url = value.c_str();
    }

    // Access libgit2 C ptr
    const git_repository_init_options *c_ptr() const { return c_ptr_; }

  private:
    friend class repository;
    git_repository_init_options *c_ptr_;
    git_repository_init_options default_options_;
  };

  // Create a new Git repository in the given folder with extended controls.
  static repository init_ext(const std::string &repo_path,
                             const init_options &options = init_options());

  // Open a git repository
  // Auto-detects if `path` is normal or bare repo and fails if neither
  static repository open(const std::string &path);

  // Fast open for bare repositories
  // Useful when hosting repos and need to access them effeciently
  static repository open_bare(const std::string &path);

  enum class open_flag {
    // Only open the repository if it can be immediately found in the
    // start_path. Do not walk up from the start_path looking at parent
    // directories.
    no_search = (1 << 0),

    // unless this flag is set, open will not continue searching across
    // filesystem boundaries (i.e. when `st_dev` changes from the `stat`
    // system call).  for example, searching in a user's home directory at
    // "/home/user/source/" will not return "/.git/" as the found repo if
    // "/" is a different filesystem than "/home".
    cross_fs = (1 << 1),

    // open repository as a bare repo regardless of core.bare config, and
    // defer loading config file for faster setup.
    // unlike `git_repository_open_bare`, this can follow gitlinks.
    bare = (1 << 2),

    // do not check for a repository by appending /.git to the start_path;
    // only open the repository if start_path itself points to the git
    // directory.
    no_dotgit = (1 << 3),

    // find and open a git repository, respecting the environment variables
    // used by the git command-line tools.
    // if set, `git_repository_open_ext` will ignore the other flags and
    // the `ceiling_dirs` argument, and will allow a null `path` to use
    // `git_dir` or search from the current directory.
    // the search for a repository will respect $git_ceiling_directories and
    // $git_discovery_across_filesystem.  the opened repository will
    // respect $git_index_file, $git_namespace, $git_object_directory, and
    // $git_alternate_object_directories.
    // in the future, this flag will also cause `git_repository_open_ext`
    // to respect $git_work_tree and $git_common_dir; currently,
    // `git_repository_open_ext` with this flag will error out if either
    // $git_work_tree or $git_common_dir is set.
    from_env = (1 << 4),
  };

  // Find and open a repository with extended controls.
  static repository open_ext(const std::string &path, open_flag flags,
                             const std::string &ceiling_dirs);

  // Open working tree as a repository
  // Open the working directory of the working tree as a
  // normal repository that can then be worked on.
  static repository open_from_worktree(const worktree &wt);

  static repository clone(const std::string &url, const std::string &local_path,
                          const clone::options &options = clone::options());

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
  void detach_head() const;

  // Look for a git repository and return path
  // Method will auto-detect if a repo is bare (if there is a repo)
  //
  // If across_fs is true, then the lookup will not stop when a
  // filesystem device change is detected while exploring parent
  // directories.
  //
  // ceiling_dirs: A GIT_PATH_LIST_SEPARATOR separated list of absolute
  // symbolic link free paths. The lookup will stop when any of this paths
  // is reached.
  static std::string discover_path(const std::string &start_path,
                                   bool across_fs,
                                   const std::string &ceiling_dirs);

  static std::string discover_path(const std::string &start_path);

  // Invoke 'visitor' for each entry in the given FETCH_HEAD file.
  // See git_repository_fetchhead_foreach_cb
  void for_each_fetch_head(
      std::function<void(const std::string &, const std::string &, const oid &,
                         bool)>
          visitor) const;

  // If a merge is in progress, invoke 'visitor'
  // for each commit ID in the MERGE_HEAD file.
  void for_each_merge_head(std::function<void(const oid &)> visitor) const;

  // Currently active namespace for this repo
  std::string namespace_() const;

  // Calculate hash of file using repo filtering rules
  oid hashfile(const std::string &path, object::object_type type,
               const std::string &as_path) const;

  // hashfile overload that uses as_path = path
  oid hashfile(const std::string &path, object::object_type type) const;

  // Retrieve and resolve the reference pointed at by HEAD.
  reference head() const;

  // Retrieve the referenced HEAD for the worktree
  reference head_for_worktree(const std::string &name) const;

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

  // Get the Object Database for this repository.
  cppgit2::odb odb() const;

  // Get the Reference Database Backend for this repository.
  // If a custom refsdb has not been set, the default database for the
  // repository will be returned (the one that manipulates loose and packed
  // references in the .git directory).
  cppgit2::refdb refdb() const;

  // Remove the message that the above message() call retrieves.
  void remove_message() const;

  // Make the repository HEAD point to the specified reference.
  void set_head(const std::string &refname) const;

  // Make the repository HEAD directly point to the Commit.
  void set_head_detached(const oid &commitish) const;

  // Make the repository HEAD directly point to the Commit.
  // This behaves like git_repository_set_head_detached() but takes an
  // annotated commit, which lets you specify which extended sha syntax
  // string was specified by a user, allowing for more exact reflog messages.
  void set_head_detached(const annotated_commit &commitish) const;

  // Set the identity to be used for writing reflogs
  void set_identity(const std::string &name, const std::string &email) const;

  // Unset the identity used for writing reflogs
  // When unset, the identity will be taken from the repository's configuration.
  void unset_identity() const;

  // Sets the active namespace for this Git Repository
  // This should not include the refs folder, e.g. to namespace all
  // references under `refs/namespaces/foo/`, use `foo` as the namespace.
  void set_namespace(const std::string &nmspace) const;

  // Set the path to the working directory for this repository
  // The working directory doesn't need to be the same one that contains the
  // .git folder for this repository.
  void set_workdir(const std::string &workdir, bool update_gitlink) const;

  // Remove all the metadata associated with an ongoing command like merge,
  // revert, cherry-pick, etc. For example: MERGE_HEAD, MERGE_MSG, etc.
  void cleanup_state() const;

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

  // Create a "fake" repository to wrap an object database
  // Create a repository object to wrap an object database to be used with the
  // API when all you have is an object database. This doesn't have any paths
  // associated with it, so use with care.
  static repository wrap_odb(const cppgit2::odb &odb);

  // Access to libgit2 C ptr
  const git_repository *c_ptr() const;

  /*
   * Annotated Commit API
   * See git_annotated_commit_* functions
   */

  // Creates an annotated_commit from the given fetch head data
  annotated_commit create_annotated_commit(const std::string &branch_name,
                                           const std::string &remote_url,
                                           const oid &id) const;

  // Create annotated commit from revspec
  // (the extended sha syntax string to use to lookup the commit)
  annotated_commit create_annotated_commit(const std::string &revspec) const;

  // Creates an annotated_commit from the given reference (
  // (used to lookup the git_annotated_commit)
  annotated_commit create_annotated_commit(const reference &ref) const;

  // Lookup annotated_commit from the given commit id
  annotated_commit lookup_annotated_commit(const oid &id) const;

  /*
   * Apply API
   * See git_apply_* functions
   */

  // Apply a git_diff to the given repository, making changes directly in the
  // working directory, the index, or both.
  void apply_diff(const diff &diff, apply::location location,
                  const apply::options &options = apply::options()) const;

  // Apply a git_diff to a git_tree, and return the resulting image as an index.
  cppgit2::index apply_diff(const tree &preimage, const diff &diff,
                            const apply::options &options = apply::options()) const;

  /*
   * Attributes API
   * See git_attr_* functions
   */

  // Add a macro definition.
  // Macros will automatically be loaded from the top level .gitattributes file
  // of the repository (plus the build-in "binary" macro). This function allows
  // you to add others. For example, to add the default macro, you would call:
  //     repo.add_attributes_macro("binary", "-diff -crlf");
  void add_attributes_macro(const std::string &name, const std::string &values) const;

  // Flush the gitattributes cache.
  //
  // Call this if you have reason to believe that the attributes files on disk
  // no longer match the cached contents of memory. This will cause the
  // attributes files to be reloaded the next time that an attribute access
  // function is called.
  void flush_attributes_cache() const;

  // Loop over all the git attributes for a path.
  void for_each_attribute(
      attribute::flag flags, const std::string &path,
      std::function<void(const std::string &, const std::string &)> visitor) const;

  // Look up the value of one git attribute for path.
  std::string lookup_attribute(attribute::flag flags, const std::string &path,
                               const std::string &name) const;

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
                             const std::vector<std::string> &names) const;

  /*
   * BLAME API
   * See git_blame_* functions
   */
  blame blame_file(const std::string &path,
                   blame::options options = blame::options()) const;

  /*
   * BLOB API
   * See git_blob_* functions
   */

  // Write an in-memory buffer to the ODB as a blob
  oid create_blob_from_buffer(const std::string &buffer) const;

  // Read a file from the filesystem and write its content to the Object
  // Database as a loose blob
  oid create_blob_from_disk(const std::string &path) const;
  // Read a file from the working folder of a repository
  // and write it to the Object Database as a loose blob
  oid create_blob_from_workdir(const std::string &relative_path) const;

  // Lookup a blob object from a repository.
  blob lookup_blob(const oid &id) const;

  // Lookup a blob object from a repository,
  // given a prefix of its identifier (short id).
  blob lookup_blob(const oid &id, size_t len) const;

  /*
   * BRANCH API
   * See git_branch_* functions
   */

  // Create a new branch pointing at a target commit
  reference create_branch(const std::string &branch_name, const commit &target,
                          bool force) const;

  // Create a new branch pointing at a target commit
  // Takes an annotated commit, which lets you specify which extended
  // sha syntax string was specified by a user, allowing for more exact reflog
  // messages.
  reference create_branch(const std::string &branch_name,
                          const annotated_commit &commit, bool force) const;

  // Delete an existing branch given its name
  void delete_branch(const reference &ref) const;
  void
  delete_branch(const std::string &branch_name,
                branch::branch_type branch_type = branch::branch_type::local) const;

  // Determine if any HEAD points to the current branch
  bool is_branch_checked_out(const reference &ref) const;

  // Determine if any HEAD points to the current branch
  bool is_branch_checked_out(
      const std::string &branch_name,
      branch::branch_type branch_type = branch::branch_type::local) const;

  // Determine if HEAD points to the given branch
  bool is_head_pointing_to_branch(const reference &ref) const;

  // Determine if HEAD points to the given branch
  bool is_head_pointing_to_branch(
      const std::string &branch_name,
      branch::branch_type branch_type = branch::branch_type::local) const;

  // Move/rename an existing local branch reference.
  reference rename_branch(const reference &ref,
                          const std::string &new_branch_name, bool force) const;

  // Move/rename an existing local branch reference.
  reference
  rename_branch(const std::string &branch_name,
                const std::string &new_branch_name, bool force,
                branch::branch_type branch_type = branch::branch_type::local) const;

  // Get the branch name
  std::string branch_name(const reference &branch) const;

  // Find the remote name of a remote-tracking branch
  // This will return the name of the remote whose fetch refspec is matching
  // the given branch. E.g. given a branch "refs/remotes/test/master", it will
  // extract the "test" part. If refspecs from multiple remotes match,
  // the function will return GIT_EAMBIGUOUS.
  std::string branch_remote_name(const std::string &refname) const;

  // Set a branch's upstream branch
  void set_branch_upstream(const reference &ref,
                           const std::string &upstream_name) const;

  // Set a branch's upstream branch
  void set_branch_upstream(const std::string &branch_name,
                           const std::string &upstream_name) const;

  // Unset the branch's upstream branch
  void unset_branch_upstream(const reference &ref) const;

  // Unset the branch's upstream branch
  void unset_branch_upstream(const std::string &branch_name) const;

  // Get the upstream of a branch
  reference branch_upstream(const reference &local_branch) const;

  // Get the upstream of a branch
  reference branch_upstream(const std::string &local_branch_name) const;
  // Get the upstream name of a branch
  //
  // Given a local branch, this will return its remote-tracking branch
  // information, as a full reference name, ie. "feature/nice" would become
  // "refs/remote/origin/feature/nice", depending on that branch's
  // configuration.
  std::string branch_upstream_name(const std::string &refname) const;

  // Retrieve the upstream remote of a local branch
  //
  // This will return the currently configured "branch.*.remote" for a given
  // branch. This branch must be local.
  std::string branch_upstream_remote(const std::string &refname) const;

  // Lookup a branch by its name in a repository.
  //
  // The branch name will be checked for validity.
  reference lookup_branch(const std::string &branch_name,
                          branch::branch_type branch_type) const;

  // Run visitor function for each branch in the repository
  void
  for_each_branch(std::function<void(const reference &)> visitor,
                  branch::branch_type branch_type = branch::branch_type::local) const;

  /*
   * CHECKOUT API
   * See git_checkout_* functions
   */

  // Updates files in the index and the working tree to match
  // the content of the commit pointed at by HEAD.
  //
  // NOTE: repository must be non-bare
  void checkout_head(const checkout::options &options = checkout::options()) const;

  // Updates files in the working tree to match the content of the index.
  //
  // NOTE: repository must be non-bare
  void checkout_index(const cppgit2::index &index,
                      const checkout::options &options = checkout::options()) const;

  // Updates files in the index and working tree to match the
  // content of the tree pointed at by the treeish.
  void checkout_tree(const object &treeish,
                     const checkout::options &options = checkout::options()) const;

  /*
   * CHERRYPICK API
   * See git_cherrypick_* functions
   */

  // Cherry-pick the given commit, producing changes in the index and working
  // directory.
  void cherrypick_commit(
      const commit &commit,
      const cherrypick::options &options = cherrypick::options(nullptr)) const;

  // Cherry-picks the given commit against the given "our" commit, producing an
  // index that reflects the result of the cherry-pick.
  cppgit2::index cherrypick_commit(
      const commit &cherrypick_commit, const commit &our_commit,
      unsigned int mainline,
      const merge::options &merge_options = merge::options(nullptr)) const;

  /*
   * COMMIT API
   * See git_commit_* functions
   */

  // Create new commit in the repository from a list of git objects
  oid create_commit(const std::string &update_ref, const signature &author,
                    const signature &committer,
                    const std::string &message_encoding,
                    const std::string &message, const tree &tree,
                    const std::vector<commit> &parents) const;

  // Create a commit and write it into a buffer
  data_buffer create_commit(const signature &author, const signature &committer,
                            const std::string &message_encoding,
                            const std::string &message, const tree &tree,
                            const std::vector<commit> &parents) const;

  // Create a commit object from the given buffer and signature
  oid create_commit(const std::string &commit_content,
                    const std::string &signature = "",
                    const std::string &signature_field = "gpgsig") const;

  // Extract the signature from a commit
  std::pair<data_buffer, data_buffer>
  extract_signature_from_commit(oid id,
                                const std::string &signature_field = "gpgsig") const;

  // Lookup a commit object from a repository.
  commit lookup_commit(const oid &id) const;

  // Lookup a commit object from a repository given a prefix (short ID)
  commit lookup_commit(const oid &id,
                       size_t length /* length of short identifier*/) const;

  // The following for_each methods are convenience versions of
  // for_each_revision where the commit (instead of the oid) is passed to the
  // visitor function

  // Run operation for each commit in the repository
  void for_each_commit(std::function<void(const commit &id)> visitor,
                       revision::sort sort_ordering = revision::sort::none) const;

  // Run operation for each commit in the repository
  void for_each_commit(std::function<void(const commit &id)> visitor,
                       const commit &start_from,
                       revision::sort sort_ordering = revision::sort::none) const;

  /*
   * CONFIG API
   * See git_config_* functions
   */
  void add_ondisk_config_file(const cppgit2::config &cfg,
                              const std::string &path,
                              config::priority_level level, bool force) const;

  /*
   * DIFF API
   * See git_diff_* functions
   */

  // Create an e-mail ready patch for a commit.
  data_buffer create_diff_commit_as_email(
      const commit &commit, size_t patch_no, size_t total_patches,
      diff::format_email_flag flags,
      const diff::options &options = diff::options(nullptr)) const;

  // Create a diff with the difference between two index objects.
  diff create_diff_index_to_index(
      const cppgit2::index &old_index, const cppgit2::index &new_index,
      const diff::options &options = diff::options(nullptr)) const;

  // Create a diff between the repository index and the workdir directory.
  diff create_diff_index_to_workdir(
      const cppgit2::index &index,
      const diff::options &options = diff::options(nullptr)) const;

  // Create a diff between a tree and repository index.
  diff create_diff_tree_to_index(
      const tree &old_tree, const cppgit2::index &index,
      const diff::options &options = diff::options(nullptr)) const;

  // Create a diff with the difference between two tree objects.
  diff create_diff_tree_to_tree(
      const tree &old_tree, const tree &new_tree,
      const diff::options &options = diff::options(nullptr)) const;

  // Create a diff between a tree and the working directory.
  diff create_diff_tree_to_workdir(
      const tree &old_tree,
      const diff::options &options = diff::options(nullptr)) const;

  // Create a diff between a tree and the working directory using index data to
  // account for staged deletes, tracked files, etc.
  diff create_diff_tree_to_workdir_with_index(
      const tree &old_tree,
      const diff::options &options = diff::options(nullptr)) const;

  /*
   * GRAPH API
   * See git_graph_* functions
   */

  // Count the number of unique commits between two commit objects
  //
  // There is no need for branches containing the commits to have any upstream
  // relationship, but it helps to think of one as a branch and the other as its
  // upstream, the ahead and behind values will be what git would report for the
  // branches.
  std::pair<size_t, size_t>
  unique_commits_ahead_behind(const oid &local, const oid &upstream) const;

  // Determine if a commit is the descendant of another commit.
  // Note that a commit is not considered a descendant of itself, in contrast to
  // git merge-base --is-ancestor.
  bool is_descendant_of(const oid &commit, const oid &ancestor) const;

  /*
   * IGNORE API
   * See git_ignore_* functions
   */

  // Add ignore rules for a repository.
  //   Text of rules, a la the contents of a .gitignore file.
  //   It is okay to have multiple rules in the text;
  //   if so, each rule should be terminated with a newline.
  //
  // Example usage:
  //   repo.add_ignore_rules("*.c/ with space");
  // This would add three rules to the ignores.
  void add_ignore_rules(const std::string &rules) const;

  // Clear ignore rules that were explicitly added.
  //
  // Resets to the default internal ignore rules.
  // This will not turn off rules in .gitignore files
  // that actually exist in the filesystem.
  void clear_ignore_rules() const;

  // Test if the ignore rules apply to a given path.
  //
  // This function checks the ignore rules to see if they
  /// would apply to the given file. This indicates if the
  // file would be ignored regardless of whether the file is
  // already in the index or committed to the repository.
  bool is_path_ignored(const std::string &path) const;

  /*
   * MERGE API
   * See git_merge_* functions
   */

  // Analyzes the given branch(es) and determines the opportunities for merging
  // them into the HEAD of the repository.
  std::pair<merge::analysis_result, merge::preference>
  analyze_merge(const std::vector<annotated_commit> &their_heads) const;

  // Analyzes the given branch(es) and determines the opportunities for merging
  // them into a reference.
  std::pair<merge::analysis_result, merge::preference>
  analyze_merge(const reference &our_ref,
                const std::vector<annotated_commit> &their_heads) const;

  // Find a merge base between two commits
  oid find_merge_base(const oid &first_commit, const oid &second_commit) const;

  // Find a merge base given a list of commits
  oid find_merge_base(const std::vector<oid> &commits) const;

  // Find a merge base in preparation for an octopus merge
  oid find_merge_base_for_octopus_merge(const std::vector<oid> &commits) const;

  // Find merge bases between two commits
  std::vector<oid> find_merge_bases(const oid &first_commit,
                                    const oid &second_commit) const;

  // Find all merge bases given a list of commits
  std::vector<oid> find_merge_bases(const std::vector<oid> &commits) const;

  // Merges the given commit(s) into HEAD, writing the results into the working
  // directory. Any changes are staged for commit and any conflicts are written
  // to the index. Callers should inspect the repository's index after this
  // completes, resolve any conflicts and prepare a commit.
  void merge_commits(
      const std::vector<annotated_commit> &their_heads,
      const merge::options &merge_options = merge::options(),
      const checkout::options &checkout_options = checkout::options()) const;

  // Merge two commits, producing a git_index that reflects the result of the
  // merge. The index may be written as-is to the working directory or checked
  // out. If the index is to be converted to a tree, the caller should resolve
  // any conflicts that arose as part of the merge.
  cppgit2::index
  merge_commits(const commit &our_commit, const commit &their_commit,
                const merge::options &merge_options = merge::options()) const;

  // Merge two files as they exist in the index, using the given common ancestor
  // as the baseline, producing a git_merge_file_result that reflects the merge
  // result. The git_merge_file_result must be freed with
  // git_merge_file_result_free.
  merge::file::result merge_file_from_index(
      const index::entry &ancestor, const index::entry &ours,
      const index::entry &theirs,
      const merge::file::options &options = merge::file::options()) const;

  // Merge two trees, producing a git_index that reflects the result of the
  // merge. The index may be written as-is to the working directory or checked
  // out. If the index is to be converted to a tree, the caller should resolve
  // any conflicts that arose as part of the merge.
  cppgit2::index merge_trees(const tree &ancestor_tree, const tree &our_tree,
                             const tree &their_tree,
                             const merge::options &options = merge::options()) const;

  /*
   * NOTE API
   * See git_note_* functions
   */

  // Add a note for an object
  oid create_note(const std::string &notes_ref, const signature &author,
                  const signature &committer, const oid &id,
                  const std::string &note, bool force) const;

  // Add a note for an object from a commit
  std::pair<oid, oid> create_note(const commit &parent, const signature &author,
                                  const signature &committer, const oid &id,
                                  const std::string &note,
                                  bool allow_note_override) const;

  // Read the note for an object
  note read_note(const std::string &notes_ref, const oid &id) const;

  // Read the note for an object from a note commit
  note read_note(const commit &notes_commit, const oid &id) const;

  // Remove the note for an object
  void remove_note(const std::string &notes_ref, const signature &author,
                   const signature &committer, const oid &id) const;

  // Remove the note for an object
  oid remove_note(const commit &notes_commit, const signature &author,
                  const signature &committer, const oid &id) const;

  // Get the default notes reference for a repository
  data_buffer detault_notes_reference() const;

  // Loop over all the notes within a specified namespace and
  // issue a callback for each one.
  void for_each_note(const std::string &notes_ref,
                     std::function<void(const oid &, const oid &)> visitor) const;

  /*
   * OBJECT API
   * See git_object_* functions
   */

  // Lookup a reference to one of the objects in a repository.
  // The generated reference is owned by the repository and
  // should be closed with the git_object_free method instead of free'd
  // manually.
  object lookup_object(const oid &id, object::object_type type) const;

  // Lookup a reference to one of the objects in a repository,
  // given a prefix of its identifier (short id).
  object lookup_object(const oid &id, size_t length,
                       object::object_type type) const;

  // Lookup an object that represents a tree entry.
  // Path is the relative path from the root object to the desired object
  object lookup_object(const object &treeish, const std::string &path,
                       object::object_type type) const;

  /*
   * PACKBUILDER API
   * See git_packbuilder_* functions
   */

  // Initialize a new packbuilder
  pack_builder initialize_pack_builder() const;

  /*
   * REBASE API
   * See git_rebase_* functions
   */

  // Initializes a rebase operation to rebase the changes in branch relative
  // to upstream onto another branch. To begin the rebase process,
  // call git_rebase_next.
  //
  // Owned by user
  rebase init_rebase(const annotated_commit &branch,
                     const annotated_commit &upstream,
                     const annotated_commit &onto,
                     const rebase::options &options = rebase::options()) const;

  // Opens an existing rebase that was previously started by either an
  // invocation of git_rebase_init or by another client.
  rebase open_rebase(const rebase::options &options = rebase::options()) const;

  /*
   * REFDB API
   * See git_refdb_* functions
   */

  // Create a new reference database with no backends.
  // Before the Ref DB can be used for read/writing, a custom database
  // backend must be manually set using git_refdb_set_backend()
  cppgit2::refdb create_refdb() const;

  // Create a new reference database and automatically add the default backends:
  cppgit2::refdb open_refdb() const;

  /*
   * REFERENCE API
   * See git_reference_* functions
   */

  // Create a new direct reference.
  reference create_reference(const std::string &name, const oid &id, bool force,
                             const std::string &log_message) const;

  // Conditionally create new direct reference
  reference create_reference(const std::string &name, const oid &id, bool force,
                             const oid &current_id,
                             const std::string &log_message) const;

  // Delete an existing reference by name
  void delete_reference(const std::string &refname) const;

  // Ensure there is a reflog for a particular reference.
  void ensure_reflog_for_reference(const std::string &refname) const;

  // Check if a reflog exists for the specified reference.
  bool reference_has_reflog(const std::string &refname) const;

  // Fill a list with all the references that can be found in a repository.
  // The string array will be filled with the names of all references
  strarray reference_list() const;

  // Lookup a reference by name in a repository.
  reference lookup_reference(const std::string &refname) const;

  // Lookup a reference by DWIMing its short name
  reference lookup_reference_by_dwim(const std::string &shorthand_name) const;

  // Lookup a reference by name and resolve immediately to OID.
  oid reference_name_to_id(const std::string &refname) const;

  // Create a new symbolic reference.
  reference create_symbolic_reference(const std::string &name,
                                      const std::string &target, bool force,
                                      const std::string &log_message) const;

  // Conditionally create a new symbolic reference.
  reference create_symbolic_reference(const std::string &name,
                                      const std::string &target, bool force,
                                      const std::string &current_value,
                                      const std::string &log_message) const;

  // Perform a callback on each reference in the repository.
  void for_each_reference(std::function<void(const reference &)> visitor) const;

  // Callback used to iterate over reference names
  void
  for_each_reference_name(std::function<void(const std::string &)> visitor) const;

  // Perform a callback on each reference in the repository
  // whose name matches the given pattern.
  void
  for_each_reference_glob(const std::string &glob,
                          std::function<void(const std::string &)> visitor) const;

  /*
   * REFLOG API
   * See git_reflog_* functions
   */

  // Delete the reflog for the given reference
  void delete_reflog(const std::string &name) const;

  // Read the reflog for the given reference
  reflog read_reflog(const std::string &name) const;

  // Rename a reflog
  void rename_reflog(const std::string &old_name, const std::string &name) const;

  /*
   * RESET API
   * See git_reset_* functions
   */

  // Sets the current head to the specified commit oid
  // and optionally resets the index and working tree to match.
  void reset(const object &target, reset::reset_type reset_type,
             const checkout::options &options = checkout::options()) const;

  // Updates some entries in the index from the target commit tree.
  void reset_default(const object &target,
                     const std::vector<std::string> &pathspecs) const;

  // Sets the current head to the specified annotated commit oid
  // and optionally resets the index and working tree to match.
  void reset(const annotated_commit &target, reset::reset_type reset_type,
             const checkout::options &options = checkout::options()) const;

  /*
   * REMOTE API
   * See git_remote_* functions
   */

  // Add a fetch refspec to the remote's configuration
  void add_fetch_refspec_to_remote(const std::string &remote,
                                   const std::string &refspec) const;

  // Add a push refspec to the remote's configuration
  void add_push_refspec_to_remote(const std::string &remote,
                                  const std::string &refspec) const;

  // Add a remote with the default fetch refspec to the repository's
  // configuration.
  remote create_remote(const std::string &name, const std::string &url) const;

  // Create an anonymous remote
  remote create_anonymous_remote(const std::string &url) const;

  // Add a remote with the provided fetch refspec
  //  to the repository's configuration.
  remote create_remote(const std::string &name, const std::string &url,
                       const std::string &fetch_refspec) const;

  // Delete an existing persisted remote.
  // All remote-tracking branches and configuration
  // settings for the remote will be removed.
  void delete_remote(const std::string &name) const;

  // Get a list of the configured remotes for a repo
  strarray remote_list() const;

  // Get the information for a particular remote
  remote lookup_remote(const std::string &name) const;

  // Give the remote a new name
  // All remote-tracking branches and configuration settings for the remote are
  // updated.
  //
  // @return:
  //   non-default refspecs cannot be renamed and will be stored here
  //   for further processing by the caller. Always free this strarray
  //   on successful return.
  strarray rename_remote(const std::string &name, const std::string &new_name) const;

  // Set the remote's tag following setting.
  // The change will be made in the configuration. No loaded remotes will be
  // affected.
  void set_remote_autotag(const std::string &remote,
                          fetch::options::autotag option) const;

  // Set the remote's url for pushing in the configuration.
  // Remote objects already in memory will not be affected.
  // This assumes the common case of a single-url remote and
  // will otherwise return an error.
  void set_remote_push_url(const std::string &remote, const std::string &url) const;

  // Set the remote's url in the configuration
  void set_remote_url(const std::string &remote, const std::string &url) const;

  /*
   * REVERT API
   */

  // Reverts the given commit, producing changes
  // in the index and working directory.
  void revert_commit(const commit &commit,
                     const revert::options &options = revert::options()) const;

  // Reverts the given commit against the given "our" commit,
  // producing an index that reflects the result of the revert.
  cppgit2::index
  revert_commit(const commit &revert_commit, const commit &our_commit,
                unsigned int mainline,
                const merge::options &options = merge::options()) const;

  /*
   * REVPARSE API
   * See git_revparse_* functions
   */

  // Parse a revision string for from, to, and intent.
  // See `man gitrevisions` for more information
  revspec revparse(const std::string &spec) const;

  // Find a single object and intermediate reference by a revision string.
  // In some cases (@{<-n>} or <branchname>@{upstream}), the expression may
  // point to an intermediate reference. When such expressions are being passed
  // in, reference_out will be valued as well.
  std::pair<object, reference>
  revparse_to_object_and_reference(const std::string &spec) const;

  // Find a single object, as specified by a revision string.
  object revparse_to_object(const std::string &spec) const;

  /*
   * REVWALK API
   * See git_revwalk_* functions
   */

  // Allocate a new revision walker to iterate through a repo.
  //
  // This revision walker uses a custom memory pool and an internal commit
  // cache, so it is relatively expensive to allocate.
  revwalk create_revwalk() const;

  /*
   * SIGNATURE API
   * See git_signature_* functions
   */
  signature default_signature() const;

  /*
   * STASH API
   * See git_stash_* functions
   */

  // Apply a single stashed state from the stash list.
  // index is the position within the stash list.
  // -> 0 points to the most recent stashed state.
  void
  apply_stash(size_t index,
              const stash::apply::options &options = stash::apply::options()) const;

  // Remove a single stashed state from the stash list.
  // index is the position within the stash list.
  // -> 0 points to the most recent stashed state.
  void drop_stash(size_t index) const;

  // Loop over all the stashed states and issue a callback for each one.
  void for_each_stash(
      std::function<void(size_t, const std::string &, const oid &)> visitor)
      const;

  // Apply a single stashed state from the stash list
  // and remove it from the list if successful.
  void
  pop_stash(size_t index,
            const stash::apply::options &options = stash::apply::options()) const;

  // Save the local modifications to a new stash.
  oid save_stash(const signature &stasher, const std::string &message,
                 stash::apply::flag flags) const;

  /*
   * STATUS API
   * See git_status_* functions
   */

  // Get file status for a single file.
  status::status_type status_file(const std::string &path) const;

  // Gather file statuses and run a callback for each one.
  void for_each_status(
      std::function<void(const std::string &, status::status_type)> visitor) const;

  // Gather file status information and run callbacks as requested.
  void for_each_status(
      const status::options &options,
      std::function<void(const std::string &, status::status_type)> visitor) const;

  // Gather file status information and populate the git_status_list.
  status::list status_list(const status::options &options = status::options()) const;

  // Test if the ignore rules apply to a given file.
  // Returns false if the file is not ignored, 1 if it is
  //
  // One way to think of this is if you were to do
  // "git add ." on the directory containing the file,
  // would it be added or not?
  bool should_ignore(const std::string &path) const;

  /*
   * SUBMODULE API
   * See git_submodule_* functions
   */

  // Set up a new git submodule for checkout.
  //
  // This does "git submodule add" up to the fetch and checkout of the submodule
  // contents. It preps a new submodule, creates an entry in .gitmodules and
  // creates an empty initialized repository either at the given path in the
  // working directory or in .git/modules with a gitlink from the working
  // directory to the new repo.
  //
  // To fully emulate "git submodule add" call this function, then open the
  // submodule repo and perform the clone step as needed (if you don't need
  // anything custom see git_submodule_add_clone()). Lastly, call
  // git_submodule_add_finalize() to wrap up adding the new submodule and
  // .gitmodules to the index to be ready to commit.
  submodule setup_submodule(const std::string &url, const std::string &path,
                            bool use_gitlink) const;

  // Iterate over all tracked submodules of a repository.
  void for_each_submodule(
      std::function<void(const submodule &, const std::string &)> visitor) const;

  // Lookup submodule information by name or path.
  submodule lookup_submodule(const std::string &name) const;

  // Resolve a submodule url relative to the given repository.
  data_buffer resolve_submodule_url(const std::string &url) const;

  // Set the branch for the submodule in the configuration
  void set_submodule_branch(const std::string &submodule_name,
                            const std::string &branch_name) const;

  // Set the fetchRecurseSubmodules rule for a submodule in the configuration
  void set_submodule_fetch_recurse_option(
      const std::string &submodule_name,
      submodule::recurse fetch_recurse_submodules) const;

  // Set the ignore rule for the submodule in the configuration
  void set_submodule_ignore_option(const std::string &submodule_name,
                                   submodule::ignore ignore) const;

  // Set the update rule for the submodule in the configuration
  void set_submodule_update_option(const std::string &submodule_name,
                                   submodule::update_strategy update) const;

  // Set the URL for the submodule in the configuration
  void set_submodule_url(const std::string &submodule_name,
                         const std::string &submodule_url) const;

  // Get the status for a submodule.
  submodule::status submodule_status(const std::string &name,
                                     submodule::ignore ignore) const;

  /*
   * TAG API
   * See git_tag_* funcitons
   */

  // Create a new tag in the object database pointing to a git_object
  oid create_tag_annotation(const std::string &tag_name, const object &target,
                            const signature &tagger,
                            const std::string &message) const;

  // Create a new tag in the repository from an object
  oid create_tag(const std::string &tag_name, const object &target,
                 const signature &tagger, const std::string &message,
                 bool force) const;

  // Create a new tag in the repository from a buffer
  oid create_tag(const std::string &buffer, bool force) const;

  // Create a new lightweight tag pointing at a target object
  oid create_lightweight_tag(const std::string &tag_name, const object &target,
                             bool force) const;

  // Delete an existing tag reference.
  void delete_tag(const std::string &tag_name) const;

  // Callback used to iterate over tags
  void
  for_each_tag(std::function<void(const std::string &, const oid &)> visitor) const;

  // Fill a list with all the tags in the Repository
  // The string array will be filled with the names of the matching tags;
  // these values are owned by the user.
  strarray tags() const;

  // Fill a list with all the tags in the Repository which name match a defined
  // pattern
  strarray tags_that_match(const std::string &pattern) const;

  // Lookup a tag object from the repository.
  tag lookup_tag(const oid &id) const;

  // Lookup a tag object from the repository, given a prefix of its identifier
  // (short id).
  tag lookup_tag(const oid &id, size_t length) const;

  /*
   * TRANSACTION API
   * See git_transaction_* functions
   */
  transaction create_transaction() const;

  /*
   * TREE API
   * See git_tree_* functions
   */

  // Convert a tree entry to the git_object it points to.
  object tree_entry_to_object(const tree::entry &entry) const;

  // Lookup a tree object from the repository.
  tree lookup_tree(const oid &id) const;

  // Lookup a tree object from the repository, given a prefix of its identifier
  // (short id).
  tree lookup_tree(const oid &id, size_t length) const;

  // Create a tree based on another one with the specified modifications
  oid create_updated_tree(const tree &baseline,
                          std::vector<tree::update> updates) const;

  /*
   * WORKTREE API
   * See git_worktree_* functions
   */

  // Add a new working tree
  worktree
  add_worktree(const std::string &name, const std::string &path,
               const worktree::add_options &options = worktree::add_options()) const;

  // List names of linked working trees
  // Returned array is owned by the user
  strarray list_worktrees() const;

  // Lookup a working tree by its name for a given repository
  worktree lookup_worktree(const std::string &name) const;

  // Open a worktree of a given repository
  // If a repository is not the main tree but a worktree,
  // this function will look up the worktree inside the parent
  // repository and create a new git_worktree structure.
  worktree open_worktree() const;

private:
  friend class index;
  friend class pathspec;
  friend class remote;
  friend class submodule;
  friend class tree_builder;
  git_repository *c_ptr_;
};
ENABLE_BITMASK_OPERATORS(repository::init_flag);
ENABLE_BITMASK_OPERATORS(repository::open_flag);

} // namespace cppgit2
