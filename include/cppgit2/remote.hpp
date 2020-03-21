#pragma once
#include <cppgit2/bitmask_operators.hpp>
#include <cppgit2/connection_direction.hpp>
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

  class callbacks : public libgit2_api {
  public:
    callbacks() : c_ptr_(nullptr) {
      auto ret = git_remote_init_callbacks(&default_options_,
                                           GIT_REMOTE_CALLBACKS_VERSION);
      c_ptr_ = &default_options_;
      if (ret != 0)
        throw git_exception();
    }

    callbacks(git_remote_callbacks *c_ptr) : c_ptr_(c_ptr) {}

    // Access libgit2 C ptr
    const git_remote_callbacks *c_ptr() const { return c_ptr_; }

  private:
    git_remote_callbacks *c_ptr_;
    git_remote_callbacks default_options_;
  };

  // Open a connection to a remote
  // The transport is selected based on the URL. The direction argument is due
  // to a limitation of the git protocol (over TCP or SSH) which starts up a
  // specific binary which can only do the one or the other.
  void connect(connection_direction direction,
               const callbacks &remote_callbacks = callbacks(),
               const proxy::options &proxy_options = proxy::options(),
               const strarray &custom_headers = strarray(nullptr));

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
      auto ret = git_remote_create_init_options(
          &default_options_, GIT_REMOTE_CREATE_OPTIONS_VERSION);
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
    std::string name() const {
      return c_ptr_->name ? std::string(c_ptr_->name) : "";
    }
    void set_name(const std::string &name) { c_ptr_->name = name.c_str(); }

    // Fetchspec
    std::string fetchspec() const {
      return c_ptr_->fetchspec ? std::string(c_ptr_->fetchspec) : "";
    }
    void set_fetchspec(const std::string &fetchspec) {
      c_ptr_->fetchspec = fetchspec.c_str();
    }

    // Flags
    create_flag flags() const {
      return static_cast<create_flag>(c_ptr_->flags);
    }
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
  void fetch_(
      const strarray &refspecs, const std::string &reflog_message,
      const cppgit2::fetch::options &options = cppgit2::fetch::options());

  // Get the remote's list of fetch refspecs
  // The memory is owned by the user and should be freed
  strarray fetch_refspec() const;

  // Description of a reference advertised by a remote server, given out on `ls`
  // calls.
  class head : public libgit2_api {
  public:
    head(const git_remote_head *c_ptr) : c_struct_(*c_ptr) {}

    bool local() const { return c_struct_.local; }

    oid id() const { return oid(&c_struct_.oid); }

    oid lid() const { return oid(&c_struct_.loid); }

    std::string name() const {
      return c_struct_.name ? std::string(c_struct_.name) : "";
    }

    // If the server send a symref mapping for this ref, this will point to the
    // target.
    std::string symref_target() const {
      return c_struct_.symref_target ? std::string(c_struct_.symref_target)
                                     : "";
    }

  private:
    git_remote_head c_struct_;
  };

  // Ensure the remote name is well-formed.
  static bool is_valid_name(const std::string &name);

  // Get the remote repository's reference advertisement list
  std::vector<head> reference_advertisement_list();

  // Get the remote's name
  std::string name() const;

  // Get the remote's repository
  class repository owner() const;

  // Prune tracking refs that are no longer present on remote
  void prune(const callbacks &remote_callbacks = callbacks());

  // Retrieve the ref-prune setting
  void prune_references();

  // Peform all the steps from a push.
  void push(const strarray &refspecs,
            const push::options &options = push::options());

  // Get the remote's list of push refspecs
  // The memory is owned by the user and should be freed
  strarray push_refspec() const;

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

  // Update the tips to the new state
  void update_tips(const callbacks &remote_callbacks, bool update_fetchhead,
                   cppgit2::fetch::options::autotag download_tags,
                   const std::string &reflog_message);

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