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
// `bsls::AssertTestException`, that provides a mechanism to convey context
// information from a failing assertion to a test handler.  The context that is
// captured consists of the program source of the failing expression, the name
// of the file containing the assertion, the line number within that file where
// the asserted expression may be found, and the level of the assertion that
// has failed.
//
// Note that this class is intended as an implementation detail of the `bsls`
// testing framework (see {`bsls_asserttest`}), though it may be used in other
// contexts.
//
///Usage
///-----
// First we write a macro to act as an `assert` facility that will throw an
// exception of type `bsls::AssertTestException` if the asserted expression
// fails.  The thrown exception will capture the source code, filename, and
// line number of the failing expression:
// ```
// #define TEST_ASSERT(EXPRESSION)                                          \$
//     if (!(EXPRESSION)) {                                                 \$
//         throw bsls::AssertTestException(#EXPRESSION, __FILE__, __LINE__, \$
//                                         "LEVEL");                        \$
// }
// ```
// Next we use the macro inside a `try`-block, so that we can catch the
// exception thrown if the tested expression fails:
// ```
// try {
//     void *p = NULL;
//     TEST_ASSERT(0 != p);
// }
// ```
// If the assertion fails, catch the exception and confirm that it correctly
// recorded the context of where the assertion failed:
// ```
// catch (const bsls::AssertTestException& exception) {
//     assert(0 == strcmp("0 != p", exception.expression()));
//     assert(0 == strcmp(__FILE__, exception.filename()));
//     assert(9 == __LINE__ - exception.lineNumber());
//     assert(0 == strcmp("LEVEL", exception.level());
// }
// ```

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>

#include <cstring>
#include <cstdlib>

namespace BloombergLP {

namespace bsls {

                      // ================================
                      // class AssertTestException_String
                      // ================================

/// This non-assignable class owns a null-terminated string, provides no
/// accessors to manipulate that string, and allocates a new copy of that
/// string when copied.
class AssertTestException_String {
    const char *d_data;  // the string

  private:
    // NOT IMPLEMENTED
    AssertTestException_String& operator=(
                       const AssertTestException_String&) BSLS_KEYWORD_DELETED;

  public:
    // Create an `AssertTestException_String` that owns a copy of the string
    // denoted by the specified `data` if it is not `NULL`.
    AssertTestException_String(const char *data);

    // Create an `AssertionTestException_String` that owns a string whose value
    // is the same as that owned by `original` if any.
    AssertTestException_String(const AssertTestException_String& original);

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    // Take ownership of the string owned by the specified `original`, if any,
    // and set `original` to own no string.
    AssertTestException_String(AssertTestException_String&& original);
#endif

    // Free the string owned by this object, if any.
    ~AssertTestException_String();

    // Return a pointer to the owned string.
    const char *data() const;
};

                         // =========================
                         // class AssertTestException
                         // =========================

/// This class is an implementation detail of the `bsls` testing framework
/// and should not be used directly in user code.  It implements an
/// immutable mechanism to communicate information about the context of an
/// assertion that fails to a test-case handler.
class AssertTestException {

    // DATA
    AssertTestException_String d_expression;
        // expression that failed to assert as 'true'
    AssertTestException_String d_filename;
        // name of file where the assert failed
    const int                  d_lineNumber;
        // line number in file where the assert failed
    AssertTestException_String d_level;
        // level of failed assertion or review

  private:
    // NOT IMPLEMENTED
    AssertTestException& operator=(
                              const AssertTestException&) BSLS_KEYWORD_DELETED;

  public:
    // CREATORS

    /// Create a `AssertTestException` object with the specified
    /// `expression`, `filename`, `lineNumber`, and `level`.  The behavior
    /// is undefined unless `0 < line` and all of `expression`, `filename`,
    /// and `level` are `NULL` or point to valid null-terminated character
    /// strings.
    AssertTestException(const char *expression,
                        const char *filename,
                        int         lineNumber,
                        const char *level = "UNKNOWN");

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    // To avoid warnings about future incompatibility due to the deleted copy
    // assignment operator we declare the copy constructor as implicitly
    // generated.  For consistency the destructor was also placed here and
    // declared to be explicitly generated.

    /// Create a `AssertTestException` object that is a copy of the
    /// specified `original`, having the same value for the `expression`,
    /// `filename`, and `lineNumber` attributes.  Note that this
    /// constructor's definition is compiler generated.
    AssertTestException(const AssertTestException& original) = default;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    /// Create a `AssertTestException` object whose value is that of
    /// specified `original`, having the same value for the `expression`,
    /// `filename`, and `lineNumber` attributes.  Note that this
    /// constructor's definition is compiler generated.
    AssertTestException(AssertTestException&& original) = default;
#endif

    
    /// Destroy this object.  Note that this destructor's definition is
    /// compiler generated.
    ~AssertTestException() = default;
#endif

    // ACCESSORS

    /// Return a string containing the program source of the assertion that
    /// has failed.
    const char *expression() const;

    /// Return a string containing the filename of the source file
    /// containing the assertion that has failed.
    const char *filename() const;

    /// Return a string containing a representation of the level of
    /// assertion or review macro that failed.
    const char *level() const;

    /// Return the number of the line within the file `filename` containing
    /// the assertion that failed.
    int lineNumber() const;
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                         // =========================
                         // class AssertTestException
                         // =========================

// CREATORS
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
    return d_expression.data();
}

inline
const char *AssertTestException::filename() const
{
    return d_filename.data();
}

inline
const char *AssertTestException::level() const
{
    return d_level.data();
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

/// This alias is defined for backward compatibility.
typedef bsls::AssertTestException bsls_AssertTestException;
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
