#include <cppgit2/submodule.hpp>

namespace cppgit2 {

submodule::submodule() : c_ptr_(nullptr), owner_(ownership::libgit2) {}

submodule::submodule(git_submodule *c_ptr, ownership owner)
    : c_ptr_(c_ptr), owner_(owner) {}

submodule::~submodule() {
  if (c_ptr_ && owner_ == ownership::user)
    git_submodule_free(c_ptr_);
}

void submodule::init(bool overwrite) {
  if (git_submodule_init(c_ptr_, overwrite))
    throw git_exception();
}

void submodule::resolve_setup() {
  if (git_submodule_add_finalize(c_ptr_))
    throw git_exception();
}

void submodule::add_to_index(bool write_index) {
  if (git_submodule_add_to_index(c_ptr_, write_index))
    throw git_exception();
}

std::string submodule::branch_name() const {
  auto ret = git_submodule_branch(c_ptr_);
  if (ret)
    return std::string(ret);
  else
    return "";
}

submodule::recurse submodule::recurse_submodules_option() const {
  return static_cast<submodule::recurse>(
      git_submodule_fetch_recurse_submodules(c_ptr_));
}

oid submodule::head_id() const { return oid(git_submodule_head_id(c_ptr_)); }

submodule::ignore submodule::ignore_option() const {
  return static_cast<submodule::ignore>(git_submodule_ignore(c_ptr_));
}

submodule::update submodule::update_strategy() const {
  return static_cast<submodule::update>(git_submodule_update_strategy(c_ptr_));
}

oid submodule::index_id() const { return oid(git_submodule_index_id(c_ptr_)); }

std::string submodule::name() const {
  auto ret = git_submodule_name(c_ptr_);
  if (ret)
    return std::string(ret);
  else
    return "";
}

std::string submodule::path() const {
  auto ret = git_submodule_path(c_ptr_);
  if (ret)
    return std::string(ret);
  else
    return "";
}

void submodule::reload(bool force) {
  if (git_submodule_reload(c_ptr_, force))
    throw git_exception();
}

void submodule::sync() {
  if (git_submodule_sync(c_ptr_))
    throw git_exception();
}

std::string submodule::url() const {
  auto ret = git_submodule_url(c_ptr_);
  if (ret)
    return std::string(ret);
  else
    return "";
}

const git_submodule *submodule::c_ptr() const { return c_ptr_; }

} // namespace cppgit2