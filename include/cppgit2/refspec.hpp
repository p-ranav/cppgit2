#pragma once
#include <cppgit2/connection_direction.hpp>
#include <cppgit2/data_buffer.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/ownership.hpp>
#include <git2.h>
#include <string>

namespace cppgit2 {

class refspec : public libgit2_api {
public:
  // Default construct a refspec object
  refspec();

  // Construct from libgit2 C ptr
  refspec(git_refspec *c_ptr, ownership owner = ownership::libgit2);

  // Cleanup refspec object
  ~refspec();

  // Get the refspec's direction.
  connection_direction direction() const;

  // Get the destination specifier
  std::string destination() const;

  // Check if a refspec's destination descriptor matches a reference
  bool destination_matches_reference(const std::string &refname) const;

  // Get the force update setting
  bool is_force_update_enabled() const;

  // Parse a given refspec string
  static refspec parse(const std::string &input, bool is_fetch);

  // Transform a target reference to its
  // source reference following the refspec's rules
  data_buffer transform_target_to_source_reference(const std::string &name);

  // Get the source specifier
  std::string source() const;

  // Check if a refspec's source descriptor matches a reference
  bool source_matches_reference(const std::string &refname) const;

  // Get the refspec's string
  std::string to_string() const;

  // Transform a reference to its target following the refspec's rules
  data_buffer transform_reference(const std::string &name);

  // Access to libgit2 C ptr
  const git_refspec *c_ptr() const;

private:
  ownership owner_;
  git_refspec *c_ptr_;
};

} // namespace cppgit2