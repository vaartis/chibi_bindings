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

    /** Current context used in this instance. */
    sexp context;

    ~Chibi();
};
