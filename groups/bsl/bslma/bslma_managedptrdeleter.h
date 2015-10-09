// bslma_managedptrdeleter.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMA_MANAGEDPTRDELETER
#define INCLUDED_BSLMA_MANAGEDPTRDELETER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide an in-core value-semantic class to call a delete function.
//
//@CLASSES:
//  bslma::ManagedPtrDeleter: in-core value-semantic class storing a deleter
//
//@SEE_ALSO: bslma_managedptr
//
//@DESCRIPTION: This component provides a single, complex-constrained in-core
// value-semantic attribute class, 'bslma::ManagedPtrDeleter', that is used to
// store a bound function call for a "factory" to destroy an "object".
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
    typedef void(*Deleter)(void *managedObject, void *cookie);
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

    void clear();
        // Reset this 'ManagedPtrDeleter' to its uninitialized state.

    void set(void *object, void *factory, Deleter deleter);
        // Set this 'ManagedPtrDeleter' to refer to the specified 'object', to
        // the specified 'factory', and to the specified 'deleter'.  The
        // behavior is undefined unless 'deleter' is either 0, or points to a
        // function whose behavior is also defined if called once with 'object'
        // and 'factory' as arguments.

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
//                      INLINE FUNCTION DEFINITIONS
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
void ManagedPtrDeleter::clear()
{
    d_object_p  = 0;
    d_factory_p = 0;
    d_deleter   = 0;
}

inline
void ManagedPtrDeleter::set(void *object, void *factory, Deleter deleter)
{
    d_object_p  = object;
    d_factory_p = factory;
    d_deleter   = deleter;
}

// ACCESSORS
inline
void ManagedPtrDeleter::deleteManagedObject() const
{
    BSLS_ASSERT_SAFE(0 != d_deleter);

    d_deleter(d_object_p, d_factory_p);
}

inline
ManagedPtrDeleter::Deleter
ManagedPtrDeleter::deleter() const
{
    return d_deleter;
}

inline
void *ManagedPtrDeleter::factory() const
{
    return d_factory_p;
}

inline
void *ManagedPtrDeleter::object() const
{
    return d_object_p;
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

}  // close namespace bslmf
}  // close enterprise namespace
#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
