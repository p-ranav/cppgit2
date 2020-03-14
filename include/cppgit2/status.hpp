#pragma once
#include <cppgit2/bitmask_operators.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/ownership.hpp>
#include <cppgit2/strarray.hpp>
#include <cppgit2/tree.hpp>
#include <git2.h>

namespace cppgit2 {

class status : public libgit2_api {
public:
  // Status flags for a single file.
  //
  // The `index_` set of flags
  // represents the status of file in the index relative to the HEAD
  //
  // `wt_` set of flags represent the status of the file in the
  // working directory relative to the index.
  enum class status_type {
    current = 0,

    index_new = (1u << 0),
    index_modified = (1u << 1),
    index_deleted = (1u << 2),
    index_renamed = (1u << 3),
    index_typechange = (1u << 4),

    wt_new = (1u << 7),
    wt_modified = (1u << 8),
    wt_deleted = (1u << 9),
    wt_typechange = (1u << 10),
    wt_renamed = (1u << 11),
    wt_unreadable = (1u << 12),

    ignored = (1u << 14),
    conflicted = (1u << 15)
  };

  // Select the files on which to report status
  enum class show { index_and_workdir = 0, index_only = 1, workdir_only = 2 };

  class options : public libgit2_api {
  public:
    options() {
      auto ret = git_status_init_options(&default_options_,
                                         GIT_STATUS_OPTIONS_VERSION);
      c_ptr_ = &default_options_;
      if (ret != 0)
        throw git_exception();
    }

    options(git_status_options *c_ptr) : c_ptr_(c_ptr) {}

    // Version
    unsigned int version() const { return c_ptr_->version; }
    void set_version(unsigned int value) { c_ptr_->version = value; }

    // Show setting
    status::show show() const {
      return static_cast<status::show>(c_ptr_->show);
    }
    void set_show(status::show value) {
      c_ptr_->show = static_cast<git_status_show_t>(value);
    }

    enum class flag {
      include_untracked = (1u << 0),
      include_ignored = (1u << 1),
      include_unmodified = (1u << 2),
      exclude_submodules = (1u << 3),
      recurse_untracked_dirs = (1u << 4),
      disable_pathspec_match = (1u << 5),
      recurse_ignored_dirs = (1u << 6),
      renames_head_to_index = (1u << 7),
      renames_index_to_workdir = (1u << 8),
      sort_case_sensitively = (1u << 9),
      sort_case_insensitively = (1u << 10),
      renames_from_rewrites = (1u << 11),
      no_refresh = (1u << 12),
      update_index = (1u << 13),
      include_unreadable = (1u << 14),
      include_unreadable_as_untracked = (1u << 15)
    };

    // Flags
    flag flags() const { return static_cast<flag>(c_ptr_->flags); }
    void set_flags(flag value) {
      c_ptr_->flags = static_cast<unsigned int>(value);
    }

    // Pathspec
    strarray pathspec() const { return strarray(&c_ptr_->pathspec); }
    void set_pathspec(const strarray &value) {
      c_ptr_->pathspec = *(value.c_ptr());
    }

    // Baseline tree
    tree baseline() const { return tree(c_ptr_->baseline); }
    void set_baseline(const tree &baseline_tree) {
      c_ptr_->baseline = const_cast<git_tree *>(baseline_tree.c_ptr());
    }

    // Access libgit2 C ptr
    const git_status_options *c_ptr() const { return c_ptr_; }

  private:
    friend status;
    git_status_options *c_ptr_;
    git_status_options default_options_;
  };

  class entry : public libgit2_api {
  public:
    entry(const git_status_entry *c_ptr) : c_ptr_(c_ptr) {}

  private:
    const git_status_entry *c_ptr_;
  };

  class list : public libgit2_api {
  public:
    list() : c_ptr_(nullptr), owner_(ownership::libgit2) {}

    list(git_status_list *c_ptr, ownership owner = ownership::libgit2)
        : c_ptr_(c_ptr), owner_(owner) {}

    ~list() {
      if (c_ptr_ && owner_ == ownership::user)
        git_status_list_free(c_ptr_);
    }

    // Get one of the entries in the status list.
    // The entry is not modifiable and should not be freed.
    const entry operator[](size_t index) const {
      return entry(git_status_byindex(c_ptr_, index));
    }

    // Gets the count of status entries in this list.
    size_t size() const { return git_status_list_entrycount(c_ptr_); }

  private:
    friend class status;
    friend class repository;
    ownership owner_;
    git_status_list *c_ptr_;
  };

private:
};
ENABLE_BITMASK_OPERATORS(status::status_type);
ENABLE_BITMASK_OPERATORS(status::options::flag);

} // namespace cppgit2