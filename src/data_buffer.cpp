#include <cppgit2/data_buffer.hpp>

namespace cppgit2 {

/* Used as default value for git_buf->ptr so that people can always
 * assume ptr is non-NULL and zero terminated even for new git_bufs.
 */
char git_buf__initbuf[1];
/* Use to initialize buffer structure when git_buf is on stack */
#define GIT_BUF_INIT { git_buf__initbuf, 0, 0 }

data_buffer::data_buffer() {
  c_struct_ = GIT_BUF_INIT;
}

data_buffer::data_buffer(size_t n) {
  c_struct_.ptr = (char *)malloc(n * sizeof(char));
  if (c_struct_.ptr)
    memset(c_struct_.ptr, '\0', n * sizeof(char));
  c_struct_.asize = n;
  c_struct_.size = 0;
}

data_buffer::data_buffer(const git_buf *c_ptr) {
  c_struct_.ptr = (char *)malloc(c_ptr->asize * sizeof(char));
  c_struct_.asize = c_ptr->asize;
  c_struct_.size = c_ptr->size;
  if (c_struct_.ptr)
    strncpy(c_struct_.ptr, c_ptr->ptr, c_ptr->asize);
}

data_buffer::~data_buffer() {
  if (c_struct_.size)
    git_buf_dispose(&c_struct_);
}

bool data_buffer::contains_nul() const {
  return git_buf_contains_nul(&c_struct_);
}

void data_buffer::grow_to_size(size_t target_size) {
  if (git_buf_grow(&c_struct_, target_size))
    throw git_exception();
}

bool data_buffer::is_binary() const { return git_buf_is_binary(&c_struct_); }

void data_buffer::set_buffer(const std::string &buffer) {
  if (git_buf_set(&c_struct_, buffer.c_str(), buffer.size()))
    throw git_exception();
}

std::string data_buffer::to_string() const {
  if (c_struct_.size)
    return std::string(c_struct_.ptr);
  else
    return "";
}

git_buf *data_buffer::c_ptr() { return &c_struct_; }

const git_buf *data_buffer::c_ptr() const { return &c_struct_; }

} // namespace cppgit2
