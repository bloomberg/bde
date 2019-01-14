// bdlb_indexspan.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLB_INDEXSPAN
#define INCLUDED_BDLB_INDEXSPAN

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a value-semantic attribute type for position and length.
//
//@CLASSES:
//  bdlb::IndexSpan: value-semantic attribute type to represent position/length
//
//@SEE_ALSO: bdlb_indexspanutil, bdlb_indexspanstringutil
//
//@DESCRIPTION: This component provides a value-semantic, constrained
// attribute type 'IndexSpan' that stores a position and a length.  The
// constraint is that the sum of the two attributes must be representable by
// the type 'bsl::size_t'.
//
///Attributes
///----------
//..
//  Name                Type                  Default  Simple Constraints
//  ------------------  --------------------  -------  ------------------
//  position            IndexSpan::size_type  0        none
//  length              IndexSpan::size_type  0        none
//..
//: o 'position': this span starts at this position in a sequence.
//:
//: o 'length': this span contains this many elements.
//
// For example, an 'IndexSpan' describing the middle name in the string
// "James Tiberius Kirk" will have the position 6 (the 'T' of "Tiberius" is the
// 7th character of the string) and the length 8 ("Tiberius is 8 characters
// long).
//
// Note that the sum of the two attributes must be representable by the
// 'bsl::size_t' type.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: A Parsed Path
/// - - - - - - - - - - - -
// Suppose that we work with file system paths and we frequently need the path,
// the base name, the extension, and the filename, as well as the full path
// itself.  Therefore we want to create a class that stores the full path and a
// way to quickly access the individual parts.  (To keep things simple we will
// make the example code work on POSIX-style paths so we do not need to deal
// with the difference in path separators.)
//
// If we try the naive solution -- storing a 'bsl::string' of the full path and
// 'bslstl::StringRef' for path, basename and extension -- the class
// 'bslstl::StringRef' members will reference back into memory owned by the
// object, and the result is the compiler-generated (and naively written) move
// and copy operations will be broken.  In addition, explicitly implementing
// move and copy operations is quite difficult and error prone.
//
// The simplest (most readable) solution to the problem is to store a position
// and a length for the path, basename, and extension (rather than a
// 'bslstl::StringRef') because that representation is independent of the
// location of the memory for the string.  'IndexSpan' provides such a
// representation, and allow us to implement our class using a simple copy and
// move operations (or in other contexts, compiler supplied operations).
//
// First, we define the members and interface of the class.
//..
//  class ParsedPath {
//    private:
//      // DATA
//      bsl::string     d_full;  // The full path
//      bdlb::IndexSpan d_path;  // The path part if present, otherwise empty
//      bdlb::IndexSpan d_base;  // The base name if present, otherwise empty
//      bdlb::IndexSpan d_ext;   // The extension if present, otherwise empty
//
//    public:
//      // CREATE
//      explicit ParsedPath(const bslstl::StringRef&  full,
//                          bslma::Allocator         *basicAllocator = 0);
//          // Create a new 'ParsedPath' object by storing and parsing the
//          // specified 'full' path.  Use the optionally specified
//          // 'basicAllocator' to allocate memory.  If 'basicAllocator' is 0,
//          // use the currently installed default allocator.
//
//      ParsedPath(const ParsedPath& original,
//                 bslma::Allocator *basicAllocator = 0);
//          // Create a new 'ParsedPath' object that stores the same parsed
//          // path as the specified 'original'.  Use the optionally specified
//          // 'basicAllocator' to allocate memory.  Use the currently
//          // installed default allocator if 'basicAllocator' is zero.
//
//      // ACCESSORS
//      const bsl::string& full() const;
//          // Return a const reference to the full path parsed by this object.
//
//      bslstl::StringRef path() const;
//          // Return a string reference to the path part.  Note that it may be
//          // an empty string reference.
//
//      bslstl::StringRef base() const;
//          // Return a string reference to the base name part.  Note that it
//          // may be an empty string reference.
//
//      bslstl::StringRef ext()  const;
//          // Return a string reference to the extension part.  Note that it
//          // may be an empty string reference.
//  };
//..
//  Next, to make the parsing code short and readable, we implement a helper
//  function to create 'IndexSpan' objects from two positions.  (In practice we
//  would use the higher level utility function
//  'IndexSpanStringUtil::createFromPosition'.)
//..
//  bdlb::IndexSpan createFromPositions(bdlb::IndexSpan::size_type startPos,
//                                      bdlb::IndexSpan::size_type endPos)
//      // Return an 'IndexSpan' describing the specified '[startPos, endPos)'
//      // positions.
//  {
//      return bdlb::IndexSpan(startPos, endPos - startPos);
//  }
//..
//  Then, we implement the parsing constructor using the 'create' function.
//..
//  // CREATORS
//  ParsedPath::ParsedPath(const bslstl::StringRef&  full,
//                         bslma::Allocator         *basicAllocator)
//  : d_full(full, basicAllocator)
//  {
//      typedef bsl::string::size_type Size;
//
//      static const Size npos = bsl::string::npos;
//
//      const Size slashPos = d_full.rfind('/');
//      const Size dotPos   = d_full.rfind('.');
//
//      const bool dotIsPresent   = (dotPos   != npos);
//      const bool slashIsPresent = (slashPos != npos);
//
//      const bool dotIsInPath = slashIsPresent && (dotPos < slashPos);
//
//      const bool isDotFile = dotIsPresent &&
//                               dotPos == (slashIsPresent ? slashPos + 1 : 0);
//
//      const bool hasExtension = dotIsPresent && !dotIsInPath && !isDotFile;
//      const bool hasPath      = slashIsPresent;
//
//      if (hasPath) {
//          d_path = createFromPositions(0, slashPos + 1);
//      }
//
//      d_base = createFromPositions(slashPos + 1,
//                                   hasExtension ? dotPos : full.length());
//
//      if (hasExtension) {
//          d_ext = createFromPositions(dotPos + 1, full.length());
//      }
//  }
//..
// Next, we implement the (now) simple copy constructor:
//..
//  ParsedPath::ParsedPath(const ParsedPath& original,
//                         bslma::Allocator *basicAllocator)
//  : d_full(original.d_full, basicAllocator)
//  , d_path(original.d_path)
//  , d_base(original.d_base)
//  , d_ext(original.d_ext)
//  {
//  }
//..
//  Then, to make the accessors simple (and readable), we implement a helper
//  function that creates a 'StringRef' from a 'StringRef' and an 'IndexSpan'.
//  (In practice we would use the higher level utility function
//  'IndexSpanStringUtil::bind'.)
//..
//  bslstl::StringRef bindSpan(const bslstl::StringRef& full,
//                             const bdlb::IndexSpan&   part)
//      // Return a string reference to the substring of the specified 'full'
//      // string defined by the specified 'part'.
//  {
//      BSLS_ASSERT(part.position() <= full.length());
//      BSLS_ASSERT(part.position() + part.length() <= full.length());
//
//      return bslstl::StringRef(full.data() + part.position(), part.length());
//  }
//..
//  Next, we implement the accessors:
//..
//  // ACCESSORS
//  bslstl::StringRef ParsedPath::base() const
//  {
//      return bindSpan(d_full, d_base);
//  }
//
//  bslstl::StringRef ParsedPath::ext() const
//  {
//      return bindSpan(d_full, d_ext);
//  }
//
//  const bsl::string& ParsedPath::full() const
//  {
//      return d_full;
//  }
//
//  bslstl::StringRef ParsedPath::path() const
//  {
//      return bindSpan(d_full, d_path);
//  }
//..
// Finally, we verify that the resulting class is copied properly.  We do that
// by determining that the original and the copy object has equal but distinct
// strings and that the (other) accessors return references into those strings.
//..
//  ParsedPath aPath("path/path/basename.extension");
//  ParsedPath theCopy(aPath);
//
//  assert(aPath.full()        == theCopy.full());
//  assert(aPath.full().data() != theCopy.full().data());
//
//  assert(aPath.path()        == theCopy.path());
//  assert(aPath.path().data() != theCopy.path().data());
//  assert(aPath.path().data() >= aPath.full().data());
//  assert(aPath.path().data() + aPath.path().length() <=
//                                aPath.full().data() + aPath.full().length());
//
//  assert(aPath.base()        == theCopy.base());
//  assert(aPath.base().data() != theCopy.base().data());
//  assert(aPath.base().data() >= aPath.full().data());
//  assert(aPath.base().data() + aPath.base().length() <=
//                                aPath.full().data() + aPath.full().length());
//
//  assert(aPath.ext()        == theCopy.ext());
//  assert(aPath.ext().data() != theCopy.ext().data());
//  assert(aPath.ext().data() >= aPath.full().data());
//  assert(aPath.ext().data() + aPath.ext().length() <=
//                                aPath.full().data() + aPath.full().length());
//..

#include <bdlscm_version.h>

#include <bdlb_printmethods.h>

#include <bslh_hash.h>

#include <bslmf_istriviallycopyable.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>

#include <bsl_cstddef.h>
#include <bsl_iosfwd.h>
#include <bsl_limits.h>

namespace BloombergLP {
namespace bdlb {

                            // ===============
                            // class IndexSpan
                            // ===============

class IndexSpan {
    // A constrained attribute type that represents a position and a length.
    // The constraint is that the sum of the position and length attributes
    // must be representable by the 'bsl::size_t' type.

  public:
    // TYPES
    typedef bsl::size_t size_type;
        // The type of the position and length attributes.

  private:
    // DATA
    size_type d_position;  // The spans starts at this position
    size_type d_length;    // The span has this length

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(IndexSpan, bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(IndexSpan, bdlb::HasPrintMethod);

    // CREATORS
    IndexSpan();
        // Create an 'IndexSpan' object with position and length of 0.

    IndexSpan(size_type position, size_type length);
        // Create an 'IndexSpan' object with the specified 'position' and
        // 'length'.  The behavior is undefined unless
        // 'position <= bsl::numeric_limits<size_t>::max() - length'.

    // IndexSpan(const IndexSpan& original) = default;
        // Create an index span having the value of the specified 'original'
        // index span.  Note that this trivial copy constructor is generated by
        // the compiler.

    // ~IndexSpan() = default;
        // Destroy this index span object.  Note that this trivial destructor
        // is generated by the compiler.

    // MANIPULATORS
    // IndexSpan& operator=(const IndexSpan& rhs) = default;
        // Assign to this index span the value of the specified 'rhs' index
        // span, and return a reference providing modifiable access to this
        // object.  Note that this trivial assignment operation is generated by
        // the compiler.

    // ACCESSORS
    bool isEmpty() const;
        // Return 'true' if the length attribute is 0; and return 'false'
        // otherwise.

    size_type length() const;
        // Return the length attribute.

    size_type position() const;
        // Return the position attribute.

                                  // Aspects

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.
};

// FREE OPERATORS
bool operator==(const IndexSpan& lhs, const IndexSpan& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' index span objects have
    // the same value, and 'false' otherwise.  Two index span objects have the
    // same value if their position and length attributes compare equal.

bool operator!=(const IndexSpan& lhs, const IndexSpan& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' index span objects do not
    // have the same value, and 'false' otherwise.  Two index span objects do
    // not have the same value if either their position or length attributes
    // do not compare equal.

bsl::ostream& operator<<(bsl::ostream& stream, const IndexSpan& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

template <class HASH_ALGORITHM>
void hashAppend(HASH_ALGORITHM& hashAlgorithm, const IndexSpan& object);
    // Invoke the specified 'hashAlgorithm' on the attributes of the specified
    // 'object'.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                            // ---------------
                            // class IndexSpan
                            // ---------------

// CREATORS
inline
IndexSpan::IndexSpan()
: d_position(0u)
, d_length(0u)
{
}

inline
IndexSpan::IndexSpan(size_type position, size_type length)
: d_position(position)
, d_length(length)
{
    BSLS_ASSERT(position <= bsl::numeric_limits<size_t>::max() - length);
}

// ACCESSORS
inline
bool IndexSpan::isEmpty() const
{
    return 0 == d_length;
}

inline
IndexSpan::size_type IndexSpan::length() const
{
    return d_length;
}

inline
IndexSpan::size_type IndexSpan::position() const
{
    return d_position;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdlb::operator==(const IndexSpan& lhs, const IndexSpan& rhs)
{
    return lhs.position() == rhs.position()
        && lhs.length()   == rhs.length();
}

inline
bool bdlb::operator!=(const IndexSpan& lhs, const IndexSpan& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& bdlb::operator<<(bsl::ostream&    stream,
                               const IndexSpan& object)
{
    return object.print(stream, 0, -1);
}

template <class HASH_ALGORITHM>
inline
void bdlb::hashAppend(HASH_ALGORITHM& hashAlgorithm, const IndexSpan& object)
{
    const IndexSpan::size_type pos = object.position();
    const IndexSpan::size_type len = object.length();

    using bslh::hashAppend;
    hashAppend(hashAlgorithm, pos);
    hashAppend(hashAlgorithm, len);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
