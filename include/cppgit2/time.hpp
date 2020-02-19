#pragma once
#include <cppgit2/libgit2_api.hpp>
#include <git2.h>

namespace cppgit2 {

using epoch_time_seconds = git_time_t;
using offset_minutes = int;

class time : public libgit2_api {
public:
  time(git_time *c_ptr) : c_ptr_(c_ptr) {}

private:
  git_time *c_ptr_;
};

} // namespace cppgit2
