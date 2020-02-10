#include <cppgit2/note.hpp>

namespace cppgit2 {

note::note() : c_ptr_(nullptr), owner_(ownership::libgit2) {
  git_libgit2_init();
}

note::note(git_note * c_ptr, ownership owner) : c_ptr_(c_ptr), owner_(owner) {
  git_libgit2_init();
}

note::~note() {
  if (c_ptr_ && owner_ == ownership::user)
    git_note_free(c_ptr_);
  git_libgit2_shutdown();
}

signature note::author() const { return signature(git_note_author(c_ptr_)); }

signature note::committer() const {
    return signature(git_note_committer(c_ptr_));
}

oid note::id() const {
    return oid(git_note_id(c_ptr_));
}

std::string note::message() const {
  auto ret = git_note_message(c_ptr_);
  if (ret)
    return std::string(ret);
  else 
    return "";
}

const git_note * note::c_ptr() const { return c_ptr_; }

}