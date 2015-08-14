// balxml_listparser.h                                                -*-C++-*-
#ifndef INCLUDED_BALXML_LISTPARSER
#define INCLUDED_BALXML_LISTPARSER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide push parser for lists.
//
//@CLASSES:
//   balxml::ListParser: push parser for lists
//
//@SEE_ALSO: bdlat_arrayfunctions
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: The 'balxml::ListParser<TYPE>' class template provided by this
// component can be used to parse lists into an object that supports
// 'bdeat_ArrayFunctions'.
//
// This class template is a model of the 'PushParser' concept, which contains
// the following methods:
//..
//  int beginParse(TYPE *object);
//      // Prepare the parser to start parsing a new value and associate the
//      // specified 'object' with the parser.  Return 0 if successful and
//      // non-zero otherwise.
//
//  int endParse();
//      // Ends the parse operation and store the value parsed from the pushed
//      // characters into the associated object.  Return 0 if successful and
//      // non-zero otherwise.  The behavior is undefined unless an object is
//      // associated with this parser.  Upon successful completion, the parser
//      // will be disassociated with the object.
//
//  template <typename INPUT_ITERATOR>
//  int pushCharacters(INPUT_ITERATOR begin, INPUT_ITERATOR end);
//      // Push the characters ranging from the specified 'begin' up to (but
//      // not including) the specified 'end' into this parser.  Return 0 if
//      // successful and non-zero otherwise.  The parameterized
//      // 'INPUT_ITERATOR' must be dereferenceable to a 'char' value.  The
//      // behavior is undefined unless an object is associated with this
//      // parser.
//..
//
///Usage
///-----
// The following snippets of code illustrate the usage of this component.
// Suppose you had an input stream that contained a list of doubles.  The
// following 'loadDoublesFromListStream' function loads this data into an
// 'bsl::vector<double>':
//..
//  #include <balxml_listparser.h>
//
//  #include <bdlt_date.h>
//
//  #include <istream>
//  #include <iterator>
//  #include <vector>
//  #include <sstream>
//  #include <string>
//
//  using namespace BloombergLP;
//
//  int parseDouble(double *result, const char *data, int dataLength);
//
//  int loadDoublesFromListStream(bsl::vector<double> *result,
//                                bsl::istream&        stream)
//  {
//      enum { BAEXML_FAILURE = -1 };
//
//      balxml::ListParser<bsl::vector<double> > parser(&parseDouble);
//
//      if (0 != parser.beginParse(result)) {
//          return BAEXML_FAILURE;
//      }
//
//      if (0 != parser.pushCharacters(bsl::istreambuf_iterator<char>(stream),
//                                     bsl::istreambuf_iterator<char>())) {
//          return BAEXML_FAILURE;
//      }
//
//      return parser.endParse();
//  }
//..
// The 'parseDouble' function is implemented as follows:
//..
//  int parseDouble(double *result, const char *data, int dataLength)
//  {
//      bsl::stringstream ss(bsl::string(data, dataLength));
//      ss >> (*result);
//      return 0;
//  }
//..
// The following function demonstrates the 'loadDoublesFromListStream'
// function:
//..
//  void usageExample()
//  {
//      const char INPUT[] = "1.5 2.0 3.8 1.0";
//
//      bsl::vector<double> vec;
//      bsl::istringstream  iss(INPUT);
//
//      int result = loadDoublesFromListStream(&vec, iss);
//
//      assert(0   == result);
//      assert(4   == vec.size());
//      assert(1.5 == vec[0]);
//      assert(2.0 == vec[1]);
//      assert(3.8 == vec[2]);
//      assert(1.0 == vec[3]);
//  }
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BDLAT_ARRAYFUNCTIONS
#include <bdlat_arrayfunctions.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDLF_BIND
#include <bdlf_bind.h>
#endif

#ifndef INCLUDED_BDLF_FUNCTION
#include <bdlf_function.h>
#endif

#ifndef INCLUDED_BDLF_PLACEHOLDER
#include <bdlf_placeholder.h>
#endif

#ifndef INCLUDED_BDLB_CHARTYPE
#include <bdlb_chartype.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

namespace BloombergLP {

namespace balxml {
                           // =============================
                           // class ListParser<TYPE>
                           // =============================

template <class TYPE>
class ListParser {
    // This is a push parser for lists.

    // PRIVATE TYPES
    typedef typename
    bdeat_ArrayFunctions::ElementType<TYPE>::Type ElementType;

  public:
    // TYPES
    typedef int (*ParseElementFunction)(ElementType*, const char*, int);
    typedef bdlf::Function<ParseElementFunction> ParseElementCallback;

  private:
    // PRIVATE DATA MEMBERS
    bsl::string           d_characters;            // accumulated characters
    TYPE                 *d_object_p;              // associated object
    ParseElementCallback  d_parseElementCallback;  // callback for parsing
                                                   // elements

    // NOT IMPLEMENTED
    ListParser(const ListParser&);
    ListParser& operator=(const ListParser&);

    // PRIVATE MANIPULATORS
    int appendElement(const char *data, int dataLength);
        // Append an element to the associated object having the specified
        // 'data' of the specified 'dataLength'.

  public:
    // CREATORS
    explicit ListParser(ParseElementCallback  parseElementCallback,
                               bslma::Allocator     *basicAllocator = 0);
        // Create a parser for lists using the specified 'parseElementCallback'
        // functor to parse each element and the specified 'basicAllocator' for
        // supplying memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator will be used.

#ifdef DOXYGEN // Generated by compiler:

    ~ListParser();
        // Destroy this object.

#endif

    int beginParse(TYPE *object);
        // Prepare the parser to start parsing a new value and associate the
        // specified 'object' with the parser.  Return 0 if successful and
        // non-zero otherwise.

    int endParse();
        // Ends the parse operation and store the value parsed from the pushed
        // characters into the associated object.  Return 0 if successful and
        // non-zero otherwise.  The behavior is undefined unless an object is
        // associated with this parser.  Upon successful completion, the parser
        // will be disassociated with the object.

    template <class INPUT_ITERATOR>
    int pushCharacters(INPUT_ITERATOR begin, INPUT_ITERATOR end);
        // Push the characters ranging from the specified 'begin' up to (but
        // not including) the specified 'end' into this parser.  Return 0 if
        // successful and non-zero otherwise.  The parameterized
        // 'INPUT_ITERATOR' must be dereferenceable to a 'char' value.  The
        // behavior is undefined unless an object is associated with this
        // parser.
};

// ============================================================================
//                        INLINE FUNCTION DEFINITIONS
// ============================================================================

                           // -----------------------------
                           // class ListParser<TYPE>
                           // -----------------------------

// PRIVATE MANIPULATORS

template <class TYPE>
int ListParser<TYPE>::appendElement(const char *data, int dataLength)
{
    BSLS_ASSERT_SAFE(data);
    BSLS_ASSERT_SAFE(0 < dataLength);

    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    const int i = static_cast<int>(bdeat_ArrayFunctions::size(*d_object_p));

    bdeat_ArrayFunctions::resize(d_object_p, i + 1);

    typedef bdlf::Function<int(*)(ElementType*)> Functor;

    using bdlf::PlaceHolders::_1;

    Functor parseElementFunctor = bdlf::BindUtil::bind(d_parseElementCallback,
                                                      _1,
                                                      data,
                                                      dataLength);

    if (0 != bdeat_ArrayFunctions::manipulateElement(d_object_p,
                                                     parseElementFunctor,
                                                     i)) {
        // remove the new object from the array
        bdeat_ArrayFunctions::resize(d_object_p, i);

        return BAEXML_FAILURE;                                        // RETURN
    }

     return BAEXML_SUCCESS;
}

// CREATORS

template <class TYPE>
ListParser<TYPE>::ListParser(
    ParseElementCallback  parseElementCallback,
    bslma::Allocator     *basicAllocator)
: d_characters(basicAllocator)
, d_object_p(0)
, d_parseElementCallback(parseElementCallback, basicAllocator)
{
}

// MANIPULATORS

template <class TYPE>
int ListParser<TYPE>::beginParse(TYPE *object)
{
    BSLS_ASSERT_SAFE(object);

    enum { BAEXML_SUCCESS = 0 };

    d_characters.clear();
    d_object_p = object;

    bdeat_ArrayFunctions::resize(d_object_p, 0);

    return BAEXML_SUCCESS;
}

template <class TYPE>
int ListParser<TYPE>::endParse()
{
    BSLS_ASSERT_SAFE(d_object_p);

    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    if (!d_characters.empty()) {
        if (0 != appendElement(d_characters.data(),
                               static_cast<int>(d_characters.length()))) {
            return BAEXML_FAILURE;                                    // RETURN
        }
    }

    d_object_p = 0;

    return BAEXML_SUCCESS;
}

template <class TYPE>
template <class INPUT_ITERATOR>
int ListParser<TYPE>::pushCharacters(INPUT_ITERATOR begin,
                                            INPUT_ITERATOR end)
{
    BSLS_ASSERT_SAFE(d_object_p);

    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    while (begin != end) {
        const char character = *begin;

        ++begin;

        if (bdlb::CharType::isSpace(character)) {
            if (!d_characters.empty()) {
                if (0 != appendElement(
                                    d_characters.data(),
                                    static_cast<int>(d_characters.length()))) {
                    return BAEXML_FAILURE;                            // RETURN
                }

                d_characters.clear();
            }
        }
        else {
            d_characters.push_back(character);
        }
    }

    return BAEXML_SUCCESS;
}
}  // close package namespace

}  // close enterprise namespace

#endif // ! defined(INCLUDED_BAEXML_LISTPARSER)

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
