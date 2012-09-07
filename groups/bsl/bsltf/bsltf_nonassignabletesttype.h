// bsltf_nonassignabletesttype.h                                      -*-C++-*-
#ifndef INCLUDED_BSLTF_NONASSIGNABLETESTTYPE
#define INCLUDED_BSLTF_NONASSIGNABLETESTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class to which can not be assigned.
//
//@CLASSES:
//   bsltf::NonAssignableTestType: non-assignable test type
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@AUTHOR: Chen He (che2)
//
//@DESCRIPTION: This component provides a single, unconstrained
// (value-semantic) attribute class, 'NonAssignableTestType', that does not not
// support assignment.  This is particularly valuable when testing container
// operations that works with non-assignable types.
//
///Attributes
///----------
//..
//  Name                Type         Default
//  ------------------  -----------  -------
//  data                int          0
//..
//: o 'data': representation of the class value
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Demonstrating The Type Can't Be Assigned To
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wanted to show 'NonAssignableTestType' can't be assigned to:
//
// First, we create two 'NonAssignableTestType' objects, 'X' and 'Y':
//..
//  NonAssignableTestType X(1);
//  NonAssignableTestType Y(2);
//..
// Now, we show that assigning 'X' from Y will not compile:
//..
//  X = Y; // This will not compile
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {
namespace bsltf {

                        // ===========================
                        // class NonAssignableTestType
                        // ===========================

class NonAssignableTestType {
    // This unconstrained (value-semantic) attribute class does not
    // provide an assignment operator.

    // DATA
    int d_data;  // integer class value

  private:
    // NOT IMPLEMENTED
    NonAssignableTestType& operator=(const NonAssignableTestType& rhs);

  public:

    // CREATORS
    NonAssignableTestType();
        // Create a 'NonAssignableTestType' object having the (default)
        // attribute values:
        //..
        //  data() == 0
        //..

    explicit NonAssignableTestType(int data);
        // Create a 'NonAssignableTestType' object having the specified
        // 'data' attribute value.

    // NonAssignableTestType(const NonAssignableTestType& original) = default;
        // Create a 'SimpleTestType' object having the same value as the
        // specified 'original' object.

    // ~NonAssignableTestType() = default;
        // Destroy this object.

    // MANIPULATORS
    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.
};

// FREE OPERATORS
bool operator==(const NonAssignableTestType& lhs,
                const NonAssignableTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'NonAssignableTestType'
    // objects have the same if their 'data' attributes are the same.

bool operator!=(const NonAssignableTestType& lhs,
                const NonAssignableTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'NonAssignableTestType'
    // objects do not have the same value if their 'data' attributes are not
    // the same.

// ===========================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ===========================================================================

                        // ----------------------------------
                        // class NonAssignableTestType
                        // ----------------------------------

// CREATORS
inline
NonAssignableTestType::NonAssignableTestType()
: d_data(0)
{
}

inline
NonAssignableTestType::NonAssignableTestType(int data)
: d_data(data)
{
}

// MANIPULATORS
inline
void NonAssignableTestType::setData(int value)
{
    d_data = value;
}

// ACCESSORS
inline
int NonAssignableTestType::data() const
{
    return d_data;
}

// FREE OPERATORS
inline
bool operator==(const bsltf::NonAssignableTestType& lhs,
                const bsltf::NonAssignableTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool operator!=(const bsltf::NonAssignableTestType& lhs,
                const bsltf::NonAssignableTestType& rhs)
{
    return lhs.data() != rhs.data();
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
