// bsls_asserttestexception.h                                         -*-C++-*-
#ifndef INCLUDED_BSLS_ASSERTTESTEXCEPTION
#define INCLUDED_BSLS_ASSERTTESTEXCEPTION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an exception type to support testing for failed assertions.
//
//@CLASSES:
//  bsls_AssertTestException: type describing the context of a failed assertion
//
//@SEE_ALSO: bsls_assert, bsls_asserttest
//
//@AUTHOR: Alisdair Meredith (ameredit)
//
//@DESCRIPTION: This component implements an exception class,
// 'bsls_AssertTestException', that provides a mechanism to convey context
// information from a failing assertion to a test handler.  The context that is
// captured consists of the program source of the failing expression, the name
// of the file containing the assertion, and the line number within that file
// where the asserted expression may be found.
//
///Usage
///-----
// First we write a macro to act as an 'assert' facility that will throw an
// exception of type 'bsls_AssertTestException' if the asserted expression
// fails.  The thrown exception will capture the source code, filename, and
// line number of the failing expression:
//..
//  #define TEST_ASSERT(EXPRESSION)                                          \$
//      if (!(EXPRESSION)) {                                                 \$
//          throw bsls_AssertTestException(#EXPRESSION, __FILE__, __LINE__); \$
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
//  catch (const bsls_AssertTestException& exception) {
//      assert(0 == strcmp("0 != p", exception.expression()));
//      assert(0 == strcmp(__FILE__, exception.filename()));
//      assert(9 == __LINE__ - exception.lineNumber());
//  }
//..

namespace BloombergLP {

                       // ==============================
                       // class bsls_AssertTestException
                       // ==============================

class bsls_AssertTestException {
    // This class is an implementation detail of the 'bsls' testing framework
    // and should not be used directly in user code.  It implements an
    // immutable mechanism to communicate information about the context of an
    // assertion that fails to a test-case handler.

    // DATA
    const char *d_expression;  // expression that failed to assert as 'true'
    const char *d_filename;    // name of file where the assert failed
    const int   d_lineNumber;  // line number in file where the assert failed

  private:
    // NOT IMPLEMENTED
    bsls_AssertTestException& operator=(const bsls_AssertTestException&);

  public:
    // CREATORS
    bsls_AssertTestException(const char *expression,
                             const char *filename,
                             int         lineNumber);
        // Create a 'bsls_AssertTestException' object with the specified
        // 'expression', 'filename', and 'lineNumber'.  The behavior is
        // undefined unless '0 < line' and both 'expression' and 'filename'
        // point to valid null-terminated character strings that will remain
        // unmodified for the lifetime of this object (e.g., string literals).

    //! bsls_AssertTestException(const bsls_AssertTestException& original);
        // Create a 'bsls_AssertTestException' object that is a copy of the
        // specified 'original', having the same value for the 'expression',
        // 'filename', and 'lineNumber' attributes.  Note that this trivial
        // constructor's definition is compiler generated.

    //! ~bsls_AssertTestException();
        // Destroy this 'bsls_AssertTestException' object.  Note that this
        // trivial destructor's definition is compiler generated.

    // ACCESSORS
    const char *expression() const;
        // Return a string containing the program source of the assertion that
        // has failed.

    const char *filename() const;
        // Return a string containing the filename of the source file
        // containing the assertion that has failed.

    int lineNumber() const;
        // Return the number of the line within the file 'filename' containing
        // the assertion that failed.
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                          // ==============================
                          // class bsls_AssertTestException
                          // ==============================

// CREATORS
inline
bsls_AssertTestException::bsls_AssertTestException(const char *expression,
                                                   const char *filename,
                                                   int         lineNumber)
: d_expression(expression)
, d_filename(filename)
, d_lineNumber(lineNumber)
{
}

// ACCESSORS
inline
const char *bsls_AssertTestException::expression() const
{
    return d_expression;
}

inline
const char *bsls_AssertTestException::filename() const
{
    return d_filename;
}

inline
int bsls_AssertTestException::lineNumber() const
{
    return d_lineNumber;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
