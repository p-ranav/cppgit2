#include <cppgit2/patch.hpp>
using namespace cppgit2;
#include <functional>

patch::patch() : c_ptr_(nullptr), owner_(ownership::libgit2) {}

patch::patch(git_patch *c_ptr, ownership owner)
    : c_ptr_(c_ptr), owner_(owner) {}

patch::patch(const blob &old_blob, const std::string &old_as_path,
             const void *buffer, size_t buffer_length,
             const std::string &buffer_as_path, const diff::options &options) {
  owner_ = ownership::user;
  if (git_patch_from_blob_and_buffer(&c_ptr_, old_blob.c_ptr(),
                                     old_as_path.c_str(), buffer, buffer_length,
                                     buffer_as_path.c_str(), options.c_ptr()))
    throw git_exception();
}

patch::patch(const blob &old_blob, const std::string &old_as_path,
             const blob &new_blob, const std::string &new_as_path,
             const diff::options &options) {
  owner_ = ownership::user;
  if (git_patch_from_blobs(&c_ptr_, old_blob.c_ptr(), old_as_path.c_str(),
                           new_blob.c_ptr(), new_as_path.c_str(),
                           options.c_ptr()))
    throw git_exception();
}

patch::patch(const void *old_buffer, size_t old_buffer_length,
             const std::string &old_as_path, const void *new_buffer,
             size_t new_buffer_length, const std::string &new_as_path,
             const diff::options &options) {
  owner_ = ownership::user;
  if (git_patch_from_buffers(&c_ptr_, old_buffer, old_buffer_length,
                             old_as_path.c_str(), new_buffer, new_buffer_length,
                             new_as_path.c_str(), options.c_ptr()))
    throw git_exception();
}

patch::patch(const diff &diff, size_t index) {
  owner_ = ownership::user;
  if (git_patch_from_diff(&c_ptr_, diff.c_ptr_, index))
    throw git_exception();
}

patch::~patch() {
  if (c_ptr_ && owner_ == ownership::user)
    git_patch_free(c_ptr_);
}

diff::delta patch::delta() const {
  return diff::delta(git_patch_get_delta(c_ptr_));
}

std::pair<diff::hunk, size_t> patch::hunk(size_t hunk_index) const {
  diff::hunk hunk_result(nullptr);
  auto hunk_result_ptr = hunk_result.c_ptr();
  size_t lines_in_hunk;
  if (git_patch_get_hunk(&hunk_result_ptr, &lines_in_hunk, c_ptr_, hunk_index))
    throw git_exception();
  return std::pair<diff::hunk, size_t>{hunk_result, lines_in_hunk};
}

diff::line patch::line_in_hunk(size_t hunk_index, size_t line_of_hunk) const {
  diff::line result(nullptr);
  auto result_ptr = result.c_ptr();
  if (git_patch_get_line_in_hunk(&result_ptr, c_ptr_, hunk_index, line_of_hunk))
    throw git_exception();
  return result;
}

std::tuple<size_t, size_t, size_t> patch::line_stats() const {
  size_t context_lines = 0, addition_lines = 0, deletion_lines = 0;
  if (git_patch_line_stats(&context_lines, &addition_lines, &deletion_lines,
                           c_ptr_))
    throw git_exception();
  return std::tuple<size_t, size_t, size_t>{context_lines, addition_lines,
                                            deletion_lines};
}

size_t patch::num_hunks() const { return git_patch_num_hunks(c_ptr_); }

size_t patch::num_lines_in_hunk(size_t hunk_index) const {
  return git_patch_num_lines_in_hunk(c_ptr_, hunk_index);
}

void patch::print(std::function<void(const diff::delta &, const diff::hunk &,
                                     const diff::line &)>
                      line_callback) {

  // Prepare wrapper to pass to C API
  struct visitor_wrapper {
    std::function<void(const diff::delta &, const diff::hunk &,
                       const diff::line &)>
        line_callback;
  };

  visitor_wrapper wrapper;
  wrapper.line_callback = line_callback;

  auto line_callback_c = [](const git_diff_delta *delta_c,
                            const git_diff_hunk *hunk_c,
                            const git_diff_line *line_c, void *payload) {
    auto wrapper = reinterpret_cast<visitor_wrapper *>(payload);
    if (wrapper->line_callback)
      wrapper->line_callback(diff::delta(delta_c), diff::hunk(hunk_c),
                             diff::line(line_c));
    return 0;
  };

  if (git_patch_print(c_ptr_, line_callback_c, (void *)(&wrapper)))
    throw git_exception();
}

size_t patch::size(bool include_context, bool include_hunk_headers,
                   bool include_file_headers) const {
  return git_patch_size(c_ptr_, include_context, include_hunk_headers,
                        include_file_headers);
}

data_buffer patch::to_buffer() {
  data_buffer result;
  if (git_patch_to_buf(result.c_ptr(), c_ptr_))
    throw git_exception();
  return result;
}

const git_patch *patch::c_ptr() const { return c_ptr_; }