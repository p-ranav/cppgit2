#pragma once
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/data_buffer.hpp>

namespace cppgit2 {

using blob_size = git_off_t;

// Always owned by user
// Free'd in destructor
class blob : public libgit2_api {
public:
  // Default construct a blob
  blob();

  // Construct a blob from libgit2 C ptr
  // Calls git_blob_dup to create a duplicate
  blob(const git_blob *c_ptr);

  // Free blob with git_blob_free
  ~blob();

  // Owner repository
  class repository owner() const;

  // Create an in-memory copy of a blob
  blob copy() const;

  // SHA1 hash for this blob
  oid id() const;

  // Determine if the blob content is binary or not
  bool is_binary() const;

  // Get read-only buffer with raw contents of this blob
  const void *raw_contents() const;

  // Get size in bytes of the contents of this blob
  blob_size raw_size() const;

  // Access libgit2 C ptr
  const git_blob *c_ptr() const;

  class filter : public libgit2_api {
  public:

    // Flag to control behavior of git_blob_filter
    enum class flag {
      // when set, filters will not be applied to binary files
      check_for_binary = (1 << 0),
      // When set, filters will not load configuration from the
      // system-wide `gitattributes` in `/etc` (or system equivalent).
      no_system_attributes = (1 << 1),
      // When set, filters will be loaded from a `.gitattributes` file
      // in the HEAD commit.
      attributes_from_head = (1 << 2)
    };

    friend inline flag operator|(flag a, flag b) {
      return static_cast<flag>(static_cast<uint32_t>(a) |
        static_cast<uint32_t>(b));
    }

    friend inline flag operator&(flag a, flag b) {
      return static_cast<flag>(static_cast<uint32_t>(a)&
        static_cast<uint32_t>(b));
    }

    class options : public libgit2_api {
    public:
      options() {
        c_ptr_ = &default_options_;
      }

      options(git_blob_filter_options* c_ptr) : c_ptr_(c_ptr) {}

      int version() const { return c_ptr_->version; }
      void set_version(int version) { c_ptr_->version = version; }

      filter::flag flags() const { return static_cast<filter::flag>(c_ptr_->flags); }
      void set_flags(filter::flag flags) {
        c_ptr_->flags = static_cast<uint32_t>(flags);
      }

      git_blob_filter_options* c_ptr() { return c_ptr_; }

    private:
      friend class diff;
      git_blob_filter_options* c_ptr_;
      git_blob_filter_options default_options_;
    };
  };

  // Get a buffer with the filtered content of a blob.
  data_buffer filter(const std::string &as_path, filter::options options = filter::options());

private:
  friend class repository;
  git_blob *c_ptr_;
};

} // namespace cppgit2
