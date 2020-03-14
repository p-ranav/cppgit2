#pragma once
#include <cppgit2/data_buffer.hpp>
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/ownership.hpp>
#include <cppgit2/transaction.hpp>
#include <functional>
#include <git2.h>
#include <string>

namespace cppgit2 {

class config : public libgit2_api {
public:
  config();
  config(git_config *c_ptr, ownership owner = ownership::libgit2);
  ~config();

  // Priority levels correspond to natural escalation logic
  // when searching for config entries in git
  enum class priority_level {
    program_data = 1,
    system = 2,
    xdg = 3,
    global = 4,
    locla = 5,
    app = 6,
    highest = -1
  };

  // An entry in the configuration file
  class entry : public libgit2_api {
  public:
    // Default constructed config entry
    entry() : c_ptr_(nullptr), owner_(ownership::libgit2) {}

    // Construct from libgit2 C ptr
    // If owned by user, free'd in destructor
    entry(git_config_entry *c_ptr, ownership owner = ownership::libgit2)
        : c_ptr_(c_ptr), owner_(owner) {}

    // Free config entry if needed
    ~entry() {
      if (c_ptr_ && owner_ == ownership::user)
        git_config_entry_free(c_ptr_);
    }

    // Name of the entry (normalized)
    std::string name() const {
      if (c_ptr_->name)
        return std::string(c_ptr_->name);
      else
        return "";
    }

    // String value of the entry
    std::string value() const {
      if (c_ptr_->value)
        return std::string(c_ptr_->value);
      else
        return "";
    }

    // Depth of includes where this variable was found
    unsigned int include_depth() const { return c_ptr_->include_depth; }

    // Which config file was this entry found in
    config::priority_level priority_level() const {
      return static_cast<config::priority_level>(c_ptr_->level);
    }

    // Access to libgit2 C ptr
    const git_config_entry *c_ptr() const { return c_ptr_; }

  private:
    friend config;
    git_config_entry *c_ptr_;
    ownership owner_;
  };

  // Delete a config variable from the config file with the
  // highest level (usually the local one)
  void delete_entry(const std::string &name);

  // Remove one or several entries from a multivar in the
  // local config file
  void delete_entry(const std::string &name, const std::string &regexp);

  // Locate path to the global config file
  static std::string locate_global_config();

  // Look for config file in %PROGRAMDATA% used by portable git.
  static std::string locate_global_config_in_programdata();

  // If /etc/gitconfig doesn't exist, checks in %PROGRAMFILES%
  static std::string locate_global_system_config();

  static config new_config();

  // Open the global, XDG, and system config files
  // into a single prioritized config object
  static config open_default_config();

  // Open global/XDG config file according to git's rules
  // Git allows to store global config in $HOME/.gitconfig
  // or $XDG_CONFIG_HOME/git/config.
  static config open_global_config(config &conf);

  // Build a single-level focused config object from
  // a multi-level one. The returned config object can be used
  // to perform get/set/delete operations on a specific level.
  static config open_config_at_level(const config &parent,
                                     priority_level level);

  // Locate the path to the global xdg-compatible config file
  // Usually located in $HOME/.config/git/config
  static std::string locate_global_xdg_compatible_config();

  // Get the config entry of a config variable
  entry operator[](const std::string &name);

  // Getters
  bool value_as_bool(const std::string &name);
  int32_t value_as_int32(const std::string &name);
  int64_t value_as_int64(const std::string &name);
  std::string value_as_string(const std::string &name);
  data_buffer value_as_data_buffer(const std::string &name);

  // Value of a path config variable
  // A leading '~' will be expanded to the global search path
  std::string path(const std::string &name);

  // Insert entry
  void insert_entry(const std::string &name, bool value);
  void insert_entry(const std::string &name, int32_t value);
  void insert_entry(const std::string &name, int64_t value);
  void insert_entry(const std::string &name, const std::string &value);

  // Set a multivar in the local config file
  void insert_multiple(const std::string &name, const std::string &regexp,
                       const std::string &new_value);

  // Lock the backend with the highest priority
  // Use returned transaction to commit or undo the changes
  transaction lock();

  // Parsers

  // Valid values:
  // true: 'true', 'yes', 'on', number != 0
  // false: 'false', 'no', 'off', 0
  static bool parse_as_bool(const std::string &value);

  // An optional value suffix of 'k', 'm', or 'g'
  // will cause the value to be multiplied by 1024,
  // 1048576, or 1073741824
  static int32_t parse_as_int32(const std::string &value);

  // Similar to parse_as_int32
  static int64_t parse_as_int64(const std::string &value);

  // A leading '~' will be expanded to the global search path
  // (which defaults to the user's home directory) but can
  // be overridden via git_libgit2_opts()
  static std::string parse_path(const std::string &value);

  // Create a snapshot of the configuration
  config snapshot();

  size_t size() const;

  // Perform an operation on each config variable
  void for_each(std::function<void(const entry &)> visitor);

  // Perform an operation on each config variable matching a regular expression.
  void for_each(const std::string &regexp,
                std::function<void(const entry &)> visitor);

  // Access libgit2 C ptr
  const git_config *c_ptr() const;

private:
  friend class repository;
  git_config *c_ptr_;
  ownership owner_;
};

} // namespace cppgit2
