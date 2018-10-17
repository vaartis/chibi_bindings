#include "gtest/gtest.h"

#include "Chibi.hpp"
#include "SExpConverters.hpp"

struct Testy {
    explicit Testy(int x = 21) : x(x) { }

    // Function
    std::string fnc(sexp_sint_t x, std::string y) { return std::to_string(x) + " " + y; }

    // Field
    sexp_sint_t x;
};

struct ClassRegistratorTests : public testing::Test {
    Chibi chibi;
    Chibi::ClassRegistrator<Testy> registrator = chibi.register_class<Testy>("testy");
};

TEST_F(ClassRegistratorTests, Method) {
    auto testy_ptr = std::make_shared<Testy>();
    auto testy_ptr_sexp = chibi.make_cpointer(testy_ptr.get());

    registrator.register_method("fnc", &Testy::fnc);
    auto fnc_to_call = chibi.env_ref("testy-fnc");
    EXPECT_EQ(
        FromSExp<std::string>::from(
            chibi,
            *fnc_to_call.apply(testy_ptr_sexp, chibi.make_integer(10), chibi.make_string("abc"))
        ),
        "10 abc"
    );
}

TEST_F(ClassRegistratorTests, Field) {
    auto testy_ptr = std::make_shared<Testy>();
    auto testy_ptr_sexp = chibi.make_cpointer(testy_ptr.get());

    registrator.register_field("x", &Testy::x, true);
    auto field_getter = chibi.env_ref("testy-x");
    auto field_setter = chibi.env_ref("set-testy-x!");

    EXPECT_EQ(
        FromSExp<sexp_sint_t>::from(chibi, *field_getter.apply(testy_ptr_sexp)),
        21
    );
    field_setter.apply(testy_ptr_sexp, chibi.make_integer(23));
    EXPECT_EQ(
        FromSExp<sexp_sint_t>::from(chibi, *field_getter.apply(testy_ptr_sexp)),
        23
    );
}

TEST_F(ClassRegistratorTests, Constructor) {
    registrator
        .register_default_constructor() // With a default argument
        .register_constructor<sexp_sint_t>("with-int"); // Without a default argument

    auto constructor = chibi.env_ref("make-testy");
    auto constructor_with_int = chibi.env_ref("make-testy-with-int");

    Testy *testy_constructed = *FromSExp<Testy *>::from(chibi, *constructor.apply());
    EXPECT_EQ(
        testy_constructed->x,
        21
    );

    Testy *testy_with_int_constructed = *FromSExp<Testy *>::from(chibi, *constructor_with_int.apply(chibi.make_integer(211)));
    EXPECT_EQ(
        testy_with_int_constructed->x,
        211
    );
}
