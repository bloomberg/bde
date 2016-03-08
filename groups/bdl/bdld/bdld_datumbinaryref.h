// bdld_datumbinaryref.h                                              -*-C++-*-
#ifndef INCLUDED_BDLD_DATUMBINARYREF
#define INCLUDED_BDLD_DATUMBINARYREF

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a type to represent binary data and its size.
//
//@CLASSES:
//  bdld::DatumBinaryRef: a type representing binary data and its size
//
//@SEE_ALSO: bdld_datum, bdld_datumerror, bdld_datumudt
//
//@DESCRIPTION:  This component implements a class, 'bdld::DatumBinaryRef',
// that represents a pointer to a non-modifiable binary data (as a 'void *')
// and an integral size value.  Note, that 'bdld::DatumBinaryRef' is nether a
// value-semantic type, nor is it an in-core value-semantic type (see
// 'bsldoc_glossary').  The 'bdld::DatumBinaryRef' notion of value is expressed
// by its equality-operator - two 'bdld:DatumBinaryRef' compare equal if the
// binary data they refer to is identical (both by size and by content).
// Accessors inside 'Datum' class that need to return a binary data, return an
// instance of 'bdld::DatumBinaryRef'.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic 'DatumBinaryRef' usage
///- - - - - - - - - - - - - - - - - - - -
// Suppose we have three functions. Data are obtained in the first one (with
// memory allocation), processed in the second one and released (with memory
// deallocation) in the third one.  The following code illustrates how to use
// 'bdld::DatumBinaryRef' to pass information about memory storage between
// them.
//
// First, we write all three functions:
//..
//  bdld::DatumBinaryRef obtainData(size_t size)
//      // Allocate array of the specified 'size' and initialize it with some
//      // values.
//  {
//      if (0 == size) {
//          return bdld::DatumBinaryRef();                            // RETURN
//      }
//      int *buffer = new int[size];
//      for (size_t i = 0; i < size; ++i) {
//          buffer[i] = static_cast<int>(i);
//      }
//      return bdld::DatumBinaryRef(static_cast<void *>(buffer), size);
//  }
//
//  int processData(const bdld::DatumBinaryRef& binaryData)
//      // Process data, held by the specified 'binaryData' object.
//  {
//      ostringstream out;
//      binaryData.print(out);
//
//      if (binaryData == bdld::DatumBinaryRef()) {
//          return 0;                                                 // RETURN
//      }
//
//      int        result = 0;
//      const int *array = static_cast<const int *>(binaryData.data());
//      for (size_t i = 0; i < binaryData.size(); ++i) {
//          result += array[i];
//      }
//      return result;
//  }
//
//  void releaseData(const bdld::DatumBinaryRef& binaryData)
//      // Release memory, held by the specified 'binaryData' object.
//  {
//      const int *array = static_cast<const int *>(binaryData.data());
//      delete [] array;
//  }
//..
// Next, we call first one to obtain data:
//..
//  bdld::DatumBinaryRef binaryData = obtainData(5);
//..
// Then we verify the results of second one's call:
//..
//  assert(10 == processData(binaryData));
//..
// Finally, we release allocated memory:
//..
//  releaseData(binaryData);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

namespace bdld {
                                 // ====================
                                 // class DatumBinaryRef
                                 // ====================
class DatumBinaryRef {
    // This class provides a type to represent a pointer to non-modifiable
    // binary data and an integral size value.

  public:
    // TYPES
    typedef bsls::Types::size_type SizeType;
        // 'SizeType' is an alias for an unsigned integral value, representing
        // the length of the binary array.

  private:
    // DATA
    const void *d_data_p;  // pointer to the binary data (not owned)
    SizeType    d_size;    // size of the binary data

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(DatumBinaryRef, bsl::is_trivially_copyable);

    // CREATORS
    DatumBinaryRef();
        // Create a 'DatumBinaryRef' object having zero data pointer and size.

    DatumBinaryRef(const void *data, SizeType size);
        // Create a 'DatumBinaryRef' object having the specified 'data' and
        // 'size'.  The behavior is undefined unless '0 != data || 0 == size'.
        // Note that the memory pointed by 'data' is never accessed by a
        // 'DatumBinaryRef' object.

    //! DatumBinaryRef(const DatumBinaryRef& origin) = default;
    //! ~DatumBinaryRef() = default;

    // MANIPULATORS
    //! DatumBinaryRef& operator=(const DatumBinaryRef& rhs) = default;

    // ACCESSORS
    const void *data() const;
        // Return the pointer to the non-modifiable binary data.

    SizeType size() const;
        // Return the size of the binary data.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a reference to the modifyable
        // 'stream'.  Optionally specify an initial indentation 'level', whose
        // absolute value is incremented recursively for nested objects.  If
        // 'level' is specified, optionally specify 'spacesPerLevel', whose
        // absolute value indicates the number of spaces per indentation level
        // for this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.  Note that this
        // human-readable format is not fully specified, and can change without
        // notice.
};

// FREE OPERATORS
bool operator==(const DatumBinaryRef& lhs, const DatumBinaryRef& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have the same value, and
    // 'false' otherwise.  Two 'DatumBinaryRef' objects have the same value if
    // they refer to arrays of bytes of the same size and having the same
    // content.

bool operator!=(const DatumBinaryRef& lhs, const DatumBinaryRef& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' have different values,
    // and 'true' otherwise.  Two 'DatumBinaryRef' objects have different
    // values if they refer to arrays of bytes of different sizes or having a
    // different content.

bool operator<(const DatumBinaryRef& lhs, const DatumBinaryRef& rhs);
    // Return 'true' if the specified 'lhs' compares smaller than the specified
    // 'rhs', and 'false' otherwise.  A byte comparision is performed using
    // memcpy.

bool operator>(const DatumBinaryRef& lhs, const DatumBinaryRef& rhs);
    // Return 'true' if the specified 'lhs' compares larger than the specified
    // 'rhs', and 'false' otherwise.  A byte comparision is performed using
    // memcpy.

bool operator<=(const DatumBinaryRef& lhs, const DatumBinaryRef& rhs);
    // Return 'true' if the specified 'lhs' compares smaller than or equal to
    // the specified 'rhs', and 'false' otherwise.  A byte comparision is
    // performed using memcpy.

bool operator>=(const DatumBinaryRef& lhs, const DatumBinaryRef& rhs);
    // Return 'true' if the specified 'lhs' compares larger than or equal to
    // the specified 'rhs', and 'false' otherwise.  A byte comparision is
    // performed using memcpy.

bsl::ostream& operator<<(bsl::ostream& stream, const DatumBinaryRef& rhs);
    // Write the value of the specified 'rhs' object to the specified output
    // 'stream' in a single-line format, and return a reference to the
    // modifyable 'stream'.  If 'stream' is not valid on entry, this operation
    // has no effect.  Note that this human-readable format is not fully
    // specified, can change without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

// ============================================================================
//                               INLINE DEFINITIONS
// ============================================================================

                                // --------------------
                                // class DatumBinaryRef
                                // --------------------

// CREATORS
inline
DatumBinaryRef::DatumBinaryRef()
: d_data_p(0)
, d_size(0)
{
}

inline
DatumBinaryRef::DatumBinaryRef(const void *data, SizeType  size)
: d_data_p(data)
, d_size(size)
{
    BSLS_ASSERT_SAFE(0 != data || 0 == size);
}

// ACCESSORS
inline
const void *DatumBinaryRef::data() const
{
    return d_data_p;
}

inline
DatumBinaryRef::SizeType DatumBinaryRef::size() const
{
    return d_size;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdld::operator==(const DatumBinaryRef& lhs, const DatumBinaryRef& rhs)
{
    return (lhs.size() == rhs.size())
            && !bsl::memcmp(lhs.data(), rhs.data(), rhs.size());
}


inline
bool bdld::operator!=(const DatumBinaryRef& lhs, const DatumBinaryRef& rhs)
{
    return !(lhs == rhs);
}

inline
bool bdld::operator<(const DatumBinaryRef& lhs, const DatumBinaryRef& rhs)
{
    int cmp = bsl::memcmp(lhs.data(),
                          rhs.data(),
                          bsl::min(lhs.size(), rhs.size()));
    return cmp == 0 ? lhs.size() < rhs.size() : cmp < 0;
}

inline
bool bdld::operator>(const DatumBinaryRef& lhs, const DatumBinaryRef& rhs)
{
    int cmp = bsl::memcmp(lhs.data(),
                          rhs.data(),
                          bsl::min(lhs.size(), rhs.size()));
    return cmp == 0 ? lhs.size() > rhs.size() : cmp > 0;
}

inline
bool bdld::operator<=(const DatumBinaryRef& lhs, const DatumBinaryRef& rhs)
{
    return !(lhs > rhs);
}

inline
bool bdld::operator>=(const DatumBinaryRef& lhs, const DatumBinaryRef& rhs)
{
    return !(lhs < rhs);
}

inline
bsl::ostream& bdld::operator<<(bsl::ostream& stream, const DatumBinaryRef& rhs)
{
    return rhs.print(stream, 0 , -1);
}

}  // close enterprise namespace

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

