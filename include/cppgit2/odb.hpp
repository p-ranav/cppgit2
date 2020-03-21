#pragma once
#include <cppgit2/file_mode.hpp>
#include <cppgit2/git_exception.hpp>
#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/object.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/ownership.hpp>
#include <functional>
#include <git2.h>
#include <tuple>
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
    // Default construct an ODB backend
    backend() : c_ptr_(nullptr) {}

    // Construct from libgit2 C ptr
    backend(git_odb_backend *c_ptr) : c_ptr_(c_ptr) {}

    // Access libgit2 C ptr
    const git_odb_backend *c_ptr() const { return c_ptr_; }

  private:
    friend class odb;
    git_odb_backend *c_ptr_;
  };

  // Add a custom backend to an existing Object DB; this backend will work as an
  // alternate.
  //
  // Alternate backends are always checked for objects after all the main
  // backends have been exhausted. The backends are checked in relative
  // ordering, based on the value of the priority parameter. Writing is disabled
  // on alternate backends.
  void add_alternate_backend(const backend &backend, int priority);

  // Add a custom backend to an existing Object DB
  // The backends are checked in relative ordering, based on the value of the
  // priority parameter.
  void add_backend(const backend &backend, int priority);

  // Add an on-disk alternate to an existing Object DB.
  // Note that the added path must point to an objects, not to a full
  // repository, to use it as an alternate store. Alternate backends are always
  // checked for objects after all the main backends have been exhausted.
  // Writing is disabled on alternate backends.
  void add_disk_alternate_backend(const std::string &path);

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
    cppgit2::object::object_type type() const {
      return static_cast<cppgit2::object::object_type>(c_struct_.type);
    }
    void set_type(cppgit2::object::object_type type) {
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
  static oid hash(const void *data, size_t length,
                  cppgit2::object::object_type type);

  // Read a file from disk and fill a git_oid with the object id that the file
  // would have if it were written to the Object Database as an object of the
  // given type (w/o applying filters). Similar functionality to git.git's git
  // hash-object without the -w flag, however, with the --no-filters flag. If
  // you need filters, see git_repository_hashfile.
  static oid hash_file(const std::string &path,
                       cppgit2::object::object_type type);

  // Lookup an ODB backend object by index
  backend operator[](size_t index) const;

  // An object read from the ODB
  class object : public libgit2_api {
  public:
    // Construct from libgit2 C ptr
    object(git_odb_object *c_ptr) : c_ptr_(c_ptr) {}

    // Cleanup ODB object
    ~object() {
      if (c_ptr_)
        git_odb_object_free(c_ptr_);
    }

    // Create a copy of an odb_object
    object copy() const {
      object result(nullptr);
      if (git_odb_object_dup(&result.c_ptr_, c_ptr_))
        throw git_exception();
      return result;
    }

    // Return the data of an ODB object
    // This is the uncompressed, raw data as read from the ODB, without the
    // leading header. This pointer is owned by the object and shall not be
    // free'd.
    const void *data() const { return git_odb_object_data(c_ptr_); }

    // Return the OID of an ODB object
    // This is the OID from which the object was read from
    const oid id() const { return oid(git_odb_object_id(c_ptr_)); }

    // Return the size of an ODB object
    // This is the real size of the data buffer, not the actual size of the
    // object.
    size_t size() const { return git_odb_object_size(c_ptr_); }

    // Return the type of an ODB object
    cppgit2::object::object_type type() const {
      return static_cast<cppgit2::object::object_type>(
          git_odb_object_type(c_ptr_));
    }

    // Access libgit2 C ptr
    const git_odb_object *c_ptr() const { return c_ptr_; }

  private:
    friend odb;
    git_odb_object *c_ptr_;
  };

  // Read an object from the database.
  // This method queries all available ODB backends trying to read the given
  // OID.
  object read(const oid &id) const;

  // Read the header of an object from the database, without reading its full
  // contents. Returns {header_length, object_type} The header includes the
  // length and the type of an object. Note that most backends do not support
  // reading only the header of an object, so the whole object will be read and
  // then the header will be returned.
  std::pair<size_t, cppgit2::object::object_type>
  read_header(const oid &id) const;

  // Read an object from the database, given a prefix of its identifier.
  //
  // This method queries all available ODB backends trying to match the 'len'
  // first hexadecimal characters of the 'short_id'. The remaining
  // (GIT_OID_HEXSZ-len)*4 bits of 'short_id' must be 0s. 'len' must be at least
  // GIT_OID_MINPREFIXLEN, and the prefix must be long enough to identify a
  // unique object in all the backends; the method will fail otherwise.
  object read_prefix(const oid &id, size_t length) const;

  // Refresh the object database to load newly added files.
  //
  // If the object databases have changed on disk while the library is running,
  // this function will force a reload of the underlying indexes.
  //
  // Use this function when you're confident that an external application has
  // tampered with the ODB.
  //
  // NOTE that it is not necessary to call this function at all. The library
  // will automatically attempt to refresh the ODB when a lookup fails, to see
  // if the looked up object exists on disk but hasn't been loaded yet.
  void refresh();

  // Get the number of ODB backend objects
  size_t size() const;

  // A stream to read/write from the ODB
  class stream : public libgit2_api {
  public:
    // Construct from libgit2 C ptr
    stream(git_odb_stream *c_ptr) : c_ptr_(c_ptr) {}

    // Clean up stream
    ~stream() {
      if (c_ptr_)
        git_odb_stream_free(c_ptr_);
    }

    // Backend
    odb::backend backend() const { return c_ptr_->backend; }
    void set_backend(const odb::backend &backend) {
      c_ptr_->backend = backend.c_ptr_;
    }

    // Mode
    unsigned int mode() const { return c_ptr_->mode; }
    void set_mode(unsigned int value) { c_ptr_->mode = value; }

    // Hash context
    void *hash_ctx() const { return c_ptr_->hash_ctx; }
    void set_hash_ctx(void *ctx) { c_ptr_->hash_ctx = ctx; }

    // Declared size
    cppgit2::object::object_size declared_size() const {
      return c_ptr_->declared_size;
    }
    void set_declared_size(cppgit2::object::object_size value) {
      c_ptr_->declared_size = value;
    }

    // Received bytes
    cppgit2::object::object_size received_bytes() const {
      return c_ptr_->received_bytes;
    }
    void set_received_bytes(cppgit2::object::object_size value) {
      c_ptr_->received_bytes = value;
    }

    // Finish writing to an odb stream
    // The object will take its final name and will be available to the odb.
    // This method will fail if the total number of received bytes differs from
    // the size declared with git_odb_open_wstream()
    oid finalize_write() {
      oid result;
      if (git_odb_stream_finalize_write(result.c_ptr(), c_ptr_))
        throw git_exception();
      return result;
    }

    // Read from an odb stream
    // Most backends don't implement streaming reads
    void read(char *buffer, size_t length) {
      if (git_odb_stream_read(c_ptr_, buffer, length))
        throw git_exception();
    }

    // Write to an odb stream
    // This method will fail if the total number of received bytes exceeds the
    // size declared with git_odb_open_wstream()
    void write(const char *buffer, size_t length) {
      if (git_odb_stream_write(c_ptr_, buffer, length))
        throw git_exception();
    }

  private:
    friend odb;
    git_odb_stream *c_ptr_;
  };

  // Create a new object database and automatically add the two default
  // backends:
  static odb open(const std::string &objects_dir);

  // Open a stream to read an object from the ODB
  //
  // Note that most backends do not support streaming reads because they store
  // their objects as compressed/delta'ed blobs.
  //
  // It's recommended to use git_odb_read instead, which is assured to work on
  // all backends.
  //
  // Returns {stream, length of object, type of object}
  std::tuple<stream, size_t, cppgit2::object::object_type>
  open_rstream(const oid &id);

  // Open a stream to write an object into the ODB
  // The type and final length of the object must be specified when opening the
  // stream. The returned stream will be of type GIT_STREAM_WRONLY, and it won't
  // be effective until git_odb_stream_finalize_write is called and returns
  // without an error
  stream open_wstream(cppgit2::object::object_size size,
                      cppgit2::object::object_type type);

  // Write an object directly into the ODB
  oid write(const void *data, size_t length, cppgit2::object::object_type type);

  // Access libgit2 C ptr
  const git_odb *c_ptr() const;

private:
  friend class indexer;
  friend class repository;
  git_odb *c_ptr_;
  ownership owner_;
};

} // namespace cppgit2
