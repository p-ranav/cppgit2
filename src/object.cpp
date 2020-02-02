#include <cppgit2/object.hpp>
#include <cppgit2/data_buffer.hpp>

namespace cppgit2 {

object::object() : c_ptr_(nullptr) { git_libgit2_init(); }

object::object(const git_object *c_ptr) {
  git_libgit2_init();
  if (git_object_dup(&c_ptr_, const_cast<git_object *>(c_ptr)))
    throw exception();
}

object::~object() { git_libgit2_shutdown(); }

oid object::id() const { return oid(git_object_id(c_ptr_)->id); }

std::string object::short_id() const {
  git_buf result;
  if (git_object_short_id(&result, c_ptr_))
    throw exception();
  return data_buffer(&result).to_string();
}
  
object object::copy() const {
  object result;
  if (git_object_dup(&result.c_ptr_, c_ptr_))
    throw exception();
  return result;
}

object object::peel_until(object_type target_type) {
  object result;
  if (git_object_peel(&result.c_ptr_, c_ptr_,
                      static_cast<git_object_t>(target_type)))
    throw exception();
  return result;
}

object::object_type object::type() const {
  return static_cast<object_type>(git_object_type(c_ptr_));
}

std::string object::type_string() const {
  return object::string_from_type(this->type());
}

object::object_type object::type_from_string(const std::string &type_string) {
  return static_cast<object_type>(git_object_string2type(type_string.c_str()));
}

std::string object::string_from_type(object_type type) {
  return std::string(git_object_type2string(static_cast<git_object_t>(type)));
}

bool object::is_type_loose(object_type type) {
  return git_object_typeisloose(static_cast<git_object_t>(type));
}

} // namespace cppgit2
