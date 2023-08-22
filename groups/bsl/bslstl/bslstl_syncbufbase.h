// bslstl_syncbufbase.h                                               -*-C++-*-
#ifndef INCLUDED_BSLSTL_SYNCBUFBASE
#define INCLUDED_BSLSTL_SYNCBUFBASE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an allocator-independent base-class for 'basic_syncbuf'.
//
//@CLASSES:
//  bsl::SyncBufBase: allocator-independent base of 'basic_syncbuf'.
//
//@SEE_ALSO: bslstl_syncbuf
//
//@DESCRIPTION: This component is for internal use only.  Please don't use it
// directly.
//
// This component provides a protocol class, 'SyncBufBase', that is intended
// for use as a base class for synchronized stream buffers. The 'SyncBufBase'
// protocol provides methods to emit the synchronized buffer, and mark the
// buffer to be emitted on a call to 'sync' on an associated stream. This
// operations are exposed by 'SyncBufBaseUtil' for use by standard functions,
// like 'bsl::emit_on_flush', 'bsl::noemit_on_flush', and 'bsl::flush_emit',
// while not exposing them into the public interface of derived types (like
// 'basic_syncbuf') whose interface in defined by an external standard.
//
// Factoring these operations into a non-templatized base class allows
// operations (like 'bsl::emit_on_flush' etc) to applied to derived objects in
// contexts where it is impossible to deduce the 'ALLOCATOR' type of the
// inheriting synchornized buffer.

#include <bslscm_version.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>

namespace BloombergLP {
namespace bslstl {

struct SyncBufBaseUtil;  // forward

                            // =================
                            // class SyncBufBase
                            // =================

class SyncBufBase {
    // Allocator-independent base of 'basic_syncbuf'.  Also this is used by the
    // manipulators: 'emit_on_flush', 'noemit_on_flush', 'flush_emit' (the
    // 'ALLOCATOR' template argument cannot be deduced from
    // 'basic_ostream<CHAR_TYPE, CHAR_TRAITS>').

    // PRIVATE MANIPULATORS
    virtual bool emitInternal() = 0;
        // Atomically transfer any characters buffered by this object to the
        // wrapped stream buffer, so that it appears in the output stream as a
        // contiguous sequence of characters.  The wrapped stream buffer is
        // flushed if and only if a call was made to 'sync' since the most
        // recent call to 'emit' or construction.  Return 'true' if
        // 'get_wrapped() != nullptr', and all of the characters in the
        // associated output were successfully transferred, and the flush (if
        // any) succeeded; return 'false' otherwise.

    virtual void setEmitOnSync(bool value) BSLS_KEYWORD_NOEXCEPT = 0;
        // Set the "emit-on-sync" property of the synchronized buffer to the
        // specified 'value'.  If "emit-on-sync" is 'true', the synchronized
        // buffer will emit (see 'emitInternal') its buffered contents on a
        // call to 'sync' (or 'pubsync'); otherwise the synchronized buffer
        // will not be emitted on a 'sync'.

    // FRIENDS
    friend struct SyncBufBaseUtil;

  public:
    // CREATORS
    virtual ~SyncBufBase();
        // Destroy this object.
};

                            // =====================
                            // class SyncBufBaseUtil
                            // =====================

struct SyncBufBaseUtil {
    // Internal utils.

    // CLASS METHODS
    static bool emit(SyncBufBase *syncBuf);
        // Atomically transfer any characters buffered by the specified
        // 'syncBuf' object to the wrapped stream buffer, so that it appears in
        // the output stream as a contiguous sequence of characters.  The
        // wrapped stream buffer is flushed if and only if a call was made to
        // 'sync' since the most recent call to 'emit' or construction.  Return
        // 'true' if 'get_wrapped() != nullptr', and all of the characters in
        // the associated output were successfully transferred, and the flush
        // (if any) succeeded; return 'false' otherwise.  The behavior is
        // undefined unless 'syncBuf' is not null.

    static void setEmitOnSync(SyncBufBase *syncBuf, bool value);
        // Set the "emit-on-sync" property of the synchronized buffer to the
        // specified 'value'.  If "emit-on-sync" is 'true', the synchronized
        // buffer will emit (see 'emitInternal') its buffered contents on a
        // call to 'sync' (or 'pubsync'); otherwise the synchronized buffer
        // will not be emitted on a 'sync'.  The behavior is undefined unless
        // 'syncBuf' is not null.
};


                            // ---------------------
                            // class SyncBufBaseUtil
                            // ---------------------

// CLASS METHODS
inline
bool SyncBufBaseUtil::emit(SyncBufBase *syncBuf)
{
    BSLS_ASSERT(syncBuf);
    return syncBuf->emitInternal();
}

inline
void SyncBufBaseUtil::setEmitOnSync(SyncBufBase *syncBuf, bool value)
{
    BSLS_ASSERT(syncBuf);
    syncBuf->setEmitOnSync(value);
}

}  // close package namespace
}  // close enterprise namespace

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
