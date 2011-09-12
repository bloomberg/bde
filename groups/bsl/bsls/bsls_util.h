// bsls_util.h                                                        -*-C++-*-
#ifndef INCLUDED_BSLS_UTIL
#define INCLUDED_BSLS_UTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide essential utilities supporting portable generic code.
//
//@CLASSES:
//  bsls_Util : utility class supplying essential, portable functionality
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component supplies essential low-level utilities for
// implementing generic facilities like the C++ standard library.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Obtain the address of a 'class' that defines 'operator&'.
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// There are times, especially within low-level library functions, where it is
// necessary to obtain the address of an object even if that object's class
// overloads 'operator&' to return something other than the object's address.
//
// First we create a special reference-like type that can refer to a single
// bit within a byte:
//..
//  class BitReference {
//      // DATA
//      char *d_byteptr_p;
//      int   d_bitpos;
//
//    public:
//      // CREATORS
//      BitReference(char *byteptr = 0, int bitpos = 0)
//      : d_byteptr_p(byteptr)
//      , d_bitpos(bitpos)
//      {
//      }
//
//      // ACCESSORS
//      operator bool() const { return (*d_byteptr_p >> d_bitpos) & 1; }
//
//      char *byteptr() const { return d_byteptr_p; }
//      int bitpos() const { return d_bitpos; }
//  };
//..
// Then, we create a pointer-like type that can point to a single bit:
//..
//  class BitPointer {
//      // DATA
//      char *d_byteptr_p;
//      int   d_bitpos;
//
//    public:
//      // CREATORS
//      BitPointer(char *byteptr = 0, int bitpos = 0)
//      : d_byteptr_p(byteptr)
//      , d_bitpos(bitpos)
//      { 
//      }
//
//      // ACCESSORS
//      BitReference operator*() const
//          { return BitReference(d_byteptr_p, d_bitpos); }
//
//      // etc.
//  };
//..
// Next, we overload 'operator&' for 'BitReference' to return a 'BitPointer'
// instead of a raw pointer, completing the picture:
//..
//  inline BitPointer operator&(const BitReference& ref) {
//      return BitPointer(ref.byteptr(), ref.bitpos());
//  }
//..
// Finally, we note that there are times when it might be desirable to get the
// true address of a 'BitReference'.  Since the above overload prevents the
// obvious syntax from working, we use 'bsls_Util::addressOf' to accomplish
// this task.
//
// First, we create a 'BitReference' object:
//..
//  char c[4];
//  BitReference br(c, 3);
//..
// Then, we invoke 'bsls_Util::addressOf' to obtain and save the address of
// 'br':
//..
//  BitReference *p = bsls_Util::addressOf(br);  // OK
//  // BitReference *p = &br;                    // Won't compile
//..
// Notice that the commented line illustrates canonical use of 'operator&' that
// would not compile in this example.
//
// Finally, we verify that address obtained is the correct one, running some
// sanity checks:
//..
//  assert(0 != p);
//  assert(c == p->byteptr());
//  assert(3 == p->bitpos());
//..
                               // ==============
                               // Free Functions
                               // ==============

namespace BloombergLP {

struct bsls_Util {
    // This utility class supplies essential low-level utilities for
    // implementing generic facilities like the C++ standard library.

    template <class BSLS_TYPE>
    static BSLS_TYPE *addressOf(BSLS_TYPE& obj);
        // Return the address of the specified 'obj', even if 'operator&' is
        // overloaded for objects of type 'BSLS_TYPE'.  This function conforms
        // to the C++11 definition for 'addressof' as specified in the section
        // [specialized.addressof] (20.6.12.1) of the C++11 standard, except
        // that behaviour is undefined if 'BSLS_TYPE' is not an object type.
};

                                   // ======
                                   // MACROS
                                   // ======

#ifdef BSLS_PLATFORM__CMP_MSVC
#   define BSLS_UTIL_ADDRESSOF(OBJ) ::BloombergLP::bsls_Util::addressOf(OBJ)

#   if !defined(BDE_USE_ADDRESSOF)
#       define BDE_USE_ADDRESSOF
#   endif
#else
#   define BSLS_UTIL_ADDRESSOF(OBJ) (&(OBJ))
#endif

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

// FREE FUNCTIONS
template <class BSLS_TYPE>
inline
BSLS_TYPE *bsls_Util::addressOf(BSLS_TYPE& obj)
{
    return static_cast<BSLS_TYPE *>(
        static_cast<void *>(
            const_cast<char *>(&reinterpret_cast<const volatile char&>(obj))));
}

}  // close enterprise namespace

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
