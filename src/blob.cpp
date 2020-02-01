#include <cppgit2/blob.hpp>

namespace cppgit2 {

blob::blob() : c_ptr_(nullptr) { git_libgit2_init(); }

blob::blob(const git_blob *c_ptr) {
  git_libgit2_init();
  if (git_blob_dup(&c_ptr_, const_cast<git_blob *>(c_ptr)))
    throw exception();
}

blob::~blob() {
  if (c_ptr_)
    git_blob_free(c_ptr_);
  git_libgit2_shutdown();
}

blob blob::copy() const {
  blob result;
  if (git_blob_dup(&result.c_ptr_, c_ptr_))
    throw exception();
  return result;
}

oid blob::id() const { return oid(git_blob_id(c_ptr_)); }

bool blob::is_binary() const { return git_blob_is_binary(c_ptr_); }

const void *blob::raw_contents() const { return git_blob_rawcontent(c_ptr_); }

blob_size blob::raw_size() const { return git_blob_rawsize(c_ptr_); }

const git_blob *blob::c_ptr() const { return c_ptr_; }

} // namespace cppgit2
