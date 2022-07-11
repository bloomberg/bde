// bdlb_printadapter.h                                                -*-C++-*-
#ifndef INCLUDED_BDLB_PRINTADAPTER
#define INCLUDED_BDLB_PRINTADAPTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide object for streaming objects using 'bdlb::PrintMethods'.
//
//@CLASSES:
//  bdlb::PrintAdapter: class for streaming objects using 'bdlb::PrintMethods'.
//  bdlb::PrintAdapterUtil: utility for constructing 'bdlb::PrintAdapter'.
//
//@DESCRIPTION: This component provides a template 'class' that will enable any
// object that can be streamed using 'bdlb::PrintMethods' to be streamed via
// '<<'.
//
// Through using this 'class', one can stream an object using 'operator<<' and
// at the same time specify 'level' and 'spacesPerLevel' arguments to
// appropriately indent and format the output.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Use in C++03
///- - - - - - - - - - - -
// Suppose we have a few 'bdlb::IdentSpan' objects that we want to print, only
// we want to stream them with 'operator<<' and we want to specify non-default
// values of 'level' and 'spacesPerLevel' to their streaming.
//..
//  const bdlb::IndexSpan a(3, 7), b(241, 22), c(23, 17);
//..
// First, we create a typedef of 'PrintAdapter<IdentSpan>' to use:
//..
//  typedef bdlb::PrintAdapter<bdlb::IndexSpan> PAIS;
//..
// Then, we create a line of dashes:
//..
//  const char * const line = "------------------------------------"
//                            "------------------------------------\n";
//..
// Now, we use the 'typedef' to construct temporary 'PrintAdapter' objects to
// stream our 'IndexSpan' objects:
//..
//  cout << "    a: " << line << PAIS(&a, 2, 2)
//       << "    b: " << line << PAIS(&b, 3, 2)
//       << "    c: " << line << PAIS(&c, 4, 2);
//..
// Finally, we see the output:
//..
//  a: ------------------------------------------------------------------------
//  [
//    position = 3
//    length = 7
//  ]
//  b: ------------------------------------------------------------------------
//    [
//      position = 241
//      length = 22
//    ]
//  c: ------------------------------------------------------------------------
//      [
//        position = 23
//        length = 17
//      ]
//..
//
///Example 2: Use of 'PrintAdapterUtil::makeAdapter'
///- - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a few 'bdlb::IdentSpan' objects that we want to print, only
// we want to stream them with 'operator<<' and we want to specify
// non-default values of 'level' and 'spacesPerLevel' to their streaming.
//..
//  const bdlb::IndexSpan a(3, 7), b(241, 22), c(23, 17);
//..
// First, we make a typedef to the namespace 'struct':
//..  
//  typedef bdlb::PrintAdapterUtil Util;
//..
// Then, we create a line of dashes:
//..
//  const char * const line = "------------------------------------"
//                            "------------------------------------\n";
//..
// Now, we call the static function 'PrintAdapterUtil::makeAdapter' on the
// 'IndexSpan' objects to stream which will return streamable 'PrintAdapter'
// objects:
//..
//  cout << "    a: " << line << Util::makeAdapter(a, 2, 2)
//       << "    b: " << line << Util::makeAdapter(b, 3, 2)
//       << "    c: " << line << Util::makeAdapter(c, 4, 2);
//..
// Finally, we see the output:
//..
//  a: ------------------------------------------------------------------------
//  [
//    position = 3
//    length = 7
//  ]
//  b: ------------------------------------------------------------------------
//    [
//      position = 241
//      length = 22
//    ]
//  c: ------------------------------------------------------------------------
//      [
//        position = 23
//        length = 17
//      ]
//..
//
///Example 3: Use in C++17 With CTAD
///- - - - - - - - - - - - - - - - -
// Suppose you have a few 'bdlb::IdentSpan' objects that you want to print,
// only you want to stream them with 'operator<<' and you want to specify
// non-default values of 'level' and 'spacesPerLevel' to their streaming.
//..
//  const bdlb::IndexSpan a(3, 7), b(241, 22), c(23, 17);
//..
// First, we create a line of dashes:
//..
//  const char * const line = "------------------------------------"
//                            "------------------------------------\n";
//..
// Now, you call the constructor 'PrintAdapter' on the 'IndexSpan' objects to
// stream and CTAD will create the right template specification:
//..
//  cout << "    a: " << line << bdlb::PrintAdapter(&a, 2, 2)
//       << "    b: " << line << bdlb::PrintAdapter(&b, 3, 2)
//       << "    c: " << line << bdlb::PrintAdapter(&c, 4, 2);
//..
// Finally, we see the output:
//..
//  a: ------------------------------------------------------------------------
//  [
//    position = 3
//    length = 7
//  ]
//  b: ------------------------------------------------------------------------
//    [
//      position = 241
//      length = 22
//    ]
//  c: ------------------------------------------------------------------------
//      [
//        position = 23
//        length = 17
//      ]
//..

#include <bdlscm_version.h>

#include <bdlb_printmethods.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdlb {

                              // ==================
                              // class PrintAdapter
                              // ==================

template <class TYPE>
class PrintAdapter {
    // This 'class' provides an object that may be streamed with 'operator<<'
    // to give full control of the indentation format of the streaming.

    // DATA
    const TYPE *d_object_p;
    int         d_level;
    int         d_spacesPerLevel;

  public:
    // CREATORS
    explicit
    PrintAdapter(const TYPE *object,
                 int         level          = 0,
                 int         spacesPerLevel = 4);
        // Create a print adapter object bound to the specified 'object', and
        // with the optionally specified 'level' and 'spacesPerLevel'
        // attributes, to be used when the 'PrintAdapter' object is streamed.
        // Unless 'level' is specified, a value of 0 is used, unless
        // 'spacesPerLevel' is specified, a value of 4 is used.

    // PrintAdapter(const PrintAdapter&) = default;

    // MANIPULATORS
    // PrintAdapter& operator=(const PrintAdapter&) = default;

    // ACCESSORS
    int level() const;
        // Return the 'level' attribute.

    const TYPE& object() const;
        // Return a reference to the object referred to by this print adapter.

    int spacesPerLevel() const;
        // Return the 'spacesPerLevel' attribute.
};

                            // ======================
                            // class PrintAdapterUtil
                            // ======================

struct PrintAdapterUtil {
    // This 'struct' provides a namespace for a function template that
    // facilitates the construction of a 'PrintAdapter' without having to
    // explicitly specify template arguments in C++03.  Note that in C++17 and
    // later, CTAD may be used instead of this 'struct' to call the
    // 'PrintAdapter' constructor without having to explicitly specify the
    // template argument.

    // CLASS METHOD
    template <class TYPE>
    static
    PrintAdapter<TYPE> makeAdapter(const TYPE& object,
                                   int         level          = 0,
                                   int         spacesPerLevel = 4);
        // Create and return a 'PrintAdapter' object, passing the constructor
        // the specified 'object', 'level', and 'spacesPerLevel'.
};

// FREE OPERATORS
template <class TYPE>
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const PrintAdapter<TYPE>& adapter);
    // Write the value of the specified 'object' to the specified output
    // 'stream' and return a non-'const' reference to 'stream'.  If 'stream' is
    // not valid on entry, this operation has no effect.  Note that this
    // human-readable format is not fully specified and can change without
    // notice.

// ============================================================================
//                         INLINE FUNCTION DEFINITIONS
// ============================================================================

                              // ------------
                              // PrintAdapter
                              // ------------

// CREATORS
template <class TYPE>
inline
PrintAdapter<TYPE>::PrintAdapter(const TYPE *object,
                                 int         level,
                                 int         spacesPerLevel)
: d_object_p(object)
, d_level(level)
, d_spacesPerLevel(spacesPerLevel)
{}

// ACCESSORS
template <class TYPE>
inline
int PrintAdapter<TYPE>::level() const
{
    return d_level;
}

template <class TYPE>
inline
const TYPE& PrintAdapter<TYPE>::object() const
{
    return *d_object_p;
}

template <class TYPE>
inline
int PrintAdapter<TYPE>::spacesPerLevel() const
{
    return d_spacesPerLevel;
}

// FREE OPERATORS
template <class TYPE>
inline
bsl::ostream& operator<<(bsl::ostream&             stream,
                         const PrintAdapter<TYPE>& adapter)
    // Format this object to the specified output 'stream' at the (absolute
    // value of) the optionally specified indentation 'level' and return a
    // reference to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.
{
    return bdlb::PrintMethods::print(stream,
                                     adapter.object(),
                                     adapter.level(),
                                     adapter.spacesPerLevel());
}

                            // ----------------
                            // PrintAdapterUtil
                            // ----------------

// CLASS METHOD
template <class TYPE>
inline
PrintAdapter<TYPE> PrintAdapterUtil::makeAdapter(const TYPE& object,
                                                 int         level,
                                                 int         spacesPerLevel)
{
    return PrintAdapter<TYPE>(&object, level, spacesPerLevel);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
