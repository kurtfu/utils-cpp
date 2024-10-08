/*****************************************************************************/
/*** HEADER INCLUDES *********************************************************/

#include <catch2/catch_test_macros.hpp>

#include "either.hpp"

#include <stdexcept>
#include <string>

/*****************************************************************************/
/*** TEST CASES **************************************************************/

utils::either<int, std::string> good()
{
    return utils::success<int>(0);
}

utils::either<int, std::string> bad()
{
    return utils::fail<std::string>("bad");
}

TEST_CASE("Simple success assignment")
{
    auto result = good();

    REQUIRE(result.has_value() == true);
    REQUIRE_NOTHROW(result.value() == 0);
}

TEST_CASE("Simple fail assignment")
{
    auto result = bad();

    REQUIRE(result.has_value() == false);
    REQUIRE_NOTHROW(result.error() == "bad");
}

TEST_CASE("Bad value access")
{
    auto result = bad();

    REQUIRE_THROWS_AS(result.value(), std::logic_error);
}

TEST_CASE("Bad error access")
{
    auto result = good();

    REQUIRE_THROWS_AS(result.error(), std::logic_error);
}

TEST_CASE("Monadic 'and_then'")
{
    auto advance = [](int value)
    {
        return utils::either<int, std::string>(utils::success<int>(value + 1));
    };

    auto result = good().and_then(advance);

    REQUIRE(result.value() == 1);
}

TEST_CASE("Monadic 'or_else'")
{
    auto encode = [](const std::string& error)
    {
        std::string error_code = "Error: " + error;
        return utils::either<int, std::string>(utils::fail<std::string>(error_code));
    };

    auto result = bad().or_else(encode);

    REQUIRE(result.error() == "Error: bad");
}
