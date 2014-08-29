// bslx_testinstreamexception.h                                       -*-C++-*-
#ifndef INCLUDED_BSLX_TESTINSTREAMEXCEPTION
#define INCLUDED_BSLX_TESTINSTREAMEXCEPTION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an exception class for unexternalization operations.
//
//@CLASSES:
//  bslx::TestInStreamException: exception containing unexternalization info
//
//@SEE_ALSO: bslx_testinstream
//
//@DESCRIPTION: This component defines a simple exception object for testing
// exceptions during unexternalization operations.  The exception object
// 'bslx::TestInStreamException' contains information about an
// unexternalization request, which can be queried by the "catcher" of the
// exception.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1:  Throw and Catch
///- - - - - - - - - - - - - -
// In the following example, the 'bslx::TestInStreamException' object is
// thrown, caught, and examined.   First, set up a 'try' block and throw the
// exception with the indication that the exception occurred during
// unexternalization of an 'unsigned int':
//..
//  try {
//      throw TestInStreamException(bslx::TypeCode::e_UINT32);
//..
// Then, catch the exception and verify the indicated cause of the exception:
//..
//  } catch (const bslx::TestInStreamException& e) {
//      assert(bslx::TypeCode::e_UINT32 == e.dataType());
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLX_TYPECODE
#include <bslx_typecode.h>
#endif

namespace BloombergLP {
namespace bslx {

                        // ===========================
                        // class TestInStreamException
                        // ===========================

class TestInStreamException {
    // This class defines an exception object for unexternalization operations.
    // Instances of this class contain information about an unexternalization
    // request.

    // DATA
    TypeCode::Enum d_dataType;  // type of the input data requested

  public:
    // CREATORS
    explicit TestInStreamException(TypeCode::Enum type);
        // Create an exception object initialized with the specified 'type'.

    //! ~TestInStreamException() = default;
        // Destroy this object.  Note that this method's definition is compiler
        // generated.

    // ACCESSORS
    TypeCode::Enum dataType() const;
        // Return the type code that was supplied at construction of this
        // exception object.
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                        // ---------------------------
                        // class TestInStreamException
                        // ---------------------------

// CREATORS
inline
TestInStreamException::TestInStreamException(TypeCode::Enum type)
: d_dataType(type)
{
}

// ACCESSORS
inline
TypeCode::Enum TestInStreamException::dataType() const
{
    return d_dataType;
}

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
