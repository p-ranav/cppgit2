#include <cppgit2/signature.hpp>
#include <doctest.hpp>
using doctest::test_suite;
using namespace cppgit2;

TEST_CASE("Default, empty signature" * test_suite("signature")) {
  signature foo;
  REQUIRE(foo.name() == "");
  REQUIRE(foo.email() == "");
  REQUIRE(foo.time() == 0);
  REQUIRE(foo.offset() == 0);
}

TEST_CASE("Construct signature with name and email" * test_suite("signature")) {
  signature foo("Foo Bar", "foo.bar@baz.com");
  REQUIRE(foo.name() == "Foo Bar");
  REQUIRE(foo.email() == "foo.bar@baz.com");
}

TEST_CASE("Construct signature with name, email, time and offset" *
          test_suite("signature")) {
  signature foo("Foo Bar", "foo.bar@baz.com", 50, 3);
  REQUIRE(foo.name() == "Foo Bar");
  REQUIRE(foo.email() == "foo.bar@baz.com");
  REQUIRE(foo.time() == 50);
  REQUIRE(foo.offset() == 3);
}

TEST_CASE("Construct signature from formatted buffer" *
          test_suite("signature")) {
  std::string buffer{"Foo Bar <foo.bar@baz.com> 1580583850756 0600"};
  signature foo(buffer);
  REQUIRE(foo.name() == "Foo Bar");
  REQUIRE(foo.email() == "foo.bar@baz.com");
  REQUIRE(foo.time() == 1580583850756);
}

TEST_CASE("Construct signature from ptr to libgit2 C-struct" *
          test_suite("signature")) {
  signature foo("Foo Bar", "foo.bar@baz.com", 50, 3);
  signature bar(foo.c_ptr());
  REQUIRE(bar.name() == "Foo Bar");
  REQUIRE(bar.email() == "foo.bar@baz.com");
  REQUIRE(bar.time() == 50);
  REQUIRE(bar.offset() == 3);
}

TEST_CASE("Clone signature struct" * test_suite("signature")) {
  signature foo("Foo Bar", "foo.bar@baz.com", 50, 3);
  auto bar = foo.copy();
  REQUIRE(bar.name() == "Foo Bar");
  REQUIRE(bar.email() == "foo.bar@baz.com");
  REQUIRE(bar.time() == 50);
  REQUIRE(bar.offset() == 3);
}
