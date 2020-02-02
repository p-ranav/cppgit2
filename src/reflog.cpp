#include <cppgit2/reflog.hpp>

namespace cppgit2 {

  reflog::reflog() : c_ptr_(nullptr), owner_(ownership::libgit2) {
    git_libgit2_init();
  }

  reflog::reflog(git_reflog * c_ptr, ownership owner) :
    c_ptr_(c_ptr), owner_(owner) {
    git_libgit2_init();
  }

  reflog::~reflog() {
    if (c_ptr_ && owner_ == ownership::user)
      git_reflog_free(c_ptr_);
    git_libgit2_shutdown();
  }

  void reflog::remove(size_t index,
		      bool rewrite_previous_entry) {
    if (git_reflog_drop(c_ptr_, index, rewrite_previous_entry))
      throw exception();
  }

  void reflog::append(const oid &id, const signature &committer,
		      const std::string &message) {
    if (git_reflog_append(c_ptr_, id.c_ptr(), committer.c_ptr(),
			  message.c_str()))
      throw exception();
  }

  reflog::entry reflog::operator[](size_t index) const {
    return reflog::entry(git_reflog_entry_byindex(c_ptr_, index));
  }

  void reflog::write_to_disk() {
    if (git_reflog_write(c_ptr_))
      throw exception();
  }

  size_t reflog::size() const {
    return git_reflog_entrycount(c_ptr_);
  }

  const git_reflog * reflog::c_ptr() const { return c_ptr_; }
  
}
