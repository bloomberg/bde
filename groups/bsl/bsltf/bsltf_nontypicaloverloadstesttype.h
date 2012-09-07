// bsltf_nontypicaloverloadstesttype.h                                -*-C++-*-
#ifndef INCLUDED_BSLSTL_NONTYPICALOVERLOADSTESTTYPE
#define INCLUDED_BSLSTL_NONTYPICALOVERLOADSTESTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class that overloads the non-typical operators.
//
//@CLASSES:
//   bsltf::NonTypicalOverloadsTestType: test class that non-typical operators
//
//@AUTHOR: Chen He (che2)
//
//@DESCRIPTION: This component provides a single, unconstrained
// (value-semantic) attribute class, 'NonTypicalOverloadsTestType', that
// defines overloads to some non-typically-overloaded operators ('operator
// new', 'operator delete', and 'operator&') to ensure that they do not get
// invoked.
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
///Example 1: Demonstrating Calling Operator New/Delete Induce Assertions
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we wanted to show that calling 'operator new' and
// 'operator delete on 'NonTypicalOverloadsTestType' causes an assertion.
//
// First, we verify that calling 'operator new' will result in an
// assertion:
//..
//  bsls::AssertFailureHandlerGuard g(bsls::AssertTest::failTestDriver);
//  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(new NonTypicalOverloadsTestType());
//..
// Finally, we verify that calling 'operator delete' will result in an
// assertion:
//..
//  NonTypicalOverloadsTestType obj;
//  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(delete obj);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>       // for 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {
namespace bsltf {

                        // =================================
                        // class NonTypicalOverloadsTestType
                        // =================================

class NonTypicalOverloadsTestType {
    // This unconstrained (value-semantic) attribute class ensures that the
    // operator new member and operator delete member never gets called by
    // overloading and asserting in them.  See the Attributes section under
    // @DESCRIPTION in the component-level documentation for information on the
    // class attributes.

    // DATA
    int d_data;

  public:
    // CLASS METHODS
    static void* operator new(std::size_t size);
        // Overload 'operator new' and assert this method is not called.

    static void* operator new(std::size_t size, void *ptr);
        // Overload in place 'new' and assert this method is not called.

    static void operator delete(void *ptr);
        // Overload 'operator delete' and assert this method is not called.

    // CREATORS
    explicit NonTypicalOverloadsTestType();
        // Create a 'NonTypicalOverloadsTestType' object having the (default)
        // attribute values:
        //..
        //  data() == 0
        //..

    NonTypicalOverloadsTestType(int data);
        // Create a 'NonTypicalOverloadsTestType' object having the specified
        // 'data' attribute value.

    NonTypicalOverloadsTestType(const NonTypicalOverloadsTestType& original);
        // Create a 'NonTypicalOverloadsTestType' object having the same value
        // as the specified 'original' object.

    // ~NonTypicalOverloadsTestType() = default;
        // Destroy this object.

    // MANIPULATORS
    NonTypicalOverloadsTestType& operator=(
                                       const NonTypicalOverloadsTestType& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.


    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'.


#ifdef BSLS_PLATFORM__CMP_MSVC
    // ACCESSORS
    NonTypicalOverloadsTestType* operator&();
        // Overload 'operator&' and assert this method is not called.  This is
        // defined only on the Microsoft platform, because we in general do not
        // support types that overrides 'operator&' in our standard containers
        // implementation except on Windows.  See the component-level
        // documentation of bsls_util for more information.
#endif

    int data() const;
        // Return the value of the 'data' attribute of this object.
};

// FREE OPERATORS
bool operator==(const NonTypicalOverloadsTestType& lhs,
                const NonTypicalOverloadsTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'NonTypicalOverloadsTestType' objects
    // have the same if their 'data' attributes are the same.

bool operator!=(const NonTypicalOverloadsTestType& lhs,
                const NonTypicalOverloadsTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'NonTypicalOverloadsTestType'
    // objects do not have the same value if their 'data' attributes are not
    // the same.

// ===========================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ===========================================================================

                        // ---------------------------------
                        // class NonTypicalOverloadsTestType
                        // ---------------------------------

// CLASS METHODS
inline
void* NonTypicalOverloadsTestType::operator new(std::size_t size)
{
    BSLS_ASSERT_OPT(0);
    return ::operator new(size);
}

inline
void* NonTypicalOverloadsTestType::operator new(std::size_t, void *ptr)
{
    BSLS_ASSERT_OPT(0);
    return ptr;
}

inline
void NonTypicalOverloadsTestType::operator delete(void *)
{
    BSLS_ASSERT_OPT(0);
}

// CREATORS
inline
NonTypicalOverloadsTestType::NonTypicalOverloadsTestType()
: d_data(0)
{
}

inline
NonTypicalOverloadsTestType::NonTypicalOverloadsTestType(int data)
: d_data(data)
{
}

inline
NonTypicalOverloadsTestType::NonTypicalOverloadsTestType(
                                   const NonTypicalOverloadsTestType& original)
: d_data(original.d_data)
{
}

// MANIPULATORS
inline
NonTypicalOverloadsTestType& NonTypicalOverloadsTestType::operator=(
                                        const NonTypicalOverloadsTestType& rhs)
{
    d_data = rhs.d_data;
    return *this;
}

inline
void NonTypicalOverloadsTestType::setData(int value)
{
    d_data = value;
}

#ifdef BSLS_PLATFORM__CMP_MSVC
// ACCESSORS
inline
NonTypicalOverloadsTestType* NonTypicalOverloadsTestType::operator&()
{
    BSLS_ASSERT_OPT(0);
    return 0;
}
#endif

inline
int NonTypicalOverloadsTestType::data() const
{
    return d_data;
}

// FREE OPERATORS
inline
bool operator==(const NonTypicalOverloadsTestType& lhs,
                const NonTypicalOverloadsTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool operator!=(const NonTypicalOverloadsTestType& lhs,
                const NonTypicalOverloadsTestType& rhs)
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
