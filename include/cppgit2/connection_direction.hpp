#pragma once

namespace cppgit2 {

// Direction of the connection
// We need this because we need to know whether
// we should call git-upload-pack or git-receive-pack
// on the remote end when get_refs gets called.
enum class connection_direction { fetch, push };

} // namespace cppgit2