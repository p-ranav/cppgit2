#include <cppgit2/oid.hpp>
#include <iostream>

namespace cppgit2 {

oid::oid() { git_libgit2_init(); }

oid::oid(const std::string &hex_string) {
  git_libgit2_init();
  if (git_oid_fromstr(&c_struct_, hex_string.c_str()))
    throw exception();
}

oid::oid(const std::string &hex_string, size_t length) {
  git_libgit2_init();
  if (git_oid_fromstrn(&c_struct_, hex_string.c_str(), length))
    throw exception();
}

oid::oid(const git_oid *c_ptr) {
  git_libgit2_init();
  // Convert git_oid * to string and construct this oid
  size_t n = GIT_OID_HEXSZ;
  std::string buffer(n, '0');
  if (!git_oid_tostr(const_cast<char *>(buffer.c_str()), n + 1, c_ptr))
    throw exception();
  // Construct from string
  if (git_oid_fromstr(&c_struct_, buffer.c_str()))
    throw exception();
}

oid::~oid() { git_libgit2_shutdown(); }

int oid::compare(const oid &rhs) const {
  return git_oid_cmp(&c_struct_, rhs.c_ptr());
}

int oid::compare(const oid &rhs, size_t length) const {
  return git_oid_ncmp(&c_struct_, rhs.c_ptr(), length);
}

oid oid::copy() const {
  oid result;
  git_oid_cpy(&result.c_struct_, &c_struct_);
  return result;
}

bool oid::is_zero() const { return git_oid_iszero(&c_struct_); }

bool oid::operator==(const oid &rhs) const {
  return git_oid_equal(&c_struct_, rhs.c_ptr());
}

bool oid::operator==(const std::string &rhs) const {
  return git_oid_streq(&c_struct_, rhs.c_str()) ? false : true;
}

std::string oid::to_hex_string(size_t n) const {
  std::string result(n, '0');
  if (!git_oid_tostr(const_cast<char *>(result.c_str()), n + 1, &c_struct_))
    throw exception();
  return result;
}

std::string oid::to_path_string() const {
  std::string result(GIT_OID_HEXSZ + 1, '0');
  git_oid_pathfmt(const_cast<char *>(result.c_str()), &c_struct_);
  return result;
}

const git_oid *oid::c_ptr() const { return &c_struct_; }

} // namespace cppgit2
