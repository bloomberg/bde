// bdld_datumudt.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLD_DATUMUDT
#define INCLUDED_BDLD_DATUMUDT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a type to represent a user-defined type.
//
//@CLASSES:
//  bdld::DatumUdt: a *value-semantic* type representing user-defined type
//
//@SEE_ALSO: bdld_datum, bdld_datumerror, bdld_binaryref
//
//@DESCRIPTION:  This component provides a single, simply-constrained (in-core
// value-semantic) attribute class, 'bdld::DatumUdt', that is used to extend
// the set of possible types that can be held by 'Datum' objects.
//
///Attributes
///----------
//..
//  Name                Type         Simple Constraints
//  ------------------  -----------  ------------------
//  data_p              void *       none
//  type                int          [0 .. 65355]
//..
//: o 'data_p': pointer to an external object/memory.
//:
//: o 'type': user type associated with the pointer.
//
// This component provides a way to extend the set of data types that are
// natively supported by the 'Datum' mechanism by allowing the client associate
// an opaque pointer to user data with the data type information. Note that the
// 'bdld::DatumUdt' does not access the pointer nor it interpret the type
// information supplied at construction time. The client code is responsible
// for interpretation of the type information and casting the pointer back to
// the pointer to the actual type.
//
// Accessors inside 'Datum' class that need to return a user-defined object,
// return an instance of 'DatumUdt'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic 'DatumUdt' usage
///- - - - - - - - - - - - - - - - -
// Imagine we are using 'Datum' within an expression evaluation subsystem.
// Within that subsystem, along with the set of types defined by
// 'Datum::DataType' we also need to hold 'Sequence' and 'Choice' types within
// 'Datum' values (which are not natively represented by 'Datum').  First, we
// define the set of types used by our subsystem that are an extension to the
// types in 'DatumType':
//..
//  struct Sequence {
//      struct Sequence *d_next_p;
//      int              d_value;
//  };
//
//  enum ExtraExpressionTypes {
//      e_SEQUENCE = 5,
//      e_CHOICE = 6
//  };
//..
// Notice that the numeric values will be provided as the 'type' attribute to
// 'DatumUdt'.
//
// Then we create a 'Sequence' object, and create a 'DatumUdt' to hold it (note
// that we've created the object on the stack for clarity):
//..
//  Sequence sequence;
//  {
//      const bdld::DatumUdt udt(&sequence, e_SEQUENCE);
//      assert(e_SEQUENCE == udt.type());
//      assert(&sequence  == udt.data());
//  }
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace bdld {
                                 // ==============
                                 // class DatumUdt
                                 // ==============
class DatumUdt {
    // This class provides a type to represent a pointer to an object of some
    // user-defined type and an integer value denoting what actual type is
    // represented.

  private:
    // DATA
    void *d_data_p;  // pointer to user-defined object (not owned)
    int   d_type;    // type of user-defined object

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(DatumUdt, bsl::is_trivially_copyable);

    // CREATORS
    DatumUdt(void *data, int type);
        // Create a 'DatumUdt' object having the specified 'data' and 'type'.
        // Note that the pointer to the 'data' is just copied bitwise.  Also
        // note that the memory pointed by 'data' is never accessed by a
        // 'DatumUdt' object.

    //! DatumUdt(const DatumUdt& origin) = default;
    //! ~DatumUdt() = default;

    // MANIPULATORS
    //! DatumUdt& operator=(const DatumUdt& rhs) = default;

    // ACCESSORS
    void *data() const;
        // Return the pointer to the user-defined object.

    int type() const;
        // Return the type of the user-defined object.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a reference to the modifyable
        // 'stream'.  Optionally specify an initial indentation 'level', whose
        // absolute value is incremented recursively for nested objects.  If
        // 'level' is specified, optionally specify 'spacesPerLevel', whose
        // absolute value indicates the number of spaces per indentation level
        // for this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.  Note that this
        // human-readable format is not fully specified, and can change without
        // notice.
};

// FREE OPERATORS
bool operator==(const DatumUdt& lhs, const DatumUdt& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two 'DatumUdt' objects have the same value if they
    // have the same data and type values.

bool operator!=(const DatumUdt& lhs, const DatumUdt& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have different values,
    // and 'false' otherwise.  Two 'DatumUdt' objects have different values if
    // they have different data or type values.

bool operator<(const DatumUdt& lhs, const DatumUdt& rhs);
    // Return 'true' if value of the specified 'lhs' is less than value of the
    // specified 'rhs' and 'false' otherwise.  Value of 'lhs' is less than
    // value of 'rhs', if data value of 'lhs' is less than data value of 'rhs',
    // or they have the same data value and type value of 'lhs' is less than
    // type value of 'rhs'.

bool operator<=(const DatumUdt& lhs, const DatumUdt& rhs);
    // Return 'true' if value of the specified 'lhs' is less than or equal to
    // value of the specified 'rhs' and 'false' otherwise.  Value of 'lhs' is
    // less than or equal to value of 'rhs', if data value of 'lhs' is less
    // than or equal to data value of 'rhs', or they have the same data value
    // and type value of 'lhs' is less than or equal to type value of 'rhs'.

bool operator>(const DatumUdt& lhs, const DatumUdt& rhs);
    // Return 'true' if value of the specified 'lhs' is greater than value of
    // the specified 'rhs' and 'false' otherwise.  Value of 'lhs' is greater
    // than value of 'rhs', if data value of 'lhs' is greater than data value
    // of 'rhs', or they have the same data value and type value of 'lhs' is
    // greater than type value of 'rhs'.

bool operator>=(const DatumUdt& lhs, const DatumUdt& rhs);
    // Return 'true' if value of the specified 'lhs' is greater than or equal
    // to value of the specified 'rhs' and 'false' otherwise.  Value of 'lhs'
    // is greater than or equal to value of 'rhs', if data value of 'lhs' is
    // greater than or equal to data value of 'rhs', or they have the same data
    // value and type value of 'lhs' is greater than or equal to type value of
    // 'rhs'.

bsl::ostream& operator<<(bsl::ostream& stream, const DatumUdt& rhs);
    // Write the value of the specified 'rhs' object to the specified output
    // 'stream' in a single-line format, and return a reference to the
    // modifyable 'stream'.  If 'stream' is not valid on entry, this operation
    // has no effect.  Note that this human-readable format is not fully
    // specified, can change without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..
    // Single line output format for the 'DatumUdt' object is shown below:
    //..
    //  user-defined(<address>,type)
    //..
    //(where 'address' is hex value of the pointer to the user-define object
    //and 'type' is the type of the user-defined object)

// ============================================================================
//                               INLINE DEFINITIONS
// ============================================================================

                                // --------------
                                // class DatumUdt
                                // --------------

// CREATORS
inline
DatumUdt::DatumUdt(void *data, int type)
: d_data_p(data)
, d_type(type)
{
}

// ACCESSORS
inline
void *DatumUdt::data() const
{
    return d_data_p;
}

inline
int DatumUdt::type() const
{
    return d_type;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdld::operator==(const DatumUdt& lhs, const DatumUdt& rhs)
{
    return (lhs.data() == rhs.data() &&
            lhs.type() == rhs.type());
}

inline
bool bdld::operator!=(const DatumUdt& lhs, const DatumUdt& rhs)
{
    return (lhs.data() != rhs.data() ||
            lhs.type() != rhs.type());
}

inline
bool bdld::operator<(const DatumUdt& lhs, const DatumUdt& rhs)
{
    return (lhs.data() < rhs.data() ||
           (lhs.data() == rhs.data() && lhs.type() < rhs.type()));
}

inline
bool bdld::operator<=(const DatumUdt& lhs, const DatumUdt& rhs)
{
    return (lhs == rhs || lhs < rhs);
}

inline
bool bdld::operator>(const DatumUdt& lhs, const DatumUdt& rhs)
{
    return !(lhs <= rhs);
}

inline
bool bdld::operator>=(const DatumUdt& lhs, const DatumUdt& rhs)
{
    return !(lhs < rhs);
}

inline
bsl::ostream& bdld::operator<<(bsl::ostream& stream, const DatumUdt& rhs)
{
    return rhs.print(stream, 0 , -1);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------

