// bdld_manageddatum.h                                                -*-C++-*-
#ifndef INCLUDED_BDLD_MANAGEDDATUM
#define INCLUDED_BDLD_MANAGEDDATUM

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$ $CSID$")

//@PURPOSE: Provide a value-semantic type for general-purpose values.
//
//@CLASSES:
//  bdld::ManagedDatum: full value-semantic type on top of 'Datum'
//
//@SEE_ALSO: bdld_datum, bdld_error, bdld_udt
//
//@AUTHOR: Rishi Wani (pwani)
//
//@DESCRIPTION: This component defines a full *value-semantic* type
// 'ManagedDatum', capable of efficiently representing general-purpose values
// and their aggregates.  All the types that can be represented are listed
// below.  This class wraps a 'Datum' and an optional 'bslma::Allocator' and
// creates a full *value-semantic* type.
//
// 'ManagedDatum' can hold the following types of values:
//     * boolean (bool)
//     * integer (int)
//     * real (double)
//     * 64 bit integer (bsls::Types::Int64)
//     * error (bdld::Error)
//     * date (bdlt::Date)
//     * time (bdlt::Time)
//     * date+time (bdlt::Datetime)
//     * date+time interval (bdlt::DatetimeInterval)
//     * string (const char *)
//     * user-defined type objects (void *)
//     * array of 'Datum' objects ('ConstDatumArrayRef')
//     * map of 'Datum' objects keyed by string values ('ConstDatumMapRef')
// 'ManagedDatum' can also have no value, i.e., be null.
//
///Usage
///-----
// The following snippets of code illustrate how to create and use a
// 'ManagedDatum' object.
//
// First create a 'ManagedDatum' object that holds a double value and verify
// that it has the same double value inside it.
//..
//  const ManagedDatum realObj(-3.457);
//  assert(realObj->isDouble());
//  assert(-3.457 == realObj->theDouble());
//..
// Next we create a 'ManagedDatum' object that holds a string value and verify
// that it has the same string value inside it.
//..
//  bslma::TestAllocator defaultAlloc = bslma::TestAllocator(false);
//  const char *str = "This is a string";
//  const ManagedDatum strObj(str, &defaultAlloc);
//  assert(strObj->isString());
//  assert(bslstl::StringRef(str, bsl::strlen(str)) == strObj->theString());
//..
// Then assign this 'ManagedDatum' object to another object and verify both
// objects have the same value.
//..
//  const ManagedDatum strObj1 = strObj;
//  assert(strObj == strObj1);
//..
// Next copy-construct this 'ManagedDatum' object and verify that the copy has
// the same value as the original.
//..
//  const ManagedDatum strObj2(strObj, &defaultAlloc);
//  assert(strObj == strObj2);
//..
// Then we create a 'ManagedDatum' object that holds an opaque pointer to a
// 'bdlt::Date' object and verify that it has the same user-defined value inside
// it.
//..
//  const bdlt::Date dt;
//  ManagedDatum udtObj(Udt(&dt, 1));
//  assert(udtObj->isUdt());
//  assert(&dt == udtObj->theUdt().data());
//  assert(1 == udtObj->theUdt().type());
//..
// Next we assign a boolean value to this 'ManagedDatum' object and verify that
// it has the new value.
//..
//  udtObj.adopt(Datum::createBoolean(true));
//  assert(udtObj->isBoolean());
//  assert(true == udtObj->theBoolean());
//..
// Then we create a 'ManagedDatum' object having an array and verify that it
// has the same array value.
//..
//  const Datum datumArray[2] = {
//      Datum::createInteger(12),
//      Datum::copyString("A long string", &defaultAlloc)
//  };
//  DatumArrayRef arr;
//  Datum::createUninitializedArray(&arr, 2 , &ta);
//  for (int i = 0; i < 2; ++i) {
//      arr.data()[i] = datumArray[i];
//  }
//  *(arr.length()) = 2;
//  const ManagedDatum arrayObj(Datum::adoptArray(arr),
//                              &ta);
//  assert(arrayObj.isArray());
//  assert(ConstDatumArrayRef(datumArray, 2) == arrayObj.theArray());
//..
// Next we create a 'ManagedDatum' object having a map and verify that it has
// the same map value.
//..
//  const DatumMapEntry datumMap[2] = {
//      DatumMapEntry(bslstl::StringRef("first", bsl::strlen("first")),
//                    Datum::createInteger(12)),
//      DatumMapEntry(bslstl::StringRef("second", bsl::strlen("second")),
//                    Datum::copyString("A long string",
//                                      &defaultAlloc))
//  };
//  DatumMapRef mp;
//  Datum::createUninitializedMap(&mp, 2 , &ta);
//  for (int i = 0; i < 2; ++i) {
//      mp.data()[i] = datumMap[i];
//  }
//  *(mp.size()) = 2;
//  const ManagedDatum mapObj(Datum::adoptMap(mp), &ta);
//  assert(mapObj.isMap());
//  assert(ConstDatumMapRef(datumMap, 2) == mapObj.theMap());
//..
// Then we create a 'Datum' object and assign its ownership to a 'ManagedDatum'
// object and verify that the ownership was transferred.
//..
//  const Datum rcObj = Datum::copyString("This is a string",
//                                        &defaultAlloc);
//  ManagedDatum obj(1, &defaultAlloc);
//  obj.adoptDatum(rcObj);
//  assert(obj.datum() == rcObj);
//..
// Next we release the 'Datum' object inside the 'ManagedDatum' object and
// verify that it was released.
//..
//  const Datum internalObj = obj.release();
//  assert(obj->isNull());
//  assert(internalObj == rcObj);
//..
// Finally we destroy the released 'Datum' object.
//..
//  Datum::destroy(internalObj, obj.allocator());
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLD_DATUM
#include <bdld_datum.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
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
    // This class provides a full *value-semantic* representation of general-
    // purpose values and their aggregates.  This class manages the life-time
    // of the contained 'Datum' object

  private:
    // DATA
    Datum             d_data;         // storage for data
    bslma::Allocator *d_allocator_p;  // allocator to allocate any dynamic
                                      // memory

  public:
    // CREATORS
    explicit ManagedDatum(bslma::Allocator *basicAllocator = 0);
        // Create a 'ManagedDatum' object having the default (no) value.  Use
        // the optionally specified 'basicAllocator' to supply memory (if
        // needed).  Note that if 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ManagedDatum(const Datum&      value,
                 bslma::Allocator *basicAllocator);
        // Create a 'ManagedDatum' assuming ownership of the specified 'value'.
        // Note that the caller must not destroy 'value' after after creating
        // 'ManagedDatum' object using this constructor.  The behavior is
        // undefined unless '0 != basicAllocator' and 'value' was allocated
        // using the specified 'basicAllocator'.

    ManagedDatum(const ManagedDatum&  original,
                 bslma::Allocator    *basicAllocator = 0);
        // Create an object of type 'ManagedDatum' having the same value as the
        // specified 'original'.  Use the optionally specified 'basicAllocator'
        // to supply memory (if needed).  Note that if 'basicAllocator' is 0,
        // the currently installed default allocator is used.

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
        // is not managed by any 'ManagedDatum' object.  Note that the caller
        // must not destroy 'obj' after calling 'adopt'.

    void clone(const Datum& value);
        // Assign to this object the specified 'value' by making a "deep-copy"
        // of 'value', so that any dynamically allocated memory is cloned and
        // not shared with 'value'.

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
        // Return the address of the 'Datum' object held inside this object.

    const Datum& operator*() const;
        // Return a reference providing non-modifiable access to the 'Datum'
        // object held inside this object.

    bslma::Allocator *allocator() const;
        // Return the allocator associated with this 'ManagedDatum'.

    const Datum& datum() const;
        // Return the 'Datum' object stored inside this object.

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

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(ManagedDatum,
                                  bslalg::TypeTraitUsesBslmaAllocator,
                                  bslalg::TypeTraitBitwiseMoveable);
        // 'ManagedDatum' objects use 'bslma::Allocator' and they are bitwise
        // movable.
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

bsl::ostream& operator<<(bsl::ostream& output, const ManagedDatum& rhs);
    // Write the specified 'rhs' value to the specified output 'stream' and
    // return a reference to the modifiable 'stream'.  Note that this method
    // invokes the operator '<<' defined for 'Datum'.  For a detailed
    // explanation of the format of the output, refer to the documentation of
    // operator '<<' defined for 'Datum'.  The function will have no effect if
    // the specified 'stream' is not valid.

// FREE FUNCTIONS
void swap(ManagedDatum& lhs, ManagedDatum& rhs);
    // Swap contents of 'lhs' 'ManagedDatum' object with the contents of the
    // specified 'rhs' 'ManagedDatum' object.  It is undefined behavior to swap
    // two 'ManagedDatum' objects that are allocated using different
    // allocators.

// ===========================================================================
//                       INLINE FUNCTION DEFINITIONS
// ===========================================================================

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
, d_allocator_p(basicAllocator)
{
    BSLS_ASSERT(0 != basicAllocator);
}

inline
ManagedDatum::ManagedDatum(const ManagedDatum&  value,
                           bslma::Allocator    *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    d_data = value.d_data.clone(d_allocator_p);
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
    if (this != &rhs) { // check against self-assignment
        ManagedDatum copy(rhs, d_allocator_p);
        swap(copy);
    }
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
    if (&value != &d_data) {
        Datum data = value.clone(d_allocator_p);
        ManagedDatum(data, d_allocator_p).swap(*this);
    }
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

}  // close bdld namespace

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

// FREE FUNCTIONS
inline
void bdld::swap(ManagedDatum& lhs, ManagedDatum& rhs)
{
    lhs.swap(rhs);
}

}  // close BloombergLP namespace

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
