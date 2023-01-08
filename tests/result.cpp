#include <catch2/catch_test_macros.hpp>
#include <result/result.h>

template <typename T, typename E>
void require_result_ok(result::_::result_base<T, E>& result) {
    REQUIRE(result.is_ok());
    REQUIRE_FALSE(result.is_err());
    REQUIRE_THROWS_AS(result.err(), std::runtime_error);
}

template <typename T, typename E>
void require_result_err(result::_::result_base<T, E>& result, const std::string& expected) {
    REQUIRE_FALSE(result.is_ok());
    REQUIRE(result.is_err());
    REQUIRE(result.err() == expected);
}

TEST_CASE("Result", "[result]") {
    SECTION("result::val") {
        SECTION("ok with value") {
            result::val<int> result = result::ok(42);
            require_result_ok(result);
            REQUIRE(result.get() == 42);
        }
        SECTION("ok with reference") {
            int value = 42;
            result::val<int> result = result::ok(value);
            require_result_ok(result);
            REQUIRE(result.get() == 42);
        }
        SECTION("ok with pointer") {
            int value = 42;
            result::val<int*> result = result::ok(&value);
            require_result_ok(result);
            REQUIRE(result.get() == &value);
            REQUIRE(*result.get() == 42);
        }
        SECTION("err") {
            result::val<int> result = result::err("error");
            require_result_err(result, "error");
            REQUIRE_THROWS_AS(result.get(), std::runtime_error);
        }
    }

    SECTION("result::ref") {
        SECTION("ok with reference") {
            int value = 42;
            result::ref<int> result = result::ok(value);
            require_result_ok(result);
            int& ref = result.get();
            REQUIRE(&ref == &value);
            REQUIRE(ref == 42);
            value = 43;
            REQUIRE(ref == 43);
        }
        // TODO: Won't compile
        //        SECTION("ok with pointer") {
        //            int value = 42;
        //            result::ref<int> result = result::ok(&value);
        //            require_result_ok(result);
        //            int* ref = result.get();
        //            REQUIRE(ref == &value);
        //            REQUIRE(*ref == 42);
        //            value = 43;
        //            REQUIRE(*ref == 43);
        //        }
        SECTION("err") {
            result::ref<int> result = result::err("error");
            require_result_err(result, "error");
            REQUIRE_THROWS_AS(result.get(), std::runtime_error);
        }
    }

    SECTION("result::ptr") {
        SECTION("ok with pointer") {
            result::ptr<int> result = result::ok(new int(42));
            require_result_ok(result);
            std::unique_ptr<int> ptr = result.get();
            REQUIRE(*ptr == 42);
        }
        SECTION("err") {
            result::ptr<int> result = result::err("error");
            require_result_err(result, "error");
            REQUIRE_THROWS_AS(result.get(), std::runtime_error);
        }
    }
}