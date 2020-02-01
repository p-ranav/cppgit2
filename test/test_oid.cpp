#include <cppgit2/oid.hpp>
#include <doctest.hpp>
using doctest::test_suite;
using namespace cppgit2;

TEST_CASE("Construct oid from string" * test_suite("oid")) {
  oid oid("f9de917ac729414151fdce077d4098cfec9a45a5");
  REQUIRE(oid.to_hex_string() == "f9de917ac729414151fdce077d4098cfec9a45a5");
}

TEST_CASE("Construct oid from C ptr" * test_suite("oid")) {
  oid oid1("f9de917ac729414151fdce077d4098cfec9a45a5");
  oid oid2(oid1.c_ptr());
  REQUIRE(oid2.to_hex_string() == "f9de917ac729414151fdce077d4098cfec9a45a5");
}

TEST_CASE("Invalid oid construction" * test_suite("oid")) {
  REQUIRE_THROWS(oid("f9de90000&^$%_%#"));
}

TEST_CASE("Clone oid from existing oid struct" * test_suite("oid")) {
  oid id("f9de917ac729414151fdce077d4098cfec9a45a5");
  oid clone = id.copy();
  REQUIRE(clone.to_hex_string() == "f9de917ac729414151fdce077d4098cfec9a45a5");
}

TEST_CASE("Get path formatted string from oid" * test_suite("oid")) {
  oid id("f9de917ac729414151fdce077d4098cfec9a45a5");
  REQUIRE(id.to_path_string() == "f9/de917ac729414151fdce077d4098cfec9a45a5");
}

TEST_CASE("Construct oid from string with 5 characters" * test_suite("oid")) {
  oid oid("f9de917ac729414151fdce077d4098cfec9a45a5", 5);
  REQUIRE(oid.to_hex_string(5) == "f9de9");
}

TEST_CASE("to_hex_string() on oid constructed with 5 characters has 35 zeros" *
          test_suite("oid")) {
  oid oid("f9de917ac729414151fdce077d4098cfec9a45a5", 5);
  REQUIRE(oid.to_hex_string() == "f9de900000000000000000000000000000000000");
}

TEST_CASE("Check if oid is all zeros" * test_suite("oid")) {
  oid oid1("f9de917ac729414151fdce077d4098cfec9a45a5");
  REQUIRE(oid1.is_zero() == false);

  oid oid2("f9de917ac729414151fdce077d4098cfec9a45a5", 5);
  REQUIRE(oid2.is_zero() == false);

  oid oid3("00000", 3);
  REQUIRE(oid3.is_zero() == true);

  oid oid4("0000000000000000000000000000000000000000");
  REQUIRE(oid4.is_zero() == true);
}

TEST_CASE("Check if two oids are equal" * test_suite("oid")) {
  oid oid1("f9de917ac729414151fdce077d4098cfec9a45a5");
  oid oid2("f9de917ac729414151fdce077d4098cfec9a45a5");
  REQUIRE_FALSE(oid1 == "fa64cv");
  REQUIRE(oid1 == "f9de917ac729414151fdce077d4098cfec9a45a5");
  REQUIRE(oid1 == oid2);
  REQUIRE(oid2 == oid1);
}

TEST_CASE("Compare two oids" * test_suite("oid")) {
  oid oid1("f9de917ac729414151fdce077d4098cfec9a45a5");
  oid oid2("698b74a011ce48d2cae918129e8392c8987e0777");
  REQUIRE(oid1.compare(oid2) > 0);
  REQUIRE(oid2.compare(oid1) < 0);

  oid oid3("69de917ac729414151fdce077d4098cfec9a45a5");
  REQUIRE(oid2.compare(oid3) < 0);
  REQUIRE(oid2.compare(oid3, 2) == 0);

  oid oid4("f9de900000000000000000000000000000000000");
  REQUIRE(oid1.compare(oid4, 5) == 0);
}

TEST_CASE("Create an OID shortener and add oids to it" * test_suite("oid")) {
  oid::shortener shortener(1);
  REQUIRE(shortener.add("502ce33af489e7641a8e23171e71a775b1731fae") == 1);
  REQUIRE(shortener.add("503c2217a080538ffc3e47114405883f0c672964") == 41);
  REQUIRE(shortener.add("69de917ac729414151fdce077d4098cfec9a45a5") == 41);
}
