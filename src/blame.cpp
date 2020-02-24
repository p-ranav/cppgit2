#include <cppgit2/blame.hpp>

namespace cppgit2 {

blame::blame() : c_ptr_(nullptr), owner_(ownership::libgit2) {}

blame::blame(git_blame *c_ptr, ownership owner)
    : c_ptr_(c_ptr), owner_(owner) {}

blame::~blame() {
  if (c_ptr_ && owner_ == ownership::user)
    git_blame_free(c_ptr_);
}

blame blame::get_blame_for_buffer(const blame &reference,
                                  const std::string &buffer) {
  blame result;
  if (git_blame_buffer(&result.c_ptr_,
                       const_cast<git_blame *>(reference.c_ptr()),
                       buffer.c_str(), buffer.size()))
    throw git_exception();
  return result;
}

const git_blame *blame::c_ptr() const { return c_ptr_; }

} // namespace cppgit2