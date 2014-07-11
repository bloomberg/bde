// bdlb_sysrandom.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLB_SYSRANDOM
#define INCLUDED_BDLB_SYSRANDOM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a universal interface to a system random number generators.
//
//@CLASSES:
// bdlb::SysRandom: namespace for a suite of system random-number generators.
//
//@SEE_ALSO:
//
//@AUTHOR: Mickey Sweatt (msweatt1)
//
//@DESCRIPTION: This component provides a namespace, 'bdlb::SysRandom', for a
// suite of functions used to generate random numbers from platform dependent
// random number generators.  Two variants are provided one which blocks, but
// which potentially sample from a stronger distribution.  The strength of
// these random numbers and the performance of these calls is strongly
// dependent on the underlying system.
//
///Usage
///-----
// The following snippets of code illustrate how to create and use a
// 'bdlb_SysRandom' object.
//..
//  template <class CHOICE_TYPE>
//  class RandomChoice
//  {
//      CHOICE_TYPE *d_choices;  // the possibilities
//      unsigned     d_size;
//
//    public:
//      // CREATORS
//      RandomChoice(CHOICE_TYPE choices[], unsigned numChoices);
//          // Create an object to return a random one of the first specified
//          // 'numChoices' elements of the specified 'choices' array.
//
//      ~RandomChoice();
//          // Delete this object

//      // ACCESSOR
//      const CHOICE_TYPE& choice() const;
//          // Return a random member of the 'choices'.
//  };

//  // CREATORS
//  template <class CHOICE_TYPE>
//  RandomChoice<CHOICE_TYPE>::RandomChoice(CHOICE_TYPE choices[],
//                                          unsigned     numChoices)
// :  d_size(numChoices)
//  {
//      d_choices = new CHOICE_TYPE[numChoices];
//      for (unsigned i = 0; i < numChoices; ++i)
//      {
//          d_choices[i] = choices[i];
//      }
//  }

//  template <class CHOICE_TYPE>
//  RandomChoice<CHOICE_TYPE>::~RandomChoice()
//  {
//      delete  [] d_choices;
//  }

//  // ACCESSORS
//  template <class CHOICE_TYPE>
//  const CHOICE_TYPE& RandomChoice<CHOICE_TYPE>::choice() const
//  {
//      int index;
//      bdlb::SysRandom::urandomN(&index, sizeof(index));
//      return d_choices[index % d_size];
//  }
//..
//  Initialize an array of colors to choose between.
//..
//      string colors[] = {"Red" , "Orange", "Yellow", "Green",
//                         "Blue", "Indigo", "Violet"};
//      unsigned numColors = sizeof(colors)/sizeof(colors[0]);
//..
// Request a random color.
//..
//   RandomChoice<string> chooseColor(colors, numColors);
//..
// Finally we stream the value of this color to 'stdout':
//..
//  if (verbose)
//      cout << chooseColor.choice() << endl;
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {

namespace bdlb {

                        // ================
                        // struct SysRandom
                        // ================

struct SysRandom {
    // This 'struct' provides a namespace for a suite of functions used for
    // acquiring random numbers from the system.

    // CLASS METHODS
    static int randomN(void *buffer, unsigned numBytes = 1);
        // Read the the optionally specified 'numBytes' from the from the
        // system random number generator into the specified 'buffer'.  Returns
        // 0 on success, non-zero otherwise.  Note that this method may block
        // if called repeatedly or if 'numBytes' is high.

    static int urandomN(void *buffer, unsigned numBytes = 1);
        // Read the the optionally specified 'numBytes' from the from the
        // system non-blocking random number generator into the specified
        // 'buffer'.  Returns 0 on success, non-zero otherwise.  Note that this
        // method may return a random-number unsuitable for cryptography.
};

}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
