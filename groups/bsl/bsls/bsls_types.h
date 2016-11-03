// bsls_types.h                                                       -*-C++-*-
#ifndef INCLUDED_BSLS_TYPES
#define INCLUDED_BSLS_TYPES

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a consistent interface for platform-dependent types.
//
//@CLASSES:
//  bsls::Types: namespace for platform-neutral type names
//
//@DESCRIPTION: This component provides a namespace for a set of 'typedef's
// that provide a stable, portable interface to platform-dependent types.  In
// particular, this component supplies portable typenames for signed and
// unsigned 64-bit integers ('bsls::Types::Int64' and 'bsls::Types::Uint64',
// respectively), as well as the preferred integral type denoting the number of
// elements in a container, and the number of bytes in a single block of memory
// supplied by an allocator ('bsls::Types::size_type').
//
///Usage
///-----
// The following illustrates how some of the types supplied by this component
// might be used.
//
// 'bsls::Types::Int64' and 'bsls::Types::Uint64' identify the preferred
// fundamental types denoting signed and unsigned 64-bit integers,
// respectively:
//..
//  bsls::Types::Uint64 stimulus = 787000000000ULL;
//..
// Clients can use these types in the same way as an 'int'.  Clients can also
// mix usage with other fundamental integral types:
//..
//  bsls::Types::Uint64 nationalDebt = 1000000000000ULL;
//  nationalDebt += stimulus;
//
//  unsigned int deficitReduction = 1000000000;
//  nationalDebt -= deficitReduction;
//
//  std::cout << "National Debt Level: " << nationalDebt << std::endl;
//..
// 'bsls::Types::size_type' identifies the preferred integral type denoting the
// number of elements in a container, and the number of bytes in a single block
// of memory supplied by an allocator.  For example, a typical use is as a
// 'typedef' in an STL container:
//..
//  class vector {
//
//      // ...
//
//    public:
//      typedef bsls::Types::size_type size_type;
//
//      // ...
//  };
//..
// Note that Standard Library facilities that work with numeric types can be
// used with 'bsls::Types' as well.  For example, the following code finds out
// some facts about 'bsls::Types::Int64' in a platform-independent way:
//..
//  std::cout << "Min Int64 value: "
//            << std::numeric_limits<bsls::Types::Int64>::min() << std::endl
//            << "Max Int64 value: "
//            << std::numeric_limits<bsls::Types::Int64>::max() << std::endl;
//..

#ifndef INCLUDED_CSTDDEF
#include <cstddef>
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {

namespace bsls {

                          // ============
                          // struct Types
                          // ============

struct Types{
    // Provide a namespace for a suite of 'typedef's that encapsulate
    // platform-dependent types.

    // TYPES
    typedef std::size_t size_type;
        // The alias 'size_type' refers to the preferred type for denoting a
        // number of elements in either 'bslma' allocators or container types.

    typedef std::size_t    UintPtr;
    typedef std::ptrdiff_t IntPtr;
        // The aliases 'UintPtr' and 'IntPtr' are guaranteed to have the same
        // size as pointers.

    typedef          long long Int64;
    typedef unsigned long long Uint64;
        // The aliases 'Int64' and 'Uint64' stand for the appropriate types
        // that define signed and unsigned 64-bit integers, respectively, for
        // the appropriate supported platforms.
};

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
