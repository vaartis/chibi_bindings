#include "gtest/gtest.h"

#include "SExpConverters.hpp"

TEST(SExpConvertersTests, FromToSexp) {
    Chibi chibi;

    std::string str_val = "test";
    EXPECT_EQ(
        FromSExp<std::string>::from(
            chibi,
            ToSExp<std::string>::to(chibi, str_val)
        ),
        str_val
    );

    sexp_sint_t int_val = 10;
    EXPECT_EQ(
        FromSExp<sexp_sint_t>::from(
            chibi,
            ToSExp<sexp_sint_t>::to(chibi, int_val)
        ),
        int_val
    );

    bool bool_val = false;
    EXPECT_EQ(
        FromSExp<bool>::from(
            chibi,
            ToSExp<bool>::to(chibi, bool_val)
        ),
        bool_val
    );

    float float_val = 11.0;
    EXPECT_FLOAT_EQ(
        FromSExp<float>::from(
            chibi,
            ToSExp<float>::to(chibi, float_val)
        ).value(),
        float_val
    );

    int int_for_ptr = 12;
    EXPECT_EQ(
        FromSExp<int *>::from(
            chibi,
            ToSExp<int *>::to(chibi, &int_for_ptr)
        ),
        &int_for_ptr
    );

    std::string symbol_val = "symbol";
    EXPECT_EQ(
        FromSExp<Symbol>::from(
            chibi,
            ToSExp<Symbol>::to(chibi, symbol_val)
        ),
        Symbol(symbol_val)
    );

}
