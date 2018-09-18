#include <iostream>
#include <cassert>

#include <chibi/eval.h>

#include "Chibi.hpp"

Chibi::Chibi() {
    context = sexp_make_eval_context(nullptr, nullptr, nullptr, 0, 0);
    sexp_load_standard_env(context, nullptr, SEXP_SEVEN);
    sexp_load_standard_ports(context, NULL, stdin, stdout, stderr, 1);

    std::string s = R"(
        (begin
         (write "test"))
        )";

    sexp res = sexp_eval_string(context, s.c_str(), -1, sexp_context_env(context));

    if (sexp_exceptionp(res)) {
        sexp_print_exception(context, res, sexp_current_error_port(context));
    }
}

Chibi::~Chibi() {
    sexp_destroy_context(context);
}
