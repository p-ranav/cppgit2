#pragma once
#include <cppgit2/data_buffer.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/ownership.hpp>
#include <cppgit2/push.hpp>
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

  // Check whether the remote's underlying transport
  // is connected to the remote host.
  bool is_connected() const;

  // Create a remote without a connected local repo
  // You can use this when you have a URL instead of a remote's name.
  //
  // Contrasted with repository::create_anonymous_remote,
  // a detached remote will not consider any repo configuration
  // values (such as instead of url substitutions).
  static remote create_detached_remote(const std::string &url);

  // Retrieve the name of the remote's default branch
  // This function must only be called after connecting.
  data_buffer default_branch() const;

  // Disconnect from the remote
  void disconnect();

  // Create a copy of an existing remote.
  // All internal strings are also duplicated.
  // Callbacks are not duplicated.
  remote copy() const;

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

  // Peform all the steps from a push.
  void push(const strarray &refspecs,
            const push::options &options = push::options());

  // Get the remote's url for pushing
  // If url.*.pushInsteadOf has been configured for this URL,
  // it will return the modified URL.
  std::string push_url() const;

  // Get the number of refspecs for a remote
  size_t size() const;

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

} // namespace cppgit2