#include "gtest/gtest.h"

#include <Chibi.hpp>

#include <iostream>

TEST(ChibiTests, EvalString) {
    Chibi chibi;

    SExp one_sexp = chibi.eval_string("1");

    EXPECT_EQ(one_sexp.to<sexp_sint_t>(), 1);
}

TEST(ChibiTests, CallCFunction) {
    Chibi chibi;

    auto f = +[](sexp ctx, sexp self, long n, sexp arg) { return arg; };
    chibi.register_function("f", f);
    SExp res = chibi.eval_string("(f 10)");

    EXPECT_EQ(res.to<sexp_sint_t>(), 10);
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

TEST(ChibiTests, EnvRef) {
    Chibi chibi;

    SExp doesnt = chibi.env_ref("does-not-exist");
    EXPECT_EQ(doesnt, SEXP_FALSE);

    SExp exists = chibi.env_ref("assoc");
    EXPECT_NE(exists, SEXP_FALSE);
}

TEST(ChibiTests, Struct) {
    /*
    Chibi chibi;

    auto rs = chibi.eval_strings(
        "(import (srfi 99))",
        "(begin"
        " (define-record-type thing (make-thing a b) thing? (a thing-a) (b thing-b))"
        " (make-thing 1 2)"
        ")"
    );

    try {
        chibi.print_exception(rs[0]);
        chibi.print_exception(rs[1]);
    } catch (...) {}

    std::cout << chibi.sexp_to_string(rs[1]) << std::endl;
    */
}
