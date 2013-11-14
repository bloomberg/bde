// bslma_managedptrdeleter.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMA_MANAGEDPTRDELETER
#define INCLUDED_BSLMA_MANAGEDPTRDELETER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide an in-core value semantic class to call a delete function.
//
//@CLASSES:
//  bslma::ManagedPtrDeleter: in-core value semantic class storing a deleter
//
//@AUTHOR: Alisdair Meredith (ameredith1@bloomberg.net)
//
//@SEE_ALSO: bslma_managedptr
//
//@DESCRIPTION: This component provides a single, complex-constrained in-core
// value-semantic attribute class, 'ManagedPtrDeleter', that is used to store a
// bound function call for a "factory" to destroy an "object".
//
///Attributes
///----------
//..
//  Name              Type                      Default
//  ----------------  ------------------------  -------
//  object            void *                    0
//  factory           void *                    0
//  deleter           void (*)(void *, void *)  0
//
//  Complex Constraints
//  -----------------------------------------------------------------
//  '0 == deleter' or 'deleter(object, factory)' has defined behavior.
//..
//: o object  address of the object to be destroyed by the factory.
//:
//: o factory  address of the factory object that is responsible for destroying
//:            'object'
//:
//: o deleter  address of the function that knows how to restore the erased
//:            types of 'object' and 'factory', and how to invoke the 'factory'
//:            method to destroy object.


#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace bslma {

                       // =======================
                       // class ManagedPtrDeleter
                       // =======================

class ManagedPtrDeleter {
    // This complex constrained in-core value-semantic class holds the
    // information necessary for 'ManagedPtr' correctly manage its underlying
    // object, namely the addresses of 'object' and 'factory', and the
    // 'deleter' function, optionally supplied through the constructors or
    // through the 'set' method.  It is stored in a sub-structure to allow the
    // compiler to copy it more efficiently.

    // See the Attributes section under @DESCRIPTION in the component-level
    // documentation.  Note that the class invariants are identically the
    // constraints on the individual attributes.
    //
    // This class:
    //: o supports a complete set of *value-semantic* operations
    //: o is *exception-safe*
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

  public:
    // PUBLIC TYPES
    typedef void(*Deleter)(void *, void *);
        // Deleter function prototype used to destroy the managed pointer.

  private:
    // DATA
    void    *d_object_p;   // pointer to the actual managed object
    void    *d_factory_p;  // optional factory to be specified to the deleter
    Deleter  d_deleter;    // deleter used to destroy the managed object

  public:
    // CREATORS
    ManagedPtrDeleter();
        // Create an uninitialized 'ManagedPtrDeleter' object that does not
        // refer to any object or factory instance.

    //! ManagedPtrDeleter(const ManagedPtrDeleter& original);
        // Create a 'ManagedPtrDeleter' object having the same value as the
        // specified 'original' object.  Note that this trivial copy
        // constructor's definition is compiler generated.

    ManagedPtrDeleter(void *object, void *factory, Deleter deleter);
        // Create a 'ManagedPtrDeleter' struct that refers to the object and
        // factory instance located at the specified 'object' and 'factory'
        // memory locations, and the specified 'deleter'.  The behavior is
        // undefined unless 'deleter' is either 0, or points to a function
        // whose behavior is also defined if called once with 'object' and
        // 'factory' as arguments.

    //! ~ManagedPtrDeleter() = default;
        // Destroy this object.  Note that this trivial destructor's definition
        // is compiler generated.

    // MANIPULATORS
    //! ManagedPtrDeleter& operator=(const ManagedPtrDeleter& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that this trivial copy-assignment operator's definition is compiler
        // generated.

    void set(void *object, void *factory, Deleter deleter);
        // Set this 'ManagedPtrDeleter' to refer to the object and factory
        // instance located at the specified 'object' and 'factory' memory
        // locations, and to the specified 'deleter'.  The behavior is
        // undefined unless 'deleter' is either 0, or points to a function
        // whose behavior is also defined if called once with 'object' and
        // 'factory' as arguments.

    void clear();
        // Reset this 'ManagedPtrDeleter' to its uninitialized state.

    // ACCESSORS
    void deleteManagedObject() const;
        // Invoke the deleter object.  The behavior is undefined unless
        // 'deleter' is not 0.

    Deleter deleter() const;
        // Return the deleter function associated with this deleter.

    void *factory() const;
        // Return a pointer to the factory instance associated with this
        // deleter.

    void *object() const;
        // Return a pointer to the managed object associated with this deleter.
};

// FREE OPERATORS
bool operator==(const ManagedPtrDeleter& lhs, const ManagedPtrDeleter& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'ManagedPtrDeleter' objects have the
    // same value if the corresponding values of their 'object', 'factory' and
    // 'deleter' attributes are the same.

bool operator!=(const ManagedPtrDeleter& lhs, const ManagedPtrDeleter& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'ManagedPtrDeleter' objects do
    // not have the same value if any of the corresponding values of their
    // 'object', 'factory' and 'deleter' attributes are not the same.

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                       // -----------------------
                       // class ManagedPtrDeleter
                       // -----------------------

// CREATORS
inline
ManagedPtrDeleter::ManagedPtrDeleter()
: d_object_p(0)
, d_factory_p(0)
, d_deleter(0)
{
}

inline
ManagedPtrDeleter::ManagedPtrDeleter(void    *object,
                                     void    *factory,
                                     Deleter  deleter)
: d_object_p(object)
, d_factory_p(factory)
, d_deleter(deleter)
{
}

// MANIPULATORS
inline
void ManagedPtrDeleter::set(void *object, void *factory, Deleter deleter)
{
    d_object_p  = object;
    d_factory_p = factory;
    d_deleter   = deleter;
}

inline
void ManagedPtrDeleter::deleteManagedObject() const
{
    BSLS_ASSERT_OPT(0 != d_deleter);

    d_deleter(d_object_p, d_factory_p);
}

inline
void ManagedPtrDeleter::clear()
{
    d_object_p  = 0;
    d_factory_p = 0;
    d_deleter   = 0;
}

// ACCESSORS
inline
void *ManagedPtrDeleter::object() const
{
    return d_object_p;
}

inline
void *ManagedPtrDeleter::factory() const
{
    return d_factory_p;
}

inline
ManagedPtrDeleter::Deleter
ManagedPtrDeleter::deleter() const
{
    return d_deleter;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bslma::operator==(const ManagedPtrDeleter& lhs,
                       const ManagedPtrDeleter& rhs)
{
    return lhs.object()  == rhs.object()
        && lhs.factory() == rhs.factory()
        && lhs.deleter() == rhs.deleter();
}

inline
bool bslma::operator!=(const ManagedPtrDeleter& lhs,
                       const ManagedPtrDeleter& rhs)
{
    return lhs.object()  != rhs.object()
        || lhs.factory() != rhs.factory()
        || lhs.deleter() != rhs.deleter();
}

// TYPE TRAITS
namespace bslmf {

template <>
struct IsBitwiseMoveable<bslma::ManagedPtrDeleter>
    : bsl::integral_constant<bool, true> {};

}  // close package namespace
}  // close enterprise namespace
#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
