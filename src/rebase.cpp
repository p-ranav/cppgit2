#include <cppgit2/rebase.hpp>
using namespace cppgit2;

rebase::rebase() : c_ptr_(nullptr), owner_(ownership::libgit2) {}

rebase::rebase(git_rebase *c_ptr, ownership owner)
    : c_ptr_(c_ptr), owner_(owner) {}

rebase::~rebase() {
  if (c_ptr_ && owner_ == ownership::user)
    git_rebase_free(c_ptr_);
}

void rebase::abort() {
  if (git_rebase_abort(c_ptr_))
    throw git_exception();
}

oid rebase::commit(const signature &author, const signature &committer,
                   const std::string &message_encoding,
                   const std::string &message) {
  oid result;
  if (git_rebase_commit(result.c_ptr(), c_ptr_, author.c_ptr(),
                        committer.c_ptr(), message_encoding.c_str(),
                        message.c_str()))
    throw git_exception();
  return result;
}

void rebase::finish(const signature &sig) {
  if (git_rebase_finish(c_ptr_, sig.c_ptr()))
    throw git_exception();
}

cppgit2::index rebase::index() {
  cppgit2::index result(nullptr, ownership::user);
  if (git_rebase_inmemory_index(&result.c_ptr_, c_ptr_))
    throw git_exception();
  return result;
}

rebase::operation rebase::next() {
  operation result;
  if (git_rebase_next(&result.c_ptr_, c_ptr_))
    throw git_exception();
  return result;
}

oid rebase::onto_id() { return oid(git_rebase_onto_id(c_ptr_)); }

std::string rebase::onto_refname() {
  auto ret = git_rebase_onto_name(c_ptr_);
  return ret ? std::string(ret) : "";
}

rebase::operation rebase::operator[](size_t index) {
  return operation(git_rebase_operation_byindex(c_ptr_, index));
}

size_t rebase::current_operation() {
  return git_rebase_operation_current(c_ptr_);
}

size_t rebase::size() { return git_rebase_operation_entrycount(c_ptr_); }

oid rebase::original_head_id() { return oid(git_rebase_orig_head_id(c_ptr_)); }

std::string rebase::original_head_name() {
  auto ret = git_rebase_orig_head_name(c_ptr_);
  return ret ? std::string(ret) : "";
}