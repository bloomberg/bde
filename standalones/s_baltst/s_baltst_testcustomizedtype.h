// s_baltst_testcustomizedtype.h                                      -*-C++-*-
#ifndef INCLUDED_S_BALTST_TESTCUSTOMIZEDTYPE
#define INCLUDED_S_BALTST_TESTCUSTOMIZEDTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a test implementation of a 'bdlat' "customized type".
//
//@CLASSES:
//  s_baltst::TestCustomizedType: test implementation of a customized type

#include <bdlat_customizedtypefunctions.h>

#include <bslalg_constructorproxy.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bslmf_isconvertible.h>

#include <bsl_ostream.h>

namespace BloombergLP {
namespace s_baltst {

                          // ========================
                          // class TestCustomizedType
                          // ========================

template <class VALUE_TYPE, class BASE_TYPE>
class TestCustomizedType {
    // This in-core value-semantic class provides a basic implementation of the
    // 'bdlat' 'CustomizedType' concept.  The template parameter 'VALUE_TYPE'
    // specifies the underlying value for objects of this type, and the
    // template parameter 'BASE_TYPE' specifies one of the public base types of
    // 'VALUE_TYPE'.  This type is said to "customize" the 'BASE_TYPE'.  The
    // program is ill-formed unless 'VALUE_TYPE' is the same as 'BASE_TYPE', or
    // publicly inherits from it.

  public:
    // TYPES
    typedef VALUE_TYPE Value;
    typedef BASE_TYPE  BaseType;

  private:
    // DATA
    bslalg::ConstructorProxy<Value> d_value;  // underlying value
    bslma::Allocator *d_allocator_p;  // memory supply (held, not owned)

  public:
    // CLASS METHODS
    static bool areEqual(const TestCustomizedType& lhs,
                         const TestCustomizedType& rhs);

    // CREATORS
    TestCustomizedType();

    explicit TestCustomizedType(bslma::Allocator *basicAllocator);

    explicit TestCustomizedType(const Value&      value,
                                bslma::Allocator *basicAllocator = 0);

    TestCustomizedType(const TestCustomizedType&  original,
                       bslma::Allocator          *basicAllocator = 0);

    // MANIPULATORS
    TestCustomizedType& operator=(const TestCustomizedType& original);

    template <class OTHER_BASE_TYPE>
    int convertFromBaseType(const OTHER_BASE_TYPE& value);

    void setValue(const Value& value);

    void reset();

    // ACCESSORS
    const char *className() const;

    const BaseType& convertToBaseType() const;

    const Value& value() const;
};

// FREE FUNCTIONS
template <class VALUE_TYPE, class BASE_TYPE>
bsl::ostream& operator<<(
                      bsl::ostream&                                    stream,
                      const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& object);

template <class VALUE_TYPE, class BASE_TYPE>
inline
bool operator==(const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& lhs,
                const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& rhs);

template <class VALUE_TYPE, class BASE_TYPE>
inline
bool operator!=(const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& lhs,
                const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& rhs);

// TRAITS
template <class VALUE_TYPE, class BASE_TYPE>
const char *bdlat_TypeName_className(
                      const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& object);

template <class VALUE_TYPE, class BASE_TYPE, class OTHER_BASE_TYPE>
int bdlat_customizedTypeConvertFromBaseType(
                             TestCustomizedType<VALUE_TYPE, BASE_TYPE> *object,
                             const OTHER_BASE_TYPE&                     base);

template <class VALUE_TYPE, class BASE_TYPE>
const BASE_TYPE& bdlat_customizedTypeConvertToBaseType(
                      const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& object);

template <class VALUE_TYPE, class BASE_TYPE>
int bdlat_valueTypeAssign(
                        TestCustomizedType<VALUE_TYPE, BASE_TYPE>        *lhs,
                        const TestCustomizedType<VALUE_TYPE, BASE_TYPE>&  rhs);

template <class VALUE_TYPE, class BASE_TYPE>
void bdlat_valueTypeReset(TestCustomizedType<VALUE_TYPE, BASE_TYPE> *object);

}  // close package namespace

namespace bdlat_CustomizedTypeFunctions {

template <class VALUE_TYPE, class BASE_TYPE>
struct IsCustomizedType<s_baltst::TestCustomizedType<VALUE_TYPE, BASE_TYPE> >
: public bsl::true_type
{
};

template <class VALUE_TYPE, class BASE_TYPE>
struct BaseType<s_baltst::TestCustomizedType<VALUE_TYPE, BASE_TYPE> > {
    typedef BASE_TYPE Type;
};

}  // close bdlat_CustomizedTypeFunctions namespace

namespace s_baltst {

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                          // ------------------------
                          // class TestCustomizedType
                          // ------------------------

// CLASS METHODS
template <class VALUE_TYPE, class BASE_TYPE>
bool TestCustomizedType<VALUE_TYPE, BASE_TYPE>::areEqual(
                                                 const TestCustomizedType& lhs,
                                                 const TestCustomizedType& rhs)
{
    return lhs.d_value.object() == rhs.d_value.object();
}

// CREATORS
template <class VALUE_TYPE, class BASE_TYPE>
TestCustomizedType<VALUE_TYPE, BASE_TYPE>::TestCustomizedType()
: d_value(bslma::Default::allocator())
, d_allocator_p(bslma::Default::allocator())
{
}

template <class VALUE_TYPE, class BASE_TYPE>
TestCustomizedType<VALUE_TYPE, BASE_TYPE>::TestCustomizedType(
                                              bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class VALUE_TYPE, class BASE_TYPE>
TestCustomizedType<VALUE_TYPE, BASE_TYPE>::TestCustomizedType(
                                              const Value&      value,
                                              bslma::Allocator *basicAllocator)
: d_value(value, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class VALUE_TYPE, class BASE_TYPE>
TestCustomizedType<VALUE_TYPE, BASE_TYPE>::TestCustomizedType(
                                     const TestCustomizedType&  original,
                                     bslma::Allocator          *basicAllocator)
: d_value(original.d_value.object(), basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

// MANIPULATORS
template <class VALUE_TYPE, class BASE_TYPE>
TestCustomizedType<VALUE_TYPE, BASE_TYPE>&
TestCustomizedType<VALUE_TYPE, BASE_TYPE>::operator=(
                                            const TestCustomizedType& original)
{
    d_value.object() = original.d_value.object();

    return *this;
}

template <class VALUE_TYPE, class BASE_TYPE>
template <class OTHER_BASE_TYPE>
int TestCustomizedType<VALUE_TYPE, BASE_TYPE>::convertFromBaseType(
                                                  const OTHER_BASE_TYPE& value)
    // If an explicit conversion from the specified 'OTHER_BASE_TYPE' type to
    // the 'Value' type exists, load into the value of this object the value of
    // the specified 'base' object explicitly converted to 'Value'.  Return 0
    // on success, and a non-zero value otherwise.
{
    if (!bslmf::IsConvertible<Value, OTHER_BASE_TYPE>::value) {
        return -1;                                                    // RETURN
    }

    d_value.object() = static_cast<Value>(value);
    return 0;
}

template <class VALUE_TYPE, class BASE_TYPE>
void TestCustomizedType<VALUE_TYPE, BASE_TYPE>::setValue(const Value& value)
{
    d_value.object() = value;
}

template <class VALUE_TYPE, class BASE_TYPE>
void TestCustomizedType<VALUE_TYPE, BASE_TYPE>::reset()
{
    d_value.object() = Value();
}

// ACCESSORS
template <class VALUE_TYPE, class BASE_TYPE>
const char *TestCustomizedType<VALUE_TYPE, BASE_TYPE>::className() const
    // Return a null-terminated string containing the exported name for this
    // type.
{
    return "MyCustomizedType";
}

template <class VALUE_TYPE, class BASE_TYPE>
const BASE_TYPE&
TestCustomizedType<VALUE_TYPE, BASE_TYPE>::convertToBaseType() const
    // Return a reference providing non-modifiable access to the 'Base'
    // subobject of the underlying value of this object.
{
    return d_value.object();
}

template <class VALUE_TYPE, class BASE_TYPE>
const VALUE_TYPE& TestCustomizedType<VALUE_TYPE, BASE_TYPE>::value() const
{
    return d_value.object();
}

// FREE FUNCTIONS
template <class VALUE_TYPE, class BASE_TYPE>
bsl::ostream& operator<<(
                       bsl::ostream&                                    stream,
                       const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& object)
{
    return stream << "[" << object.className() << " value " << object.value()
                  << "]";
}

template <class VALUE_TYPE, class BASE_TYPE>
inline
bool operator==(const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& lhs,
                const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& rhs)
{
    return TestCustomizedType<VALUE_TYPE, BASE_TYPE>::areEqual(lhs, rhs);
}

template <class VALUE_TYPE, class BASE_TYPE>
inline
bool operator!=(const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& lhs,
                const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& rhs)
{
    return !TestCustomizedType<VALUE_TYPE, BASE_TYPE>::areEqual(lhs, rhs);
}

// TRAITS
template <class VALUE_TYPE, class BASE_TYPE>
const char *bdlat_TypeName_className(
                       const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& object)
    // Return a null-terminated string containing the exported name of the type
    // for the specified 'object'.
{
    return object.className();
}

template <class VALUE_TYPE, class BASE_TYPE, class OTHER_BASE_TYPE>
int bdlat_customizedTypeConvertFromBaseType(
                             TestCustomizedType<VALUE_TYPE, BASE_TYPE> *object,
                             const OTHER_BASE_TYPE&                     base)
    // If an explicit conversion from the specified 'OTHER_BASE_TYPE' type to
    // the specified 'VALUE_TYPE' type exists, load into the underlying value
    // of the specified 'object' the value of the specified 'base' object
    // explicitly converted to 'VALUE_TYPE'.  Return 0 on success, and a
    // non-zero value otherwise.
{
    return object->convertFromBaseType(base);
}

template <class VALUE_TYPE, class BASE_TYPE>
const BASE_TYPE& bdlat_customizedTypeConvertToBaseType(
                       const TestCustomizedType<VALUE_TYPE, BASE_TYPE>& object)
    // Return a reference providing non-modifiable access to the 'Base'
    // subobject of the underlying value of the specified 'object'.
{
    return object.convertToBaseType();
}

template <class VALUE_TYPE, class BASE_TYPE>
int bdlat_valueTypeAssign(
                         TestCustomizedType<VALUE_TYPE, BASE_TYPE>        *lhs,
                         const TestCustomizedType<VALUE_TYPE, BASE_TYPE>&  rhs)
{
    *lhs = rhs;
    return 0;
}

template <class VALUE_TYPE, class BASE_TYPE>
void bdlat_valueTypeReset(TestCustomizedType<VALUE_TYPE, BASE_TYPE> *object)
{
    object->reset();
}


}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_TESTCUSTOMIZEDTYPE

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
