#include <cppgit2/data_buffer.hpp>

namespace cppgit2 {

data_buffer::data_buffer(size_t n) {
  git_libgit2_init();
  c_struct_.ptr = (char *)malloc(n * sizeof(char));
  memset(c_struct_.ptr, '\0', n * sizeof(char));
  c_struct_.asize = n;
  c_struct_.size = 0;
}

data_buffer::data_buffer(const git_buf *c_ptr) {
  git_libgit2_init();
  c_struct_.ptr = (char *)malloc(c_ptr->asize * sizeof(char));
  c_struct_.asize = c_ptr->asize;
  c_struct_.size = c_ptr->size;
  strncpy(c_struct_.ptr, c_ptr->ptr, c_ptr->asize);
}

data_buffer::~data_buffer() {
  if (c_struct_.size)
    git_buf_dispose(&c_struct_);
  git_libgit2_shutdown();
}

bool data_buffer::contains_nul() const {
  return git_buf_contains_nul(&c_struct_);
}

void data_buffer::grow_to_size(size_t target_size) {
  if (git_buf_grow(&c_struct_, target_size))
    throw exception();
}

bool data_buffer::is_binary() const { return git_buf_is_binary(&c_struct_); }

void data_buffer::set_buffer(const std::string &buffer) {
  if (git_buf_set(&c_struct_, buffer.c_str(), buffer.size()))
    throw exception();
}

std::string data_buffer::to_string() const {
  if (c_struct_.size)
    return std::string(c_struct_.ptr);
  else
    return "";
}

const git_buf *data_buffer::c_ptr() const { return &c_struct_; }

} // namespace cppgit2
