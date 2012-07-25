// bcem_aggregateerror.h                                              -*-C++-*-
#ifndef INCLUDED_BCEM_AGGREGATEERROR
#define INCLUDED_BCEM_AGGREGATEERROR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a type to describe errors in dynamically-typed operations.
//
//@CLASSES:
//   bcem_AggregateError: type for descriptive errors
//
//@SEE_ALSO: bcem_aggregate, bcem_aggregateraw
//
//@AUTHOR: David Schumann (dschumann1)
//
//@DESCRIPTION: This component provides a simply-constrained value-semantic
// class, 'bcem_AggregateError', combining an enumerated error condition with a
// human-readable verbose description.
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
//  Name          Type               Default
//  ----------    ---------------    ----------------
//  code          int                BCEM_SUCCESS (0)
//  description   bsl::string        ""
//..
//: o 'code': one of the enumerated values of 'bcem_AggregateError::Code'.
//:
//: o 'description': a human-readable description of the error, if any.
//
///Usage
///-----
// 'bcem_AggregateError' is a vocabulary type for communicating errors from
// operations on a 'bcem_Aggregate' objects.  In this example, we elide the
// actual function invocation and instead focus on handling the error
// condition:
//..
//  bcem_AggregateError error;
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

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                       // =========================
                       // class bcem_AggregateError
                       // =========================

class bcem_AggregateError {
    // This value-semantic attribute class provides an enumerated error code
    // and a human-readable message to describe errors arising from the
    // the usage of dynamically-typed objects.

    // DATA
    bsl::string          d_description;                // description of error
    bcem_ErrorCode::Code d_code;                       // error code

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bcem_AggregateError,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit bcem_AggregateError(bslma_Allocator *basicAllocator = 0);
        // Create an object of type 'AggregateError' having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    bcem_AggregateError(bcem_ErrorCode::Code  code,
                        const char           *description,
                        bslma_Allocator      *basicAllocator = 0);
        // Create an AggregateError object having the specified 'code' and
        // 'description'.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    bcem_AggregateError(const bcem_AggregateError&  original,
                        bslma_Allocator            *basicAllocator = 0);
        // Create an object of type 'AggregateError' having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    //! ~bcem_AggregateError() = default;
        // Destroy this object.  Note that this trivial destructor is generated
        // by the compiler.

    // MANIPULATORS
    // bcem_AggregateError& operator=(const bcem_AggregateError& rhs);
        // Assign to this object the value of the specified 'rhs' object.
        // Note that the compiler generated default is used.

    bcem_ErrorCode::Code& code();
        // Return a modifiable reference to the "code" attribute of this
        // object.

    bsl::string& description();
        // Return a modifiable reference to the "description" attribute of this
        // object.

    // ACCESSORS
    bcem_ErrorCode::Code code() const;
        // Return a non-modifiable reference to the "code" attribute of this
        // object.

    const bsl::string& description() const;
        // Return a non-modifiable reference to the "description" attribute of
        // this object.

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
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const bcem_AggregateError& object);
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

// CREATORS
inline
bcem_AggregateError::bcem_AggregateError(bslma_Allocator *basicAllocator)
: d_description(basicAllocator)
, d_code(bcem_ErrorCode::BCEM_SUCCESS)
{
}

inline
bcem_AggregateError::bcem_AggregateError(
                                    const bcem_AggregateError&  original,
                                    bslma_Allocator            *basicAllocator)
: d_description(original.d_description, basicAllocator)
, d_code(original.d_code)
{
}

inline
bcem_AggregateError::bcem_AggregateError(bcem_ErrorCode::Code  code,
                                         const char           *description,
                                         bslma_Allocator      *basicAllocator)
: d_description(description, basicAllocator)
, d_code(code)
{
}

// MANIPULATORS
inline
bcem_ErrorCode::Code& bcem_AggregateError::code()
{
    return d_code;
}

inline
bsl::string& bcem_AggregateError::description()
{
    return d_description;
}

// ACCESSORS
inline
bcem_ErrorCode::Code bcem_AggregateError::code() const
{
    return d_code;
}

inline
const bsl::string& bcem_AggregateError::description() const
{
    return d_description;
}

// FREE OPERATORS
inline
bsl::ostream& operator<<(bsl::ostream& stream, const bcem_AggregateError& rhs)
{
    return rhs.print(stream, 0, -1);
}

} //  close namespace BloombergLP

#endif // INCLUDED_BCEM_AGGREGATEERROR

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
