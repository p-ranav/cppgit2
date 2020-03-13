#pragma once
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/time.hpp>
#include <git2.h>
#include <string>

namespace cppgit2 {

class signature : public libgit2_api {
public:
  signature();

  // Create a new action signature with
  // 1. name of person
  // 2. email of person
  // Uses "now" for signature time
  signature(const std::string &name, const std::string &email);

  // Create a new action signature with
  // 1. name of person
  // 2. email of person
  // 3. time (in seconds from epoch) when the action happened
  // 4. offset (timezone offset in minutes) for the time
  signature(const std::string &name, const std::string &email,
            epoch_time_seconds time, int offset_minutes);

  // Create a new action signature from buffer
  // Buffer should be in the format:
  //   "Real Name <email> timestamp tzoffset"
  //
  // timestamp is the number of seconds since the Unix epoch
  // tzoffset is the timezone offset in hhmm format
  // - (note the lack of a colon separator).
  signature(const std::string &buffer);

  // Construct from C ptr
  signature(const git_signature *c_ptr);

  // Duplicate this signature
  signature copy() const;

  // Getters
  std::string name() const;
  std::string email() const;
  epoch_time_seconds time() const;
  offset_minutes offset() const;

  // Access libgit2 C pointer
  const git_signature *c_ptr() const;

private:
  friend class repository;
  git_signature *c_ptr_;
  git_signature default_;
  std::string default_name_{""};
  std::string default_email_{""};
};

} // namespace cppgit2
