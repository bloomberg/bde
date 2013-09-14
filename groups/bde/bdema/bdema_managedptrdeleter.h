// bdema_managedptrdeleter.h                                          -*-C++-*-
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


#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDPTRDELETER
#include <bslma_managedptrdeleter.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#endif

namespace BloombergLP {

typedef bslma::ManagedPtrDeleter bdema_ManagedPtrDeleter;

bsl::ostream& operator<<(bsl::ostream&                  stream,
                         const bdema_ManagedPtrDeleter& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified and can change
    // without notice.  Also note that this method has the same behavior as
    // 'object.print(stream, 0, -1)' with the attribute names elided.

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
