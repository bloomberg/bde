// bslstl_exception.cpp                                               -*-C++-*-
#include <bslstl_exception.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>

#ifndef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

// Clang, libstdc++, and MSVC have the implementation of 'uncaught_exceptions'
// in their dylib.  We can just call it.  Otherwise, we simulate it from the
// older call 'uncaught_exception'.

#if defined(BSLS_LIBRARYFEATURES_STDCPP_GNU) ||    \
    defined(BSLS_LIBRARYFEATURES_STDCPP_MSVC) ||   \
    defined(BSLS_LIBRARYFEATURES_STDCPP_LLVM)
#define UNCAUGHT_EXCEPTIONS_SIMULATED 0
#else
#define UNCAUGHT_EXCEPTIONS_SIMULATED 1
#endif

#if UNCAUGHT_EXCEPTIONS_SIMULATED
namespace bsl {

int uncaught_exceptions() throw()
{
    return std::uncaught_exception() ? 1 : 0;
}

} // close namespace bsl
#else

namespace std { int uncaught_exceptions() throw (); }

namespace bsl {

int uncaught_exceptions() throw()
{
    return std::uncaught_exceptions();
}

} // close namespace bsl
#endif  // UNCAUGHT_EXCEPTIONS_EMULATED

#endif

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
