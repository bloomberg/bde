// bdld_manageddatum.h                                                -*-C++-*-
#ifndef INCLUDED_BDLD_MANAGEDDATUM
#define INCLUDED_BDLD_MANAGEDDATUM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a smart-pointer like manager for a 'Datum' object.
//
//@CLASSES:
//  bdld::ManagedDatum: a smart-pointer like manager for a 'Datum' object
//
//@SEE_ALSO: bdld_datum
//
//@DESCRIPTION: This component implements a type, 'bdld::ManagedDatum', that
// provides two important services for 'Datum' objects:
//
//: 1 'ManagedDatum' provides value-semantic-like operations for 'Datum'.
//:
//: 2 'ManagedDatum' is a resource manager, similar to a smart-pointer, for
//:   'Datum'.
//
// These services allow clients to use a 'ManagedDatum' object in most contexts
// where an object of a value-semantic type can be used (passed by value,
// stored in containers, and so on), even though 'ManagedDatum' is not strictly
// value-semantic.  These services are explored in subsequent sections.
//
// The 'Datum' type maintained by a 'ManagedDatum' provides a space-efficient
// discriminated union (i.e., a variant) holding the value of a scalar type
// (e.g., 'int', 'double', 'string') or an aggregate of other 'Datum' objects.
// See {'bdld_datum'} for more details.
//
///Value-Semantics
///---------------
// 'ManagedDatum', while not strictly a value-semantic type, provides the full
// set of value-semantic-like operations for 'Datum' (see
// {'bsldoc_glossary'|Value-Semantic Operations}):
//
//   o Equality and Non-Equality Comparisons
//   o Copy Construction
//   o Copy Assignment
//   o Default Construction
//   o 'ostream' Printing
//
// In other words the syntax of 'ManagedDatum' *regular*, but not all of its
// copy behavior is value-semantic.  Specifically, for certain values (i.e.,
// those where 'isExternalReference' is 'true') 'ManagedDatum' performs a
// shallow copy (copying the reference rather than the value), which is
// inconsistent with value-semantics.
//
// Note that a default constructed 'ManagedDatum', or a 'ManagedDatum' on which
// 'release' has been called will have the null 'Datum' value.
//
///Resource-Management
///-------------------
// A 'Datum' object's relationship to memory can be seen as analogous to a raw
// pointer, requiring calls to static functions 'Datum::create*' and
// 'Datum::destroy' to initialize and release resources (see the {'bdld_datum'}
// component documentation).  A 'ManagedDatum', by extension, provides a
// resource manager for a 'Datum' that is an analogous to a smart-pointer.
//
// The 'adopt' method of a 'ManagedDatum' is used to take ownership of a
// supplied 'Datum' object, after which point the 'ManagedDatum' object's
// destructor will free the resources of the managed 'Datum' (unless 'release'
// is subsequently called).  Similar to a smart-pointer, a 'ManagedDatum'
// provides dereference operators to access the 'Datum' object under
// management.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bdld::MangedDatum'
///- - - - - - - - - - - - - - - - - - - - - -
// The example demonstrates the basic construction and manipulation of a
// 'ManagedDatum' object.
//
// First, we create a 'ManagedDatum' object that holds a double value and
// verify that it has the same double value inside it:
//..
//  bslma::TestAllocator ta("test", veryVeryVerbose);
//
//  const ManagedDatum realObj(Datum::createDouble(-3.4375), &ta);
//
//  assert(realObj->isDouble());
//  assert(-3.4375 == realObj->theDouble());
//..
// Next, we create a 'ManagedDatum' object that holds a string value and verify
// that it has the same string value inside it:
//..
//  const char         *str = "This is a string";
//  const ManagedDatum  strObj(Datum::copyString(str, &ta), &ta);
//
//  assert(strObj->isString());
//  assert(str == strObj->theString());
//..
// Then, assign this 'ManagedDatum' object to another object and verify both
// objects have the same value:
//..
//  ManagedDatum strObj1(&ta);
//  strObj1 = strObj;
//  assert(strObj == strObj1);
//..
// Next, copy-construct this 'ManagedDatum' object and verify that the copy has
// the same value as the original:
//..
//  const ManagedDatum strObj2(strObj, &ta);
//  assert(strObj == strObj2);
//..
// Then, we create a 'ManagedDatum' object that holds an opaque pointer to a
// 'bdlt::Date' object and verify that it has the same user-defined value
// inside it:
//..
//  bdlt::Date   udt;
//  ManagedDatum udtObj(Datum::createUdt(&udt, UDT_TYPE), &ta);
//
//  assert(udtObj->isUdt());
//  assert(&udt == udtObj->theUdt().data());
//  assert(UDT_TYPE == udtObj->theUdt().type());
//..
// Next, we assign a boolean value to this 'ManagedDatum' object and verify
// that it has the new value:
//..
//  udtObj.adopt(Datum::createBoolean(true));
//  assert(udtObj->isBoolean());
//  assert(true == udtObj->theBoolean());
//..
// Then, we create a 'ManagedDatum' object having an array and verify that it
// has the same array value.  Note that in practice we would use
// {'bdld_datumarraybuilder'}, but do not do so here to for dependency reasons.
//..
//  const Datum datumArray[2] = {
//      Datum::createInteger(12),
//      Datum::copyString("A long string", &ta)
//  };
//
//  DatumMutableArrayRef arr;
//  Datum::createUninitializedArray(&arr, 2 , &ta);
//  for (int i = 0; i < 2; ++i) {
//      arr.data()[i] = datumArray[i];
//  }
//  *(arr.length()) = 2;
//  const ManagedDatum arrayObj(Datum::adoptArray(arr), &ta);
//
//  assert(arrayObj->isArray());
//  assert(DatumArrayRef(datumArray, 2) == arrayObj->theArray());
//..
// Next, we create a 'ManagedDatum' object having a map and verify that it has
// the same map value.  Note that in practice we would use
// {'bdld_datummapbuilder'}, but do not do so here to for dependency reasons.
//..
//  const DatumMapEntry datumMap[2] = {
//      DatumMapEntry(StringRef("first", static_cast<int>(strlen("first"))),
//                    Datum::createInteger(12)),
//      DatumMapEntry(StringRef("second", static_cast<int>(strlen("second"))),
//                    Datum::copyString("A very long string", &ta))
//  };
//
//  DatumMutableMapRef mp;
//  Datum::createUninitializedMap(&mp, 2 , &ta);
//  for (int i = 0; i < 2; ++i) {
//      mp.data()[i] = datumMap[i];
//  }
//  *(mp.size()) = 2;
//  const ManagedDatum mapObj(Datum::adoptMap(mp), &ta);
//
//  assert(mapObj->isMap());
//  assert(DatumMapRef(datumMap, 2, false, false) == mapObj->theMap());
//..
// Then, we create a 'Datum' object and assign its ownership to a
// 'ManagedDatum' object and verify that the ownership was transferred:
//..
//  const Datum  rcObj = Datum::copyString("This is a string", &ta);
//  ManagedDatum obj(Datum::createInteger(1), &ta);
//  obj.adopt(rcObj);
//  assert(obj.datum() == rcObj);
//..
// Next, we release the 'Datum' object inside the 'ManagedDatum' object and
// verify that it was released:
//..
//  const Datum internalObj = obj.release();
//  assert(obj->isNull());
//  assert(internalObj == rcObj);
//..
// Finally, we destroy the released 'Datum' object:
//..
//  Datum::destroy(internalObj, obj.allocator());
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLD_DATUM
#include <bdld_datum.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace bdld {

                            // ==================
                            // class ManagedDatum
                            // ==================

class ManagedDatum {
    // This class implements a smart-pointer-like resource manager for a
    // 'Datum' object.

  private:
    // DATA
    Datum             d_data;         // storage for data
    bslma::Allocator *d_allocator_p;  // allocator to allocate any dynamic
                                      // memory

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ManagedDatum, bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(ManagedDatum, bslmf::IsBitwiseMoveable);
        // 'ManagedDatum' objects use 'bslma::Allocator' and they are bitwise
        // movable and trivially copyable.

    // CREATORS
    explicit ManagedDatum(bslma::Allocator *basicAllocator = 0);
        // Create a 'ManagedDatum' object having the default (null) value.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  De-refering the managed 'Datum' object and calling 'isNull'
        // will return 'true'.

    explicit ManagedDatum(const Datum&      value,
                          bslma::Allocator *basicAllocator = 0);
        // Create a 'ManagedDatum' assuming ownership of the specified 'value'.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless 'value' was allocated using
        // the indicated allocator, and 'value' is not subsequently destroyed
        // externally using the 'Datum::destroy' function.

    ManagedDatum(const ManagedDatum&  original,
                 bslma::Allocator    *basicAllocator = 0);
        // Create an object of type 'ManagedDatum' having the same value as the
        // specified 'original'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    ~ManagedDatum();
        // Destroy this object.  Use the stored allocator to release any memory
        // that was previously allocated.

    // MANIPULATORS
    ManagedDatum& operator=(const ManagedDatum& rhs);
        // Assign to this object the value of the specified 'rhs' object and
        // return a reference to this object.

    void adopt(const Datum& obj);
        // Take the ownership of the specified 'obj'.  Destroy the previous
        // value stored within this object.  The behavior is undefined unless
        // 'obj' was allocated using the same allocator as this object and it
        // is not subsequently destroyed externally using the 'Datum::destroy'
        // function.

    void clone(const Datum& value);
        // Assign to this object the specified 'value' by making a "deep-copy"
        // of 'value', so that any dynamically allocated memory managed by
        // 'value' is cloned and not shared with 'value'.

    void makeNull();
        // Make the value of this object to the null value.  Any dynamically
        // allocated value within this object is properly released.

    Datum release();
        // Return the 'Datum' value held inside this object and set the value
        // of this object to the null value.  Note that the previous value is
        // not destroyed.

    void swap(ManagedDatum& other);
        // Swap contents of this 'ManagedDatum' object with the contents of the
        // specified 'other' 'ManagedDatum' object.  The behavior is undefined
        // unless both the 'ManagedDatum' objects are allocated using the same
        // allocator.

    // ACCESSORS
    const Datum *operator->() const;
        // Return a pointer providing non-modifiable access to the 'Datum'
        // object held inside this object.

    const Datum& operator*() const;
        // Return a reference providing non-modifiable access to the 'Datum'
        // object held inside this object.

    bslma::Allocator *allocator() const;
        // Return a pointer providing modifiable access to the allocator
        // associated with this 'ManagedDatum'.

    const Datum& datum() const;
        // Return a reference providing non-modifiable access to the 'Datum'
        // object held inside this object.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to the modifiable 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // 'level').  If 'stream' is not valid on entry, this operation has no
        // effect.
};

// FREE OPERATORS
bool operator==(const ManagedDatum& lhs, const ManagedDatum& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'ManagedDatum' objects
    // have the same value, and 'false' otherwise.  Two 'ManagedDatum' objects
    // have the same value if their corresponding contained 'Datum' objects
    // have the same value.  For a detailed explanation about equality of
    // 'Datum' objects, refer to the documentation of operator '==' defined
    // for 'Datum'.

bool operator!=(const ManagedDatum& lhs, const ManagedDatum& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'ManagedDatum' objects do
    // not have the same value, and 'false' otherwise.  Two 'ManagedDatum'
    // objects do not have the same value if their corresponding contained
    // 'Datum' objects have the same value.  For a detailed explanation about
    // inequality of 'Datum' objects, refer to the documentation of operator
    // '==' defined for 'Datum'.

bsl::ostream& operator<<(bsl::ostream& stream, const ManagedDatum& rhs);
    // Write the specified 'rhs' value to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.  Note that this method
    // invokes the operator '<<' defined for 'Datum'.  For a detailed
    // explanation of the format of the output, refer to the documentation of
    // operator '<<' defined for 'Datum'.  The function will have no effect if
    // 'stream' is not valid.

// FREE FUNCTIONS
void swap(ManagedDatum& a, ManagedDatum& b);
    // Swap contents of the specified 'a' 'ManagedDatum' object with the
    // contents of the specified 'b' 'ManagedDatum' object.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                            // ------------------
                            // class ManagedDatum
                            // ------------------

// CREATORS
inline
ManagedDatum::ManagedDatum(bslma::Allocator *basicAllocator)
: d_data(Datum::createNull())
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
ManagedDatum::ManagedDatum(const Datum&      value,
                           bslma::Allocator *basicAllocator)
: d_data(value)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

inline
ManagedDatum::ManagedDatum(const ManagedDatum&  original,
                           bslma::Allocator    *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_data = original.d_data.clone(d_allocator_p);
}

inline
ManagedDatum::~ManagedDatum()
{
    Datum::destroy(d_data, d_allocator_p);
}

// MANIPULATORS
inline
ManagedDatum& ManagedDatum::operator=(const ManagedDatum& rhs)
{
    ManagedDatum copy(rhs, d_allocator_p);
    swap(copy);
    return *this;
}

inline
void ManagedDatum::adopt(const Datum& obj)
{
    if (&obj != &d_data) {
        ManagedDatum(obj, d_allocator_p).swap(*this);
    }
}

inline
void ManagedDatum::clone(const Datum& value)
{
    Datum data = value.clone(d_allocator_p);
    ManagedDatum(data, d_allocator_p).swap(*this);
}

inline
void ManagedDatum::makeNull()
{
    ManagedDatum(d_allocator_p).swap(*this);
}

inline
Datum ManagedDatum::release()
{
    Datum temp = d_data;
    d_data = Datum::createNull();
    return temp;
}

inline
void ManagedDatum::swap(ManagedDatum& other)
{
    BSLS_ASSERT_SAFE(d_allocator_p == other.d_allocator_p);
    using bsl::swap;
    swap(d_data, other.d_data);
}

// ACCESSORS
inline
const Datum *ManagedDatum::operator->() const
{
    return &d_data;
}

inline
const Datum& ManagedDatum::operator*() const
{
    return d_data;
}

inline
bslma::Allocator *ManagedDatum::allocator() const
{
    return d_allocator_p;
}

inline
const Datum& ManagedDatum::datum() const
{
    return d_data;
}

inline
bsl::ostream& ManagedDatum::print(bsl::ostream& stream,
                                  int           level,
                                  int           spacesPerLevel) const
{
    return d_data.print(stream, level, spacesPerLevel);
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdld::operator==(const ManagedDatum& lhs, const ManagedDatum& rhs)
{
    return (lhs.datum() == rhs.datum());
}

inline
bool bdld::operator!=(const ManagedDatum& lhs, const ManagedDatum& rhs)
{
    return (lhs.datum() != rhs.datum());
}

inline
bsl::ostream& bdld::operator<<(bsl::ostream& stream, const ManagedDatum& rhs)
{
    return (stream << rhs.datum());
}

// FREE FUNCTIONS
inline
void bdld::swap(ManagedDatum& a, ManagedDatum& b)
{
    if (a.allocator() == b.allocator()) {
        a.swap(b);
    }
    else {
        ManagedDatum tempA(a, b.allocator());
        ManagedDatum tempB(b, a.allocator());

        a.swap(tempB);
        b.swap(tempA);
    }
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
