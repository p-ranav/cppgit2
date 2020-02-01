#include <cppgit2/data_buffer.hpp>
#include <doctest.hpp>
using doctest::test_suite;
using namespace cppgit2;

TEST_CASE("Construct data buffer of size `n`" * test_suite("data_buffer")) {
  data_buffer foo(5); // 5 characters
  foo.set_buffer("ABCDE");
  REQUIRE(foo.to_string() == "ABCDE");
}

TEST_CASE("Construct data buffer from C ptr" * test_suite("data_buffer")) {
  data_buffer foo(5); // 5 characters
  foo.set_buffer("ABCDE");
  data_buffer bar(foo.c_ptr());
  REQUIRE(bar.to_string() == "ABCDE");
}

TEST_CASE("Grow a data buffer to size `n`" * test_suite("data_buffer")) {
  data_buffer foo(5); // 5 characters
  foo.set_buffer("ABCDE");
  foo.grow_to_size(10);
  foo.set_buffer("ABCDEFGHIJ");
  REQUIRE(foo.to_string() == "ABCDEFGHIJ");
}
