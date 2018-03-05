// btlb_blobstreambuf.h                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BTLB_BLOBSTREAMBUF
#define INCLUDED_BTLB_BLOBSTREAMBUF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide blob implementing the 'streambuf' interface.
//
//@CLASSES:
// btlb::InBlobStreamBuf: 'btlb::Blob' input 'streambuf'
// btlb::OutBlobStreamBuf: 'btlb::Blob' output 'streambuf'
//
//@AUTHOR: Guillaume Morin (gmorin1)
//
//@SEE_ALSO: btlb_blob
//
//@DESCRIPTION: This component implements the input and output
// 'bsl::basic_streambuf' protocol using a user-supplied 'btlb::Blob'.  Method
// names necessarily correspond to the protocol-specified method names.  Refer
// to the C++ Standard, Section 27.5.2, for a full specification of the
// interface.
//
// A 'btlb::Blob' is an indexed sequence of 'btlb::BlobBuffer' of potentially
// different sizes.  The number of buffers in the sequence can increase or
// decrease, but the order of the buffers cannot change.  Therefore, the blob
// behaves logically as a single indexed buffer.  'btlb::InBlobStreamBuf' and
// 'btlb::OutBlobStreamBuf' can therefore respectively read from and write to
// this buffer as if there were a single continuous index.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLBB_BLOBSTREAMBUF
#include <bdlbb_blobstreambuf.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_BTLB_BLOB
#include <btlb_blob.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOS
#include <bsl_ios.h>  // for 'bsl::streamsize'
#endif

#ifndef INCLUDED_BSL_STREAMBUF
#include <bsl_streambuf.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>  // bsl::size_t
#endif

#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {
namespace btlb {

typedef bdlbb::InBlobStreamBuf  InBlobStreamBuf;
typedef bdlbb::OutBlobStreamBuf OutBlobStreamBuf ;

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
