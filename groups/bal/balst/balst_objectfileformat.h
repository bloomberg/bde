// balst_objectfileformat.h                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALST_OBJECTFILEFORMAT
#define INCLUDED_BALST_OBJECTFILEFORMAT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide platform-dependent object file format trait definitions.
//
//@CLASSES:
//   balst::ObjectFileFormat: namespace for object file format traits
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a set of traits that identify and
// describe a platform's object file format properties.  For example, the
// 'balst::ObjectFileFormat::ResolverPolicy' trait is ascribed a "value" (i.e.,
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
// In this section we show the intended usage of this component.
//
///Example 1: Accessing 'balst::ObjectFileFormat' Information at Run Time
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The templated (specialized) 'typeTest' function returns a unique, non-zero
// value when passed an object of types
// 'balst::ObjectFileFormat::{Elf,Xcoff,Windows}', and 0 otherwise.
//..
//  template <typename TYPE>
//  int typeTest(const TYPE &)
//  {
//      return 0;
//  }
//
//  int typeTest(const balst::ObjectFileFormat::Elf &)
//  {
//      return 1;
//  }
//
//  int typeTest(const balst::ObjectFileFormat::Xcoff &)
//  {
//      return 2;
//  }
//
//  int typeTest(const balst::ObjectFileFormat::Windows &)
//  {
//      return 3;
//  }
//
//  int main() ...
//..
// We define an object 'policy' of type 'balst::ObjectFileFormat::Policy',
// which will be of type '...::Elf', '...::Xcoff', or '...::Windows'
// appropriate for the platform.
//..
//      balst::ObjectFileFormat::Policy policy;
//..
// We now test it using 'typeTest':
//..
//      assert(typeTest(policy) > 0);
//
//  #if defined(BALST_OBJECTFILEFORMAT_RESOLVER_ELF)
//      assert(1 == typeTest(policy));
//  #endif
//
//  #if defined(BALST_OBJECTFILEFORMAT_RESOLVER_XCOFF)
//      assert(2 == typeTest(policy));
//  #endif
//
//  #if defined(BALST_OBJECTFILEFORMAT_RESOLVER_WINDOWS)
//      assert(3 == typeTest(policy));
//  #endif
//  }
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

namespace BloombergLP {

namespace balst {
                           // ======================
                           // class ObjectFileFormat
                           // ======================

struct ObjectFileFormat {
    // This 'struct' is named 'ObjectFileFormat' for historical reasons, what
    // it really determines is resolving strategy.  Linux, for example, can be
    // resolved using either the 'Elf' or 'Dladdr' policies.  We choose 'Elf'
    // for linux because that mode of resolving yields more information.

    struct Elf {};        // resolve as elf object

    struct Xcoff {};      // resolve as xcoff object

    struct Windows {};    // format used on Microsoft Windows platform

    struct Dladdr {};     // resulve using the 'dladdr' call

    struct Dummy {};

#if defined(BSLS_PLATFORM_OS_SOLARIS) || \
    defined(BSLS_PLATFORM_OS_LINUX)   || \
    defined(BSLS_PLATFORM_OS_HPUX)

    typedef Elf Policy;
#   define BALST_OBJECTFILEFORMAT_RESOLVER_ELF 1

#elif defined(BSLS_PLATFORM_OS_AIX)

    typedef Xcoff Policy;
#   define BALST_OBJECTFILEFORMAT_RESOLVER_XCOFF 1

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    typedef Windows Policy;
#   define BALST_OBJECTFILEFORMAT_RESOLVER_WINDOWS 1

#elif defined(BSLS_PLATFORM_OS_DARWIN)

    typedef Dladdr Policy;
#   define BALST_OBJECTFILEFORMAT_RESOLVER_DLADDR 1

#else

    typedef Dummy Policy;
#   error unrecognized platform
#   define BALST_OBJECTFILEFORMAT_RESOLVER_UNIMPLEMENTED 1

#endif

};
}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
