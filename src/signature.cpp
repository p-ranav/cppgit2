#include <cppgit2/signature.hpp>

namespace cppgit2 {

signature::signature() {
  git_libgit2_init();
  default_.name = const_cast<char *>(default_name_.c_str());
  default_.email = const_cast<char *>(default_email_.c_str());
  default_.when.time = 0;
  default_.when.offset = 0;
  c_ptr_ = &default_;
}

signature::signature(const std::string &name, const std::string &email) {
  git_libgit2_init();
  if (git_signature_now(&c_ptr_, name.c_str(), email.c_str()))
    throw exception();
}

signature::signature(const std::string &name, const std::string &email,
                     epoch_time_seconds time, int offset_minutes) {
  git_libgit2_init();
  if (git_signature_new(&c_ptr_, name.c_str(), email.c_str(), time,
                        offset_minutes))
    throw exception();
}

signature::signature(const std::string &buffer) {
  git_libgit2_init();
  if (git_signature_from_buffer(&c_ptr_, buffer.c_str()))
    throw exception();
}

signature::signature(const git_signature *c_ptr) {
  git_libgit2_init();
  if (git_signature_dup(&c_ptr_, c_ptr))
    throw exception();
}

signature::~signature() { git_libgit2_shutdown(); }

signature signature::copy() const {
  signature result;
  if (git_signature_dup(&result.c_ptr_, c_ptr_))
    throw exception();
  return result;
}

std::string signature::name() const {
  if (c_ptr_)
    return c_ptr_->name ? c_ptr_->name : "";
  else
    throw exception("signature is null");
}

std::string signature::email() const {
  if (c_ptr_)
    return c_ptr_->email ? c_ptr_->email : "";
  else
    throw exception("signature is null");
}

epoch_time_seconds signature::time() const {
  if (c_ptr_)
    return c_ptr_->when.time;
  else
    throw exception("signature is null");
}

offset_minutes signature::offset() const {
  if (c_ptr_)
    return c_ptr_->when.offset;
  else
    throw exception("signature is null");
}

const git_signature *signature::c_ptr() const { return c_ptr_; }

} // namespace cppgit2
