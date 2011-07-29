// bsls_addressof.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLS_ADDRESSOF
#define INCLUDED_BSLS_ADDRESSOF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Return the address of an object, even if 'operator&' is overloaded
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION:   The 'bsls_addressOf' function defined in this
// component provides this ability.  It conforms to the C++0x definition for
// 'addressof' as specified in the section [specialized.addressof] (20.6.12.1)
// of the FDIS.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Obtain the address of a 'class' that defines 'operator&'.
// There are times, especially within low-level library functions, where it is
// necessary to obtain the address of an object even if that object's class
// overloads 'operator&' to return something other than the object's address.
//
// Assume we have a special reference-like type that can refer to a single bit:
//..
//  class BitReference {
//      char *d_byteptr;
//      int   d_bitpos;
//
//  public:
//      BitReference(char *byteptr = 0, int bitpos = 0)
//          : d_byteptr(byteptr), d_bitpos(bitpos) { }
//
//      operator bool() const { return (*d_byteptr >> d_bitpos) & 1; }
//
//      char *byteptr() const { return d_byteptr; }
//      int bitpos() const { return d_bitpos; }
//  };
//..
// and a pointer-like type that can point to a single bit:
//..
//  class BitPointer {
//      char *d_byteptr;
//      int   d_bitpos;
//
//  public:
//      BitPointer(char *byteptr = 0, int bitpos = 0)
//          : d_byteptr(byteptr), d_bitpos(bitpos) { }
//
//      BitReference operator*() const
//          { return BitReference(d_byteptr, d_bitpos); }
//
//      // etc.
//  };
//..
// To complete the picture, we overload 'operator&' for 'BitReference' to
// return a 'BitPointer' instead of a raw pointer:
//..
//  inline BitPointer operator&(const BitReference& ref) {
//      return BitPointer(ref.byteptr(), ref.bitpos());
//  }
//..
// However, there are times when it might be desirable to get the true
// address of a 'BitReference'.  Since the above overload prevents
// the obvious syntax from working, we use 'bsls_addressof' to accomplish this
// task.
//
// First, we create a 'BitReference' object:
//..
//  char c[4];
//  BitReference br(c, 3);
//..
// Now, we invoke 'bsls_addressOf' to obtain and save the address of 'br:
//..
//  BitReference *p = bsls_addressOf(br);  // OK
//  // BitReference *p = &br;              // Won't compile
//..
// Notice that the commented line illustrates canonical use of 'operator&' that
// would not compile in this example.
//
// Finally, we verify that address obtained is the correct one, running some
// sanity checks:
//..
//  assert(0 != p);
//  assert(p->byteptr() == c);
//  assert(p->bitpos()  == 3);
//..
                               // ==============
                               // Free Functions
                               // ==============

namespace BloombergLP {

template <class TYPE>
inline
TYPE *bsls_addressOf(TYPE& obj);
    // Return the address of the specified 'obj', even in the presence of an
    // overloaded 'operator&' for 'TYPE'.

                                   // ======
                                   // MACROS
                                   // ======

#ifdef BSLS_PLATFORM__CMP_MSVC 
#   define BSLS_ADDRESSOF(OBJ)     ::BloombergLP::bsls_addressOf(OBJ)

#   if !defined(BDE_USE_ADDRESSOF)
#       define BDE_USE_ADDRESSOF
#   endif
#else
#   define BSLS_ADDRESSOF(OBJ)     (&(OBJ))
#endif

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

// FREE FUNCTIONS
template <class TYPE>
inline
TYPE *bsls_addressOf(TYPE& obj)
{
    return static_cast<TYPE *>(
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
