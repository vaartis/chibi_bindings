#pragma once

#include <string>

#include <chibi/eval.h>

#include "sexp.hpp"

template <typename... Args>
SExp Chibi::register_function(const std::string &name, sexp (*fnc)(sexp, sexp, long, Args...), sexp opcode_data) {

    return make_SExp(
        sexp_define_foreign_aux(
            context,
            env,
            name.c_str(),
            sizeof...(Args),
            0, // Flags, no idea what those are
            name.c_str(),
            // This is how you do variadic functions in C it seems, it will be cast to the appropriate function by scheme
            reinterpret_cast<sexp_proc1>(fnc),
            // Pass the function ptr as opcode data
            opcode_data
        )
    );
}

template<typename T>
SExp Chibi::make_cpointer(T *ptr, sexp_proc2 freeing_callback) {
    auto type_name = std::string("--cpointer-type-") + typeid(T).name(); // Make a type name that will be unique for every type

    SExp sexp_type = env_ref(type_name);
    if (sexp_type == SEXP_FALSE) {
        sexp_type = make_SExp(sexp_register_c_type(context, make_string(type_name), freeing_callback));

        // Save the type for future use
        sexp_env_define(context, env, make_symbol(type_name), sexp_type);
    }

    return make_SExp(
        sexp_make_cpointer(context, sexp_type_tag(sexp(sexp_type)), ptr, SEXP_FALSE, freeing_callback != nullptr) // Use the finalizer if there is one
    );
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

template<typename Class>
Chibi::ClassRegistrator<Class> Chibi::register_class(std::string name) {
    return ClassRegistrator<Class>(*this, name);
}
