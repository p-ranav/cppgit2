#include <cppgit2/pack_builder.hpp>

namespace cppgit2 {

pack_builder::pack_builder() : c_ptr_(nullptr), owner_(ownership::libgit2) {}

pack_builder::pack_builder(git_packbuilder *c_ptr, ownership owner)
    : c_ptr_(c_ptr), owner_(owner) {}

pack_builder::~pack_builder() {
  if (c_ptr_ && owner_ == ownership::user)
    git_packbuilder_free(c_ptr_);
}

void pack_builder::for_each_object(std::function<void(void *object_data, size_t object_size)> visitor) {
  struct visitor_wrapper {
    std::function<void(void *object_data, size_t object_size)> fn;
  };

  visitor_wrapper wrapper;
  wrapper.fn = visitor;

  auto callback_c = [](void *buffer, size_t size, void *payload) {
    auto wrapper = reinterpret_cast<visitor_wrapper *>(payload);
    wrapper->fn(buffer, size);
    return 0;
  };

  if (git_packbuilder_foreach(c_ptr_, callback_c, (void *)(&wrapper)))
    throw git_exception();
}

oid pack_builder::hash() {
  return oid(git_packbuilder_hash(c_ptr_));
}

oid pack_builder::id() const { return oid(git_packbuilder_hash(c_ptr_)); }

void pack_builder::insert_commit(const oid &commit_id) {
  if (git_packbuilder_insert_commit(c_ptr_, commit_id.c_ptr()))
    throw git_exception();
}

void pack_builder::insert_object(const oid &commit_id,
                                 const std::string &name) {
  auto name_c = name.empty() ? nullptr : name.c_str();
  if (git_packbuilder_insert(c_ptr_, commit_id.c_ptr(), name_c))
    throw git_exception();
}

void pack_builder::insert_object_recursively(const oid &commit_id,
                                             const std::string &name) {
  auto name_c = name.empty() ? nullptr : name.c_str();
  if (git_packbuilder_insert_recur(c_ptr_, commit_id.c_ptr(), name_c))
    throw git_exception();
}

void pack_builder::insert_tree(const oid &tree_id) {
  if (git_packbuilder_insert_tree(c_ptr_, tree_id.c_ptr()))
    throw git_exception();
}

void pack_builder::insert_revwalk(const revwalk &walk) {
  if (git_packbuilder_insert_walk(c_ptr_, walk.c_ptr_))
    throw git_exception();
}

size_t pack_builder::size() const {
  return git_packbuilder_object_count(c_ptr_);
}

void pack_builder::set_progress_callback(
    std::function<void(int, uint32_t, uint32_t)> callback) {
  // Wrap user provided visitor in a callback fn
  struct visitor_wrapper {
    std::function<void(int, uint32_t, uint32_t)> fn;
  };

  visitor_wrapper wrapper;
  wrapper.fn = callback;

  // Prepare callback function to pass to git_packbuilder_set_callbacks(...)
  auto callback_c = [](int stage, uint32_t current, uint32_t total,
                       void *payload) {
    auto wrapper = reinterpret_cast<visitor_wrapper *>(payload);
    wrapper->fn(stage, current, total);
    return 0;
  };

  if (git_packbuilder_set_callbacks(c_ptr_, callback_c, (void *)(&wrapper)))
    throw git_exception();
}

void pack_builder::set_threads(unsigned int num_threads) {
  if (git_packbuilder_set_threads(c_ptr_, num_threads))
    throw git_exception();
}

void pack_builder::write(const std::string &path, unsigned int mode, 
  std::function<void(const indexer::progress &)> &progress_callback) {
  
  struct visitor_wrapper {
    std::function<void(const indexer::progress &)> fn;
  };

  visitor_wrapper wrapper;
  wrapper.fn = progress_callback;

  auto callback_c = [](const git_indexer_progress *	stats, void *payload) {
    auto wrapper = reinterpret_cast<visitor_wrapper *>(payload);
    wrapper->fn(indexer::progress(stats));
    return 0;
  };

  if (git_packbuilder_write(c_ptr_, path.c_str(), mode, callback_c, (void *)(&wrapper)))
    throw git_exception();
}

data_buffer pack_builder::write_to_buffer() {
  data_buffer result;
  if (git_packbuilder_write_buf(result.c_ptr(), c_ptr_))
    throw git_exception();
  return result;
}

size_t pack_builder::written() const { return git_packbuilder_written(c_ptr_); }

const git_packbuilder *pack_builder::c_ptr() const { return c_ptr_; }

} // namespace cppgit2