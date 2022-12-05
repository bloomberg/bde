// bdld_manageddatum.h                                                -*-C++-*-
#ifndef INCLUDED_BDLD_MANAGEDDATUM
#define INCLUDED_BDLD_MANAGEDDATUM

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a smart-pointer-like manager for a 'Datum' object.
//
//@CLASSES:
//  bdld::ManagedDatum: a smart-pointer-like manager for a 'Datum' object
//
//@SEE_ALSO: bdld_datum
//
//@DESCRIPTION: This component implements a type, 'bdld::ManagedDatum', that
// provides two important services for 'Datum' objects:
//
//: 1 'ManagedDatum' provides value-semantic-like operations for 'Datum'.
//:
//: 2 'ManagedDatum' is a resource manager, similar to a smart pointer, for
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
///Value Semantics
///---------------
// 'ManagedDatum', while not strictly a value-semantic type, provides the full
// set of value-semantic-like operations for 'Datum' (see
// {'bsldoc_glossary'|Value-Semantic Operations}):
//
//: o Equality and Non-Equality Comparisons
//: o Copy Construction
//: o Copy Assignment
//: o Default Construction
//: o 'ostream' Printing
//
// In other words, the syntax of 'ManagedDatum' is *regular*, but not all of
// its copy behavior is value-semantic.  Specifically, for User Defined Types
// (i.e., those that 'bdld::Datum::clone' does not deep-copy) 'ManagedDatum'
// performs a shallow copy (copying the reference rather than the value), which
// is inconsistent with value-semantics.  For *all* other types 'ManagedDatum'
// copy operations (copy construction, copy assignment, and non-member 'swap'
// when the allocators differ) will deep-copy the value using 'Datum::clone,
// which creates a completely independent copy, with independent lifetime, by
// duplicating all data, even referenced data (except for UDTs).
//
// Note that a default constructed 'ManagedDatum', or a 'ManagedDatum' on which
// 'release' has been called, will have the null 'Datum' value.
//
///Resource Management
///-------------------
// A 'Datum' object's relationship to memory can be seen as analogous to a raw
// pointer, requiring calls to static functions 'Datum::create*' and
// 'Datum::destroy' to initialize and release resources (see the {'bdld_datum'}
// component documentation).  A 'ManagedDatum', by extension, provides a
// resource manager for a 'Datum' that is analogous to a smart pointer.
//
// The 'adopt' method of a 'ManagedDatum' is used to take ownership of a
// supplied 'Datum' object, after which point the 'ManagedDatum' object's
// destructor will free the resources of the managed 'Datum' (unless 'release'
// is subsequently called).  Similar to a smart pointer, a 'ManagedDatum'
// provides dereference operators to access the 'Datum' object under
// management.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bdld::ManagedDatum'
/// - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates the basic construction and manipulation of a
// 'ManagedDatum' object.
//
// First, we create a 'ManagedDatum' object that manages a 'Datum' holding a
// 'double' and verify that the managed object has the expected type and value:
//..
//  bslma::TestAllocator ta("test", veryVeryVerbose);
//
//  const ManagedDatum realObj(Datum::createDouble(-3.4375), &ta);
//
//  assert(realObj->isDouble());
//  assert(-3.4375 == realObj->theDouble());
//..
// Next, we create a 'ManagedDatum' object that holds a string and again verify
// that it has the expected type and value:
//..
//  const char         *str = "This is a string";
//  const ManagedDatum  strObj(Datum::copyString(str, &ta), &ta);
//
//  assert(strObj->isString());
//  assert(str == strObj->theString());
//..
// Then, we assign this 'ManagedDatum' object to another object and verify both
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
// 'bdlt::Date' object and verify that the managed 'Date' has the expected
// value:
//..
//  bdlt::Date   udt;
//  ManagedDatum udtObj(Datum::createUdt(&udt, UDT_TYPE), &ta);
//
//  assert(udtObj->isUdt());
//  assert(&udt == udtObj->theUdt().data());
//  assert(UDT_TYPE == udtObj->theUdt().type());
//..
// Next, we assign a boolean value to this 'ManagedDatum' object and verify
// that it has the new type and value:
//..
//  udtObj.adopt(Datum::createBoolean(true));
//  assert(udtObj->isBoolean());
//  assert(true == udtObj->theBoolean());
//..
// Then, we create a 'ManagedDatum' object having an array and verify that it
// has the same array value.  Note that in practice we would use
// {'bdld_datumarraybuilder'}, but do not do so here for dependency reasons:
//..
//  const Datum datumArray[2] = {
//      Datum::createInteger(12),
//      Datum::copyString("A long string", &ta)
//  };
//
//  DatumMutableArrayRef arr;
//  Datum::createUninitializedArray(&arr, 2, &ta);
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
//  Datum::createUninitializedMap(&mp, 2, &ta);
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
// Next, we release the 'Datum' object managed by 'obj' and verify that it was
// released:
//..
//  const Datum internalObj = obj.release();
//  assert(obj->isNull());
//  assert(internalObj == rcObj);
//..
// Finally, we destroy the released 'Datum' object:
//..
//  Datum::destroy(internalObj, obj.get_allocator().mechanism());
//..

#include <bdlscm_version.h>

#include <bdld_datum.h>

#include <bslma_allocator.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_isbitwisemoveable.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_review.h>

#include <bsl_algorithm.h>
#include <bsl_iosfwd.h>

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
    Datum                d_data;       // storage for data
    bsl::allocator<char> d_allocator;  // allocator of dynamic memory

  public:
    // TYPES
    typedef bsl::allocator<char> allocator_type;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ManagedDatum, bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(ManagedDatum, bslmf::IsBitwiseMoveable);
        // 'ManagedDatum' objects are allocator-aware and bitwise movable.

    // CREATORS
    ManagedDatum();
    explicit ManagedDatum(const allocator_type& allocator);
        // Create a 'ManagedDatum' object having the default (null) value.
        // Optionally specify an 'allocator' (e.g., the address of a
        // 'bslma::Allocator' object) to supply memory; otherwise, the default
        // allocator is used.  Calling 'isNull' on the resulting managed
        // 'Datum' object returns 'true'.

    explicit ManagedDatum(const Datum&          datum,
                          const allocator_type& allocator = allocator_type());
        // Create a 'ManagedDatum' object that assumes ownership of the
        // specified 'datum'.  Optionally specify an 'allocator' (e.g., the
        // address of a 'bslma::Allocator' object) to supply memory; otherwise,
        // the default allocator is used.  The behavior is undefined unless
        // 'datum' was allocated using the indicated allocator and is not
        // subsequently destroyed externally using 'Datum::destroy'.

    ManagedDatum(const ManagedDatum&   original,
                 const allocator_type& allocator = allocator_type());
        // Create a 'ManagedDatum' object having the same value as the
        // specified 'original' object.  Optionally specify an 'allocator'
        // (e.g., the address of a 'bslma::Allocator' object) used to supply
        // memory; otherwise, the default allocator is used.  This operation
        // performs a 'clone' of the underlying 'Datum', see {Value Semantics}
        // for more detail.

    ~ManagedDatum();
        // Destroy this object and release all dynamically allocated memory
        // managed by this object.

    // MANIPULATORS
    ManagedDatum& operator=(const ManagedDatum& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a non-'const' reference to this object.  This operation
        // performs a 'clone' of the underlying 'Datum', see {Value Semantics}
        // for more detail.

    void adopt(const Datum& obj);
        // Take ownership of the specified 'obj' and destroy the 'Datum'
        // object previously managed by this object.  The behavior is undefined
        // unless 'obj' was allocated using the same allocator used by this
        // object and is not subsequently destroyed externally using
        // 'Datum::destroy'.

    void clone(const Datum& value);
        // Assign to this object the specified 'value' by making a "deep copy"
        // of 'value', so that any dynamically allocated memory managed by
        // 'value' is cloned and not shared with 'value'.

    void makeNull();
        // Make the 'Datum' object managed by this object null and release all
        // dynamically allocated memory managed by this object.

    Datum release();
        // Return, *by* *value*, the 'Datum' object managed by this object and
        // set the managed object to null.  Ownership of the previously managed
        // 'Datum' object is transferred to the caller.

    void swap(ManagedDatum& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    const Datum *operator->() const;
        // Return an address providing non-modifiable access to the 'Datum'
        // object managed by this object.

    const Datum& operator*() const;
        // Return a 'const' reference to the 'Datum' object managed by this
        // object.

    const Datum& datum() const;
        // Return a 'const' reference to the 'Datum' object managed by this
        // object.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // !DEPRECATED!: Use 'get_allocator()' instead.
        //
        // Return 'get_allocator().mechanism()'.

    allocator_type get_allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the default
        // allocator in effect at construction is used.

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
    // have the same value if their corresponding managed 'Datum' objects have
    // the same value.  See the function-level documentation of the 'Datum'
    // equality-comparison operators for details.

bool operator!=(const ManagedDatum& lhs, const ManagedDatum& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' 'ManagedDatum' objects do
    // not have the same value, and 'false' otherwise.  Two 'ManagedDatum'
    // objects do not have the same value if their corresponding managed
    // 'Datum' objects do not have the same value.  See the function-level
    // documentation of the 'Datum' equality-comparison operators for details.

bsl::ostream& operator<<(bsl::ostream& stream, const ManagedDatum& rhs);
    // Write the specified 'rhs' value to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.  This function has no
    // effect if 'stream' is not valid on entry.  Note that this method invokes
    // 'operator<<' defined for 'Datum'.  See the function-level documentation
    // of the 'operator<<' defined for 'Datum' for details of the format of the
    // output.

// FREE FUNCTIONS
void swap(ManagedDatum& a, ManagedDatum& b);
    // Exchange the values of the specified 'a' and 'b' objects.  This function
    // provides the no-throw exception-safety guarantee if the two objects were
    // created with the same allocator and the basic guarantee otherwise.  Note
    // that in case the allocators are different this function places a *clone*
    // of 'a' into 'b', and vice versa.  See {Value Semantics} on details of
    // the cloning that may happen.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                            // ------------------
                            // class ManagedDatum
                            // ------------------

// CREATORS
inline
ManagedDatum::ManagedDatum()
: d_data(Datum::createNull())
, d_allocator()
{
}

inline
ManagedDatum::ManagedDatum(const allocator_type& allocator)
: d_data(Datum::createNull())
, d_allocator(allocator)
{
}

inline
ManagedDatum::ManagedDatum(const Datum& datum, const allocator_type& allocator)
: d_data(datum)
, d_allocator(allocator)
{
}

inline
ManagedDatum::ManagedDatum(const ManagedDatum&   original,
                           const allocator_type& allocator)
: d_allocator(allocator)
{
    d_data = original.d_data.clone(d_allocator.mechanism());
}

inline
ManagedDatum::~ManagedDatum()
{
    Datum::destroy(d_data, d_allocator.mechanism());
}

// MANIPULATORS
inline
ManagedDatum& ManagedDatum::operator=(const ManagedDatum& rhs)
{
    ManagedDatum copy(rhs, d_allocator);
    swap(copy);
    return *this;
}

inline
void ManagedDatum::adopt(const Datum& obj)
{
    if (&obj != &d_data) {
        ManagedDatum(obj, d_allocator).swap(*this);
    }
}

inline
void ManagedDatum::clone(const Datum& value)
{
    Datum data = value.clone(d_allocator.mechanism());
    ManagedDatum(data, d_allocator).swap(*this);
}

inline
void ManagedDatum::makeNull()
{
    ManagedDatum(d_allocator).swap(*this);
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
    BSLS_ASSERT(d_allocator == other.get_allocator());

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
const Datum& ManagedDatum::datum() const
{
    return d_data;
}

                                  // Aspects

inline
bslma::Allocator *ManagedDatum::allocator() const
{
    return get_allocator().mechanism();
}

inline
ManagedDatum::allocator_type ManagedDatum::get_allocator() const
{
    return d_allocator;
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
    if (a.get_allocator() == b.get_allocator()) {
        a.swap(b);
    }
    else {
        ManagedDatum tempA(a, b.get_allocator());
        ManagedDatum tempB(b, a.get_allocator());

        a.swap(tempB);
        b.swap(tempA);
    }
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
