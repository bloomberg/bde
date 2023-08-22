// bslstl_syncbuf.cpp                                                 -*-C++-*-
#include <bslstl_syncbuf.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslstl_syncbuf_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_bslonce.h>
#include <bsls_compilerfeatures.h>

namespace bsl {

template class basic_syncbuf<char>;
template class basic_syncbuf<wchar_t>;

SyncBuf_Mutex *SyncBuf_MutexUtil::get(void *streambuf) BSLS_KEYWORD_NOEXCEPT
{
    BSLS_ASSERT(streambuf);
#if BSLS_COMPILERFEATURES_CPLUSPLUS < 201103L  // C++03
    using namespace BloombergLP::bsls;
    static SyncBuf_Mutex *mutex = 0;
    static BslOnce        once  = BSLS_BSLONCE_INITIALIZER;

    BslOnceGuard onceGuard;
    if (onceGuard.enter(&once)) {
        static SyncBuf_Mutex theMutex;
        mutex = &theMutex;
    }
    return streambuf ? mutex : 0;
#else  // C++11+
    static SyncBuf_Mutex mutex;
    return streambuf ? &mutex : nullptr;
#endif
}

}  // close namespace bsl

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
