#pragma once

#include "Chibi.hpp"

// Converters to sexp from values.

template<>
struct ToSExp<sexp_sint_t> {
    static SExp to(Chibi &chibi, sexp_sint_t from) {
        return chibi.make_integer(from);
    }
};

template<>
struct ToSExp<bool> {
    static SExp to(Chibi &chibi, bool from) {
        return chibi.make_bool(from);
    }
};

template<>
struct ToSExp<std::string> {
    static SExp to(Chibi &chibi, const std::string &from) {
        return chibi.make_string(from);
    }
};

template<>
struct ToSExp<float> {
    static SExp to(Chibi &chibi, float from) {
        return chibi.make_float(from);
    }
};

template<typename T>
struct ToSExp<T *> {
    static SExp to(Chibi &chibi, T *from) {
        return chibi.make_cpointer(from);
    }
};

template<>
struct ToSExp<Symbol> {
    static SExp to(Chibi &chibi, std::string from) {
        return chibi.make_symbol(from);
    }
};

// Converters from sexp to values.

template<>
struct FromSExp<sexp_sint_t> {
    static std::optional<sexp_sint_t> from(Chibi &chibi, sexp from) {
        return sexp_fixnump(from)
            ? std::make_optional(sexp_unbox_fixnum(from))
            : std::nullopt;
    }
};

template<>
struct FromSExp<float> {
    static std::optional<float> from(Chibi &chibi, sexp from) {
        return sexp_flonump(from)
            ? std::make_optional(sexp_flonum_value(from))
            : std::nullopt;
    }
};

template<>
struct FromSExp<std::string> {
    static std::optional<std::string> from(Chibi &chibi, sexp from) {
        return sexp_stringp(from)
            ? std::make_optional(std::string(sexp_string_data(from)))
            : std::nullopt;
    }
};

template<>
struct FromSExp<bool> {
    static std::optional<bool> from(Chibi &chibi, sexp from) {
        return sexp_booleanp(from)
            ? std::make_optional(static_cast<bool>(sexp_unbox_boolean(from)))
            : std::nullopt;
    }
};

template<>
struct FromSExp<Symbol> {
    static std::optional<Symbol> from(Chibi &chibi, sexp from) {
        return sexp_symbolp(from)
            ? std::make_optional(
                Symbol(
                    sexp_string_data(
                        sexp_symbol_to_string(chibi.context, from)
                    )
                )
            )
            : std::nullopt;

    }
};

template<typename T>
struct FromSExp<T *> {
    static std::optional<T *> from(Chibi &chibi, sexp from) {
        std::string type_name = std::string("--cpointer-type-") + typeid(T).name();

        if (SExp sexp_type = chibi.env_ref(type_name); sexp_type != SEXP_FALSE) {
            if (chibi.make_SExp(sexp_object_type(chibi.context, from)) == sexp_type) {
                return static_cast<T *>(sexp_cpointer_value(from));
            }
        }

        return std::nullopt;
    }
};

template<typename T>
struct FromSExp<std::vector<T>> {
    static std::optional<std::vector<T>> from(Chibi &chibi, sexp from) {
        std::vector<T> res;

        if (sexp_vectorp(from)) {
            sexp_uint_t len = sexp_vector_length(from);
            auto vecdata = sexp_vector_data(from);

            for (int i = 0; i < len; i++) {
                SExp tr = chibi.make_SExp(vecdata[i]);

                if (std::optional<T> o = FromSExp<T>::from(chibi, tr); o.has_value()) {
                    res.push_back(o.value());
                } else {
                    return std::nullopt; // There's a non-Output value in the list somewhere..
                }
            }

            return res;
        } else if (sexp_pairp(from)) {
            for (auto i = from; sexp_pairp(i); i = sexp_cdr(i)) {
                SExp tr = chibi.make_SExp(sexp_car(i));

                if (std::optional<T> o = FromSExp<T>::from(chibi, tr); o.has_value()) {
                    res.push_back(o.value());
                } else {
                    return std::nullopt; // There's a non-Output value in the list somewhere..
                }
            }

            return res;
        }

        return std::nullopt;
    }
};
