#include <cppgit2/repository.hpp>

namespace cppgit2 {

repository::repository(git_repository *c_ptr) : c_ptr_(c_ptr) {}

repository::repository(const std::string &path, bool is_bare) {
  if (git_repository_init(&c_ptr_, path.c_str(), is_bare))
    throw git_exception();
}

repository::~repository() {
  if (c_ptr_)
    git_repository_free(c_ptr_);
}

void repository::open(const std::string &path) {
  if (c_ptr_)
    git_repository_free(c_ptr_);
  if (git_repository_open(&c_ptr_, path.c_str()))
    throw git_exception();
}

void repository::open_bare(const std::string &path) {
  if (c_ptr_)
    git_repository_free(c_ptr_);
  if (git_repository_open_bare(&c_ptr_, path.c_str()))
    throw git_exception();
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
    throw git_exception();
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
    throw git_exception();
}

cppgit2::config repository::config() const {
  cppgit2::config result;
  if (git_repository_config(&result.c_ptr_, c_ptr_))
    throw git_exception();
  return result;
}

cppgit2::config repository::config_snapshot() const {
  cppgit2::config result;
  if (git_repository_config_snapshot(&result.c_ptr_, c_ptr_))
    throw git_exception();
  return result;
}

void repository::detach_head() {
  if (git_repository_detach_head(c_ptr_))
    throw git_exception();
}

std::string repository::discover_path(const std::string &start_path,
                                      bool across_fs,
                                      const std::string &ceiling_dirs) {
  // TODO: Update this hardcoded size
  data_buffer buffer(1024);
  if (git_repository_discover(buffer.c_ptr(), start_path.c_str(), across_fs,
                              ceiling_dirs.c_str()))
    throw git_exception();
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
    throw git_exception("namespace directory does not exist");
}

oid repository::hashfile(const std::string &path, object::object_type type,
                         const std::string &as_path) {
  oid result;
  if (git_repository_hashfile(result.c_ptr(), c_ptr_, path.c_str(),
                              static_cast<git_object_t>(type), as_path.c_str()))
    throw git_exception();
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
    throw git_exception();
}

bool repository::is_head_detached_for_worktree(const std::string &path) {
  auto ret = git_repository_head_detached_for_worktree(c_ptr_, path.c_str());
  if (ret == 0)
    return false;
  else if (ret == 1)
    return true;
  else
    throw git_exception();
}

bool repository::is_head_unborn() const {
  auto ret = git_repository_head_unborn(c_ptr_);
  if (ret == 0)
    return false;
  else if (ret == 1)
    return true;
  else
    throw git_exception();
}

std::pair<std::string, std::string> repository::identity() const {
  const char *name_c, *email_c;
  if (git_repository_ident(&name_c, &email_c, c_ptr_))
    throw git_exception();
  return {name_c ? name_c : "", email_c ? email_c : ""};
}

cppgit2::index repository::index() const {
  cppgit2::index result(nullptr);
  if (git_repository_index(&result.c_ptr_, c_ptr_))
    throw git_exception();
  return result;
}

std::string repository::path(repository::item item) const {
  // TODO: Update this hardcoded size
  data_buffer buffer(1024);
  if (git_repository_item_path(buffer.c_ptr(), c_ptr_,
                               static_cast<git_repository_item_t>(item)))
    throw git_exception();
  return buffer.to_string();
}

std::string repository::message() const {
  data_buffer buffer(1024);
  if (git_repository_message(buffer.c_ptr(), c_ptr_))
    throw git_exception();
  return buffer.to_string();
}

void repository::remove_message() { git_repository_message_remove(c_ptr_); }

void repository::set_head(const std::string &refname) {
  if (git_repository_set_head(c_ptr_, refname.c_str()))
    throw git_exception();
}

void repository::set_head_detached(const oid &commitish) {
  if (git_repository_set_head_detached(c_ptr_, commitish.c_ptr()))
    throw git_exception();
}

void repository::set_identity(const std::string &name,
                              const std::string &email) {
  if (git_repository_set_ident(c_ptr_, name.c_str(), email.c_str()))
    throw git_exception();
}

void repository::unset_identity() {
  if (git_repository_set_ident(c_ptr_, nullptr, nullptr))
    throw git_exception();
}

void repository::set_namespace(const std::string &namespace_) {
  if (git_repository_set_namespace(c_ptr_, namespace_.c_str()))
    throw git_exception();
}

void repository::set_workdir(const std::string &workdir, bool update_gitlink) {
  if (git_repository_set_workdir(c_ptr_, workdir.c_str(), update_gitlink))
    throw git_exception();
}

void repository::cleanup_state() {
  if (git_repository_state_cleanup(c_ptr_))
    throw git_exception();
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
    throw git_exception("working directory does not exist");
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
    throw git_exception();
  return result;
}

annotated_commit
repository::create_annotated_commit(const std::string &revspec) {
  annotated_commit result;
  if (git_annotated_commit_from_revspec(&result.c_ptr_, c_ptr_,
                                        revspec.c_str()))
    throw git_exception();
  return result;
}

annotated_commit repository::create_annotated_commit(const reference &ref) {
  annotated_commit result;
  if (git_annotated_commit_from_ref(&result.c_ptr_, c_ptr_, ref.c_ptr()))
    throw git_exception();
  return result;
}

annotated_commit repository::lookup_annotated_commit(const oid &id) {
  annotated_commit result;
  if (git_annotated_commit_lookup(&result.c_ptr_, c_ptr_, id.c_ptr()))
    throw git_exception();
  return result;
}

void repository::apply_diff(const diff &diff, apply::location location,
                            const apply::options &options) {
  if (git_apply(c_ptr_, const_cast<git_diff *>(diff.c_ptr()),
                static_cast<git_apply_location_t>(location), options.c_ptr()))
    throw git_exception();
}

cppgit2::index repository::apply_diff(const tree &preimage, const diff &diff,
                                      const apply::options &options) {
  git_index *result; // the postimage of the application
  if (git_apply_to_tree(&result, c_ptr_,
                        const_cast<git_tree *>(preimage.c_ptr()),
                        const_cast<git_diff *>(diff.c_ptr()), options.c_ptr()))
    throw git_exception();
  return cppgit2::index(result, ownership::user);
}

void repository::add_attributes_macro(const std::string &name,
                                      const std::string &values) {
  if (git_attr_add_macro(c_ptr_, name.c_str(), values.c_str()))
    throw git_exception();
}

void repository::flush_attributes_cache() {
  if (git_attr_cache_flush(c_ptr_))
    throw git_exception();
}

void repository::for_each_attribute(
    attribute::flag flags, const std::string &path,
    std::function<void(const std::string &, const std::string &)> visitor) {

  struct visitor_wrapper {
    std::function<void(const std::string &, const std::string &)> fn;
  };

  visitor_wrapper wrapper;
  wrapper.fn = visitor;

  auto callback_c = [](const char *name, const char *value, void *payload) {
    auto wrapper = reinterpret_cast<visitor_wrapper *>(payload);
    wrapper->fn(name, value);
    return 0;
  };

  if (git_attr_foreach(c_ptr_, static_cast<uint32_t>(flags), path.c_str(),
                       callback_c, (void *)(&wrapper)))
    throw git_exception();
}

std::string repository::lookup_attribute(attribute::flag flags,
                                         const std::string &path,
                                         const std::string &name) {
  const char *result;
  if (git_attr_get(&result, c_ptr_, static_cast<uint32_t>(flags), path.c_str(),
                   name.c_str()))
    throw git_exception();
  if (result)
    return std::string(result);
  else
    return "";
}

std::vector<std::string>
repository::lookup_multiple_attributes(attribute::flag flags,
                                       const std::string &path,
                                       const std::vector<std::string> &names) {
  const char** values = (const char**)malloc(names.size());
  // const char *values[names.size()]; // TODO: Fix this

  std::vector<const char *> names_c;
  for (auto &name : names)
    names_c.push_back(name.c_str());

  if (git_attr_get_many(values, c_ptr_, static_cast<uint32_t>(flags),
                        path.c_str(), names.size(), names_c.data()))
    throw git_exception();

  std::vector<std::string> result;
  for (size_t i = 0; i < names.size(); ++i) {
    if (values[i]) {
      result.push_back(values[i]);
    } else {
      result.push_back("");
    }
  }
  free(values);
  return result;
}

blame repository::blame_file(const std::string &path,
                             blame::options options) const {
  blame result;
  if (git_blame_file(&result.c_ptr_, c_ptr_, path.c_str(), options.c_ptr_))
    throw git_exception();
  return result;
}

oid repository::create_blob_from_buffer(const std::string &buffer) {
  oid result;
  if (git_blob_create_frombuffer(result.c_ptr(), c_ptr_, buffer.c_str(),
                                 buffer.size()))
    throw git_exception();
  return result;
}

oid repository::create_blob_from_disk(const std::string &path) {
  oid result;
  if (git_blob_create_fromdisk(result.c_ptr(), c_ptr_, path.c_str()))
    throw git_exception();
  return result;
}

oid repository::create_blob_from_workdir(const std::string &relative_path) {
  oid result;
  if (git_blob_create_fromworkdir(result.c_ptr(), c_ptr_,
                                  relative_path.c_str()))
    throw git_exception();
  return result;
}

blob repository::lookup_blob(const oid &id) const {
  blob result;
  if (git_blob_lookup(&result.c_ptr_, c_ptr_, id.c_ptr()))
    throw git_exception();
  return result;
}

blob repository::lookup_blob(const oid &id, size_t len) const {
  blob result;
  if (git_blob_lookup_prefix(&result.c_ptr_, c_ptr_, id.c_ptr(), len))
    throw git_exception();
  return result;
}

reference repository::create_branch(const std::string &branch_name,
                                    const commit &target, bool force) {
  reference result(nullptr, ownership::user);
  if (git_branch_create(&result.c_ptr_, c_ptr_, branch_name.c_str(),
                        target.c_ptr(), force))
    throw git_exception();
  return result;
}

reference repository::create_branch(const std::string &branch_name,
                                    const annotated_commit &commit,
                                    bool force) {
  reference result(nullptr, ownership::user);
  if (git_branch_create_from_annotated(
          &result.c_ptr_, c_ptr_, branch_name.c_str(), commit.c_ptr(), force))
    throw git_exception();
  return result;
}

void repository::delete_branch(const reference &ref) {
  if (git_branch_delete(ref.c_ptr_))
    throw git_exception();
}

void repository::delete_branch(const std::string &branch_name,
                               branch::branch_type branch_type) {
  delete_branch(lookup_branch(branch_name, branch_type));
}

bool repository::is_branch_checked_out(const reference &ref) const {
  auto ret = git_branch_is_checked_out(ref.c_ptr());
  if (ret == 1)
    return true;
  else if (ret == 0)
    return false;
  else
    throw git_exception();
}

bool repository::is_branch_checked_out(const std::string &branch_name,
                                       branch::branch_type branch_type) {
  return is_branch_checked_out(lookup_branch(branch_name, branch_type));
}

bool repository::is_head_pointing_to_branch(const reference &ref) const {
  // 1 if HEAD points at the branch, 0 if it isn't, or a negative value as an
  // error code.
  auto ret = git_branch_is_head(ref.c_ptr());
  if (ret == 1)
    return true;
  else if (ret == 0)
    return false;
  else
    throw git_exception();
}

bool repository::is_head_pointing_to_branch(const std::string &branch_name,
                                            branch::branch_type branch_type) {
  return is_head_pointing_to_branch(lookup_branch(branch_name, branch_type));
}

reference repository::rename_branch(const reference &ref,
                                    const std::string &new_branch_name,
                                    bool force) {
  reference result(nullptr, ownership::user);
  if (git_branch_move(&result.c_ptr_, ref.c_ptr_, new_branch_name.c_str(),
                      force))
    throw git_exception();
  return result;
}

reference repository::rename_branch(const std::string &branch_name,
                                    const std::string &new_branch_name,
                                    bool force,
                                    branch::branch_type branch_type) {
  auto ref = lookup_branch(branch_name, branch_type);
  return rename_branch(ref, new_branch_name, force);
}

std::string repository::branch_name(const reference &branch) const {
  const char *name;
  auto ret = git_branch_name(&name, branch.c_ptr());
  if (ret == 0) {
    if (name)
      return std::string(name);
    else
      return "";
  } else
    throw git_exception();
}

std::string repository::branch_remote_name(const std::string &refname) const {
  data_buffer result(1024);
  if (git_branch_remote_name(result.c_ptr(), c_ptr_, refname.c_str()))
    throw git_exception();
  return result.to_string();
}

void repository::set_branch_upstream(const reference &ref,
                                     const std::string &upstream_name) {
  if (git_branch_set_upstream(ref.c_ptr_, upstream_name.c_str()))
    throw git_exception();
}

void repository::set_branch_upstream(const std::string &branch_name,
                                     const std::string &upstream_name) {
  auto ref = lookup_branch(branch_name, branch::branch_type::local);
  return set_branch_upstream(ref, upstream_name);
}

void repository::unset_branch_upstream(const reference &ref) {
  if (git_branch_set_upstream(ref.c_ptr_, nullptr))
    throw git_exception();
}

void repository::unset_branch_upstream(const std::string &branch_name) {
  auto ref = lookup_branch(branch_name, branch::branch_type::local);
  return unset_branch_upstream(ref);
}

reference repository::branch_upstream(const reference &local_branch) const {
  reference result(nullptr, ownership::user);
  if (git_branch_upstream(&result.c_ptr_, local_branch.c_ptr()))
    throw git_exception();
  return result;
}

reference repository::branch_upstream(const std::string &local_branch_name) {
  auto ref = lookup_branch(local_branch_name, branch::branch_type::local);
  return branch_upstream(ref);
}

std::string repository::branch_upstream_name(const std::string &refname) const {
  data_buffer result(1024);
  if (git_branch_upstream_name(result.c_ptr(), c_ptr_, refname.c_str()))
    throw git_exception();
  return result.to_string();
}

std::string
repository::branch_upstream_remote(const std::string &refname) const {
  data_buffer result(1024);
  if (git_branch_upstream_remote(result.c_ptr(), c_ptr_, refname.c_str()))
    throw git_exception();
  return result.to_string();
}

reference repository::lookup_branch(const std::string &branch_name,
                                    branch::branch_type branch_type) {
  reference result(nullptr, ownership::user);
  if (git_branch_lookup(&result.c_ptr_, c_ptr_, branch_name.c_str(),
                        static_cast<git_branch_t>(branch_type)))
    throw git_exception();
  return result;
}

void repository::for_each_branch(std::function<void(const reference &)> visitor,
                                 branch::branch_type branch_type) {
  git_branch_t branch_type_c = static_cast<git_branch_t>(branch_type);
  git_branch_iterator *iter;
  git_branch_iterator_new(&iter, c_ptr_, branch_type_c);
  git_reference *ref_c;
  int ret;
  while ((ret = git_branch_next(&ref_c, &branch_type_c, iter)) == 0) {
    reference payload(ref_c);
    visitor(payload);
  }
  git_branch_iterator_free(iter);
}

void repository::checkout_head(const checkout::options &options) {
  // Note that this is not the correct mechanism used to switch branches; do not
  // change your HEAD and then call this method, that would leave you with
  // checkout conflicts since your working directory would then appear to be
  // dirty. Instead, checkout the target of the branch and then update HEAD
  // using repository.set_head to point to the branch you checked out.
  if (git_checkout_head(c_ptr_, options.c_ptr())) // options may be NULL
    throw git_exception();
}

void repository::checkout_index(const cppgit2::index &index,
                                const checkout::options &options) {
  if (git_checkout_index(c_ptr_, const_cast<git_index *>(index.c_ptr()),
                         options.c_ptr())) // index & options may be NULL
    throw git_exception();
}

void repository::checkout_tree(const object &treeish,
                               const checkout::options &options) {
  if (git_checkout_tree(c_ptr_, treeish.c_ptr(), options.c_ptr()))
    throw git_exception();
}

oid repository::create_commit(const std::string &update_ref, const signature &author, const signature &committer,
                  const std::string &message_encoding, const std::string &message, const tree &tree,
                  const std::vector<commit> &parents) {
  oid result;
  const char *update_ref_c = (update_ref == "") ? NULL : update_ref.c_str();
  const char *message_encoding_c = message_encoding == "" ? NULL : message_encoding.c_str();
  std::vector<const git_commit *> parents_c;
  for (auto &p : parents) {
    parents_c.push_back(p.c_ptr());
  }
  if (git_commit_create(result.c_ptr(), c_ptr_, update_ref_c, author.c_ptr(),
                               committer.c_ptr(), message_encoding_c, message.c_str(), tree.c_ptr(),
                               parents.size(), parents_c.data()))
    throw git_exception();
  return result;
}

data_buffer repository::create_commit(const signature &author, const signature &committer,
                          const std::string &message_encoding, const std::string &message,
                          const tree &tree, const std::vector<commit> &parents) {
  data_buffer result(1024);
  const char *message_encoding_c = message_encoding == "" ? NULL : message_encoding.c_str();
  std::vector<const git_commit *> parents_c;
  for (auto &p : parents) {
    parents_c.push_back(p.c_ptr());
  }
  if (git_commit_create_buffer(result.c_ptr(), c_ptr_, author.c_ptr(), committer.c_ptr(),
                                      message_encoding_c, message.c_str(), tree.c_ptr(),
                                      parents.size(), parents_c.data()))
    throw git_exception();
  return result;
}

oid repository::create_commit(const std::string &commit_content, const std::string &signature,
                  const std::string &signature_field) {
  oid result;
  const char *signature_c = signature == "" ? NULL : signature.c_str();
  const char *signature_field_c = signature_field == "" ? "" : signature_field.c_str();
  if (git_commit_create_with_signature(result.c_ptr(), c_ptr_, commit_content.c_str(),
                                              signature_c, signature_field_c))
    throw git_exception();
  return result;
}

std::pair<data_buffer, data_buffer> 
repository::extract_signature_from_commit(oid id, const std::string &signature_field) {
  data_buffer sig(1024), signed_data(1024);
  if (git_commit_extract_signature(sig.c_ptr(), signed_data.c_ptr(), c_ptr_, id.c_ptr(),
                                          signature_field.c_str()))
    throw git_exception();
  return {sig, signed_data};
}

commit repository::lookup_commit(const oid &id) {
  commit result;
  if (git_commit_lookup(&result.c_ptr_, c_ptr_, id.c_ptr()))
    throw git_exception();
  return result;
}

commit repository::lookup_commit(const oid &id, size_t length) {
  commit result;
  if (git_commit_lookup_prefix(&result.c_ptr_, c_ptr_, id.c_ptr(), length))
    throw git_exception();
  return result;
}

void repository::for_each_commit(std::function<void(const commit &id)> visitor,
                      revision::sort sort_ordering) {
  git_revwalk *iter;
  auto ret = git_revwalk_new(&iter, c_ptr_);
  git_revwalk_push_head(iter);

  if (ret != 0) {
    git_revwalk_free(iter);
    throw git_exception();
  }

  git_revwalk_sorting(iter, static_cast<unsigned int>(sort_ordering));
  git_oid id_c;
  while ((ret = git_revwalk_next(&id_c, iter)) == 0) {
    oid id(&id_c);
    visitor(lookup_commit(id));
  }
  git_revwalk_free(iter);
}

void repository::for_each_commit(std::function<void(const commit &id)> visitor, const commit &start_from,
                      revision::sort sort_ordering) {
  git_revwalk *iter;
  auto ret = git_revwalk_new(&iter, c_ptr_);
  git_revwalk_push(iter, start_from.id().c_ptr());

  if (ret != 0) {
    git_revwalk_free(iter);
    throw git_exception();
  }

  git_revwalk_sorting(iter, static_cast<unsigned int>(sort_ordering));
  git_oid id_c;
  while ((ret = git_revwalk_next(&id_c, iter)) == 0) {
    oid payload(&id_c);
    visitor(lookup_commit(payload));
  }
  git_revwalk_free(iter);
}

object repository::lookup_object(const oid& id, object::object_type type) const {
  object result;
  if (git_object_lookup(&result.c_ptr_, c_ptr_, id.c_ptr(), static_cast<git_object_t>(type)))
    throw git_exception();
  return result;
}

object repository::lookup_object(const oid& id, size_t length, object::object_type type) const {
  object result;
  if (git_object_lookup_prefix(&result.c_ptr_, c_ptr_, id.c_ptr(), length, static_cast<git_object_t>(type)))
    throw git_exception();
  return result;
}

object repository::lookup_object(const object& treeish, const std::string& path, object::object_type type) const {
  object result;
  if (git_object_lookup_bypath(&result.c_ptr_, treeish.c_ptr_, path.c_str(), static_cast<git_object_t>(type)))
    throw git_exception();
  return result;
}

object repository::tree_to_object(const tree::entry &entry) {
  object result;
  if (git_tree_entry_to_object(&result.c_ptr_, c_ptr_, entry.c_ptr()))
    throw git_exception();
  return result;
}

tree repository::lookup_tree(const oid &id) {
  tree result;
  if (git_tree_lookup(&result.c_ptr_, c_ptr_, id.c_ptr()))
    throw git_exception();
  return result;
}

tree repository::lookup_tree(const oid &id, size_t length) {
  tree result;
  if (git_tree_lookup_prefix(&result.c_ptr_, c_ptr_, id.c_ptr(), length))
    throw git_exception();
  return result;
}

} // namespace cppgit2