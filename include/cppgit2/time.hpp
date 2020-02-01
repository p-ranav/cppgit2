#pragma once
#include <git2.h>

namespace cppgit2 {

using epoch_time_seconds = git_time_t;
using offset_minutes = int;
  
class time {
public:
  time(git_time * c_ptr) : c_ptr_(c_ptr) { git_libgit2_init(); }
  ~time() { git_libgit2_shutdown(); }
  
private:
  git_time * c_ptr_;
};
  
}
