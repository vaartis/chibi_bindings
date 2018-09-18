#include <chibi/eval.h>

class Chibi {
public:
    Chibi();
    ~Chibi();

private:
    sexp_struct x;
    sexp context;
    sexp env;
};
