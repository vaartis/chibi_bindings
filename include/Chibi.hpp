#pragma once

#include <optional>
#include <vector>

#include <chibi/eval.h>

class SExp;

class Chibi {
public:
    Chibi();
    Chibi(sexp context);

    /** Evaluates a string as a scheme expression and returns the result. */
    SExp eval_string(const std::string &str);

    template <typename... Args>
    /** Evaluates several strings in order and returns the results.

        This might be useful when one needs to first import something, as imports take effect starting from the next expression.
    */
    std::vector<SExp> eval_strings(const Args &...strs);

    /** Registers a function pointer or a lambda to be callable from scheme.

        @arg name name, under which this function will be known in scheme
        @arg fnc the function pointer to use
     */
    template <typename... Args>
    SExp register_function(const std::string &name, sexp (*fnc)(sexp, sexp, long, Args...));

    /** Class that helps adding C++ class members to scheme. */
    template<typename Class>
    class ClassRegistrator {
    public:
        /** Create an instance that will prepand \a name to it's members when registering. */
        ClassRegistrator(Chibi &chibi, std::string &name);

        /** Register a member function and add it to scheme under the name "<class-name>-<member-name>".
         *
         * The registered function will accept a cpointer with a pointer to the class used as `this` and all
         * other arguments after that will be passed to the function. They'll be converted from sexp to C++
         * using the #make_from function. The result will be converted from C++ back to scheme using the #SExp.to
         * function.
         */
        template<typename Return, typename... Args>
        ClassRegistrator<Class> &register_method(std::string &&name, Return (Class::*member_function)(Args...));
    private:
        Chibi &chibi;

        std::string &class_name;
    };

    template<typename Class>
    ClassRegistrator<Class> register_class(std::string name);

    /** Add a directory to the module loading path. */
    SExp add_module_directory(std::string dir);


    // Functions to create scheme values


    /** Look up a value in the environment and return it, otherwise return \p dflt. */
    SExp env_ref(const std::string &name, sexp dflt = SEXP_FALSE);

    /** Create a scheme cpointer to an actual pointer of T.

        The pointer will not be freed by scheme, which means you will need to free it yourself.
        The scheme name of the type will be the one returnred by typeid (which varies between compilers).
     */
    template<typename T>
    SExp make_cpointer(T *ptr);

    /** Make, or "intern", a symbol from a given string.  */
    SExp make_symbol(const std::string &symbol);

    /** Create a list from expressions provided, making it a proper linked list with a nil at the end */
    template <typename Elem, typename... Elems>
    SExp make_list(Elem elem, Elems... elems);
    SExp make_list();

    /** Creates a scheme string from a C++ string. Unlike sexp_make_string, this function fills the string with actual content. */
    SExp make_string(const std::string &str);

    /** Create a scheme float. */
    SExp make_float(float num);

    /** Create an scheme integer, heap allocating as a bignum if needed. */
    SExp make_integer(sexp_sint_t num);

    /** Wraps a sexp into a SExp class */
    SExp make_SExp(const sexp &exp);


    SExp make_from(sexp_sint_t from);
    SExp make_from(std::string from);
    SExp make_from(float from);
    template<typename T>
    SExp make_from(std::vector<T> &from);

    ~Chibi();

    /** Current context used in this instance. */
    sexp context;

private:
    /** If the context is borrowed, we should not destroy it when we destroy the class. */
    bool borrowed_context = false;

    /** Current environment used in the context */
    sexp env;
};

#include "Chibi_impl.hpp"
#include "ClassRegistrator_impl.hpp"
