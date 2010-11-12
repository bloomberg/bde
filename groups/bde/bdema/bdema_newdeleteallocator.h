// bdema_newdeleteallocator.h   -*-C++-*-
#ifndef INCLUDED_BDEMA_NEWDELETEALLOCATOR
#define INCLUDED_BDEMA_NEWDELETEALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide singleton new/delete adaptor to 'bdema_Allocator' protocol.
//
//@DEPRECATED: Use 'bslma_newdeleteallocator' instead.
//
//@CLASSES:
//   bdema_NewDeleteAllocator: support new/delete style allocation/deallocation
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component implements the 'bdema_Allocator' protocol to
// provide the user with direct access to the (system-supplied) global 'new'
// and 'delete' operators via that interface.
//..
//                    ( bdema_NewDeleteAllocator )
//                                 |              ctor/dtor
//                                 |              allocator
//                                 v              singleton
//                        ( bdema_Allocator )
//                                          allocate
//                                          deallocate
//..
// The key purpose of this component is to facilitate the (default) use of
// global 'new' and 'delete' in all 'bde' components that accept a
// user-supplied allocator derived from 'bdema_Allocator'.  To accomplish this
// goal, 'new' and 'delete' are wrapped in a singleton instance of this
// component whose lifetime is guaranteed to exceed any possibility of its use.
//
///Thread Safety
///-------------
// A single object of 'bdema_NewDeleteAllocator' is safe for concurrent
// access by multiple threads.  The underlying implementation of 'new' and
// 'delete' will ensure that heap corruption does not occur.  Note that this
// allocator therefore has a stronger thread safety guarantee than is
// required by the base-class contract or than is provided by other
// allocators.
//
///Usage
///-----
// The most common "usage" of 'bdema_NewDeleteAllocator' is by default, when
// constructing, for example, a container such as 'my_ShortArray'.  The
// following snippet of code creates an array that obtains its memory from
// global operator 'new', but through the 'bdema_Allocator' protocol.
//..
//   my_ShortArray a;
//..
// In the above example, the array 'a' is constructed via the default
// constructor for class 'my_ShortArray' defined in 'my_shortarray.h' below:
//..
//  // my_shortarray.h:
//  // ...
//  class bdema_Allocator;
//
//  class my_ShortArray {
//      short *d_array_p; // dynamically-allocated array of short integers
//      int d_size;       // physical size of the 'd_array_p' array (elements)
//      int d_length;     // logical length of the 'd_array_p' array (elements)
//      bdema_Allocator *d_allocator_p;  // holds (but doesn't own) allocator
//
//    public:
//      my_ShortArray(bdema_Allocator *basicAllocator = 0);
//          // Create a empty array.  Optionally specify a 'basicAllocator'
//          // used to supply memory.  If 'basicAllocator' is 0, the
//          // currently installed default allocator is used.
//     // ...
//  }
//..
// This default constructor is implemented in the 'my_shortarray.cpp'
// file as follows:
//..
//  // my_shortarray.cpp:
//  #include <my_shortarray.h>
//  #include <bdema_newdeleteallocator.h>
//
//  //  enum { INITIAL_SIZE = 1, GROW_FACTOR = 2 };
//
//  // ...
//
//  my_ShortArray::my_ShortArray(bdema_Allocator *basicAllocator)
//  : d_size(INITIAL_SIZE)
//  , d_length(0)
//  , d_allocator_p(bdema_NewDeleteAllocator::allocator(basicAllocator))
//      // The above initialization expression is equivalent to 'basicAllocator
//      // ? basicAllocator : &bdema_NewDeleteAllocator::singleton()'.
//  {
//      assert(d_allocator_p);
//      d_array_p = (short *)
//                  d_allocator_p->allocate(d_size * sizeof *d_array_p);
//  }
//
// // ...
//..
// When the constructor is called, the default size and length are recorded in
// data members via the initialization list.  The static function 'allocator'
// (provided in 'bdema_NewDeleteAllocator' as a convenience) is used to assign
// the value of the 'basicAllocator' address passed in, or if that is 0,
// the address of the singleton instance of a 'bdema_NewDeleteAllocator'
// defined in this component.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLMA_NEWDELETEALLOCATOR
#include <bslma_newdeleteallocator.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

typedef bslma_NewDeleteAllocator bdema_NewDeleteAllocator;
    // This class provides direct access to the (system-supplied) global
    // operators 'new' and 'delete'.  A static method is provided for obtaining
    // a singleton instance that is valid from the time the method is called
    // until after the program (not just 'main') exits.

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
