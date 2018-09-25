#include <optional>

#include <chibi/eval.h>

class Chibi {
public:
    Chibi();

    /** Prints a chibi exception to the specified port.
     *
     * @throws std::invalid_argument if `exception` is not an exception
     */
    void print_exception(sexp exception, std::optional<sexp> port = std::nullopt);

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
