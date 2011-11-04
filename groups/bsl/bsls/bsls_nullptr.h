// bsls_nullptr.h                                                       -*-C++-*-
#ifndef INCLUDED_BSLS_NULLPTR
#define INCLUDED_BSLS_NULLPTR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a distinct type for null pointer literals.
//
//@CLASSES:
//   bsls_Nullptr: namespace for a type matching only null pointer literals
//
//@AUTHOR: Alisdair Meredith (ameredit)
//
//@DESCRIPTION: This component provides a limited emulation of the C++11 type,
// 'std::nullptr_t', which can be used as a function parameter type to help
// overload sets treat null pointer literals specially.
//
///Usage
///-----

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

namespace BloombergLP {

                       // ==============================
                       // class bsls_Nullptr
                       // ==============================

struct bsls_Nullptr {
    // This 'struct' provides an alias for a type that can match a null pointer
    // literal, but is not a pointer itself.  It is a limited emulation of the
    // C++11 'std::nullptr_t' type.

  private:
    struct Nullptr_ProxyType { int dummy; }; // private class to supply a
                                             // unique pointer-to-member type.

  public:
    typedef int Nullptr_ProxyType::* Type;   // alias to an "unspecified" null
                                             // pointer type.
};

}  // close namespace BloombergLP

namespace bsl
{
    typedef BloombergLP::bsls_Nullptr::Type nullptr_t;
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
