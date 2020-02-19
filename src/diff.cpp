#include <cppgit2/diff.hpp>

namespace cppgit2 {

diff::diff() : c_ptr_(nullptr), owner_(ownership::libgit2) {}

diff::diff(git_diff *c_ptr, ownership owner) : c_ptr_(c_ptr), owner_(owner) {}

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
      old_file.second == "" ? nullptr : old_file.second.c_str();
  const char *new_as_path_c =
      new_file.second == "" ? nullptr : new_file.second.c_str();

  auto ret = git_diff_blobs(old_file.first.c_ptr(), old_as_path_c,
                            new_file.first.c_ptr(), new_as_path_c,
                            options.c_ptr_, file_callback, nullptr, nullptr,
                            nullptr, (void *)(&result));
  if (ret)
    throw exception();
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
    throw exception();
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
  data_buffer result(1024);
  if (git_diff_to_buf(result.c_ptr(), c_ptr_,
                      static_cast<git_diff_format_t>(format_type)))
    throw exception();
  return result.to_string();
}

const git_diff *diff::c_ptr() const { return c_ptr_; }

} // namespace cppgit2