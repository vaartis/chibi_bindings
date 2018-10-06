#pragma once

#include <vector>
#include <optional>
#include <functional>

#include <chibi/eval.h>

class Chibi;

/** A symbol, basically a wrapper around a string. */
struct Symbol {
    Symbol(const std::string &name) : name(name) {}
    Symbol(const char *name) : name(name) {}

    operator std::string() { return name; }
    bool operator ==(const Symbol &other) const { return other.name == name; }

    std::string name;
};

/** A uility class on top of sexp. */
class SExp {
public:
    SExp(Chibi &chibi, sexp the_expression);

    operator sexp() const { return underlying; }

    bool operator==(const sexp &other) const;

    template <typename... Args>
    std::optional<SExp> apply(Args... args) const;

    /** Convert the expression to a value specified.
     *
     * If the value is not of this type, nullopt will be returned.
     */
    template <typename Output>
    std::optional<Output> to() const;

    /** Convert a vector or a list of expressions to an std::vector of specified type.
     *
     * If the value is not of this type, nullopt will be returned.
     */
    template <typename Output>
    std::optional<std::vector<Output>> to_vec_of() const;

    /** Run the provided function on every element of the underlying list or vector.
     *
     * If the underlying value is neither a list, nor a vector, this function does nothing.
     */
    void for_each(std::function<void(SExp)> f) const;

    /** Dump the text representation of the expression to a specified port (current output port, by default).
     *
     * This function also converts exceptions to show their inner text.
     */
    void dump_to_port(std::optional<sexp> port = std::nullopt) const;

    /** A wrapper around SExp::dump_to_port() that creates a port and reads from it to a string. */
    std::string dump_to_string() const;

    ~SExp();

private:
    Chibi &chibi;
    sexp underlying;
};

#include "SExp_impl.hpp"
