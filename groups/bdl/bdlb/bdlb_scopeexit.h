// bdlb_scopeexit.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLB_SCOPEEXIT
#define INCLUDED_BDLB_SCOPEEXIT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a general-purpose guard object for scope-exit logic.
//
//@CLASSES:
//  bdlb::ScopeExit: executes a function upon destruction
//  bdlb::ScopeExitAny: an alias to 'ScopeExit<bsl::function<void()>>'
//  bdlb::ScopeExitUtil: factory method for creating guards
//
//@SEE_ALSO:
// P0052R6 - Generic Scope Guard and RAII Wrapper for the Standard Library
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0052r6.pdf
//
//@DESCRIPTION:
// This component provides a class template mechanism 'bdlb::ScopeExit', that
// will invoke a client supplied function upon it's destruction.
// 'bdlb::ScopeExit' is intended to facilitate creating scoped-guards (similar
// to those found in {'bslma'}).  This component also defines a utility struct
// 'bdlb::ScopeExitUtil', containing factory methods for creating a
// 'bdlb::ScopeExit' object ('makeScopeExit').  Finally, this component defines
// a type alias 'bdlb::ScopeExitAny' that may be used in C++03 code to store a
// guard where the type of the exit function is not known (for example it is
// the result of a bind expression).  See also {C++03 restrictions}.
//
///C++03 Restrictions
///------------------
// The first restriction when using this component with C++03 is related to the
// fact that 'bdlb::ScopeExit' is a move-only type, and although this class
// tries its best to emulate move semantics, it isn't enough to be able to
// cleanly return an instance of 'bdlb::ScopeExit' from a factory function.  It
// is technically possible, but would require a lot of boilerplate code on the
// user side.
//
// The second restriction is the absence of the 'auto' keyword, which makes it
// impossible to create an instance of 'bdlb::ScopeExit' using a factory
// function without knowing the exact type of the exit function, which is what
// the factory function is for.
//
// Given these two restrictions, the following design decisions have been made:
// 1. Do not provide factory functions when using C++03.
// 2. Provide a type 'bdlb::ScopeExitAny' that is an alias to
//    'bdlb::ScopeExit<bsl::function<void()> >'.  An instance of
//    'bdlb::ScopeExitAny' can be created without the need to know the type
//    of the exit function it is constructed from.  The downside is, of course,
//    type erasure and potential unnecessary memory allocation that comes with
//    'bsl::function'.
//
///Memory Allocation and Relationship with BDE Allocators
///------------------------------------------------------
// The exit function is created as a member of the guard object, so no memory
// is allocated for storing it.  However, if the exit function copy or move
// constructors require memory allocation, that memory is supplied by the
// currently installed default allocator.  Currently this component does not
// support custom allocators, and since the main use-case of a guard object is
// to be created on the stack, no such support is needed.
//
///Usage
///-----
// In this section, we show the intended usage of this component.
//
///Using a Scope Exit Guard in C++11
///- - - - - - - - - - - - - - - - -
// Suppose we are creating a simple database that stores names and their
// associated addresses and we store the names and addresses in two separate
// "tables".  While adding data, these tables may fail the insertion, in which
// case we need to "roll back" the already inserted data, such as if we
// inserted the address first, we need to remove it if insertion of the
// associated name fails.
//
// First we emulate our our database access with the following simple functions
// for the sake of a working example:
//..
//  int insertAddress(const char *address)
//  {
//      // Implementation is omitted for brevity
//      (void)address;
//      return 42;
//  }
//
//  int insertName(const char *name, int addressId)
//  {
//      // Implementation is omitted for brevity
//      (void)name;
//      (void)addressId;
//      throw 42; // Simulate failure
//  }
//
//  int removedAddress = 0;
//  void removeAddress(int id)
//  {
//      // Implementation is omitted for brevity
//      removedAddress = id;
//  }
//..
// As it is shown in the example above, our 'insertName' function fails.  Next,
// we draw up our insert function.
//..
//  int insertCustomer(const char *name, const char *address)
//  {
//..
// Then, in the insert function we use a 'try' block to swallow the exception
// thrown by our fake database function.  In this try block we successfully
// insert the address.
//..
//      int addressId = -1;
//      try {
//          addressId = insertAddress(address);
//..
// Next, before we try to insert the name, we need to set up the guard to
// remove the already inserted address when the name insert fails.
//..
//          auto guard = bdlb::ScopeExitUtil::makeScopeExit(
//                                         [=](){ removeAddress(addressId); });
//..
// Then we attempt to insert the name (which will always fail in our toy
// example).
//..
//          int nameId = insertName(name, addressId);
//..
// Next, if the insertion succeeds we need to release the guard and return the
// ID.
//..
//          guard.release();
//
//          return nameId;                                            // RETURN
//      }
//..
// Then we ignore the exception for the sake of this toy example.
//..
//      catch (int) {
//          // We ignore the exception for this example
//      }
//..
// Finally we verify that the 'removeAddress' function was called with the
// right identifier.
//..
//      assert(removedAddress == addressId);
//
//      return -1;
//  }
//..
//
///Using a scope exit guard in C++03
///- - - - - - - - - - - - - - - - -
// Suppose we are in the same situation as in the C++11 example, but we have to
// create a C++03 solution.  C++03 does not support lambdas, so first we have
// to hand-craft a functor that calls 'removeAddress' with a given ID.
//..
//  struct RemoveAddress {
//      int d_id;
//
//      explicit RemoveAddress(int id)
//      : d_id(id)
//      {
//      }
//
//      void operator()() const
//      {
//          removeAddress(d_id);
//      }
//  };
//..
// Then we basically have the same code as for C++11, except for the guard
// specification.
//..
//  int insertCustomer03(const char *name, const char *address)
//  {
//      int addressId = -1;
//      try {
//          addressId = insertAddress(address);
//..
// The guards type is explicitly specified and the functor is used instead of
// a lambda.  Alternatively (for example if binding is involved) we could use
// 'ScopeExitAny' instead of a specific type.  Notice the extra parenthesis to
// avoid "the most vexing parse"
// (https://en.wikipedia.org/wiki/Most_vexing_parse) issue.  Since we are in
// C++03, we cannot use curly braces to avoid the issue.
//..
//          bdlb::ScopeExit<RemoveAddress> guard((RemoveAddress(addressId)));
//
//          int nameId = insertName(name, addressId);
//
//          guard.release();
//
//          return nameId;                                            // RETURN
//      }
//      catch (int) {
//          // We ignore the exception for this example
//      }
//..
// Finally we verify that the 'removeAddress' function was called with the
// right identifier.
//..
//      assert(removedAddress == addressId);
//
//      return -1;
//  }
//..

#include <bdlscm_version.h>

#include <bslmf_conditional.h>
#include <bslmf_decay.h>
#include <bslmf_enableif.h>
#include <bslmf_isconvertible.h>
#include <bslmf_isnothrowmoveconstructible.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>
#include <bslmf_util.h>

#include <bsls_buildtarget.h>
#include <bsls_compilerfeatures.h>
#include <bsls_cpp11.h>

#include <bsl_functional.h>

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
#include <bsl_type_traits.h>
#endif
#include <bsl_utility.h>

#ifdef BDE_BUILD_TARGET_EXC
#define BDLB_SCOPEEXIT_NOEXCEPT_SPEC BSLS_CPP11_NOEXCEPT_SPECIFICATION
#else
#define BDLB_SCOPEEXIT_NOEXCEPT_SPEC(...)
#endif

namespace BloombergLP {
namespace bdlb {

template <class>
class ScopeExit;

typedef ScopeExit<bsl::function<void()> > ScopeExitAny;
    // 'ScopeExitAny' is an alias to 'ScopeExit<bsl::function<void()> >',
    // effectively making it a polymorphic scope guard type.

                               // ===============
                               // class ScopeExit
                               // ===============

template <class EXIT_FUNC>
class ScopeExit
    // 'ScopeExit' is a general-purpose scope guard that calls its exit
    // function when a scope is exited.
    //
    // Template argument 'EXIT_FUNC' shall be a function object type, lvalue
    // reference to function, or lvalue reference to function object type.  If
    // 'EXIT_FUNC' is an object type, it shall satisfy the requirements of
    // Destructible and Callable as specified in the C++ standard.
{
  private:
    // PRIVATE DATA
    EXIT_FUNC d_exitFunction;
    bool      d_executeOnDestruction;

  private:
    // NOT IMPLEMENTED
    ScopeExit(const ScopeExit&)            BSLS_CPP11_DELETED;
    ScopeExit& operator=(const ScopeExit&) BSLS_CPP11_DELETED;
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    ScopeExit& operator=(ScopeExit&&)      BSLS_CPP11_DELETED;
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

  public:
    // CREATORS
    template <class EXIT_FUNC_PARAM>
    explicit ScopeExit(
              BSLS_COMPILERFEATURES_FORWARD_REF(EXIT_FUNC_PARAM) function,
              typename bsl::enable_if<
                  !bsl::is_same<
                      ScopeExit<EXIT_FUNC>,
                      typename bsl::decay<EXIT_FUNC_PARAM>::type>::value
                          && bsl::is_convertible<EXIT_FUNC_PARAM,
                                                 EXIT_FUNC>::value>::type* = 0)
#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
        // The full exception specification needs to be duplicated because the
        // Microsoft Visual Studio 2017 compiler does not recognize
        // preprocessor directives in macro arguments.
      BDLB_SCOPEEXIT_NOEXCEPT_SPEC(
               bsl::is_nothrow_constructible<EXIT_FUNC, EXIT_FUNC_PARAM>::value
                     );                                             // IMPLICIT
#else // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
      BDLB_SCOPEEXIT_NOEXCEPT_SPEC(false);                          // IMPLICIT
#endif // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
        // Create a 'ScopeExit' object, which, upon its destruction will invoke
        // the specified 'function' (or functor).  If 'function' must be
        // copied, and that operation throws an exception, invoke 'function'
        // and rethrow the exception.  If 'EXIT_FUNC_PARAM' cannot be move
        // converted to 'EXIT_FUNC' via no-throw means, 'function' will always
        // be copied into the member.  That is necessary because if the move
        // fails (with an exception) we would not be able to invoke 'function'
        // as its state would be unknown.  The behavior is undefined if
        // 'function', when called, throws an exception.

    ScopeExit(bslmf::MovableRef<ScopeExit> original)
#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
        // The full exception specification needs to be duplicated because the
        // Microsoft Visual Studio 2017 compiler does not recognize
        // preprocessor directives in macro arguments.
        BDLB_SCOPEEXIT_NOEXCEPT_SPEC(
                         bsl::is_nothrow_move_constructible<EXIT_FUNC>::value
                      || bsl::is_nothrow_copy_constructible<EXIT_FUNC>::value
                     );                                             // IMPLICIT
#else // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
        BDLB_SCOPEEXIT_NOEXCEPT_SPEC(
                         bsl::is_nothrow_move_constructible<EXIT_FUNC>::value
                     );                                             // IMPLICIT
#endif // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
        // If 'bsl::is_nothrow_move_constructible<EXIT_FUNC>::value' is true or
        // 'EXIT_FUNC' is a move-only type, move construct, otherwise, copy
        // construct the exit function from the specified 'original'.  If
        // construction succeeds, call 'release()' on 'original'.  The behavior
        // is undefined if the exit function's move constructor throws an
        // exception.

    ~ScopeExit();
        // Destroy this guard.  Execute the exit function unless the guard have
        // been released with a call to 'release()'.

  public:
    // MANIPULATORS
    void release() BSLS_CPP11_NOEXCEPT;
        // Release this guard.  The result of releasing the guard is that the
        // exit function is not invoked on destruction.
};

                             // ===================
                             // class ScopeExitUtil
                             // ===================

struct ScopeExitUtil {
    // Provides factory functions for scope guards.

    // CLASS METHODS
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)
    template <class EXIT_FUNC>
    static ScopeExit<typename bsl::decay<EXIT_FUNC>::type>
    makeScopeExit(EXIT_FUNC&& function) BDLB_SCOPEEXIT_NOEXCEPT_SPEC(
                         bsl::is_nothrow_constructible<
                             ScopeExit<typename bsl::decay<EXIT_FUNC>::type>,
                             EXIT_FUNC
                         >::value
                         && bsl::is_nothrow_move_constructible<
                              ScopeExit<typename bsl::decay<EXIT_FUNC>::type>
                         >::value);
        // Return a 'ScopeExit' guard that has the specified 'function' as its
        // exit function.

#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES &&
       // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
};


// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                             // ---------------
                             // class ScopeExit
                             // ---------------

// CREATORS
template <class EXIT_FUNC>
template <class EXIT_FUNC_PARAM>
inline
ScopeExit<EXIT_FUNC>::ScopeExit(
             BSLS_COMPILERFEATURES_FORWARD_REF(EXIT_FUNC_PARAM) function,
             typename bsl::enable_if<
                 !bsl::is_same<ScopeExit<EXIT_FUNC>,
                               typename bsl::decay<EXIT_FUNC_PARAM>::type
                 >::value
                 && bsl::is_convertible<EXIT_FUNC_PARAM, EXIT_FUNC
                                       >::value>::type*)
#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
        // The full exception specification needs to be duplicated because the
        // Microsoft Visual Studio 2017 compiler does not recognize
        // preprocessor directives in macro arguments.
      BDLB_SCOPEEXIT_NOEXCEPT_SPEC(
              bsl::is_nothrow_constructible<EXIT_FUNC, EXIT_FUNC_PARAM>::value)
#else // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
      BDLB_SCOPEEXIT_NOEXCEPT_SPEC(false)
#endif // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
#ifdef BDE_BUILD_TARGET_EXC
try
#endif // BDE_BUILD_TARGET_EXC
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)
: d_exitFunction(bsl::forward<
          typename bsl::conditional<
                         bsl::is_nothrow_constructible<EXIT_FUNC,
                                                       EXIT_FUNC_PARAM>::value,
                         EXIT_FUNC_PARAM,
                         const EXIT_FUNC_PARAM&
                   >::type>(function))
#else
: d_exitFunction(function)
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES &&
       // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
, d_executeOnDestruction(true)
{
}
#ifdef BDE_BUILD_TARGET_EXC
catch (...)
{
    function();
#ifdef BSLS_PLATFORM_CMP_MSVC
    // This warning has to be completely disabled because a) Microsoft issues
    // it at template instantiation time and b) we cannot use 'if constexpr' to
    // not have this 'throw' statement in case we promised not to throw,
    // because we do not have C++17 as our minimal supported language version.
#pragma warning(disable:4297) // function assumed not to throw
#endif
    throw;
}
#endif // BDE_BUILD_TARGET_EXC

template <class EXIT_FUNC>
inline
ScopeExit<EXIT_FUNC>::ScopeExit(bslmf::MovableRef<ScopeExit> original)
#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
        // The full exception specification needs to be duplicated because the
        // Microsoft Visual Studio 2017 compiler does not recognize
        // preprocessor directives in macro arguments.
        BDLB_SCOPEEXIT_NOEXCEPT_SPEC(
                         bsl::is_nothrow_move_constructible<EXIT_FUNC>::value
                      || bsl::is_nothrow_copy_constructible<EXIT_FUNC>::value
                     )
#else // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
        BDLB_SCOPEEXIT_NOEXCEPT_SPEC(
                         bsl::is_nothrow_move_constructible<EXIT_FUNC>::value
                     )
#endif // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
: d_exitFunction(
    bslmf::MovableRefUtil::move_if_noexcept(
           bslmf::MovableRefUtil::access(original).d_exitFunction))
, d_executeOnDestruction(true)
{
    bslmf::MovableRefUtil::access(original).release();
}

template <class EXIT_FUNC>
inline
ScopeExit<EXIT_FUNC>::~ScopeExit()
{
    if (d_executeOnDestruction) {
        d_exitFunction();
    }
}

// MANIPULATORS
template <class EXIT_FUNC>
inline
void ScopeExit<EXIT_FUNC>::release() BSLS_CPP11_NOEXCEPT
{
    d_executeOnDestruction = false;
}

                         // -------------------
                         // class ScopeExitUtil
                         // -------------------

// CLASS METHODS
#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES) && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER)
template <class EXIT_FUNC>
inline
ScopeExit<typename bsl::decay<EXIT_FUNC>::type>
ScopeExitUtil::makeScopeExit(EXIT_FUNC&& function)
               BDLB_SCOPEEXIT_NOEXCEPT_SPEC(
                    bsl::is_nothrow_constructible<
                         ScopeExit<typename bsl::decay<EXIT_FUNC>::type>,
                         EXIT_FUNC
                    >::value
                 && bsl::is_nothrow_move_constructible<
                         ScopeExit<typename bsl::decay<EXIT_FUNC>::type>
                    >::value)
{
    return ScopeExit<typename bsl::decay<EXIT_FUNC>::type>(
                                            bsl::forward<EXIT_FUNC>(function));
}
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES &&
       // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER

}  // close package namespace
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
