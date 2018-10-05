#include "sexp.hpp"
#include "Chibi.hpp"

/** A record type descriptor wrapper for scheme records as defined in SRFI-99. */
class RTD {
public:
    /** Construct an RTD for a type named \a type_name */
    RTD(Chibi &chibi, const std::string &type_name);

    /** List the fields that the record has. */
    std::vector<Symbol> fields(bool with_parent = true);

    /** Check if the provided object is of this record type. */
    bool obj_is(SExp obj);

    /** Possible outcomes of RecordTypeDescriptor::set_field_for. */
    enum class SetFieldResult {
        WrongType /*< The object provided was not of this record type. */,
        NoField /*< There provided fields does not exist in this record type. */,
        Ok /*< Everything went ok and the value should now be set. */
    };
    /** Set the value of a field in this record type.
     *
     * @arg obj the object for which the field should be set
     * @arg field te field name to set
     * @arg value the field value to set
     */
    SetFieldResult set_field_for(SExp obj, std::string field, SExp value);

    /** Get the value of a field in this record type.
     *
     * @arg obj the object to get the value from
     * @arg field the field name to get the value from
     */
    std::optional<SExp> get_field_from(SExp obj, std::string field);

    /** The RTD's name. */
    std::string type_name;

private:
    Chibi &chibi;

    SExp rtd = chibi.env_ref(type_name);

    SExp is_rtd = chibi.env_ref("rtd?");
    SExp rtd_predicate = chibi.env_ref("rtd-predicate");
    
    SExp rtd_field_names = chibi.env_ref("rtd-field-names");
    SExp rtd_all_field_names = chibi.env_ref("rtd-all-field-names");
    
    SExp rtd_mutator = chibi.env_ref("rtd-mutator");
    SExp rtd_accessor = chibi.env_ref("rtd-accessor");
    
};
