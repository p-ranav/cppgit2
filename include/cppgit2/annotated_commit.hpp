#include <cppgit2/libgit2_api.hpp>
#include <cppgit2/oid.hpp>
#include <cppgit2/ownership.hpp>
#include <git2.h>

namespace cppgit2 {

class annotated_commit : public libgit2_api {
public:
  // Default construct annotated commit
  annotated_commit();

  // Construct from libgit2 C ptr
  // If owned by user, will be free'd in the destructor
  annotated_commit(git_annotated_commit *c_ptr,
                   ownership owner = ownership::libgit2);

  // Cleanup
  ~annotated_commit();

  // Gets the commit ID that the given git_annotated_commit refers to.
  oid id() const;

  // Get the refname that the given git_annotated_commit refers to.
  std::string refname() const;

  const git_annotated_commit *c_ptr() const;

private:
  friend class repository;
  git_annotated_commit *c_ptr_;
  ownership owner_;
};

} // namespace cppgit2