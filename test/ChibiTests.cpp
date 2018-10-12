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

template<typename Class>
class ChibiClassRegistrator {
public:
    ChibiClassRegistrator(Chibi &chibi, Class *class_ptr) : chibi(chibi), class_ptr(class_ptr) {

    }

    // template<typename ReturnType, typename... Args>
    // static call_function

    template<typename ReturnType, typename... Args>
    SExp register_function(std::string &name, ReturnType (Class::*member_function)(Args...)) {
        auto memfn_ptr = new decltype(member_function);

        auto freeing_fnc = +[](sexp context, sexp member) {
                                auto member_cpp = static_cast<decltype(member_function) *>(sexp_cpointer_value(member));
                                delete member_cpp;
                            };

        SExp memfn_ptr_type = chibi.make_SExp(sexp_register_c_type(chibi.context, chibi.make_string(typeid(memfn_ptr).name()), freeing_fnc));

        SExp wrapped_memfn_ptr = sexp_make_cpointer(chibi.context, memfn_ptr_type, memfn_ptr, SEXP_FALSE, 1);
    }
private:
    Chibi &chibi;
    Class *class_ptr;
    SExp class_ptr_sexp;
};

template<typename T>
SExp register_class(Chibi &chibi, std::string name) {
    auto this_ptr = std::make_shared<T>();
    SExp class_ptr_sexp = chibi.make_cpointer(class_ptr);

    return ChibiClassRegistrator(chibi, this_ptr);
}

TEST(ChibiTests, MemberFunctionCall) {
    Chibi chibi;

    struct Testy {
        int fnc(int x) { return x + 1; }
    };

    register_class<Testy>(chibi, "testy");

}
