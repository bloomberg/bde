// bsls_fuzztestpreconditionexception.h                               -*-C++-*-
#ifndef INCLUDED_BSLS_FUZZTESTPRECONDITIONEXCEPTION
#define INCLUDED_BSLS_FUZZTESTPRECONDITIONEXCEPTION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an exception type for handling failed preconditions.
//
//@CLASSES:
//  bsls::FuzzTestPreconditionException: type describing a failed precondition
//
//@SEE_ALSO: bsls_fuzztest
//
//@DESCRIPTION: This component implements an exception class,
// 'bsls::FuzzTestPreconditionException', that provides a mechanism to convey
// context information from a failing precondition to a test handler.  The
// context that is captured consists of the program source of the failing
// expression, the name of the file containing the assertion, the line number
// within that file where the asserted expression may be found, and the level
// of the assertion that has failed.
//
///Usage
///-----
// First we write a macro to act as a precondition testing 'assert' facility
// that will throw an exception of type 'bsls::FuzzTestPreconditionException'
// if the asserted expression fails.  The thrown exception will capture the
// source-code of the expression, the filename and line number of the failing
// expression.
//..
//  #define TEST_PRECONDITION(EXPRESSION)                                    \$
//      if (!(EXPRESSION)) {                                                 \$
//          bsls::AssertViolation violation(#EXPRESSION,                     \$
//                                          __FILE__,                        \$
//                                          __LINE__,                        \$
//                                          "LEVEL");                        \$
//          throw bsls::FuzzTestPreconditionException(violation);            \$
//      }
//..
// Next we use the macro inside a try-block, so that we can catch the exception
// thrown if the tested expression fails.
//..
//      try {
//          void *p = NULL;
//          TEST_PRECONDITION(0 != p);
//      }
//..
// If the assertion fails, catch the exception and confirm that it correctly
// recorded the context of where the assertion failed.
//..
//      catch (const bsls::FuzzTestPreconditionException& exception) {
//          assert(0  == strcmp("0 != p",
//                              exception.assertViolation().comment()));
//          assert(0  == strcmp(__FILE__,
//                              exception.assertViolation().fileName()));
//          assert(11 == __LINE__ - exception.assertViolation().lineNumber());
//          assert(0  == strcmp("LEVEL",
//                              exception.assertViolation().assertLevel()));
//      }
//..

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

namespace BloombergLP {

namespace bsls {

                         // ===================================
                         // class FuzzTestPreconditionException
                         // ===================================

class FuzzTestPreconditionException {
    // This class is an implementation detail of the 'bsls' fuzz testing
    // framework and should not be used directly in user code.  It implements
    // an immutable mechanism to communicate to a test-case handler information
    // about the context of a precondition that fails.

  private:
    // DATA
    const AssertViolation d_assertViolation;  // from a failed precondition
                                              // check

  private:
    // NOT IMPLEMENTED
    FuzzTestPreconditionException& operator=(
                    const FuzzTestPreconditionException&) BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    explicit BSLS_KEYWORD_CONSTEXPR
    FuzzTestPreconditionException(const AssertViolation& assertViolation);
        // Create a 'FuzzTestPreconditionException' object with the specified
        // 'assertViolation'.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    // To avoid warnings about future incompatibility due to the deleted copy
    // assignment operator we declare the copy constructor as implicitly
    // generated.  For consistency the destructor was also placed here and
    // declared to be explicitly generated.

    FuzzTestPreconditionException(
                      const FuzzTestPreconditionException& original) = default;
        // Create a 'FuzzTestPreconditionException' object that is a copy of
        // the specified 'original', having the same value for the
        // 'assertViolation' attribute.  Note that this trivial constructor's
        // definition is compiler generated.

    ~FuzzTestPreconditionException() = default;
        // Destroy this object.  Note that this trivial destructor's definition
        // is compiler generated.
#endif

    // ACCESSORS
    const AssertViolation& assertViolation() const;
        // Return an 'AssertViolation' containing the details of the
        // precondition that has failed.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                    // ===================================
                    // class FuzzTestPreconditionException
                    // ===================================

// CREATORS
BSLS_KEYWORD_CONSTEXPR
inline
FuzzTestPreconditionException::FuzzTestPreconditionException(
                                        const AssertViolation& assertViolation)
: d_assertViolation(assertViolation)
{
}

// ACCESSORS
inline
const AssertViolation& FuzzTestPreconditionException::assertViolation() const
{
    return d_assertViolation;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
