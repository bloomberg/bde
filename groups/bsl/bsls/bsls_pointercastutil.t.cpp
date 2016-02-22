// bsls_pointercastutil.t.cpp                                         -*-C++-*-
#include <bsls_pointercastutil.h>

#include <bsls_bsltestutil.h>

#include <stddef.h>
#include <cstdio>
#include <cstdlib>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// This test driver tests utility functions provided by the
// 'bsls::PointerCastUtil' namespace.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 1] cast(const From& from);
// ----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bsls::PointerCastUtil Util;
extern "C" typedef int (*printf_t)(const char *, ...);

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -*

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
//------
// This section illustrates intended use of this component.
//
///Example 1: Using a function pointer as a closure parameter
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose there is an event-handling service which requires registration of a
// combination of object and closure value, and which invokes a method on the
// object, passing back the closure.
//
// First we define the service and its handler:
//..
    struct Handler { virtual void handle(void *closure) = 0; };
    class Service {
        Handler *d_handler_p;
        void    *d_closure_p;
      public:
        void registerHandler(Handler *handler, void *closure = 0) {
            d_handler_p = handler;
            d_closure_p = closure;
        }
        void eventOccurred() { d_handler_p->handle(d_closure_p); }
    };
//..
// Then, we want to define a handler which will receive a function pointer as
// the closure object and invoke it.  In order to do that, we must cast it to a
// function pointer, but some compilers may not allow it.  We can use
// 'bsls::PointerCastUtil::cast' to accomplish this:
// ..
    struct MyHandler : Handler {
        void handle(void *closure) {
             bsls::PointerCastUtil::cast<void(*)()>(closure)();
        }
    };
//..
// Next, we will set up a sample service and our handler function:
//..
    Service aService;
    static int counter = 0;
    void event() { ++counter; }
//..

// BDE_VERIFY pragma: pop

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = (argc > 1) ? atoi(argv[1]) : 1;

    bool verbose = argc > 2;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 2: {
      // --------------------------------------------------------------------
      // USAGE EXAMPLE
      //   Extracted from component header file.
      //
      // Concerns:
      //: 1 The usage example provided in the component header file compiles,
      //:   links, and runs as shown.
      //
      // Plan:
      //: 1 Incorporate usage example from header into test driver, remove
      //:   leading comment characters, and replace 'assert' with 'ASSERT'.
      //:   (C-1)
      //
      // Testing:
      //   USAGE EXAMPLE
      // --------------------------------------------------------------------

      if (verbose) printf("\nUSAGE EXAMPLE"
                          "\n=============\n");

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -*

// Finally, we will register our handler and then trigger events to verify that
// our handler is recording them correctly.  To register the function pointer
// as a closure object, we must cast it to a data pointer.  Again, we can use
// 'bsls::PointerCastUtil::cast' to accomplish this:
//..
    MyHandler ah;
    aService.registerHandler(&ah, bsls::PointerCastUtil::cast<void *>(event));
    aService.eventOccurred();
    aService.eventOccurred();
    ASSERT(counter == 2);
//..

// BDE_VERIFY pragma: pop
      } break;
      case 1: {
      // --------------------------------------------------------------------
      // TESTING 'cast'
      //
      // Concerns:
      //: 1 Casting from a function pointer to a data pointer and back yields
      //:   the original pointer.
      //
      // Plan:
      //: 1 Take a function pointer, cast it to a data pointer, cast that back
      //:   to a function pointer, and verify that the value remains the same.
      //:   (C-1)
      //
      // Testing:
      //   cast(const From& from);
      // --------------------------------------------------------------------

      if (verbose) printf("\nTESTING 'cast'"
                          "\n==============\n");

      {
          typedef void (*assert_t)(bool, const char *, int);

          const assert_t  original = aSsErT;
          void           *as_void_p = Util::cast<void *>(original);
          const assert_t  restored = Util::cast<assert_t>(as_void_p);

          ASSERT(aSsErT == original);
          ASSERT(aSsErT == restored);
      }

      {
          const printf_t  original = std::printf;
          void           *as_void_p = Util::cast<void *>(original);
          const printf_t  restored = Util::cast<printf_t>(as_void_p);

          ASSERT(std::printf == original);
          ASSERT(std::printf == restored);
      }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr,"Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
