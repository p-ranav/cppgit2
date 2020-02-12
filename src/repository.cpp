#include <cppgit2/repository.hpp>

namespace cppgit2 {

repository::repository() : c_ptr_(nullptr) { git_libgit2_init(); }

repository::repository(const std::string &path, bool is_bare) {
  git_libgit2_init();
  if (git_repository_init(&c_ptr_, path.c_str(), is_bare))
    throw exception();
}

repository::~repository() {
  if (c_ptr_)
    git_repository_free(c_ptr_);
}

void repository::open(const std::string &path) {
  if (c_ptr_)
    git_repository_free(c_ptr_);
  if (git_repository_open(&c_ptr_, path.c_str()))
    throw exception();
}

void repository::open_bare(const std::string &path) {
  if (c_ptr_)
    git_repository_free(c_ptr_);
  if (git_repository_open_bare(&c_ptr_, path.c_str()))
    throw exception();
}

std::string repository::path() const {
  return std::string(git_repository_path(c_ptr_));
}

bool repository::is_bare() const { return git_repository_is_bare(c_ptr_); }

bool repository::is_empty() const {
  auto ret = git_repository_is_empty(c_ptr_);
  if (ret == 1)
    return true;
  else if (ret == 0)
    return false;
  else
    throw exception();
}

bool repository::is_shallow() const {
  return git_repository_is_shallow(c_ptr_);
}

bool repository::is_worktree() const {
  return git_repository_is_worktree(c_ptr_);
}

std::string repository::commondir() const {
  auto ret = git_repository_commondir(c_ptr_);
  if (ret)
    return std::string(ret);
  else
    throw exception();
}

cppgit2::config repository::config() const {
  cppgit2::config result;
  if (git_repository_config(&result.c_ptr_, c_ptr_))
    throw exception();
  return result;
}

cppgit2::config repository::config_snapshot() const {
  cppgit2::config result;
  if (git_repository_config_snapshot(&result.c_ptr_, c_ptr_))
    throw exception();
  return result;
}

void repository::detach_head() {
  if (git_repository_detach_head(c_ptr_))
    throw exception();
}

std::string repository::discover_path(const std::string &start_path,
                                      bool across_fs,
                                      const std::string &ceiling_dirs) {
  // TODO: Update this hardcoded size
  data_buffer buffer(256);
  if (git_repository_discover(buffer.c_ptr(), start_path.c_str(), across_fs,
                              ceiling_dirs.c_str()))
    throw exception();
  return buffer.to_string();
}

std::string repository::discover_path(const std::string &start_path) {
  return discover_path(start_path, false, "");
}

std::string repository::namespace_() const {
  auto ret = git_repository_get_namespace(c_ptr_);
  if (ret)
    return std::string(ret);
  else
    throw exception("namespace directory does not exist");
}

oid repository::hashfile(const std::string &path, object::object_type type,
                         const std::string &as_path) {
  oid result;
  if (git_repository_hashfile(result.c_ptr(), c_ptr_, path.c_str(),
                              static_cast<git_object_t>(type), as_path.c_str()))
    throw exception();
  return result;
}

oid repository::hashfile(const std::string &path, object::object_type type) {
  return hashfile(path, type, path);
}

bool repository::is_head_detached() const {
  auto ret = git_repository_head_detached(c_ptr_);
  if (ret == 0)
    return false;
  else if (ret == 1)
    return true;
  else
    throw exception();
}

bool repository::is_head_detached_for_worktree(const std::string &path) {
  auto ret = git_repository_head_detached_for_worktree(c_ptr_, path.c_str());
  if (ret == 0)
    return false;
  else if (ret == 1)
    return true;
  else
    throw exception();
}

bool repository::is_head_unborn() const {
  auto ret = git_repository_head_unborn(c_ptr_);
  if (ret == 0)
    return false;
  else if (ret == 1)
    return true;
  else
    throw exception();
}

std::pair<std::string, std::string> repository::identity() const {
  const char *name_c, *email_c;
  if (git_repository_ident(&name_c, &email_c, c_ptr_))
    throw exception();
  return {name_c ? name_c : "", email_c ? email_c : ""};
}

cppgit2::index repository::index() const {
  cppgit2::index result(nullptr);
  if (git_repository_index(&result.c_ptr_, c_ptr_))
    throw exception();
  return result;
}

const git_repository *repository::c_ptr() const { return c_ptr_; }

} // namespace cppgit2