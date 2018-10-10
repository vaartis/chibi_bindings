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

struct Testy {
    Testy(int x) : x(x) { }
    int x;

    sexp fnc(sexp context, sexp self, long n, sexp arg1) {

        return SEXP_NULL;
    }
};

TEST(ChibiTests, Struct) {
    Chibi chibi;

    std::shared_ptr<Testy> xv = std::make_shared<Testy>(10);

    auto f = +[](sexp ctx, sexp self, long n, sexp this_ptr, sexp arg1) {
                  Testy *testy_ptr = reinterpret_cast<Testy *>(sexp_cpointer_value(this_ptr));
                  int arg1_i = sexp_unbox_fixnum(arg1);
                  testy_ptr->x = arg1_i;

                  return SEXP_NULL;
              };

    sexp testy_type = sexp_register_c_type(chibi.context, chibi.make_string("testy-type"), nullptr);
    sexp xv_ptr = sexp_make_cpointer(chibi.context, sexp_type_tag(testy_type), xv.get(), chibi.context, 0);

    chibi.register_function("fnc", f);
    SExp ff = chibi.env_ref("fnc");
    ff.apply(xv_ptr, chibi.make_integer(11))->dump_to_port();

    std::cout << xv->x;
}
