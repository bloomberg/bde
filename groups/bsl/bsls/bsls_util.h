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
//  bsls::Util: utility class supplying essential, low-level functionality
//
//@MACROS:
//  BSLS_UTIL_ADDRESSOF(OBJ): address of 'OBJ', even if 'operator&' overloaded
//
//@DESCRIPTION: This component defines a utility 'struct', 'bsls::Util', that
// serves as a namespace for a suite of pure functions that supply essential
// low-level support for implementing portable generic facilities such as might
// be found in the C++ standard library.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Obtain the Address of a 'class' That Defines 'operator&'.
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
//      BitReference(char *byteptr = 0, int bitpos = 0)             // IMPLICIT
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
//      BitPointer(char *byteptr = 0, int bitpos = 0)               // IMPLICIT
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
// obvious syntax from working, we use 'bsls::Util::addressOf' to accomplish
// this task.
//
// Next, we create a 'BitReference' object:
//..
//  char c[4];
//  BitReference br(c, 3);
//..
// Now, we invoke 'bsls::Util::addressOf' to obtain and save the address of
// 'br':
//..
//  BitReference *p = bsls::Util::addressOf(br);  // OK
//  // BitReference *p = &br;                     // Won't compile
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

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

namespace BloombergLP {

namespace bsls {

template <class TYPE>
struct Util_Identity {
    // This class template provides an easy way to alias a function pointer
    // type when used as the return type of a function.  The syntax for a
    // function returning a function pointer is otherwise quite obscure, and
    // difficult to read.  As we want to return function pointers taking
    // parameters and returning a result specified by template parameters
    // below, it is not possible to define a simple typedef to the function
    // type outside the function template itself.

    typedef TYPE type;  // alias of the template parameter 'TYPE'.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class TYPE>
struct Util_RemoveReference {
    typedef TYPE type;
};

template <class TYPE>
struct Util_RemoveReference<TYPE&> {
    typedef TYPE type;
};

template <class TYPE>
struct Util_RemoveReference<TYPE&&> {
    typedef TYPE type;
};

#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

                                 // ===========
                                 // struct Util
                                 // ===========

struct Util {
    // This 'struct' provides a namespace for essential low-level functions for
    // implementing portable generic facilities such as the C++ standard
    // library.

    // CLASS METHODS
    template <class TYPE>
    static TYPE *addressOf(TYPE& obj);
        // Return the address of the specified 'obj', even if 'operator&' is
        // overloaded for objects of type 'BSLS_TYPE'.  Behavior is undefined
        // unless 'BSLS_TYPE' is an object type.  Note that this function
        // conforms to the C++11 definition for 'addressof' as specified in the
        // section [specialized.addressof] (20.6.12.1) of the C++11 standard,
        // except that function types, which are not object types, are
        // supported by 'std::addressof' in C++11.

    template <class RESULT>
    static
    typename Util_Identity<RESULT()>::type *addressOf(RESULT (&fn)());
    template <class RESULT, class ARG>
    static
    typename Util_Identity<RESULT(ARG)>::type *addressOf(RESULT (&fn)(ARG));
    template <class RESULT, class ARG1, class ARG2>
    static
    typename Util_Identity<RESULT(ARG1, ARG2)>::type *addressOf(
                                                     RESULT (&fn)(ARG1, ARG2));
        // Return the address of the specified function 'fn'.  Note that this
        // implementation supports functions of only a limited number of
        // parameters, determined by the current needs of the BDE software.  A
        // more general form that will support an arbitrary number of function
        // parameters will be available with C++11.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    template <class TYPE>
    static TYPE&& forward(typename Util_RemoveReference<TYPE>::type&  t);
    template <class TYPE>
    static TYPE&& forward(typename Util_RemoveReference<TYPE>::type&& t);
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
};

}  // close package namespace

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
#ifdef BSLS_PLATFORM_CMP_MSVC
#   define BSLS_UTIL_ADDRESSOF(OBJ) ::BloombergLP::bsls::Util::addressOf(OBJ)

#   if !defined(BDE_USE_ADDRESSOF)
#       define BDE_USE_ADDRESSOF
#   endif
#else
#   define BSLS_UTIL_ADDRESSOF(OBJ) (&(OBJ))
#endif

namespace bsls {

// This macro takes the address of an object by calling 'Util::addressOf' on
// Windows, and simply taking the address with the '&' operator on all other
// platforms.

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

// CLASS METHODS
template <class TYPE>
inline
TYPE *Util::addressOf(TYPE& obj)
{
    return static_cast<TYPE *>(
        static_cast<void *>(
            const_cast<char *>(&reinterpret_cast<const volatile char&>(obj))));
}

template <class RESULT>
inline
typename Util_Identity<RESULT()>::type *
Util::addressOf(RESULT (&fn)())
{
    return fn;
}

template <class RESULT, class ARG>
inline
typename Util_Identity<RESULT(ARG)>::type *
Util::addressOf(RESULT (&fn)(ARG))
{
    return fn;
}

template <class RESULT, class ARG1, class ARG2>
inline
typename Util_Identity<RESULT(ARG1, ARG2)>::type *
Util::addressOf(RESULT (&fn)(ARG1, ARG2))
{
    return fn;
}

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
template <class TYPE>
TYPE&& Util::forward(typename Util_RemoveReference<TYPE>::type& t)
{
    return static_cast<TYPE&&>(t);
}

template <class TYPE>
TYPE&& Util::forward(typename Util_RemoveReference<TYPE>::type&& t)
{
    return static_cast<TYPE&&>(t);
}
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
