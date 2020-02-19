#include <cppgit2/repository.hpp>

namespace cppgit2 {

reference::reference() : c_ptr_(nullptr), owner_(ownership::libgit2) {}

reference::reference(git_reference *c_ptr, ownership owner)
    : c_ptr_(c_ptr), owner_(owner) {}

reference::~reference() {
  if (c_ptr_ && owner_ == ownership::user)
    git_reference_free(c_ptr_);
}

int reference::compare(const reference &rhs) const {
  return git_reference_cmp(c_ptr_, rhs.c_ptr());
}

reference reference::copy() const {
  reference result(nullptr, ownership::user);
  if (git_reference_dup(&result.c_ptr_, c_ptr_))
    throw exception();
  return result;
}

void reference::delete_reference(reference &ref) {
  if (git_reference_delete(ref.c_ptr_))
    throw exception();
}

bool reference::is_branch() const { return git_reference_is_branch(c_ptr_); }

bool reference::is_note() const { return git_reference_is_note(c_ptr_); }

bool reference::is_remote() const { return git_reference_is_remote(c_ptr_); }

bool reference::is_tag() const { return git_reference_is_tag(c_ptr_); }

bool reference::is_valid_name(const std::string &refname) {
  return git_reference_is_valid_name(refname.c_str());
}

std::string reference::name() const {
  auto ret = git_reference_name(c_ptr_);
  if (ret)
    return std::string(ret);
  else
    return "";
}

std::string reference::shorthand_name() const {
  auto ret = git_reference_shorthand(c_ptr_);
  if (ret)
    return std::string(ret);
  else
    return "";
}

repository reference::owner() const {
  return repository(git_reference_owner(c_ptr_));
}

object reference::peel_until(object::object_type type) {
  object result;
  if (git_reference_peel(&result.c_ptr_, c_ptr_,
                         static_cast<git_object_t>(type)))
    throw exception();
  return result;
}

reference reference::resolve() {
  reference result(nullptr, ownership::user);
  if (git_reference_resolve(&result.c_ptr_, c_ptr_))
    throw exception();
  return result;
}

oid reference::target() const { return oid(git_reference_target(c_ptr_)); }

oid reference::peeled_target() const {
  return oid(git_reference_target_peel(c_ptr_));
}

reference::reference_type reference::type() const {
  return static_cast<reference_type>(git_reference_type(c_ptr_));
}

const git_reference *reference::c_ptr() const { return c_ptr_; }

} // namespace cppgit2