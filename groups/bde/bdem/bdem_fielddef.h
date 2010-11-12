// bdem_fielddef.h                                                    -*-C++-*-
#ifndef INCLUDED_BDEM_FIELDDEF
#define INCLUDED_BDEM_FIELDDEF

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a class representing the definition for a single field.
//
//@CLASSES:
//  bdem_FieldDef: definition for a single field
//
//@SEE_ALSO: bdem_fielddefattributes, bdem_schema
//
//@AUTHOR: Tom Marshall, Henry Verschell
//
//@DESCRIPTION: This component implements a collaborative type,
// 'bdem_FieldDef', that is an integral part of a fully-value semantic schema
// object, and is used to define a single field.  A field, in this context, is
// a value of one of the types supported by the 'bdem' package (see
// 'bdem_elemtype'), generally in the context of a record (or sequence of
// fields).  A 'bdem_FieldDef' has attributes describing the data type, default
// value, nullability, formatting mode, and constraint of a field.  In general,
// clients should *not* construct a 'bdem_FieldDef' directly, but instead
// obtain a field definition from a 'bdem_RecordDef' (see 'bdem_recorddef' and
// 'bdem_schema' for more information).  The 'elemType' of a field definition
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
// 'bdem_RecordDef').  For a 'CHOICE' and 'CHOICE_ARRAY' the constraining
// record definition must be of type 'bdem_RecordDef::BDEM_CHOICE_RECORD', and
// the constraint indicates the selection of types that can be made for the
// defined field.  For a 'LIST' or 'TABLE' the constraining record definition
// must be of type 'bdem_RecordDef::BDEM_SEQUENCE_RECORD', and the constraint
// indicates the types of the sub-fields contained within the defined
// aggregate field.  Field definitions of type 'STRING', 'INT',
// 'STRING_ARRAY', or 'INT_ARRAY', can be constrained by a enumeration
// definition (i.e., a 'bdem_EnumerationDef').  The enumeration constraint
// indicates the set of valid integer or string values to which fields of those
// types can be set.  Note that 'setConstraint' does not validate whether the
// supplied constraint is valid for the type of field.
//
///Usage
///-----
// The following usage examples demonstrate how to construct, manipulate, and
// access a field definition in isolation.  In general, clients should *not*
// create a 'bdem_FieldDef' directly, but obtain one from a record definition
// (see 'bdem_recorddef').
//
///Directly Creating a 'bdem_FieldDef' (*Not Recommended*)
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates how to create a 'bdem_FieldDef' in
// isolation -- this is *not* the recommended usage of 'bdem_FieldDef':
// Clients should obtain one from a record definition (see 'bdem_recorddef').
// For the purpose of this usage example, we define a dummy "stand-in" types
// for 'bdem_RecordDef' and 'bdem_Schema'.  These types are used *in* *name*
// *only* by 'bdem_FieldDef', so no particular definition is necessary:
//..
//  namespace BloombergLP {
//
//  class bdem_RecordDef {
//      // Dummy type.
//  };
//
//  class bdem_Schema {
//      // Dummy type.
//  };
//
//  }
//..
// Next we create a couple field definition objects:
//..
//  bslma_Allocator           *allocator = bslma_Default::allocator();
//  bdema_SequentialAllocator  seqAllocator(allocator);
//
//  bdem_FieldDefAttributes  intAttr(bdem_ElemType::BDEM_INT,  allocator);
//  bdem_FieldDefAttributes listAttr(bdem_ElemType::BDEM_LIST, allocator);
//
//  bdem_FieldDef  intField("intField",  5,  intAttr, allocator);
//  bdem_FieldDef listField("listField", 2, listAttr, allocator);
//..
// We now provide constraints for these fields.  Note that 'bdem_RecordDef'
// and 'bdem_Schema' are the dummy place-holder types defined above.  Clients
// should refer to the documentation of 'bdem_schema' for more information on
// specifying field constraints.
//..
//  bdem_Schema         schema;
//  bdem_EnumerationDef enumerationDef(&schema, 0, "enum", &seqAllocator);
//  bdem_RecordDef      recordConstraint;
//
//  intField.setConstraint(&enumerationConstraint);
//  listField.setConstraint(&recordConstraint);
//..
//
///Accessing a 'bdem_FieldDef'
///- - - - - - - - - - - - - -
// In this example, we demonstrate how to access the properties of a
// 'bdem_FieldDef' object.  The two objects, 'intField' and 'listField' were
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

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEM_ELEMREF
#include <bdem_elemref.h>
#endif

#ifndef INCLUDED_BDEM_ELEMTYPE
#include <bdem_elemtype.h>
#endif

#ifndef INCLUDED_BDEM_FIELDDEFATTRIBUTES
#include <bdem_fielddefattributes.h>
#endif

#ifndef INCLUDED_BDEM_FIELDSPEC
#include <bdem_fieldspec.h>
#endif

#ifndef INCLUDED_BDEM_ENUMERATIONDEF
#include <bdem_enumerationdef.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

namespace BloombergLP {

class bdem_RecordDef;
class bdem_EnumerationDef;

                          // ===================
                          // class bdem_FieldDef
                          // ===================

class bdem_FieldDef {
    // This collaborative class represents the definition for a single field,
    // and is an integral part of a fully value-semantic schema object.  A
    // field definition has attributes specifying the data type, nullability,
    // default value, and formatting for the field being defined.  In
    // addition, a field definition optionally contains a name and,
    // independently, optionally contains a constraint.  In general, clients
    // should *not* construct a 'bdem_FieldDef' directly, but instead obtain a
    // field definition from a 'bdem_RecordDef' (see 'bdem_recorddef' and
    // 'bdem_schema' for more information).  The 'name' of a field
    // definition is supplied at construction and may be 0.  A field
    // definition provides manipulators to set the constraint of a field.
    // These manipulators do *not* validate the values provided (see the
    // documentation for 'bdem_recorddef' for information on validating
    // constraints).

    // DATA
    bdem_FieldDefAttributes  d_attributes;  // field type

    const char              *d_name_p;      // name, if any (held, not owned)

    int                      d_id;          // field id, if any

    union {
        const bdem_RecordDef      *d_recordConstraint_p;
                                            // constraint on choice, choice
                                            // array, list, or table (held,
                                            // not owned)

        const bdem_EnumerationDef *d_enumConstraint_p;
                                            // enumeration constraint on
                                            // string or int (held, not
                                            // owned)
    };

  private:
    // NOT IMPLEMENTED
    bdem_FieldDef(const bdem_FieldDef&);
    bdem_FieldDef& operator=(const bdem_FieldDef&);

  public:
    // TYPES
    struct LowPrecedenceEnumPtr {
        // We use this struct instead of a 'bdem_EnumerationDef *' in an
        // overloaded function signature to address overloading ambiguities
        // that would otherwise arise from 'bdem_EnumerationDef *' and
        // 'bdem_RecordDef *' when a literal 0 is passed in as the argument.

        const bdem_EnumerationDef *d_ptr;
        LowPrecedenceEnumPtr(const bdem_EnumerationDef *p)
        : d_ptr(p) { }
    };

    // CREATORS
    bdem_FieldDef(const char                     *name,
                  int                             id,
                  const bdem_FieldDefAttributes&  attributes,
                  bslma_Allocator                *basicAllocator);
        // Create a field definition having the specified 'name', id', and
        // 'attributes', using the specified 'basicAllocator' to supply memory.
        // If 'name' is 0, the field definition is unnamed.  Note that
        // clients, in general, should *not* construct a 'bdem_FieldDef'
        // directly (see 'bdem_recorddef' and 'bdem_schema').  The behavior is
        // undefined if 'basicAllocator' is 0.

    ~bdem_FieldDef();
        // Destroy this field definition.

    // MANIPULATORS
    void setIsNullable(bool value);
        // Set the nullability attribute of this field definition to the
        // specified 'value'.

    void setConstraint(const bdem_RecordDef *constraint);
        // Set the constraint of this field definition to the specified
        // *record* 'constraint'.  The behavior is undefined unless 'elemType'
        // is an aggregate type -- i.e., 'LIST', 'TABLE, 'CHOICE', or
        // 'CHOICE_ARRAY'.

    void setConstraint(const bdem_EnumerationDef *constraint);
        // Set the constraint of this field definition to the specified
        // *enumeration* 'constraint'.   The behavior is undefined unless
        // 'elemType' is an enumerable type -- i.e., 'INT''STRING',
        // 'INT_ARRAY', or 'STRING_ARRAY'.

    // ACCESSORS
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
    const bdem_RecordDef *constraint() const;
        // Return the same result as that returned by 'recordConstraint'.
        //
        // DEPRECATED: Use 'recordConstraint' or 'enumerationConstraint'.
#endif

    const bdem_EnumerationDef *enumerationConstraint() const;
        // Return the address of this field's non-modifiable enumeration
        // constraint, and 0 if no such constraint exists.  Note that
        // enumeration constraints may exist for only enumerable types --
        // i.e., 'STRING', 'INT', 'STRING_ARRAY', and 'INT_ARRAY' types.

    const bdem_RecordDef *recordConstraint() const;
        // Return the address of this field's non-modifiable record
        // constraint, and 0 if no such constraint exists.  Note that record
        // constraints may exist for only aggregate types -- i.e., 'CHOICE',
        // 'CHOICE_ARRAY', 'LIST', and 'TABLE' types.

    bdem_ConstElemRef defaultValue() const;
        // Return a 'bdem' const-element reference to the default value
        // attribute of this object.  If 'hasDefaultValue' would have initially
        // returned 'false', a valid element reference to the default-value
        // object of the appropriate type (having the null value) is returned.
        // If the element type of this attribute is an array of scalars, then
        // the default value will be of the corresponding scalar type.  The
        // behavior is undefined if the type of this field definition is an
        // aggregate type -- i.e., 'LIST', 'TABLE', 'CHOICE', or
        // 'CHOICE_ARRAY'.  Note that the default value is provided, at
        // construction, via a 'bdem_FieldDefAttributes' object.

    bdem_ElemType::Type elemType() const;
        // Return the 'bdem' element type of a field described by this
        // definition.

    bdem_FieldSpec fieldSpec() const;
        // Return the value of the field spec for this field definition.
        //
        // DEPRECATED: Access the properties directly from this field
        // definition instead.

    int fieldId() const;
        // Return the id of this field definition supplied at construction.
        // See 'bdem_recorddef' and 'bdem_schema' for further discussion of
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
        // construction, via a 'bdem_FieldDefAttributes' object.

    bool hasDefaultValue() const;
        // Return 'true' if this field definition has a default value, and
        // 'false' otherwise.  Note that 'false' is returned if 'elemType'
        // returns an aggregate type -- i.e., 'LIST', 'TABLE', 'CHOICE', or
        // 'CHOICE_ARRAY'.   Also note that the default value is provided, at
        // construction, via a 'bdem_FieldDefAttributes' object.

    bool isNullable() const;
        // Return 'true' if a field described by this definition may have a
        // null value, and 'false' otherwise.  Note that the nullability value
        // is provided, at construction, via a 'bdem_FieldDefAttributes'
        // object.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                          // -------------------
                          // class bdem_FieldDef
                          // -------------------

// CREATORS
inline
bdem_FieldDef::bdem_FieldDef(const char                     *name,
                             int                             id,
                             const bdem_FieldDefAttributes&  attributes,
                             bslma_Allocator                *basicAllocator)
: d_attributes(attributes, basicAllocator)
, d_name_p(name)
, d_id(id)
, d_enumConstraint_p(0)  // simultaneously zeroes 'd_recordConstraint_p'
{
    BSLS_ASSERT_SAFE(0 != basicAllocator);
    BSLS_ASSERT_SAFE(0 <= attributes.elemType());
}

inline
bdem_FieldDef::~bdem_FieldDef()
{
}

// MANIPULATORS
inline
void bdem_FieldDef::setIsNullable(bool value)
{
    d_attributes.setIsNullable(value);
}

inline
void bdem_FieldDef::setConstraint(const bdem_RecordDef *constraint)
{
    BSLS_ASSERT_SAFE(!constraint
                  || (bdem_ElemType::BDEM_LIST         == elemType()
                   || bdem_ElemType::BDEM_TABLE        == elemType()
                   || bdem_ElemType::BDEM_CHOICE       == elemType()
                   || bdem_ElemType::BDEM_CHOICE_ARRAY == elemType()));

    d_recordConstraint_p = constraint;
}

inline
void bdem_FieldDef::setConstraint(const bdem_EnumerationDef *constraint)
{
    BSLS_ASSERT_SAFE(0 == constraint
                  || bdem_EnumerationDef::canHaveEnumConstraint(elemType()));

    d_enumConstraint_p = constraint;
}

// ACCESSORS
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
inline
const bdem_RecordDef *bdem_FieldDef::constraint() const
{
    return recordConstraint();
}
#endif

inline
const bdem_RecordDef *bdem_FieldDef::recordConstraint() const
{

    return bdem_ElemType::isAggregateType(elemType()) ? d_recordConstraint_p
                                                      : 0;
}

inline
const bdem_EnumerationDef *
bdem_FieldDef::enumerationConstraint() const
{
    return bdem_EnumerationDef::canHaveEnumConstraint(elemType())
         ? d_enumConstraint_p
         : 0;
}

inline
bdem_ConstElemRef bdem_FieldDef::defaultValue() const
{
    return d_attributes.defaultValue();
}

inline
bdem_ElemType::Type bdem_FieldDef::elemType() const
{
    return d_attributes.elemType();
}

inline
int bdem_FieldDef::fieldId() const
{
    return d_id;
}

inline
const char *bdem_FieldDef::fieldName() const
{
    return d_name_p;
}

inline
int bdem_FieldDef::formattingMode() const
{
    return d_attributes.formattingMode();
}

inline
bool bdem_FieldDef::isNullable() const
{
    return d_attributes.isNullable();
}

inline
bool bdem_FieldDef::hasDefaultValue() const
{
    return d_attributes.hasDefaultValue();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
