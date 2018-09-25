#include <iostream>
#include <cassert>

#include <chibi/eval.h>

#include "Chibi.hpp"

Chibi::Chibi() {
    context = sexp_make_eval_context(nullptr, nullptr, nullptr, 0, 0);
    env = sexp_context_env(context);
    sexp_load_standard_ports(context, NULL, stdin, stdout, stderr, 1);

    // FIXME: add an actual path configuration or something
    sexp_add_module_directory(context, sexp_c_string(context, "../deps/chibi/share/chibi/", -1), SEXP_TRUE);

    sexp std_loading_res = sexp_load_standard_env(context, sexp_context_env(context), SEXP_SEVEN);
}

std::string Chibi::sexp_to_string(sexp exp) {
    sexp sexp_str = sexp_write_to_string(context, exp);
    return std::string(sexp_string_data(sexp_str));
}

void Chibi::print_exception(sexp exception, std::optional<sexp> port) {
    if (!sexp_exceptionp(exception)) {
        throw std::invalid_argument(
            "Expected an exception to print, but the expression was " + sexp_to_string(exception)
        );
    }

    sexp_print_exception(context, exception, port.value_or(sexp_current_error_port(context)));
}

sexp Chibi::eval_string(std::string str) {
    return sexp_eval_string(context, str.c_str(), -1, nullptr);
}

Chibi::~Chibi() {
    sexp_destroy_context(context);
}
