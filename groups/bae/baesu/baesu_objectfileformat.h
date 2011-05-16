// bdesu_objectfileformat.h                                           -*-C++-*-
#ifndef INCLUDED_BDESU_OBJECTFILEFORMAT
#define INCLUDED_BDESU_OBJECTFILEFORMAT

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide platform-dependent object file format trait definitions.
//
//@CLASSES:
//   bdesu_ObjectFileFormat: namespace for object file format traits
//
//@SEE_ALSO:
//
//@AUTHOR: Bill Chapman
//
//@DESCRIPTION: This component defines a set of traits that identify and
// describe a platform's object file format properties.  For example, the
// 'bdesu_ObjectFileFormat::ResolverPolicy' trait is ascribed a "value" (i.e.,
// 'Elf' or 'Xcoff') appropriate for each supported platform.  The various
// stack trace traits are actually types declared in the
// 'bdescu_ObjectFileFormat' 'struct'.  These types are intended to be used in
// specializing template implementations or to enable function overloading
// based on the prevalent system's characteristics.  #defines are also
// provided by this component to facilitate conditional compilation depending
// upon object file formats.
//
///Usage
///-----
// 'typeTest' is a template function that will return 1 if passed an object of
// types 'bdesu_ObjectFileFormat::{Elf,Xcoff,Windows}' appropriate for the
// current platform and 0 otherwise.
//..
//  template <typename TYPE>
//  int typeTest(const TYPE &) {
//      return 0;
//  }
//
//  #if defined(BDESU_OBJECTFILEFORMAT_RESOLVER_ELF)
//
//  int typeTest(const bdesu_ObjectFileFormat::Elf &)
//  {
//      return 1;
//  }
//
//  #endif
//  #if defined(BDESU_OBJECTFILEFORMAT_RESOLVER_XCOFF)
//
//  int typeTest(const bdesu_ObjectFileFormat::Xcoff &)
//  {
//      return 1;
//  }
//
//  #endif
//  #if defined(BDESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS)
//
//  int typeTest(const bdesu_ObjectFileFormat::Windows &)
//  {
//      return 1;
//  }
//
//  #endif
//
//  int main()
//  {
//..
// First, verify ResolverPolicy
//..
//    bdesu_ObjectFileFormat::ResolverPolicy policy;
//    BSLS_ASSERT(1 == typeTest(policy));
//..
// Finally, verify exactly 1 'BDESU_OBJECTFILEFORMAT_RESOLVER_*' #define
// exists
//..
//    int count = 0;
//  #if defined(BDESU_OBJECTFILEFORMAT_RESOLVER_ELF)
//    ++count;
//    BSLS_ASSERT(1 == BDESU_OBJECTFILEFORMAT_RESOLVER_ELF);
//  #endif
//  #if defined(BDESU_OBJECTFILEFORMAT_RESOLVER_XCOFF)
//    ++count;
//    BSLS_ASSERT(1 == BDESU_OBJECTFILEFORMAT_RESOLVER_XCOFF);
//  #endif
//  #if defined(BDESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS)
//    ++count;
//    BSLS_ASSERT(1 == BDESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS);
//  #endif
//    BSLS_ASSERT(1 == count);
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

namespace BloombergLP {

                        // ============================
                        // class bdesu_ObjectFileFormat
                        // ============================

struct bdesu_ObjectFileFormat {
    // This 'struct' provides a namespace for object file format trait
    // definitions, the 'Policy' trait, and within the code #defines are
    // defined to drive conditional compilation in client components.

    struct Elf {};        // format used on most Unix platforms

    struct Xcoff {};      // format used on some Unix platforms

    struct Windows {};    // format used on Microsoft Windows platform

#if defined(BSLS_PLATFORM__OS_SOLARIS) || \
    defined(BSLS_PLATFORM__OS_LINUX)   || \
    defined(BSLS_PLATFORM__OS_HPUX)

    typedef Elf Policy;
#   define BDESU_OBJECTFILEFORMAT_RESOLVER_ELF 1

#endif

#if defined(BSLS_PLATFORM__OS_AIX)

    typedef Xcoff Policy;
#   define BDESU_OBJECTFILEFORMAT_RESOLVER_XCOFF 1

#endif

#if defined(BSLS_PLATFORM__OS_WINDOWS)

    typedef Windows Policy;
#   define BDESU_OBJECTFILEFORMAT_RESOLVER_WINDOWS 1

#endif

};

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
