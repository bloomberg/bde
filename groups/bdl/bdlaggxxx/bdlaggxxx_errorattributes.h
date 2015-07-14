// bdlaggxxx_errorattributes.h                                             -*-C++-*-
#ifndef INCLUDED_BDLAGGXXX_ERRORATTRIBUTES
#define INCLUDED_BDLAGGXXX_ERRORATTRIBUTES

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a type to describe errors in dynamically-typed operations.
//
//@CLASSES:
//   bdlaggxxx::ErrorAttributes: type for descriptive errors
//
//@SEE_ALSO: bdlaggxxx_aggregate, bdlaggxxx_aggregateraw, bdlaggxxx_errorcode
//
//@AUTHOR: David Schumann (dschumann1), Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides a simply-constrained value-semantic
// class, 'bdlaggxxx::ErrorAttributes', combining an enumerated error code with
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
//  code          bdlaggxxx::ErrorCode::Code   BCEM_SUCCESS (0)
//  description   bsl::string            ""
//..
//: o 'code': one of the enumerated values of 'bdlaggxxx::ErrorCode::Code'.
//:
//: o 'description': a human-readable description of the error, if any.
//
///Usage
///-----
// 'bdlaggxxx::ErrorAttributes' is a vocabulary type for communicating errors from
// operations on 'bdlaggxxx::Aggregate' objects.  In this example, we elide the
// actual function invocation and instead focus on handling the error
// condition:
//..
//  bdlaggxxx::ErrorAttributes error;
//
//  // Manipulate 'object' in some way, passing the address of  'error'
//  // ... snip ...
//
//  if (0 != error.code()) {
//      bsl::cout << "Error code: " << error.code()
//                << " description: " << error.description() << bsl::endl;
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLAGGXXX_ERRORCODE
#include <bdlaggxxx_errorcode.h>
#endif

#ifndef INCLUDED_BDLB_XXXSTRINGREF
#include <bdlb_xxxstringref.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

namespace bdlaggxxx {
                       // ==========================
                       // class ErrorAttributes
                       // ==========================

class ErrorAttributes {
    // This value-semantic attribute class provides an enumerated error code
    // and a human-readable message to describe errors arising from the usage
    // of dynamically-typed objects.  See the Attributes section under
    // @DESCRIPTION in the component-level documentation for information on the
    // class attributes.  Note that the class invariants are identically the
    // constraints on the individual attributes.
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral* (agnostic)
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // DATA
    bsl::string          d_description;  // description of error
    ErrorCode::Code d_code;         // error code

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(ErrorAttributes,
                                 bslalg::TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit ErrorAttributes(bslma::Allocator *basicAllocator = 0);
        // Create a 'ErrorAttributes' object having the default value.
        // Use the optionally specified 'basicAllocator' to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ErrorAttributes(ErrorCode::Code  code,
                         const char           *description,
                         bslma::Allocator     *basicAllocator = 0);
        // Create a 'ErrorAttributes' object having the specified 'code'
        // and 'description'.  Use the optionally specified 'basicAllocator' to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ErrorAttributes(const ErrorAttributes&  original,
                         bslma::Allocator            *basicAllocator = 0);
        // Create a 'ErrorAttributes' object having the value of the
        // specified 'original' object.  Use the optionally specified
        // 'basicAllocator' to supply memory.  If 'basicAllocator' is 0, the
        // currently installed default allocator is used.

    //! ~ErrorAttributes() = default;
        // Destroy this object.  Note that this trivial destructor is generated
        // by the compiler.

    // MANIPULATORS
    // ErrorAttributes& operator=(const ErrorAttributes& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that the compiler generated default is used.

    void setCode(ErrorCode::Code value);
        // Set the 'code' attribute of this object to the specified 'value'.

    void setDescription(const bdlb::StringRef& value);
        // Set the 'description' attribute of this object to the specified
        // 'value'.

    // ACCESSORS
    ErrorCode::Code code() const;
        // Return the 'code' attribute of this object.

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
bool operator==(const ErrorAttributes& lhs,
                const ErrorAttributes& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'ErrorAttributes' objects have
    // the same value if all of the corresponding values of their 'code' and
    // 'description' attributes are the same.

bool operator!=(const ErrorAttributes& lhs,
                const ErrorAttributes& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'ErrorAttributes' objects
    // do not have the same value if any of the corresponding values of their
    // 'code' or 'description' attributes are not the same.

bsl::ostream& operator<<(bsl::ostream&               stream,
                         const ErrorAttributes& object);
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
                     // class ErrorAttributes
                     // --------------------------

// CREATORS
inline
ErrorAttributes::ErrorAttributes(bslma::Allocator *basicAllocator)
: d_description(basicAllocator)
, d_code(ErrorCode::BCEM_SUCCESS)
{
}

inline
ErrorAttributes::ErrorAttributes(
                                   const ErrorAttributes&  original,
                                   bslma::Allocator            *basicAllocator)
: d_description(original.d_description, basicAllocator)
, d_code(original.d_code)
{
}

inline
ErrorAttributes::ErrorAttributes(
                                          ErrorCode::Code  code,
                                          const char           *description,
                                          bslma::Allocator     *basicAllocator)
: d_description(description, basicAllocator)
, d_code(code)
{
}

// MANIPULATORS
inline
void ErrorAttributes::setCode(ErrorCode::Code value)
{
    d_code = value;
}

inline
void ErrorAttributes::setDescription(const bdlb::StringRef& value)
{
    d_description = value;
}

// ACCESSORS
inline
ErrorCode::Code ErrorAttributes::code() const
{
    return d_code;
}

inline
const bsl::string& ErrorAttributes::description() const
{
    return d_description;
}
}  // close package namespace

// FREE OPERATORS
inline
bool bdlaggxxx::operator==(const ErrorAttributes& lhs,
                const ErrorAttributes& rhs)
{
    return lhs.code()        == rhs.code()
        && lhs.description() == rhs.description();
}

inline
bool bdlaggxxx::operator!=(const ErrorAttributes& lhs,
                const ErrorAttributes& rhs)
{
    return lhs.code()        != rhs.code()
        || lhs.description() != rhs.description();
}

inline
bsl::ostream& bdlaggxxx::operator<<(bsl::ostream& stream, const ErrorAttributes& rhs)
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
