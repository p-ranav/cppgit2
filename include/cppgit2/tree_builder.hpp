#pragma once
#include <cppgit2/data_buffer.hpp>
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/repository.hpp>
#include <cppgit2/tree.hpp>
#include <functional>
#include <git2.h>
#include <string>

namespace cppgit2 {

// Always owned by user
class tree_builder : public libgit2_api {
public:
  // Create a new tree builder.
  // The tree builder can be used to create or modify trees in memory and write
  // them as tree objects to the database.
  tree_builder(repository &repo, tree source = tree());

  // Free git_treebuilder c_ptr_
  ~tree_builder();

  // Clear all the entires in the builder
  void clear();

  // The filter callback will be called for each entry in the
  // tree with a pointer to the entry and the provided payload;
  // if the callback returns non-zero, the entry will be filtered
  // (removed from the builder).
  void filter(std::function<int(const tree::entry &)> visitor);

  // Get the number of entries listed in a treebuilder
  size_t size() const;

  // Get an entry from the builder from its filename
  tree::entry operator[](const std::string &filename) const;

  // Add or update an entry to the builder
  void insert(const std::string &filename, const oid &id, file_mode mode);

  // Remove an entry from the builder by its filename
  void remove(const std::string &filename);

  // Write the contents of the tree builder as a tree object
  // The tree builder will be written to the given repo, and its identifying
  // SHA1 hash will be stored in the id pointer.
  oid write();

  // Write the contents of the tree builder as a tree object using a shared
  // git_buf.
  oid write(data_buffer &tree);

  // Acesss libgit2 C ptr
  const git_treebuilder *c_ptr() const;

private:
  git_treebuilder *c_ptr_;
};

} // namespace cppgit2