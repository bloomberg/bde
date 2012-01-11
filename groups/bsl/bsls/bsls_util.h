// bsls_util.h                                                        -*-C++-*-
#ifndef INCLUDED_BSLS_UTIL
#define INCLUDED_BSLS_UTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide essential, low-level support for portable generic code.
//
//@CLASSES:
//  bsls_Util: utility class supplying essential, low-level functionality
//
//@AUTHOR: Pablo Halpern (phalpern), Alisdair Meredith (ameredit)
//
//@DESCRIPTION: This component defines a utility 'struct', 'bsls_Util', that
// serves as a namespace for a suite of pure functions that supply essential
// low-level support for implementing portable generic facilities such as might
// be found in the C++ standard library.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Obtain the address of a 'class' that defines 'operator&'.
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// There are times, especially within low-level library functions, where it is
// necessary to obtain the address of an object even if that object's class
// overloads 'operator&' to return something other than the object's address.
//
// First, we create a special reference-like type that can refer to a single
// bit within a byte (inline implementations are provided in class scope for
// ease of exposition):
//..
//  class BitReference {
//
//      // DATA
//      char *d_byte_p;
//      int   d_bitpos;
//
//    public:
//      // CREATORS
//      BitReference(char *byteptr = 0, int bitpos = 0)
//      : d_byte_p(byteptr)
//      , d_bitpos(bitpos)
//      {
//      }
//
//      // ACCESSORS
//      operator bool() const { return (*d_byte_p >> d_bitpos) & 1; }
//
//      char *byteptr() const { return d_byte_p; }
//      int bitpos() const { return d_bitpos; }
//  };
//..
// Then, we create a pointer-like type that can point to a single bit:
//..
//  class BitPointer {
//
//      // DATA
//      char *d_byte_p;
//      int   d_bitpos;
//
//    public:
//      // CREATORS
//      BitPointer(char *byteptr = 0, int bitpos = 0)
//      : d_byte_p(byteptr)
//      , d_bitpos(bitpos)
//      {
//      }
//
//      // ACCESSORS
//      BitReference operator*() const
//      {
//          return BitReference(d_byte_p, d_bitpos);
//      }
//
//      // etc.
//  };
//..
// Next, we overload 'operator&' for 'BitReference' to return a 'BitPointer'
// instead of a raw pointer, completing the setup:
//..
//  inline BitPointer operator&(const BitReference& ref)
//  {
//      return BitPointer(ref.byteptr(), ref.bitpos());
//  }
//..
// Then, we note that there are times when it might be desirable to get the
// true address of a 'BitReference'.  Since the above overload prevents the
// obvious syntax from working, we use 'bsls_Util::addressOf' to accomplish
// this task.
//
// Next, we create a 'BitReference' object:
//..
//  char c[4];
//  BitReference br(c, 3);
//..
// Now, we invoke 'bsls_Util::addressOf' to obtain and save the address of
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

                               // ================
                               // struct bsls_Util
                               // ================

namespace BloombergLP {

struct bsls_Util {
    // This 'struct' provides a namespace for essential low-level functions
    // for implementing portable generic facilities such as the C++ standard
    // library.

    // CLASS METHODS
    template <class BSLS_TYPE>
    static BSLS_TYPE *addressOf(BSLS_TYPE& obj);
        // Return the address of the specified 'obj', even if 'operator&' is
        // overloaded for objects of type 'BSLS_TYPE'.  Behavior is undefined
        // unless 'BSLS_TYPE' is an object type.  Note that this function
        // conforms to the C++11 definition for 'addressof' as specified in
        // the section [specialized.addressof] (20.6.12.1) of the C++11
        // standard, except that function types, which are not object types,
        // are supported by 'std::addressof' in C++11.
};

                                   // ======
                                   // MACROS
                                   // ======

// The following macros are private to the BDE implementation and not intended
// for widespread use.  They support the BDE STL decision for the standard
// containers to support types that overload 'operator&' only on the Microsoft
// platform.  This support is provided on Microsoft to enable containers
// holding the 'CComPtr' type from the Microsoft Foundation Class library
// (which overloads 'operator&'), but is not provided on UNIX platforms to
// avoid additional template bloat in the 'big' only to support a class design
// that is almost certainly an error.
#ifdef BSLS_PLATFORM__CMP_MSVC
#   define BSLS_UTIL_ADDRESSOF(OBJ) ::BloombergLP::bsls_Util::addressOf(OBJ)

#   if !defined(BDE_USE_ADDRESSOF)
#       define BDE_USE_ADDRESSOF
#   endif
#else
#   define BSLS_UTIL_ADDRESSOF(OBJ) (&(OBJ))
#endif
// This macro takes the address of an object by calling 'bsls_Util::addressOf'
// on Windows, and simply taking the address with the '&' operator on all other
// platforms.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

// CLASS METHODS
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
