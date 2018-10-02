#include <algorithm>

#include "RecordTypeDescriptor.hpp"

RecordTypeDescriptor::RecordTypeDescriptor(Chibi &chibi, const std::string &type_name) : chibi(chibi), type_name(type_name) {
    SExp is_type_rtd = is_rtd.apply(rtd).value();

    if (!is_type_rtd.to<bool>()) {
        throw std::invalid_argument(type_name + " is not a record type descriptor");
    }
}

std::vector<Symbol> RecordTypeDescriptor::fields(bool with_parent) {
    if (with_parent) {
        return rtd_all_field_names.apply(rtd)->to_vec_of<Symbol>().value();
    } else {
        return rtd_field_names.apply(rtd)->to_vec_of<Symbol>().value();
    }
}


bool RecordTypeDescriptor::obj_is(SExp obj) {
    auto predicate = rtd_predicate.apply(rtd); // Create the predicate function
    return predicate->apply(obj)->to<bool>().value();
}

RecordTypeDescriptor::SetFieldResult RecordTypeDescriptor::set_field_for(SExp obj, std::string field, SExp value) {
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

std::optional<SExp> RecordTypeDescriptor::get_field_from(SExp obj, std::string field) {
    if (obj_is(obj)) {
        auto all_fields = fields();

        if (std::find(all_fields.begin(), all_fields.end(), Symbol(field)) != all_fields.end()) {
            auto accessor = rtd_accessor.apply(rtd, chibi.make_symbol(field)); // Create an accessor
            return accessor->apply(obj);
        }
    }

    return std::nullopt;
}
