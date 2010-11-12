// bdem_fieldspec.h                                                   -*-C++-*-
#ifndef INCLUDED_BDEM_FIELDSPEC
#define INCLUDED_BDEM_FIELDSPEC

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a specification for the type of a field.
//
//@DEPRECATED: Use 'bdem_fielddefattributes' or 'bdem_fielddef' instead.
//
//@CLASSES:
//  bdem_FieldSpec: specification for the type of a field
//
//@SEE_ALSO: bdem_fielddefattributes, bdem_schema
//
//@AUTHOR: Tom Marshall, Henry Verschell
//
//@DESCRIPTION: [!DEPRECATED!] This component provides a type,
// 'bdem_FieldSpec', which is used to describe a field.  A field, in this
// context, is a single value of a type supported by the 'bdem' package (see
// 'bdem_elemtype'), generally in the context of a record (or sequence of
// fields).  A 'bdem_FieldSpec' is an *in-core* value-semantic type with
// properties describing the data type, default value, nullability, formatting
// mode, and constraint of a field.  The 'elemType' of a field spec defines
// the data type of the field.  The 'isNullable' flag indicates whether values
// of the field may be null.  The 'formattingMode' is one of the enumerated
// mode values defined by 'bdeat_FormattingMode' (e.g., 'BDEAT_DEC',
// 'BDEAT_HEX', 'BDEAT_BASE64', etc).  The 'formattingMode' can be used to
// determine how a field's value should be written as text (e.g., if the value
// were being serialized as XML).  By default the formatting-mode is
// 'BDEAT_DEFAULT'.  Finally, a field specification contains an optional
// constraint for a field specification, may either be 0, a record definition,
// or an enumeration definition.
//
///Field Constraint
///----------------
// A field specifications's constraint indicates a constraint on the values of
// the type of field specified.  The constraint of a field specification may
// be 0, in which case the field's of that type are unconstrainted (i.e., they
// may have any value of the specified data type).  Field specifications of
// type 'CHOICE', 'CHOICE_ARRAY', 'LIST', and 'TABLE' can be constrained by a
// record definition (i.e., a 'bdem_RecordDef').  For a 'CHOICE' and
// 'CHOICE_ARRAY' the constraining record definition must be of type
// 'bdem_RecordDef::BDEM_CHOICE_RECORD', and the constraint indicates the
// selection of types that can be made for the specified field.   For a 'LIST'
// or 'TABLE' the constraining record definition must be of type
// 'bdem_RecordDef::BDEM_SEQUENCE_RECORD', and the constraint indicates the
// types of the sub-fields contained within the specified aggregate field.
// A 'bdem_FieldSpec' does *not* validate that supplied record constraints are
// of the correct record type.  Field specifications of type 'STRING', 'INT',
// 'STRING_ARRAY', or 'INT_ARRAY', can be constrained by a enumeration
// definition (i.e., a 'bdem_EnumerationDef').  The enumeration constraint
// indicates the set of valid integer or string values to which fields of that
// type specified can be set.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_ENUMERATIONDEF
#include <bdem_enumerationdef.h>
#endif

#ifndef INCLUDED_BDEM_FIELDDEFATTRIBUTES
#include <bdem_fielddefattributes.h>
#endif

#ifndef INCLUDED_BDEAT_FORMATTINGMODE
#include <bdeat_formattingmode.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

class bdem_RecordDef;
class bdem_EnumerationDef;

                         // ====================
                         // class bdem_FieldSpec
                         // ====================

class bdem_FieldSpec {
    // [!DEPRECATED!] This simply constrained, *in-process* *value-semantic*
    // attribute class comprises the fixed set of individual values specify
    // the value of a field definition for a record in a 'bdem' schema.  A
    // field is fully specified by:
    //..
    //  (1) its 'fieldAttributes' (i.e., a 'bdem_FieldDefAttributes'
    //      value).
    //  (2) a 'constraint', which is either 0 or, optionally, if 'elemType'
    //      is an aggregate type -- i.e., 'CHOICE', 'CHOICE_ARRAY', 'LIST',
    //      or 'TABLE' -- the address of a non-modifiable record definition
    //      that serves to constrain that aggregate to a fixed structure, or,
    //      optionally, if 'elemType' is 'INT', 'STRING', 'INT_ARRAY', or
    //      'STRING_ARRAY', a non-modifiable enumeration definition
    //      specifying possible valid values for the field
    //..
    // Constraints are optional, but can be applied to only 'CHOICE',
    // 'CHOICE_ARRAY', 'LIST', 'TABLE', 'INT', 'STRING', 'INT_ARRAY', and
    // 'STRING_ARRAY' data types.  If no constraint exists, then that object
    // is *unconstrained* and may have any structure or contents.  If not
    // specified during construction, the formatting-mode, nullabilityFlag,
    // 'hasDefaultValueFlag' attributes of a field spec have the values
    // 'bdeat_FormattingMode::DEFAULT', 'false' and 'false', respectively.
    //
    // More generally, this class supports a complete set of *in-process*
    // *value* *semantic* operations, including copy construction, assignment,
    // equality comparison, and 'ostream' printing, however 'bdex'
    // serialization is not supported.  (A precise operational definition of
    // when two objects have the same value can be found in the description
    // of 'operator==' for the class.)  This class is *exception* *neutral*
    // with no guarantee of rollback: if an exception is thrown during the
    // invocation of a method on a pre-existing object, the object is left in
    // a valid state, but its value is undefined.  In no event is memory
    // leaked.  Finally, *aliasing* (e.g., using all or part of an object as
    // both source and destination) is supported in all cases.
    //
    // Note that this type has been deprecated, clients should instead use
    // either 'bdem_fielddefattributes' or 'bdem_fielddef'.

   // DATA
    bdem_FieldDefAttributes  d_attributes;  // type of the specified field

    union {
        const bdem_RecordDef      *d_recordConstraint_p;
            // constraint on choice, choice array, list, or table.
            // (held, not owned)

        const bdem_EnumerationDef *d_enumConstraint_p;
            // Enumeration constraint on string or int.  (held, not owned.)
    };

    // FRIENDS
    friend bool operator==(const bdem_FieldSpec&, const bdem_FieldSpec&);

  public:
    // TYPES
    struct LowPrecedenceEnumPtr {
        // We use this struct instead of a 'bdem_EnumerationDef *' in an
        // overloaded function signature to address overloading ambiguities
        // that would otherwise arise from 'bdem_EnumerationDef *' and
        // 'bdem_RecordDef *' when a literal 0 is passed in as the argument.
        const bdem_EnumerationDef *d_ptr;
        LowPrecedenceEnumPtr(const bdem_EnumerationDef *p) : d_ptr(p) { }
    };

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdem_FieldSpec,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit
    bdem_FieldSpec(bdem_ElemType::Type  type = bdem_ElemType::BDEM_VOID,
                   bslma_Allocator     *basicAllocator = 0);
        // Create a field specification.  Optionally specify the 'type' of the
        // field.  Optionally specify the 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  After construction, 'formattingMode()' returns
        // 0, 'isNullable()' and 'hasDefaultValue()' both return 'false', and
        // 'recordConstraint' and 'enumerationConstraint' both return 0.

    bdem_FieldSpec(bdem_ElemType::Type   type,
                   const bdem_RecordDef *constraint,
                   bslma_Allocator      *basicAllocator = 0);
        // Create a field spec having the specified 'type' and
        // 'constraint'.  Optionally specify the 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  If 'type' is an aggregate -- i.e.,
        // 'CHOICE', 'CHOICE_ARRAY', 'LIST', or 'TABLE' -- optionally specify
        // a 'constraint'.  The behavior is undefined unless one of the
        // following is 'true':
        //..
        //  (1) 'constraint' is 0,
        //  (2) 'type' is 'CHOICE' or 'CHOICE_ARRAY' and
        //      'constraint' is of record type
        //      'bdem_RecordDef::BDEM_CHOICE_RECORD', or
        //  (3) 'type' is 'LIST' or 'TABLE' and 'constraint' is of record
        //      type 'bdem_RecordDef::BDEM_SEQUENCE_RECORD'.
        //..
        // After construction, 'formattingMode()' returns 0, and 'isNullable()'
        // and 'hasDefaultValue()' both return 'false'.

    bdem_FieldSpec(bdem_ElemType::Type   type,
                   LowPrecedenceEnumPtr  constraint,
                   bslma_Allocator      *basicAllocator = 0);
        // Create a field spec having the specified 'type'.  If 'type' is
        // 'STRING', 'INT', 'STRING_ARRAY' or 'INT_ARRAY', optionally specify
        // an enumeration 'constraint'.  Optionally specify the
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless either 'constraint' is 0 or 'type' is 'STRING' or
        // 'INT' or the respective array types.  After construction,
        // 'formattingMode()' returns 0, and 'isNullable()' and
        // 'hasDefaultValue()' both return 'false'.

    bdem_FieldSpec(bdem_ElemType::Type   type,
                   const bdem_RecordDef *constraint,
                   int                   formattingMode,
                   bool                  nullabilityFlag,
                   bslma_Allocator      *basicAllocator = 0);
    bdem_FieldSpec(bdem_ElemType::Type   type,
                   LowPrecedenceEnumPtr  constraint,
                   int                   formattingMode,
                   bool                  nullabilityFlag,
                   bslma_Allocator      *basicAllocator = 0);
        // Create a field spec having the specified 'type', 'constraint',
        // 'formattingMode', and 'nullabilityFlag' attributes, respectively.
        // The behavior is undefined unless one of the following is true: (1)
        // 'constraint' is 0, (2) 'type' is 'CHOICE' or 'CHOICE_ARRAY' and
        // 'constraint' is a 'bdem_RecordDef' with record type
        // 'BDEM_CHOICE_RECORD', (3) 'type' is 'LIST' or 'TABLE' and the type
        // of 'constraint' is a 'bdem_RecordDef' with record type
        // 'BDEM_SEQUENCE_RECORD' or (4) 'type' is 'STRING', 'INT',
        // 'STRING_ARRAY', or 'INT_ARRAY' and 'constraint' is a
        // 'bdem_EnumerationDef'.  After construction 'isNullable()' has
        // the value passed to 'nullabilityFlag' and 'hasDefaultValue()'
        // returns 'false'.

    bdem_FieldSpec(const bdem_FieldSpec&  original,
                   bslma_Allocator       *basicAllocator = 0);
        // Create a field spec having the value of the specified 'original'
        // field spec.

    ~bdem_FieldSpec();
        // Destroy this field spec.

    // MANIPULATORS
    bdem_FieldSpec& operator=(const bdem_FieldSpec& rhs);
        // Assign to this field spec the value of the specified 'rhs' field
        // spec, and return a reference to this modifiable field spec.

    bdem_ElemRef defaultValue();
        // Return a 'bdem' element reference to the modifiable default value
        // of this field spec.  If 'hasDefaultValue()' returns 'false', an
        // element reference to the appropriate null value is returned.  If
        // the element type of this field spec is an array of scalars, then
        // the default value will be of the corresponding scalar type.  The
        // behavior is undefined unless the type of this field spec is not of
        // aggregate type.

    int reset(bdem_ElemType::Type   type,
              const bdem_RecordDef *constraint = 0);
    int reset(bdem_ElemType::Type   type,
              LowPrecedenceEnumPtr  constraint);
        // Reset this field spec to have the specified 'type'.  Optionally
        // specify a 'constraint'.  Return 0 on success, and a
        // non-zero value unless either:
        //..
        //  (1) 'constraint' is 0,
        //  (2) 'type' is 'CHOICE', 'CHOICE_ARRAY', 'LIST', or 'TABLE' and
        //      'constraint' is a 'bdem_RecordDef'.
        //  (3) 'type' is 'STRING', 'INT', 'STRING_ARRAY', or 'INT_ARRAY'
        //      and 'constraint' is a 'bdem_EnumerationDef' (wrapped in a
        //      'LowPrecedenceEnumPtr').
        //..
        // After a 'reset' 'formattingMode()' returns 0, 'isNullable()'
        // and 'hasDefaultValue()' both return 'false'.  The behavior is
        // undefined unless the record constraint, if supplied, is of a valid
        // record type for 'type' (see 'bdem_recorddef' for more information
        // on validating record constraints).

    int reset(bdem_ElemType::Type   type,
              const bdem_RecordDef *constraint,
              int                   formattingMode,
              bool                  nullabilityFlag);
    int reset(bdem_ElemType::Type   type,
              LowPrecedenceEnumPtr  constraint,
              int                   formattingMode,
              bool                  nullabilityFlag);
        // Reset this field spec to have the specified 'type', 'constraint',
        // 'formattingMode', and 'nullabilityFlag' attributes.  Return 0 on
        // success, and a non-zero value unless either:
        //..
        //  (1) 'constraint' is 0,
        //  (2) 'type' is 'CHOICE', 'CHOICE_ARRAY', 'LIST', or 'TABLE' and
        //     'constraint' is a 'bdem_RecordDef'.
        //  (3) 'type' is 'STRING', 'INT', 'STRING_ARRAY', or 'INT_ARRAY'
        //      and 'constraint' is a 'bdem_EnumerationDef' (wrapped in a
        //      'LowPrecedenceEnumPtr').
        //..
        // After a 'reset' 'hasDefaultValue()' returns 'false'.  The behavior
        // is undefined unless the record constraint, if supplied, is of a
        // valid record type for 'type' (see 'bdem_recorddef' for more
        // information on validating record constraints).

    int setConstraint(const bdem_RecordDef *constraint);
    int setConstraint(LowPrecedenceEnumPtr  constraint);
        // Set the constraint of this field spec to the specified 'constraint'.
        // Return 0 on success, and a non-zero value unless either:
        //..
        //  (1) 'constraint' is 0,
        //  (2) 'type' is 'CHOICE', 'CHOICE_ARRAY', 'LIST', or 'TABLE' and
        //      'constraint' is a 'bdem_RecordDef'.
        //  (3) 'type' is 'STRING', 'INT', 'STRING_ARRAY', or 'INT_ARRAY'
        //      and 'constraint' is a 'bdem_EnumerationDef' (wrapped in a
        //      'LowPrecedenceEnumPtr').
        //..
        // The behavior is undefined unless the record constraint, if
        // supplied, is of a valid record type for 'type' (see
        // 'bdem_recorddef' for more information on validating record
        // constraints).

    void setFormattingMode(int formattingMode);
        // Set the formatting-mode of this field spec to the specified
        // 'formattingMode' value.  Return 0 on success, and a non-zero value
        // otherwise.  The behavior is undefined unless 'formattingMode' is a
        // valid mode defined by 'bdeat_FormattingMode'.

    void setIsNullable(bool value);
        // Set the nullability attribute of this field spec to the specified
        // 'value' value.

    // ACCESSORS
    const bdem_RecordDef *constraint() const;
        // DEPRECATED: Use 'recordConstraint' or 'enumerationConstraint'.
        // Return 'recordConstraint()'.

    const bdem_RecordDef *recordConstraint() const;
        // Return the address of this field spec's non-modifiable record
        // constraint if it exists, and 0 otherwise.  Note that record
        // constraints may exist only for aggregate types -- i.e., 'CHOICE',
        // 'CHOICE_ARRAY', 'LIST', and 'TABLE'.

    const bdem_EnumerationDef *enumerationConstraint() const;
        // Return the address of this field spec's non-modifiable enumeration
        // constraint if it exists, and 0 otherwise.  Note that enumeration
        // constraints may exist only for enumerable types -- i.e., 'STRING',
        // 'INT', 'STRING_ARRAY', and 'INT_ARRAY'.

    bdem_ConstElemRef defaultValue() const;
        // Return an element reference to the non-modifiable default value of
        // this field spec.  If 'hasDefaultValue()' returns 'false', an element
        // reference to the appropriate null value is returned.  If the element
        // type of this field spec is an array of scalars, then the default
        // value will be of the corresponding scalar type.  The behavior is
        // undefined unless the type of this field spec is not of aggregate
        // type.

    bdem_ElemType::Type elemType() const;
        // Return the 'bdem' element type of a field described by this spec.

    const bdem_FieldDefAttributes& fieldAttributes() const;
        // Return a reference to the non-modifiable field type of fields
        // described by this spec.

    int formattingMode() const;
        // Return the formatting-mode of fields described by this spec.  The
        // returned value will be one of the mode values defined by
        // 'bdeat_FormattingMode' (e.g., 'BDEAT_DEFAULT', 'BDEAT_DEC',
        // 'BDEAT_HEX', etc).  Note that this value can be used to determine
        // how a field's value should be written to text.

    bool hasDefaultValue() const;
        // Return 'true' if this field spec has a default value, and 'false'
        // otherwise.  Note that 'false' is returned if 'elemType()' is an
        // aggregate type.

    bool isNullable() const;
        // Return 'true' if a field described by this spec may have a null
        // value, and 'false' otherwise.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the
        // (absolute value of) the optionally specified indentation 'level'
        // and return a reference to 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one line,
        // suppressing all but the initial indentation (as governed by the
        // product of 'level' and (the absolute value of) 'spacesPerLevel').
        // If 'stream' is not valid on entry, this operation has no effect.
};

// FREE OPERATORS
bool operator==(const bdem_FieldSpec& lhs, const bdem_FieldSpec& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' field specs have the same
    // value, and 'false' otherwise.  Two field specs have the same value if
    // they have (1) the same field type, (2) (identically) the same constraint
    // (or are both unconstrained), (3) the same formatting-mode, (4) the
    // same nullability value, and (5) in the case of 'bdem' scalar and array
    // types, have the same default value (or both have no default value).

inline
bool operator!=(const bdem_FieldSpec& lhs, const bdem_FieldSpec& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' field specs do not have
    // the same value, and 'false' otherwise.  Two field specs do not have the
    // same value if they differ in field type, constraint, formatting-mode,
    // nullability, or, in the case of 'bdem' scalar and array types, default
    // value.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // --------------------
                        // class bdem_FieldSpec
                        // --------------------


// CREATORS
inline
bdem_FieldSpec::bdem_FieldSpec(bdem_ElemType::Type  type,
                               bslma_Allocator     *basicAllocator)
: d_attributes(type, basicAllocator)
, d_recordConstraint_p(0)
{
}

inline
bdem_FieldSpec::bdem_FieldSpec(bdem_ElemType::Type   type,
                               const bdem_RecordDef *constraint,
                               bslma_Allocator      *basicAllocator)
: d_attributes(type, basicAllocator)
, d_recordConstraint_p(constraint)
{
}

inline
bdem_FieldSpec::bdem_FieldSpec(bdem_ElemType::Type   type,
                               LowPrecedenceEnumPtr  constraint,
                               bslma_Allocator      *basicAllocator)
: d_attributes(type, basicAllocator)
, d_enumConstraint_p(constraint.d_ptr)
{
}

inline
bdem_FieldSpec::bdem_FieldSpec(bdem_ElemType::Type   type,
                               const bdem_RecordDef *constraint,
                               int                   formattingMode,
                               bool                  nullabilityFlag,
                               bslma_Allocator      *basicAllocator)
: d_attributes(type, nullabilityFlag, formattingMode, basicAllocator)
, d_recordConstraint_p(constraint)
{
}

inline
bdem_FieldSpec::bdem_FieldSpec(bdem_ElemType::Type   type,
                               LowPrecedenceEnumPtr  constraint,
                               int                   formattingMode,
                               bool                  nullabilityFlag,
                               bslma_Allocator      *basicAllocator)
: d_attributes(type, nullabilityFlag, formattingMode, basicAllocator)
, d_enumConstraint_p(constraint.d_ptr)
{
}

inline
bdem_FieldSpec::bdem_FieldSpec(const bdem_FieldSpec&  original,
                               bslma_Allocator       *basicAllocator)
: d_attributes(original.d_attributes, basicAllocator)
, d_recordConstraint_p(original.d_recordConstraint_p)
{
}

inline
bdem_FieldSpec::~bdem_FieldSpec()
{
}

// MANIPULATORS
inline
bdem_FieldSpec& bdem_FieldSpec::operator=(const bdem_FieldSpec& rhs)
{
    d_attributes          = rhs.d_attributes;
    d_recordConstraint_p = rhs.d_recordConstraint_p;
    return *this;
}

inline
bdem_ElemRef bdem_FieldSpec::defaultValue()
{
    return d_attributes.defaultValue();
}

inline
int bdem_FieldSpec::reset(bdem_ElemType::Type   type,
                          const bdem_RecordDef *constraint)
{
    return reset(type,
                 constraint,
                 bdeat_FormattingMode::BDEAT_DEFAULT,
                 false);
}

inline
int bdem_FieldSpec::reset(bdem_ElemType::Type  type,
                          LowPrecedenceEnumPtr constraint)
{
    return reset(type,
                 constraint,
                 bdeat_FormattingMode::BDEAT_DEFAULT,
                 false);
}

inline
void bdem_FieldSpec::setFormattingMode(int formattingMode)
{
    d_attributes.setFormattingMode(formattingMode);
}

inline
void bdem_FieldSpec::setIsNullable(bool value)
{
    d_attributes.setIsNullable(value);
}

// ACCESSORS
inline
const bdem_RecordDef *bdem_FieldSpec::recordConstraint() const
{
    return bdem_ElemType::isAggregateType(elemType()) ? d_recordConstraint_p
                                                      : 0;
}

inline
const bdem_EnumerationDef *
bdem_FieldSpec::enumerationConstraint() const
{
    return bdem_EnumerationDef::canHaveEnumConstraint(elemType())
         ? d_enumConstraint_p
         : 0;
}

inline
const bdem_RecordDef *bdem_FieldSpec::constraint() const
{
    return recordConstraint();
}

inline
bdem_ConstElemRef bdem_FieldSpec::defaultValue() const
{
    return d_attributes.defaultValue();
}

inline
bdem_ElemType::Type bdem_FieldSpec::elemType() const
{
    return d_attributes.elemType();
}

inline
int bdem_FieldSpec::formattingMode() const
{
    return d_attributes.formattingMode();
}

inline
bool bdem_FieldSpec::hasDefaultValue() const
{
    return d_attributes.hasDefaultValue();
}

inline
bool bdem_FieldSpec::isNullable() const
{
    return d_attributes.isNullable();
}

inline
const bdem_FieldDefAttributes& bdem_FieldSpec::fieldAttributes() const
{
    return d_attributes;
}

// FREE OPERATORS
inline
bool operator==(const bdem_FieldSpec& lhs, const bdem_FieldSpec& rhs)
{
    return lhs.d_attributes     == rhs.d_attributes
        && lhs.constraint()     == rhs.constraint()
        && lhs.formattingMode() == rhs.formattingMode();
}

inline
bool operator!=(const bdem_FieldSpec& lhs, const bdem_FieldSpec& rhs)
{
    return !(lhs == rhs);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
