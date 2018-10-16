#pragma once

#include <type_traits>
#include <optional>

#include <chibi/sexp.h>

/** \file
 *  Converters from and to #SExp are placed here. The usage is as following:
 *  \code
 *  // To convert to a C++ value to SExp, use ToSExp
 *  SExp str_sexp = ToSExp<std::string>::to(chibi, "1234");
 *  // To convert from SExp to a C++ value, use FromSExp
 *  FromSExp<std::string>::to(chibi, str_sexp);
 *  // These two should produce equivalent values
 *  \endcode
 *
 *  ToSExp and FromSExp can be overloaded/specialized for your own types in order for them to be
 *  automatically convertible by e.g. #Chibi::ClassRegistrator. Here is an example of how to overload them for a custom type:
 *  \code
 *  struct YourType { int x; }
 *
 *  template<>
 *  struct ToSExp<YourType> {
 *      static SExp to(Chibi &chibi, YourType from) {
 *          // Make your type into sexp here
 *      }
 *  };
 *
 * template<>
 * struct FromSExp<YourType> {
 *     static std::optional<YourType> from(Chibi &chibi, YourType from) {
 *         // Make your type from sexp here
 *     }
 * };
 *
 *  \endcode
 *
 * Some types are already convertible and available out of the box. This includes any kind of pointer (you can still overload it
 * to work differently on your pointers).
 */

class Chibi;
class SExp;

template<typename T>
struct ToSExp {
    static SExp to(Chibi &chibi, T from);
};

template<typename T>
struct FromSExp {
    static std::optional<T> from(Chibi &chibi, sexp from);
};

#include "SExpConverters_impl.hpp"
