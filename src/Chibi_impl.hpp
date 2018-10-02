#pragma once

#include <string>

#include <chibi/eval.h>

#include "sexp.hpp"

template <typename... Args>
void Chibi::register_function(const std::string &name, sexp (*fnc)(sexp, sexp, long, Args...)) {
    sexp_define_foreign(context, env, name.c_str(), sizeof...(Args), fnc);
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
