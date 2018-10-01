#pragma once

#include <vector>
#include <optional>

#include <chibi/eval.h>

class Chibi;

/** A uility class on top of sexp. */
class SExp {
public:
    SExp(Chibi &chibi, sexp the_expression);

    operator sexp() const { return underlying; }

    bool operator==(const sexp &other) const;

    template <typename... Args>
    std::optional<SExp> apply(Args... args);

    /** Convert the expression to a value specified.
     *
     * If the value is not of this type, nullopt will be returned.
     */
    template <typename Output>
    std::optional<Output> to() const;

    /** Convert the list of expressions to a vector of values specified.
     *
     * If the value is not of this type, nullopt will be returned.
p     */
    template <typename Output>
    std::optional<std::vector<Output>> to_vec_of() const;

    /** Dump the text representation of the expression to a specified port (current output port, by default).
     *
     * This function also converts exceptions to show their inner text.
     */
    void dump(std::optional<sexp> port = std::nullopt);

    ~SExp();

private:
    Chibi &chibi;
    sexp underlying;
};

#include "SExp_impl.hpp"
