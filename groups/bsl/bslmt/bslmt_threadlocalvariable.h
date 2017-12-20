// bslmt_threadlocalvariable.h                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_THREADLOCALVARIABLE
#define INCLUDED_BSLMT_THREADLOCALVARIABLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a macro to declare a thread-local variable.
//
//@CLASSES:
//   BSLMT_THREAD_LOCAL_VARIABLE: macro to declare a thread-local variable
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component should *not* be used outside of the 'bce'
// package at this time.
//
// This component defines a macro for declaring a 'static' thread-local
// variable.  Where a normal static variable is located at the same memory
// location for all threads within a process, a thread-local static variable
// has a different memory location for each thread in the process:
//..
//  BSLMT_THREAD_LOCAL_VARIABLE(BASIC_TYPE, VARIABLE_NAME, INITIAL_VALUE)
//      Declare, at function or namespace scope, a thread-local 'static'
//      variable having the specified 'VARIABLE_NAME' of the specified
//      'BASIC_TYPE' in the current context and initialize it with the
//      specified 'INITIAL_VALUE'.  The 'BASIC_TYPE' must be a valid
//      typename, and that typename must represent either a fundamental or a
//      pointer type.  The specified 'VARIABLE_NAME' must be a valid variable
//      name in the scope in which the macro is employed.  The specified
//      'INITIAL_VALUE' must evaluate to a *compile-time* *constant* *value*
//      of type 'BASIC_TYPE'.  If 'VARIABLE_NAME' is not a valid variable
//      name, or the type of 'INITIAL_VALUE' is not convertible to type
//      'BASIC_TYPE', the instantiation will result in a *compile* *time*
//      error.  The behavior is undefined unless this macro is instantiated
//      within a function or at file (or namespace) scope (i.e., *not* at
//      class scope), and 'INITIAL_VALUE' is a *compile-time* constant.  Note
//      that the instantiation of this macro is similar to the declaration:
//      'static BASIC_TYPE VARIABLE_NAME = INITIAL_VALUE;'
//      except that the declared variable, 'VARIABLE_NAME', refers to a
//      different memory location for each thread in the process.
//..
// Note that, 'BSLMT_THREAD_LOCAL_VARIABLE' should *not* be instantiated at
// class scope.
//
///Platform Notes
///--------------
// Since the C++ standard does not define a thread-local storage specifier,
// support varies between platforms and compilers.  Moreover, thread-local
// variables are *not* supported on (1) Solaris with gcc or (2) Windows due to
// compiler or runtime limitations.  See the following table for details.
//..
// +----------+----------------------------------------------------------------
// | Platform |  Note
// |----------+----------------------------------------------------------------
// |  AIX     |  o A bug in the AIX compiler prevents compiling translation
// |          |    units with more than 10 thread-local variables with debug
// |          |    symbols.  This has been fixed in xlC8 (internal ticket
// |          |    DRQS 13819416), but has not yet been fixed for xlC10.
// |          |
// |          |  o xlC8 does not support class scoped thread-local variables.
// |----------+----------------------------------------------------------------
// | Solaris  |  o Thread local variables are not currently supported due to a
// |  gcc     |    run-time failure (on thread-exit) for tasks using
// |          |    thread-local variables.  Note that both the Solaris native
// |          |    compiler, and gcc on Linux do support thread-local
// |          |    variables.
// +----------+----------------------------------------------------------------
// | Windows/ |  o Thread local variables cause run-time errors when used
// | MSVC     |    inside a dynamic-library (.dll) that is loaded at runtime
// |          |    using 'LoadLibrary' or 'LoadLibraryEx' on versions of
// |          |    Windows prior to Vista (e.g., XP).  The problem is noted
// |          |    here:
// |          |    http://support.microsoft.com/kb/118816
// +----------+----------------------------------------------------------------
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Service Request Processor with Thread Local Context
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we create a 'RequestProcessor' that places context
// information for the current request in a thread-local variable.
//
// First, we define a trivial structure for a request context.
//..
//  // requestprocessor.h
//
//  struct RequestContext {
//
//      // DATA
//      int d_userId;       // BB user id
//      int d_workstation;  // BB LUW
//  };
//..
// Next, we create a trivial 'RequestProcessor' that provides a 'static' class
// method that returns the 'RequestContext' for the current thread, or 0 if the
// current thread is not processing a request.
//..
//  class RequestProcessor {
//      // This class implements an "example" request processor.
//
//      // NOT IMPLEMENTED
//      RequestProcessor(const RequestProcessor&);
//      RequestProcessor& operator=(const RequestProcessor&);
//
//      // PRIVATE CLASS METHODS
//      static const RequestContext *&contextReference();
//          // Return a reference to a *modifiable* thread-local pointer to the
//          // non-modifiable request context for this thread.  Note that this
//          // method explicitly allows the pointer (but not the
//          // 'RequestContext' object) to be modified by the caller to allow
//          // other methods to assign the thread-local context pointer to a
//          // new address.
//
//    public:
//
//      // CLASS METHODS
//      static const RequestContext *requestContext();
//          // Return the address of the non-modifiable, request context for
//          // this thread, or 0 if none has been set.
//
//      // CREATORS
//      RequestProcessor() {}
//          // Create a 'RequestProcessor'.
//
//      ~RequestProcessor() {}
//          // Destroy this request processor.
//
//      // MANIPULATORS
//      void processRequest(int userId, int workstation, const char *request);
//          // Process (in the caller's thread) the specified 'request' for
//          // the specified 'userId' and 'workstation'.
//  };
//
//  // requestprocessor.cpp
//
//  // PRIVATE CLASS METHODS
//..
// Now, we define the 'contextReference' method, which defines a thread-local
// 'RequestContext' pointer, 'context', initialized to 0, and returns a
// reference providing modifiable access to that pointer.
//..
//  const RequestContext *&RequestProcessor::contextReference()
//  {
//      BSLMT_THREAD_LOCAL_VARIABLE(const RequestContext *, context, 0);
//      return context;
//  }
//
//  // CLASS METHODS
//  const RequestContext *RequestProcessor::requestContext()
//  {
//      return contextReference();
//  }
//
//  // MANIPULATORS
//..
// Then, we define the 'processRequest' method, which first sets the
// thread-local pointer containing the request context, and then processes the
// 'request'.
//..
//  void RequestProcessor::processRequest(int         userId,
//                                        int         workstation,
//                                        const char *request)
//  {
//      RequestContext currentContext = {userId, workstation};
//
//      contextReference() = &currentContext;
//
//      // Process the request.
//
//      contextReference() = 0;
//  }
//..
// Finally, we define a separate function 'myFunction' that uses the
// 'RequestProcessor' class to access the 'RequestContext' for the current
// thread.
//..
//  void myFunction()
//  {
//      const RequestContext *context = RequestProcessor::requestContext();
//
//      // Perform some task that makes use of this threads 'requestContext'.
//      // ...
//  }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

                            // =================
                            // Macro Definitions
                            // =================

#if defined(BSLS_PLATFORM_CMP_SUN)                                            \
 || (defined(BSLS_PLATFORM_CMP_GNU) && !(defined(BSLS_PLATFORM_CPU_SPARC)))   \
 || (defined(BSLS_PLATFORM_CMP_CLANG) && !(defined(BSLS_PLATFORM_CPU_SPARC)))
#define BSLMT_THREAD_LOCAL_VARIABLE(BASIC_TYPE, VARIABLE_NAME, INITIAL_VALUE) \
static BSLMT_THREAD_LOCAL_KEYWORD BASIC_TYPE VARIABLE_NAME = INITIAL_VALUE;
    // This macro should *not* be used by clients outside of the 'bce'
    // package-group at this time.
    //
    // On IBM, the introduction of thread-local storage causes static
    // initializers for shared objects to stop running.  This problem is
    // described in DRQS 16438026.  Therefore, even though thread-local storage
    // is supported by xlC10, it is explicitly not used here.
    //
    // On Windows/MSVC, compiler-based thread-local storage may cause a crash
    // at runtime when used from a DLL on Windows versions below Vista.
    // Therefore, it is explicitly not used here.
    //
    // Define, at function or namespace scope, a thread-local 'static' variable
    // having the specified 'VARIABLE_NAME' of the specified 'BASIC_TYPE',
    // initialized with the specified 'INITIAL_VALUE'.  If 'VARIABLE_NAME' is
    // not a valid variable name, or 'INITIAL_VALUE' is not convertible to the
    // type 'BASIC_TYPE', the instantiation of this macro will result in a
    // compile time error.  The behavior is undefined unless 'INITIAL_VALUE' is
    // a *compile* *time* constant value.
#endif

#if defined(BSLS_PLATFORM_CMP_SUN)                                            \
 || (defined(BSLS_PLATFORM_CMP_GNU) && !(defined(BSLS_PLATFORM_CPU_SPARC)))   \
 || (defined(BSLS_PLATFORM_CMP_CLANG) && !(defined(BSLS_PLATFORM_CPU_SPARC)))
#define BSLMT_THREAD_LOCAL_KEYWORD __thread
#endif

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
