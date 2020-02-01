#include <cppgit2/oid.hpp>

using namespace cppgit2;

oid::oid() {
  git_libgit2_init();
}

oid::oid(const std::string &hex_string) {
  git_libgit2_init();
  if (!git_oid_fromstr(&c_struct_, hex_string.c_str()))
    throw exception();
}
