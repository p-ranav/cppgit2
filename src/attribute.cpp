#include <cppgit2/attribute.hpp>

namespace cppgit2 {

attribute::attribute_value value(const std::string &attr) {
  auto attr_c = attr.empty() ? nullptr : attr.c_str();
  return static_cast<attribute::attribute_value>(git_attr_value(attr_c));
}

} // namespace cppgit2