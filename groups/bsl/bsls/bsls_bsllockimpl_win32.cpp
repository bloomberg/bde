// bsls_bsllockimpl_win32.cpp                                         -*-C++-*-
#include <bsls_bsllockimpl_win32.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bsls_bsllockimpl_win32_cpp,"$Id$ $CSID$")

#include <bsls_annotation.h>
#include <bsls_platform.h>

// Include 'windows.h' here to check that our declarations of windows API
// functions and types do not conflict with 'windows.h'.  Also, verify that the
// size of the buffer allocated for the critical section is sufficient.

#ifdef BSLS_PLATFORM_OS_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace {
    // This code is below bslmf (and we cannot use BSLMF_ASSERT), so we have to
    // implement minimalistic compile time assert.

template <bool>
struct CompileTimeAssert;
    // Primary template for compile-time assertion that has no definition to
    // cause a compile-time failure if instantiated.

template<>
struct CompileTimeAssert<true>
    // Explicit specialization with an empty definition for the compile-time
    // assertion predicate being 'true'.
{
};


BSLS_ANNOTATION_UNUSED void size_check() {
    CompileTimeAssert<sizeof(CRITICAL_SECTION) ==
                 sizeof(void *) * BloombergLP::bsls::BslLockImpl_win32::
                                            k_CRITICAL_SECTION_BUFFER_SIZE>();
}

}  // close unnamed namespace

namespace BloombergLP {

}  // close enterprise namespace

#endif  //BSLS_PLATFORM_OS_WINDOWS

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
