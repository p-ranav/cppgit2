#include <cppgit2/diff.hpp>

namespace cppgit2 {

diff::diff() : c_ptr_(nullptr), owner_(ownership::libgit2) {}

diff::diff(git_diff *c_ptr, ownership owner) : c_ptr_(c_ptr), owner_(owner) {}

diff::diff(const std::string &buffer) {
  if (git_diff_from_buffer(&c_ptr_, buffer.c_str(), buffer.size()))
    throw git_exception();
}

diff::~diff() {
  if (c_ptr_ && owner_ == ownership::user)
    git_diff_free(c_ptr_);
}

diff::delta diff::compare_files(const std::pair<blob, std::string> &old_file,
                                const std::pair<blob, std::string> &new_file,
                                diff::options options) {
  delta result;
  auto file_callback = [](const git_diff_delta *delta_c, float progress,
                          void *payload) {
    auto result = (delta *)payload;
    auto &result_c = result->c_struct_;
    auto payload_c = (git_diff_delta *)delta_c;
    // Copy result over into payload ptr for use outside
    // this lambda function
    result_c.flags = payload_c->flags;
    result_c.new_file = payload_c->new_file;
    result_c.old_file = payload_c->old_file;
    result_c.nfiles = payload_c->nfiles;
    result_c.similarity = payload_c->similarity;
    result_c.status = payload_c->status;
    return 0;
  };

  const char *old_as_path_c =
      old_file.second.empty() ? nullptr : old_file.second.c_str();
  const char *new_as_path_c =
      new_file.second.empty() ? nullptr : new_file.second.c_str();

  auto ret = git_diff_blobs(old_file.first.c_ptr(), old_as_path_c,
                            new_file.first.c_ptr(), new_as_path_c,
                            options.c_ptr_, file_callback, nullptr, nullptr,
                            nullptr, (void *)(&result));
  if (ret)
    throw git_exception();
  return result;
}

const diff::delta diff::operator[](size_t index) const {
  return diff::delta(git_diff_get_delta(c_ptr_, index));
}

bool diff::is_sorted_case_sensitive() const {
  return git_diff_is_sorted_icase(c_ptr_);
}

void diff::merge(const diff &from) {
  if (git_diff_merge(c_ptr_, from.c_ptr()))
    throw git_exception();
}

size_t diff::size() const { return git_diff_num_deltas(c_ptr_); }

size_t diff::size(delta::type type) const {
  return git_diff_num_deltas_of_type(c_ptr_, static_cast<git_delta_t>(type));
}

char diff::status_char(delta::type status) const {
  return git_diff_status_char(static_cast<git_delta_t>(status));
}

std::string diff::to_string(diff::format format_type) const {
  // TODO: Check this hardcoded size
  data_buffer result;
  if (git_diff_to_buf(result.c_ptr(), c_ptr_,
                      static_cast<git_diff_format_t>(format_type)))
    throw git_exception();
  return result.to_string();
}

const git_diff *diff::c_ptr() const { return c_ptr_; }

diff::stats diff::diff_stats() const {
  diff::stats result(nullptr);
  if (git_diff_get_stats(&result.c_ptr_, c_ptr_))
    throw git_exception();
  return result;
}

data_buffer diff::format_email(const format_email_options &options) {
  data_buffer result;
  if (git_diff_format_email(result.c_ptr(), c_ptr_, options.c_ptr()))
    throw git_exception();
  return result;
}

void diff::for_each(
    std::function<void(const diff::delta &, float)> file_callback,
    std::function<void(const diff::delta &, const diff::binary &)>
        binary_callback,
    std::function<void(const diff::delta &, const diff::hunk &)> hunk_callback,
    std::function<void(const diff::delta &, const diff::hunk &,
                       const diff::line &)>
        line_callback) {

  // Prepare wrapper to pass to C API
  struct visitor_wrapper {
    std::function<void(const diff::delta &, float)> file_callback;
    std::function<void(const diff::delta &, const diff::binary &)>
        binary_callback;
    std::function<void(const diff::delta &, const diff::hunk &)> hunk_callback;
    std::function<void(const diff::delta &, const diff::hunk &,
                       const diff::line &)>
        line_callback;
  };

  visitor_wrapper wrapper;
  wrapper.file_callback = file_callback;
  wrapper.binary_callback = binary_callback;
  wrapper.hunk_callback = hunk_callback;
  wrapper.line_callback = line_callback;

  auto file_callback_c = [](const git_diff_delta *delta_c, float progress,
                            void *payload) {
    auto wrapper = reinterpret_cast<visitor_wrapper *>(payload);
    wrapper->file_callback(delta(delta_c), progress);
    return 0;
  };

  auto binary_callback_c = [](const git_diff_delta *delta_c,
                              const git_diff_binary *binary_c, void *payload) {
    auto wrapper = reinterpret_cast<visitor_wrapper *>(payload);
    if (wrapper->binary_callback)
      wrapper->binary_callback(delta(delta_c), binary(binary_c));
    return 0;
  };

  auto hunk_callback_c = [](const git_diff_delta *delta_c,
                            const git_diff_hunk *hunk_c, void *payload) {
    auto wrapper = reinterpret_cast<visitor_wrapper *>(payload);
    if (wrapper->hunk_callback)
      wrapper->hunk_callback(delta(delta_c), hunk(hunk_c));
    return 0;
  };

  auto line_callback_c = [](const git_diff_delta *delta_c,
                            const git_diff_hunk *hunk_c,
                            const git_diff_line *line_c, void *payload) {
    auto wrapper = reinterpret_cast<visitor_wrapper *>(payload);
    if (wrapper->line_callback)
      wrapper->line_callback(delta(delta_c), hunk(hunk_c), line(line_c));
    return 0;
  };

  if (git_diff_foreach(c_ptr_, file_callback_c, binary_callback_c,
                       hunk_callback_c, line_callback_c, (void *)(&wrapper)))
    throw git_exception();
}

void diff::print(diff::format format,
                 std::function<void(const diff::delta &, const diff::hunk &,
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
      wrapper->line_callback(delta(delta_c), hunk(hunk_c), line(line_c));
    return 0;
  };

  if (git_diff_print(c_ptr_, static_cast<git_diff_format_t>(format),
                     line_callback_c, (void *)(&wrapper)))
    throw git_exception();
}

void diff::diff_blob_to_buffer(
    const blob &old_blob, const std::string &old_as_path,
    const char *new_buffer, size_t new_buffer_length,
    const std::string &new_as_path, const diff::options &options,
    std::function<void(const diff::delta &, float)> file_callback,
    std::function<void(const diff::delta &, const diff::binary &)>
        binary_callback,
    std::function<void(const diff::delta &, const diff::hunk &)> hunk_callback,
    std::function<void(const diff::delta &, const diff::hunk &,
                       const diff::line &)>
        line_callback) {

  // Prepare wrapper to pass to C API
  struct visitor_wrapper {
    std::function<void(const diff::delta &, float)> file_callback;
    std::function<void(const diff::delta &, const diff::binary &)>
        binary_callback;
    std::function<void(const diff::delta &, const diff::hunk &)> hunk_callback;
    std::function<void(const diff::delta &, const diff::hunk &,
                       const diff::line &)>
        line_callback;
  };

  visitor_wrapper wrapper;
  wrapper.file_callback = file_callback;
  wrapper.binary_callback = binary_callback;
  wrapper.hunk_callback = hunk_callback;
  wrapper.line_callback = line_callback;

  auto file_callback_c = [](const git_diff_delta *delta_c, float progress,
                            void *payload) {
    auto wrapper = reinterpret_cast<visitor_wrapper *>(payload);
    wrapper->file_callback(delta(delta_c), progress);
    return 0;
  };

  auto binary_callback_c = [](const git_diff_delta *delta_c,
                              const git_diff_binary *binary_c, void *payload) {
    auto wrapper = reinterpret_cast<visitor_wrapper *>(payload);
    if (wrapper->binary_callback)
      wrapper->binary_callback(delta(delta_c), binary(binary_c));
    return 0;
  };

  auto hunk_callback_c = [](const git_diff_delta *delta_c,
                            const git_diff_hunk *hunk_c, void *payload) {
    auto wrapper = reinterpret_cast<visitor_wrapper *>(payload);
    if (wrapper->hunk_callback)
      wrapper->hunk_callback(delta(delta_c), hunk(hunk_c));
    return 0;
  };

  auto line_callback_c = [](const git_diff_delta *delta_c,
                            const git_diff_hunk *hunk_c,
                            const git_diff_line *line_c, void *payload) {
    auto wrapper = reinterpret_cast<visitor_wrapper *>(payload);
    if (wrapper->line_callback)
      wrapper->line_callback(delta(delta_c), hunk(hunk_c), line(line_c));
    return 0;
  };

  if (git_diff_blob_to_buffer(old_blob.c_ptr(), old_as_path.c_str(), new_buffer,
                              new_buffer_length, new_as_path.c_str(),
                              options.c_ptr(), file_callback_c,
                              binary_callback_c, hunk_callback_c,
                              line_callback_c, (void *)(&wrapper)))
    throw git_exception();
}

void diff::diff_between_buffers(
    const void *old_buffer, size_t old_buffer_length,
    const std::string &old_as_path, const void *new_buffer,
    size_t new_buffer_length, const std::string &new_as_path,
    const diff::options &options,
    std::function<void(const diff::delta &, float)> file_callback,
    std::function<void(const diff::delta &, const diff::binary &)>
        binary_callback,
    std::function<void(const diff::delta &, const diff::hunk &)> hunk_callback,
    std::function<void(const diff::delta &, const diff::hunk &,
                       const diff::line &)>
        line_callback) {

  // Prepare wrapper to pass to C API
  struct visitor_wrapper {
    std::function<void(const diff::delta &, float)> file_callback;
    std::function<void(const diff::delta &, const diff::binary &)>
        binary_callback;
    std::function<void(const diff::delta &, const diff::hunk &)> hunk_callback;
    std::function<void(const diff::delta &, const diff::hunk &,
                       const diff::line &)>
        line_callback;
  };

  visitor_wrapper wrapper;
  wrapper.file_callback = file_callback;
  wrapper.binary_callback = binary_callback;
  wrapper.hunk_callback = hunk_callback;
  wrapper.line_callback = line_callback;

  auto file_callback_c = [](const git_diff_delta *delta_c, float progress,
                            void *payload) {
    auto wrapper = reinterpret_cast<visitor_wrapper *>(payload);
    wrapper->file_callback(delta(delta_c), progress);
    return 0;
  };

  auto binary_callback_c = [](const git_diff_delta *delta_c,
                              const git_diff_binary *binary_c, void *payload) {
    auto wrapper = reinterpret_cast<visitor_wrapper *>(payload);
    if (wrapper->binary_callback)
      wrapper->binary_callback(delta(delta_c), binary(binary_c));
    return 0;
  };

  auto hunk_callback_c = [](const git_diff_delta *delta_c,
                            const git_diff_hunk *hunk_c, void *payload) {
    auto wrapper = reinterpret_cast<visitor_wrapper *>(payload);
    if (wrapper->hunk_callback)
      wrapper->hunk_callback(delta(delta_c), hunk(hunk_c));
    return 0;
  };

  auto line_callback_c = [](const git_diff_delta *delta_c,
                            const git_diff_hunk *hunk_c,
                            const git_diff_line *line_c, void *payload) {
    auto wrapper = reinterpret_cast<visitor_wrapper *>(payload);
    if (wrapper->line_callback)
      wrapper->line_callback(delta(delta_c), hunk(hunk_c), line(line_c));
    return 0;
  };

  if (git_diff_buffers(old_buffer, old_buffer_length, old_as_path.c_str(),
                       new_buffer, new_buffer_length, new_as_path.c_str(),
                       options.c_ptr(), file_callback_c, binary_callback_c,
                       hunk_callback_c, line_callback_c, (void *)(&wrapper)))
    throw git_exception();
}

void diff::find_similar(const find_options &options) {
  if (git_diff_find_similar(c_ptr_, options.c_ptr()))
    throw git_exception();
}

} // namespace cppgit2