// balm_category.t.cpp                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <balm_category.h>

#include <bslma_allocator.h>
#include <bslma_testallocator.h>
#include <bslma_defaultallocatorguard.h>

#include <bsl_ostream.h>
#include <bsl_cstring.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsls_atomic.h>

#include <bslim_testutil.h>

using namespace BloombergLP;

using bsl::cout;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// A 'balm::Category' is a simple mechanism class containing a character
// pointer and a boolean value.
// ----------------------------------------------------------------------------
// balm::Category
// CREATORS
// [ 3] explicit balm::Category(const char *, bool    );
// [ 3] ~balm::Category();
// MANIPULATORS
// [ 4] void setName(const char *);
// [ 6] void setEnabled(bool );
// [ 6] void registerCategoryHolder(balm::CategoryHolder *);
// ACCESSORS
// [ 3] const char *name() const;
// [ 3] bool enabled() const;
// [ 6] bool isEnabledRaw() const;
// [ 5] bsl::ostream& print(bsl::ostream& ) const;
// FREE OPERATORS
// [ 5] bsl::ostream& operator<<(bsl::ostream& , const balm::Category& );
// ----------------------------------------------------------------------------
// balm::CategoryHolder
// MANIPULATORS
// [ 2] void reset();
// [ 2] void setCategory(const balm::Category *category);
// [ 2] void setEnabled(bool enabledFlag);
// [ 2] void setNext(balm::CategoryHolder *holder);
// ACCESSORS
// [ 2] const balm::Category *category() const;
// [ 2] bool enabled() const;
// [ 2] balm::CategoryHolder *next() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef balm::Category       Obj;
typedef balm::CategoryHolder Holder;
// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------



// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    bslma::TestAllocator testAlloc; bslma::TestAllocator *Z = &testAlloc;
    bslma::TestAllocator defaultAllocator;
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 7: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

///Usage
///-----
// The following example demonstrates how to create, access, and modify a
// 'balm::Category' object.  We start by creating three category objects with
// different names:
//..
    balm::Category categoryA("A", true);
    balm::Category categoryB("B", false);
    balm::Category categoryC("C");
//..
// Once the category object have been created we can use the 'name' and
// 'enabled' method to access their value.
//..
    ASSERT(0 == bsl::strcmp("A", categoryA.name()));
    ASSERT(0 == bsl::strcmp("B", categoryB.name()));
    ASSERT(0 == bsl::strcmp("C", categoryC.name()));

    ASSERT( categoryA.enabled());
    ASSERT(!categoryB.enabled());
    ASSERT( categoryC.enabled());
//..
// Finally, we modify the enablement status of one of the categories, and then
// write all three categories to the console.
//..
    categoryC.setEnabled(false);

    bsl::cout << "categoryA: " << categoryA << bsl::endl
              << "categoryB: " << categoryB << bsl::endl
              << "categoryC: " << categoryC << bsl::endl;
//..
// The console output will look like:
//..
//  categoryA: [ A ENABLED ]
//  categoryB: [ B DISABLED ]
//  categoryC: [ C DISABLED ]
//..
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATORS: registerCategoryHolder, setEnabled
        //
        // Concerns:
        //   The manipulators properly modify the object under test.
        //
        // Plan:
        //   Specify a set S of (unique) values with substantial and varied
        //   differences in value.  Construct a set of objects from a default
        //   value and then use the manipulator methods to set the value to a
        //   value in the set 'S'.
        //
        // Testing:
        //   void registerCategoryHolder(balm::CategoryHolder *holder);
        //   void setEnabled(bool );
        //   void isEnabledRaw();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'registerCategoryHolder' and "
                          << "'setEnabled" << endl;
        enum { NUM_ELEMENTS = 10 };
        balm::CategoryHolder holders[NUM_ELEMENTS];
        {
            balm::Category c("A");
            for (int i = 0; i < NUM_ELEMENTS; ++i) {
                c.registerCategoryHolder(&holders[i]);
                balm::CategoryHolder *next = (0 == i) ? 0 : &holders[i - 1];
                ASSERT(&c   == holders[i].category());
                ASSERT(true == holders[i].enabled());
                ASSERT(next == holders[i].next());
            }

            for (int i = 0; i < NUM_ELEMENTS; ++i) {
                balm::CategoryHolder *next = (0 == i) ? 0 : &holders[i - 1];
                ASSERT(&c   == holders[i].category());
                ASSERT(true == holders[i].enabled());
                ASSERT(next == holders[i].next());
            }

            const bsls::AtomicInt *enabled_p = &c.isEnabledRaw();

            c.setEnabled(false);

            ASSERT(false == *enabled_p);
            ASSERT(&c.isEnabledRaw() == enabled_p);

            for (int i = 0; i < NUM_ELEMENTS; ++i) {
                balm::CategoryHolder *next = (0 == i) ? 0 : &holders[i - 1];
                ASSERT(&c    == holders[i].category());
                ASSERT(false == holders[i].enabled());
                ASSERT(next  == holders[i].next());
            }

            c.setEnabled(true);

            ASSERT(true == *enabled_p);
            ASSERT(&c.isEnabledRaw() == enabled_p);

            for (int i = 0; i < NUM_ELEMENTS; ++i) {
                balm::CategoryHolder *next = (0 == i) ? 0 : &holders[i - 1];
                ASSERT(&c   == holders[i].category());
                ASSERT(true == holders[i].enabled());
                ASSERT(next == holders[i].next());
            }
        }
        for (int i = 0; i < NUM_ELEMENTS; ++i) {
            ASSERT(0     == holders[i].category());
            ASSERT(false == holders[i].enabled());
            ASSERT(0     == holders[i].next());
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR:
        //
        // Concerns:
        //   The output operator properly writes formatted information
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   'ostrstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the expected
        //   output format.
        //
        // Testing:
        //   operator<<(ostream&, const balm::Category&);
        //   ostream& print(ostream&);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTesting ouput operators" << endl;
        struct {
            const char *d_name;
            bool        d_enabled;
            const char *d_expected;
        } DATA [] = {
            {      "A",  true, "[ A ENABLED ]" },
            {      "A", false, "[ A DISABLED ]"},
            { "myCatB",  true, "[ myCatB ENABLED ]"},
            { "myCatB", false, "[ myCatB DISABLED ]"},
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            Obj mX(DATA[i].d_name, DATA[i].d_enabled); const Obj& MX = mX;

            bsl::ostringstream printBuf, operBuf;
            MX.print(printBuf);
            operBuf << MX;

            bsl::string EXP(DATA[i].d_expected);
            bsl::string printVal(printBuf.str());
            bsl::string operVal(operBuf.str());

            if (veryVerbose) {
                P_(printVal); P_(operVal); P(EXP);
            }
            LOOP_ASSERT(i, EXP == printVal);
            LOOP_ASSERT(i, EXP == operVal);
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING MANIPULATORS
        //
        // Concerns:
        //   The manipulators properly modify the object under test.
        //
        // Plan:
        //   Specify a set S of (unique) values with substantial and varied
        //   differences in value.  Construct a set of objects from a default
        //   value and then use the manipulator methods to set the value to a
        //   value in the set 'S'.
        //
        // Testing:
        //   void setName(const char *name);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Manipulators" << endl;
        struct {
            const char *d_name;
            bool        d_enabled;
        } DATA [] = {
            {      "",  false },
            {      "",  true  },
            {      "A", false },
            {      "A", true  },
            {      " ", true  },
            {      " ", false },
            {      "B", true  },
            {      "B", false },
            {   "CAT1", true  },
            {   "CAT1", false },
            {   "cat1", true  },
            {   "cat1", false },
            { "myCatA", true  },
            { "myCatA", false }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            for (int j = 0; j < NUM_DATA; ++j) {
                Obj u(DATA[i].d_name, DATA[i].d_enabled); const Obj& U = u;
                Obj w(DATA[j].d_name, DATA[j].d_enabled); const Obj& W = w;

                ASSERT(DATA[i].d_name      == U.name());
                ASSERT(DATA[i].d_enabled == U.enabled());

                // Set the value.
                u.setName(DATA[j].d_name);

                ASSERT(DATA[j].d_name      == U.name());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING BASIC CONSTRUCTOR and ACCESSORS (BOOTSTRAP):
        //
        // Concerns:
        //   The primary fields must be correctly modifiable and accessible.
        //
        // Plan:
        //   First, verify the default enabled parameter by testing the value
        //   of the resulting object.
        //
        //   Next, for a sequence of independent test values, use the to
        //   create an object with a particular value.  Verify that value
        //   using the basic accessors.  Note that the destructor is exercised
        //   on each configuration as the object being tested leaves scope.
        //
        // Testing:
        //   explicit balm::Category(const char *name, bool enabled = true);
        //   const char *name() const;
        //   bool enabled() const;
        // --------------------------------------------------------------------
          {
              if (veryVerbose) cout << "\tTest constructor default args."
                                    << bsl::endl;

              Obj mX("X"); const Obj& MX = mX;
              Obj mY("Y"); const Obj& MY = mY;

              ASSERT(MX.enabled());
              ASSERT(MY.enabled());
              ASSERT(true == MX.isEnabledRaw());
              ASSERT(true == MY.isEnabledRaw());
              ASSERT(&MX.isEnabledRaw() != &MY.isEnabledRaw());
          }
          {
              if (veryVerbose)
                  cout << "\tTest constructor and basic accessors (bootstrap)"
                       << bsl::endl;

              struct {
                  const char *d_name;
                  bool        d_enabled;
              } DATA [] = {
                  {      "",  false },
                  {      "",  true  },
                  {      "A", false },
                  {      "A", true  },
                  {      " ", true  },
                  {      " ", false },
                  {      "B", true  },
                  {      "B", false },
                  {   "CAT1", true  },
                  {   "CAT1", false },
                  {   "cat1", true  },
                  {   "cat1", false },
                  { "myCatA", true  },
                  { "myCatA", false }
              };
              const int NUM_DATA = sizeof DATA / sizeof *DATA;

              for (int i = 0; i < NUM_DATA; ++i) {
                  Obj mX(DATA[i].d_name, DATA[i].d_enabled);
                  const Obj& MX = mX;

                  ASSERT(DATA[i].d_name    == MX.name());
                  ASSERT(DATA[i].d_enabled == MX.enabled());
                  ASSERT(DATA[i].d_enabled == MX.isEnabledRaw());
                  ASSERT(0 == defaultAllocator.numBytesInUse());
                  ASSERT(0 == testAlloc.numBytesInUse());
              }
          }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING BASIC CONSTRUCTOR and ACCESSORS (BOOTSTRAP):
        //                                                 balm::CategoryHolder
        //
        // Concerns:
        //   The primary fields must be correctly modifiable and accessible.
        //
        // Plan:
        //   First, verify the default enabled parameter by testing the value
        //   of the resulting object.
        //
        //   Next, for a sequence of independent test values, use the to
        //   create an object with a particular value.  Verify that value
        //   using the basic accessors.  Note that the destructor is exercised
        //   on each configuration as the object being tested leaves scope.
        //
        // Testing:
        //   void reset();
        //   void setCategory(const balm::Category *category);
        //   void setEnabled(bool enabledFlag);
        //   void setNext(balm::CategoryHolder *holder);
        //   const balm::Category *category() const;
        //   bool enabled() const;
        //   balm::CategoryHolder *next() const;
        // --------------------------------------------------------------------


        balm::Category a("A");
        balm::Category b("B");
        balm::CategoryHolder ha;
        balm::CategoryHolder hb;
        struct {
            const balm::Category *d_category_p;
            bool                 d_enabled;
            balm::CategoryHolder *d_next_p;
        } VALUES[]  = {
            {  0, false,   0 },
            {  0, true,    0 },
            { &a, false,   0 },
            { &a, false, &ha },
            { &a, false, &hb },
            { &a, true,    0 },
            { &a, true,  &ha },
            { &a, true,  &hb },
            { &b, false,   0 },
            { &b, false, &ha },
            { &b, false, &hb },
            { &b, true,    0 },
            { &b, true,  &ha },
            { &b, true,  &hb }
        };
        const int NUM_VALUES = sizeof(VALUES)/sizeof(*VALUES);

        {
            if (veryVerbose) cout << "\tTest initialization and accessors"
                                  << bsl::endl;

            for (int i = 0; i < NUM_VALUES; ++i) {
                Holder mX = { VALUES[i].d_enabled,
                              VALUES[i].d_category_p,
                              VALUES[i].d_next_p };
                const Holder& MX = mX;

                ASSERT(VALUES[i].d_category_p == MX.category());
                ASSERT(VALUES[i].d_enabled    == MX.enabled());
                ASSERT(VALUES[i].d_next_p     == MX.next());
            }
        }
        {
            if (veryVerbose) cout << "\tTest manipulators" << bsl::endl;
            for (int i = 0; i < NUM_VALUES; ++i) {
                Holder mX = { false, 0, 0 }; const Holder& MX = mX;

                ASSERT(0     == MX.category());
                ASSERT(false == MX.enabled());
                ASSERT(0     == MX.next());

                mX.setCategory(VALUES[i].d_category_p);
                mX.setEnabled(VALUES[i].d_enabled);
                mX.setNext(VALUES[i].d_next_p);

                ASSERT(VALUES[i].d_category_p == MX.category());
                ASSERT(VALUES[i].d_enabled    == MX.enabled());
                ASSERT(VALUES[i].d_next_p     == MX.next());

                mX.reset();

                ASSERT(0     == MX.category());
                ASSERT(false == MX.enabled());
                ASSERT(0     == MX.next());
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        const char *VAL_A = "A";
        const char *VAL_B = "B";
        const char *VAL_C = "C";
        Obj mA(VAL_A); const Obj &MA = mA;
        Obj mB(VAL_B, true); const Obj &MB = mB;
        Obj mC(VAL_C, false); const Obj &MC = mC;

        ASSERT(VAL_A == MA.name());
        ASSERT(VAL_B == MB.name());
        ASSERT(VAL_C == MC.name());
        ASSERT(true  == MA.enabled());
        ASSERT(true  == MA.isEnabledRaw());
        ASSERT(true  == MB.enabled());
        ASSERT(true  == MB.isEnabledRaw());
        ASSERT(false == MC.enabled());
        ASSERT(false == MC.isEnabledRaw());

        Obj mX(VAL_A);  const Obj& MX = mX;
        ASSERT(VAL_A == MX.name());
        ASSERT(true  == MX.enabled());
        ASSERT(true  == MX.isEnabledRaw());

        mX.setName(VAL_B);
        ASSERT(VAL_B == MX.name());
        ASSERT(true  == MX.enabled());
        ASSERT(true  == MX.isEnabledRaw());

        mX.setEnabled(false);
        ASSERT(VAL_B == MX.name());
        ASSERT(false == MX.enabled());
        ASSERT(false == MX.isEnabledRaw());

        mX.setEnabled(true);
        ASSERT(VAL_B == MX.name());
        ASSERT(true  == MX.enabled());
        ASSERT(true  == MX.isEnabledRaw());

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

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
