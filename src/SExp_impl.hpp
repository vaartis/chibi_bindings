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
