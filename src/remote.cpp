#include <cppgit2/repository.hpp>
using namespace cppgit2;

remote::remote() : c_ptr_(nullptr), owner_(ownership::libgit2) {}

remote::remote(git_remote *c_ptr, ownership owner)
    : c_ptr_(c_ptr), owner_(owner) {}

remote::~remote() {
  if (c_ptr_ && owner_ == ownership::user)
    git_remote_free(c_ptr_);
}

remote::autotag remote::autotag_option() {
  return static_cast<remote::autotag>(git_remote_autotag(c_ptr_));
}

bool remote::is_connected() const { return git_remote_connected(c_ptr_); }

remote remote::create_detached_remote(const std::string &url) {
  remote result;
  if (git_remote_create_detached(&result.c_ptr_, url.c_str()))
    throw git_exception();
  return result;
}

data_buffer remote::default_branch() const {
  data_buffer result;
  if (git_remote_default_branch(result.c_ptr(), c_ptr_))
    throw git_exception();
  return result;
}

void remote::disconnect() {
  if (git_remote_disconnect(c_ptr_))
    throw git_exception();
}

remote remote::copy() const {
  remote result;
  if (git_remote_dup(&result.c_ptr_, c_ptr_))
    throw git_exception();
  return result;
}

strarray remote::fetch_refspec() const {
  strarray result;
  if (git_remote_get_fetch_refspecs(&result.c_struct_, c_ptr_))
    throw git_exception();
  return result;
}

strarray remote::push_refspec() const {
  strarray result;
  if (git_remote_get_push_refspecs(&result.c_struct_, c_ptr_))
    throw git_exception();
  return result;
}

bool remote::is_valid_name(const std::string &name) {
  return git_remote_is_valid_name(name.c_str());
}

std::string remote::name() const {
  auto ret = git_remote_name(c_ptr_);
  return ret ? std::string(ret) : "";
}

repository remote::owner() const {
  return repository(git_remote_owner(c_ptr_));
}

void remote::push(const strarray &refspecs, const push::options &options) {
  if (git_remote_push(c_ptr_, refspecs.c_ptr(), options.c_ptr()))
    throw git_exception();
}

std::string remote::push_url() const {
  auto ret = git_remote_pushurl(c_ptr_);
  return ret ? std::string(ret) : "";
}

size_t remote::size() const { return git_remote_refspec_count(c_ptr_); }

void remote::stop() {
  if (git_remote_stop(c_ptr_))
    throw git_exception();
}

void remote::upload(const strarray &refspecs, const push::options &options) {
  if (git_remote_upload(c_ptr_, refspecs.c_ptr(), options.c_ptr()))
    throw git_exception();
}

std::string remote::url() const {
  auto ret = git_remote_url(c_ptr_);
  return ret ? std::string(ret) : "";
}