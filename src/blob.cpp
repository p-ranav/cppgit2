#include <cppgit2/repository.hpp>

namespace cppgit2 {

blob::blob() : c_ptr_(nullptr) {}

blob::blob(const git_blob *c_ptr) {
  if (git_blob_dup(&c_ptr_, const_cast<git_blob *>(c_ptr)))
    throw git_exception();
}

blob::~blob() {
  if (c_ptr_)
    git_blob_free(c_ptr_);
}

repository blob::owner() const { return repository(git_blob_owner(c_ptr_)); }

blob blob::copy() const {
  blob result;
  if (git_blob_dup(&result.c_ptr_, c_ptr_))
    throw git_exception();
  return result;
}

oid blob::id() const { return oid(git_blob_id(c_ptr_)); }

bool blob::is_binary() const { return git_blob_is_binary(c_ptr_); }

const void *blob::raw_contents() const { return git_blob_rawcontent(c_ptr_); }

blob_size blob::raw_size() const { return git_blob_rawsize(c_ptr_); }

const git_blob *blob::c_ptr() const { return c_ptr_; }

data_buffer blob::filter(const std::string& as_path, filter::options options) {
  data_buffer result(nullptr);
  if (git_blob_filter(result.c_ptr(), c_ptr_, as_path.c_str(), options.c_ptr()))
    throw git_exception();
  return result;
}

} // namespace cppgit2
