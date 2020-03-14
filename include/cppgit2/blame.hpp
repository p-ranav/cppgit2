#pragma once
#include <cppgit2/bitmask_operators.hpp>
#include <cppgit2/git_exception.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/ownership.hpp>
#include <cppgit2/signature.hpp>
#include <git2.h>

namespace cppgit2 {

class blame : public libgit2_api {
public:
  // Default construct a blame object
  blame();

  // Construct from libgit2 C ptr
  // If owned by user, will be free'd in the destructor
  blame(git_blame *c_ptr, ownership owner = ownership::libgit2);

  // Free blame object if owned by user
  ~blame();

  // Get blame data for a file that has been modified in memory. The reference
  // parameter is a pre-calculated blame for the in-odb history of the file.
  // This means that once a file blame is completed (which can be expensive),
  // updating the buffer blame is very fast.
  static blame get_blame_for_buffer(const blame &reference,
                                    const std::string &buffer);

  class options : public libgit2_api {
  public:
    options() : c_ptr_(nullptr) {
      auto ret =
          git_blame_init_options(&default_options_, GIT_BLAME_OPTIONS_VERSION);
      c_ptr_ = &default_options_;
      if (ret != 0)
        throw git_exception();
    }

    options(git_blame_options *c_ptr) : c_ptr_(c_ptr) {}

    enum class flag {
      normal = 0,
      // Track lines that have moved within a file (like `git blame -M`).
      // NOT IMPLEMENTED.
      track_copies_same_file = (1 << 0),

      // Track lines that have moved across files in the same commit (like `git
      // blame -C`). NOT IMPLEMENTED.
      track_copies_same_commit_moves = (1 << 1),

      // Track lines that have been copied from another file that exists in the
      // same commit (like `git blame -CC`). Implies SAME_FILE.
      //  NOT IMPLEMENTED.
      track_copies_same_commit_copies = (1 << 2),

      // Track lines that have been copied from another file that exists in
      // *any* commit (like `git blame -CCC`). Implies SAME_COMMIT_COPIES. NOT
      // IMPLEMENTED.
      track_copies_any_commit_copies = (1 << 3),

      // Restrict the search of commits to those reachable following only the
      // first parents
      first_parent = (1 << 4),

      // Use mailmap file to map author and committer names and email addresses
      // to canonical real names and email addresses. The mailmap will be read
      // from the working directory, or HEAD in a bare repository.
      use_mailmap = (1 << 5)
    };

    unsigned int version() const { return c_ptr_->version; }
    void set_version(unsigned int version) { c_ptr_->version = version; }

    // Flags
    // A combination of blame::flag flags
    flag flags() const {
      return static_cast<blame::options::flag>(c_ptr_->flags);
    }
    void set_flags(flag flags) {
      c_ptr_->flags = static_cast<git_blame_flag_t>(flags);
    }

    // Min. match characters
    // The lower bound on the number of alphanumeric characters that must be
    // detected as moving/copying within a file for it to associate those lines
    // with the parent commit. The default value is 20. This value only takes
    // effect if any of the `GIT_BLAME_TRACK_COPIES_*` flags are specified.
    uint16_t min_match_characters() const {
      return c_ptr_->min_match_characters;
    }
    void set_min_match_characters(uint16_t min_match_characters) {
      c_ptr_->min_match_characters = min_match_characters;
    }

    // Newest commit
    // The id of the newest commit to consider. The default is HEAD.
    oid newest_commit() const { return oid(&c_ptr_->newest_commit); }
    void set_newest_commit(const oid &id) {
      git_oid_fromstr(&c_ptr_->newest_commit, id.to_hex_string().c_str());
    }

    // Oldest commit
    // The id of the oldest commit to consider. The default is the first commit
    // encountered with a NULL parent.
    oid oldest_commit() const { return oid(&c_ptr_->oldest_commit); }
    void set_oldest_commit(const oid &id) {
      git_oid_fromstr(&c_ptr_->oldest_commit, id.to_hex_string().c_str());
    }

    // Min line
    // The first line in the file to blame. The default is 1 (line numbers start
    // with 1).
    size_t min_line() const { return c_ptr_->min_line; }
    void set_min_line(size_t min_line) { c_ptr_->min_line = min_line; }

    // Max line
    // The last line in the file to blame. The default is the last line of the
    // file.
    size_t max_line() const { return c_ptr_->max_line; }
    void set_max_line(size_t max_line) { c_ptr_->max_line = max_line; }

    const git_blame_options *c_ptr() const { return c_ptr_; }

  private:
    friend class repository;
    git_blame_options *c_ptr_;
    git_blame_options default_options_;
  };

  // Always owned by libgit2
  class hunk : public libgit2_api {
  public:
    hunk(const git_blame_hunk *c_ptr) : c_ptr_(c_ptr) {}

    // Number of lines in hunk
    size_t lines_in_hunk() const { return c_ptr_->lines_in_hunk; }

    // OID of the commit where this line was last changed
    oid final_commit_id() const { return oid(&c_ptr_->final_commit_id); }

    // 1-based line number where this hunk begins, in the final version of the
    // file
    size_t final_start_line_number() const {
      return c_ptr_->final_start_line_number;
    }

    // Author of the final commit ID
    signature final_signature() const {
      return signature(c_ptr_->final_signature);
    }

    // OID of the commit where this hunk was found
    oid orig_commit_id() const { return oid(&c_ptr_->orig_commit_id); }

    // Path of the file where this hunk originated, as specified by
    // orig_commit_id
    std::string orig_path() const {
      if (c_ptr_->orig_path)
        return std::string(c_ptr_->orig_path);
      else
        return "";
    }

    // 1-based line number where this hunk begins in the file named orig_path
    // in the commit specified by orig_commit_id
    size_t orig_start_line_number() const {
      return c_ptr_->orig_start_line_number;
    }

    // Author of orig_commit_id
    signature orig_signature() const {
      return signature(c_ptr_->orig_signature);
    }

    // 1 iff the hunk has been tracked to a boundary commit
    char boundary() const { return c_ptr_->boundary; }

  private:
    const git_blame_hunk *c_ptr_;
  };

  // Gets the blame hunk at the given index.
  hunk hunk_by_index(uint32_t index) const;

  // Gets the hunk that relates to the given line number in the newest commit.
  // lineno is the (1-based) line number to find a hunk for
  hunk hunk_by_line(size_t lineno) const;

  // Gets the number of hunks that exist in the blame structure.
  size_t hunk_count() const;

  // Access libgit2 C ptr
  const git_blame *c_ptr() const;

private:
  friend class repository;
  git_blame *c_ptr_;
  ownership owner_;
};
ENABLE_BITMASK_OPERATORS(blame::options::flag);

} // namespace cppgit2
