#pragma once
#include <cppgit2/bitmask_operators.hpp>
#include <cppgit2/data_buffer.hpp>
#include <cppgit2/fetch.hpp>
#include <cppgit2/indexer.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/ownership.hpp>
#include <cppgit2/push.hpp>
#include <cppgit2/refspec.hpp>
#include <cppgit2/strarray.hpp>
#include <git2.h>
#include <string>

namespace cppgit2 {

class remote : public libgit2_api {
public:
  // Default construct a remote object
  remote();

  // Construct from libgit2 C ptr
  remote(git_remote *c_ptr, ownership owner = ownership::libgit2);

  // Cleanup remote object
  ~remote();

  // Retrieve the tag auto-follow setting
  fetch::options::autotag autotag_option();

  // Check whether the remote's underlying transport
  // is connected to the remote host.
  bool is_connected() const;

  // Remote creation options flags
  enum class create_flag {
    // Ignore the repository apply.insteadOf configuration
    skip_insteadof = (1 << 0),
    // Don't build a fetchspec from the name if none is set
    default_fetchspec = (1 << 1)
  };

  class create_options : public libgit2_api {
  public:
    create_options() : c_ptr_(nullptr) {
      auto ret =
          git_remote_create_init_options(&default_options_, GIT_REMOTE_CREATE_OPTIONS_VERSION);
      c_ptr_ = &default_options_;
      if (ret != 0)
        throw git_exception();
    }

    create_options(git_remote_create_options *c_ptr) : c_ptr_(c_ptr) {}

    // Version
    unsigned int version() const { return c_ptr_->version; }
    void set_version(unsigned int version) { c_ptr_->version = version; }

    // Repository
    class repository repository() const;
    void set_repository(const class repository &repo);

    // Name
    std::string name() const { return c_ptr_->name ? std::string(c_ptr_->name) : ""; }
    void set_name(const std::string &name) { c_ptr_->name = name.c_str(); }

    // Fetchspec
    std::string fetchspec() const { return c_ptr_->fetchspec ? std::string(c_ptr_->fetchspec) : ""; }
    void set_fetchspec(const std::string &fetchspec) { c_ptr_->fetchspec = fetchspec.c_str(); }

    // Flags
    create_flag flags() const { return static_cast<create_flag>(c_ptr_->flags); }
    void set_flags(const create_flag &flags) {
      c_ptr_->flags = static_cast<unsigned int>(flags);
    }

    // Access libgit2 C ptr
    const git_remote_create_options *c_ptr() const { return c_ptr_; }

  private:
    git_remote_create_options *c_ptr_;
    git_remote_create_options default_options_;
  };

  // Create a copy of an existing remote.
  // All internal strings are also duplicated.
  // Callbacks are not duplicated.
  remote copy() const;

  // Create a remote without a connected local repo
  // You can use this when you have a URL instead of a remote's name.
  //
  // Contrasted with repository::create_anonymous_remote,
  // a detached remote will not consider any repo configuration
  // values (such as instead of url substitutions).
  static remote create_detached_remote(const std::string &url);

  // Create a remote, with options.
  // This function allows more fine-grained control over the remote creation.
  static remote create_remote(const std::string &url, 
    const create_options &options = create_options());

  // Retrieve the name of the remote's default branch
  // This function must only be called after connecting.
  data_buffer default_branch() const;

  // Disconnect from the remote
  void disconnect();

  // Download and index the packfile
  void download(const strarray &refspecs,
                const fetch::options &options = fetch::options());

  // Download new data and update tips
  void fetch(const strarray &refspecs, const std::string &reflog_message,
             const fetch::options &options = fetch::options());

  // Get the remote's list of fetch refspecs
  // The memory is owned by the user and should be freed
  strarray fetch_refspec() const;

  // Get the remote's list of push refspecs
  // The memory is owned by the user and should be freed
  strarray push_refspec() const;

  // Ensure the remote name is well-formed.
  static bool is_valid_name(const std::string &name);

  // Get the remote's name
  std::string name() const;

  // Get the remote's repository
  class repository owner() const;

  // Retrieve the ref-prune setting
  void prune_references();

  // Peform all the steps from a push.
  void push(const strarray &refspecs,
            const push::options &options = push::options());

  // Get the remote's url for pushing
  // If url.*.pushInsteadOf has been configured for this URL,
  // it will return the modified URL.
  std::string push_url() const;

  // Get the number of refspecs for a remote
  size_t size() const;

  // Get a refspec from the remote
  refspec operator[](size_t n);

  // Get the statistics structure that is filled in by the fetch operation.
  indexer::progress stats() const;

  // Cancel the operation
  // At certain points in its operation, the network code checks
  // whether the operation has been cancelled and if so stops the operation.
  void stop();

  // Create a packfile and send it to the server
  //
  // Connect to the remote if it hasn't been done yet,
  // negotiate with the remote git which objects are missing,
  // create a packfile with the missing objects and send it.
  void upload(const strarray &refspecs,
              const push::options &options = push::options());

  // Get the remote's url
  std::string url() const;

private:
  friend class repository;
  ownership owner_;
  git_remote *c_ptr_;
};
ENABLE_BITMASK_OPERATORS(remote::create_flag);

} // namespace cppgit2