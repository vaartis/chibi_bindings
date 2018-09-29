#include "gtest/gtest.h"

#include <Chibi.hpp>

TEST(SExpTests, Dump) {
    Chibi chibi;

    SExp outputPort = chibi.make_SExp(sexp_open_output_string(chibi.context));

    SExp exp = chibi.eval_string("(error \"an error\")");
    exp.dump(outputPort);

    EXPECT_EQ(chibi.make_SExp(sexp_get_output_string(chibi.context, outputPort)).to<std::string>(), "ERROR: an error\n");

    SExp outputPort2 = chibi.make_SExp(sexp_open_output_string(chibi.context));
    SExp exp2 = chibi.eval_string("1");
    exp2.dump(outputPort2);
    EXPECT_EQ(chibi.make_SExp(sexp_get_output_string(chibi.context, outputPort2)).to<std::string>(), "1\n");
}

TEST(SExpTests, To) {
    Chibi chibi;

    std::string str = "1";
    SExp lst = chibi.eval_string(str);
    EXPECT_EQ(lst.to<sexp_sint_t>(), 1);

    str = "#t";
    lst = chibi.eval_string(str);
    EXPECT_EQ(lst.to<bool>(), true);

    str = "\"abc\"";
    lst = chibi.eval_string(str);
    EXPECT_EQ(lst.to<std::string>(), "abc");
}
