#include "SExp.hpp"

SExp::SExp(const sexp &context, const sexp &the_expression)
    : context(context)
    , underlying(the_expression) {
    sexp_preserve_object(context, underlying);
}

void SExp::dump(std::optional<sexp> port) {
    auto prt = port.value_or(sexp_current_error_port(context));
    if (sexp_exceptionp(underlying)) {
        sexp_print_exception(context, underlying, prt);
    } else {
        sexp_write(context, underlying, prt);
    }
}

bool SExp::operator==(const sexp &other) const {
    return sexp_unbox_boolean(
        sexp_equalp(context, underlying, other)
    );
}

template<>
std::optional<sexp_sint_t> SExp::to() const {
    return sexp_fixnump(underlying)
        ? std::make_optional(sexp_unbox_fixnum(underlying))
        : std::nullopt;
}

template<>
std::optional<std::string> SExp::to() const {
    return sexp_stringp(underlying)
        ? std::make_optional(std::string(sexp_string_data(underlying)))
        : std::nullopt;;
}

template<>
std::optional<bool> SExp::to() const {
    return sexp_booleanp(underlying)
        ? std::make_optional(static_cast<bool>(sexp_unbox_boolean(underlying)))
        : std::nullopt;;
}

SExp::~SExp() {
    sexp_release_object(context, underlying);
}
