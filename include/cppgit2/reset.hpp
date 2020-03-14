#include <cppgit2/libgit2_api.hpp>

namespace cppgit2 {

class reset : public libgit2_api {
public:
  enum class reset_type {
    soft = 1,  // Move the head to the given commit
    mixed = 2, // SOFT plus reset index to the commit
    hard = 3   // MIXED plus changes in working tree discarded
  };
};

} // namespace cppgit2