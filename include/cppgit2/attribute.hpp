#pragma once
#include <cppgit2/bitmask_operators.hpp>
#include <cppgit2/git_exception.hpp>
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

  enum class flag {
    // Reading values from index and working directory.
    //
    // When checking attributes, it is possible to check attribute files
    // in both the working directory (if there is one) and the index (if
    // there is one).  You can explicitly choose where to check and in
    // which order using the following flags.
    //
    // Core git usually checks the working directory then the index,
    // except during a checkout when it checks the index first.  It will
    // use index only for creating archives or for a bare repo (if an
    // index has been specified for the bare repo).
    file_then_index = 0,
    index_then_file = 1,
    index_only = 2,
    // Controlling extended attribute behavior.
    //
    // Normally, attribute checks include looking in the /etc (or system
    // equivalent) directory for a `gitattributes` file.  Passing this
    // flag will cause attribute checks to ignore that file.
    // equivalent) directory for a `gitattributes` file.  Passing the
    // `GIT_ATTR_CHECK_NO_SYSTEM` flag will cause attribute checks to
    // ignore that file.
    //
    // Passing the `GIT_ATTR_CHECK_INCLUDE_HEAD` flag will use attributes
    // from a `.gitattributes` file in the repository at the HEAD revision.
    no_system = (1 << 2),
    include_head = (1 << 3)
  };
};
ENABLE_BITMASK_OPERATORS(attribute::flag);

} // namespace cppgit2