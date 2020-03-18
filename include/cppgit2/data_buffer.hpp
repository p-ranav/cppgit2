#pragma once
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cstdlib>
#include <cstring>
#include <git2.h>
#include <string>

namespace cppgit2 {

class data_buffer : public libgit2_api {
public:
  // Default construct a data buffer using GIT_BUF_INIT
  data_buffer();

  // Construct buffer of size n
  // Contains a git_buf with char buffer of size n
  data_buffer(size_t n);

  // Construct buffer from libgit2 C ptr
  data_buffer(const git_buf *c_ptr);

  // Dispose internal buffer
  ~data_buffer();

  // Check quickly if buffer contains a NUL byte
  bool contains_nul() const;

  // Resize buffer allocation to make more space
  // Currently, this will never shrink a buffer, only expand it
  void grow_to_size(size_t target_size);

  // Check quickly if buffer looks like it contains binary data
  bool is_binary() const;

  // Set buffer to a copy of some raw data
  void set_buffer(const std::string &buffer);

  // Get string representation of data buffer
  std::string to_string() const;

  // Access libgit2 C ptr
  git_buf *c_ptr();
  const git_buf *c_ptr() const;

private:
  git_buf c_struct_;
};

} // namespace cppgit2
