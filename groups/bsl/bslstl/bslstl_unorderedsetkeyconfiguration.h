// bslstl_unorderedsetkeyconfiguration.h                              -*-C++-*-
#ifndef INCLUDED_BSLSTL_UNORDEREDSETKEYCONFIGURATION
#define INCLUDED_BSLSTL_UNORDEREDSETKEYCONFIGURATION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a configuration class to use a whole object as its own key.
//
//@CLASSES:
//   bslstl::UnorderedSetKeyConfiguration: trivial identity transformation
//
//@SEE_ALSO: bslstl_unorderedmapkeyconfiguration, bslalg_hashtableimputil
//
//@DESCRIPTION: This component provides an identity transformation.
// 'bslalg::HashTableImpUtil' has a static 'extractKey' function template
// that, given a 'value type', will represent objects stored in a data
// structure, will abstract out the 'key type' portion of that object.  In the
// case of the 'unordered_set' data structure, the 'key type' and the
// 'value type' are one and the same, so the 'extractKey' transformation is a
// trivial identity transformation.
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
// Then, we define an array full of integers to be sorted.
//..
//  int intArray[] = { 8, 3728, 2919, 27438, -2837, 18282, 34, -3 };
//  const int NUM_INTS = sizeof intArray / sizeof *intArray;
//..
// Next, we want to sort the integers.  In this case, the key being sorted on
// is the whole 'int', so we use 'bslstl::UnorderedSetKeyConfiguration<int>' as
// our extractor:
//..
//  bslstl::UnorderedSetKeyConfiguration<int> intExtractor;
//..
// Now, we sort the array using our 'sort' function and the extractor, and
// printout out the sorted array:
//..
//  mySort(intArray + 0, intArray + NUM_INTS, intExtractor);
//
//  if (verbose) {
//      printf("\nSorted integer array:\n"
//               "====================\n");
//
//      for (int i = 0; i < NUM_INTS; ++i) {
//          printf("%s%d", (i ? ", " : ""), intArray[i]);
//      }
//      printf("\n");
//  }
//..
// Finally, we observe that the output produced is:
//..
//  Sorted integer array:
//  ====================
//  -2837, -3, 8, 34, 2919, 3728, 18282, 27438
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace BloombergLP {

namespace bslstl {

                       // ===================================
                       // struct UnorderedSetKeyConfiguration
                       // ===================================

template <class VALUE_TYPE>
struct UnorderedSetKeyConfiguration {
  public:
    typedef VALUE_TYPE ValueType;
    typedef ValueType  KeyType;

    // Choosing to implement for each configuration, to reduce the template
    // mess.  With only two policies, not much is saved using a shared
    // dependent base class to provide a common implementation.

    // CLASS METHODS
    static const KeyType& extractKey(const VALUE_TYPE& object);
        // Given a specified 'object', return a reference to the 'KeyType'
        // contained within that object.  In this case, the 'KeyType' returned
        // is simply the object itself.
};

// ============================================================================
//                  TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                      //------------------------------------
                      // struct UnorderedSetKeyConfiguration
                      //------------------------------------


template <class VALUE_TYPE>
inline
const typename UnorderedSetKeyConfiguration<VALUE_TYPE>::KeyType&
UnorderedSetKeyConfiguration<VALUE_TYPE>::extractKey(const VALUE_TYPE& object)
{
    return object;
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
