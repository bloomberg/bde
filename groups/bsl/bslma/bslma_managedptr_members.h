// bslma_managedptr_members.h                                         -*-C++-*-
#ifndef INCLUDED_BSLMA_MANAGEDPTR_MEMBERS
#define INCLUDED_BSLMA_MANAGEDPTR_MEMBERS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide the internal state of a managed pointer class.
//
//@CLASSES:
//  bslma::ManagedPtr_Members: internal state of a 'bslma::ManagedPtr' object
//
//@SEE_ALSO: bslma_managedptr
//
//@DESCRIPTION: This component provides a class, 'bslma::ManagedPtr_Members',
// to store and manage the internal state of a 'bslma::ManagedPtr' object.  It
// enforces the rules for correct transfer of ownership from one
// 'bslma::ManagedPtr' object to another.  A 'bslma::ManagedPtr_Members' object
// has two attributes:
//: o pointer - address of the object being managed, or an alias to the object
//:
//: o deleter - address of a function used to destroy the managed object

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDPTRDELETER
#include <bslma_managedptrdeleter.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace bslma {

                     // ========================
                     // class ManagedPtr_Members
                     // ========================

class ManagedPtr_Members {
    // This class provides a type-agnostic container for managed pointer data
    // members.  This type exists so that a 'ManagedPtr_Ref' (see
    // 'bslma_managedptr') can point to the representation of a 'ManagedPtr'
    // even if the 'ManagedPtr_Ref' object is instantiated on a different type
    // than the managed pointer type (e.g., in the case of conversions or
    // aliasing).  'ManagedPtr_Members' also "erases" the type of each member,
    // so that the same object code can be shared between all instantiations of
    // the 'ManagedPtr' class template, reducing template bloat.  Note that
    // objects of this type have an "unset" state, where the pointer member has
    // a null value and the deleter member has an unspecified value.

  private:
    // PRIVATE TYPES
    typedef ManagedPtrDeleter::Deleter DeleterFunc;

    // DATA
    void              *d_obj_p;    // pointer to the managed object, or an
                                   // alias to the object; 'void *' is used so
                                   // that this single (non-template) class may
                                   // be used for any instantiation of
                                   // 'bslma::ManagedPtr'

    ManagedPtrDeleter  d_deleter;  // deleter-related information

  private:
    // NOT IMPLEMENTED
    ManagedPtr_Members& operator=(const ManagedPtr_Members&);

  public:
    // CREATORS
    ManagedPtr_Members();
        // Create a 'ManagedPtr_Members' object in an unset state, i.e., where
        // 'pointer' is null and 'deleter' has an unspecified value.

    explicit ManagedPtr_Members(ManagedPtr_Members& other);
        // Create a 'ManagedPtr_Members' object having the same 'pointer' as
        // the specified 'other' object, and, if '0 != other.pointer()', the
        // same deleter as 'other', then put 'other' into an unset state.

    ManagedPtr_Members(void *object, void *factory, DeleterFunc deleter);
        // Create a 'ManagedPtr_Members' object having the specified 'object',
        // 'factory', and 'deleter'.  The newly created object does not manage
        // a pointer if '0 == object'.  The behavior is undefined unless
        // 'object' is null if 'deleter' is null.

    ManagedPtr_Members(void        *object,
                       void        *factory,
                       DeleterFunc  deleter,
                       void        *alias);
        // Create a 'ManagedPtr_Members' object having the specified 'object',
        // 'factory', 'deleter', and 'alias'.  The newly created object does
        // not manage a pointer if '0 == object'.  The behavior is undefined
        // unless 'object' is null if 'deleter' is null, and '0 == object' if
        // and only if '0 == alias'.  Note that this constructor is important
        // for managed pointers pointing to one of multiple base classes of a
        // class using multiple inheritance.

    //! ~ManagedPtr_Members() = default;
        // Destroy this object.  Note that this trivial destructor's definition
        // is compiler generated.

    // MANIPULATORS
    void clear();
        // Reset this object to an unset state with no effect on the previously
        // managed object (if any).

    void move(ManagedPtr_Members *other);
        // Reset this object to have the same 'pointer' as the specified
        // 'other' object, and, if '0 != other->pointer()', the same deleter as
        // 'other', then put 'other' into an unset state.  This method has no
        // effect on the previously managed object (if any).  The behavior is
        // undefined unless 'this != other'.

    void moveAssign(ManagedPtr_Members *other);
        // Destroy the currently managed object (if any), reset this object to
        // have the same 'pointer' as the specified 'other' object, and, if
        // '0 != other->pointer()', the same deleter as 'other', then put
        // 'other' into an unset state.  This method has no effect if
        // 'this == other'.

    void set(void *object, void *factory, DeleterFunc deleter);
        // Reset this object to have the specified 'object', 'factory', and
        // 'deleter' with no effect on the previously managed object (if any).
        // The object does not manage a pointer if '0 == object'.  The behavior
        // is undefined unless 'object' is null if 'deleter' is null.

    void setAliasPtr(void *alias);
        // Set 'pointer' to have the specified 'alias'.  If '0 == alias', then
        // this object will have an unset state with no effect on the
        // previously managed object (if any).  The behavior is undefined
        // unless '0 == pointer()' if and only if '0 == alias'.

    void swap(ManagedPtr_Members& other);
        // Efficiently exchange the state of this object with the state of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  Note that if either object is in an
        // unset state, then the only guarantee about the unset state that may
        // be exchanged is that the 'pointer' value shall be null.

    // ACCESSORS
    const ManagedPtrDeleter& deleter() const;
        // Return a reference providing non-modifiable access to the deleter
        // used to destroy the currently managed object (if any).  The behavior
        // is undefined unless 'pointer' is not null.

    void *pointer() const;
        // Return the address providing modifiable access to the currently
        // managed object, or 0 if this object is in an unset state.  Note that
        // the returned pointer may be an alias to the managed object.

    void runDeleter() const;
        // Destroy the currently managed object (if any).  Note that calling
        // this method twice on an object that is not in an unset state,
        // without assigning a new pointer to manage between the two calls,
        // results in undefined behavior unless this object's deleter
        // specifically supports such usage.
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                    // ------------------------
                    // class ManagedPtr_Members
                    // ------------------------

// CREATORS
inline
ManagedPtr_Members::ManagedPtr_Members()
: d_obj_p(0)
{
}

inline
ManagedPtr_Members::ManagedPtr_Members(ManagedPtr_Members& other)
: d_obj_p(other.d_obj_p)
{
    if (d_obj_p) {
        d_deleter = other.d_deleter;
    }
    other.clear();
}

inline
ManagedPtr_Members::ManagedPtr_Members(void        *object,
                                       void        *factory,
                                       DeleterFunc  deleter)
: d_obj_p(object)
, d_deleter(object, factory, deleter)
{
    BSLS_ASSERT_SAFE(0 != deleter || 0 == object);
}

inline
ManagedPtr_Members::ManagedPtr_Members(void        *object,
                                       void        *factory,
                                       DeleterFunc  deleter,
                                       void        *alias)
: d_obj_p(alias)
, d_deleter(object, factory, deleter)
{
    BSLS_ASSERT_SAFE(0 != deleter || 0 == object);
    BSLS_ASSERT_SAFE(!object == !alias);  // both are null, or neither is null
}

// MANIPULATORS
inline
void ManagedPtr_Members::clear()
{
    d_obj_p = 0;
}

inline
void ManagedPtr_Members::move(ManagedPtr_Members *other)
{
    BSLS_ASSERT_SAFE(other);
    BSLS_ASSERT_SAFE(this != other);

    // If 'other->d_obj_p' is null, then 'other->d_deleter' has an unspecified
    // value.

    d_obj_p = other->d_obj_p;
    if (other->d_obj_p) {
        d_deleter = other->d_deleter;
    }

    other->clear();
}

inline
void ManagedPtr_Members::moveAssign(ManagedPtr_Members *other)
{
    BSLS_ASSERT_SAFE(other);

    // Must protect against self-assignment due to destructive move.

    if (this != other) {
        runDeleter();
        move(other);
    }
}

inline
void ManagedPtr_Members::set(void *object, void *factory, DeleterFunc deleter)
{
    BSLS_ASSERT_SAFE(0 != deleter || 0 == object);

    // Note that 'factory' may be null if 'deleter' supports it, so that cannot
    // be asserted here.

    d_obj_p = object;
    if (object) {
        d_deleter.set(object, factory, deleter);
    }
}

inline
void ManagedPtr_Members::setAliasPtr(void *alias)
{
    BSLS_ASSERT_SAFE(!alias == !d_obj_p);  // both are null, or neither is null

    d_obj_p = alias;
}

// ACCESSORS
inline
const ManagedPtrDeleter& ManagedPtr_Members::deleter() const
{
    BSLS_ASSERT_SAFE(d_obj_p);

    return d_deleter;
}

inline
void *ManagedPtr_Members::pointer() const
{
    return d_obj_p;
}

inline
void ManagedPtr_Members::runDeleter() const
{
    if (d_obj_p) {
        d_deleter.deleteManagedObject();
    }
}

}  // close package namespace

// TYPE TRAITS
namespace bslmf {

template <>
struct IsBitwiseMoveable<bslma::ManagedPtr_Members> : bsl::true_type
{
};

}  // close namespace bslmf
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
