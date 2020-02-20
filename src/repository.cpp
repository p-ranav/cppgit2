#include <cppgit2/repository.hpp>

namespace cppgit2 {

repository::repository(git_repository *c_ptr) : c_ptr_(c_ptr) {}

repository::repository(const std::string &path, bool is_bare) {
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
  data_buffer buffer(1024);
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

std::string repository::path(repository::item item) const {
  // TODO: Update this hardcoded size
  data_buffer buffer(1024);
  if (git_repository_item_path(buffer.c_ptr(), c_ptr_,
                               static_cast<git_repository_item_t>(item)))
    throw exception();
  return buffer.to_string();
}

std::string repository::message() const {
  data_buffer buffer(1024);
  if (git_repository_message(buffer.c_ptr(), c_ptr_))
    throw exception();
  return buffer.to_string();
}

void repository::remove_message() { git_repository_message_remove(c_ptr_); }

void repository::set_head(const std::string &refname) {
  if (git_repository_set_head(c_ptr_, refname.c_str()))
    throw exception();
}

void repository::set_head_detached(const oid &commitish) {
  if (git_repository_set_head_detached(c_ptr_, commitish.c_ptr()))
    throw exception();
}

void repository::set_identity(const std::string &name,
                              const std::string &email) {
  if (git_repository_set_ident(c_ptr_, name.c_str(), email.c_str()))
    throw exception();
}

void repository::unset_identity() {
  if (git_repository_set_ident(c_ptr_, nullptr, nullptr))
    throw exception();
}

void repository::set_namespace(const std::string &namespace_) {
  if (git_repository_set_namespace(c_ptr_, namespace_.c_str()))
    throw exception();
}

void repository::set_workdir(const std::string &workdir, bool update_gitlink) {
  if (git_repository_set_workdir(c_ptr_, workdir.c_str(), update_gitlink))
    throw exception();
}

void repository::cleanup_state() {
  if (git_repository_state_cleanup(c_ptr_))
    throw exception();
}

repository::repository_state repository::state() const {
  switch (git_repository_state(c_ptr_)) {
  case GIT_REPOSITORY_STATE_NONE:
    return repository_state::none;
  case GIT_REPOSITORY_STATE_REBASE_INTERACTIVE:
    return repository_state::rebase_interactive;
  case GIT_REPOSITORY_STATE_REBASE_MERGE:
    return repository_state::rebase_merge;
  case GIT_REPOSITORY_STATE_REBASE:
    return repository_state::rebase;
  case GIT_REPOSITORY_STATE_APPLY_MAILBOX:
    return repository_state::apply_mailbox;
  case GIT_REPOSITORY_STATE_APPLY_MAILBOX_OR_REBASE:
    return repository_state::apply_mailbox_or_rebase;
  case GIT_REPOSITORY_STATE_MERGE:
    return repository_state::merge;
  case GIT_REPOSITORY_STATE_REVERT:
    return repository_state::revert;
  case GIT_REPOSITORY_STATE_REVERT_SEQUENCE:
    return repository_state::revert_sequence;
  case GIT_REPOSITORY_STATE_CHERRYPICK:
    return repository_state::cherrypick;
  case GIT_REPOSITORY_STATE_CHERRYPICK_SEQUENCE:
    return repository_state::cherrypick_sequence;
  case GIT_REPOSITORY_STATE_BISECT:
    return repository_state::bisect;
  case -1:
  default:
    return repository_state::unknown;
  }
}

std::string repository::workdir() const {
  auto ret = git_repository_workdir(c_ptr_);
  if (ret) {
    return std::string(ret);
  } else {
    throw exception("working directory does not exist");
  }
}

const git_repository *repository::c_ptr() const { return c_ptr_; }

annotated_commit
repository::create_annotated_commit(const std::string &branch_name,
                                    const std::string &remote_url,
                                    const oid &id) {
  annotated_commit result;
  if (git_annotated_commit_from_fetchhead(&result.c_ptr_, c_ptr_,
                                          branch_name.c_str(),
                                          remote_url.c_str(), id.c_ptr()))
    throw exception();
  return result;
}

annotated_commit
repository::create_annotated_commit(const std::string &revspec) {
  annotated_commit result;
  if (git_annotated_commit_from_revspec(&result.c_ptr_, c_ptr_,
                                        revspec.c_str()))
    throw exception();
  return result;
}

annotated_commit repository::create_annotated_commit(const reference &ref) {
  annotated_commit result;
  if (git_annotated_commit_from_ref(&result.c_ptr_, c_ptr_, ref.c_ptr()))
    throw exception();
  return result;
}

annotated_commit repository::lookup_annotated_commit(const oid &id) {
  annotated_commit result;
  if (git_annotated_commit_lookup(&result.c_ptr_, c_ptr_, id.c_ptr()))
    throw exception();
  return result;
}

} // namespace cppgit2