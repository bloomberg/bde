// bslmf_if.cpp                                                       -*-C++-*-
#include <bslmf_if.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_platform.h>

#if defined(BSLS_PLATFORM_CMP_GNU) || defined(BSLS_PLATFORM_CMP_CLANG)
// For g++ 4.3.2 builds on Sun, a completely empty archive causes
//      ld: elf error: file ./libbslmf.dbg_exc_mt.a: elf_getarsym
// errors.  The presence of any "real" symbol in the archive suffices to avoid
// the error, even if it's an unreferenced static.
// Our old RCS ID scheme used to avoid this error unintentionally, by using
// static char arrays for the RCS IDs.

char bslmf_if_filename[] = __FILE__;
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
