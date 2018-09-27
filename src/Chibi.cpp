#include <chibi/eval.h>

#include "Chibi.hpp"

Chibi::Chibi() {
    context = sexp_make_eval_context(nullptr, nullptr, nullptr, 0, 0);
    env = sexp_context_env(context);

    // FIXME: add an actual path configuration or something
    sexp_add_module_directory(context, make_string("../deps/chibi/share/chibi/"), SEXP_TRUE);
    sexp_add_module_directory(context, make_string("../deps/chibi/lib/chibi/"), SEXP_TRUE);

    sexp_load_standard_env(context, nullptr, SEXP_SEVEN);
    sexp_load_standard_ports(context, nullptr, stdin, stdout, stderr, 1);
}

SExp Chibi::eval_string(const std::string &str) {
    return make_SExp(sexp_eval_string(context, str.c_str(), -1, nullptr));
}

SExp Chibi::make_string(const std::string &str) {
    return make_SExp(sexp_c_string(context, str.c_str(), -1));
}

SExp Chibi::make_integer(sexp_sint_t num) {
    return make_SExp(sexp_make_integer(context, num));
}

SExp Chibi::make_SExp(const sexp &exp) {
    return SExp(context, exp);
}

Chibi::~Chibi() {
    sexp_destroy_context(context);
}
