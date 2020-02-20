#include <cppgit2/annotated_commit.hpp>

namespace cppgit2 {

annotated_commit::annotated_commit() : c_ptr_(nullptr), owner_(ownership::libgit2) {}

annotated_commit::annotated_commit(git_annotated_commit *c_ptr, ownership owner) 
  : c_ptr_(c_ptr), owner_(owner) {}

annotated_commit::~annotated_commit() {
  if (c_ptr_ && owner_ == ownership::user)
    git_annotated_commit_free(c_ptr_);
}

oid annotated_commit::id() const { return oid(git_annotated_commit_id(c_ptr_)); }

std::string annotated_commit::refname() const {
  auto ret = git_annotated_commit_ref(c_ptr_);
  if (ret)
    return std::string(ret);
  else
    return "";
}

const git_annotated_commit *annotated_commit::c_ptr() const { return c_ptr_; }

} // namespace cppgit2