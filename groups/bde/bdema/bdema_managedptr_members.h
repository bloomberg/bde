// bdema_managedptr_members.h                                         -*-C++-*-
#ifndef INCLUDED_BDEMA_MANAGEDPTR_MEMBERS
#define INCLUDED_BDEMA_MANAGEDPTR_MEMBERS

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a managed pointer class.
//
//@CLASSES:
//        bdema_ManagedPtr_Members: internal state of a bcema_ManagedPtr object
//
//@AUTHOR: Alisdair Meredith (ameredith1@bloomberg.net)
//
//@SEE_ALSO: bdema_managedptr
//
//@DESCRIPTION: This component provides a class to store and manage the
// internal state of a 'bdema_ManagedPtr' object.  It enforces the rules for
// correct transfer of ownership from one Members object to another.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEMA_MANAGEDPTRDELETER
#include <bdema_managedptrdeleter.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

namespace BloombergLP {

                     // ==============================
                     // class bdema_ManagedPtr_Members
                     // ==============================

class bdema_ManagedPtr_Members {
    // Non-type-specific managed pointer member variables.  This type exists
    // so that a 'bdema_ManagedPtr_Ref' can point to the representation of a
    // 'bdema_ManagedPtr' even if the '_Ref' object is instantiated on a
    // different type than the managed pointer type (e.g., in the case of
    // conversions or aliasing).  'bdema_ManagerPtr_Members' also "erases" the
    // types of each member, so that the same object code can be shared between
    // multiple instantiations of the 'bdema_ManagedPtr' class template,
    // reducing template bloat.  Note that objects of this type may have an
    // "unset" state, where the 'd_obj_p' pointer has a null value, and the
    // 'd_deleter' member does not have a specified value.  If 'd_obj_p' has
    // a null pointer value, then this object must be in an unset state.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdema_ManagedPtr_Members,
                                 bslalg_TypeTraitBitwiseMoveable);

  private:
    // PRIVATE TYPES
    typedef bdema_ManagedPtrDeleter::Deleter DeleterFunc;

    void                    *d_obj_p;     // pointer to the managed instance.
                                          // A void pointer is used so that a
                                          // bdem_ManagedPtrRef of a different
                                          // type can point to this pointer.
    bdema_ManagedPtrDeleter  d_deleter;   // deleter-related information

  private:
    // NOT IMPLEMENTED
    bdema_ManagedPtr_Members& operator=(const bdema_ManagedPtr_Members&);

  public:
    //CREATORS

    bdema_ManagedPtr_Members();
        // Create a 'bdema_ManagedPtr_Members' object in an unset state.  Sets
        // 'd_obj_p' to 0, and default constructs 'd_deleter'.

    explicit bdema_ManagedPtr_Members(bdema_ManagedPtr_Members& other);
        // Create a 'bdema_ManagedPtr_Members' object having the same 'd_obj_p'
        // and, if that 'd_obj_p' is not null, 'd_deleter' values as the
        // specified 'other', and then put 'other' into an unset state.

    bdema_ManagedPtr_Members(void *object, void *factory, DeleterFunc deleter);
        // If 'object' is null, create a 'bdema_ManagedPtr_Members' object that
        // does not manage a pointer; otherwise create a
        // 'bdema_ManagedPtr_Members' object having the specified 'object',
        // 'factory' and 'deleter'.

    bdema_ManagedPtr_Members(void        *object,
                             void        *factory,
                             DeleterFunc  deleter,
                             void        *alias);
        // If 'object' is null, create a 'bdema_ManagedPtr_Members' object that
        // does not manage a pointer; otherwise create a
        // 'bdema_ManagedPtr_Members' object having the specified 'object',
        // 'factory' and 'deleter', but aliasing 'alias'.  Note that this
        // constructor is important for managed pointers pointing to one of
        // multiple base classes of a class using multiple inheritance.

    //! ~bdema_ManagedPtr_Members() = default;
        // Destroy this 'bdema_ManagedPtr_Members' object.  Note that this
        // trivial destructor's definition is compiler generated.

    //MANIPULATORS

    void clear();
        // Reset this object to an unset state.  Note that any previously
        // managed object will not be destroyed.

    void move(bdema_ManagedPtr_Members& other);
        // Re-initialize this object, having the same 'd_obj_p' and, if that
        // 'd_obj_p' is not null, 'd_deleter' values as the specified 'other',
        // and then put 'other' into an unset set.  Note that any previously
        // managed object will not be destroyed.

    void set(void *object, void *factory, DeleterFunc deleter);
        // Re-initialize this object with the specified 'object' pointer
        // value, and the specified 'deleter' function, using the specified
        // 'factory'.  Note that any previously managed object will not be
        // destroyed.

    void setAliasPtr(void *ptr);
        // Set 'd_obj_p' to have the specified 'ptr' value.  If 'ptr' is null
        // then this object will have an unset state.

    void swap(bdema_ManagedPtr_Members& other);
        // Efficiently exchange the state of this object with the state of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  Note that if either object is in an
        // unset state, there are no guarantees about the unset state that may
        // be exchanged, other than 'd_obj_p' shall be null.

    //ACCESSORS

    void runDeleter() const;
        // Destroy the currently managed object(if any).  Note that calling
        // this method twice without assigning a new pointer to manage will
        // produce undefined behavior, unless this object's current deleter
        // specifically supports such usage.

    void *pointer() const;
        // Return a pointer to the currently managed object.

    const bdema_ManagedPtrDeleter& deleter() const;
        // Return the 'bdema_ManagedPtrDeleter' object that should be used to
        // destroy the currently managed object, if any.  Behavior is
        // undefined unless 'pointer' is not null.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                    // ------------------------------
                    // class bdema_ManagedPtr_Members
                    // ------------------------------

// CREATORS
inline
bdema_ManagedPtr_Members::bdema_ManagedPtr_Members()
: d_obj_p(0)
{
}

inline
bdema_ManagedPtr_Members::bdema_ManagedPtr_Members(void *object,
                                                   void *factory,
                                                   DeleterFunc deleter)
: d_obj_p(object)
, d_deleter(object, factory, deleter)
{
    BSLS_ASSERT_SAFE(0 != deleter || 0 == object);
}

inline
bdema_ManagedPtr_Members::bdema_ManagedPtr_Members(void        *object,
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
void bdema_ManagedPtr_Members::setAliasPtr(void *ptr)
{
    BSLS_ASSERT_SAFE(!ptr == !d_obj_p); // both are null, or neither is null

    d_obj_p = ptr;
}

inline
void bdema_ManagedPtr_Members::runDeleter() const
{
    if (d_obj_p) {
        d_deleter.deleteManagedObject();
    }
}

inline
void *bdema_ManagedPtr_Members::pointer() const
{
    return d_obj_p;
}

inline
const bdema_ManagedPtrDeleter& bdema_ManagedPtr_Members::deleter() const
{
    BSLS_ASSERT_SAFE(d_obj_p);

    return d_deleter;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
