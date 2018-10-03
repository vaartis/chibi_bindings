#include "gtest/gtest.h"

#include <Chibi.hpp>

TEST(SExpTests, DumpToPort) {
    Chibi chibi;

    SExp outputPort = chibi.make_SExp(sexp_open_output_string(chibi.context));

    SExp exp = chibi.eval_string("(error \"an error\")");
    exp.dump_to_port(outputPort);

    EXPECT_EQ(chibi.make_SExp(sexp_get_output_string(chibi.context, outputPort)).to<std::string>(), "ERROR: an error\n");

    SExp outputPort2 = chibi.make_SExp(sexp_open_output_string(chibi.context));
    SExp exp2 = chibi.eval_string("1");
    exp2.dump_to_port(outputPort2);
    EXPECT_EQ(chibi.make_SExp(sexp_get_output_string(chibi.context, outputPort2)).to<std::string>(), "1\n");
}

TEST(SExpTests, DumpToString) {
    Chibi chibi;

    SExp exp = chibi.eval_string("(error \"an error\")");


    EXPECT_EQ(exp.dump_to_string(), "ERROR: an error\n");

    SExp exp2 = chibi.eval_string("1");
    EXPECT_EQ(exp2.dump_to_string(), "1\n");
}

TEST(SExpTests, To) {
    Chibi chibi;

    std::string str = "1";
    SExp lst = chibi.eval_string(str);
    EXPECT_EQ(lst.to<sexp_sint_t>(), 1);

    str = "#t";
    SExp lst2 = chibi.eval_string(str);
    EXPECT_EQ(lst2.to<bool>(), true);

    str = "\"abc\"";
    SExp lst3 = chibi.eval_string(str);
    EXPECT_EQ(lst3.to<std::string>(), "abc");
}

TEST(SExpTests, ToVecOf) {
    Chibi chibi;

    std::string str = "#(1 2 3)";
    std::vector<sexp_sint_t> r {1, 2, 3};

    SExp lst = chibi.eval_string(str);

    EXPECT_EQ(lst.to_vec_of<sexp_sint_t>().value(), r);
}

TEST(SExpTests, Apply) {
    Chibi chibi;

    EXPECT_TRUE(
        chibi.env_ref("string?").apply(chibi.eval_string("\"a\""))->to<bool>()
    );

    EXPECT_EQ(
        chibi.env_ref("not-a-function").apply(),
        std::nullopt
   );
}
