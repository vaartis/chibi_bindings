#pragma once

#include <optional>
#include <vector>
#include <string>

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
        @arg opcode_data optional data associated with the function opcode
     */
    template <typename... Args>
    SExp register_function(const std::string &name, sexp (*fnc)(sexp, sexp, long, Args...), sexp opcode_data = nullptr);

    /** Class that helps adding C++ class members to scheme. */
    template <typename Class>
    class ClassRegistrator {
    public:
        /** Create an instance that will prepend \a name to its members when registering. */
        ClassRegistrator(Chibi &chibi, const std::string &name);

        /** Register a member function and add it to scheme under the name "<class-name>-<member-name>".
         *
         * The registered function will accept a cpointer with a pointer to the class used as `this` and all
         * other arguments after that will be passed to the function. They'll be converted from sexp to C++
         * using the #make_from function. The result will be converted from C++ back to scheme using the #SExp.to
         * function.
         */
        template <typename Return, typename... Args>
        ClassRegistrator<Class> &register_method(const std::string &name, Return (Class::*member_function)(Args...));

        template <typename... Args>
        /** Same as #register_method except for methods returning void. */
        ClassRegistrator<Class> &register_noreturn_method(const std::string &name, void (Class::*member_function)(Args...));


        /** Register a class field to be accessible from scheme under the name "<class-name>-<field-name>".
         *  Optionally, add a setter named "set-<class-name>-<field-name>!".
         *
         * @arg name field name
         * @arg field field reference
         * @arg generate_setter whether to generate a setter, it's not generated by default
         */
        template <typename FieldType>
        ClassRegistrator<Class> &register_field(const std::string &name, FieldType Class::*field, bool generate_setter = false);

        /** Register the default (no argument) constructor as make-<class-name>-<postfix>.
         *
         * @args postfix postfix to append to the constructor's name in scheme
         */
        Chibi::ClassRegistrator<Class> &register_default_constructor(std::string postfix = "");

        /** Register the class constructor with specified argument types as make-<class-name>-<postfix>.
         *
         * @args postfix postfix to append to the constructor's name in scheme
         */
        template <typename... Args>
        Chibi::ClassRegistrator<Class> &register_constructor(std::string postfix = "");

    private:
        Chibi &chibi;

        const std::string class_name;
    };

    template <typename Class>
    ClassRegistrator<Class> register_class(std::string name);

    /** Add a directory to the module loading path. */
    SExp add_module_directory(std::string dir);


    // Functions to create scheme values


    /** Look up a value in the environment and return it, otherwise return \p dflt. */
    SExp env_ref(const std::string &name, sexp dflt = SEXP_FALSE);

    /** Create a scheme cpointer to an actual pointer of T.

        The pointer will not be freed by scheme, which means you will need to free it yourself.
        The scheme name of the type will be the one returnred by typeid (which varies between compilers).

        @arg ptr the pointer to use
        @arg freeing_callback when the reference count reaches zero or the context is destroyed, this function will be called
                              like a normal scheme function with it's first non-utility argument being the cpointer to free
     */
    template <typename T>
    SExp make_cpointer(T *ptr, sexp_proc2 freeing_callback = nullptr);

    /** Make, or "intern", a symbol from a given string.  */
    SExp make_symbol(const std::string &symbol);

    /** Create a list from expressions provided, making it a proper linked list with a nil at the end */
    template <typename Elem, typename... Elems>
    SExp make_list(Elem elem, Elems... elems);
    SExp make_list();

    /** Creates a scheme string from a C++ string. Unlike sexp_make_string, this function fills the string with actual content. */
    SExp make_string(const std::string &str);

    /** Creates a scheme boolean. */
    SExp make_bool(bool b);

    /** Create a scheme float. */
    SExp make_float(float num);

    /** Create an scheme integer, heap allocating as a bignum if needed. */
    SExp make_integer(sexp_sint_t num);

    /** Wraps a sexp into a SExp class */
    SExp make_SExp(const sexp &exp);

    /** A function to convert some  C++ values to scheme. Useful in template context, when the exact type is unkown. */
    SExp make_from(sexp_sint_t from);
    SExp make_from(const std::string &from);
    SExp make_from(float from);
    template <typename T>
    SExp make_from(const std::vector<T> &from);

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
