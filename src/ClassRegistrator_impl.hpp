#include <string>

#include "sexp.hpp"

namespace { // Implementation detail, so it's in an anon namespace
    template<typename Class>
    struct ClassRegistratorHelpers {
        template<typename Return, typename Arg1>
        static sexp call(sexp context, sexp self, long n, sexp this_ptr_sexp, sexp arg1) {
            auto this_ptr = static_cast<Class *>(sexp_cpointer_value(this_ptr_sexp));
            auto fnc_ptr = static_cast<Return (Class::**) (Arg1)>(sexp_cpointer_value(sexp_opcode_data(self))); // Extract the function pointer from opcode data

            Chibi chibi(context);

            Return res = (this_ptr->**fnc_ptr)(
                ClassRegistratorHelpers<Class>::convert<Arg1>(chibi, arg1)
            );

            return chibi.make_from(res);
        }

        template<typename Return, typename Arg1, typename Arg2>
        static sexp call(sexp context, sexp self, long n, sexp this_ptr_sexp, sexp arg1, sexp arg2) {
            auto this_ptr = static_cast<Class *>(sexp_cpointer_value(this_ptr_sexp));
            auto fnc_ptr = static_cast<Return (Class::**) (Arg1, Arg2)>(sexp_cpointer_value(sexp_opcode_data(self)));

            Chibi chibi(context);

            Return res = (this_ptr->**fnc_ptr)(
                ClassRegistratorHelpers<Class>::convert<Arg1>(chibi, arg1),
                ClassRegistratorHelpers<Class>::convert<Arg2>(chibi, arg2)
            );

            return chibi.make_from(res);
        }
    private:
        /** Convert from sexp to the expected value type or throw an exception. */
        template<typename Res>
        static Res convert(Chibi &chibi, sexp s) {
            SExp se = chibi.make_SExp(s);

            if (auto r = se.to<Res>(); r.has_value()) {
                return *r;
            } else {
                throw std::runtime_error(se.dump_to_string() + " was not of type\n " + typeid(Res).name() + "\nwhen converting");
            }
        }
    };
}

template<typename Class>
Chibi::ClassRegistrator<Class>::ClassRegistrator(Chibi &chibi, std::string &name) : chibi(chibi), class_name(name) { }

template<typename Class>
template<typename Return, typename... Args>
Chibi::ClassRegistrator<Class> &Chibi::ClassRegistrator<Class>::register_method(std::string &&name, Return (Class::*member_function)(Args...)) {
    auto memfn_ptr = new decltype(member_function)(member_function);

    auto memfn_freeing_fun = +[](sexp ptr_to_free) {
                                  // Free the pointer we've created
                                  delete static_cast<decltype(member_function) *>(sexp_cpointer_value(ptr_to_free));
                              };

    SExp memfn_ptr_type = chibi.make_SExp(sexp_register_c_type(chibi.context, chibi.make_string(typeid(memfn_ptr).name()), memfn_freeing_fun));
    SExp wrapped_memfn_ptr = chibi.make_SExp(sexp_make_cpointer(chibi.context, sexp_type_tag(sexp(memfn_ptr_type)), memfn_ptr, SEXP_FALSE, 1));

    std::string full_name = class_name + "-" + name;
    chibi.register_function(full_name, ClassRegistratorHelpers<Class>::template call<Return, Args...>, wrapped_memfn_ptr);

    return *this;
}
