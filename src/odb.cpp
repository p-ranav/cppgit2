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

void odb::expand_ids(const std::vector<expand_id> &ids) {
  std::vector<git_odb_expand_id> ids_c;
  for (auto &id : ids)
    ids_c.push_back(id.c_struct_);

  if (git_odb_expand_ids(c_ptr_, ids_c.data(), ids.size()))
    throw git_exception();
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

void odb::for_each(std::function<void(const oid &)> visitor) {
  // Prepare wrapper to pass to C API
  struct visitor_wrapper {
    std::function<void(const oid &)> fn;
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

oid odb::hash(const void *data, size_t length,
              cppgit2::object::object_type type) {
  oid result;
  if (git_odb_hash(result.c_ptr(), data, length,
                   static_cast<git_object_t>(type)))
    throw git_exception();
  return result;
}

oid odb::hash_file(const std::string &path, cppgit2::object::object_type type) {
  oid result;
  if (git_odb_hashfile(result.c_ptr(), path.c_str(),
                       static_cast<git_object_t>(type)))
    throw git_exception();
  return result;
}

odb::backend odb::operator[](size_t index) const {
  odb::backend result;
  if (git_odb_get_backend(&result.c_ptr_, c_ptr_, index))
    throw git_exception();
  return result;
}

odb::object odb::read(const oid &id) const {
  odb::object result(nullptr);
  if (git_odb_read(&result.c_ptr_, c_ptr_, id.c_ptr()))
    throw git_exception();
  return result;
}

std::pair<size_t, cppgit2::object::object_type>
odb::read_header(const oid &id) const {
  size_t length_out;
  git_object_t object_type_out;
  if (git_odb_read_header(&length_out, &object_type_out, c_ptr_, id.c_ptr()))
    throw git_exception();
  return {length_out,
          static_cast<cppgit2::object::object_type>(object_type_out)};
}

odb::object odb::read_prefix(const oid &id, size_t length) const {
  odb::object result(nullptr);
  if (git_odb_read_prefix(&result.c_ptr_, c_ptr_, id.c_ptr(), length))
    throw git_exception();
  return result;
}

void odb::refresh() {
  if (git_odb_refresh(c_ptr_))
    throw git_exception();
}

size_t odb::size() const { return git_odb_num_backends(c_ptr_); }

odb odb::open(const std::string &objects_dir) {
  odb result(nullptr, ownership::user);
  if (git_odb_open(&result.c_ptr_, objects_dir.c_str()))
    throw git_exception();
  return result;
}

std::tuple<odb::stream, size_t, cppgit2::object::object_type> odb::open_rstream(const oid &id) {
  stream result(nullptr);
  size_t length;
  git_object_t type;
  if (git_odb_open_rstream(&result.c_ptr_, &length, &type, c_ptr_, id.c_ptr()))
    throw git_exception();
  return {result, length, static_cast<cppgit2::object::object_type>(type)};
}

odb::stream odb::open_wstream(cppgit2::object::object_size size,
                            cppgit2::object::object_type type) {
  stream result(nullptr);
  if (git_odb_open_wstream(&result.c_ptr_, c_ptr_, static_cast<git_object_size_t>(size), 
    static_cast<git_object_t>(type)))
    throw git_exception();
  return result;
}

// Access libgit2 C ptr
const git_odb *odb::c_ptr() const { return c_ptr_; }