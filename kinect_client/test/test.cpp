#define CATCH_CONFIG_MAIN
#include "catch.hpp"

TEST_CASE("example test", "[example tag (optional)]") {
  int x = 4; x++;

  REQUIRE(x == 5);
}
