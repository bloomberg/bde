// bslstl_unorderedmapkeyconfiguration.h                              -*-C++-*-
#ifndef INCLUDED_BSLSTL_UNORDEREDMAPKEYCONFIGURATION
#define INCLUDED_BSLSTL_UNORDEREDMAPKEYCONFIGURATION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class template to extract keys as the 'first' attribute.
//
//@CLASSES:
//   bslalg::UnorderedMapKeyConfiguration : extracts 'key' from 'value' type
//
//@SEE_ALSO: bslalg_hashtableimputil
//
//@DESCRIPTION: This component will, given an object of a value type consisting
// of a key type and some other information, return a const reference to only
// the key type within that object.  The object passed will be of parameterized
// type 'VALUE_TYPE', for which a type 'VALUE_TYPE::first_type' must be
// defined and be of the key type, and for which the operation '.first' must be
// defined and must yield the object of the key type.
//
// 'bslalg::HashTableImpUtil' has a static 'extractKey' function template that,
// given a 'value type', will represent objects stored in a data structure,
// will abstract out the 'key type' portion of that object.  In the case of the
// 'unordered_map' data structure, the 'value type' will be 'bsl::pair', and
// the key type will 'bsl::pair::first_type'.
//
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
//..
//  template <class VALUE_TYPE, class KEY_EXTRACTOR>
//  void mySort(VALUE_TYPE *begin, VALUE_TYPE *end, const KEY_EXTRACTOR&)
//      // This function provides an order-preserving sort of the items in the
//      // range '[begin .. end)', where 'KEY_EXTRACTOR::extractKey' yields the
//      // key being sorted over.  We require that 'VALUE_TYPE' support copy
//      // construction and assignment.
//  {
//      while (begin < --end) {
//          for (VALUE_TYPE *it = begin; it < end; ++it) {
//              if (KEY_EXTRACTOR::extractKey(it[1]) <
//                                          KEY_EXTRACTOR::extractKey(it[0])) {
//                  // they're in the wrong order -- swap them
//
//                  VALUE_TYPE tmp(it[0]);
//                  it[0] = it[1];
//                  it[1] = tmp;
//              }
//          }
//
//          // '*end' is now the highest element in the range '[begin .. end]',
//          // so we only have to sort the elements before it in the next pass.
//      }
//  }
//..
// Then, we define 'StudentRecord', which keeps some vital statistics on
// students:
//..
//  struct StudentRecord {
//      const char *d_name;
//      double      d_gpa;
//      int         d_age;
//  };
//..
// Next, we define two extractors for 'StudentRecord', which will yield the
// 'GPA' or 'Age' fields:
//..
//  struct StudentRecordGPAExtractor {
//      static
//      const double& extractKey(const StudentRecord& record)
//      {
//          return record.d_gpa;
//      }
//  };
//
//  struct StudentRecordAgeExtractor {
//      static
//      const int& extractKey(const StudentRecord& record)
//      {
//          return record.d_age;
//      }
//  };
//..
// Then, in 'main', we create an array of 'StudentRecord's describing a set of
// students, with their names, GPA's, and ages.
//..
//  StudentRecord studentArray[] = {
//      { "Phil",  3.4, 19 },
//      { "Bob",   2.7, 20 },
//      { "Bill",  4.2, 21 },
//      { "Stan",  1.9, 18 },
//      { "Ann",   2.3, 21 },
//      { "Julie", 2.3, 20 } };
//  const int NUM_STUDENTS = sizeof studentArray / sizeof *studentArray;
//..
// Next, using our GPA extractor and our 'mySort' function, we sort the
// students by GPA:
//..
//  StudentRecordGPAExtractor gpaExtractor;
//
//  mySort(studentArray + 0,
//         studentArray + NUM_STUDENTS,
//         gpaExtractor);
//..
// Then, we print out the sorted array of students:
//..
//  if (verbose) {
//      printf("\nList of students, lowest GPA first:\n");
//      printf(  "===================================\n");
//
//      printf("Name   GPA  AGE\n"
//             "-----  ---  ---\n");
//      for (int i = 0; i < NUM_STUDENTS; ++i) {
//          const StudentRecord& record = studentArray[i];
//
//          printf("%-5s  %g  %3d\n", record.d_name,
//                                    record.d_gpa,
//                                    record.d_age);
//      }
//  }
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
//  StudentRecordAgeExtractor ageExtractor;
//
//  mySort(studentArray + 0,
//         studentArray + NUM_STUDENTS,
//         ageExtractor);
//
//  if (verbose) {
//      printf("\nList of students, youngest first:\n");
//      printf(  "================================\n");
//
//      printf("Name   GPA  AGE\n"
//             "-----  ---  ---\n");
//      for (int i = 0; i < NUM_STUDENTS; ++i) {
//          const StudentRecord& record = studentArray[i];
//
//          printf("%-5s  %g  %3d\n", record.d_name,
//                                    record.d_gpa,
//                                    record.d_age);
//      }
//  }
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
//
// Then, suppose we are storing information about employees in 'MyPair'
// objects, where 'first' is a double storing the employees hourly wage, and
// 'second' in the employee's name.  Suppose we want to sort the employees by
// their hourly wages, which is the '.first' field of the pair.
//
// We declare our employee pair type:
//..
//  typedef MyPair<double, const char *> EmployeePair;
//..
// Next, we define an array of employee pairs for employees' wages and names:
//..
//  EmployeePair employees[] = {
//      { 12.25, "Kyle" },
//      { 15.00, "Eric" },
//      { 12.25, "Stan" },
//      {  7.75, "Kenny" } };
//  const int NUM_EMPLOYEES = sizeof employees / sizeof *employees;
//..
// Then, we create an 'UnorderedMapKeyConfiguration' type parameterized on
// 'EmployeePair', which will extract the '.first' field, which is the wage,
// from an employee pair:
//..
//  bslstl::UnorderedMapKeyConfiguration<EmployeePair> wageExtractor;
//..
// Next, we sort:
//..
//  mySort(employees + 0, employees + NUM_EMPLOYEES, wageExtractor);
//..
// Now, we print out our results:
//..
//  if (verbose) {
//      printf("\nList of employees, cheapest first:\n"
//               "==================================\n");
//
//      printf("Name   Wage\n"
//             "-----  -----\n");
//
//      for (int i = 0; i < NUM_EMPLOYEES; ++i) {
//          const EmployeePair& employee = employees[i];
//
//          printf("%-5s  %5.2f\n", employee.second, employee.first);
//      }
//  }
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

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {
namespace bslstl {

                      // ===================================
                      // struct UnorderedMapKeyConfiguration
                      // ===================================

template <class VALUE_TYPE>
struct UnorderedMapKeyConfiguration {
  public:
    typedef          VALUE_TYPE             ValueType;
    typedef typename ValueType::first_type  KeyType;

    // Choosing to implement for each configuration, to reduce the template
    // mess.  With only two policies, not much is saved using a shared
    // dependent base class to provide a common implementation.  This is the
    // key abstraction, turning 'bslalg::BidirectionalLink*' into 'VALUE_TYPE&'

    // CLASS METHODS
    static const KeyType& extractKey(const VALUE_TYPE& obj);
        // Return the member 'first' of the specified object 'obj'.
        // 'obj.first' must of of type 'VALUE_TYPE::first_type', which is the
        // 'key' portion of 'obj'.
};

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                       //-----------------------------------
                       // class UnorderedMapKeyConfiguration
                       //-----------------------------------

// CLASS METHODS
template <class VALUE_TYPE>
inline
const typename UnorderedMapKeyConfiguration<VALUE_TYPE>::KeyType&
UnorderedMapKeyConfiguration<VALUE_TYPE>::extractKey(const VALUE_TYPE& obj)
{
    return obj.first;
}

}  // close package namespace

}  // close enterprise namespace

#endif

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
