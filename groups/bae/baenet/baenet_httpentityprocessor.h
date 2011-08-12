// baenet_httpentityprocessor.h   -*-C++-*-
#ifndef INCLUDED_BAENET_HTTPENTITYPROCESSOR
#define INCLUDED_BAENET_HTTPENTITYPROCESSOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")



//@PURPOSE: Provide a protocol for HTTP entity processors.
//
//@CLASSES:
//   baenet_HttpEntityProcessor: protocol for HTTP entity processors
//
//@SEE_ALSO: baenet_httpheader baenet_httpstartline
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION:  This component provides a protocol class for processing HTTP
// entities.  For each entity, the following methods are invoked:
//..
//  onStartEntity(startLine, header);
//  onEntityData(data);   // called 0 or more times
//  onEndEntity();
//..
// An entity may or may not contain a body.  If the entity does not contain a
// body, then the 'onEntityData' method will not be invoked.  For large
// entities, the 'onEntityData' method can be invoked multiple times (for each
// blob of data).
//
// An entity processor also acts as a factory for HTTP header objects.  The
// 'createHeader' method returns an implementation of the  'baenet_HttpHeader'
// protocol.  Fields can be added to this header, then the header is used in
// the call to 'onStartEntity'.  This allows the entity processor to use custom
// HTTP header types.
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// First, we will create a header and add fields to it:
//..
//  bcema_SharedPtr<baenet_HttpHeader> hdr = ep.createHeader();
//
//  hdr.addField("SOAPAction", "http://www.bloomberg.com/services/sqrt");
//  hdr.addField("Content-Length", "211");
//..
// HTTP entities can be processed as follows:
//..
//  baenet_HttpStartLine startLine;
//  bcema_Blob           data;
//
//  startLine.createRequestLine();
//  startLine.requestLine().method()     = baenet_HttpRequestMethod::POST;
//  startLine.requestLine().requestUri() = "/services/sqrt";
//
//  ep.onStartEntity(startLine, hdr);
//
//  dataAvailable = readData(&data);
//
//  while (dataAvailable) {
//     ep.onEntityData(data);
//     dataAvailable = readData(&data);
//  }
//
//  ep.onEndEntity();
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAENET_HTTPHEADER
#include <baenet_httpheader.h>
#endif

#ifndef INCLUDED_BCEMA_SHAREDPTR
#include <bcema_sharedptr.h>
#endif

namespace BloombergLP {

class baenet_HttpStartLine;
class bcema_Blob;

                      // ================================
                      // class baenet_HttpEntityProcessor
                      // ================================

class baenet_HttpEntityProcessor {
    // Protocol for HTTP entity processors.

  public:
    // CREATORS
    virtual ~baenet_HttpEntityProcessor();
        // For syntactic purposes only.

    // MANIPULATORS
    virtual void onStartEntity(
                         const baenet_HttpStartLine&               startLine,
                         const bcema_SharedPtr<baenet_HttpHeader>& header) = 0;
        // Start processing a new entity with the specified 'startLine' and the
        // specified 'header'.

    virtual void onEntityData(const bcema_Blob& data) = 0;
        // Process the specified 'data' as part of the current entity.

    virtual void onEndEntity() = 0;
        // Finish processing the current entity.

    // ACCESSORS
    virtual bcema_SharedPtr<baenet_HttpHeader> createHeader() const = 0;
        // Return a new HTTP header that will be used when invoking
        // 'onStartEntity'.
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
