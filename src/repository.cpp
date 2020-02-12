#include <cppgit2/repository.hpp>

namespace cppgit2 {

repository::repository() : c_ptr_(nullptr) {
  git_libgit2_init();
}

repository::repository(const std::string &path, bool is_bare) {
  git_libgit2_init();
  if (git_repository_init(&c_ptr_, path.c_str(), is_bare))
    throw exception();
}

repository::~repository() {
  if (c_ptr_) {
    git_repository_free(c_ptr_);
  }
}

void repository::open(const std::string &path) {
  if (c_ptr_) {
    git_repository_free(c_ptr_);
  }
  if (git_repository_open(&c_ptr_, path.c_str()))
    throw exception();
}

void repository::open_bare(const std::string &path) {
  if (c_ptr_) {
    git_repository_free(c_ptr_);
  }
  if (git_repository_open_bare(&c_ptr_, path.c_str()))
    throw exception();
}

std::string repository::path() const {
  return std::string(git_repository_path(c_ptr_));
}

}