#include <cppgit2/repository.hpp>

namespace cppgit2 {

commit::commit() : c_ptr_(nullptr), owner_(ownership::libgit2) {}

commit::commit(git_commit *c_ptr, ownership owner)
    : c_ptr_(c_ptr), owner_(owner) {}

commit::~commit() {
  if (c_ptr_ && owner_ == ownership::user)
    git_commit_free(c_ptr_);
}

void commit::amend(const oid &id, const std::string &update_ref,
                   const signature &author, const signature &committer,
                   const std::string &message_encoding,
                   const std::string &message, const cppgit2::tree &tree) {
  if (git_commit_amend(const_cast<git_oid *>(id.c_ptr()), c_ptr_,
                       update_ref.c_str(), author.c_ptr(), committer.c_ptr(),
                       message_encoding.c_str(), message.c_str(), tree.c_ptr()))
    throw git_exception();
}

signature commit::author() const {
  auto ret = git_commit_author(c_ptr_);
  return signature(ret->name, ret->email, ret->when.time, ret->when.offset);
}

std::string commit::body() const {
  auto ret = git_commit_body(c_ptr_);
  if (ret)
    return std::string(ret);
  else
    return "";
}

signature commit::committer() const {
  auto ret = git_commit_committer(c_ptr_);
  return signature(ret->name, ret->email, ret->when.time, ret->when.offset);
}

commit commit::copy() const {
  commit result;
  if (git_commit_dup(&result.c_ptr_, c_ptr_))
    throw git_exception();
  return result;
}

std::string commit::operator[](const std::string &field) const {
  data_buffer result(nullptr);
  if (git_commit_header_field(result.c_ptr(), c_ptr_, field.c_str()))
    throw git_exception();
  return result.to_string();
}

oid commit::id() const { return oid(git_commit_id(c_ptr_)); }

std::string commit::message() const {
  auto ret = git_commit_message(c_ptr_);
  if (ret)
    return std::string(ret);
  else
    return "";
}

std::string commit::message_encoding() const {
  auto ret = git_commit_message_encoding(c_ptr_);
  if (ret)
    return std::string(ret);
  else
    return "";
}

std::string commit::message_raw() const {
  auto ret = git_commit_message_raw(c_ptr_);
  if (ret)
    return std::string(ret);
  else
    return "";
}

commit commit::ancestor(unsigned long n) const {
  commit result(nullptr, ownership::user);
  if (git_commit_nth_gen_ancestor(&result.c_ptr_, c_ptr_, n))
    throw git_exception();
  return result;
}

commit commit::parent(const unsigned int n) const {
  commit result(nullptr, ownership::user);
  if (git_commit_parent(&result.c_ptr_, c_ptr_, n))
    throw git_exception();
  return result;
}

oid commit::parent_id(unsigned int n) const {
  return oid(git_commit_parent_id(c_ptr_, n));
}

unsigned int commit::parent_count() const {
  return git_commit_parentcount(c_ptr_);
}

std::string commit::raw_header() const {
  auto ret = git_commit_raw_header(c_ptr_);
  if (ret)
    return std::string(ret);
  else
    return "";
}

std::string commit::summary() const {
  auto ret = git_commit_summary(c_ptr_);
  if (ret)
    return std::string(ret);
  else
    return "";
}

epoch_time_seconds commit::time() const { return git_commit_time(c_ptr_); }

offset_minutes commit::time_offset() const {
  return git_commit_time_offset(c_ptr_);
}

tree commit::tree() const {
  cppgit2::tree result(nullptr, ownership::user);
  if (git_commit_tree(&result.c_ptr_, c_ptr_))
    throw git_exception();
  return result;
}

oid commit::tree_id() const { return oid(git_commit_tree_id(c_ptr_)); }

repository commit::owner() const {
  return repository(git_commit_owner(c_ptr_));
}

const git_commit *commit::c_ptr() const { return c_ptr_; }

} // namespace cppgit2