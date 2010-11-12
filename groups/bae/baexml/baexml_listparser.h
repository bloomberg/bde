// baexml_listparser.h                  -*-C++-*-
#ifndef INCLUDED_BAEXML_LISTPARSER
#define INCLUDED_BAEXML_LISTPARSER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide push parser for lists.
//
//@CLASSES: baexml_ListParser: push parser for lists.
//
//@SEE_ALSO: bdeat_arrayfunctions
//
//@AUTHOR: Shezan Baig (sbaig)
//
//@CONTACT: Rohan Bhindwale (rbhindwa)
//
//@DESCRIPTION: The 'baexml_ListParser<TYPE>' class template provided by this
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
//  #include <baexml_listparser.h>
//
//  #include <bdet_date.h>
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
//      baexml_ListParser<bsl::vector<double> > parser(&parseDouble);
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

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BDEAT_ARRAYFUNCTIONS
#include <bdeat_arrayfunctions.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BDEF_BIND
#include <bdef_bind.h>
#endif

#ifndef INCLUDED_BDEF_FUNCTION
#include <bdef_function.h>
#endif

#ifndef INCLUDED_BDEF_PLACEHOLDER
#include <bdef_placeholder.h>
#endif

#ifndef INCLUDED_BDEU_CHARTYPE
#include <bdeu_chartype.h>
#endif

#ifndef INCLUDED_BCEM_AGGREGATE
#include <bcem_aggregate.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {

                           // =============================
                           // class baexml_ListParser<TYPE>
                           // =============================

template <typename TYPE>
class baexml_ListParser {
    // This is a push parser for lists.

    // PRIVATE TYPES
    typedef typename
    bdeat_ArrayFunctions::ElementType<TYPE>::Type ElementType;

  public:
    // TYPES
    typedef int (*ParseElementFunction)(ElementType*, const char*, int);
    typedef bdef_Function<ParseElementFunction> ParseElementCallback;

  private:
    // PRIVATE DATA MEMBERS
    bsl::string           d_characters;            // accumulated characters
    TYPE                 *d_object_p;              // associated object
    ParseElementCallback  d_parseElementCallback;  // callback for parsing
                                                   // elements

    // NOT IMPLEMENTED
    baexml_ListParser(const baexml_ListParser&);
    baexml_ListParser& operator=(const baexml_ListParser&);

    // PRIVATE MANIPULATORS
    int appendElement(const char *data, int dataLength);
        // Append an element to the associated object having the specified
        // 'data' of the specified 'dataLength'.

  public:
    // CREATORS
    explicit baexml_ListParser(ParseElementCallback  parseElementCallback,
                               bslma_Allocator      *basicAllocator = 0);
        // Create a parser for lists using the specified 'parseElementCallback'
        // functor to parse each element and the specified 'basicAllocator' for
        // supplying memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator will be used.

#ifdef DOXYGEN // Generated by compiler:

    ~baexml_ListParser();
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

    template <typename INPUT_ITERATOR>
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
                           // class baexml_ListParser<TYPE>
                           // -----------------------------

// PRIVATE MANIPULATORS

template <typename TYPE>
int baexml_ListParser<TYPE>::appendElement(const char *data, int dataLength)
{
    BSLS_ASSERT_SAFE(data);
    BSLS_ASSERT_SAFE(0 < dataLength);

    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    int i = bdeat_ArrayFunctions::size(*d_object_p);

    bdeat_ArrayFunctions::resize(d_object_p, i + 1);

    typedef bdef_Function<int(*)(ElementType*)> Functor;

    using bdef_PlaceHolders::_1;

    Functor parseElementFunctor = bdef_BindUtil::bind(d_parseElementCallback,
                                                      _1,
                                                      data,
                                                      dataLength);

    if (0 != bdeat_ArrayFunctions::manipulateElement(d_object_p,
                                                     parseElementFunctor,
                                                     i)) {
        // remove the new object from the array
        bdeat_ArrayFunctions::resize(d_object_p, i);

        return BAEXML_FAILURE;
    }

     return BAEXML_SUCCESS;
}

template <>
inline
int baexml_ListParser<bcem_Aggregate>::appendElement(const char *data,
                                                     int         dataLength)
{
    BSLS_ASSERT_SAFE(data);
    BSLS_ASSERT_SAFE(0 < dataLength);

    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    int i = bdeat_ArrayFunctions::size(*d_object_p);

    bdeat_ArrayFunctions::resize(d_object_p, i + 1);

    bcem_Aggregate agg = (*d_object_p)[i];

    if (0 != d_parseElementCallback(&agg, data, dataLength)) {
        // remove the new object from the array
        bdeat_ArrayFunctions::resize(d_object_p, i);

        return BAEXML_FAILURE;
    }

     return BAEXML_SUCCESS;
}

// CREATORS

template <typename TYPE>
baexml_ListParser<TYPE>::baexml_ListParser(
    ParseElementCallback  parseElementCallback,
    bslma_Allocator      *basicAllocator)
: d_characters(basicAllocator)
, d_object_p(0)
, d_parseElementCallback(parseElementCallback, basicAllocator)
{
}

// MANIPULATORS

template <typename TYPE>
int baexml_ListParser<TYPE>::beginParse(TYPE *object)
{
    BSLS_ASSERT_SAFE(object);

    enum { BAEXML_SUCCESS = 0 };

    d_characters.clear();
    d_object_p = object;

    bdeat_ArrayFunctions::resize(d_object_p, 0);

    return BAEXML_SUCCESS;
}

template <typename TYPE>
int baexml_ListParser<TYPE>::endParse()
{
    BSLS_ASSERT_SAFE(d_object_p);

    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    if (!d_characters.empty()) {
        if (0 != appendElement(d_characters.data(), d_characters.length())) {
            return BAEXML_FAILURE;
        }
    }

    d_object_p = 0;

    return BAEXML_SUCCESS;
}

template <typename TYPE>
template <typename INPUT_ITERATOR>
int baexml_ListParser<TYPE>::pushCharacters(INPUT_ITERATOR begin,
                                            INPUT_ITERATOR end)
{
    BSLS_ASSERT_SAFE(d_object_p);

    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    while (begin != end) {
        const char character = *begin;

        ++begin;

        if (bdeu_CharType::isSpace(character)) {
            if (!d_characters.empty()) {
                if (0 != appendElement(d_characters.data(),
                                       d_characters.length())) {
                    return BAEXML_FAILURE;
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

}  // close namespace BloombergLP

#endif // ! defined(INCLUDED_BAEXML_LISTPARSER)

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
