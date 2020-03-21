#include <cppgit2/odb.hpp>
using namespace cppgit2;

odb::odb() : c_ptr_(nullptr), owner_(ownership::user) {
  git_odb_new(&c_ptr_); // owned by user
}

odb::odb(git_odb *c_ptr, ownership owner) : c_ptr_(c_ptr), owner_(owner) {}

odb::~odb() {
  if (c_ptr_ && owner_ == ownership::user)
    git_odb_free(c_ptr_);
}

bool odb::exists(const oid &id) const {
  return git_odb_exists(c_ptr_, id.c_ptr());
}

oid odb::exists(const oid &id, size_t length) const {
  oid result;
  if (git_odb_exists_prefix(result.c_ptr(), c_ptr_, id.c_ptr(), length))
    throw git_exception();
  return result;
}

void odb::for_each(std::function<void(const oid&)> visitor) {
  // Prepare wrapper to pass to C API
  struct visitor_wrapper {
    std::function<void(const oid&)> fn;
  };

  visitor_wrapper wrapper;
  wrapper.fn = visitor;

  auto callback_c = [](const git_oid *oid_c, void *payload) {
    auto wrapper = reinterpret_cast<visitor_wrapper *>(payload);
    if (wrapper->fn)
      wrapper->fn(oid(oid_c));
    return 0;
  };

  if (git_odb_foreach(c_ptr_, callback_c, (void *)(&wrapper)))
    throw git_exception();
}

odb::backend odb::operator[](size_t index) const {
  odb::backend result;
  if (git_odb_get_backend(&result.c_ptr_, c_ptr_, index))
    throw git_exception();
  return result;
}

size_t odb::size() const { return git_odb_num_backends(c_ptr_); }

odb::backend
odb::create_backend_for_loose_objects(const std::string &objects_dir,
                                      int compression_level, bool do_fsync,
                                      unsigned int dir_mode, file_mode mode) {
  odb::backend result;
  if (git_odb_backend_loose(&result.c_ptr_, objects_dir.c_str(),
                            compression_level, do_fsync, dir_mode,
                            static_cast<unsigned int>(mode)))
    throw git_exception();
  return result;
}

odb::backend
odb::create_backend_for_one_packfile(const std::string &index_file) {
  odb::backend result;
  if (git_odb_backend_one_pack(&result.c_ptr_, index_file.c_str()))
    throw git_exception();
  return result;
}

odb::backend odb::create_backend_for_packfiles(const std::string &objects_dir) {
  odb::backend result;
  if (git_odb_backend_pack(&result.c_ptr_, objects_dir.c_str()))
    throw git_exception();
  return result;
}

// Access libgit2 C ptr
const git_odb *odb::c_ptr() const { return c_ptr_; }