#pragma once
#include <cppgit2/data_buffer.hpp>
#include <cppgit2/indexer.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/ownership.hpp>
#include <cppgit2/revwalk.hpp>
#include <functional>
#include <git2.h>
#include <string>

namespace cppgit2 {

class pack_builder : public libgit2_api {
public:
  // Default construct a pack_builder
  pack_builder();

  // Construct from libgit2 C ptr
  pack_builder(git_packbuilder *c_ptr, ownership owner);

  // Free packbuilder ptr if owned by user
  ~pack_builder();

  // Create the new pack and pass each object to the callback
  void for_each_object(
      std::function<void(void *object_data, size_t object_size)> visitor);

  // Get the packfile's hash
  // A packfile's name is derived from the sorted hashing of all object names.
  // This is only correct after the packfile has been written.
  oid hash();

  // Get the packfile's hash
  oid id() const;

  // Insert a commit object
  void insert_commit(const oid &commit_id);

  // Insert a single object
  // For an optimal pack it's mandatory to insert objects in recency order,
  // commits followed by trees and blobs.
  void insert_object(const oid &commit_id, const std::string &name = "");

  // Recursively insert an object and its referenced objects
  // Insert the object as well as any object it references
  void insert_object_recursively(const oid &commit_id,
                                 const std::string &name = "");

  // Insert a tree object
  // This will add the tree as well as all referenced trees and blobs
  void insert_tree(const oid &tree_id);

  // Insert objects as given by the walk
  void insert_revwalk(const revwalk &walk);

  // Get the total number of objects the packbuilder will write out
  size_t size() const;

  // Set the callbacks for a packbuilder
  void
  set_progress_callback(std::function<void(int, uint32_t, uint32_t)> callback);

  // Set number of threads to spawn
  void set_threads(unsigned int num_threads);

  // Write the new pack and corresponding index file to path.
  void write(const std::string &path, unsigned int mode,
             std::function<void(const indexer::progress &)> &progress_callback);

  // Write the contents of the packfile to an in-memory buffer
  data_buffer write_to_buffer();

  // Get the number of objects the packbuilder has already written out
  size_t written() const;

  // Access libgit2 C ptr
  const git_packbuilder *c_ptr() const;

private:
  friend class repository;
  git_packbuilder *c_ptr_;
  ownership owner_;
};

} // namespace cppgit2