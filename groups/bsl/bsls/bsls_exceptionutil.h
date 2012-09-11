// bsls_exceptionutil.h                                               -*-C++-*-
#ifndef INCLUDED_BSLS_EXCEPTIONUTIL
#define INCLUDED_BSLS_EXCEPTIONUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Simplify exception constructs for non-exception builds
//
//@CLASSES:
//  bsls::ExceptionUtil: namespace for exception utility functions
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: Code that uses 'try', 'throw' and 'catch' constructs will
// often fail to compile when exceptions are disabled using a compiler switch,
// even if the 'throw' statement is unlikely to be executed at run-time or if
// the 'catch' clause can safely ignore an exception that will never occur.
// This component provides macros to replace 'try', 'throw' and 'catch'.
// These macros expand to normal exception constructs when exceptions are
// enabled and reasonable alternatives (usually no-ops) when exceptions are
// disabled.
//
///Usage
///-----
// Define a few exception classes:
//..
//  class my_ExClass1
//  {
//  };
//
//  class my_ExClass2
//  {
//  };
//..
// Define a function that never throws an exception:
//..
//  int noThrowFunc() BSLS_NOTHROW_SPEC
//  {
//      return -1;
//  }
//..
// Define a function that might throw 'my_ExClass1' or 'my_ExClass2':
//..
//  int doThrowSome(int i) BSLS_EXCEPTION_SPEC((my_ExClass1, my_ExClass2))
//  {
//      switch (i) {
//        case 0: break;
//        case 1: BSLS_THROW(my_ExClass1());
//        case 2: BSLS_THROW(my_ExClass2());
//      }
//      return i;
//  }
//..
// In the main program, loop three times if exceptions are enabled, but only
// once if exceptions are disabled.  The only conditional compilation is for
// the loop counter:
//..
//  int main()
//  {
//  #ifdef BDE_BUILD_TARGET_EXC
//      const int ITERATIONS = 3;
//  #else
//      const int ITERATIONS = 1;
//  #endif
//
//      for (int i = 0; i < ITERATIONS; ++i) {
//..
// The loop contains a pair of nested 'try' blocks constructed using the
// macros so that it will compile and run whether or not exceptions are
// enabled.  Note that the curly brace placement is identical to normal 'try'
// and 'catch' constructs.  The outer 'try' block catches 'my_ExClass2':
//..
//  int caught = -1;
//  BSLS_TRY {
//..
// The inner 'try' block catches 'my_ExClass1' and also has a "catch-all"
// handler:
//..
//
//  BSLS_TRY {
//      noThrowFunc();
//      doThrowSome(i);
//
//      caught = 0; // Got here if no throw
//  }
//  BSLS_CATCH(my_ExClass1) {
//      caught = 1;
//  }
//  BSLS_CATCH(...) {
//..
// Within the catch-all handler, use the 'BSLS_RETHROW' macro to re-throw the
// exception to the outer 'try' block:
//..
//                  BSLS_RETHROW;
//              } // end inner try-catch
//          }
//          BSLS_CATCH(my_ExClass2) {
//              caught = 2;
//          }
//          BSLS_CATCH(...) {
//              assert("Should not get here" && 0);
//          } // end outer try-catch
//
//          if (0 != caught) {
//              std::printf("Caught exception my_ExClass%d\n", caught);
//          }
//          else {
//              std::printf("Caught no exceptions\n", caught);
//          }
//          assert(i == caught);
//
//      } // end for (i)
//
//      return 0;
//  }
//..

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

                        // ======
                        // macros
                        // ======

#ifdef BDE_BUILD_TARGET_EXC

        // ------------------
        // Exceptions enabled
        // ------------------

#   define BSLS_TRY try
        // Start a try block.
        // Exceptions enabled: 'try'
        // Exceptions disabled: start of a normal (non-try) block

#   define BSLS_CATCH(X) catch (X)
        // Catch exception 'X'.  'X' must be a type with optional variable name
        // Exceptions enabled: 'catch (X)'
        // Exceptions disabled: ignore following block

#   define BSLS_THROW(X) throw X
        // Throw exception object 'X'.
        // Exceptions enabled: 'throw (X)'
        // Exceptions disabled: abort with a message

#   define BSLS_RETHROW throw
        // Within a 'BSLS_CATCH' clause, re-throw the last exception
        // Exceptions enabled: 'throw'
        // Exceptions disabled: abort with a message

#   define BSLS_EXCEPTION_SPEC(SPEC) throw SPEC
        // Declare the exception specification for a function.
        // Usage:
        //..
        //  void f() BSLS_EXCEPTION_SPEC((std::logic_error));
        //..
        // 'SPEC' must be a comma-separated list of one or more exception
        // types enclosed in parenthesis.  (Double parenthesis are mandatory)
        // Use 'BSLS_NOTHROW_SPEC' (below) to declare that a function does not
        // throw.
        // Exceptions enabled: 'throw SPEC'
        // Exceptions disabled: empty

#   define BSLS_NOTHROW_SPEC throw ()
        // Declare that a function does not throw any exceptions:
        // Usage:
        //..
        //  void f() BSLS_NOTHROW_SPEC;
        //..
        // Exceptions enabled: 'throw ()'
        // Exceptions disabled: empty

#else // If exceptions are disabled

        // -------------------
        // Exceptions disabled
        // -------------------

#   define BSLS_TRY if (1)

#   define BSLS_CATCH(X) else if (0)

#   define BSLS_THROW(X)                                                    \
    BloombergLP::bsls::Assert::invokeHandler("Tried to throw " #X           \
                                            " with exceptions disabled",    \
                                            __FILE__, __LINE__)

#   define BSLS_RETHROW                                                     \
    BloombergLP::bsls::Assert::invokeHandler("Tried to re-throw exception " \
                                            "with exceptions disabled",     \
                                            __FILE__, __LINE__)

#   define BSLS_EXCEPTION_SPEC(SPEC)

#   define BSLS_NOTHROW_SPEC

#endif // BDE_BUILD_TARGET_EXC

namespace BloombergLP
{
}

#endif // ! defined(INCLUDED_BSLS_EXCEPTIONUTIL)

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
