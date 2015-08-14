// bdlmxxx_fielddef.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLMXXX_FIELDDEF
#define INCLUDED_BDLMXXX_FIELDDEF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class representing the definition for a single field.
//
//@CLASSES:
//  bdlmxxx::FieldDef: definition for a single field
//
//@SEE_ALSO: bdlmxxx_fielddefattributes, bdlmxxx_schema
//
//@AUTHOR: Tom Marshall, Henry Verschell
//
//@DESCRIPTION: This component implements a collaborative type,
// 'bdlmxxx::FieldDef', that is an integral part of a fully-value semantic schema
// object, and is used to define a single field.  A field, in this context, is
// a value of one of the types supported by the 'bdem' package (see
// 'bdlmxxx_elemtype'), generally in the context of a record (or sequence of
// fields).  A 'bdlmxxx::FieldDef' has attributes describing the data type, default
// value, nullability, formatting mode, and constraint of a field.  In general,
// clients should *not* construct a 'bdlmxxx::FieldDef' directly, but instead
// obtain a field definition from a 'bdlmxxx::RecordDef' (see 'bdlmxxx_recorddef' and
// 'bdlmxxx_schema' for more information).  The 'elemType' of a field definition
// identifies the data type of the field.  The 'isNullable' flag indicates
// whether values of the field may be null.  The 'formattingMode' is one of the
// enumerated mode values defined by 'bdeat_FormattingMode' (e.g., 'BDEAT_DEC',
// 'BDEAT_HEX', 'BDEAT_BASE64', etc.).  The 'formattingMode' can be used to
// determine how a field's value should be written as text (e.g., if the value
// were being serialized as XML).  Finally, a field definition contains an
// optional constraint, which may be either 0, a record definition, or an
// enumeration definition.
//
///Field Constraints
///-----------------
// A field definition's constraint, if one is specified, indicates a
// constraint on the values of the type of field being defined.  The
// constraint of a field definition may be 0, in which case the field's of
// that type are unconstrainted (i.e., they may have any value of the
// specified data type).  Field definitions of type 'CHOICE', 'CHOICE_ARRAY',
// 'LIST', and 'TABLE' can be constrained by a record definition (i.e., a
// 'bdlmxxx::RecordDef').  For a 'CHOICE' and 'CHOICE_ARRAY' the constraining
// record definition must be of type 'bdlmxxx::RecordDef::BDEM_CHOICE_RECORD', and
// the constraint indicates the selection of types that can be made for the
// defined field.  For a 'LIST' or 'TABLE' the constraining record definition
// must be of type 'bdlmxxx::RecordDef::BDEM_SEQUENCE_RECORD', and the constraint
// indicates the types of the sub-fields contained within the defined
// aggregate field.  Field definitions of type 'STRING', 'INT',
// 'STRING_ARRAY', or 'INT_ARRAY', can be constrained by a enumeration
// definition (i.e., a 'bdlmxxx::EnumerationDef').  The enumeration constraint
// indicates the set of valid integer or string values to which fields of those
// types can be set.  Note that 'setConstraint' does not validate whether the
// supplied constraint is valid for the type of field.
//
///Usage
///-----
// The following usage examples demonstrate how to construct, manipulate, and
// access a field definition in isolation.  In general, clients should *not*
// create a 'bdlmxxx::FieldDef' directly, but obtain one from a record definition
// (see 'bdlmxxx_recorddef').
//
///Directly Creating a 'bdlmxxx::FieldDef' (*Not Recommended*)
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates how to create a 'bdlmxxx::FieldDef' in
// isolation -- this is *not* the recommended usage of 'bdlmxxx::FieldDef':
// Clients should obtain one from a record definition (see 'bdlmxxx_recorddef').
// For the purpose of this usage example, we define a dummy "stand-in" types
// for 'bdlmxxx::RecordDef' and 'bdlmxxx::Schema'.  These types are used *in* *name*
// *only* by 'bdlmxxx::FieldDef', so no particular definition is necessary:
//..
//  namespace BloombergLP {
//
//  class bdlmxxx::RecordDef {
//      // Dummy type.
//  };
//
//  class bdlmxxx::Schema {
//      // Dummy type.
//  };
//
//  }
//..
// Next we create a couple field definition objects:
//..
//  bslma::Allocator          *allocator = bslma::Default::allocator();
//  bdlma::SequentialAllocator  seqAllocator(allocator);
//
//  bdlmxxx::FieldDefAttributes  intAttr(bdlmxxx::ElemType::BDEM_INT,  allocator);
//  bdlmxxx::FieldDefAttributes listAttr(bdlmxxx::ElemType::BDEM_LIST, allocator);
//
//  bdlmxxx::FieldDef  intField("intField",  5,  intAttr, allocator);
//  bdlmxxx::FieldDef listField("listField", 2, listAttr, allocator);
//..
// We now provide constraints for these fields.  Note that 'bdlmxxx::RecordDef'
// and 'bdlmxxx::Schema' are the dummy place-holder types defined above.  Clients
// should refer to the documentation of 'bdlmxxx_schema' for more information on
// specifying field constraints.
//..
//  bdlmxxx::Schema         schema;
//  bdlmxxx::EnumerationDef enumerationDef(&schema, 0, "enum", &seqAllocator);
//  bdlmxxx::RecordDef      recordConstraint;
//
//  intField.setConstraint(&enumerationConstraint);
//  listField.setConstraint(&recordConstraint);
//..
//
///Accessing a 'bdlmxxx::FieldDef'
///- - - - - - - - - - - - - -
// In this example, we demonstrate how to access the properties of a
// 'bdlmxxx::FieldDef' object.  The two objects, 'intField' and 'listField' were
// defined in the preceding example:
//..
//  assert(0 == bsl::strcmp("intField",  intField.fieldName()));
//  assert(0 == bsl::strcmp("listField", listField.fieldName()));
//
//  assert(5 == intField.fieldId());
//  assert(2 == listField.fieldId());
//..
// An enumeration definition constraint can be provided for fields of type
// 'BDEM_INT', 'BDEM_STRING', 'BDEM_INT_ARRAY', and 'BDEM_STRING_ARRAY':
//..
//  assert(&enumerationConstraint == intField.enumerationConstraint());
//  assert(0                      == listField.enumerationConstraint());
//..
// A record definition constraint can be provided for fields of an aggregate
// type ('BDEM_LIST', 'BDEM_CHOICE', 'BDEM_TABLE', 'BDEM_CHOICE_ARRAY'):
//..
//  assert(0                 == intField.recordConstraint());
//  assert(&recordConstraint == listField.recordConstraint());
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMREF
#include <bdlmxxx_elemref.h>
#endif

#ifndef INCLUDED_BDLMXXX_ELEMTYPE
#include <bdlmxxx_elemtype.h>
#endif

#ifndef INCLUDED_BDLMXXX_FIELDDEFATTRIBUTES
#include <bdlmxxx_fielddefattributes.h>
#endif

#ifndef INCLUDED_BDLMXXX_FIELDSPEC
#include <bdlmxxx_fieldspec.h>
#endif

#ifndef INCLUDED_BDLMXXX_ENUMERATIONDEF
#include <bdlmxxx_enumerationdef.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

namespace BloombergLP {


namespace bdlmxxx {class RecordDef;
class EnumerationDef;

                          // ===================
                          // class FieldDef
                          // ===================

class FieldDef {
    // This collaborative class represents the definition for a single field,
    // and is an integral part of a fully value-semantic schema object.  A
    // field definition has attributes specifying the data type, nullability,
    // default value, and formatting for the field being defined.  In
    // addition, a field definition optionally contains a name and,
    // independently, optionally contains a constraint.  In general, clients
    // should *not* construct a 'FieldDef' directly, but instead obtain a
    // field definition from a 'RecordDef' (see 'bdlmxxx_recorddef' and
    // 'bdlmxxx_schema' for more information).  The 'name' of a field
    // definition is supplied at construction and may be 0.  A field
    // definition provides manipulators to set the constraint of a field.
    // These manipulators do *not* validate the values provided (see the
    // documentation for 'bdlmxxx_recorddef' for information on validating
    // constraints).

    // DATA
    FieldDefAttributes  d_attributes;  // field type

    const char              *d_name_p;      // name, if any (held, not owned)

    int                      d_id;          // field id, if any

    union {
        const RecordDef      *d_recordConstraint_p;
                                            // constraint on choice, choice
                                            // array, list, or table (held,
                                            // not owned)

        const EnumerationDef *d_enumConstraint_p;
                                            // enumeration constraint on
                                            // string or int (held, not
                                            // owned)
    };

  private:
    // NOT IMPLEMENTED
    FieldDef(const FieldDef&);
    FieldDef& operator=(const FieldDef&);

  public:
    // TYPES
    struct LowPrecedenceEnumPtr {
        // We use this struct instead of a 'EnumerationDef *' in an
        // overloaded function signature to address overloading ambiguities
        // that would otherwise arise from 'EnumerationDef *' and
        // 'RecordDef *' when a literal 0 is passed in as the argument.

        const EnumerationDef *d_ptr;
        LowPrecedenceEnumPtr(const EnumerationDef *p)
        : d_ptr(p) { }
    };

    // CREATORS
    FieldDef(const char                     *name,
                  int                             id,
                  const FieldDefAttributes&  attributes,
                  bslma::Allocator               *basicAllocator);
        // Create a field definition having the specified 'name', id', and
        // 'attributes', using the specified 'basicAllocator' to supply memory.
        // If 'name' is 0, the field definition is unnamed.  Note that
        // clients, in general, should *not* construct a 'FieldDef'
        // directly (see 'bdlmxxx_recorddef' and 'bdlmxxx_schema').  The behavior is
        // undefined if 'basicAllocator' is 0.

    ~FieldDef();
        // Destroy this field definition.

    // MANIPULATORS
    void setIsNullable(bool value);
        // Set the nullability attribute of this field definition to the
        // specified 'value'.

    void setConstraint(const RecordDef *constraint);
        // Set the constraint of this field definition to the specified
        // *record* 'constraint'.  The behavior is undefined unless 'elemType'
        // is an aggregate type -- i.e., 'LIST', 'TABLE, 'CHOICE', or
        // 'CHOICE_ARRAY'.

    void setConstraint(const EnumerationDef *constraint);
        // Set the constraint of this field definition to the specified
        // *enumeration* 'constraint'.   The behavior is undefined unless
        // 'elemType' is an enumerable type -- i.e., 'INT''STRING',
        // 'INT_ARRAY', or 'STRING_ARRAY'.

    // ACCESSORS
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    const RecordDef *constraint() const;
        // Return the same result as that returned by 'recordConstraint'.
        //
        // DEPRECATED: Use 'recordConstraint' or 'enumerationConstraint'.
#endif // BDE_OMIT_INTERNAL_DEPRECATED

    const EnumerationDef *enumerationConstraint() const;
        // Return the address of this field's non-modifiable enumeration
        // constraint, and 0 if no such constraint exists.  Note that
        // enumeration constraints may exist for only enumerable types --
        // i.e., 'STRING', 'INT', 'STRING_ARRAY', and 'INT_ARRAY' types.

    const RecordDef *recordConstraint() const;
        // Return the address of this field's non-modifiable record
        // constraint, and 0 if no such constraint exists.  Note that record
        // constraints may exist for only aggregate types -- i.e., 'CHOICE',
        // 'CHOICE_ARRAY', 'LIST', and 'TABLE' types.

    ConstElemRef defaultValue() const;
        // Return a 'bdem' const-element reference to the default value
        // attribute of this object.  If 'hasDefaultValue' would have initially
        // returned 'false', a valid element reference to the default-value
        // object of the appropriate type (having the null value) is returned.
        // If the element type of this attribute is an array of scalars, then
        // the default value will be of the corresponding scalar type.  The
        // behavior is undefined if the type of this field definition is an
        // aggregate type -- i.e., 'LIST', 'TABLE', 'CHOICE', or
        // 'CHOICE_ARRAY'.  Note that the default value is provided, at
        // construction, via a 'FieldDefAttributes' object.

    ElemType::Type elemType() const;
        // Return the 'bdem' element type of a field described by this
        // definition.

    FieldSpec fieldSpec() const;
        // Return the value of the field spec for this field definition.
        //
        // DEPRECATED: Access the properties directly from this field
        // definition instead.

    int fieldId() const;
        // Return the id of this field definition supplied at construction.
        // See 'bdlmxxx_recorddef' and 'bdlmxxx_schema' for further discussion of
        // field ids.

    const char *fieldName() const;
        // Return the non-modifiable name of this field definition, and 0 if
        // this field it describes is unnamed.

    int formattingMode() const;
        // Return the formatting-mode attribute of this object.  The returned
        // value will be one of the mode values defined by
        // 'bdeat_FormattingMode' (e.g., 'BDEAT_DEFAULT', 'BDEAT_DEC',
        // 'BDEAT_HEX').  Note that this value is used to determine how a
        // field described by these attributes should be formatted in
        // text.  Also note that the formatting mode is provided, at
        // construction, via a 'FieldDefAttributes' object.

    bool hasDefaultValue() const;
        // Return 'true' if this field definition has a default value, and
        // 'false' otherwise.  Note that 'false' is returned if 'elemType'
        // returns an aggregate type -- i.e., 'LIST', 'TABLE', 'CHOICE', or
        // 'CHOICE_ARRAY'.   Also note that the default value is provided, at
        // construction, via a 'FieldDefAttributes' object.

    bool isNullable() const;
        // Return 'true' if a field described by this definition may have a
        // null value, and 'false' otherwise.  Note that the nullability value
        // is provided, at construction, via a 'FieldDefAttributes'
        // object.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                          // -------------------
                          // class FieldDef
                          // -------------------

// CREATORS
inline
FieldDef::FieldDef(const char                     *name,
                             int                             id,
                             const FieldDefAttributes&  attributes,
                             bslma::Allocator               *basicAllocator)
: d_attributes(attributes, basicAllocator)
, d_name_p(name)
, d_id(id)
, d_enumConstraint_p(0)  // simultaneously zeroes 'd_recordConstraint_p'
{
    BSLS_ASSERT_SAFE(0 != basicAllocator);
    BSLS_ASSERT_SAFE(0 <= attributes.elemType());
}

inline
FieldDef::~FieldDef()
{
}

// MANIPULATORS
inline
void FieldDef::setIsNullable(bool value)
{
    d_attributes.setIsNullable(value);
}

inline
void FieldDef::setConstraint(const RecordDef *constraint)
{
    BSLS_ASSERT_SAFE(!constraint
                  || (ElemType::BDEM_LIST         == elemType()
                   || ElemType::BDEM_TABLE        == elemType()
                   || ElemType::BDEM_CHOICE       == elemType()
                   || ElemType::BDEM_CHOICE_ARRAY == elemType()));

    d_recordConstraint_p = constraint;
}

inline
void FieldDef::setConstraint(const EnumerationDef *constraint)
{
    BSLS_ASSERT_SAFE(0 == constraint
                  || EnumerationDef::canHaveEnumConstraint(elemType()));

    d_enumConstraint_p = constraint;
}
}  // close package namespace

// ACCESSORS
#ifndef BDE_OMIT_INTERNAL_DEPRECATED

namespace bdlmxxx {inline
const RecordDef *FieldDef::constraint() const
{
    return recordConstraint();
}
}  // close package namespace
#endif // BDE_OMIT_INTERNAL_DEPRECATED

namespace bdlmxxx {
inline
const RecordDef *FieldDef::recordConstraint() const
{

    return ElemType::isAggregateType(elemType()) ? d_recordConstraint_p
                                                      : 0;
}

inline
const EnumerationDef *
FieldDef::enumerationConstraint() const
{
    return EnumerationDef::canHaveEnumConstraint(elemType())
         ? d_enumConstraint_p
         : 0;
}

inline
ConstElemRef FieldDef::defaultValue() const
{
    return d_attributes.defaultValue();
}

inline
ElemType::Type FieldDef::elemType() const
{
    return d_attributes.elemType();
}

inline
int FieldDef::fieldId() const
{
    return d_id;
}

inline
const char *FieldDef::fieldName() const
{
    return d_name_p;
}

inline
int FieldDef::formattingMode() const
{
    return d_attributes.formattingMode();
}

inline
bool FieldDef::isNullable() const
{
    return d_attributes.isNullable();
}

inline
bool FieldDef::hasDefaultValue() const
{
    return d_attributes.hasDefaultValue();
}
}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
