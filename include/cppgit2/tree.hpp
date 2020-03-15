#pragma once
#include <cppgit2/file_mode.hpp>
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/object.hpp>
#include <cppgit2/ownership.hpp>
#include <functional>
#include <git2.h>
#include <memory>
#include <string>
#include <utility>

namespace cppgit2 {

class tree : public libgit2_api {
public:
  // Default construct a tree
  tree();

  // Construct from libgit2 C ptr
  // If owned by user, will be free'd in destructor
  tree(git_tree *c_ptr, ownership owner = ownership::libgit2);

  // Cleanup
  ~tree();

  class entry : public libgit2_api {
  public:
    // Default construction
    entry() : c_ptr_(nullptr), owner_(ownership::libgit2) {}

    // Construct from libgit2 C ptr
    // If owned by user, will be free'd in destructor
    entry(git_tree_entry *c_ptr, ownership owner = ownership::libgit2)
        : c_ptr_(c_ptr), owner_(owner) {}

    entry(const git_tree_entry *c_ptr)
        : c_ptr_(const_cast<git_tree_entry *>(c_ptr)),
          owner_(ownership::libgit2) {}

    // Clean up tree entry
    ~entry() {
      if (c_ptr_ && owner_ == ownership::user)
        git_tree_entry_free(c_ptr_);
    }

    // Duplicate a tree entry
    entry copy() const {
      entry result;
      if (git_tree_entry_dup(&result.c_ptr_, c_ptr_))
        throw git_exception();
      return result;
    }

    // UNIX file attributes of a tree entry
    file_mode filemode() const {
      return static_cast<file_mode>(git_tree_entry_filemode(c_ptr_));
    }

    // Raw UNIX file attributes of this tree entry
    file_mode raw_filemode() const {
      return static_cast<file_mode>(git_tree_entry_filemode_raw(c_ptr_));
    }

    // SHA1 hash of this tree entry
    oid id() const { return oid(git_tree_entry_id(c_ptr_)); }

    // Filename of tree entry
    std::string filename() const {
      auto entry_name = git_tree_entry_name(c_ptr_);
      if (entry_name)
        return std::string(entry_name);
      else
        return "";
    }

    // Type of object pointed to by this entry
    object::object_type type() const {
      return static_cast<object::object_type>(git_tree_entry_type(c_ptr_));
    }

    // Compare two tree entries
    // < 0 if this is before e2
    //   0 if this == e2
    // > 0 if this is after e2
    int compare(const entry &e2) {
      return git_tree_entry_cmp(c_ptr_, e2.c_ptr());
    }

    // Access libgit2 C ptr
    git_tree_entry *c_ptr() { return c_ptr_; }
    const git_tree_entry *c_ptr() const { return c_ptr_; }

  private:
    friend tree;
    friend class tree_builder;
    git_tree_entry *c_ptr_;
    ownership owner_;
  };

  // SHA1 hash of this tree
  oid id() const;

  // Duplicate of tree
  tree copy() const;

  // Lookup tree entry by SHA value
  // Returned entry is owned by the tree
  // This must examine every entry in the tree, so it's not fast
  entry lookup_entry_by_id(const oid &id) const;

  // Lookup tree entry by its position in the tree
  // Returned entry is owned by the tree
  entry lookup_entry_by_index(size_t index) const;

  // Lookup tree entry by its filename
  // Returned entry is owned by the tree
  entry lookup_entry_by_name(const std::string &filename) const;

  // Lookup tree entry given its relative path
  // Returned tree entry is owned by the user
  entry lookup_entry_by_path(const std::string &path) const;

  // Number of entries in tree
  size_t size() const;

  // Tree traversal modes
  enum class traversal_mode { preorder = 0, postorder = 1 };

  // Owning repository
  class repository owner() const;

  // Traverse the entries in a tree and its subtrees in post or pre order.
  void walk(traversal_mode mode,
            std::function<void(const std::string &, const tree::entry &)>
                visitor) const;

  enum class update_type {
    upsert, // Update or insert an entry at the specified path
    remove  // Remove an entry from the specified path
  };

  class update : public libgit2_api {
  public:
    tree::update_type action() const {
      return static_cast<tree::update_type>(c_struct_.action);
    }

    void set_action(tree::update_type update_type) {
      c_struct_.action = static_cast<git_tree_update_t>(update_type);
    }

    oid id() const { return oid(&c_struct_.id); }

    void set_id(const oid &id) { c_struct_.id = *(id.c_ptr()); }

    cppgit2::file_mode file_mode() const {
      return static_cast<cppgit2::file_mode>(c_struct_.filemode);
    }

    void set_file_mode(cppgit2::file_mode mode) {
      c_struct_.filemode = static_cast<git_filemode_t>(mode);
    }

    std::string path() const {
      if (c_struct_.path)
        return std::string(c_struct_.path);
      else
        return "";
    }

    void set_path(const std::string &path) { c_struct_.path = path.c_str(); }

  private:
    friend class repository;
    git_tree_update c_struct_;
  };

  // Access libgit2 C ptr
  git_tree *c_ptr();
  const git_tree *c_ptr() const;

private:
  friend class commit;
  friend class pathspec;
  friend class repository;
  git_tree *c_ptr_;
  ownership owner_;
};

} // namespace cppgit2
