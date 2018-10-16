#include "gtest/gtest.h"

#include "Chibi.hpp"
#include "SExpConverters.hpp"

TEST(SExpTests, DumpToPort) {
    Chibi chibi;

    SExp outputPort = chibi.make_SExp(sexp_open_output_string(chibi.context));

    SExp exp = chibi.eval_string("(error \"an error\")");
    exp.dump_to_port(outputPort);

    EXPECT_EQ(
        FromSExp<std::string>::from(chibi, sexp_get_output_string(chibi.context, outputPort)),
        "ERROR: an error\n"
    );

    SExp outputPort2 = chibi.make_SExp(sexp_open_output_string(chibi.context));
    SExp exp2 = chibi.eval_string("1");
    exp2.dump_to_port(outputPort2);
    EXPECT_EQ(
        FromSExp<std::string>::from(chibi, sexp_get_output_string(chibi.context, outputPort2)),
        "1\n"
    );
}

TEST(SExpTests, DumpToString) {
    Chibi chibi;

    SExp exp = chibi.eval_string("(error \"an error\")");


    EXPECT_EQ(exp.dump_to_string(), "ERROR: an error\n");

    SExp exp2 = chibi.eval_string("1");
    EXPECT_EQ(exp2.dump_to_string(), "1\n");
}

TEST(SExpTests, ForEach) {
    Chibi chibi;

    std::string str = "#(1 2 3)";
    std::string str_l = "'(1 2 3)";
    int x1 = 0, x2 = 0;

    SExp vc = chibi.eval_string(str);
    SExp ls = chibi.eval_string(str_l);

    vc.for_each([&x1, &chibi](auto exp) {
                    x1 += *FromSExp<sexp_sint_t>::from(chibi, exp);
                }
    );

    ls.for_each([&x2, &chibi](auto exp) {
                    x2 += *FromSExp<sexp_sint_t>::from(chibi, exp);
                }
    );

    EXPECT_EQ(x1, x2);
    EXPECT_EQ(x1, 6);
}


TEST(SExpTests, Apply) {
    Chibi chibi;

    EXPECT_TRUE(
        FromSExp<bool>::from(chibi, *chibi.env_ref("string?").apply(chibi.eval_string("\"a\"")))
    );

    EXPECT_EQ(
        chibi.env_ref("not-a-function").apply(),
        std::nullopt
   );
}
