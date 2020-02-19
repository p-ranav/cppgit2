#include <cppgit2/tree.hpp>

namespace cppgit2 {

tree::tree() : c_ptr_(nullptr), owner_(ownership::libgit2) {}

tree::tree(git_tree *c_ptr, ownership owner) : c_ptr_(c_ptr), owner_(owner) {}

tree::~tree() {
  if (c_ptr_ && owner_ == ownership::user)
    git_tree_free(c_ptr_);
}

tree::entry tree::lookup_entry_by_id(const oid &id) const {
  return tree::entry(
      const_cast<git_tree_entry *>(git_tree_entry_byid(c_ptr_, id.c_ptr())));
}

tree::entry tree::lookup_entry_by_index(size_t index) const {
  return tree::entry(
      const_cast<git_tree_entry *>(git_tree_entry_byindex(c_ptr_, index)));
}

tree::entry tree::lookup_entry_by_name(const std::string &filename) const {
  return tree::entry(const_cast<git_tree_entry *>(
      git_tree_entry_byname(c_ptr_, filename.c_str())));
}

tree::entry tree::lookup_entry_by_path(const std::string &path) const {
  tree::entry result;
  result.owner_ = ownership::user;
  if (git_tree_entry_bypath(&result.c_ptr_, c_ptr_, path.c_str()))
    throw exception();
  return result;
}

oid tree::id() const { return oid(git_tree_id(c_ptr_)); }

tree tree::copy() const {
  tree result;
  if (git_tree_dup(&result.c_ptr_, c_ptr_))
    throw exception();
  return result;
}

size_t tree::size() const { return git_tree_entrycount(c_ptr_); }

git_tree *tree::c_ptr() { return c_ptr_; }

const git_tree *tree::c_ptr() const { return c_ptr_; }

} // namespace cppgit2
