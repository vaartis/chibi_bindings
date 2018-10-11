#include "sexp.hpp"
#include "Chibi.hpp"

SExp::SExp(const SExp &other)
    : chibi(other.chibi)
    , underlying(other) {
    sexp_preserve_object(chibi.context, underlying);
}

SExp::SExp(Chibi &chibi) : chibi(chibi) {
    SExp(chibi, SEXP_NULL);
}

SExp::SExp(Chibi &chibi, sexp the_expression)
    : chibi(chibi)
    , underlying(the_expression) {
    sexp_preserve_object(chibi.context, underlying);
}

SExp &SExp::operator=(const SExp &other) {
    sexp_release_object(chibi.context, underlying);

    chibi = other.chibi;
    underlying = other;

    // Increment the GC counter for the new object
    sexp_preserve_object(chibi.context, underlying);

    return *this;
}

bool SExp::operator==(const sexp &other) const {
    return sexp_unbox_boolean(
        sexp_equalp(chibi.context, underlying, other)
    );
}

template <>
std::optional<sexp_sint_t> SExp::to() const {
    return sexp_fixnump(underlying)
        ? std::make_optional(sexp_unbox_fixnum(underlying))
        : std::nullopt;
}

template <>
std::optional<float> SExp::to() const {
    return sexp_flonump(underlying)
        ? std::make_optional(sexp_flonum_value(underlying))
        : std::nullopt;
}

template <>
std::optional<std::string> SExp::to() const {
    return sexp_stringp(underlying)
        ? std::make_optional(std::string(sexp_string_data(underlying)))
        : std::nullopt;
}

template <>
std::optional<bool> SExp::to() const {
    return sexp_booleanp(underlying)
        ? std::make_optional(static_cast<bool>(sexp_unbox_boolean(underlying)))
        : std::nullopt;
}

template <>
std::optional<Symbol> SExp::to() const {
    return sexp_symbolp(underlying)
        ? std::make_optional(
            Symbol(
                sexp_string_data(
                    sexp_symbol_to_string(chibi.context, underlying)
                    )
            )
          )
        : std::nullopt;
}

void SExp::for_each(std::function<void(SExp)> f) const {
    if (sexp_vectorp(underlying)) {
        sexp_uint_t len = sexp_vector_length(underlying);
        auto vecdata = sexp_vector_data(underlying);

        for (int i = 0; i < len; i++) {
            SExp tr = chibi.make_SExp(vecdata[i]);

            f(tr);
        }
    } else if (sexp_pairp(underlying)) {
        for (auto i = underlying; sexp_pairp(i); i = sexp_cdr(i)) {
            SExp tr = chibi.make_SExp(sexp_car(i));

            f(tr);
        }
    }
}

void SExp::dump_to_port(std::optional<sexp> port) const {
    auto prt = port.value_or(sexp_current_error_port(chibi.context));
    if (sexp_exceptionp(underlying)) {
        sexp_print_exception(chibi.context, underlying, prt);
    } else {
        sexp_write(chibi.context, underlying, prt);
        sexp_newline(chibi.context, prt);
    }
}

std::string SExp::dump_to_string() const {
    auto prt = chibi.make_SExp(sexp_open_output_string(chibi.context));
    dump_to_port(prt);

    return chibi.make_SExp(sexp_get_output_string(chibi.context, prt)).to<std::string>().value();
}


SExp::~SExp() {
    sexp_release_object(chibi.context, underlying);
}
