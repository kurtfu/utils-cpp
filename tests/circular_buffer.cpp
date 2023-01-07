/*****************************************************************************/
/*  HEADER INCLUDES                                                          */
/*****************************************************************************/

#include <catch2/catch_test_macros.hpp>

#include "circular_buffer.hpp"

/*****************************************************************************/
/*  TEST CASES                                                               */
/*****************************************************************************/

TEST_CASE("Simple push")
{
    circular_buffer<int, 2> cbuf{};

    cbuf.push(1);

    REQUIRE(cbuf.size() == 1);
    REQUIRE(cbuf.front() == 1);
}

TEST_CASE("Overflow")
{
    circular_buffer<int, 2> cbuf{};

    int value = 0;

    for (std::size_t i = 0; i < cbuf.size() + 1; ++i)
    {
        cbuf.push(value++);
    }

    int expected = 1;

    while (!cbuf.empty())
    {
        REQUIRE(cbuf.front() == expected++);
        cbuf.pop();
    }
}
