#include <chibi/eval.h>
#include <chibi/gc_heap.h>

#include "Chibi.hpp"

Chibi::Chibi() {
    context = sexp_make_eval_context(nullptr, nullptr, nullptr, 0, 0);

    add_module_directory("./chibi_lib");

    sexp_load_standard_env(context, nullptr, SEXP_SEVEN);
    sexp_load_standard_ports(context, nullptr, stdin, stdout, stderr, 1);

    env = sexp_context_env(context);
}

Chibi::Chibi(sexp context) {
    if (!sexp_contextp(context)) {
        throw std::runtime_error("Passed object is not a context");
    }

    this->context = context;
    env = sexp_context_env(context);

    // The context is borrowed, do not destroy it
    borrowed_context = true;
}

SExp Chibi::add_module_directory(std::string dir) {
    return make_SExp(sexp_add_module_directory(context, make_string(dir), SEXP_TRUE));
}

SExp Chibi::env_ref(const std::string &name, sexp dflt) {
    return make_SExp(sexp_env_ref(context, env, make_symbol(name), dflt));
}

SExp Chibi::eval_string(const std::string &str) {
    return make_SExp(sexp_eval_string(context, str.c_str(), -1, nullptr));
}

SExp Chibi::make_symbol(const std::string &symbol) {
    return make_SExp(sexp_intern(context, symbol.c_str(), -1));
}

SExp Chibi::make_list() { return make_SExp(SEXP_NULL); }

SExp Chibi::make_string(const std::string &str) {
    return make_SExp(sexp_c_string(context, str.c_str(), -1));
}

SExp Chibi::make_float(float num) {
    return make_SExp(sexp_make_flonum(context, num));
}

SExp Chibi::make_integer(sexp_sint_t num) {
    return make_SExp(sexp_make_integer(context, num));
}

SExp Chibi::make_SExp(const sexp &exp) {
    return SExp(*this, exp);
}

SExp Chibi::make_from(sexp_sint_t from) {
    return make_integer(from);
}

SExp Chibi::make_from(const std::string &from) {
    return make_string(from);
}

SExp Chibi::make_from(float from) {
    return make_float(from);
}

template<typename T>
SExp Chibi::make_from(const std::vector<T> &from) {
    return make_float(from);
}

Chibi::~Chibi() {
    if (!borrowed_context) {
        sexp_destroy_context(context);
    }
}
