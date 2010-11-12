// bdeci_hashtable.cpp              -*-C++-*-
#include <bdeci_hashtable.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdeci_hashtable_cpp,"$Id$ $CSID$")

//
///IMPLEMENTATION NOTES
///--------------------
// This component has a physical size and a logical size.  The physical size
// determines the amount of memory allocated.  The logical size is the value
// used in most operations (e.g., for the number of slots in the hash function
// and in the comparison operator).  The logical size is at most as large as
// the physical size.
//
// The 'CAPACITY_RATIO' specifies - in an optimal situation - the maximum
// number of elements per slot.  In practice, it defines the maximum load
// factor (the number of elements divided by the number of slots.  When this
// maximum is reached, a resize is performed to lower the load factor.
//
// The "value" of this component is extremely arbitrary.  Arguably, this
// component should NOT have a value; it should be a mechanism.  However,
// since the copy constructor and assignment operator are required by other
// components (e.g, sets) an equality operator should be provided and hence a
// value must be provided.  The chosen value was determined as a convenience to
// support a fast 'operator=='.
//
// The chosen definition of value makes streaming inconvenient.  To effectively
// stream with minimal modification, the hashtable sizes provided by
// 'bdeci_HashtableImpUtil' can be added to the definition of the value.

#include <bsls_platformutil.h>                  // for testing only

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
