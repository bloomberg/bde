// bdlb_indexspanutil.h                                               -*-C++-*-
#ifndef INCLUDED_BDLB_INDEXSPANUTIL
#define INCLUDED_BDLB_INDEXSPANUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functions that operate on 'IndexSpan' objects.
//
//@CLASSES:
//  IndexSpanUtil: namespace for functions that operate on 'IndexSpan' objects
//
//@SEE_ALSO: bdlb_indexspan
//
//@DESCRIPTION: This component provides a struct, 'IndexSpanUtil', that serves
// as a namespace for utility functions that operate on 'IndexSpan' objects.
// At the moment the only function provided is 'shrink', and it creates a new
// 'IndexSpan' that represents a (possibly) smaller span than the argument.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example1: Taking a IPv6 address out of a URI
/// - - - - - - - - - - - - - - - - - - - - - -
// Suppose we a class that stores a parsed URL using a string to store the full
// URL and 'IndexSpan' objects to describe the individual parts of the URL, and
// we want to add accessors that handle the case when the host part of the URL
// is an IPv6 address, such as "http://[ff:fe:9]/index.html".  As observed, an
// IPv6 address is indicated by the '[' and ']' characters (the URL is ill
// formed if the closing ']' is not present).  We want to implement two
// methods, one to query if the host part of the URL is IPv6 ('isIPv6Host') and
// another to get the IPv6 address (the part without the square brackets) if
// the host is actually an IPv6 address ('getIPv6Host').
//
// First let us create a 'ParsedUrl' class.  For brevity, the class has only
// those parts that are needed to implement 'isIPv6Host' and 'getIPv6Host'.
//..
//  class ParsedUrl {
//    private:
//      // DATA
//      bsl::string     d_url;
//      bdlb::IndexSpan d_host;
//
//    public:
//      // CREATORS
//      ParsedUrl(const bslstl::StringRef& url, bdlb::IndexSpan host)
//          // Create a 'ParsedUrl' from the specified 'url', and 'host'.
//      : d_url(url)
//      , d_host(host)
//      {
//      }
//
//      // ACCESSORS
//      bool isIPv6Host() const;
//          // Return 'true' if the host part represents an IPv6 address and
//          // 'false' otherwise.
//
//      bslstl::StringRef getIPv6Host() const;
//          // Return a string reference to the IPv6 address in the host part
//          // of this URL.  The behavior is undefined unless
//          // 'isIPv6Host() == true' for this object.
//  };
//..
// Next, we implement 'isIPv6Host'.
//..
//  bool ParsedUrl::isIPv6Host() const
//  {
//      return !d_host.isEmpty() && '[' == d_url[d_host.position()];
//  }
//..
//  Then, to make the accessors simple (and readable), we implement a helper
//  function that creates a 'StringRef' from a 'StringRef' and an 'IndexSpan'.
//  (Don't do this in real code, use 'IndexSpanStringUtil::bind' that is
//  levelized above this component - so we cannot use it here.)
//..
//  bslstl::StringRef bindSpan(const bslstl::StringRef& full,
//                             const bdlb::IndexSpan&   part)
//      // Return a string reference to the substring of the specified 'full'
//      // thing defined by the specified 'part'.
//  {
//      BSLS_ASSERT(part.position() <= full.length());
//      BSLS_ASSERT(part.position() + part.length() <= full.length());
//
//      return bslstl::StringRef(full.data() + part.position(), part.length());
//  }
//..
// Next, we implement 'getIPv6Host' using 'bdlb::IndexSpanUtil::shrink'.
//..
//  bslstl::StringRef ParsedUrl::getIPv6Host() const
//  {
//      BSLS_ASSERT(isIPv6Host());
//
//      return bindSpan(d_url, bdlb::IndexSpanUtil::shrink(d_host, 1, 1));
//  }
//..
// Finally, we verify the two methods with URLs.
//..
//  ParsedUrl pu1("https://host/path/", bdlb::IndexSpan(8, 4));
//  assert(false == pu1.isIPv6Host());
//
//  ParsedUrl pu2("https://[12:3:fe:9]/path/", bdlb::IndexSpan(8, 11));
//  assert(true == pu2.isIPv6Host());
//  assert("12:3:fe:9" == pu2.getIPv6Host());
//..

#include <bdlscm_version.h>

#include <bdlb_indexspan.h>

#include <bsls_assert.h>

namespace BloombergLP {
namespace bdlb {

                        // ====================
                        // struct IndexSpanUtil
                        // ====================

struct IndexSpanUtil {
    // This struct serves as a namespace for utility functions that operate on
    // 'IndexSpan'objects.
  public:
    // CLASS METHODS
    static IndexSpan shrink(const IndexSpan&     original,
                            IndexSpan::size_type shrinkBegin,
                            IndexSpan::size_type shrinkEnd);
        // Return an 'IndexSpan' object transformed from the specified
        // 'original' using the specified 'shrinkBegin' and 'shrinkEnd' so that
        // its position is 'original.position() + shrinkBegin' and whose length
        // is 'original.length() - (shrinkBegin + shrinkEnd)'.  The behavior is
        // undefined unless 'shrinkBegin + shrinkEnd <= original.length()'.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                        // --------------------
                        // struct IndexSpanUtil
                        // --------------------

// CLASS METHODS
inline
IndexSpan IndexSpanUtil::shrink(const IndexSpan&     original,
                                IndexSpan::size_type shrinkBegin,
                                IndexSpan::size_type shrinkEnd)
{
    BSLS_ASSERT(shrinkBegin + shrinkEnd <= original.length());

    return IndexSpan(original.position() + shrinkBegin,
                     original.length() - (shrinkBegin + shrinkEnd));
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
