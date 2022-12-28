// bsls_asserttestexception.h                                         -*-C++-*-
#ifndef INCLUDED_BSLS_ASSERTTESTEXCEPTION
#define INCLUDED_BSLS_ASSERTTESTEXCEPTION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an exception type to support testing for failed assertions.
//
//@CLASSES:
//  bsls::AssertTestException: type describing the context of failed assertion
//
//@SEE_ALSO: bsls_assert, bsls_asserttest
//
//@DESCRIPTION: This component implements an exception class,
// 'bsls::AssertTestException', that provides a mechanism to convey context
// information from a failing assertion to a test handler.  The context that is
// captured consists of the program source of the failing expression, the name
// of the file containing the assertion, the line number within that file where
// the asserted expression may be found, and the level of the assertion that
// has failed.
//
// Note that this class is intended as an implementation detail of the 'bsls'
// testing framework (see {'bsls_asserttest'}), though it may be used in other
// contexts.
//
///Usage
///-----
// First we write a macro to act as an 'assert' facility that will throw an
// exception of type 'bsls::AssertTestException' if the asserted expression
// fails.  The thrown exception will capture the source code, filename, and
// line number of the failing expression:
//..
//  #define TEST_ASSERT(EXPRESSION)                                          \$
//      if (!(EXPRESSION)) {                                                 \$
//          throw bsls::AssertTestException(#EXPRESSION, __FILE__, __LINE__, \$
//                                          "LEVEL");                        \$
//  }
//..
// Next we use the macro inside a 'try'-block, so that we can catch the
// exception thrown if the tested expression fails:
//..
//  try {
//      void *p = NULL;
//      TEST_ASSERT(0 != p);
//  }
//..
// If the assertion fails, catch the exception and confirm that it correctly
// recorded the context of where the assertion failed:
//..
//  catch (const bsls::AssertTestException& exception) {
//      assert(0 == strcmp("0 != p", exception.expression()));
//      assert(0 == strcmp(__FILE__, exception.filename()));
//      assert(9 == __LINE__ - exception.lineNumber());
//      assert(0 == strcmp("LEVEL", exception.level());
//  }
//..

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

namespace BloombergLP {

namespace bsls {

                         // =========================
                         // class AssertTestException
                         // =========================

class AssertTestException {
    // This class is an implementation detail of the 'bsls' testing framework
    // and should not be used directly in user code.  It implements an
    // immutable mechanism to communicate information about the context of an
    // assertion that fails to a test-case handler.

    // DATA
    const char *d_expression;  // expression that failed to assert as 'true'
    const char *d_filename;    // name of file where the assert failed
    const int   d_lineNumber;  // line number in file where the assert failed
    const char *d_level;       // level of failed assertion or review

  private:
    // NOT IMPLEMENTED
    AssertTestException& operator=(
                              const AssertTestException&) BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    BSLS_KEYWORD_CONSTEXPR
    AssertTestException(const char *expression,
                        const char *filename,
                        int         lineNumber,
                        const char *level = "UNKNOWN");
        // Create a 'AssertTestException' object with the specified
        // 'expression', 'filename', 'lineNumber', and 'level'.  The behavior
        // is undefined unless '0 < line' and all of 'expression', 'filename',
        // and 'level' point to valid null-terminated character strings that
        // will remain unmodified for the lifetime of this object (e.g., string
        // literals).

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    // To avoid warnings about future incompatibility due to the deleted copy
    // assignment operator we declare the copy constructor as implicitly
    // generated.  For consistency the destructor was also placed here and
    // declared to be explicitly generated.

    AssertTestException(const AssertTestException& original) = default;
        // Create a 'AssertTestException' object that is a copy of the
        // specified 'original', having the same value for the 'expression',
        // 'filename', and 'lineNumber' attributes.  Note that this trivial
        // constructor's definition is compiler generated.

    ~AssertTestException() = default;
        // Destroy this object.  Note that this trivial destructor's definition
        // is compiler generated.
#endif

    // ACCESSORS
    const char *expression() const;
        // Return a string containing the program source of the assertion that
        // has failed.

    const char *filename() const;
        // Return a string containing the filename of the source file
        // containing the assertion that has failed.

    const char *level() const;
        // Return a string containing a representation of the level of
        // assertion or review macro that failed.

    int lineNumber() const;
        // Return the number of the line within the file 'filename' containing
        // the assertion that failed.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                         // =========================
                         // class AssertTestException
                         // =========================

// CREATORS
BSLS_KEYWORD_CONSTEXPR
inline
AssertTestException::AssertTestException(const char *expression,
                                         const char *filename,
                                         int         lineNumber,
                                         const char *level)
: d_expression(expression)
, d_filename(filename)
, d_lineNumber(lineNumber)
, d_level(level)
{
}

// ACCESSORS
inline
const char *AssertTestException::expression() const
{
    return d_expression;
}

inline
const char *AssertTestException::filename() const
{
    return d_filename;
}

inline
const char *AssertTestException::level() const
{
    return d_level;
}

inline
int AssertTestException::lineNumber() const
{
    return d_lineNumber;
}

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

typedef bsls::AssertTestException bsls_AssertTestException;
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

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
