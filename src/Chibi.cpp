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

sexp Chibi::make_string(std::string str) {
    return sexp_c_string(context, str.c_str(), -1);
}

sexp Chibi::make_integer(sexp_sint_t num) {
    return sexp_make_integer(context, num);
}

Chibi::~Chibi() {
    sexp_destroy_context(context);
}
