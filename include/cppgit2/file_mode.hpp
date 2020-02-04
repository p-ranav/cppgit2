#pragma once

namespace cppgit2 {

// Valid modes for index and tree entries.
enum class file_mode {
  unreadable = 0000000,
  tree = 0040000,
  blob = 0100644,
  blob_executable = 0100755,
  link = 0120000,
  commit = 0160000
};

} // namespace cppgit2
