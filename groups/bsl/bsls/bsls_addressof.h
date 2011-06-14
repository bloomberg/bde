// bsls_addressof.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLS_ADDRESSOF
#define INCLUDED_BSLS_ADDRESSOF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Return the address of an object, even if 'operator&' is overloaded
//
//@FREE FUNCTIONS: T *bsl::addressof(T&)
//
//@SEE_ALSO:
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: There are times, especially within low-level library
// functions, where it is necessary to obtain the address of an object even if
// that object's class overloads 'operator&' to return something other than
// the object's address.  The 'bsl::addressof' function defined in this
// component provides this ability.  It conforms to the C++0x definition for
// 'addressof' as specified in the section [specialized.addressof] (20.6.12.1)
// of the FDIS.
//
///Usage
///-----
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
// address of a 'BitReference'.  Unfortunately, the above overload prevents
// the obvious syntax from working:
//..
//  int main()
//  {
//      char c[4];
//      BitReference br(c, 3);
//      // BitReference *p = &br;  // Won't compile
//..
// The 'addressof' function was defined for exactly this purpose.  It returns
// the address of an object even of that object's class overloads 'operator&':
//..
//      BitReference *p = bsl::addressof(br);  // OK
//      assert(0 != p);
//      assert(p->byteptr() == c);
//      assert(p->bitpos()  == 3);
//
//      return 0;
//  }
//..

namespace bsl {

                        // =======================
                        // Free Function addressof
                        // =======================

template <class TYPE>
inline
TYPE *addressof(TYPE& obj);
    // Return the address of the specified 'obj', even in the presence of an
    // overloaded 'operator&' for 'TYPE'.

}  // close namespace bsl

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

// FREE FUNCTIONS
template <class TYPE>
inline
TYPE *bsl::addressof(TYPE& obj)
{
    return static_cast<TYPE *>(
        static_cast<void *>(
            const_cast<char *>(&reinterpret_cast<const volatile char&>(obj))));
}

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
