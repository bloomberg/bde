// s_baltst_testsequence.h                                            -*-C++-*-
#ifndef INCLUDED_S_BALTST_TESTSEQUENCE
#define INCLUDED_S_BALTST_TESTSEQUENCE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a test implementation of a 'bdlat' "sequence" type.
//
//@CLASSES:
//  s_baltst::TestSequence: test implementation of a 'bdlat' "sequence" type

#include <bdlat_sequencefunctions.h>
#include <bdlat_typetraits.h>
#include <bdlat_valuetypefunctions.h>

#include <bslalg_constructorproxy.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>
#include <bslmf_integralconstant.h>

#include <s_baltst_testattribute.h>
#include <s_baltst_testnilvalue.h>

namespace BloombergLP {
namespace s_baltst {

                        // ============================
                        // struct TestSequence_ImplUtil
                        // ============================

struct TestSequence_ImplUtil {

    // CLASS METHODS
    template <class ACCESSOR>
    static int access(ACCESSOR&                  accessor,
                      const TestNilValue         attribute,
                      const bdlat_AttributeInfo& attributeInfo);
    template <class ACCESSOR, class ATTRIBUTE>
    static int access(ACCESSOR&                  accessor,
                      const ATTRIBUTE&           attribute,
                      const bdlat_AttributeInfo& attributeInfo);

    template <class MANIPULATOR>
    static int manipulate(MANIPULATOR&                manipulator,
                          TestNilValue               *attribute,
                          const bdlat_AttributeInfo&  attributeInfo);
    template <class MANIPULATOR, class ATTRIBUTE>
    static int manipulate(MANIPULATOR&                manipulator,
                          ATTRIBUTE                  *attribute,
                          const bdlat_AttributeInfo&  attributeInfo);
};

                             // ==================
                             // class TestSequence
                             // ==================

template <class TYPED_ATTRIBUTE_0 =
              TypedTestAttribute<TestNilValue, TestAttribute<0> >,
          class TYPED_ATTRIBUTE_1 =
              TypedTestAttribute<TestNilValue, TestAttribute<0> >,
          class TYPED_ATTRIBUTE_2 =
              TypedTestAttribute<TestNilValue, TestAttribute<0> > >
class TestSequence {
    // This in-core value-semantic class provides a basic implementation of the
    // concept defined by the 'bdlat' 'Sequence' type category.  The template
    // parameters 'TYPED_ATTRIBUTE_0', 'TYPED_ATTRIBUTE_1', and
    // 'TYPED_ATTRIBUTE_2' must all satisfy the following requirements:
    //: o The type must have two member type definitions, 'Type' and
    // 'Attribute'.
    //: o 'Type' must meet the requirements of an in-core value-semantic type.
    //: o 'Type' must meet the requirements of exactly one of the
    //:    'bdlat' value categories.
    //: o 'Attribute' must be a specialization of the 'TestAttribute' type.
    // Further, each 'TestAttribute' member type definition of a template
    // argument must return values for 'TestAttribute::id()' and
    // 'TestAttribute::name()' that are different from all others within this
    // 'TestSequence' specialization.
    //
    // Additionally, the 'Type' of any template argument may be 'TestNilValue'
    // if all 'Type' member type definitions of subsequent template arguments
    // are also 'TestNilValue'.
    //
    // The 'Type' and 'Attribute' member type definitions of the template
    // arguments define the type and 'bdlat_AttributeInfo' of the attributes of
    // the 'bdlat' 'Selection' implementation provided by this class.  A
    // template argument having a 'TestNilValue' 'Type' indicates that the
    // corresponding attribute does not exist.

  public:
    // TYPES
    typedef typename TYPED_ATTRIBUTE_0::Type      Attribute0Type;
    typedef typename TYPED_ATTRIBUTE_0::Attribute Attribute0;
    typedef typename TYPED_ATTRIBUTE_1::Type      Attribute1Type;
    typedef typename TYPED_ATTRIBUTE_1::Attribute Attribute1;
    typedef typename TYPED_ATTRIBUTE_2::Type      Attribute2Type;
    typedef typename TYPED_ATTRIBUTE_2::Attribute Attribute2;

    // CLASS DATA
    enum {
        k_HAS_ATTRIBUTE_0 =
            !bslmf::IsSame<TestNilValue, Attribute0Type>::value,
        k_HAS_ATTRIBUTE_1 =
            !bslmf::IsSame<TestNilValue, Attribute1Type>::value,
        k_HAS_ATTRIBUTE_2 = !bslmf::IsSame<TestNilValue, Attribute2Type>::value
    };

  private:
    // PRIVATE TYPES
    typedef TestSequence_ImplUtil ImplUtil;

    // DATA
    bslalg::ConstructorProxy<Attribute0Type>  d_attribute0Value;
    bslalg::ConstructorProxy<Attribute1Type>  d_attribute1Value;
    bslalg::ConstructorProxy<Attribute2Type>  d_attribute2Value;
    bslma::Allocator                         *d_allocator_p;

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TestSequence, bslma::UsesBslmaAllocator);

    // CLASS METHODS
    static bool areEqual(const TestSequence& lhs, const TestSequence& rhs)
    {
        return lhs.d_attribute0Value.object() ==
                   rhs.d_attribute0Value.object() &&
               lhs.d_attribute1Value.object() ==
                   rhs.d_attribute1Value.object() &&
               lhs.d_attribute2Value.object() ==
                   rhs.d_attribute2Value.object();
    }

    // CREATORS
    TestSequence()
    : d_attribute0Value(bslma::Default::allocator())
    , d_attribute1Value(bslma::Default::allocator())
    , d_attribute2Value(bslma::Default::allocator())
    , d_allocator_p(bslma::Default::allocator())
    {
    }

    explicit TestSequence(bslma::Allocator *basicAllocator)
    : d_attribute0Value(bslma::Default::allocator(basicAllocator))
    , d_attribute1Value(bslma::Default::allocator(basicAllocator))
    , d_attribute2Value(bslma::Default::allocator(basicAllocator))
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    explicit TestSequence(const Attribute0Type  attribute0,
                          bslma::Allocator     *basicAllocator = 0)
    : d_attribute0Value(attribute0, bslma::Default::allocator(basicAllocator))
    , d_attribute1Value(bslma::Default::allocator(basicAllocator))
    , d_attribute2Value(bslma::Default::allocator(basicAllocator))
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        BSLMF_ASSERT(k_HAS_ATTRIBUTE_0);
    }

    explicit TestSequence(const Attribute0Type  attribute0,
                          const Attribute1Type  attribute1,
                          bslma::Allocator     *basicAllocator = 0)
    : d_attribute0Value(attribute0, bslma::Default::allocator(basicAllocator))
    , d_attribute1Value(attribute1, bslma::Default::allocator(basicAllocator))
    , d_attribute2Value(bslma::Default::allocator(basicAllocator))
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        BSLMF_ASSERT(k_HAS_ATTRIBUTE_0);
        BSLMF_ASSERT(k_HAS_ATTRIBUTE_1);
    }

    explicit TestSequence(const Attribute0Type  attribute0,
                          const Attribute1Type  attribute1,
                          const Attribute2Type  attribute2,
                          bslma::Allocator     *basicAllocator = 0)
    : d_attribute0Value(attribute0, bslma::Default::allocator(basicAllocator))
    , d_attribute1Value(attribute1, bslma::Default::allocator(basicAllocator))
    , d_attribute2Value(attribute2, bslma::Default::allocator(basicAllocator))
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        BSLMF_ASSERT(k_HAS_ATTRIBUTE_0);
        BSLMF_ASSERT(k_HAS_ATTRIBUTE_1);
        BSLMF_ASSERT(k_HAS_ATTRIBUTE_2);
    }

    TestSequence(const TestSequence&  original,
                 bslma::Allocator    *basicAllocator = 0)
    : d_attribute0Value(original.d_attribute0Value.object(),
                        bslma::Default::allocator(basicAllocator))
    , d_attribute1Value(original.d_attribute1Value.object(),
                        bslma::Default::allocator(basicAllocator))
    , d_attribute2Value(original.d_attribute2Value.object(),
                        bslma::Default::allocator(basicAllocator))
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
    }

    // MANIPULATORS
    TestSequence& operator=(const TestSequence& original)
    {
        d_attribute0Value.object() = original.d_attribute0Value.object();
        d_attribute1Value.object() = original.d_attribute1Value.object();
        d_attribute2Value.object() = original.d_attribute2Value.object();
        return *this;
    }

    template <class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *attributeName,
                            int           attributeNameLength)
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'attributeName'
        // and 'attributeNameLength' of this object, supplying 'manipulator'
        // with the corresponding attribute information structure.  Return a
        // non-zero value if the attribute is not found, and the value returned
        // from the invocation of 'manipulator' otherwise.
    {
        const bslstl::StringRef attributeNameRef(attributeName       ,
                                                 attributeNameLength);

        if (k_HAS_ATTRIBUTE_0 && Attribute0::name() == attributeNameRef) {
            return ImplUtil::manipulate(
                manipulator,
                &d_attribute0Value.object(),
                Attribute0::attributeInfo());                         // RETURN
        }

        if (k_HAS_ATTRIBUTE_1 && Attribute1::name() == attributeNameRef) {
            return ImplUtil::manipulate(
                manipulator,
                &d_attribute1Value.object(),
                Attribute1::attributeInfo());                         // RETURN
        }

        if (k_HAS_ATTRIBUTE_2 && Attribute2::name() == attributeNameRef) {
            return ImplUtil::manipulate(
                manipulator,
                &d_attribute2Value.object(),
                Attribute2::attributeInfo());                         // RETURN
        }

        return -1;
    }

    template <class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int attributeId)
        // Invoke the specified 'manipulator' on the address of the
        // (modifiable) attribute indicated by the specified 'attributeId' of
        // this object, supplying 'manipulator' with the corresponding
        // attribute information structure.  Return a non-zero value if the
        // attribute is not found, and the value returned from the invocation
        // of 'manipulator' otherwise.
    {
        if (k_HAS_ATTRIBUTE_0 && Attribute0::id() == attributeId) {
            return ImplUtil::manipulate(
                manipulator,
                &d_attribute0Value.object(),
                Attribute0::attributeInfo());                         // RETURN
        }

        if (k_HAS_ATTRIBUTE_1 && Attribute1::id() == attributeId) {
            return ImplUtil::manipulate(
                manipulator,
                &d_attribute1Value.object(),
                Attribute1::attributeInfo());                         // RETURN
        }

        if (k_HAS_ATTRIBUTE_2 && Attribute2::id() == attributeId) {
            return ImplUtil::manipulate(
                manipulator,
                &d_attribute2Value.object(),
                Attribute2::attributeInfo());                         // RETURN
        }

        return -1;
    }

    template <class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator)
        // Invoke the specified 'manipulator' sequentially on the address of
        // each (modifiable) attribute of this object, supplying 'manipulator'
        // with the corresponding attribute information structure until such
        // invocation returns a non-zero value.  Return the value from the last
        // invocation of 'manipulator'.
    {
        if (k_HAS_ATTRIBUTE_0) {
            int rc = ImplUtil::manipulate(manipulator,
                                          &d_attribute0Value.object(),
                                          Attribute0::attributeInfo());
            if (0 != rc) {
                return rc;                                            // RETURN
            }
        }

        if (k_HAS_ATTRIBUTE_1) {
            int rc = ImplUtil::manipulate(manipulator,
                                          &d_attribute1Value.object(),
                                          Attribute1::attributeInfo());
            if (0 != rc) {
                return rc;                                            // RETURN
            }
        }

        if (k_HAS_ATTRIBUTE_2) {
            int rc = ImplUtil::manipulate(manipulator,
                                          &d_attribute2Value.object(),
                                          Attribute2::attributeInfo());
            if (0 != rc) {
                return rc;                                            // RETURN
            }
        }

        return 0;
    }

    void reset()
    {
        if (k_HAS_ATTRIBUTE_0) {
            bdlat_ValueTypeFunctions::reset(&d_attribute0Value.object());
        }

        if (k_HAS_ATTRIBUTE_1) {
            bdlat_ValueTypeFunctions::reset(&d_attribute1Value.object());
        }

        if (k_HAS_ATTRIBUTE_2) {
            bdlat_ValueTypeFunctions::reset(&d_attribute2Value.object());
        }
    }

    // ACCESSORS
    template <class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *attributeName,
                        int         attributeNameLength) const
        // Invoke the specified 'accessor' on the (non-modifiable) attribute of
        // this object indicated by the specified 'attributeName' and
        // 'attributeNameLength', supplying 'accessor' with the corresponding
        // attribute information structure.  Return a non-zero value if the
        // attribute is not found, and the value returned from the invocation
        // of 'accessor' otherwise.
    {
        const bslstl::StringRef attributeNameRef(attributeName       ,
                                                 attributeNameLength);

        if (k_HAS_ATTRIBUTE_0 && Attribute0::name() == attributeNameRef) {
            return ImplUtil::access(accessor,
                                    d_attribute0Value.object(),
                                    Attribute0::attributeInfo());     // RETURN
        }

        if (k_HAS_ATTRIBUTE_1 && Attribute1::name() == attributeNameRef) {
            return ImplUtil::access(accessor,
                                    d_attribute1Value.object(),
                                    Attribute1::attributeInfo());     // RETURN
        }

        if (k_HAS_ATTRIBUTE_2 && Attribute2::name() == attributeNameRef) {
            return ImplUtil::access(accessor,
                                    d_attribute2Value.object(),
                                    Attribute2::attributeInfo());     // RETURN
        }

        return -1;
    }

    template <class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int attributeId) const
        // Invoke the specified 'accessor' on the attribute of this object with
        // the given 'attributeId', supplying 'accessor' with the corresponding
        // attribute information structure.  Return non-zero if the attribute
        // is not found, and the value returned from the invocation of
        // 'accessor' otherwise.
    {
        if (k_HAS_ATTRIBUTE_0 && Attribute0::id() == attributeId) {
            return ImplUtil::access(accessor,
                                    d_attribute0Value.object(),
                                    Attribute0::attributeInfo());     // RETURN
        }

        if (k_HAS_ATTRIBUTE_1 && Attribute1::id() == attributeId) {
            return ImplUtil::access(accessor,
                                    d_attribute1Value.object(),
                                    Attribute1::attributeInfo());     // RETURN
        }

        if (k_HAS_ATTRIBUTE_2 && Attribute2::id() == attributeId) {
            return ImplUtil::access(accessor,
                                    d_attribute2Value.object(),
                                    Attribute2::attributeInfo());     // RETURN
        }

        return -1;
    }

    template <class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const
        // Invoke the specified 'accessor' sequentially on each attribute of
        // this object, supplying 'accessor' with the corresponding attribute
        // information structure until such invocation returns a non-zero
        // value.  Return the value from the last invocation of 'accessor'.
    {
        if (k_HAS_ATTRIBUTE_0) {
            int rc = ImplUtil::access(accessor,
                                      d_attribute0Value.object(),
                                      Attribute0::attributeInfo());
            if (0 != rc) {
                return rc;                                            // RETURN
            }
        }

        if (k_HAS_ATTRIBUTE_1) {
            int rc = ImplUtil::access(accessor,
                                      d_attribute1Value.object(),
                                      Attribute1::attributeInfo());
            if (0 != rc) {
                return rc;                                            // RETURN
            }
        }

        if (k_HAS_ATTRIBUTE_2) {
            int rc = ImplUtil::access(accessor,
                                      d_attribute2Value.object(),
                                      Attribute2::attributeInfo());
            if (0 != rc) {
                return rc;                                            // RETURN
            }
        }

        return 0;
    }

    const char *className() const
        // Return a null-terminated string containing the exported name for
        // this class.
    {
        return "MySequence";
    }

    bool hasAttribute(const char *attributeName, int attributeNameLength) const
        // Return 'true' if this object has an attribute with the specified
        // 'attributeName' of the specified 'attributeNameLength', and 'false'
        // otherwise.
    {
        const bslstl::StringRef attributeNameRef(attributeName       ,
                                                 attributeNameLength);

        if (k_HAS_ATTRIBUTE_0 && Attribute0::name() == attributeNameRef) {
            return true;                                              // RETURN
        }

        if (k_HAS_ATTRIBUTE_1 && Attribute1::name() == attributeNameRef) {
            return true;                                              // RETURN
        }

        if (k_HAS_ATTRIBUTE_2 && Attribute2::name() == attributeNameRef) {
            return true;                                              // RETURN
        }

        return false;
    }

    bool hasAttribute(int attributeId) const
        // Return 'true' if this object has an attribute with the specified
        // 'attributeId', and 'false' otherwise.
    {
        if (k_HAS_ATTRIBUTE_0 && Attribute0::id() == attributeId) {
            return true;                                              // RETURN
        }

        if (k_HAS_ATTRIBUTE_1 && Attribute1::id() == attributeId) {
            return true;                                              // RETURN
        }

        if (k_HAS_ATTRIBUTE_2 && Attribute2::id() == attributeId) {
            return true;                                              // RETURN
        }

        return false;
    }

    const Attribute0Type attribute0() const
    {
        BSLS_ASSERT(k_HAS_ATTRIBUTE_0);
        return d_attribute0Value.object();
    }

    const Attribute1Type attribute1() const
    {
        BSLS_ASSERT(k_HAS_ATTRIBUTE_1);
        return d_attribute1Value.object();
    }

    const Attribute2Type attribute2() const
    {
        BSLS_ASSERT(k_HAS_ATTRIBUTE_2);
        return d_attribute2Value.object();
    }
};

// FREE OPERATORS
template <class V0, class V1, class V2>
bsl::ostream& operator<<(bsl::ostream&                   stream,
                         const TestSequence<V0, V1, V2>& object)
{
    typedef TestSequence<V0, V1, V2> Sequence;

    stream << "[" << object.className() << " ";

    if (Sequence::k_HAS_ATTRIBUTE_0) {
        stream << Sequence::Attribute0::name() << " = " << object.attribute0();
    }

    if (Sequence::k_HAS_ATTRIBUTE_1) {
        stream << ", " << Sequence::Attribute1::name() << " = "
               << object.attribute1();
    }

    if (Sequence::k_HAS_ATTRIBUTE_2) {
        stream << ", " << Sequence::Attribute2::name() << " = "
               << object.attribute2();
    }

    return stream << "]";
}

template <class V0, class V1, class V2>
bool operator==(const TestSequence<V0, V1, V2>& lhs,
                const TestSequence<V0, V1, V2>& rhs)
{
    return TestSequence<V0, V1, V2>::areEqual(lhs, rhs);
}

template <class V0, class V1, class V2>
bool operator!=(const TestSequence<V0, V1, V2>& lhs,
                const TestSequence<V0, V1, V2>& rhs)
{
    return !TestSequence<V0, V1, V2>::areEqual(lhs, rhs);
}

// TRAITS
template <class V0, class V1, class V2>
const char *bdlat_TypeName_className(const TestSequence<V0, V1, V2>& object)
    // Return a null-terminated string containing the exported name for the
    // type of the specified 'object'.
{
    return object.className();
}

template <class V0, class V1, class V2, class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(
                                 TestSequence<V0, V1, V2> *object,
                                 MANIPULATOR&              manipulator,
                                 const char               *attributeName,
                                 int                       attributeNameLength)
    // Invoke the specified 'manipulator' on the address of the (modifiable)
    // attribute indicated by the specified 'attributeName' and
    // 'attributeNameLength' of the specified 'object', supplying 'manipulator'
    // with the corresponding attribute information structure.  Return a
    // non-zero value if the attribute is not found, and the value returned
    // from the invocation of 'manipulator' otherwise.
{
    return object->manipulateAttribute(
        manipulator, attributeName, attributeNameLength);
}

template <class V0, class V1, class V2, class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(TestSequence<V0, V1, V2> *object,
                                      MANIPULATOR&              manipulator,
                                      int                       attributeId)
    // Invoke the specified 'manipulator' on the address of the (modifiable)
    // attribute indicated by the specified 'attributeId' of the specified
    // 'object', supplying 'manipulator' with the corresponding attribute
    // information structure.  Return a non-zero value if the attribute is not
    // found, and the value returned from the invocation of 'manipulator'
    // otherwise.
{
    return object->manipulateAttribute(manipulator, attributeId);
}

template <class V0, class V1, class V2, class MANIPULATOR>
int bdlat_sequenceManipulateAttributes(TestSequence<V0, V1, V2> *object,
                                       MANIPULATOR&              manipulator)
    // Invoke the specified 'manipulator' sequentially on the address of each
    // (modifiable) attribute of the specified 'object', supplying
    // 'manipulator' with the corresponding attribute information structure
    // until such invocation returns a non-zero value.  Return the value from
    // the last invocation of 'manipulator'.
{
    return object->manipulateAttributes(manipulator);
}

template <class V0, class V1, class V2, class ACCESSOR>
int bdlat_sequenceAccessAttribute(
                          const TestSequence<V0, V1, V2>&  object,
                          ACCESSOR&                        accessor,
                          const char                      *attributeName,
                          int                              attributeNameLength)
    // Invoke the specified 'accessor' on the (non-modifiable) attribute of the
    // specified 'object' indicated by the specified 'attributeName' and
    // 'attributeNameLength', supplying 'accessor' with the corresponding
    // attribute information structure.  Return a non-zero value if the
    // attribute is not found, and the value returned from the invocation of
    // 'accessor' otherwise.
{
    return object.accessAttribute(
        accessor, attributeName, attributeNameLength);
}

template <class V0, class V1, class V2, class ACCESSOR>
int bdlat_sequenceAccessAttribute(const TestSequence<V0, V1, V2>& object,
                                  ACCESSOR&                       accessor,
                                  int                             attributeId)
    // Invoke the specified 'accessor' on the attribute of the specified
    // 'object' with the given 'attributeId', supplying 'accessor' with the
    // corresponding attribute information structure.  Return non-zero if the
    // attribute is not found, and the value returned from the invocation of
    // 'accessor' otherwise.
{
    return object.accessAttribute(accessor, attributeId);
}

template <class V0, class V1, class V2, class ACCESSOR>
int bdlat_sequenceAccessAttributes(const TestSequence<V0, V1, V2>& object,
                                   ACCESSOR&                       accessor)
    // Invoke the specified 'accessor' sequentially on each attribute of the
    // specified 'object', supplying 'accessor' with the corresponding
    // attribute information structure until such invocation returns a non-zero
    // value.  Return the value from the last invocation of 'accessor'.
{
    return object.accessAttributes(accessor);
}

template <class V0, class V1, class V2>
bool bdlat_sequenceHasAttribute(
                          const TestSequence<V0, V1, V2>&  object,
                          const char                      *attributeName,
                          int                              attributeNameLength)
    // Return 'true' if the specified 'object' has an attribute with the
    // specified 'attributeName' of the specified 'attributeNameLength', and
    // 'false' otherwise.
{
    return object.hasAttribute(attributeName, attributeNameLength);
}

template <class V0, class V1, class V2>
bool bdlat_sequenceHasAttribute(const TestSequence<V0, V1, V2>& object,
                                int                             attributeId)
    // Return 'true' if the specified 'object' has an attribute with the
    // specified 'attributeId', and 'false' otherwise.
{
    return object.hasAttribute(attributeId);
}

template <class V0, class V1, class V2>
int bdlat_valueTypeAssign(TestSequence<V0, V1, V2>        *lhs,
                          const TestSequence<V0, V1, V2>&  rhs)
{
    *lhs = rhs;
    return 0;
}

template <class V0, class V1, class V2>
void bdlat_valueTypeReset(TestSequence<V0, V1, V2> *object)
{
    object->reset();
}

}  // close package namespace

namespace bdlat_SequenceFunctions {

template <class V0, class V1, class V2>
struct IsSequence<s_baltst::TestSequence<V0, V1, V2> > {
    enum { VALUE = 1 };
};

}  // close bdlat_SequenceFunctions namespace

namespace s_baltst {

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                             // ------------------
                             // class TestSequence
                             // ------------------

                        // ----------------------------
                        // struct TestSequence_ImplUtil
                        // ----------------------------

// CLASS METHODS
template <class ACCESSOR>
int TestSequence_ImplUtil::access(ACCESSOR&,
                                  const TestNilValue,
                                  const bdlat_AttributeInfo&)
{
    return 0;
}

template <class ACCESSOR, class ATTRIBUTE>
int TestSequence_ImplUtil::access(ACCESSOR&                  accessor,
                                  const ATTRIBUTE&           attribute,
                                  const bdlat_AttributeInfo& attributeInfo)
{
    return accessor(attribute, attributeInfo);
}

template <class MANIPULATOR>
int TestSequence_ImplUtil::manipulate(MANIPULATOR&,
                                      TestNilValue               *,
                                      const bdlat_AttributeInfo&)
{
    return 0;
}

template <class MANIPULATOR, class ATTRIBUTE>
int TestSequence_ImplUtil::manipulate(
                                    MANIPULATOR&                manipulator,
                                    ATTRIBUTE                  *attribute,
                                    const bdlat_AttributeInfo&  attributeInfo)
{
    return manipulator(attribute, attributeInfo);
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_TESTSEQUENCE

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
