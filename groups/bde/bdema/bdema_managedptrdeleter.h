// bdema_managedptr.h                                                 -*-C++-*-
#ifndef INCLUDED_BDEMA_MANAGEDPTRDELETER
#define INCLUDED_BDEMA_MANAGEDPTRDELETER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an in-core value semantic class to call a delete function.
//
//@CLASSES:
//  bdema_ManagedPtrDeleter: in-core value semantic class storing a deleter
//
//@AUTHOR: Alisdair Meredith (ameredith1@bloomberg.net)
//
//@SEE_ALSO: bdema_managedptr
//
//@DESCRIPTION: This component provides a single, complex-constrained in-core
// value-semantic attribute class, 'bdema_ManagedPtrDeleter', that is used to
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
//..
//: o object: address of the object to be destroyed by the factory.
//:
//: o factory: address of the factory object that is reponsible for destroying
//:            'object'
//: o deleter: address of the function that knows how to restore the erased
//:            types of 'object' and 'factory', and how to invoke the 'factory'
//:            method to destroy object.
//..
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Setting Linger Options
///- - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to set linger options:
//..
//  bdema_ManagedPtrDeleter lingerOptions;
//
//  // Set the lingering option with a timeout of 2 seconds.
//  lingerOptions.setUseLingeringFlag(true);
//  lingerOptions.setTimeout(2);
//..
// We can then set these linger options on any socket handle using
// 'bteso_SocketOptions'.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                       // =============================
                       // class bdema_ManagedPtrDeleter
                       // =============================

class bdema_ManagedPtrDeleter {
    // This complex constrained in-core value-semantic class  holds the
    // information necessary for 'bdema_ManagedPtr' correctly manage its
    // underlying object, namely the addresses of 'object' and 'factory', and
    // the 'deleter' function, optionally supplied through the constructors or
    // through the 'set' method.  It is stored in a sub-structure to allow the
    // compiler to copy it more efficiently.
    
//    //See the Attributes section under @DESCRIPTION in the
//    // component-level documentation.  Note that the class invariants are
//    // identically the constraints on the individual attributes.
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

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdema_ManagedPtrDeleter,
                                 bslalg_TypeTraitBitwiseMoveable);

  private:
    // DATA
    void    *d_object_p;   // pointer to the actual managed object
    void    *d_factory_p;  // optional factory to be specified to the deleter
    Deleter  d_deleter;    // deleter used to destroy the managed object

  public:
    // CREATORS
    bdema_ManagedPtrDeleter();
        // Create an uninitialized 'bdema_ManagedPtrDeleter' object that does
        // not refer to any object or factory instance.

    //! bdema_ManagedPtrDeleter(const bdema_ManagedPtrDeleter& original);
        // Create a 'bdema_ManagedPtrDeleter' object having the same value as
        // the specified 'original' object.  Note that this trivial copy
        // constructor's definition is compiler generated.

    bdema_ManagedPtrDeleter(void *object, void *factory, Deleter deleter);
        // Create a 'bdema_ManagedPtrDeleter' struct that refers to the object
        // and factory instance located at the specified 'object' and 'factory'
        // memory locations, and the specified 'deleter'.

    //! ~bdema_ManagedPtrDeleter() = default;
        // Destroy this object.  Note that this trivial destructor's definition
        // is compiler generated.

    // MANIPULATORS
    //! bdema_ManagedPtrDeleter& operator=(const bdema_ManagedPtrDeleter& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that this trivial copy-assignment operator's definintion is compiler
        // generated.

    void set(void *object, void *factory, Deleter deleter);
        // Set this 'bdema_ManagedPtrDeleter' to refer to the object and
        // factory instance located at the specified 'object' and 'factory'
        // memory locations, and to the specified 'deleter'.  Note that 
        // 'object' and 'factory' may be null if and only if the specified
        // 'deleter' function permits null pointers.

    void clear();
        // Reset this 'bdema_ManagedPtrDeleter' to its uninitialized state.


    // ACCESSORS
    void deleteManagedObject() const;
        // Invoke the deleter object.  Behavior is not defined unless this
        // object has a deleter.

    Deleter deleter() const;
        // Return the deleter function associated with this deleter.

    void *factory() const;
        // Return a pointer to the factory instance associated with this
        // deleter.

    void *object() const;
        // Return a pointer to the managed object associated with this deleter.

                                  // Aspects

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in
        // a human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute
        // value indicates the number of spaces per indentation level for this
        // and all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  Note that the
        // format is not fully specified, and can change without notice.
};

// FREE OPERATORS
inline
bool operator==(const bdema_ManagedPtrDeleter& lhs,
                const bdema_ManagedPtrDeleter& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'bdema_ManagedPtrDeleter' objects
    // have the same value if the corresponding values of their 'object',
    // 'factory' and 'deleter' attributes are the same.

inline
bool operator!=(const bdema_ManagedPtrDeleter& lhs,
                const bdema_ManagedPtrDeleter& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'bdema_ManagedPtrDeleter'
    // objects do not have the same value if any of the corresponding values of
    // their 'object', 'factory' and 'deleter' attributes are not the same.

bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const bdema_ManagedPtrDeleter& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified and can change
    // without notice.  Also note that this method has the same behavior as
    // 'object.print(stream, 0, -1)' with the attribute names elided.

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                       // -----------------------------
                       // class bdema_ManagedPtrDeleter
                       // -----------------------------

// CREATORS
inline
bdema_ManagedPtrDeleter::bdema_ManagedPtrDeleter()
: d_object_p(0)
, d_factory_p(0)
, d_deleter(0)
{
}

inline
bdema_ManagedPtrDeleter::bdema_ManagedPtrDeleter(void    *object,
                                                 void    *factory,
                                                 Deleter  deleter)
: d_object_p(object)
, d_factory_p(factory)
, d_deleter(deleter)
{
}

// MANIPULATORS
inline
void bdema_ManagedPtrDeleter::set(void *object, void *factory, Deleter deleter)
{
    d_object_p  = object;
    d_factory_p = factory;
    d_deleter   = deleter;
}

inline
void bdema_ManagedPtrDeleter::deleteManagedObject() const
{
    BSLS_ASSERT_SAFE(0 != d_deleter);

    d_deleter(d_object_p, d_factory_p);
}

inline
void bdema_ManagedPtrDeleter::clear()
{
    d_object_p  = 0;
    d_factory_p = 0;
    d_deleter   = 0;
}

// ACCESSORS
inline
void *bdema_ManagedPtrDeleter::object() const
{
    return d_object_p;
}

inline
void *bdema_ManagedPtrDeleter::factory() const
{
    return d_factory_p;
}

inline
bdema_ManagedPtrDeleter::Deleter
bdema_ManagedPtrDeleter::deleter() const
{
    return d_deleter;
}

// FREE OPERATORS
inline
bool operator==(const bdema_ManagedPtrDeleter& lhs,
                const bdema_ManagedPtrDeleter& rhs)
{
    return lhs.object()  == rhs.object()
        && lhs.factory() == rhs.factory()
        && lhs.deleter() == rhs.deleter();
}

inline
bool operator!=(const bdema_ManagedPtrDeleter& lhs,
                const bdema_ManagedPtrDeleter& rhs)
{
    return lhs.object()  != rhs.object()
        || lhs.factory() != rhs.factory()
        || lhs.deleter() != rhs.deleter();
}

}  // close namespace BloombergLP
#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
