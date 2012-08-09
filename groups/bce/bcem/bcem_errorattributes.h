// bcem_errorattributes.h                                             -*-C++-*-
#ifndef INCLUDED_BCEM_ERRORATTRIBUTES
#define INCLUDED_BCEM_ERRORATTRIBUTES

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a type to describe errors in dynamically-typed operations.
//
//@CLASSES:
//   bcem_ErrorAttributes: type for descriptive errors
//
//@SEE_ALSO: bcem_aggregate, bcem_aggregateraw
//
//@AUTHOR: David Schumann (dschumann1)
//
//@DESCRIPTION: This component provides a simply-constrained value-semantic
// class, 'bcem_ErrorAttributes', combining an enumerated error code with
// a human-readable verbose description.
//
// The error codes and the conditions that cause them are listed below.
//..
//   Error Code               Cause
//   =======================  =================================================
//   BCEM_SUCCESS             No error (numerical value 0)
//
//   BCEM_ERR_NOT_A_RECORD    Attempt to access a field (by name, ID, or
//                            index) of an aggregate that does not reference a
//                            list, row, choice, or choice array item.
//
//   BCEM_ERR_NOT_A_SEQUENCE  Attempt to perform a list or row operation on an
//                            aggregate that does not refer to a list or row
//                            (e.g., initialize from a non-sequence record
//                            definition).
//
//   BCEM_ERR_NOT_A_CHOICE    Attempt to perform a choice or choice array item
//                            operation (make selection, get selection, etc.)
//                            on an aggregate that is not a choice or choice
//                            array item.
//
//   BCEM_ERR_NOT_AN_ARRAY    Attempt to perform an array operation (index,
//                            insert, etc.) on an aggregate that is not an
//                            array, table, or choice array.
//
//   BCEM_ERR_BAD_FIELDNAME   Field name does not exist in the record def.
//
//   BCEM_ERR_BAD_FIELDID     Field ID does not exist in record def.
//
//   BCEM_ERR_BAD_FIELDINDEX  Field index is not a positive integer less than
//                            the length of the field definition.
//
//   BCEM_ERR_BAD_ARRAYINDEX  Array (or table) index is out of bounds.
//
//   BCEM_ERR_NOT_SELECTED    Attempt to access a choice field that is not
//                            the currently selected object.
//
//   BCEM_ERR_BAD_CONVERSION  Attempt to set an aggregate using a value that
//                            is not convertible to the aggregate's type.  (See
//                            "Extended Type Conversions" below.)
//
//   BCEM_ERR_BAD_ENUMVALUE   Attempt to set the value of an enumeration
//                            aggregate to a string that is not an enumerator
//                            name in the enumeration definition or to an
//                            integer that is not an enumerator ID in the
//                            enumeration definition.
//
//   BCEM_ERR_NON_CONFORMANT  Attempt to set a list, row, table, choice,
//                            choice array item, or choice array aggregate to
//                            a value of the correct type, but which does not
//                            conform to the aggregate's record definition.
//
//   BCEM_ERR_AMBIGUOUS_ANON  A reference to an anonymous field is ambiguous,
//                            typically because the aggregate contains more
//                            than one anonymous field.
//..
///Attributes
///----------
//..
//  Name          Type                   Default
//  ----------    --------------------   ----------------
//  code          bcem_ErrorCode::Code   BCEM_SUCCESS (0)
//  description   bsl::string            ""
//..
//: o 'code': one of the enumerated values of 'bcem_ErrorCode::Code'.
//:
//: o 'description': a human-readable description of the error, if any.
//
///Usage
///-----
// 'bcem_ErrorAttributes' is a vocabulary type for communicating errors from
// operations on a 'bcem_Aggregate' objects.  In this example, we elide the
// actual function invocation and instead focus on handling the error
// condition:
//..
//  bcem_ErrorAttributes error;
//
//  // Manipulate 'object' in some way, passing the address of  'error'
//  // ... snip ...
//
//  if (0 != error.code()) {
//      bsl::cout << "Error code: " << error.code()
//                << " description: " << error.description() << bsl::endl;
//  }
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEM_ERRORCODE
#include <bcem_errorcode.h>
#endif

#ifndef INCLUDED_BDEUT_STRINGREF
#include <bdeut_stringref.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

                       // ==========================
                       // class bcem_ErrorAttributes
                       // ==========================

class bcem_ErrorAttributes {
    // This value-semantic attribute class provides an enumerated error code
    // and a human-readable message to describe errors arising from the
    // the usage of dynamically-typed objects.

    // DATA
    bsl::string          d_description;                // description of error
    bcem_ErrorCode::Code d_code;                       // error code

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcem_ErrorAttributes,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit bcem_ErrorAttributes(bslma_Allocator *basicAllocator = 0);
        // Create a 'bcem_ErrorAttributes' object having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    bcem_ErrorAttributes(bcem_ErrorCode::Code  code,
                         const char           *description,
                         bslma_Allocator      *basicAllocator = 0);
        // Create a 'bcem_ErrorAttributes' object having the specified 'code'
        // and 'description'.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    bcem_ErrorAttributes(const bcem_ErrorAttributes&  original,
                         bslma_Allocator             *basicAllocator = 0);
        // Create a 'bcem_ErrorAttributes' object having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    //! ~bcem_ErrorAttributes() = default;
        // Destroy this object.  Note that this trivial destructor is generated
        // by the compiler.

    // MANIPULATORS
    // bcem_ErrorAttributes& operator=(const bcem_ErrorAttributes& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // Note that the compiler generated default is used.

    void setCode(bcem_ErrorCode::Code value);
        // Set the 'code' attribute of this object to the specified 'value'.

    void setDescription(const bdeut_StringRef& value);
        // Set the 'description' attribute of this object to the specified
        // 'value'.

    // ACCESSORS
    bcem_ErrorCode::Code code() const;
        // Return a reference providing non-modifiable access to the 'code'
        // attribute of this object.

    const bsl::string& description() const;
        // Return a reference providing non-modifiable access to the
        // 'description' attribute of this object.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a reference providing modifiable
        // access to 'stream'.  Optionally specify an initial indentation
        // 'level', whose absolute value is incremented recursively for nested
        // objects.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', whose absolute value indicates the number of
        // spaces per indentation level for this and all of its nested objects.
        // If 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // 'level').  If 'stream' is not valid on entry, this operation has no
        // effect.  Note that the format is not fully specified, and can change
        // without notice.
};

// FREE OPERATORS
bool operator==(const bcem_ErrorAttributes& lhs,
                const bcem_ErrorAttributes& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'bcem_ErrorAttributes' objects have
    // the same value if all of the corresponding values of their 'code', and
    // 'description' attributes are the same.

bool operator!=(const bcem_ErrorAttributes& lhs,
                const bcem_ErrorAttributes& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'bcem_ErrorAttributes' objects
    // do not have the same value if any of the corresponding values of their
    // 'code', or 'description' attributes are not the same.

bsl::ostream& operator<<(bsl::ostream&               stream,
                         const bcem_ErrorAttributes& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.  Note that this human-readable format is not
    // fully specified and can change without notice.  Also note that this
    // method has the same behavior as 'object.print(stream, 0, -1)' with the
    // attribute names elided.

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                     // --------------------------
                     // class bcem_ErrorAttributes
                     // --------------------------

// CREATORS
inline
bcem_ErrorAttributes::bcem_ErrorAttributes(bslma_Allocator *basicAllocator)
: d_description(basicAllocator)
, d_code(bcem_ErrorCode::BCEM_SUCCESS)
{
}

inline
bcem_ErrorAttributes::bcem_ErrorAttributes(
                                   const bcem_ErrorAttributes&  original,
                                   bslma_Allocator             *basicAllocator)
: d_description(original.d_description, basicAllocator)
, d_code(original.d_code)
{
}

inline
bcem_ErrorAttributes::bcem_ErrorAttributes(
                                          bcem_ErrorCode::Code  code,
                                          const char           *description,
                                          bslma_Allocator      *basicAllocator)
: d_description(description, basicAllocator)
, d_code(code)
{
}

// MANIPULATORS
inline
void bcem_ErrorAttributes::setCode(bcem_ErrorCode::Code value)
{
    d_code = value;
}

inline
void bcem_ErrorAttributes::setDescription(const bdeut_StringRef& value)
{
    d_description = value;
}

// ACCESSORS
inline
bcem_ErrorCode::Code bcem_ErrorAttributes::code() const
{
    return d_code;
}

inline
const bsl::string& bcem_ErrorAttributes::description() const
{
    return d_description;
}

// FREE OPERATORS
inline
bool operator==(const bcem_ErrorAttributes& lhs,
                const bcem_ErrorAttributes& rhs)
{
    return lhs.code()        == rhs.code()
        && lhs.description() == rhs.description();
}

inline
bool operator!=(const bcem_ErrorAttributes& lhs,
                const bcem_ErrorAttributes& rhs)
{
    return lhs.code()        != rhs.code()
        || lhs.description() != rhs.description();
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bcem_ErrorAttributes& rhs)
{
    return rhs.print(stream, 0, -1);
}

} //  close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
