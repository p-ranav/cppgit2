#pragma once
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/ownership.hpp>
#include <git2.h>

namespace cppgit2 {

class indexer : public libgit2_api {
public:
  // This structure is used to provide callers information about the progress of
  // indexing a packfile, either directly or part of a fetch or clone that
  // downloads a packfile.
  class progress : public libgit2_api {
  public:
    progress(git_indexer_progress *c_ptr) : c_ptr_(c_ptr) {}

    // number of objects in the packfile being indexed
    unsigned long total_objects() const { return c_ptr_->total_objects; }

    // received objects that have been hashed
    unsigned long indexer_objects() const { return c_ptr_->indexed_objects; }

    // received_objects: objects which have been downloaded
    unsigned long received_objects() const { return c_ptr_->received_objects; }

    // locally-available objects that have been injected in order to fix a thin
    // pack
    unsigned long local_objects() const { return c_ptr_->local_objects; }

    // number of deltas in the packfile being indexed
    unsigned long total_deltas() const { return c_ptr_->total_deltas; }

    // received deltas that have been indexed
    unsigned long indexed_deltas() const { return c_ptr_->indexed_deltas; }

    // size of the packfile received up to now
    size_t received_bytes() const { return c_ptr_->received_bytes; }

  private:
    git_indexer_progress *c_ptr_;
  };

private:
};

} // namespace cppgit2