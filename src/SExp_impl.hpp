#pragma once

#include <optional>

#include <chibi/eval.h>

#include "Chibi.hpp"

template <typename Output>
std::optional<Output *> SExp::to_ptr() const {
    std::string type_name = std::string("--cpointer-type-") + typeid(Output).name();

    if (SExp sexp_type = chibi.env_ref(type_name); sexp_type != SEXP_FALSE) {
        if (chibi.make_SExp(sexp_object_type(chibi.context, sexp(underlying))) == sexp_type) {
            return static_cast<Output *>(sexp_cpointer_value(underlying));
        }
    }

    return std::nullopt;
}

template <typename... Args>
std::optional<SExp> SExp::apply(Args... args) const {
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
    std::vector<Output> res;

    if (sexp_vectorp(underlying)) {
        sexp_uint_t len = sexp_vector_length(underlying);
        auto vecdata = sexp_vector_data(underlying);

        for (int i = 0; i < len; i++) {
            SExp tr = chibi.make_SExp(vecdata[i]);

            if (std::optional<Output> o = tr.to<Output>(); o.has_value()) {
                res.push_back(o.value());
            } else {
                return std::nullopt; // There's a non-Output value in the list somewhere..
            }
        }

        return res;
    } else if (sexp_pairp(underlying)) {
        for (auto i = underlying; sexp_pairp(i); i = sexp_cdr(i)) {
            SExp tr = chibi.make_SExp(sexp_car(i));

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
