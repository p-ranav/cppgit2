#include <cppgit2/indexer.hpp>
using namespace cppgit2;

indexer::indexer() : c_ptr_(nullptr), owner_(ownership::libgit2) {}

indexer::indexer(git_indexer *c_ptr, ownership owner)
    : c_ptr_(c_ptr), owner_(ownership::libgit2) {}

indexer::indexer(const std::string &path, unsigned int mode, const odb &odb,
                 const indexer::options &options) {
  if (git_indexer_new(&c_ptr_, path.c_str(), mode, odb.c_ptr_, options.c_ptr_))
    throw git_exception();
}

indexer::~indexer() {
  if (c_ptr_ && owner_ == ownership::user)
    git_indexer_free(c_ptr_);
}

void indexer::append(void *data, size_t size) {
  if (git_indexer_append(c_ptr_, data, size,
                         const_cast<git_indexer_progress *>(progress_.c_ptr_)))
    throw git_exception();
}

void indexer::commit() {
  if (git_indexer_commit(c_ptr_,
                         const_cast<git_indexer_progress *>(progress_.c_ptr_)))
    throw git_exception();
}

oid indexer::hash() { return oid(git_indexer_hash(c_ptr_)); }