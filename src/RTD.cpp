#include <algorithm>

#include "RTD.hpp"
#include "SExpConverters.hpp"

RTD::RTD(Chibi &chibi, const std::string &type_name) : chibi(chibi), type_name(type_name) {
    rtd = chibi.env_ref(type_name);

    SExp is_type_rtd = is_rtd.apply(rtd).value();
    if (!FromSExp<bool>::from(chibi, is_type_rtd).value()) {
        throw std::invalid_argument(type_name + " is not a record type descriptor");
    }
}

RTD::RTD(Chibi &chibi, SExp sxp) : chibi(chibi) {
    rtd = sxp;

    SExp is_type_rtd = is_rtd.apply(rtd).value();
    if (!FromSExp<bool>::from(chibi, is_type_rtd).value()) {
        throw std::invalid_argument("A sexp that is not a record type descriptor was passed to RTD");
    }

    type_name = FromSExp<Symbol>::from(chibi, rtd_name.apply(sxp).value()).value();
}

bool RTD::operator==(const RTD &other) {
    return rtd == other.rtd;
}

std::vector<Symbol> RTD::fields(bool with_parent) {
    if (with_parent) {
        return FromSExp<std::vector<Symbol>>::from(chibi, rtd_all_field_names.apply(rtd).value()).value();
    } else {
        return FromSExp<std::vector<Symbol>>::from(chibi, rtd_field_names.apply(rtd).value()).value();
    }
}


bool RTD::obj_is(SExp obj) {
    auto predicate = rtd_predicate.apply(rtd); // Create the predicate function
    return FromSExp<bool>::from(chibi, predicate->apply(obj).value()).value();
}

RTD::SetFieldResult RTD::set_field_for(SExp obj, std::string field, SExp value) {
    if (obj_is(obj)) { // Is it of the needed type
        auto all_fields = fields();

        if (std::find(all_fields.begin(), all_fields.end(), Symbol(field)) != all_fields.end()) {
            auto mutator = rtd_mutator.apply(rtd, chibi.make_symbol(field)); // Create a mutator
            mutator->apply(obj, value);

            return SetFieldResult::Ok;
        }

        return SetFieldResult::NoField;
    }

    return SetFieldResult::WrongType;
}

std::optional<SExp> RTD::get_field_from(SExp obj, std::string field) {
    if (obj_is(obj)) {
        auto all_fields = fields();

        if (std::find(all_fields.begin(), all_fields.end(), Symbol(field)) != all_fields.end()) {
            auto accessor = rtd_accessor.apply(rtd, chibi.make_symbol(field)); // Create an accessor
            return accessor->apply(obj);
        }
    }

    return std::nullopt;
}
