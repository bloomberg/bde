// bsls_exceptionutil.h                                               -*-C++-*-
#ifndef INCLUDED_BSLS_EXCEPTIONUTIL
#define INCLUDED_BSLS_EXCEPTIONUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide simplified exception constructs for non-exception builds.
//
//@CLASSES:
//  bsls::ExceptionUtil: namespace for exception utility functions
//
//@SEE_ALSO:
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
// This section illustrates intended use of this component.
//
///Example 1: Using 'bsls_exceptionutil' to Implement 'vector'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we wanted to define an implementation of a standard-defined
// 'vector' template.  Unfortunately, the C++ standard requires that 'vector'
// provide an 'at' method that throws an 'out_of_range' exception
// if the supplied index is not in the valid range of elements in the vector.
// In this example we show using 'BSLS_THROW' so that such an implementation
// will compile in both exception enabled and non-exception enabled builds.
// Note that apart from memory allocation, and where required by the C++
// standard, types defined in the BDE libraries do not throw exceptions, and
// are typically "exception neutral" (see {'bsldoc_glossary'}), meaning they
// behave reasonably in the presence of injected exceptions, but do not
// themselves throw any exceptions.
//
// First we open a namespace 'myStd' and define an 'out_of_range' exception
// that the 'at' method will throw (note that in practice, 'out_of_range'
// would inherit from 'logic_error')':
//..
//  namespace myStd {
//
//  class out_of_range  // ...
//  {
//     // ...
//  };
//..
// Next, we declare the 'vector' template and its template parameters (note
// that the majority of the implementation is elided, for clarity):
//..
//  template <typename VALUE, typename ALLOCATOR /* ... */>
//  class vector {
//      // DATA
//      VALUE *d_begin_p;
//      VALUE *d_end_p;
//      // ...
//
//    public:
//
//      typedef typename ALLOCATOR::size_type size_type;
//
//      //...
//..
// Then, we define the 'at' method, which is required to throw an
// 'out_of_range' exception.
//..
//      const VALUE& at(size_type index) const
//      {
//          if (d_begin_p + index < d_end_p) {
//              return d_begin_p[index];                              // RETURN
//          }
//..
// Now, we use 'BSLS_THROW' the an 'out_of_range' exception:
//..
//          BSLS_THROW(out_of_range(/* ... */));
//      }
//..
// Finally, we complete the (mostly elided) 'vector' implementation:
//..
//      // ...
//
//  };
//
//  }  // close namespace myStd
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
}  // close namespace BloombergLP

#endif // ! defined(INCLUDED_BSLS_EXCEPTIONUTIL)

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
