#pragma once

#include <optional>

#include <chibi/eval.h>

/** A uility class on top of sexp. */
class SExp {
public:
    SExp(const sexp &context, const sexp &the_expression);

    operator sexp() const { return underlying; }

    bool operator==(const sexp &other) const;

    /** Convert the expression to a value specified by a sexp_types tag.
     *
     * If the value is not of this type, nullopt will be returned.
     */
    template <typename Output>
    std::optional<Output> to() const;

    /** Dump the text representation of the expression to a specified port (current output port, by default).
     *
     * This function also converts exceptions to show their inner text.
     */
    void dump(std::optional<sexp> port = std::nullopt);

    ~SExp();

private:
    sexp context;
    sexp underlying;
};
