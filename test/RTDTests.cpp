#include "gtest/gtest.h"

#include "Chibi.hpp"
#include "RecordTypeDescriptor.hpp"

#include <iostream>

TEST(RTDTests, AllFields) {
    Chibi chibi;

    chibi.eval_strings(
        "(import (srfi 99))",
        "(define-record-type a-type #t #t a)",
        "(define-record-type (b-type a-type) #t #t b)"
    );

    RecordTypeDescriptor rtd(chibi, "b-type");

    std::vector<Symbol> flds { "b" };
    std::vector<Symbol> flds_w_parent = { "a", "b" };

    EXPECT_EQ(rtd.fields(false), flds);
    EXPECT_EQ(rtd.fields(true), flds_w_parent);
}

TEST(RTDTests, GetSetField) {
    Chibi chibi;

    chibi.eval_strings(
        "(import (srfi 99))",
        "(define-record-type a-type #t #t a)",
        "(define-record-type (b-type a-type) #t #t b)",
        "(define a-b-value (make-b-type 1 2))"
   );

    RecordTypeDescriptor rtd(chibi, "b-type");
    SExp b_value = chibi.env_ref("a-b-value");

    EXPECT_EQ(rtd.get_field_from(b_value, "a")->to<sexp_sint_t>().value(), 1);

    auto sffr = rtd.set_field_for(b_value, "a", chibi.make_integer(3));
    EXPECT_EQ(sffr, RecordTypeDescriptor::SetFieldResult::Ok);
    EXPECT_EQ(rtd.get_field_from(b_value, "a")->to<sexp_sint_t>().value(), 3);

    auto sffr_no_field = rtd.set_field_for(b_value, "c", chibi.make_integer(3));
    EXPECT_EQ(sffr_no_field, RecordTypeDescriptor::SetFieldResult::NoField);

    auto sffr_wrong_type = rtd.set_field_for(chibi.make_integer(1), "a", chibi.make_integer(3));
    EXPECT_EQ(sffr_wrong_type, RecordTypeDescriptor::SetFieldResult::WrongType);
}
