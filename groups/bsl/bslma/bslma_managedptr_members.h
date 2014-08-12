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
//  bslma::ManagedPtr_Members: internal state of a bcema_ManagedPtr object
//
//@AUTHOR: Alisdair Meredith (ameredith1@bloomberg.net)
//
//@SEE_ALSO: bslma_managedptr
//
//@DESCRIPTION: This component provides a class to store and manage the
// internal state of a 'bslma::ManagedPtr' object.  It enforces the rules for
// correct transfer of ownership from one 'bslma::ManagedPtr_Members' object to
// another.

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

#ifdef BDE_VERIFY
#pragma bde_verify set ok_unquoted object
#endif // BDE_VERIFY

namespace BloombergLP {
namespace bslma {

                     // ========================
                     // class ManagedPtr_Members
                     // ========================

class ManagedPtr_Members {
    // Non-type-specific managed pointer member variables.  This type exists so
    // that a 'ManagedPtr_Ref' can point to the representation of a
    // 'ManagedPtr' even if the '_Ref' object is instantiated on a different
    // type than the managed pointer type (e.g., in the case of conversions or
    // aliasing).  'ManagerPtr_Members' also "erases" the types of each member,
    // so that the same object code can be shared between multiple
    // instantiations of the 'ManagedPtr' class template, reducing template
    // bloat.  Note that objects of this type may have an "unset" state, where
    // the 'd_obj_p' pointer has a null value, and the 'd_deleter' member does
    // not have a specified value.  If 'd_obj_p' has a null pointer value, then
    // this object must be in an unset state.

  private:
    // PRIVATE TYPES
    typedef ManagedPtrDeleter::Deleter DeleterFunc;

    void              *d_obj_p;     // pointer to the managed object.
                                    // A pointer to void is used so that this
                                    // single (non-template) class may be used
                                    // for any instantiation of the
                                    // 'bslma::ManagedPtrRef' template.

    ManagedPtrDeleter  d_deleter;   // deleter-related information

  private:
    // NOT IMPLEMENTED
    ManagedPtr_Members& operator=(const ManagedPtr_Members&);

  public:
    // CREATORS
    ManagedPtr_Members();
        // Create a 'ManagedPtr_Members' object in an unset state.  Sets
        // 'pointer' to 0.

    explicit ManagedPtr_Members(ManagedPtr_Members& other);
        // Create a 'ManagedPtr_Members' object having the same 'pointer' as
        // the specified 'other' object, and, if 'pointer' is not 0, the same
        // deleter as 'other', and then put 'other' into an unset state.

    ManagedPtr_Members(void *object, void *factory, DeleterFunc deleter);
        // Create a 'ManagedPtr_Members' object having the specified 'object',
        // 'factory' and 'deleter' unless '0 == object', in which case create a
        // 'ManagedPtr_Members' object that does not manage a pointer.

    ManagedPtr_Members(void        *object,
                       void        *factory,
                       DeleterFunc  deleter,
                       void        *alias);
        // Create a 'ManagedPtr_Members' object having the specified 'object',
        // 'factory', and 'deleter', but aliasing the specified 'alias', unless
        // '0 == object'; if '0 == object' create a 'ManagedPtr_Members' object
        // that does not manage any pointer.  Note that this constructor is
        // important for managed pointers pointing to one of multiple base
        // classes of a class using multiple inheritance.

    //! ~ManagedPtr_Members() = default;
        // Destroy this 'ManagedPtr_Members' object.  Note that this trivial
        // destructor's definition is compiler generated.

    // MANIPULATORS
    void clear();
        // Reset this object to an unset state.  Note that any previously
        // managed object will not be destroyed.

    void move(ManagedPtr_Members *other);
        // Re-initialize this object,  having the same 'pointer' as the
        // specified 'other' object, and, if 'pointer' is not 0, the same
        // deleter as 'other', and then put 'other' into an unset set.  Note
        // that any previously managed object will not be destroyed.

    void moveAssign(ManagedPtr_Members *other);
        // Destroy the currently managed object (if any) unless the specified
        // 'other' refers to this object, then re-initialize this object,
        // having the same 'pointer' as the 'other' object, and, if 'pointer'
        // is not 0, the same deleter as 'other', and then put 'other' into an
        // unset set.

    void set(void *object, void *factory, DeleterFunc deleter);
        // Re-initialize this object with the specified 'object' pointer value,
        // and the specified 'deleter' function, using the specified 'factory'.
        // Note that any previously managed object will not be destroyed.

    void setAliasPtr(void *ptr);
        // Set 'pointer' to have the specified 'ptr' value.  If '0 == ptr' then
        // this object will have an unset state.

    void swap(ManagedPtr_Members& other);
        // Efficiently exchange the state of this object with the state of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  Note that if either object is in an
        // unset state, then the only guarantee about the unset state that may
        // be exchanged is that the 'pointer' value shall be null.

    // ACCESSORS
    const ManagedPtrDeleter& deleter() const;
        // Return the 'ManagedPtrDeleter' object that should be used to destroy
        // the currently managed object, if any.  Behavior is undefined unless
        // 'pointer' is not null.

    void *pointer() const;
        // Return a pointer to the currently managed object.

    void runDeleter() const;
        // Destroy the currently managed object(if any).  Note that calling
        // this method twice without assigning a new pointer to manage will
        // produce undefined behavior, unless this object's current deleter
        // specifically supports such usage.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
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
}


// MANIPULATORS
inline
void ManagedPtr_Members::setAliasPtr(void *ptr)
{
    BSLS_ASSERT_SAFE(!ptr == !d_obj_p); // both are null, or neither is null

    d_obj_p = ptr;
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
struct IsBitwiseMoveable<bslma::ManagedPtr_Members> : bsl::true_type {};

}  // close package namespace
}  // close enterprise namespace

#endif


// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg Finance L.P.
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
