// bsltf_noncopyconstructibletesttype.h                               -*-C++-*-
#ifndef INCLUDED_BSLTF_NONCOPYCONSTRUCTIBLETESTTYPE
#define INCLUDED_BSLTF_NONCOPYCONSTRUCTIBLETESTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an attribute class that is not copy constructable.
//
//@CLASSES:
//   bsltf::NonCopyConstructibleTestType: non-copy-constructible test type
//
//@AUTHOR: Chen He (che2)
//
//@SEE ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides a single, unconstrained
// (value-semantic) attribute class, 'NonCopyConstructibleTestType', that does
// not provide a copy constructor.  This is particularly valuable when testing
// container operations that works with non-copy-constructible types.
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
///Example 1: Demonstrating The Type Can't Be Copy Constructed
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wanted to show 'NonCopyConstructibleTestType' can't be copy
// constructed:
//
// First, we create a 'NonCopyConstructibleTestType' object, 'X':
//..
//  NonCopyConstructibleTestType X;
//..
// Now, we show that copy constructing another object from 'X' will not
// compile:
//..
//  NonCopyConstructibleTestType Y(X); // This will not compile
//..

namespace BloombergLP {
namespace bsltf {

                        // ==================================
                        // class NonCopyConstructibleTestType
                        // ==================================

class NonCopyConstructibleTestType {
    // This unconstrained (value-semantic) attribute class does not
    // provide a copy constructor.

    // DATA
    int d_data;  // integer class value

  private:
    // NOT IMPLEMENTED
    NonCopyConstructibleTestType(const NonCopyConstructibleTestType&);

  public:

    // CREATORS
    NonCopyConstructibleTestType();
        // Create a 'NonCopyConstructibleTestType' object having the (default)
        // attribute values:
        //..
        //  data() == 0
        //..

    explicit NonCopyConstructibleTestType(int data);
        // Create a 'NonCopyConstructibleTestType' object having the specified
        // 'data' attribute value.

    // ~NonCopyConstructibleTestType() = default;
        // Destroy this object.

    // MANIPULATORS
    // NonCopyConstructibleTestType& operator=(
    //                      const NonCopyConstructibleTestType& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.
};

// FREE OPERATORS
bool operator==(const NonCopyConstructibleTestType& lhs,
                const NonCopyConstructibleTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'NonCopyConstructibleTestType'
    // objects have the same if their 'data' attributes are the same.

bool operator!=(const NonCopyConstructibleTestType& lhs,
                const NonCopyConstructibleTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'NonCopyConstructibleTestType'
    // objects do not have the same value if their 'data' attributes are not
    // the same.

// ===========================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ===========================================================================

                        // ----------------------------------
                        // class NonCopyConstructibleTestType
                        // ----------------------------------

// CREATORS
inline
NonCopyConstructibleTestType::NonCopyConstructibleTestType()
: d_data(0)
{
}

inline
NonCopyConstructibleTestType::NonCopyConstructibleTestType(int data)
: d_data(data)
{
}

// MANIPULATORS
inline
void NonCopyConstructibleTestType::setData(int value)
{
    d_data = value;
}

// ACCESSORS
inline
int NonCopyConstructibleTestType::data() const
{
    return d_data;
}

// FREE OPERATORS
inline
bool operator==(const bsltf::NonCopyConstructibleTestType& lhs,
                const bsltf::NonCopyConstructibleTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool operator!=(const bsltf::NonCopyConstructibleTestType& lhs,
                const bsltf::NonCopyConstructibleTestType& rhs)
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
