#pragma once
#include <cppgit2/blob>
#include <cppgit2/data_buffer.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/ownership.hpp>
#include <git2.h>

namespace cppgit2 {

class filter {
public:
  // List of filters to be applied
  // This represents a list of filters to be applied to a file / blob. You can
  // build the list with one call, apply it with another, and dispose it with a
  // third. In typical usage, there are not many occasions where a
  // git_filter_list is needed directly since the library will generally handle
  // conversions for you, but it can be convenient to be able to build and apply
  // the list sometimes.
  class list {
  public:
    // Default construct a filter list
    list() : c_ptr_(nullptr), owner_(ownership::libgit2) {}

    // Construct from libgit2 C ptr
    list(git_filter_list *c_ptr, ownership owner = ownership::libgit2)
        : c_ptr_(c_ptr), owner_(owner) {}

    // Cleanup filter list if owned by user
    ~list() {
      if (c_ptr_ && owner_ == ownership::user)
        git_filter_list_free(c_ptr_);
    }

  private:
    ownership owner_;
    git_filter_list *c_ptr_;
  };

  // Apply a filter list to the contents of a blob
  // Takes a list of filters to a apply and a blob to filter
  // Returns a buffer with the filtered file
  static data_buffer apply_to_blob(const filter::list &filters,
                                   const blob &blob);
};

} // namespace cppgit2