#pragma once
#include <cppgit2/file_mode.hpp>
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/object.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/ownership.hpp>
#include <git2.h>
#include <utility>
#include <vector>

namespace cppgit2 {

class odb : public libgit2_api {
public:
  // Default construct an odb object
  // Owned by user. Calls git_odb_new
  odb();

  // Construct from libgit2 C ptr
  odb(git_odb *c_ptr, ownership owner = ownership::libgit2);

  // Cleanup odb object if owned by user
  ~odb();

  class backend : public libgit2_api {
  public:
    backend() : c_ptr_(nullptr) {}
    backend(git_odb_backend *c_ptr) : c_ptr_(c_ptr) {}

  private:
    friend class odb;
    git_odb_backend *c_ptr_;
  };

  // The information about object IDs to query in `git_odb_expand_ids`, which
  // will be populated upon return.
  class expand_id : public libgit2_api {
  public:
    // Construct from libgit2 C ptr
    expand_id(const git_odb_expand_id *c_ptr) : c_struct_(*c_ptr) {}

    // The object ID to expand
    oid id() const { return oid(&c_struct_.id); }
    void set_id(const oid &id) { c_struct_.id = *(id.c_ptr()); }

    // The length of the object ID (in nibbles, or packets of 4 bits; the number
    // of hex characters)
    unsigned short length() const { return c_struct_.length; }
    void set_length(unsigned short value) { c_struct_.length = value; }

    // The (optional) type of the object to search for; leave as `0` or set to
    // `GIT_OBJECT_ANY` to query for any object matching the ID.
    object::object_type type() const {
      return static_cast<object::object_type>(c_struct_.type);
    }
    void set_type(object::object_type type) {
      c_struct_.type = static_cast<git_object_t>(type);
    }

    // Access libgit2 C ptr
    const git_odb_expand_id *c_ptr() const { return &c_struct_; }

  private:
    friend odb;
    git_odb_expand_id c_struct_;
  };

  // Determine if one or more objects can be found in the object database by
  // their abbreviated object ID and type. The given array will be updated in
  // place: for each abbreviated ID that is unique in the database, and of the
  // given type (if specified), the full object ID, object ID length
  // (GIT_OID_HEXSZ) and type will be written back to the array. For IDs that
  // are not found (or are ambiguous), the array entry will be zeroed.
  void expand_ids(const std::vector<expand_id> &ids);

  // Determine if the given object can be found in the object database.
  bool exists(const oid &id) const;

  // Determine if an object can be found in the object database by an
  // abbreviated object ID. If true, a valid OID is returned
  oid exists(const oid &id, size_t length) const;

  // List all objects available in the database
  // The callback will be called for each object available in the database. Note
  // that the objects are likely to be returned in the index order, which would
  // make accessing the objects in that order inefficient.
  void for_each(std::function<void(const oid &)> visitor);

  // Determine the object-ID (sha1 hash) of a data buffer
  // The resulting SHA-1 OID will be the identifier for the data buffer as if
  // the data buffer it were to written to the ODB.
  static oid hash(const void *data, size_t length, object::object_type type);

  // Read a file from disk and fill a git_oid with the object id that the file
  // would have if it were written to the Object Database as an object of the
  // given type (w/o applying filters). Similar functionality to git.git's git
  // hash-object without the -w flag, however, with the --no-filters flag. If
  // you need filters, see git_repository_hashfile.
  static oid hash_file(const std::string &path, object::object_type type);

  // Lookup an ODB backend object by index
  backend operator[](size_t index) const;

  // Get the number of ODB backend objects
  size_t size() const;

  // Create a backend for loose objects
  static backend
  create_backend_for_loose_objects(const std::string &objects_dir,
                                   int compression_level, bool do_fsync,
                                   unsigned int dir_mode, file_mode mode);

  // Create a backend out of a single packfile
  // index_file is the path to the packfile's .idx file
  static backend create_backend_for_one_packfile(const std::string &index_file);

  // Create a backend for the packfiles.
  // objects_dir is the Git repository's objects directory
  static backend create_backend_for_packfiles(const std::string &objects_dir);

  // Access libgit2 C ptr
  const git_odb *c_ptr() const;

private:
  friend class indexer;
  friend class repository;
  git_odb *c_ptr_;
  ownership owner_;
};

} // namespace cppgit2
