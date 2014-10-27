// bdlb_randomdevice.h                                                -*-C++-*-
#ifndef INCLUDED_BDLB_RANDOMDEVICE
#define INCLUDED_BDLB_RANDOMDEVICE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a common interface to a system's random number generator.
//
//@CLASSES:
//  bdlb::RandomDevice: namespace for system specific random-number generators.
//
//@SEE_ALSO:
//
//@AUTHOR: Mickey Sweatt (msweatt1)
//
//@DESCRIPTION: This component provides a namespace, 'bdlb::RandomDevice', for
// a suite of functions used to generate random seeds from platform-dependent
// random number generators.  Two variants are provided: one which may block,
// but which potentially samples from a stronger distribution, and another
// which does not block, but which potentially should not be used for
// cryptography.  The strength of these random numbers and the performance of
// these calls is strongly dependent on the underlying system.  On UNIX-like
// platforms 'genRandomBytes()' reads from '/dev/random' and
// 'genRandonBytesNonBlocking()' reads from '/dev/urandom'.  On Windows both
// methods use 'CrypGenRandom'.
//
// Note that it is not appropriate to use these functions to generate many
// random numbers, because they are likely to exhaust available entropy and
// then be slow.  Instead, these functions should be used to seed pseudo-random
// random number generators.
//
///Usage
///-----
// This section illustrates intended use of this component.
//..
//  template <class CHOICE_TYPE>
//  class RandomChoice {
//    // This class manages selecting pseudo-random elements out of an array
//    // sampling with replacement.
//
//    // DATA
//      CHOICE_TYPE *d_choices;  // the possibilities (used not owned)
//      int          d_size;     // the number of elements to choose among
//
//    public:
//      // CREATORS
//      RandomChoice(CHOICE_TYPE choices[], int numChoices);
//          // Create an object with the specified 'choices' array with
//          // 'numChoices' elements.
//
//      ~RandomChoice();
//          // Delete this object
//
//      // ACCESSOR
//      const CHOICE_TYPE& choice() const;
//          // Return a random member of the 'choices', sampling with
//          // replacement.
//  };
//
//  // CREATORS
//  template <class CHOICE_TYPE>
//  RandomChoice<CHOICE_TYPE>::RandomChoice(CHOICE_TYPE choices[],
//                                          int         numChoices)
//  : d_choices(choices), d_size(numChoices)
//  {
//  }
//
//  template <class CHOICE_TYPE>
//  RandomChoice<CHOICE_TYPE>::~RandomChoice()
//  {
//  }
//
//  // ACCESSORS
//  template <class CHOICE_TYPE>
//  const CHOICE_TYPE& RandomChoice<CHOICE_TYPE>::choice() const
//  {
//      size_t index;
//      bdlb::RandomDevice::getRandomBytesNonBlocking(
//                                   reinterpret_cast<unsigned char *>(&index),
//                                   sizeof index);
//      return d_choices[index % d_size];
//  }
//..
// Initialize an array of colors to choose between.
//..
//      string colors[] = {"Red" , "Orange", "Yellow", "Green",
//                         "Blue", "Indigo", "Violet"};
//      unsigned numColors = sizeof colors/sizeof colors[0];
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

                        // ===================
                        // struct RandomDevice
                        // ===================

struct RandomDevice {
    // This 'struct' provides a namespace for a suite of functions used for
    // acquiring random numbers from the system.

    // TYPES
    typedef bsls::Types::size_type size_t;       // for brevity of name

    // CLASS METHODS
    static int getRandomBytes(unsigned char *buffer, size_t numBytes);
        // Read the the specified 'numBytes' from the system random number
        // generator into the specified 'buffer'.  Returns 0 on success,
        // non-zero otherwise.  Note that this method may block if called
        // repeatedly or if 'numBytes' is high.

    static int getRandomBytesNonBlocking(unsigned char *buffer,
                                         size_t         numBytes);
        // Read the the specified 'numBytes' from the system non-blocking
        // random number generator into the specified 'buffer'.  Returns 0 on
        // success, non-zero otherwise.  Note that on most platforms sampling
        // from this pool does not produce cryptographically secure numbers.
};

}  // close package namespace
}  // close enterprise namespace

#endif
// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
