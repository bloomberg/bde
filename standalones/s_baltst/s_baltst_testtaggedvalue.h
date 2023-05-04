// s_baltst_testtaggedvalue.h                                         -*-C++-*-
#ifndef INCLUDED_S_BALTST_TESTTAGGEDVALUE
#define INCLUDED_S_BALTST_TESTTAGGEDVALUE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a test 'bdlat'-compatible type with interceptable methods.
//
//@CLASSES:
//  s_baltst::TestTaggedValue: interceptible test 'bdlat'-compatible type

#include <s_baltst_testplaceholder.h>

#include <bdlat_arrayfunctions.h>
#include <bdlat_choicefunctions.h>
#include <bdlat_customizedtypefunctions.h>
#include <bdlat_enumfunctions.h>
#include <bdlat_nullablevaluefunctions.h>
#include <bdlat_sequencefunctions.h>
#include <bdlat_typecategory.h>
#include <bdlat_valuetypefunctions.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bslalg_constructorproxy.h>

namespace BloombergLP {
namespace s_baltst {

                      // ===============================
                      // class TestTaggedValue_ArrayBase
                      // ===============================

// FORWARD DECLARATIONS
template <class TAG_TYPE, class VALUE_TYPE>
class TestTaggedValue;

template <class TAG_TYPE,
          class VALUE_TYPE,
          bool = static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_ARRAY_CATEGORY ||
                 static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_DYNAMIC_CATEGORY>
class TestTaggedValue_ArrayBase {
    // This class provides a base class and member functions implementing the
    // 'bdlat' 'Array' concept for 'TestTaggedValue' specializations having a
    // 'VALUE_TYPE' that itself implements the 'bdlat' 'Array' concept.

  public:
    // TYPES
    typedef typename bdlat_ArrayFunctions::ElementType<VALUE_TYPE>::Type
        ElementType;

    // CLASS DATA
    enum {
        k_IS_ARRAY = bdlat_ArrayFunctions::IsArray<VALUE_TYPE>::VALUE
    };

  private:
    // PRIVATE MANIPULATORS
    VALUE_TYPE& object();

    // PRIVATE ACCESSORS
    const VALUE_TYPE& object() const;

  public:
    // MANIPULATORS
    template <class MANIPULATOR>
    int manipulateElement(MANIPULATOR& manipulator, int index);

    void resize(int newSize);

    // ACCESSORS
    template <class ACCESSOR>
    int accessElement(ACCESSOR& accessor, int index) const;

    bsl::size_t size() const;
};

template <class TAG_TYPE, class VALUE_TYPE>
class TestTaggedValue_ArrayBase<TAG_TYPE, VALUE_TYPE, false> {
    // This class provides a base class for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that does not implement the 'bdlat' 'Array'
    // concept.

  public:
    // CLASS DATA
    enum { k_IS_ARRAY = false };
};

                      // ================================
                      // class TestTaggedValue_ChoiceBase
                      // ================================

template <class TAG_TYPE,
          class VALUE_TYPE,
          bool = static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                        bdlat_TypeCategory::e_CHOICE_CATEGORY ||
                 static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_DYNAMIC_CATEGORY>
class TestTaggedValue_ChoiceBase {
    // This class provides a base class and member functions implementing the
    // 'bdlat' 'Choice' concept for 'TestTaggedValue' specializations having a
    // 'VALUE_TYPE' that itself implements the 'bdlat' 'Choice' concept.

  public:
    // CLASS DATA
    enum { k_IS_CHOICE = bdlat_ChoiceFunctions::IsChoice<VALUE_TYPE>::VALUE };

  private:
    // PRIVATE MANIPULATORS
    VALUE_TYPE& object();

    // PRIVATE ACCESSORS
    const VALUE_TYPE& object() const;

  public:
    // MANIPULATORS
    int makeSelection(int selectionId);

    int makeSelection(const char *selectionName, int selectionNameLength);

    template <class MANIPULATOR>
    int manipulateSelection(MANIPULATOR& manipulator);

    // ACCESSORS
    template <class ACCESSOR>
    int accessSelection(ACCESSOR& accessor) const;

    bool hasSelection(int selectionId) const;

    bool hasSelection(const char *selectionName,
                      int         selectionNameLength) const;

    int selectionId() const;
};

template <class TAG_TYPE, class VALUE_TYPE>
class TestTaggedValue_ChoiceBase<TAG_TYPE, VALUE_TYPE, false> {
    // This class provides a base class for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that does not implement the 'bdlat' 'Choice'
    // concept.

  public:
    // CLASS DATA
    enum { k_IS_CHOICE = false };
};

                  // ========================================
                  // class TestTaggedValue_CustomizedTypeBase
                  // ========================================

template <class TAG_TYPE,
          class VALUE_TYPE,
          bool = static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY ||
                 static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_DYNAMIC_CATEGORY>
class TestTaggedValue_CustomizedTypeBase {
    // This class provides a base class and member functions implementing the
    // 'bdlat' 'CustomizedType' concept for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that itself implements the 'bdlat'
    // 'CustomizedType' concept.

  public:
    // TYPES
    typedef typename bdlat_CustomizedTypeFunctions::BaseType<VALUE_TYPE>::Type
        BaseType;

    // CLASS DATA
    enum {
        k_IS_CUSTOMIZED_TYPE =
            bdlat_CustomizedTypeFunctions::IsCustomizedType<VALUE_TYPE>::VALUE
    };

  private:
    // PRIVATE MANIPULATORS
    VALUE_TYPE& object();

    // PRIVATE ACCESSORS
    const VALUE_TYPE& object() const;

  public:
    // MANIPULATORS
    template <class BASE_TYPE>
    int convertFromBaseType(const BASE_TYPE& object);

    // ACCESSORS
    const BaseType& convertToBaseType() const;
};

template <class TAG_TYPE, class VALUE_TYPE>
class TestTaggedValue_CustomizedTypeBase<TAG_TYPE, VALUE_TYPE, false> {
    // This class provides a base class for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that does not implement the 'bdlat'
    // 'CustomizedType' concept.

  public:
    // CLASS DATA
    enum { k_IS_CUSTOMIZED_TYPE = false };
};

                   // =====================================
                   // class TestTaggedValue_DynamicTypeBase
                   // =====================================

template <class TAG_TYPE,
          class VALUE_TYPE,
          bool = static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_DYNAMIC_CATEGORY>
class TestTaggedValue_DynamicTypeBase {
    // This class provides a base class and member functions implementing the
    // 'bdlat' 'DynamicType' concept for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that itself implements the 'bdlat' 'DynamicType'
    // concept.

  public:
    // CLASS DATA
    enum { k_IS_DYNAMIC_TYPE = true };

  private:
    // PRIVATE ACCESSORS
    const VALUE_TYPE& object() const;

  public:
    // ACCESSORS
    bdlat_TypeCategory::Value select() const;
};

template <class TAG_TYPE, class VALUE_TYPE>
class TestTaggedValue_DynamicTypeBase<TAG_TYPE, VALUE_TYPE, false> {
    // This class provides a base class for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that does not implement the 'bdlat' 'DynamicType'
    // concept.

  public:
    // CLASS DATA
    enum { k_IS_DYNAMIC_TYPE = false };
};

                   // =====================================
                   // class TestTaggedValue_EnumerationBase
                   // =====================================

template <class TAG_TYPE,
          class VALUE_TYPE,
          bool = static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_ENUMERATION_CATEGORY ||
                 static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_DYNAMIC_CATEGORY>
class TestTaggedValue_EnumerationBase {
    // This class provides a base class and member functions implementing the
    // 'bdlat' 'Enumeration' concept for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that itself implements the 'bdlat' 'Enumeration'
    // concept.

  public:
    // CLASS DATA
    enum {
        k_IS_ENUMERATION =
            bdlat_EnumFunctions::IsEnumeration<VALUE_TYPE>::VALUE
    };

  private:
    // PRIVATE MANIPULATORS
    VALUE_TYPE& object();

    // PRIVATE ACCESSORS
    const VALUE_TYPE& object() const;

  public:
    // MANIPULATORS
    int fromInt(int value);

    int fromString(const char *string, int stringLength);

    // ACCESSORS
    void toInt(int *result) const;

    void toString(bsl::string *result) const;
};

template <class TAG_TYPE, class VALUE_TYPE>
class TestTaggedValue_EnumerationBase<TAG_TYPE, VALUE_TYPE, false> {
    // This class provides a base class for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that does not implement the 'bdlat' 'Enumeration'
    // concept.

  public:
    // CLASS DATA
    enum { k_IS_ENUMERATION = false };
};

                  // =======================================
                  // class TestTaggedValue_NullableValueBase
                  // =======================================

template <class TAG_TYPE,
          class VALUE_TYPE,
          bool = static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY ||
                 static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_DYNAMIC_CATEGORY>
class TestTaggedValue_NullableValueBase {
    // This class provides a base class and member functions implementing the
    // 'bdlat' 'NullableValue' concept for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that itself implements the 'bdlat' 'NullableValue'
    // concept.

  public:
    // TYPES
    typedef typename bdlat_NullableValueFunctions::template ValueType<
        VALUE_TYPE>::Type ValueType;

    // CLASS DATA
    enum {
        k_IS_NULLABLE_VALUE =
            bdlat_NullableValueFunctions::IsNullableValue<VALUE_TYPE>::VALUE
    };

  private:
    // PRIVATE MANIPULATORS
    VALUE_TYPE& object();

    // PRIVATE ACCESSORS
    const VALUE_TYPE& object() const;

  public:
    // MANIPULATORS
    void makeValue();

    template <class MANIPULATOR>
    int manipulateValue(MANIPULATOR& manipulator);

    // ACCESSORS
    template <class ACCESSOR>
    int accessValue(ACCESSOR& accessor) const;

    bool isNull() const;
};

template <class TAG_TYPE, class VALUE_TYPE>
class TestTaggedValue_NullableValueBase<TAG_TYPE, VALUE_TYPE, false> {
    // This class provides a base class for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that does not implement the 'bdlat'
    // 'NullableValue' concept.

  public:
    // CLASS DATA
    enum { k_IS_NULLABLE_VALUE = false };
};

                     // ==================================
                     // class TestTaggedValue_SequenceBase
                     // ==================================

template <class TAG_TYPE,
          class VALUE_TYPE,
          bool = static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_SEQUENCE_CATEGORY ||
                 static_cast<bdlat_TypeCategory::Value>(
                     bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
                         bdlat_TypeCategory::e_DYNAMIC_CATEGORY>
class TestTaggedValue_SequenceBase {
    // This class provides a base class and member functions implementing the
    // 'bdlat' 'Sequence' concept for 'TestTaggedValue' specializations having
    // a 'VALUE_TYPE' that itself implements the 'bdlat' 'Sequence' concept.

  public:
    // CLASS DATA
    enum {
        k_IS_SEQUENCE = bdlat_SequenceFunctions::IsSequence<VALUE_TYPE>::VALUE
    };

  private:
    // PRIVATE MANIPULATORS
    VALUE_TYPE& object();

    // PRIVATE ACCESSORS
    const VALUE_TYPE& object() const;

  public:
    // MANIPULATORS
    template <class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR& manipulator, int attributeId);

    template <class MANIPULATOR>
    int manipulateAttribute(MANIPULATOR&  manipulator,
                            const char   *attributeName,
                            int           attributeNameLength);

    template <class MANIPULATOR>
    int manipulateAttributes(MANIPULATOR& manipulator);

    // ACCESSORS
    template <class ACCESSOR>
    int accessAttribute(ACCESSOR& accessor, int attributeId) const;

    template <class ACCESSOR>
    int accessAttribute(ACCESSOR&   accessor,
                        const char *attributeName,
                        int         attributeNameLength) const;

    template <class ACCESSOR>
    int accessAttributes(ACCESSOR& accessor) const;

    bool hasAttribute(int attributeId) const;

    bool hasAttribute(const char *attributeName,
                      int         attributeNameLength) const;
};

template <class TAG_TYPE, class VALUE_TYPE>
class TestTaggedValue_SequenceBase<TAG_TYPE, VALUE_TYPE, false> {
    // This class provides a base class for 'TestTaggedValue' specializations
    // having a 'VALUE_TYPE' that does not implement the 'bdlat' 'Sequence'
    // concept.

  public:
    // CLASS DATA
    enum { k_IS_SEQUENCE = false };
};

                           // =====================
                           // class TestTaggedValue
                           // =====================

template <class TAG_TYPE, class VALUE_TYPE>
class TestTaggedValue
: public TestTaggedValue_ArrayBase<TAG_TYPE, VALUE_TYPE>,
  public TestTaggedValue_ChoiceBase<TAG_TYPE, VALUE_TYPE>,
  public TestTaggedValue_CustomizedTypeBase<TAG_TYPE, VALUE_TYPE>,
  public TestTaggedValue_DynamicTypeBase<TAG_TYPE, VALUE_TYPE>,
  public TestTaggedValue_EnumerationBase<TAG_TYPE, VALUE_TYPE>,
  public TestTaggedValue_NullableValueBase<TAG_TYPE, VALUE_TYPE>,
  public TestTaggedValue_SequenceBase<TAG_TYPE, VALUE_TYPE> {
    // This class provides a wrapper around an object of the specified
    // 'VALUE_TYPE' that implements all 'bdlat' value category concepts the
    // 'VALUE_TYPE' implements by delegating the implementation to the wrapped
    // object.  However, more than just forwarding these operations, this class
    // permits one to overload particular 'bdlat' operations of the underlying
    // 'VALUE_TYPE' object by providing an overload for the corresponding
    // 'bdlat' free function with a particular 'TAG_TYPE'.  Such overloads will
    // be better matches during overload resolution.

  public:
    // TYPES
    typedef TAG_TYPE   Tag;
    typedef VALUE_TYPE Value;

  private:
    // DATA
    bslalg::ConstructorProxy<Value>  d_value;
    bslma::Allocator                *d_allocator_p;

    // FRIENDS
    friend class TestTaggedValue_ArrayBase<TAG_TYPE, VALUE_TYPE>;
    friend class TestTaggedValue_ChoiceBase<TAG_TYPE, VALUE_TYPE>;
    friend class TestTaggedValue_CustomizedTypeBase<TAG_TYPE, VALUE_TYPE>;
    friend class TestTaggedValue_DynamicTypeBase<TAG_TYPE, VALUE_TYPE>;
    friend class TestTaggedValue_EnumerationBase<TAG_TYPE, VALUE_TYPE>;
    friend class TestTaggedValue_NullableValueBase<TAG_TYPE, VALUE_TYPE>;
    friend class TestTaggedValue_SequenceBase<TAG_TYPE, VALUE_TYPE>;

  public:
    // CLASS METHODS
    bool areEqual(const TestTaggedValue& lhs, const TestTaggedValue& rhs);

    // CREATORS
    TestTaggedValue();

    explicit TestTaggedValue(bslma::Allocator *basicAllocator);

    explicit TestTaggedValue(const Value&      value,
                             bslma::Allocator *basicAllocator = 0);

    TestTaggedValue(const TestTaggedValue&  original,
                    bslma::Allocator       *basicAllocator = 0);

    // MANIPULATORS
    TestTaggedValue& operator=(const TestTaggedValue& original);

    int assign(const VALUE_TYPE& rhs);

    int assign(const TestTaggedValue& rhs);

    void reset();

    // ACCESSORS
    const char *className() const;
};

// FREE FUNCTIONS
template <class TAG_TYPE, class VALUE_TYPE>
bool operator==(const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& lhs,
                const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& rhs);

template <class TAG_TYPE, class VALUE_TYPE>
bool operator!=(const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& lhs,
                const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& rhs);

// TRAITS
template <class TAG_TYPE, class VALUE_TYPE, class MANIPULATOR>
int bdlat_arrayManipulateElement(
                            TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                            MANIPULATOR&                           manipulator,
                            int                                    index);

template <class TAG_TYPE, class VALUE_TYPE>
void bdlat_arrayResize(TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                       int                                    newSize);

template <class TAG_TYPE, class VALUE_TYPE, class ACCESSOR>
int bdlat_arrayAccessElement(
                         const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                         ACCESSOR&                                    accessor,
                         int                                          index);

template <class TAG_TYPE, class VALUE_TYPE>
bsl::size_t bdlat_arraySize(
                          const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object);

}  // close package namespace

namespace bdlat_ArrayFunctions {

template <class TAG_TYPE, class VALUE_TYPE>
struct IsArray<s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE> > {
    enum {
        VALUE = s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE>::k_IS_ARRAY
    };
};

template <class TAG_TYPE, class VALUE_TYPE>
struct ElementType<s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE> > {
    typedef
        typename s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE>::ElementType
            Type;
};

}  // close bdlat_ArrayFunctions namespace

namespace s_baltst {

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_choiceMakeSelection(
                           TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                           int                                    selectionId);

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_choiceMakeSelection(
                   TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                   const char                            *selectionName,
                   int                                    selectionNameLength);

template <class TAG_TYPE, class VALUE_TYPE, class MANIPULATOR>
int bdlat_choiceManipulateSelection(
                           TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                           MANIPULATOR&                           manipulator);

template <class TAG_TYPE, class VALUE_TYPE, class ACCESSOR>
int bdlat_choiceAccessSelection(
                        const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                        ACCESSOR&                                    accessor);

template <class TAG_TYPE, class VALUE_TYPE>
bool bdlat_choiceHasSelection(
                     const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                     int                                          selectionId);

template <class TAG_TYPE, class VALUE_TYPE>
bool bdlat_choiceHasSelection(
            const TestTaggedValue<TAG_TYPE, VALUE_TYPE>&  object,
            const char                                   *selectionName,
            int                                           selectionNameLength);

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_choiceSelectionId(
                          const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object);

}  // close package namespace

template <class TAG_TYPE, class VALUE_TYPE>
struct bdlat_IsBasicChoice<s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE> >
: bsl::integral_constant<
      bool,
      s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE>::k_IS_CHOICE> {
};

namespace bdlat_ChoiceFunctions {

template <class TAG_TYPE, class VALUE_TYPE>
struct IsChoice<s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE> > {
    enum {
        VALUE = s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE>::k_IS_CHOICE
    };
};

}  // close bdlat_ChoiceFunctions namespace

namespace s_baltst {

template <class TAG_TYPE, class VALUE_TYPE, class BASE_TYPE>
int bdlat_customizedTypeConvertFromBaseType(
                                 TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                                 const BASE_TYPE&                       value);

template <class TAG_TYPE, class VALUE_TYPE>
const typename TestTaggedValue<TAG_TYPE, VALUE_TYPE>::BaseType&
bdlat_customizedTypeConvertToBaseType(
                          const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object);

}  // close package namespace

template <class TAG_TYPE, class VALUE_TYPE>
struct bdlat_IsBasicCustomizedType<
    s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE> >
: bsl::integral_constant<
      bool,
      s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE>::k_IS_CUSTOMIZED_TYPE> {
};

namespace bdlat_CustomizedTypeFunctions {

template <class TAG_TYPE, class VALUE_TYPE>
struct IsCustomizedType<s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE> > {
    enum {
        VALUE = s_baltst::TestTaggedValue<TAG_TYPE,
                                          VALUE_TYPE>::k_IS_CUSTOMIZED_TYPE
    };
};

template <class TAG_TYPE, class VALUE_TYPE>
struct BaseType<s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE> > {
    typedef typename s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE>::BaseType
        Type;
};

}  // close bdlat_CustomizedTypeFunctions namespace

namespace s_baltst {

template <class TAG_TYPE, class VALUE_TYPE>
bdlat_TypeCategory::Value bdlat_typeCategorySelect(
                          const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object);

}  // close package namespace

template <class TAG_TYPE, class VALUE_TYPE>
struct bdlat_TypeCategoryDeclareDynamic<
    s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE> > {
    enum {
        VALUE =
            s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE>::k_IS_DYNAMIC_TYPE
    };
};

namespace s_baltst {

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_enumFromInt(TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                      int                                    value);

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_enumFromString(TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                         const char                            *string,
                         int                                    stringLength);

template <class TAG_TYPE, class VALUE_TYPE>
void bdlat_enumToInt(int                                          *result,
                     const TestTaggedValue<TAG_TYPE, VALUE_TYPE>&  object);

template <class TAG_TYPE, class VALUE_TYPE>
void bdlat_enumToString(bsl::string                                  *result,
                        const TestTaggedValue<TAG_TYPE, VALUE_TYPE>&  object);

}  // close package namespace

template <class TAG_TYPE, class VALUE_TYPE>
struct bdlat_IsBasicEnumeration<
    s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE> >
: bsl::integral_constant<
      bool,
      s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE>::k_IS_ENUMERATION> {
};

namespace bdlat_EnumFunctions {

template <class TAG_TYPE, class VALUE_TYPE>
struct IsEnumeration<s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE> > {
    enum {
        VALUE =
            s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE>::k_IS_ENUMERATION
    };
};

}  // close bdlat_EnumFunctions namespace

namespace s_baltst {

template <class TAG_TYPE, class VALUE_TYPE>
void bdlat_nullableValueMakeValue(
                                TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object);

template <class TAG_TYPE, class VALUE_TYPE, class MANIPULATOR>
int bdlat_nullableValueManipulateValue(
                           TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                           MANIPULATOR&                           manipulator);

template <class TAG_TYPE, class VALUE_TYPE, class ACCESSOR>
int bdlat_nullableValueAccessValue(
                        const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                        ACCESSOR&                                    accessor);

template <class TAG_TYPE, class VALUE_TYPE>
bool bdlat_nullableValueIsNull(
                          const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object);

}  // close package namespace

namespace bdlat_NullableValueFunctions {

template <class TAG_TYPE, class VALUE_TYPE>
struct IsNullableValue<s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE> > {
    enum {
        VALUE = s_baltst::TestTaggedValue<TAG_TYPE,
                                          VALUE_TYPE>::k_IS_NULLABLE_VALUE
    };
};

template <class TAG_TYPE, class VALUE_TYPE>
struct ValueType<s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE> > {
    typedef typename s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE>::ValueType
        Type;
};

}  // close bdlat_NullableValueFunctions namespace

namespace s_baltst {

template <class TAG_TYPE, class VALUE_TYPE, class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(
                           TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                           MANIPULATOR&                           manipulator,
                           int                                    attributeId);

template <class TAG_TYPE, class VALUE_TYPE, class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(
                   TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                   MANIPULATOR&                           manipulator,
                   const char                            *attributeName,
                   int                                    attributeNameLength);

template <class TAG_TYPE, class VALUE_TYPE, class MANIPULATOR>
int bdlat_sequenceManipulateAttributes(
                           TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                           MANIPULATOR&                           manipulator);

template <class TAG_TYPE, class VALUE_TYPE, class ACCESSOR>
int bdlat_sequenceAccessAttribute(
                     const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                     ACCESSOR&                                    accessor,
                     int                                          attributeId);

template <class TAG_TYPE, class VALUE_TYPE, class ACCESSOR>
int bdlat_sequenceAccessAttribute(
            const TestTaggedValue<TAG_TYPE, VALUE_TYPE>&  object,
            ACCESSOR&                                     accessor,
            const char                                   *attributeName,
            int                                           attributeNameLength);

template <class TAG_TYPE, class VALUE_TYPE, class ACCESSOR>
int bdlat_sequenceAccessAttributes(
                        const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                        ACCESSOR&                                    accessor);

template <class TAG_TYPE, class VALUE_TYPE>
bool bdlat_sequenceHasAttribute(
                     const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                     int                                          attributeId);

template <class TAG_TYPE, class VALUE_TYPE>
bool bdlat_sequenceHasAttribute(
            const TestTaggedValue<TAG_TYPE, VALUE_TYPE>&  object,
            const char                                   *attributeName,
            int                                           attributeNameLength);

}  // close package namespace

template <class TAG_TYPE, class VALUE_TYPE>
struct bdlat_IsBasicSequence<s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE> >
: public bsl::integral_constant<
      bool,
      s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE>::k_IS_SEQUENCE> {
};

namespace bdlat_SequenceFunctions {

template <class TAG_TYPE, class VALUE_TYPE>
struct IsSequence<s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE> > {
    enum {
        VALUE = s_baltst::TestTaggedValue<TAG_TYPE, VALUE_TYPE>::k_IS_SEQUENCE
    };
};

}  // close bdlat_SequenceFunctions namespace

namespace s_baltst {

template <class TAG_TYPE, class VALUE_TYPE>
const char *bdlat_TypeName_className(
                          const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object);

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_valueTypeAssign(TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                          const VALUE_TYPE&                      value);

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_valueTypeAssign(TestTaggedValue<TAG_TYPE, VALUE_TYPE>        *object,
                          const TestTaggedValue<TAG_TYPE, VALUE_TYPE>&  value);

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

           // =====================================================
           // class TestTaggedValue_ArrayBase<TAG, VALUE, IS_ARRAY>
           // =====================================================

// PRIVATE MANIPULATORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_ARRAY>
VALUE_TYPE& TestTaggedValue_ArrayBase<TAG_TYPE, VALUE_TYPE, IS_ARRAY>::object()
{
    typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
    return static_cast<Derived *>(this)->d_value.object();
}

// PRIVATE ACCESSORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_ARRAY>
const VALUE_TYPE&
TestTaggedValue_ArrayBase<TAG_TYPE, VALUE_TYPE, IS_ARRAY>::object() const
{
    typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
    return static_cast<const Derived *>(this)->d_value.object();
}

// MANIPULATORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_ARRAY>
template <class MANIPULATOR>
int
TestTaggedValue_ArrayBase<TAG_TYPE, VALUE_TYPE, IS_ARRAY>::manipulateElement(
                                                      MANIPULATOR& manipulator,
                                                      int          index)
{
    return bdlat_ArrayFunctions::manipulateElement(
        &this->object(), manipulator, index);
}

template <class TAG_TYPE, class VALUE_TYPE, bool IS_ARRAY>
void TestTaggedValue_ArrayBase<TAG_TYPE, VALUE_TYPE, IS_ARRAY>::resize(
                                                                   int newSize)
{
    return bdlat_ArrayFunctions::resize(&this->object(), newSize);
}

// ACCESSORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_ARRAY>
template <class ACCESSOR>
int TestTaggedValue_ArrayBase<TAG_TYPE, VALUE_TYPE, IS_ARRAY>::accessElement(
                                                         ACCESSOR& accessor,
                                                         int       index) const
{
    return bdlat_ArrayFunctions::accessElement(
        this->object(), accessor, index);
}

template <class TAG_TYPE, class VALUE_TYPE, bool IS_ARRAY>
bsl::size_t
TestTaggedValue_ArrayBase<TAG_TYPE, VALUE_TYPE, IS_ARRAY>::size() const
{
    return bdlat_ArrayFunctions::size(this->object());
}

          // =======================================================
          // class TestTaggedValue_ChoiceBase<TAG, VALUE, IS_CHOICE>
          // =======================================================

// PRIVATE MANIPULATORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_CHOICE>
VALUE_TYPE&
TestTaggedValue_ChoiceBase<TAG_TYPE, VALUE_TYPE, IS_CHOICE>::object()
{
    typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
    return static_cast<Derived *>(this)->d_value.object();
}

// PRIVATE ACCESSORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_CHOICE>
const VALUE_TYPE&
TestTaggedValue_ChoiceBase<TAG_TYPE, VALUE_TYPE, IS_CHOICE>::object() const
{
    typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
    return static_cast<const Derived *>(this)->d_value.object();
}

// MANIPULATORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_CHOICE>
int TestTaggedValue_ChoiceBase<TAG_TYPE, VALUE_TYPE, IS_CHOICE>::makeSelection(
                                                               int selectionId)
{
    return bdlat_ChoiceFunctions::makeSelection(&this->object(), selectionId);
}

template <class TAG_TYPE, class VALUE_TYPE, bool IS_CHOICE>
int TestTaggedValue_ChoiceBase<TAG_TYPE, VALUE_TYPE, IS_CHOICE>::makeSelection(
                                               const char *selectionName,
                                               int         selectionNameLength)
{
    return bdlat_ChoiceFunctions::makeSelection(
        &this->object(), selectionName, selectionNameLength);
}

template <class TAG_TYPE, class VALUE_TYPE, bool IS_CHOICE>
template <class MANIPULATOR>
int TestTaggedValue_ChoiceBase<TAG_TYPE, VALUE_TYPE, IS_CHOICE>::
    manipulateSelection(MANIPULATOR& manipulator)
{
    return bdlat_ChoiceFunctions::manipulateSelection(&this->object(),
                                                      manipulator);
}

// ACCESSORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_CHOICE>
template <class ACCESSOR>
int
TestTaggedValue_ChoiceBase<TAG_TYPE, VALUE_TYPE, IS_CHOICE>::accessSelection(
                                                      ACCESSOR& accessor) const
{
    return bdlat_ChoiceFunctions::accessSelection(this->object(), accessor);
}

template <class TAG_TYPE, class VALUE_TYPE, bool IS_CHOICE>
bool TestTaggedValue_ChoiceBase<TAG_TYPE, VALUE_TYPE, IS_CHOICE>::hasSelection(
                                                         int selectionId) const
{
    return bdlat_ChoiceFunctions::hasSelection(this->object(), selectionId);
}

template <class TAG_TYPE, class VALUE_TYPE, bool IS_CHOICE>
bool TestTaggedValue_ChoiceBase<TAG_TYPE, VALUE_TYPE, IS_CHOICE>::hasSelection(
                                         const char *selectionName,
                                         int         selectionNameLength) const
{
    return bdlat_ChoiceFunctions::hasSelection(
        this->object(), selectionName, selectionNameLength);
}

template <class TAG_TYPE, class VALUE_TYPE, bool IS_CHOICE>
int TestTaggedValue_ChoiceBase<TAG_TYPE, VALUE_TYPE, IS_CHOICE>::selectionId()
    const
{
    return bdlat_ChoiceFunctions::selectionId(this->object());
}

       // =============================================================
       // class TestTaggedValue_CustomizedTypeBase<TAG, VALUE, IS_CUST>
       // =============================================================

// PRIVATE MANIPULATORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_CUSTOMIZED_TYPE>
VALUE_TYPE& TestTaggedValue_CustomizedTypeBase<TAG_TYPE,
                                               VALUE_TYPE,
                                               IS_CUSTOMIZED_TYPE>::object()
{
    typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
    return static_cast<Derived *>(this)->d_value.object();
}

// PRIVATE ACCESSORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_CUSTOMIZED_TYPE>
const VALUE_TYPE&
TestTaggedValue_CustomizedTypeBase<TAG_TYPE, VALUE_TYPE, IS_CUSTOMIZED_TYPE>::
    object() const
{
    typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
    return static_cast<const Derived *>(this)->d_value.object();
}

// MANIPULATORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_CUSTOMIZED_TYPE>
template <class BASE_TYPE>
int TestTaggedValue_CustomizedTypeBase<
    TAG_TYPE,
    VALUE_TYPE,
    IS_CUSTOMIZED_TYPE>::convertFromBaseType(const BASE_TYPE& object)
{
    return bdlat_CustomizedTypeFunctions::convertFromBaseType(&this->object(),
                                                              object);
}

// ACCESSORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_CUSTOMIZED_TYPE>
const typename bdlat_CustomizedTypeFunctions::BaseType<VALUE_TYPE>::Type&
TestTaggedValue_CustomizedTypeBase<TAG_TYPE, VALUE_TYPE, IS_CUSTOMIZED_TYPE>::
    convertToBaseType() const
{
    return bdlat_CustomizedTypeFunctions::convertToBaseType(this->object());
}

         // =========================================================
         // class TestTaggedValue_DynamicTypeBase<TAG, VALUE, IS_DYN>
         // =========================================================

// PRIVATE ACCESSORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_DYNAMIC_TYPE>
const VALUE_TYPE&
TestTaggedValue_DynamicTypeBase<TAG_TYPE, VALUE_TYPE, IS_DYNAMIC_TYPE>::
    object() const
{
    typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
    return static_cast<const Derived *>(this)->d_value.object();
}

// ACCESSORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_DYNAMIC_TYPE>
bdlat_TypeCategory::Value
TestTaggedValue_DynamicTypeBase<TAG_TYPE, VALUE_TYPE, IS_DYNAMIC_TYPE>::
    select() const
{
    return bdlat_TypeCategoryFunctions::select(this->object());
}

         // ==========================================================
         // class TestTaggedValue_EnumerationBase<TAG, VALUE, IS_ENUM>
         // ==========================================================

// PRIVATE MANIPULATORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_ENUMERATION>
VALUE_TYPE&
TestTaggedValue_EnumerationBase<TAG_TYPE, VALUE_TYPE, IS_ENUMERATION>::object()
{
    typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
    return static_cast<Derived *>(this)->d_value.object();
}

// PRIVATE ACCESSORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_ENUMERATION>
const VALUE_TYPE&
TestTaggedValue_EnumerationBase<TAG_TYPE, VALUE_TYPE, IS_ENUMERATION>::object()
    const
{
    typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
    return static_cast<const Derived *>(this)->d_value.object();
}

// MANIPULATORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_ENUMERATION>
int
TestTaggedValue_EnumerationBase<TAG_TYPE, VALUE_TYPE, IS_ENUMERATION>::fromInt(
                                                                     int value)
{
    return bdlat_EnumFunctions::fromInt(&this->object(), value);
}

template <class TAG_TYPE, class VALUE_TYPE, bool IS_ENUMERATION>
int TestTaggedValue_EnumerationBase<TAG_TYPE, VALUE_TYPE, IS_ENUMERATION>::
    fromString(const char *string, int stringLength)
{
    return bdlat_EnumFunctions::fromString(
        &this->object(), string, stringLength);
}

// ACCESSORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_ENUMERATION>
void
TestTaggedValue_EnumerationBase<TAG_TYPE, VALUE_TYPE, IS_ENUMERATION>::toInt(
                                                             int *result) const
{
    bdlat_EnumFunctions::toInt(result, this->object());
}

template <class TAG_TYPE, class VALUE_TYPE, bool IS_ENUMERATION>
void TestTaggedValue_EnumerationBase<TAG_TYPE, VALUE_TYPE, IS_ENUMERATION>::
    toString(bsl::string *result) const
{
    bdlat_EnumFunctions::toString(result, this->object());
}

        // ============================================================
        // class TestTaggedValue_NullableValueBase<TAG, VALUE, IS_NULL>
        // ============================================================

// PRIVATE MANIPULATORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_NULLABLE_VALUE>
VALUE_TYPE& TestTaggedValue_NullableValueBase<TAG_TYPE,
                                              VALUE_TYPE,
                                              IS_NULLABLE_VALUE>::object()
{
    typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
    return static_cast<Derived *>(this)->d_value.object();
}

// PRIVATE ACCESSORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_NULLABLE_VALUE>
const VALUE_TYPE&
TestTaggedValue_NullableValueBase<TAG_TYPE, VALUE_TYPE, IS_NULLABLE_VALUE>::
    object() const
{
    typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
    return static_cast<const Derived *>(this)->d_value.object();
}

// MANIPULATORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_NULLABLE_VALUE>
void TestTaggedValue_NullableValueBase<TAG_TYPE,
                                       VALUE_TYPE,
                                       IS_NULLABLE_VALUE>::makeValue()
{
    bdlat_NullableValueFunctions::makeValue(&this->object());
}

template <class TAG_TYPE, class VALUE_TYPE, bool IS_NULLABLE_VALUE>
template <class MANIPULATOR>
int TestTaggedValue_NullableValueBase<
    TAG_TYPE,
    VALUE_TYPE,
    IS_NULLABLE_VALUE>::manipulateValue(MANIPULATOR& manipulator)
{
    return bdlat_NullableValueFunctions::manipulateValue(&this->object(),
                                                         manipulator);
}

// ACCESSORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_NULLABLE_VALUE>
template <class ACCESSOR>
int TestTaggedValue_NullableValueBase<
    TAG_TYPE,
    VALUE_TYPE,
    IS_NULLABLE_VALUE>::accessValue(ACCESSOR& accessor) const
{
    return bdlat_NullableValueFunctions::accessValue(this->object(), accessor);
}

template <class TAG_TYPE, class VALUE_TYPE, bool IS_NULLABLE_VALUE>
bool TestTaggedValue_NullableValueBase<TAG_TYPE,
                                       VALUE_TYPE,
                                       IS_NULLABLE_VALUE>::isNull() const
{
    return bdlat_NullableValueFunctions::isNull(this->object());
}

           // ------------------------------------------------------
           // class TestTaggedValue_SequenceBase<TAG, VALUE, IS_SEQ>
           // ------------------------------------------------------

// PRIVATE MANIPULATORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_SEQUENCE>
VALUE_TYPE&
TestTaggedValue_SequenceBase<TAG_TYPE, VALUE_TYPE, IS_SEQUENCE>::object()
{
    typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
    return static_cast<Derived *>(this)->d_value.object();
}

// PRIVATE ACCESSORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_SEQUENCE>
const VALUE_TYPE&
TestTaggedValue_SequenceBase<TAG_TYPE, VALUE_TYPE, IS_SEQUENCE>::object() const
{
    typedef TestTaggedValue<TAG_TYPE, VALUE_TYPE> Derived;
    return static_cast<const Derived *>(this)->d_value.object();
}

// MANIPULATORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_SEQUENCE>
template <class MANIPULATOR>
int TestTaggedValue_SequenceBase<TAG_TYPE, VALUE_TYPE, IS_SEQUENCE>::
    manipulateAttribute(MANIPULATOR& manipulator, int attributeId)
{
    return bdlat_SequenceFunctions::manipulateAttribute(
        &this->object(), manipulator, attributeId);
}

template <class TAG_TYPE, class VALUE_TYPE, bool IS_SEQUENCE>
template <class MANIPULATOR>
int TestTaggedValue_SequenceBase<TAG_TYPE, VALUE_TYPE, IS_SEQUENCE>::
    manipulateAttribute(MANIPULATOR&  manipulator,
                        const char   *attributeName,
                        int           attributeNameLength)
{
    return bdlat_SequenceFunctions::manipulateAttribute(
        &this->object(), manipulator, attributeName, attributeNameLength);
}

template <class TAG_TYPE, class VALUE_TYPE, bool IS_SEQUENCE>
template <class MANIPULATOR>
int TestTaggedValue_SequenceBase<TAG_TYPE, VALUE_TYPE, IS_SEQUENCE>::
    manipulateAttributes(MANIPULATOR& manipulator)
{
    return bdlat_SequenceFunctions::manipulateAttributes(&this->object(),
                                                         manipulator);
}

// ACCESSORS
template <class TAG_TYPE, class VALUE_TYPE, bool IS_SEQUENCE>
template <class ACCESSOR>
int TestTaggedValue_SequenceBase<TAG_TYPE, VALUE_TYPE, IS_SEQUENCE>::
    accessAttribute(ACCESSOR& accessor, int attributeId) const
{
    return bdlat_SequenceFunctions::accessAttribute(
        this->object(), accessor, attributeId);
}

template <class TAG_TYPE, class VALUE_TYPE, bool IS_SEQUENCE>
template <class ACCESSOR>
int TestTaggedValue_SequenceBase<TAG_TYPE, VALUE_TYPE, IS_SEQUENCE>::
    accessAttribute(ACCESSOR&   accessor,
                    const char *attributeName,
                    int         attributeNameLength) const
{
    return bdlat_SequenceFunctions::accessAttribute(
        this->object(), accessor, attributeName, attributeNameLength);
}

template <class TAG_TYPE, class VALUE_TYPE, bool IS_SEQUENCE>
template <class ACCESSOR>
int TestTaggedValue_SequenceBase<TAG_TYPE, VALUE_TYPE, IS_SEQUENCE>::
    accessAttributes(ACCESSOR& accessor) const
{
    return bdlat_SequenceFunctions::accessAttributes(this->object(), accessor);
}

template <class TAG_TYPE, class VALUE_TYPE, bool IS_SEQUENCE>
bool
TestTaggedValue_SequenceBase<TAG_TYPE, VALUE_TYPE, IS_SEQUENCE>::hasAttribute(
                                                         int attributeId) const
{
    return bdlat_SequenceFunctions::hasAttribute(this->object(), attributeId);
}

template <class TAG_TYPE, class VALUE_TYPE, bool IS_SEQUENCE>
bool
TestTaggedValue_SequenceBase<TAG_TYPE, VALUE_TYPE, IS_SEQUENCE>::hasAttribute(
                                         const char *attributeName,
                                         int         attributeNameLength) const
{
    return bdlat_SequenceFunctions::hasAttribute(
        this->object(), attributeName, attributeNameLength);
}

                           // ---------------------
                           // class TestTaggedValue
                           // ---------------------

// CLASS METHODS
template <class TAG_TYPE, class VALUE_TYPE>
bool TestTaggedValue<TAG_TYPE, VALUE_TYPE>::areEqual(
                                                    const TestTaggedValue& lhs,
                                                    const TestTaggedValue& rhs)
{
    return lhs.d_value.object() == rhs.d_value.object();
}

// CREATORS
template <class TAG_TYPE, class VALUE_TYPE>
TestTaggedValue<TAG_TYPE, VALUE_TYPE>::TestTaggedValue()
: d_value(bslma::Default::allocator())
, d_allocator_p(bslma::Default::allocator())
{
}

template <class TAG_TYPE, class VALUE_TYPE>
TestTaggedValue<TAG_TYPE, VALUE_TYPE>::TestTaggedValue(
                                              bslma::Allocator *basicAllocator)
: d_value(bslma::Default::allocator(basicAllocator))
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class TAG_TYPE, class VALUE_TYPE>
TestTaggedValue<TAG_TYPE, VALUE_TYPE>::TestTaggedValue(
                                              const Value&      value,
                                              bslma::Allocator *basicAllocator)
: d_value(value, bslma::Default::allocator(basicAllocator))
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class TAG_TYPE, class VALUE_TYPE>
TestTaggedValue<TAG_TYPE, VALUE_TYPE>::TestTaggedValue(
                                        const TestTaggedValue&  original,
                                        bslma::Allocator       *basicAllocator)
: d_value(original.d_value.object(), bslma::Default::allocator(basicAllocator))
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

// MANIPULATORS
template <class TAG_TYPE, class VALUE_TYPE>
TestTaggedValue<TAG_TYPE, VALUE_TYPE>&
TestTaggedValue<TAG_TYPE, VALUE_TYPE>::operator=(
                                               const TestTaggedValue& original)
{
    d_value.object() = original.d_value.object();
    return *this;
}

template <class TAG_TYPE, class VALUE_TYPE>
int TestTaggedValue<TAG_TYPE, VALUE_TYPE>::assign(const VALUE_TYPE& rhs)
{
    return bdlat_ValueTypeFunctions::assign(&d_value.object(), rhs);
}

template <class TAG_TYPE, class VALUE_TYPE>
int TestTaggedValue<TAG_TYPE, VALUE_TYPE>::assign(const TestTaggedValue& rhs)
{
    return bdlat_ValueTypeFunctions::assign(&d_value.object(),
                                            rhs.d_value.object());
}

template <class TAG_TYPE, class VALUE_TYPE>
void TestTaggedValue<TAG_TYPE, VALUE_TYPE>::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_value.object());
}

// ACCESSORS
template <class TAG_TYPE, class VALUE_TYPE>
const char *TestTaggedValue<TAG_TYPE, VALUE_TYPE>::className() const
{
    return "MyTaggedValue";
}

// FREE FUNCTIONS
template <class TAG_TYPE, class VALUE_TYPE>
bool operator==(const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& lhs,
                const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& rhs)
{
    return TestTaggedValue<TAG_TYPE, VALUE_TYPE>::areEqual(lhs, rhs);
}

template <class TAG_TYPE, class VALUE_TYPE>
bool operator!=(const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& lhs,
                const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& rhs)
{
    return TestTaggedValue<TAG_TYPE, VALUE_TYPE>::areEqual(lhs, rhs);
}

// TRAITS
template <class TAG_TYPE, class VALUE_TYPE, class MANIPULATOR>
int bdlat_arrayManipulateElement(
                            TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                            MANIPULATOR&                           manipulator,
                            int                                    index)
{
    return object->manipulateElement(manipulator, index);
}

template <class TAG_TYPE, class VALUE_TYPE>
void bdlat_arrayResize(TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                       int                                    newSize)
{
    object->resize(newSize);
}

template <class TAG_TYPE, class VALUE_TYPE, class ACCESSOR>
int bdlat_arrayAccessElement(
                         const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                         ACCESSOR&                                    accessor,
                         int                                          index)
{
    return object->accessElement(accessor, index);
}

template <class TAG_TYPE, class VALUE_TYPE>
bsl::size_t bdlat_arraySize(
                           const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object)
{
    return object.size();
}

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_choiceMakeSelection(
                            TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                            int                                    selectionId)
{
    return object->makeSelection(selectionId);
}

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_choiceMakeSelection(
                    TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                    const char                            *selectionName,
                    int                                    selectionNameLength)
{
    return object->makeSelection(selectionName, selectionNameLength);
}

template <class TAG_TYPE, class VALUE_TYPE, class MANIPULATOR>
int bdlat_choiceManipulateSelection(
                            TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                            MANIPULATOR&                           manipulator)
{
    return object->manipulateSelection(manipulator);
}

template <class TAG_TYPE, class VALUE_TYPE, class ACCESSOR>
int bdlat_choiceAccessSelection(
                         const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                         ACCESSOR&                                    accessor)
{
    return object.accessSelection(accessor);
}

template <class TAG_TYPE, class VALUE_TYPE>
bool bdlat_choiceHasSelection(
                      const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                      int                                          selectionId)
{
    return object.hasSelection(selectionId);
}

template <class TAG_TYPE, class VALUE_TYPE>
bool bdlat_choiceHasSelection(
             const TestTaggedValue<TAG_TYPE, VALUE_TYPE>&  object,
             const char                                   *selectionName,
             int                                           selectionNameLength)
{
    return object.hasSelection(selectionName, selectionNameLength);
}

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_choiceSelectionId(
                           const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object)
{
    return object.selectionId();
}

template <class TAG_TYPE, class VALUE_TYPE, class BASE_TYPE>
int bdlat_customizedTypeConvertFromBaseType(
                                 TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                                 const BASE_TYPE&                       value)
{
    return object->convertFromBaseType(value);
}

template <class TAG_TYPE, class VALUE_TYPE>
const typename TestTaggedValue<TAG_TYPE, VALUE_TYPE>::BaseType&
bdlat_customizedTypeConvertToBaseType(
                           const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object)
{
    return object.convertToBaseType();
}

template <class TAG_TYPE, class VALUE_TYPE>
bdlat_TypeCategory::Value bdlat_typeCategorySelect(
                           const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object)
{
    return object.select();
}

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_enumFromInt(TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object, int value)
{
    return object->fromInt(value);
}

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_enumFromString(TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                         const char                            *string,
                         int                                    stringLength)
{
    return object->fromString(string, stringLength);
}

template <class TAG_TYPE, class VALUE_TYPE>
void bdlat_enumToInt(int                                          *result,
                     const TestTaggedValue<TAG_TYPE, VALUE_TYPE>&  object)
{
    return object.toInt(result);
}

template <class TAG_TYPE, class VALUE_TYPE>
void bdlat_enumToString(bsl::string                                  *result,
                        const TestTaggedValue<TAG_TYPE, VALUE_TYPE>&  object)
{
    return object.toString(result);
}

template <class TAG_TYPE, class VALUE_TYPE>
void bdlat_nullableValueMakeValue(
                                 TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object)
{
    object->makeValue();
}

template <class TAG_TYPE, class VALUE_TYPE, class MANIPULATOR>
int bdlat_nullableValueManipulateValue(
                            TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                            MANIPULATOR&                           manipulator)
{
    return object->manipulateValue(manipulator);
}

template <class TAG_TYPE, class VALUE_TYPE, class ACCESSOR>
int bdlat_nullableValueAccessValue(
                         const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                         ACCESSOR&                                    accessor)
{
    return object.accessValue(accessor);
}

template <class TAG_TYPE, class VALUE_TYPE>
bool bdlat_nullableValueIsNull(
                           const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object)
{
    return object.isNull();
}

template <class TAG_TYPE, class VALUE_TYPE, class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(
                            TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                            MANIPULATOR&                           manipulator,
                            int                                    attributeId)
{
    return object->manipulateAttribute(manipulator, attributeId);
}

template <class TAG_TYPE, class VALUE_TYPE, class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(
                    TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                    MANIPULATOR&                           manipulator,
                    const char                            *attributeName,
                    int                                    attributeNameLength)
{
    return object->manipulateAttribute(
        manipulator, attributeName, attributeNameLength);
}

template <class TAG_TYPE, class VALUE_TYPE, class MANIPULATOR>
int bdlat_sequenceManipulateAttributes(
                            TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                            MANIPULATOR&                           manipulator)
{
    return object->manipulateAttributes(manipulator);
}

template <class TAG_TYPE, class VALUE_TYPE, class ACCESSOR>
int bdlat_sequenceAccessAttribute(
                      const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                      ACCESSOR&                                    accessor,
                      int                                          attributeId)
{
    return object.accessAttribute(accessor, attributeId);
}

template <class TAG_TYPE, class VALUE_TYPE, class ACCESSOR>
int bdlat_sequenceAccessAttribute(
             const TestTaggedValue<TAG_TYPE, VALUE_TYPE>&  object,
             ACCESSOR&                                     accessor,
             const char                                   *attributeName,
             int                                           attributeNameLength)
{
    return object.accessAttribute(
        accessor, attributeName, attributeNameLength);
}

template <class TAG_TYPE, class VALUE_TYPE, class ACCESSOR>
int bdlat_sequenceAccessAttributes(
                         const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                         ACCESSOR&                                    accessor)
{
    return object.accessAttributes(accessor);
}

template <class TAG_TYPE, class VALUE_TYPE>
bool bdlat_sequenceHasAttribute(
                      const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object,
                      int                                          attributeId)
{
    return object.hasAttribute(attributeId);
}

template <class TAG_TYPE, class VALUE_TYPE>
bool bdlat_sequenceHasAttribute(
             const TestTaggedValue<TAG_TYPE, VALUE_TYPE>&  object,
             const char                                   *attributeName,
             int                                           attributeNameLength)
{
    return object.hasAttribute(attributeName, attributeNameLength);
}

template <class TAG_TYPE, class VALUE_TYPE>
const char *bdlat_TypeName_className(
                           const TestTaggedValue<TAG_TYPE, VALUE_TYPE>& object)
{
    return object.className();
}

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_valueTypeAssign(TestTaggedValue<TAG_TYPE, VALUE_TYPE> *object,
                          const VALUE_TYPE&                      value)
{
    return object->assign(value);
}

template <class TAG_TYPE, class VALUE_TYPE>
int bdlat_valueTypeAssign(TestTaggedValue<TAG_TYPE, VALUE_TYPE>        *object,
                          const TestTaggedValue<TAG_TYPE, VALUE_TYPE>&  value)
{
    return object->assign(value);
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_TESTTAGGEDVALUE

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
