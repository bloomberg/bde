// bslfmt_mockparsecontext.t.cpp                                      -*-C++-*-
#include <bslfmt_mockparsecontext.h>

#include <bslfmt_formattercharutil.h>
#include <bslfmt_formatterspecificationnumericvalue.h>

#include <bsls_bsltestutil.h>
#include <bsls_util.h>  // Testing only

#include <bslstl_string.h>

#include <stdio.h>

using namespace BloombergLP;


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

#define ASSERT BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q BSLS_BSLTESTUTIL_Q    // Quote identifier literally.
#define P BSLS_BSLTESTUTIL_P    // Print identifier and value.
#define P_ BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_ BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_ BSLS_BSLTESTUTIL_L_  // current Line number

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Testing Formatter's `parse` Method
///- - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a formatter for our custom type representing a month and we
// want to test it.  The following example demonstrates how we can test its
// `parse` method using `bslfmt::MockParseContext`.
//
// First, we define our `Month` class:
// ```
   /// This class implements a complex-constrained, value-semantic type for
   /// representing months.
   class Month {
     private:
       // DATA
       int d_index;   // month's index

     public:
       // CREATORS

       /// Create an object having the value represented by the specified
       /// `index`.
       Month(int index)
       : d_index(index)
       {
           ASSERT((1 <= index) && (12 >= index));
       }

       // ACCESSORS

       /// Return the index of this month.
       int index() const { return d_index; }
   };
// ```
// Then, we define our custom formatter for this class.  In it, two methods are
// necessary: `parse` and `format`.  The `parse` method parses the format
// string itself to determine the formatting to be used by the `format` method,
// which writes the formatted object into user-supplied output iterator.
// ```
   /// This struct is a base class for `bsl::formatter` specializations for
   /// the `Month` class.
   template <class t_CHAR>
   struct MonthFormatter {
// ```
// The convenience of using the `bsl::format` function is that the users can
// come up with the description language themselves.  In our case, for
// simplicity, we will present month in two formats - numeric ("03") and verbal
// ("March").  Accordingly, to indicate the desired type, we will use one of
// the two letters in the format description: 'n' ('N') or 'v' ('V').
// Additionally, user can specify minimal width of the output either string via
// digit in the format specification or via additional parameter for
// `bsl::format` function.
// ```
       // TYPES
       enum Format {
           e_NUMERIC,  // "03"
           e_VERBAL    // "March"
       };

       typedef bslfmt::FormatterSpecificationNumericValue NumericValue;

       // DATA
       Format       d_format;    // output format
       NumericValue d_rawWidth;  // minimal output width

     public:
       // CREATORS

       /// Create a formatter that outputs values in the `e_NUMERIC` format.
       /// Thus, numeric is the default format for the `Month` object.
       BSLS_KEYWORD_CONSTEXPR_CPP20 MonthFormatter()
       : d_format(e_NUMERIC)
       {
       }

       // MANIPULATORS

       /// Parse the specified `context` and return end iterator of parsed
       /// range.
       template <class t_PARSE_CONTEXT>
       BSLS_KEYWORD_CONSTEXPR_CPP20 typename t_PARSE_CONTEXT::iterator parse(
                                                      t_PARSE_CONTEXT& context)
       {
// ```
// `MockParseContext` completely repeats the interface and behavior of the
// `bslfmt::basic_format_parse_context`, but provides an additional accessor
// that allows users to get information about the parsing process.  Therefore
// users do not need to declare a separate overload of `parse` for test
// purposes as long as their formatter's `parse` method is templated.
// ```
           typename t_PARSE_CONTEXT::const_iterator current = context.begin();
           typename t_PARSE_CONTEXT::const_iterator end     = context.end();

           // Handling empty string or empty specification
           if (current == end || *current == '}') {
               return context.begin();                                // RETURN
           }

           d_rawWidth.parse(&current, end, false);
           // Non-relative widths must be strictly positive.
           if (d_rawWidth == NumericValue(NumericValue::e_VALUE, 0)) {
               BSLS_THROW(bsl::format_error("Field widths must be > 0."));
           }

           if (d_rawWidth.category() == NumericValue::e_ARG_ID) {
               context.check_arg_id(d_rawWidth.value());
           }
           else if (d_rawWidth.category() == NumericValue::e_NEXT_ARG) {
               d_rawWidth = NumericValue(
                                     NumericValue::e_ARG_ID,
                                     static_cast<int>(context.next_arg_id()));
           }

           if (current == end || *current == '}') {
               return context.begin();                                // RETURN
           }

           // Reading format specification
           switch (*current) {
               case 'V':
               case 'v': {
                 d_format = e_VERBAL;
               } break;
               case 'N':
               case 'n': {
                 // `e_NUMERIC` value is assigned at object construction
               } break;
               default: {
                 BSLS_THROW(bsl::format_error(
                      "Unexpected symbol in format specification"));   // THROW
               }
           }

           // Move the iterator to the next position and check that there are
           // no extra characters in the description.

           ++current;

           if (current != end && *current != '}') {
               BSLS_THROW(bsl::format_error(
                       "Too many symbols in format specification"));   // THROW
           }

           context.advance_to(current);
           return context.begin();
       }
// ```
// To reduce the size of this example, we will omit the implementation of the
// `format` method as it is not essential for our purposes.
// ```
//     // ACCESSORS
//
//     /// Create string representation of the specified `value`, customized
//     /// in accordance with the requested format and the specified `context`,
//     /// and copy it to the output that the output iterator of the `context`
//     /// points to.
//     template <class t_FORMAT_CONTEXT>
//     typename t_FORMAT_CONTEXT::iterator format(
//                                             Month             value,
//                                             t_FORMAT_CONTEXT& context) const
//     {
//         typename t_FORMAT_CONTEXT::iterator outIterator = context.out();
//
//         NumericValue postprocessedWidth = d_rawWidth;
//         postprocessedWidth.postprocess(context);
//         int widthValue = 0;
//
//         switch (postprocessedWidth.category()) {
//           case NumericValue::e_DEFAULT: {
//               // Width was not presented in the specification.
//           } break;
//           case NumericValue::e_VALUE: {
//             widthValue = postprocessedWidth.value();
//           } break;
//           default: {
//             BSLS_THROW(
//                bsl::format_error("Invalid precision specifier"));   // THROW
//           }
//         }
//
//         int monthIndex = value.index();
//
//         if (e_VERBAL == d_format) {  // March
//             static const char *const months[] = {"January",
//                                                  "February",
//                                                  "March",
//                                                  "April",
//                                                  "May",
//                                                  "June",
//                                                  "July",
//                                                  "August",
//                                                  "September",
//                                                  "October",
//                                                  "November",
//                                                  "December"};
//
//             const char *name       = months[monthIndex - 1];
//             const int   nameLength = static_cast<int>(strlen(name));
//
//             outIterator = BloombergLP::bslfmt::FormatterCharUtil<
//                 t_CHAR>::outputFromChar(name,
//                                         name + nameLength,
//                                         outIterator);
//
//             // The string is padded on the right with spaces if it is
//             // shorter than the specified width.
//
//             if (nameLength < widthValue) {
//                 for (int i = 0;
//                      i < widthValue - nameLength;
//                      ++i) {
//                     outIterator = BloombergLP::bslfmt::FormatterCharUtil<
//                         t_CHAR>::outputFromChar(' ', outIterator);
//                 }
//             }
//         }
//         else if (e_NUMERIC == d_format) {  // 03
//             if (10 > monthIndex) {
//                 outIterator = BloombergLP::bslfmt::FormatterCharUtil<
//                     t_CHAR>::outputFromChar('0', outIterator);
//             }
//             else {
//                 outIterator = BloombergLP::bslfmt::FormatterCharUtil<
//                     t_CHAR>::outputFromChar('1', outIterator);
//                 monthIndex -= 10;
//
//             }
//             outIterator = BloombergLP::bslfmt::FormatterCharUtil<
//                 t_CHAR>::outputFromChar('0' + static_cast<char>(monthIndex),
//                                         outIterator);
//
//             // The string is padded on the right with spaces if it is
//             // shorter than the specified width.
//
//             if (2 < widthValue) {
//                 for (int i = 0;
//                      i < widthValue - 2;
//                      ++i) {
//                     outIterator = BloombergLP::bslfmt::FormatterCharUtil<
//                         t_CHAR>::outputFromChar(' ', outIterator);
//                 }
//             }
//         }
//         return outIterator;
//     }
// ```
   };
// ```
//
// namespace bsl {
//
// template <class t_CHAR>
// struct formatter<Month, t_CHAR> : MonthFormatter<t_CHAR> {
// };
//
// }  // close namespace bsl

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    const int  test    = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;

    printf("TEST %s CASE %d \n", __FILE__, test);

    switch (test) {  case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        //: 1 Demonstrate the functioning of this component.
        //
        // Plan:
        //: 1 Use mock parse context to parse a format specification.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

// ```
// Finally, we can test the operation of the `parse` function for different
// input specifications:
// ```
   typedef bslfmt::MockParseContext<char> Context;
   typedef Context::iterator              ContextIterator;

   {
       MonthFormatter<char> formatter;
       Context              context("v");

       ContextIterator iterator = formatter.parse(context);

       ASSERT(context.end() == iterator);
// ```
// Since width is not presented in the format specification, we don't expect
// our context to change its indexing mode.
// ```
       ASSERT(Context::e_UNKNOWN == context.indexingMode());
   }
   {
       MonthFormatter<char> formatter;
       Context              context("8v");

       ContextIterator iterator = formatter.parse(context);

       ASSERT(context.end()      == iterator);
       ASSERT(Context::e_UNKNOWN == context.indexingMode());
   }
   {
       MonthFormatter<char> formatter;
       Context              context("{}v", 1);

       ContextIterator iterator = formatter.parse(context);

       ASSERT(context.end()        == iterator);
// ```
// And here it is assumed that the width will be determined by the next
// parameter of the `bsl::format` function.  The indexing mode of the context
// changes accordingly.
// ```
       ASSERT(Context::e_AUTOMATIC == context.indexingMode());
   }
   {
       MonthFormatter<char> formatter;
       Context              context("{1}v", 2);

       ContextIterator iterator = formatter.parse(context);

       ASSERT(context.end()     == iterator);
// ```
// Here we explicitly indicate the ordinal number of the `bsl::format`
// parameter storing the width value.
// ```
       ASSERT(Context::e_MANUAL == context.indexingMode());
   }

// ```

        // Unfortunately, due to the position of this component in the class
        // hierarchy, we can not call `bsl::format` function to make sure that
        // our `MonthFormatter` works correctly.  But the following code along
        // with the `Month` and `MonthFormatter` definitions, and also with
        // appropriate `bsl::formatter` specialization can be copy-pasted to
        // some test driver that has the `bsl::format` available to check it.
        //
        // Month       april(4);
        // bsl::string result = bsl::format("{:v}", april);
        // ASSERTV(result.c_str(), bsl::string("April") == result);
        //
        // Month may(5);
        // result = bsl::format("{:8v}", may);
        // ASSERTV(result.c_str(), bsl::string("     May") == result);
        //
        // Month june(6);
        // result = bsl::format("{:{}v}", june, 6);
        // ASSERTV(result.c_str(), bsl::string("  June") == result);
        //
        // Month july(7);
        // result = bsl::format("{0:{1}v}", july, 3);
        // ASSERTV(result.c_str(), bsl::string("July") == result);
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
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nBREATHING TEST"
                   "\n==============\n");

        if (verbose) printf("\tMockParseContext\n");
        {
            typedef  bslfmt::MockParseContext<char>       CharMockParseContext;
            typedef  CharMockParseContext::const_iterator ContextConstIterator;

            bsl::string_view format = "{:}";
            size_t           numArgs = 2;

            bslfmt::MockParseContext<char> mockParseContext(format, numArgs);
            ASSERTV(mockParseContext.indexingMode(),
                    CharMockParseContext::e_UNKNOWN ==
                        mockParseContext.indexingMode());
            ContextConstIterator contextIt = mockParseContext.begin();

            ASSERTV(format.begin() == contextIt);

            contextIt = mockParseContext.end();

            ASSERTV(format.end() == contextIt);

            ContextConstIterator formatIt = format.begin();
            ++formatIt;
            mockParseContext.advance_to(formatIt);

            ASSERTV(formatIt == mockParseContext.begin());

#ifdef BDE_BUILD_TARGET_EXC
            try {
                bslfmt::MockParseContext<char> mockParseContext1(format, 0);
                ASSERTV(0 == mockParseContext1.next_arg_id());
                ASSERTV("Exception has not been thrown", false);
            }
            catch(const bsl::format_error& err) {
            }

            try {
                bslfmt::MockParseContext<char> mockParseContext2(format, 1);
                ASSERTV(0 == mockParseContext2.next_arg_id());
                ASSERTV(mockParseContext2.indexingMode(),
                        CharMockParseContext::e_AUTOMATIC ==
                        mockParseContext2.indexingMode());
                mockParseContext2.check_arg_id(1);
                ASSERTV("Exception has not been thrown", false);
            }
            catch(const bsl::format_error& err) {
            }

            try {
                bslfmt::MockParseContext<char> mockParseContext3(format, 1);
                mockParseContext3.check_arg_id(1);
                ASSERTV(mockParseContext3.indexingMode(),
                        CharMockParseContext::e_MANUAL ==
                        mockParseContext3.indexingMode());
                ASSERTV(0 == mockParseContext3.next_arg_id());
                ASSERTV("Exception has not been thrown", false);
            }
            catch(const bsl::format_error& err) {
            }

            try {
                bslfmt::MockParseContext<char> mockParseContext1(format, 1);
                ASSERTV(0 == mockParseContext1.next_arg_id());
                mockParseContext1.check_arg_id(1);
                ASSERTV("Exception has not been thrown", false);
            }
            catch(const bsl::format_error& err) {
            }
#endif
        }
      } break;
      default: {
        printf("WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        printf("Error, non-zero test status = %d .\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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

