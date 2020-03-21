#pragma once
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/odb.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/ownership.hpp>
#include <functional>
#include <git2.h>

namespace cppgit2 {

class indexer : public libgit2_api {
public:
  // Default construct an indexer
  indexer();

  // Construct from libgit2 C ptr
  indexer(git_indexer *c_ptr, ownership owner);

  // Free indexer if owned by user
  ~indexer();

  // This structure is used to provide callers information about the progress of
  // indexing a packfile, either directly or part of a fetch or clone that
  // downloads a packfile.
  class progress : public libgit2_api {
  public:
    // Default construct progress
    progress() : c_ptr_(nullptr) { c_ptr_ = &default_; }

    // Construct from libgit2 C ptr
    progress(const git_indexer_progress *c_ptr) : c_ptr_(c_ptr) {}

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

    const git_indexer_progress *c_ptr() const { return c_ptr_; }

  private:
    friend indexer;
    const git_indexer_progress *c_ptr_;
    git_indexer_progress default_;
  };

  // Add data to the indexer
  void append(void *data, size_t size);

  // Finalize the pack and index
  // Resolve any pending deltas and write out the index file
  void commit();

  // Get the packfile's hash
  // A packfile's name is derived from the sorted hashing of
  // all object names. This is only correct after the index
  // has been finalized.
  oid hash();

  class options : public libgit2_api {
  public:
    options() {
      auto ret = git_indexer_init_options(&default_options_,
                                          GIT_INDEXER_OPTIONS_VERSION);
      c_ptr_ = &default_options_;
      if (ret != 0)
        throw git_exception();
    }

    options(git_indexer_options *c_ptr) : c_ptr_(c_ptr) {}

    // Version
    unsigned int version() const { return c_ptr_->version; }
    void set_version(unsigned int value) { c_ptr_->version = value; }

    // Indexer Progress callback
    void set_indexer_progress_callback(
        std::function<void(const indexer::progress &progress)> callback) {
      struct visitor_wrapper {
        std::function<void(const indexer::progress &progress)> fn;
      };

      visitor_wrapper wrapper;
      wrapper.fn = callback;

      c_ptr_->progress_cb = [](const git_indexer_progress *stats,
                               void *payload) {
        auto wrapper = reinterpret_cast<visitor_wrapper *>(payload);
        wrapper->fn(indexer::progress(stats));
        return 0;
      };
    }

    // Do connectivity checks for the received pack
    unsigned char verify() const { return c_ptr_->verify; }

    const git_indexer_options *c_ptr() const { return c_ptr_; }

  private:
    friend indexer;
    git_indexer_options *c_ptr_;
    git_indexer_options default_options_;
  };

  // Create a new indexer instance
  indexer(const std::string &path, unsigned int mode, const odb &odb,
          const indexer::options &options = indexer::options());

private:
  friend class repository;
  progress progress_; // stat storage
  ownership owner_;
  git_indexer *c_ptr_;
};

} // namespace cppgit2