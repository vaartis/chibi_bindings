#include "gtest/gtest.h"

#include "Chibi.hpp"

#include <iostream>
#include <memory>

TEST(ChibiTests, EvalString) {
    Chibi chibi;

    SExp one_sexp = chibi.eval_string("1");

    EXPECT_EQ(one_sexp.to<sexp_sint_t>(), 1);
}

TEST(ChibiTests, CallCFunction) {
    Chibi chibi;

    auto f = +[](sexp ctx, sexp self, long n, sexp arg) { return arg; };
    auto ff = chibi.register_function("f", f);
    SExp res = chibi.eval_string("(f 10)");
    SExp res2 = ff.apply(chibi.make_integer(11)).value();

    EXPECT_EQ(res.to<sexp_sint_t>(), 10);
    EXPECT_EQ(res2.to<sexp_sint_t>(), 11);
}

TEST(ChibiTests, MakeList) {
    Chibi chibi;

    SExp res = chibi.make_list(SEXP_ONE, SEXP_TWO, SEXP_THREE);

    SExp lst = chibi.eval_string("'(1 2 3)");
    SExp lst_wrong = chibi.eval_string("'(1 2 3 4)");

    EXPECT_EQ(res, lst);
    EXPECT_NE(res, lst_wrong);
}

TEST(ChibiTests, MakeString) {
    Chibi chibi;

    SExp res = chibi.make_string("test");
    EXPECT_EQ(res.to<std::string>(), "test");
}

TEST(ChibiTests, MakeInteger) {
    Chibi chibi;

    SExp res = chibi.make_integer(128);
    EXPECT_EQ(res.to<sexp_sint_t>(), 128);
}

TEST(ChibiTests, MakeFloat) {
    Chibi chibi;

    SExp res = chibi.make_float(10.1);
    EXPECT_FLOAT_EQ(*res.to<float>(), 10.1);
}

TEST(ChibiTests, EnvRef) {
    Chibi chibi;

    SExp doesnt = chibi.env_ref("does-not-exist");
    EXPECT_EQ(doesnt, SEXP_FALSE);

    SExp exists = chibi.env_ref("assoc");
    EXPECT_NE(exists, SEXP_FALSE);
}

TEST(ChibiTests, MakeCPointerAndToPtr) {
    Chibi chibi;

    auto p = std::make_shared<int>(10);
    auto p2 = std::make_shared<int>(11);
    auto p3 = std::make_shared<int>(11);

    SExp s_ptr = chibi.make_cpointer(p.get());
    SExp s2_ptr = chibi.make_cpointer(p2.get());

    sexp_tag_t objtype1 = sexp_type_tag(sexp_object_type(chibi.context, sexp(s_ptr)));
    sexp_tag_t objtype2 = sexp_type_tag(sexp_object_type(chibi.context, sexp(s2_ptr)));

    EXPECT_EQ(objtype1, objtype2);
    EXPECT_EQ(p.get(), s_ptr.to_ptr<int>());
    EXPECT_EQ(p2.get(), s2_ptr.to_ptr<int>());
}
