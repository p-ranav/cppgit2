#pragma once
#include <cppgit2/exception.hpp>
#include <git2.h>
#include <string>

namespace cppgit2 {

class attribute {
public:
  // Possible states for an attribute
  enum class attribute_value {
    unspecified = 0, // The attribute has been left unspecified
    true_,           // The attribute has been set
    false_,          // The attribute has been unset
    string           // This attribute has a valu
  };

  // Return the value type for a given attribute.
  // This can be either TRUE, FALSE, UNSPECIFIED (if the attribute was not set
  // at all), or VALUE, if the attribute was set to an actual string.
  static attribute_value value(const std::string &attr);
};

} // namespace cppgit2