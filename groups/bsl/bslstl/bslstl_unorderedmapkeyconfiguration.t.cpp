// bslstl_unorderedmapkeyconfiguration.t.cpp                          -*-C++-*-
#include <bslstl_unorderedmapkeyconfiguration.h>

#include <bslmf_isconst.h>
#include <bslmf_issame.h>
#include <bslmf_removecv.h>

#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <stdio.h>
#include <stdlib.h>

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
//
// ----------------------------------------------------------------------------
// [  ] ...
// ----------------------------------------------------------------------------
// [  ] BREATHING TEST
// [  ] USAGE EXAMPLE

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                GLOBAL TYPEDEFS AND VARIABLES FOR TESTING
//-----------------------------------------------------------------------------

template <class FIRST_TYPE, class SECOND_TYPE>
struct MyPair {
    // PUBLIC TYPES
    typedef FIRST_TYPE  first_type;
    typedef SECOND_TYPE second_type;

    // DATA
    first_type  first;
    second_type second;
};

template <class TYPE>
bool isConstObject(TYPE&)
{
    return bsl::is_const<TYPE>::value;
}

template <class CONFIGURED_TYPE>
struct IsSameType {
    template <class OBJECT_TYPE>
    bool operator ()(const OBJECT_TYPE&) const
    {
        typedef typename bsl::remove_cv<CONFIGURED_TYPE>::type  CT;
        typedef typename bsl::remove_cv<OBJECT_TYPE>::type      OT;

        return bsl::is_same<CT, OT>::value;
    }
};

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using Multiple Extractors to Sort an Array on Different Keys
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to define a 'sort' function which will work on a variety
// of different object types.  The object has to have a 'key' within it,
// possibly the whole object, which will compare with the 'key' of other
// objects with a transitive '<' operator.
//
// First, we define our function 'mySort', which takes two template args:
// 'VALUE_TYPE', the type of object being sorted, and 'KEY_EXTRACTOR', the
// utility class that will extra which part of the objects to be sorted is the
// key which will drive the sort:

template <class VALUE_TYPE, class KEY_EXTRACTOR>
void mySort(VALUE_TYPE *begin, VALUE_TYPE *end, const KEY_EXTRACTOR&)
    // This function provides an order-preserving sort of the items in the
    // range '[ begin, end )', where 'KEY_EXTRACTOR::extractKey' yields the
    // key being sorted over.  We require that 'VALUE_TYPE' support copy
    // construction and assignment.
{
    if (begin == end) return;                                         // RETURN

    while (begin != --end) {
        for (VALUE_TYPE *it = begin; it != end; ++it) {
            if (KEY_EXTRACTOR::extractKey(it[1]) <
                                            KEY_EXTRACTOR::extractKey(it[0])) {
                // they're in the wrong order -- swap them

                VALUE_TYPE tmp(it[0]);
                it[0] = it[1];
                it[1] = tmp;
            }
        }

        // '*end' is now the highest element in the range '[ begin, end ]', so
        // we only have to sort the elements before it in the next pass.
    }
}

// Then, we define 'StudentRecord', which keeps some vital statistics on
// students:

struct StudentRecord {
    const char *d_name;
    double      d_gpa;
    int         d_age;
};

// Next, we define two extractors for 'StudentRecord', which will yield the
// 'GPA' or 'Age' fields:

struct StudentRecordGPAExtractor {
    static
    const double& extractKey(const StudentRecord& record)
    {
        return record.d_gpa;
    }
};

struct StudentRecordAgeExtractor {
    static
    const int& extractKey(const StudentRecord& record)
    {
        return record.d_age;
    }
};

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test                = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose             = argc > 2;
//  bool veryVerbose         = argc > 3;
//  bool veryVeryVerbose     = argc > 4;
//  bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE
        //
        // Concern:
        //   Demonstrate the potential usage of the component.
        //
        // Plan:
        //   Demonstrate a situation where the 'key' being sought may not be
        //   the whole object, and how the component fits in with that
        //   framework.
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

// Then, we create an array of 'StudentRecord's describing a set of students,
// with their names, GPA's, and ages.
//..
        StudentRecord studentArray[] = {
            { "Phil",  3.4, 19 },
            { "Bob",   2.7, 20 },
            { "Bill",  4.2, 21 },
            { "Stan",  1.9, 18 },
            { "Ann",   2.3, 21 },
            { "Julie", 2.3, 20 } };
        const int NUM_STUDENTS = sizeof studentArray / sizeof *studentArray;
//..
// Next, using our GPA extractor and our 'mySort' function, we sort the
// students by GPA:
//..
        StudentRecordGPAExtractor gpaExtractor;

        mySort(studentArray + 0,
               studentArray + NUM_STUDENTS,
               gpaExtractor);
//..
// Then, we print out the sorted array of students:
//..
        if (verbose) {
            printf("\nList of students, lowest GPA first:\n");
            printf(  "===================================\n");

            printf("Name   GPA  AGE\n"
                   "-----  ---  ---\n");
            for (int i = 0; i < NUM_STUDENTS; ++i) {
                const StudentRecord& record = studentArray[i];

                printf("%-5s  %g  %3d\n", record.d_name,
                                          record.d_gpa,
                                          record.d_age);
            }
        }
//..
// The output produced is:
//..
//  List of students, lowest GPA first:
//  ===================================
//                              Name   GPA  AGE
//  -----  ---  ---
//  Stan   1.9   18
//  Ann    2.3   21
//  Julie  2.3   20
//  Bob    2.7   20
//  Phil   3.4   19
//  Bill   4.2   21
//..
// Note that Ann and Julie, who have the same GPA, are still in the same order
// as they were before the sort, as 'mySort' was an order-preserving sort:
//
// Next, we sort by age with our age extractor, and print out the results:
//..
        StudentRecordAgeExtractor ageExtractor;

        mySort(studentArray + 0,
               studentArray + NUM_STUDENTS,
               ageExtractor);

        if (verbose) {
            printf("\nList of students, youngest first:\n");
            printf(  "================================\n");

            printf("Name   GPA  AGE\n"
                   "-----  ---  ---\n");
            for (int i = 0; i < NUM_STUDENTS; ++i) {
                const StudentRecord& record = studentArray[i];

                printf("%-5s  %g  %3d\n", record.d_name,
                                          record.d_gpa,
                                          record.d_age);
            }
        }
//..
// The output is:
//..
//  List of students, youngest first:
//  ================================
//                              Name   GPA  AGE
//  -----  ---  ---
//  Stan   1.9   18
//  Phil   3.4   19
//  Julie  2.3   20
//  Bob    2.7   20
//  Ann    2.3   21
//  Bill   4.2   21
//..
// Note again, the ordering of students with identical ages is preserved.

// Then, suppose we are storing information about employees in 'MyPair'
// objects, where 'first' is a double storing the employees hourly wage, and
// 'second' in the employee's name.  Suppose we want to sort the employees by
// their hourly wages, which is the '.first' field of the pair.
//
// We declare our employee pair type:
//..
        typedef MyPair<double, const char *> EmployeePair;
//..
// Next, we define an array of employee pairs for employees' wages and names:
//..
        EmployeePair employees[] = {
            { 12.25, "Kyle" },
            { 15.00, "Eric" },
            { 12.25, "Stan" },
            {  7.75, "Kenny" } };
        const int NUM_EMPLOYEES = sizeof employees / sizeof *employees;
//..
// Then, we create an 'UnorderedMapKeyConfiguration' type paramtrized on
// 'EmployeePair', which will extract the '.first' field, which is the wage,
// from an employee pair:
//..
        bslstl::UnorderedMapKeyConfiguration<EmployeePair> wageExtractor;
//..
// Next, we sort:
//..
        mySort(employees + 0, employees + NUM_EMPLOYEES, wageExtractor);
//..
// Now, we print out our results:
//..
        if (verbose) {
            printf("\nList of employees, cheapest first:\n"
                     "==================================\n");

            printf("Name   Wage\n"
                   "-----  -----\n");

            for (int i = 0; i < NUM_EMPLOYEES; ++i) {
                const EmployeePair& employee = employees[i];

                printf("%-5s  %5.2f\n", employee.second, employee.first);
            }
        }
//..
// Finally, we see our output.  Note that the ordering of Kyle and Stan, who
// are paid the same wage, is preserved.
//..
//  List of employees, cheapest first:
//  ==================================
//                              Name   Wage
//  -----  -----
//  Kenny   7.75
//  Kyle   12.25
//  Stan   12.25
//  Eric   15.00
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING RESULT HAS EXPECTED TYPE
        //
        // Concern:
        //   That the result of 'extractKey' is always the same type (except
        //   for cv qualifiers) as passed type:
        //
        // Plan:
        //   Use the 'isSameType' method defined above to verify this.
        // --------------------------------------------------------------------

        if (verbose) printf("TESTING RESULT IS CONST\n"
                            "=======================\n");

        typedef MyPair<int, double> IntDoublePr;

        int i;

        ASSERT(1 == IsSameType<int>()(i));

        const int j = 4;

        ASSERT(1 == IsSameType<int>()(j));

        FILE file;

        ASSERT(1 == IsSameType<FILE>()(file));
        ASSERT(0 == IsSameType<int >()(file));

        const FILE cFile = file;

        ASSERT(1 == IsSameType<FILE>()(cFile));
        ASSERT(0 == IsSameType<int >()(cFile));

        IntDoublePr pr = { 3, 4.7 };    const IntDoublePr& PR = pr;

        ASSERT(1 == IsSameType<int>()(
           bslstl::UnorderedMapKeyConfiguration<IntDoublePr>::extractKey(pr)));
        ASSERT(1 == IsSameType<int>()(
           bslstl::UnorderedMapKeyConfiguration<IntDoublePr>::extractKey(PR)));
        ASSERT(0 == IsSameType<IntDoublePr>()(
           bslstl::UnorderedMapKeyConfiguration<IntDoublePr>::extractKey(pr)));
        ASSERT(0 == IsSameType<double>()(
           bslstl::UnorderedMapKeyConfiguration<IntDoublePr>::extractKey(PR)));
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'KeyType, 'ValueType', RESULT IS CONST
        //
        // Concern:
        //   That the result of 'extractKey' is always a const value.
        //
        // Plan:
        //   Use the 'isConstObject' method defined above to verify this.
        // --------------------------------------------------------------------

        if (verbose) printf(
                           "TESTING 'KeyType, 'ValueType', RESULT IS CONST\n"
                           "==============================================\n");

        typedef MyPair<int, FILE> IntFilePr;

        typedef bslstl::UnorderedMapKeyConfiguration<IntFilePr>::KeyType   KT;
        typedef bslstl::UnorderedMapKeyConfiguration<IntFilePr>::ValueType VT;

        // TBD: Should 'KeyType' be const?  It's not.

        ASSERT(0 == bsl::is_const<KT>::value);
        ASSERT(0 == bsl::is_const<VT>::value);

        ASSERT(1 == (bsl::is_same<int,       KT>::value));
        ASSERT(1 == (bsl::is_same<IntFilePr, VT>::value));

        typedef MyPair<int, int> IntPair;

        IntPair ip;

        ASSERT(0 == isConstObject(ip));

        const IntPair& jp = ip;

        ASSERT(1 == isConstObject(jp));

        ASSERT(1 == isConstObject(
               bslstl::UnorderedMapKeyConfiguration<IntPair>::extractKey(ip)));
        ASSERT(1 == isConstObject(
               bslstl::UnorderedMapKeyConfiguration<IntPair>::extractKey(jp)));
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        typedef MyPair<int, int> VTypeII;
        typedef bslstl::UnorderedMapKeyConfiguration<VTypeII> ConfigurationII;

        const VTypeII v1 = { 5, 7 };

        ASSERT(5 == ConfigurationII::extractKey(v1));


        typedef MyPair<double, int> VTypeDI;
        typedef bslstl::UnorderedMapKeyConfiguration<VTypeDI> ConfigurationDI;

        const double x = 7.3;

        const VTypeDI v2 = { x, 5 };

        ASSERT(x == ConfigurationDI::extractKey(v2));
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
