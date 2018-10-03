#include <chibi/eval.h>
#include <chibi/gc_heap.h>

#include "Chibi.hpp"

Chibi::Chibi() {
    context = sexp_load_image("./chibi_lib/chibi.img", 0, 0, 0);
    if (context != nullptr) {
        env = sexp_context_env(context);

        sexp_add_module_directory(context, make_string("./chibi_lib"), SEXP_TRUE);

        sexp_load_standard_env(context, nullptr, SEXP_SEVEN);
        sexp_load_standard_ports(context, nullptr, stdin, stdout, stderr, 1);
    } else {
        throw std::runtime_error(sexp_load_image_err());
    }
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

SExp Chibi::make_integer(sexp_sint_t num) {
    return make_SExp(sexp_make_integer(context, num));
}

SExp Chibi::make_SExp(const sexp &exp) {
    return SExp(*this, exp);
}

Chibi::~Chibi() {
    sexp_destroy_context(context);
}
