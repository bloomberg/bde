// baenet_httpheader.h   -*-C++-*-
#ifndef INCLUDED_BAENET_HTTPHEADER
#define INCLUDED_BAENET_HTTPHEADER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")



//@PURPOSE: Provide a protocol for HTTP headers.
//
//@CLASSES: baenet_HttpHeader: protocol for HTTP headers
//
//@SEE_ALSO: baenet_httpbasicheaderfields
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: This component provides a protocol class for HTTP headers.  A
// HTTP header consists of a set of fields, which are key-value pairs.  Every
// HTTP header implementation must contain a 'HttpBasicHeaderFields' object,
// which contains the fields from the HTTP general header and the HTTP entity
// header.
//
// A HTTP header implementation may contain other fields.  These fields are not
// used by the 'baenet' package, but can be used by applications.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.  In
// this example, we will add a "Content-Length" field with the value of "315":
//..
//  bdeut_StringRef fieldName("Content-Length", 14);
//  bdeut_StringRef fieldValue("315", 3);
//
//  int rc = header.addField(fieldName, fieldValue);
//  assert(0 == rc);
//..
// Now we can assert the value of the "Content-Length" field:
//..
//  assert(315 == header.basicHeaderFields().contentLength().value());
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

namespace BloombergLP {

class bdeut_StringRef;
class baenet_HttpBasicHeaderFields;

                          // =======================
                          // class baenet_HttpHeader
                          // =======================

class baenet_HttpHeader {
    // Protocol for HTTP headers.

  public:
    // CREATORS
    virtual ~baenet_HttpHeader();
        // For syntactic purposes only.

    // MANIPULATORS
    virtual int addField(const bdeut_StringRef& fieldName,
                         const bdeut_StringRef& fieldValue) = 0;
        // Add the specified 'fieldValue' to the field with the specified
        // 'fieldName'.  Return 0 on success, and a non-zero value otherwise.

    // ACCESSORS
    virtual const baenet_HttpBasicHeaderFields& basicFields() const = 0;
        // Return a reference to the non-modifiable basic header fields.
};

}  // close namespace BloombergLP;

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
