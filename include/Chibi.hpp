#pragma once

#include <optional>
#include <vector>

#include <chibi/eval.h>

#include "SExp.hpp"

class Chibi {
public:
    Chibi();

    /** Evaluates a string as a scheme expression and returns the result. */
    SExp eval_string(const std::string &str);

    template <typename... Args>
    /** Evaluates several strings in order and returns the results.

        This might be useful when one needs to first import something, as imports take effect starting from the next expression.
    */
    std::vector<SExp> eval_strings(const Args &...strs);

    /** Registers a function pointer or a lambda to be callable from scheme.

        @arg name name, under which this function will be known in scheme
        @arg fnc the function pointer to use
     */
    template <typename... Args>
    void register_function(const std::string &name, sexp (*fnc)(sexp, sexp, long, Args...));

    // Functions to create scheme values
    /** Create a list from expressions provided, making it a proper linked list with a nil at the end */
    template <typename Elem, typename... Elems>
    SExp make_list(Elem elem, Elems... elems);

    template <typename Elem>
    SExp make_list(Elem elem);

    /** Creates a scheme string from a C++ string. Unlike sexp_make_string, this function fills the string with actual content. */
    SExp make_string(const std::string &str);

    /** Create an scheme integer, heap allocating as a bignum if needed. */
    SExp make_integer(sexp_sint_t num);

    /** Wraps a sexp into a SExp class */
    SExp make_SExp(const sexp &exp);

    ~Chibi();

    /** Current context used in this instance. */
    sexp context;

private:
    /** Current environment used in the context */
    sexp env;
};

template <typename... Args>
void Chibi::register_function(const std::string &name, sexp (*fnc)(sexp, sexp, long, Args...)) {
    sexp_define_foreign(context, env, name.c_str(), sizeof...(Args), fnc);
}

template <typename Elem>
SExp Chibi::make_list(Elem elem) {
    return make_SExp(sexp_list1(context, elem));
}

template <typename Elem, typename... Elems>
SExp Chibi::make_list(Elem elem, Elems... elems) {
    return make_SExp(sexp_cons(context, elem, make_list(elems...)));
}

template <typename... Args>
std::vector<SExp> Chibi::eval_strings(const Args &...strs) {
    std::vector<SExp> res;

    for (const auto &str : { strs... }) {
        res.push_back(eval_string(str));
    }

    return res;
}
