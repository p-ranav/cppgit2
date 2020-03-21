#include <cppgit2/repository.hpp>
using namespace cppgit2;

remote::remote() : c_ptr_(nullptr), owner_(ownership::libgit2) {}

remote::remote(git_remote *c_ptr, ownership owner)
    : c_ptr_(c_ptr), owner_(owner) {}

remote::~remote() {
  if (c_ptr_ && owner_ == ownership::user)
    git_remote_free(c_ptr_);
}

fetch::options::autotag remote::autotag_option() {
  return static_cast<fetch::options::autotag>(git_remote_autotag(c_ptr_));
}

bool remote::is_connected() const { return git_remote_connected(c_ptr_); }

void remote::connect(connection_direction direction,
                     const callbacks &remote_callbacks,
                     const proxy::options &proxy_options,
                     const strarray &custom_headers) {
  if (git_remote_connect(c_ptr_, static_cast<git_direction>(direction),
                         remote_callbacks.c_ptr(), proxy_options.c_ptr(),
                         custom_headers.c_ptr()))
    throw git_exception();
}

cppgit2::repository remote::create_options::repository() const {
  return cppgit2::repository(c_ptr_->repository);
}

void remote::create_options::set_repository(const cppgit2::repository &repo) {
  c_ptr_->repository = repo.c_ptr_;
}

remote remote::copy() const {
  remote result;
  if (git_remote_dup(&result.c_ptr_, c_ptr_))
    throw git_exception();
  return result;
}

remote remote::create_detached_remote(const std::string &url) {
  remote result;
  if (git_remote_create_detached(&result.c_ptr_, url.c_str()))
    throw git_exception();
  return result;
}

remote remote::create_remote(const std::string &url,
                             const create_options &options) {
  remote result;
  if (git_remote_create_with_opts(&result.c_ptr_, url.c_str(), options.c_ptr()))
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

void remote::download(const strarray &refspecs, const fetch::options &options) {
  if (git_remote_download(c_ptr_, refspecs.c_ptr(), options.c_ptr()))
    throw git_exception();
}

void remote::fetch(const strarray &refspecs, const std::string &reflog_message,
                   const fetch::options &options) {
  if (git_remote_fetch(c_ptr_, refspecs.c_ptr(), options.c_ptr(),
                       reflog_message.c_str()))
    throw git_exception();
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

std::vector<remote::head> remote::reference_advertisement_list() {
  const git_remote_head * head_ptr = nullptr;
  const git_remote_head ** head_ptr_ptr = &head_ptr;
  size_t size = 0;
  if (git_remote_ls(&head_ptr_ptr, &size, c_ptr_))
    throw git_exception();

  std::vector<head> result;
  for (size_t i = 0; i < size; ++i)
    result.push_back(head(&head_ptr[i]));
  return result;
}

std::string remote::name() const {
  auto ret = git_remote_name(c_ptr_);
  return ret ? std::string(ret) : "";
}

repository remote::owner() const {
  return repository(git_remote_owner(c_ptr_));
}

void remote::prune(const callbacks &remote_callbacks) {
  if (git_remote_prune(c_ptr_, remote_callbacks.c_ptr()))
    throw git_exception();
}

void remote::prune_references() {
  if (git_remote_prune_refs(c_ptr_))
    throw git_exception();
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

refspec remote::operator[](size_t n) {
  return refspec((git_refspec *)git_remote_get_refspec(c_ptr_, n));
}

indexer::progress remote::stats() const {
  return indexer::progress(git_remote_stats(c_ptr_));
}

void remote::stop() {
  if (git_remote_stop(c_ptr_))
    throw git_exception();
}

void remote::update_tips(const callbacks &remote_callbacks, bool update_fetchhead, 
  fetch::options::autotag download_tags, const std::string &reflog_message) {
  if (git_remote_update_tips(c_ptr_, remote_callbacks.c_ptr(), update_fetchhead, 
    static_cast<git_remote_autotag_option_t>(download_tags), reflog_message.c_str()))
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