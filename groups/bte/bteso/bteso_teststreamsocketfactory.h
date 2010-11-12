// bteso_teststreamsocketfactory.h -*-C++-*-
#ifndef INCLUDED_BTESO_TESTSTREAMSOCKETFACTORY
#define INCLUDED_BTESO_TESTSTREAMSOCKETFACTORY

//@PURPOSE:
//
//@AUTHOR: Andrei Basov (abasov)
//
//@CLASSES:
//  bteso_TestStreamSocketFactory:
//
//@SEE_ALSO:
//
//@DESCRIPTION:
//
///Thread-safety
///-------------
//
///Performance
///-----------
//
///USAGE EXAMPLE
///=============
//..
//..

#ifndef INCLUDED_BTESCM_VERSION
#include <btescm_version.h>
#endif

#ifndef INCLUDED_BTESO_STREAMSOCKETFACTORY
#include <bteso_streamsocketfactory.h>
#endif

                     // ===================================
                     // class bteso_TestStreamSocketFactory
                     // ===================================

namespace BloombergLP {

template <class ADDRESS>
class bteso_TestStreamSocketFactory : public bteso_StreamSocketFactory<ADDRESS> {
  // [TBD - Class description]
  public:
    // CREATORS
    bteso_TestStreamSocketFactory(const char *specifications);
    ~bteso_TestStreamSocketFactory();
    // MANIUPLATORS
    bteso_StreamSocket<ADDRESS> *allocate();
    void deallocate(bteso_StreamSocket<ADDRESS> *streamSocket);
    // ACCESSORS
};

//-----------------------------------------------------------------------------
//                      INLINE FUNCTIONS' DEFINITIONS
//-----------------------------------------------------------------------------

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
