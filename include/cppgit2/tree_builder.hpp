#pragma once
#include <cppgit2/data_buffer.hpp>
#include <cppgit2/exception.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/repository.hpp>
#include <cppgit2/tree.hpp>
#include <git2.h>
#include <string>

namespace cppgit2 {

// Always owned by user
class tree_builder {
public:
  // Create a new tree builder.
  // The tree builder can be used to create or modify trees in memory and write them as tree objects
  // to the database.
  tree_builder(repository &repo, tree source = tree());

  // Free git_treebuilder c_ptr_
  ~tree_builder();

  // Clear all the entires in the builder
  void clear();

  // Get the number of entries listed in a treebuilder
  size_t size() const;

  // Get an entry from the builder from its filename
  tree::entry operator[](const std::string &filename) const;

  // Add or update an entry to the builder
  void insert(const std::string &filename, const oid &id, file_mode mode);
  
  // Remove an entry from the builder by its filename
  void remove(const std::string &filename);

  // Write the contents of the tree builder as a tree object
  // The tree builder will be written to the given repo, and its identifying SHA1 hash will be
  // stored in the id pointer.
  oid write();

  const git_treebuilder * c_ptr() const;

private:
  git_treebuilder * c_ptr_;
};

}