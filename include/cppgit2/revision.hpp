#include <cppgit2/bitmask_operators.hpp>

namespace cppgit2 {

namespace revision {

// Flags to specify the sorting which a revwalk should perform.
// Used in e.g., repository.for_each_revision
enum class sort {
  // Sort the output with the same default method from `git`: reverse
  // chronological order. This is the default sorting for new walkers.
  none = 0,
  // Sort the repository contents in topological order (no parents before
  // all of its children are shown); this sorting mode can be combined
  // with time sorting to produce `git`'s `--date-order``.
  topological = 1 << 0,
  // Sort the repository contents by commit time;
  // this sorting mode can be combined with topological sorting
  commit_time = 1 << 1,
  // Iterate through the repository contents in reverse
  // order; this sorting mode can be combined with
  // any of the above.
  reverse = 1 << 2
};

} // namespace revision
ENABLE_BITMASK_OPERATORS(revision::sort);

} // namespace cppgit2