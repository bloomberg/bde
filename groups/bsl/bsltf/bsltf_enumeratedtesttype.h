// bsltf_enumeratedtesttype.h                                         -*-C++-*-
#ifndef INCLUDED_BSLTF_ENUMERATEDTESTTYPE
#define INCLUDED_BSLTF_ENUMERATEDTESTTYPE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an enumerated test type.
//
//@CLASSES:
//   bsltf::EnumeratedTestType: enumerated test type
//
//@SEE_ALSO: bsltf_templatetestfacility
//
//@DESCRIPTION: This component provides an enumeration type,
//'EnumeratedTestType', to facilitate the testing of templates.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using The Enumeration
/// - - - - - - - - - - - - - - - -
// First, we create an 'EnumeratedTestType::Enum' value and initialize it to
// the first possible value:
//..
// EnumeratedTestType::Enum first = EnumeratedTestType::FIRST;
// assert(static_cast<int>(first) == 0);
//..
// Finally, we create an 'EnumeratedTestType::Enum' value and initilize it to
// the last possible value:
//..
// EnumeratedTestType::Enum last = EnumeratedTestType::LAST;
// assert(static_cast<int>(last) == 127);
//..

namespace BloombergLP {

namespace bsltf {

                        // =========================
                        // struct EnumeratedTestType
                        // =========================

struct EnumeratedTestType {
    // This 'struct' provides a namespace for defining an 'enum' type that
    // supports explict conversion to and from an integeral type for values
    // from 0 to 127 (according to 7.2.7 of the C++11 standard).

  public:
    // TYPES
    enum Enum {
        FIRST = 0,
        LAST = 127
    };
};

// ===========================================================================
//                  INLINE AND TEMPLATE FUNCTION IMPLEMENTATIONS
// ===========================================================================


}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
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
