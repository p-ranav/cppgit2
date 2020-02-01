#include <cppgit2/strarray.hpp>
#include <doctest.hpp>
using doctest::test_suite;
using namespace cppgit2;

TEST_CASE("Default construct strarray" * test_suite("strarray")) {
  strarray arr;
  REQUIRE(arr.count() == 0);
}

TEST_CASE("Construct strarray from vector of strings" *
          test_suite("strarray")) {
  strarray arr({"Hello", "World"});
  REQUIRE(arr.count() == 2);
  REQUIRE(arr[0] == "Hello");
  REQUIRE(arr[1] == "World");
}

TEST_CASE("Construct strarray from libgit2 C ptr" * test_suite("strarray")) {
  strarray foo({"Hello", "World"});
  strarray bar(foo.c_ptr());
  REQUIRE(bar.count() == 2);
  REQUIRE(bar[0] == "Hello");
  REQUIRE(bar[1] == "World");
}

TEST_CASE("Clone strarray using .copy()" * test_suite("strarray")) {
  strarray foo({"Hello", "World", "!"});
  strarray bar = foo.copy();
  REQUIRE(bar.count() == 3);
  REQUIRE(bar[0] == "Hello");
  REQUIRE(bar[1] == "World");
  REQUIRE(bar[2] == "!");
}

TEST_CASE("Get vector of strings from strarray" * test_suite("strarray")) {
  strarray foo({"This", "is", "an", "strarray", "!"});
  auto vec = foo.to_vector();
  REQUIRE(vec.size() == 5);
  REQUIRE(vec[0] == "This");
  REQUIRE(vec[1] == "is");
  REQUIRE(vec[2] == "an");
  REQUIRE(vec[3] == "strarray");
  REQUIRE(vec[4] == "!");
}
