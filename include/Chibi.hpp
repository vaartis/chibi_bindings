#pragma once

#include <optional>

#include <chibi/eval.h>

class Chibi {
public:
    Chibi();

    /** Converts a chibi expression to it's string representation. */
    std::string sexp_to_string(sexp exp);

    /** Evaluates a string as a chibi expression and returns the result */
    sexp eval_string(std::string str);

    /** Registers a function pointer or a lambda to be callable from scheme.

        @arg name name, under which this function will be known in scheme
        @arg fnc the function pointer to use
     */
    template<typename... Args>
    void register_function(std::string name, sexp (*fnc)(sexp, sexp, long, Args...));


    // Functions to create scheme values


    /** Create a list from expressions provided, making it a proper linked list with a nil at the end */
    template<typename Elem, typename... Elems>
    sexp make_list(Elem elem, Elems... elems);
    template<typename Elem>
    sexp make_list(Elem elem);

    /** Creates a scheme string from a C++ string. Unlike sexp_make_string, this function fills the string with actual content. */
    sexp make_string(std::string str);

    /** Create an scheme integer, heap allocating as a bignum if needed. */
    sexp make_integer(sexp_sint_t num);


    // Debug/Printing


    /** Prints a chibi exception to the specified port.
     *
     * @throws std::invalid_argument if `exception` is not an exception
     */
    void print_exception(sexp exception, std::optional<sexp> port = std::nullopt);


    // Members


    /** Current context used in this instance. */
    sexp context;

    /** Current environment used in the context */
    sexp env;

    ~Chibi();
};

template<typename... Args>
void Chibi::register_function(std::string name, sexp (*fnc)(sexp, sexp, long, Args...)) {
    sexp_define_foreign(context, env, name.c_str(), sizeof...(Args), fnc);
}

template<typename Elem>
sexp Chibi::make_list(Elem elem) {
    return sexp_list1(context, elem);
}

template<typename Elem, typename... Elems>
sexp Chibi::make_list(Elem elem, Elems... elems) {
    return sexp_cons(context, elem, make_list(elems...));
}
