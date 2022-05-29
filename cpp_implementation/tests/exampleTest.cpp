#include "catch2/catch_all.hpp"
#include <iostream>
TEST_CASE("helloWorld", "[helloWorld]" ) {
    std::cout << "Hello, World!" << std::endl;
    CHECK(1==1);
}