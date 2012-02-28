// bdema_managedptr.cpp                                               -*-C++-*-
#include <bdema_managedptr.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdema_managedptr_cpp,"$Id$ $CSID$")

// Design notes
// ------------
// These notes are recorded here as they are not part of the public interface,
// but reflect certain design decisions taken when implementing this component.
// A number of redundant constructors were removed from earlier designs in
// order to produce the leanest component with minimal template bloat.  One
// side effect of this is that some signatures, such as the constructor for
// converting from a managed-pointer-of-a-different-type, are implicit and so
// no longer clearly documented in a clear place of their own.
// A second design decision was to implement aliasing as member-function
// templates taking references to arbitrary managed pointer types.  The other
// approach considered was to take 'bdema_ManagedPtr_Ref' objects by value,
// like the move operations.  This was ruled out due to the static-assert in
// the conversion-to-_Ref operator, that enforces a type-compatibility
// relationship between the aliased type and the managed pointer target type.
// This restriction is not present in the current contract, and the risks
// opened by removing the static-assert and allowing general conversions
// everywhere (as undefined behavior) were seen as too large.

namespace BloombergLP {

                    // ----------------------------------
                    // struct bdema_ManagedPtrNoOpDeleter
                    // ----------------------------------

void bdema_ManagedPtrUtil::noOpDeleter(void *, void *)
{
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
