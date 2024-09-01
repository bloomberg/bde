// s_baltst_testenumeration.h                                         -*-C++-*-
#ifndef INCLUDED_S_BALTST_TESTENUMERATION
#define INCLUDED_S_BALTST_TESTENUMERATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a test implementation of a `bdlat` "enumeration" type.
//
//@CLASSES:
//  s_baltst::TestEnumeration: test implementation of a `bdlat` "enumeration"

#include <bdlat_enumfunctions.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <s_baltst_testenumerator.h>
#include <s_baltst_testnilvalue.h>

#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace s_baltst {

                           // =====================
                           // class TestEnumeration
                           // =====================

/// This in-core value-semantic class provides a basic implementation of the
/// `bdlat` `Enumeration` concept.
template <class E0, class E1 = TestNilEnumerator, class E2 = TestNilEnumerator>
class TestEnumeration {

  public:
    // INVARIANTS
    BSLMF_ASSERT((!bslmf::IsSame<TestNilEnumerator, E0>::value));

    // TYPES
    typedef E0 Enumerator0;
    typedef E1 Enumerator1;
    typedef E2 Enumerator2;

    // CLASS DATA
    enum {
        k_HAS_ENUMERATOR_1 = !bslmf::IsSame<TestNilValue, Enumerator1>::value,
        k_HAS_ENUMERATOR_2 = !bslmf::IsSame<TestNilValue, Enumerator2>::value
    };

  private:
    // DATA
    int d_value;

  public:
    // CLASS METHODS
    static bool areEqual(const TestEnumeration& lhs,
                         const TestEnumeration& rhs);

    // CREATORS
    TestEnumeration();

    explicit TestEnumeration(int value);

    TestEnumeration(const TestEnumeration& original);

    // MANIPULATORS
    TestEnumeration& operator=(const TestEnumeration& original);

    int fromInt(int number);

    int fromString(const char *string, int stringLength);

    // ACCESSORS
    const char *className() const;

    void toInt(int *result) const;

    void toString(bsl::string *result) const;
};

// FREE FUNCTIONS
template <class E0, class E1, class E2>
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const TestEnumeration<E0, E1, E2>& object);

template <class E0, class E1, class E2>
bool operator==(const TestEnumeration<E0, E1, E2>& lhs,
                const TestEnumeration<E0, E1, E2>& rhs);

template <class E0, class E1, class E2>
bool operator!=(const TestEnumeration<E0, E1, E2>& lhs,
                const TestEnumeration<E0, E1, E2>& rhs);

// TRAITS
template <class E0, class E1, class E2>
const char *bdlat_TypeName_className(
                                    const TestEnumeration<E0, E1, E2>& object);

template <class E0, class E1, class E2>
int bdlat_enumFromInt(TestEnumeration<E0, E1, E2> *result, int number);

template <class E0, class E1, class E2>
int bdlat_enumFromString(TestEnumeration<E0, E1, E2> *result,
                         const char                  *string,
                         int                          stringLength);

template <class E0, class E1, class E2>
void bdlat_enumToInt(int *result, const TestEnumeration<E0, E1, E2>& value);

template <class E0, class E1, class E2>
void bdlat_enumToString(bsl::string                        *result,
                        const TestEnumeration<E0, E1, E2>&  value);

}  // close package namespace

namespace bdlat_EnumFunctions {

template <class E0, class E1, class E2>
struct IsEnumeration<s_baltst::TestEnumeration<E0, E1, E2> >
: public bsl::true_type {
};

}  // close bdlat_EnumFunctions namespace

namespace s_baltst {

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ---------------------
                           // class TestEnumeration
                           // ---------------------

// CLASS METHODS
template <class E0, class E1, class E2>
bool TestEnumeration<E0, E1, E2>::areEqual(const TestEnumeration& lhs,
                                           const TestEnumeration& rhs)
{
    return lhs.d_value == rhs.d_value;
}

// CREATORS
template <class E0, class E1, class E2>
TestEnumeration<E0, E1, E2>::TestEnumeration()
: d_value(Enumerator0::intValue())
{
}

template <class E0, class E1, class E2>
TestEnumeration<E0, E1, E2>::TestEnumeration(int value)
: d_value(value)
{
}

template <class E0, class E1, class E2>
TestEnumeration<E0, E1, E2>::TestEnumeration(const TestEnumeration& original)
: d_value(original.d_value)
{
}

// MANIPULATORS
template <class E0, class E1, class E2>
TestEnumeration<E0, E1, E2>& TestEnumeration<E0, E1, E2>::operator=(
                                               const TestEnumeration& original)
{
    d_value = original.d_value;
    return *this;
}

template <class E0, class E1, class E2>
int TestEnumeration<E0, E1, E2>::fromInt(int number)
{
    if (Enumerator0::intValue() == number) {
        d_value = Enumerator0::intValue();
        return 0;                                                     // RETURN
    }

    if (k_HAS_ENUMERATOR_1 && Enumerator1::intValue() == number) {
        d_value = Enumerator1::intValue();
        return 0;                                                     // RETURN
    }

    if (k_HAS_ENUMERATOR_2 && Enumerator2::intValue() == number) {
        d_value = Enumerator2::intValue();
        return 0;                                                     // RETURN
    }

    return -1;
}

template <class E0, class E1, class E2>
int TestEnumeration<E0, E1, E2>::fromString(const char *string,
                                            int         stringLength)
{
    const bsl::string_view stringRef(string, stringLength);

    if (Enumerator0::stringValue() == stringRef) {
        d_value = Enumerator0::intValue();
        return 0;                                                     // RETURN
    }

    if (k_HAS_ENUMERATOR_1 && Enumerator1::stringValue() == stringRef) {
        d_value = Enumerator1::intValue();
        return 0;                                                     // RETURN
    }

    if (k_HAS_ENUMERATOR_2 && Enumerator2::stringValue() == stringRef) {
        d_value = Enumerator2::intValue();
        return 0;                                                     // RETURN
    }

    return -1;
}

// ACCESSORS
template <class E0, class E1, class E2>
const char *TestEnumeration<E0, E1, E2>::className() const
{
    return "MyEnumeration";
}

template <class E0, class E1, class E2>
void TestEnumeration<E0, E1, E2>::toInt(int *result) const
{
    BSLS_ASSERT(d_value == Enumerator0::intValue() ||
                (k_HAS_ENUMERATOR_1 && d_value == Enumerator1::intValue()) ||
                (k_HAS_ENUMERATOR_2 && d_value == Enumerator2::intValue()));

    *result = d_value;
}

template <class E0, class E1, class E2>
void TestEnumeration<E0, E1, E2>::toString(bsl::string *result) const
{
    BSLS_ASSERT(d_value == Enumerator0::intValue() ||
                (k_HAS_ENUMERATOR_1 && d_value == Enumerator1::intValue()) ||
                (k_HAS_ENUMERATOR_2 && d_value == Enumerator2::intValue()));

    if (Enumerator0::intValue() == d_value) {
        *result = Enumerator0::stringValue();
    }

    if (k_HAS_ENUMERATOR_1 && Enumerator1::intValue() == d_value) {
        *result = Enumerator1::stringValue();
    }

    if (k_HAS_ENUMERATOR_2 && Enumerator2::intValue() == d_value) {
        *result = Enumerator2::stringValue();
    }
}

// FREE FUNCTIONS
template <class E0, class E1, class E2>
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const TestEnumeration<E0, E1, E2>& object)
{
    bsl::string value;
    object.toString(&value);

    return stream << "[" << object.className() << " value " << value << "]";
}

template <class E0, class E1, class E2>
bool operator==(const TestEnumeration<E0, E1, E2>& lhs,
                const TestEnumeration<E0, E1, E2>& rhs)
{
    return TestEnumeration<E0, E1, E2>::areEqual(lhs, rhs);
}

template <class E0, class E1, class E2>
bool operator!=(const TestEnumeration<E0, E1, E2>& lhs,
                const TestEnumeration<E0, E1, E2>& rhs)
{
    return !TestEnumeration<E0, E1, E2>::areEqual(lhs, rhs);
}

// TRAITS
template <class E0, class E1, class E2>
const char *bdlat_TypeName_className(const TestEnumeration<E0, E1, E2>& object)
{
    return object.className();
}

template <class E0, class E1, class E2>
int bdlat_enumFromInt(TestEnumeration<E0, E1, E2> *result, int number)
{
    return result->fromInt(number);
}

template <class E0, class E1, class E2>
int bdlat_enumFromString(TestEnumeration<E0, E1, E2> *result,
                         const char                  *string,
                         int                          stringLength)
{
    return result->fromString(string, stringLength);
}

template <class E0, class E1, class E2>
void bdlat_enumToInt(int *result, const TestEnumeration<E0, E1, E2>& value)
{
    value.toInt(result);
}

template <class E0, class E1, class E2>
void bdlat_enumToString(bsl::string                        *result,
                        const TestEnumeration<E0, E1, E2>&  value)
{
    value.toString(result);
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_S_BALTST_TESTENUMERATION

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
