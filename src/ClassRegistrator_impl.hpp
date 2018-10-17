#pragma once

#include <string>

#include "sexp.hpp"
#include "SExpConverters.hpp"

namespace { // Implementation detail, so it's in an anon namespace

template <typename Class>
struct ClassRegistratorHelpers {
    template <typename FieldType>
    static sexp field_get(sexp context, sexp self, long n, sexp this_ptr_sexp) {
        auto this_ptr = static_cast<Class *>(sexp_cpointer_value(this_ptr_sexp));
        auto member_ptr = static_cast<FieldType Class::**>(sexp_cpointer_value(sexp_opcode_data(self)));

        Chibi chibi(context);
        return ToSExp<FieldType>::to(chibi, this_ptr->**member_ptr);
    }

    template <typename FieldType>
    static sexp field_set(sexp context, sexp self, long n, sexp this_ptr_sexp, sexp sexp_new_value) {
        auto this_ptr = static_cast<Class *>(sexp_cpointer_value(this_ptr_sexp));
        auto member_ptr = static_cast<FieldType Class::**>(sexp_cpointer_value(sexp_opcode_data(self)));

        Chibi chibi(context);
        return convert_or_exception(chibi, self, sexp_new_value, this_ptr->**member_ptr);
    }

    template <typename Return, typename Arg1>
    static sexp call(sexp context, sexp self, long n, sexp this_ptr_sexp, sexp arg1_sexp) {
        auto this_ptr = static_cast<Class *>(sexp_cpointer_value(this_ptr_sexp));
        auto fnc_ptr = static_cast<Return (Class::**) (Arg1)>(sexp_cpointer_value(sexp_opcode_data(self))); // Extract the function pointer from opcode data

        Chibi chibi(context);

        Arg1 arg1;
        if (sexp set_result = convert_or_exception(chibi, self, arg1_sexp, arg1); set_result != SEXP_UNDEF) {
            return set_result;
        }

        Return res = (this_ptr->**fnc_ptr)(arg1);

        return ToSExp<Return>::to(chibi, res);
    }

    template <typename Return, typename Arg1, typename Arg2>
    static sexp call(sexp context, sexp self, long n, sexp this_ptr_sexp, sexp arg1_sexp, sexp arg2_sexp) {
        auto this_ptr = static_cast<Class *>(sexp_cpointer_value(this_ptr_sexp));
        auto fnc_ptr = static_cast<Return (Class::**) (Arg1, Arg2)>(sexp_cpointer_value(sexp_opcode_data(self)));

        Chibi chibi(context);

        Arg1 arg1;
        if (sexp set_result = convert_or_exception(chibi, self, arg1_sexp, arg1); set_result != SEXP_UNDEF) {
            return set_result;
        }
        Arg2 arg2;
        if (sexp set_result = convert_or_exception(chibi, self, arg2_sexp, arg2); set_result != SEXP_UNDEF) {
            return set_result;
        }

        Return res = (this_ptr->**fnc_ptr)(arg1, arg2);

        return ToSExp<Return>::to(chibi, res);
    }

private:
    /** Convert from sexp to the expected value type and set to to \a set_result_to or return an exception.
     *
     * @arg chibi chibi instance to use
     * @arg self self pointer for error reporting
     * @arg convert_from the value we're converting from
     * @arg set_result_to where the result should go on success
     *
     * @returns SEXP_UNDEF if everything went ok or a scheme exception if there was an error
     */
    template <typename Res>
    static sexp convert_or_exception(Chibi &chibi, const sexp self, const sexp convert_from, Res &set_result_to) {
        if (auto r = FromSExp<Res>::from(chibi, convert_from); r.has_value()) {
            set_result_to = *r;
        } else {
            return sexp_xtype_exception(
                chibi.context,
                self,
                (std::string("Expected a value of type ") + typeid(Res).name() + " for convertion").c_str(),
                convert_from
            );
        }

        // Everything is ok, return undef
        return SEXP_UNDEF;
    }
};

}

template <typename Class>
Chibi::ClassRegistrator<Class>::ClassRegistrator(Chibi &chibi, const std::string &name)
    : chibi(chibi)
    , class_name(name) { }

template <typename Class>
template <typename Return, typename... Args>
Chibi::ClassRegistrator<Class> &Chibi::ClassRegistrator<Class>::register_method(const std::string &name, Return (Class::*member_function)(Args...)) {
    auto memfn_ptr = new decltype(member_function)(member_function);

    auto memfn_freeing_fun = +[](sexp ctx, sexp self, long n, sexp ptr_to_free) {
                                  // Free the pointer we've created
                                  delete static_cast<decltype(memfn_ptr)>(sexp_cpointer_value(ptr_to_free));

                                  return SEXP_UNDEF;
                              };

    SExp wrapped_memfn_ptr = chibi.make_cpointer(memfn_ptr, memfn_freeing_fun);

    std::string full_name = class_name + "-" + name;
    chibi.register_function(full_name, ClassRegistratorHelpers<Class>::template call<Return, Args...>, wrapped_memfn_ptr);

    return *this;
}


template <typename Class>
template <typename FieldType>
Chibi::ClassRegistrator<Class> &Chibi::ClassRegistrator<Class>::register_field(const std::string &name, FieldType Class::*field, bool generate_setter) {
    auto field_ptr = new decltype(field)(field);

    auto field_freeing_fun = +[](sexp ctx, sexp self, long n, sexp ptr_to_free) {
                                  // Free the pointer we've created
                                  delete static_cast<decltype(field_ptr)>(sexp_cpointer_value(ptr_to_free));

                                  return SEXP_UNDEF;
                              };
    // Create the field cpointer
    SExp wrapped_field_ptr = chibi.make_cpointer(field_ptr, field_freeing_fun);

    // Register the function to access it
    std::string full_name = class_name + "-" + name;
    chibi.register_function(full_name, ClassRegistratorHelpers<Class>::template field_get<FieldType>, wrapped_field_ptr);

    // Optionally generate a setter for the field
    if (generate_setter) {
        std::string setter_full_name = std::string("set-") + class_name + "-" + name + "!";
        chibi.register_function(setter_full_name, ClassRegistratorHelpers<Class>::template field_set<FieldType>, wrapped_field_ptr);
    }

    return *this;
}
