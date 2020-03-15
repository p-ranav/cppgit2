#pragma once
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <git2.h>
#include <string>
#include <vector>

namespace cppgit2 {

class strarray : public libgit2_api {
public:
  // Default construction
  // Initializes libgit2
  strarray();

  // Construct from vector of strings
  strarray(const std::vector<std::string> &strings);

  // Construct from libgit2 C ptr
  strarray(const git_strarray *c_ptr);

  // Free the git_strarray struct
  ~strarray();

  // Duplicate this array
  strarray copy() const;

  // Build vector of tag names from strarray
  std::vector<std::string> to_vector() const;

  // Iterator for use in range-based for loops
  class iterator {
  public:
    explicit iterator(char **ptr) : ptr_(ptr) {}

    iterator operator++() {
      ++ptr_;
      return *this;
    }

    bool operator!=(const iterator &other) const { return ptr_ != other.ptr_; }

    std::string operator*() { return std::string(*ptr_); }

  private:
    char **ptr_;
  };

  iterator begin() { return iterator(&c_struct_.strings[0]); }

  iterator begin() const { return iterator(&c_struct_.strings[0]); }

  iterator end() { return iterator(&c_struct_.strings[c_struct_.count]); }

  iterator end() const { return iterator(&c_struct_.strings[c_struct_.count]); }

  // Returns number of strings in strarray
  size_t count() const { return c_struct_.count; }

  // Get string at index
  std::string operator[](size_t index) {
    return std::string(c_struct_.strings[index]);
  }

  // Access libgit2 C ptr
  const git_strarray *c_ptr() const;

private:
  friend class remote;
  friend class repository;
  git_strarray c_struct_;
};

} // namespace cppgit2
