#pragma once
#include <cppgit2/data_buffer.hpp>
#include <cppgit2/diff.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/ownership.hpp>
#include <functional>
#include <git2.h>
#include <string>
#include <tuple>

namespace cppgit2 {

class patch : public libgit2_api {
public:
  // Default construct a patch object
  patch();

  // Construct from libgit2 C ptr
  patch(git_patch *c_ptr, ownership owner = ownership::libgit2);

  // Directly generate a patch from the difference between a blob and a buffer.
  patch(const blob &old_blob, const std::string &old_as_path,
        const void *buffer, size_t buffer_length,
        const std::string &buffer_as_path,
        const diff::options &options = diff::options());

  // Directly generate a patch from the difference between two blobs.
  patch(const blob &old_blob, const std::string &old_as_path,
        const blob &new_blob, const std::string &new_as_path,
        const diff::options &options = diff::options());

  // Directly generate a patch from the difference between two buffers.
  patch(const void *old_buffer, size_t old_buffer_length,
        const std::string &old_as_path, const void *new_buffer,
        size_t new_buffer_length, const std::string &new_as_path,
        const diff::options &options = diff::options());

  // Return a patch for an entry in the diff list.
  // The git_patch is a newly created object contains the text diffs for the
  // delta.
  patch(const diff &diff, size_t index);

  // Cleanup patch object
  ~patch();

  // Get the delta associated with a patch.
  // This delta points to internal data, owned by libgit2
  diff::delta delta() const;

  // Get the information about a hunk in a patch
  // Returns a pair of results: {hunk, lines_in_hunk}
  std::pair<diff::hunk, size_t> hunk(size_t hunk_index) const;

  // Get data about a line in a hunk of a patch.
  diff::line line_in_hunk(size_t hunk_index, size_t line_of_hunk) const;

  // Get line counts of each type in a patch.
  // Returns {Count of context lines in output,
  //          Count of addition lines in output,
  //          Count of deletion lines in output}
  // This helps imitate a diff --numstat type of output
  std::tuple<size_t, size_t, size_t> line_stats() const;

  // Get the number of hunks in a patch
  size_t num_hunks() const;

  // Get the number of lines in a hunk.
  size_t num_lines_in_hunk(size_t hunk_index) const;

  // Serialize the patch to text via callback.
  void print(std::function<void(const diff::delta &, const diff::hunk &,
                                const diff::line &)>
                 line_callback);

  // Look up size of patch diff data in bytes
  //
  // This returns the raw size of the patch data.
  // This only includes the actual data from the lines of the diff,
  // not the file or hunk headers.
  size_t size(bool include_context, bool include_hunk_headers,
              bool include_file_headers) const;

  // Get the content of a patch as a single diff text.
  data_buffer to_buffer();

  // Access to libgit2 C ptr
  const git_patch *c_ptr() const;

private:
  ownership owner_;
  git_patch *c_ptr_;
};

} // namespace cppgit2