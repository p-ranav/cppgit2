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

blame::hunk blame::hunk_by_index(uint32_t index) const {
  return hunk(git_blame_get_hunk_byindex(c_ptr_, index));
}

blame::hunk blame::hunk_by_line(size_t lineno) const {
  return hunk(git_blame_get_hunk_byline(c_ptr_, lineno));
}

size_t blame::hunk_count() const { return git_blame_get_hunk_count(c_ptr_); }

const git_blame *blame::c_ptr() const { return c_ptr_; }

} // namespace cppgit2