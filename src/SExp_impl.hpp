#pragma once

#include <optional>

#include <chibi/eval.h>

#include "Chibi.hpp"

template <typename... Args>
std::optional<SExp> SExp::apply(Args... args) {
    if (sexp_applicablep(underlying)) {
        SExp args_lst = chibi.make_list(args...);
        return chibi.make_SExp(
            sexp_apply(chibi.context, underlying, args_lst)
        );
    }

    return std::nullopt;
}

template <typename Output>
std::optional<std::vector<Output>> SExp::to_vec_of() const {
    if (sexp_listp(chibi.context, underlying)) {
        sexp_uint_t len = chibi.make_SExp(
            sexp_length(chibi.context, underlying)
        ).to<sexp_sint_t>().value();

        std::vector<Output> res;
        for (sexp x = underlying; sexp_pairp(x); x = sexp_cdr(x)) {
            SExp tr = chibi.make_SExp(sexp_car(x));

            if (std::optional<Output> o = tr.to<Output>(); o.has_value()) {
                res.push_back(o.value());
            } else {
                return std::nullopt; // There's a non-Output value in the list somewhere..
            }
        }

        return res;
    }

    return std::nullopt;
}
