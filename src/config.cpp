#include <cppgit2/config.hpp>

namespace cppgit2 {

  config::config() : c_ptr_(nullptr), owner_(ownership::libgit2) {
    git_libgit2_init();
  }

  config::config(git_config * c_ptr, ownership owner) :
    c_ptr_(c_ptr), owner_(owner) {
    git_libgit2_init();
  }

  config::~config() {
    if (c_ptr_ && owner_ == ownership::user)
      git_config_free(c_ptr_);
    git_libgit2_shutdown();
  }

  void config::delete_entry(const std::string &name) {
    if (git_config_delete_entry(c_ptr_, name.c_str()))
      throw exception();
  }
  
  void config::delete_entry(const std::string &name,
			    const std::string &regexp) {
    if (git_config_delete_multivar(c_ptr_, name.c_str(),
				   regexp.c_str()))
      throw exception();
  }

  std::string config::locate_global_config() {
    data_buffer result(nullptr);
    if (git_config_find_global(result.c_ptr()))
      throw exception();
    return result.to_string();
  }

  std::string config::locate_global_config_in_programdata() {
    data_buffer result(nullptr);
    if (git_config_find_programdata(result.c_ptr()))
      throw exception();
    return result.to_string();
  }

  std::string config::locate_global_system_config() {
    data_buffer result(nullptr);
    if (git_config_find_system(result.c_ptr()))
      throw exception();
    return result.to_string();
  }

  std::string config::locate_global_xdg_compatible_config() {
    data_buffer result(nullptr);
    if (git_config_find_xdg(result.c_ptr()))
      throw exception();
    return result.to_string();
  }

  config config::open_default_config() {
    config result;
    if (git_config_open_default(&result.c_ptr_))
      throw exception();
    return result;
  }

  config config::open_global_config(config &conf) {
    config result;
    if (git_config_open_global(&result.c_ptr_, conf.c_ptr_))
      throw exception();
    return result;
  }

  config config::open_config_at_level(const config &parent,
				      priority_level level) {
    config result;
    if (git_config_open_level(&result.c_ptr_,
			      parent.c_ptr(),
			      static_cast<git_config_level_t>(level)))
      throw exception();
    return result;
  }

  config::entry config::operator[](const std::string &name) {
    config::entry result;
    if (git_config_get_entry(&result.c_ptr_, c_ptr_, name.c_str()))
      throw exception();
    return result;
  }

  bool config::value_as_bool(const std::string &name) {
    int result;
    if (git_config_get_bool(&result, c_ptr_, name.c_str()))
      throw exception();
    return result;
  }

  int32_t config::value_as_int32(const std::string &name) {
    int32_t result;
    if (git_config_get_int32(&result, c_ptr_, name.c_str()))
      throw exception();
    return result;
  }

  int64_t config::value_as_int64(const std::string &name) {
    int64_t result;
    if (git_config_get_int64(&result, c_ptr_, name.c_str()))
      throw exception();
    return result;
  }

  std::string config::value_as_string(const std::string &name) {
    const char *result;
    auto ret = git_config_get_string(&result, c_ptr_, name.c_str());
    if (ret) {
      if (result)
	return std::string(result);
      else
	return "";
    } else
      throw exception();
  }

  data_buffer config::value_as_data_buffer(const std::string &name) {
    data_buffer result(nullptr);
    if (git_config_get_string_buf(result.c_ptr(), c_ptr_,
				  name.c_str()))
      throw exception();
    return result;
  }

  std::string config::path(const std::string &name) {
    data_buffer result(nullptr);
    if (git_config_get_path(result.c_ptr(), c_ptr_, name.c_str()))
      throw exception();
    return result.to_string();
  }

  void config::insert_entry(const std::string &name, bool value) {
    if (git_config_set_bool(c_ptr_, name.c_str(), value))
      throw exception();
  }

  void config::insert_entry(const std::string &name, int32_t value) {
    if (git_config_set_int32(c_ptr_, name.c_str(), value))
      throw exception();
  }

  void config::insert_entry(const std::string &name, int64_t value) {
    if (git_config_set_int64(c_ptr_, name.c_str(), value))
      throw exception();
  }

  void config::insert_entry(const std::string &name,
			    const std::string &value) {
    if (git_config_set_string(c_ptr_, name.c_str(), value.c_str()))
      throw exception();
  }

  void config::insert_multiple(const std::string &name,
			       const std::string &regexp,
			       const std::string &new_value) {
    if (git_config_set_multivar(c_ptr_, name.c_str(), regexp.c_str(),
				new_value.c_str()))
      throw exception();
  }

  transaction config::lock() {
    transaction result;
    git_transaction * result_ptr = result.c_ptr();
    if (git_config_lock(&result_ptr, c_ptr_))
      throw exception();
    return result;
  }
  
  bool config::parse_as_bool(const std::string &value) {
    int result;
    if (git_config_parse_bool(&result, value.c_str()))
      throw exception();
    return result;
  }

  int32_t config::parse_as_int32(const std::string &value) {
    int32_t result;
    if (git_config_parse_int32(&result, value.c_str()))
      throw exception();
    return result;
  }

  int64_t config::parse_as_int64(const std::string &value) {
    int64_t result;
    if (git_config_parse_int64(&result, value.c_str()))
      throw exception();
    return result;
  }

  std::string config::parse_path(const std::string &value) {
    data_buffer result(nullptr);
    if (git_config_parse_path(result.c_ptr(), value.c_str()))
      throw exception();
    return result.to_string();
  }

  config config::snapshot() {
    config result;
    if (git_config_snapshot(&result.c_ptr_, c_ptr_))
      throw exception();
    return result;
  }

  size_t config::size() const {
    size_t result{0};
    git_config_iterator *iter;
    git_config_iterator_new(&iter, c_ptr_);
    git_config_entry *entry;
    int ret;
    while ((ret = git_config_next(&entry, iter)) == 0)
      result++;
    git_config_iterator_free(iter);
    return result;
  }

  void config::for_each(std::function<void(const entry &)> visitor) {
    git_config_iterator *iter;
    git_config_iterator_new(&iter, c_ptr_);
    git_config_entry *entry_c;
    int ret;
    while ((ret = git_config_next(&entry_c, iter)) == 0) {
      entry payload(entry_c);
      visitor(payload);
    }
    git_config_iterator_free(iter);
  }

  const git_config *config::c_ptr() const { return c_ptr_; }
  
  
}
