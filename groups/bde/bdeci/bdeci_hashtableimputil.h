// bdeci_hashtableimputil.h                -*-C++-*-
#ifndef INCLUDED_BDECI_HASHTABLEIMPUTIL
#define INCLUDED_BDECI_HASHTABLEIMPUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide implementation support for precomputed hashtable sizes.
//
//@DEPRECATED: Do not use.
//
//@CLASSES:
//   bdeci_HashtableImpUtil: namespace for precomputed hashtable sizes
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn)
//
//@DESCRIPTION:
// This component provides access to an increasing sequence of precomputed
// hashtable sizes.  These hashtable sizes range from 1 to 0x7fffffff and are
// prime except for the first value (which is 1).  The sequence of sizes grows
// - approximately - geometrically.
//
///USAGE
///-----
// The following snippet of code illustrates how to obtain hashtable sizes from
// 'bdeci_hashtableImpUtil':
//..
//      for (int i = 0; i < bdeci_HashtableImpUtil::NUM_SIZES; ++i) {
//          bsl::cout << bdeci_HashtableImpUtil::lookup(i) << bsl::endl;
//      }
//..



#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif



namespace BloombergLP {



                            // ===========================
                            // struct bdeci_HashtableSizes
                            // ===========================

struct bdeci_HashtableImpUtil {
    // This 'struct' provides a namespace for constants and pure procedures
    // related to a sequence of precomputed hashtable sizes.
  public:
    enum { BDECI_NUM_SIZES = 17 };

  private:
    static int sizes[BDECI_NUM_SIZES];

  public:
    static int lookup(int index);
        // Return the hashtable size referred to by the specified 'index'.  The
        // behavior is undefined unless 0 <= index < NUM_SIZES.  Note that
        // 1 == lookup(0), 0x7fffffff == lookup(NUM_SIZES - 1), and lookup(i)
        // for 1 <= i < NUM_SIZES is prime.  Note also that
        // lookup(j - 1) < lookup(j) for 1 <= j < NUM_SIZES.
};



// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

inline
int bdeci_HashtableImpUtil::lookup(int index)
{
    return sizes[index];
}

}  // close namespace BloombergLP



#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
