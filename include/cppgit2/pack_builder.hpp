#pragma once
#include <cppgit2/oid.hpp>
#include <cppgit2/ownership.hpp>
#include <functional>
#include <git2.h>
#include <string>

namespace cppgit2 {

class pack_builder {
public:
  // Default construct a pack_builder
  pack_builder();

  // Construct from libgit2 C ptr
  pack_builder(git_packbuilder * c_ptr, ownership owner);

  // Free packbuilder ptr if owned by user
  ~pack_builder();

  // Get the packfile's hash
  oid id() const;

  // Insert a commit object
  void insert_commit(const oid &commit_id);

  // Insert a single object
  // For an optimal pack it's mandatory to insert objects in recency order, commits followed by
  // trees and blobs.
  void insert_object(const oid &commit_id, const std::string &name = "");

  // Recursively insert an object and its referenced objects
  // Insert the object as well as any object it references
  void insert_object_recursively(const oid &commit_id, const std::string &name = "");

  // Insert a tree object
  // This will add the tree as well as all referenced trees and blobs
  void insert_tree(const oid &tree_id);

  // Get the total number of objects the packbuilder will write out
  size_t size() const;

  // Set the callbacks for a packbuilder
  void set_progress_callback(std::function<void(int, uint32_t, uint32_t)> callback);  

  // Set number of threads to spawn
  void set_threads(unsigned int num_threads);

  // Get the number of objects the packbuilder has already written out
  size_t written() const;

  // Access libgit2 C ptr
  const git_packbuilder * c_ptr() const;

private:
  git_packbuilder * c_ptr_;
  ownership owner_;
};

}