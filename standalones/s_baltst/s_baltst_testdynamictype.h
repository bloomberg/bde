// s_baltst_testdynamictype.h                                         -*-C++-*-
#ifndef INCLUDED_S_BALTST_TESTDYNAMICTYPE
#define INCLUDED_S_BALTST_TESTDYNAMICTYPE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a test implementation of a 'bdlat' "dynamic type".
//
//@CLASSES:
//  s_baltst::TestDynamicType: test implementation of 'bdlat' "dynamic type"

#include <bdlat_arrayfunctions.h>
#include <bdlat_choicefunctions.h>
#include <bdlat_customizedtypefunctions.h>
#include <bdlat_enumfunctions.h>
#include <bdlat_nullablevaluefunctions.h>
#include <bdlat_valuetypefunctions.h>
#include <bdlat_sequencefunctions.h>
#include <bdlat_typecategory.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bslalg_constructorproxy.h>

#include <s_baltst_testnilvalue.h>

namespace BloombergLP {
namespace s_baltst {

                     // ==================================
                     // class TestDynamicType_ArrayImpUtil
                     // ==================================

template <class VALUE_TYPE,
          bool IS_ARRAY =
              bdlat_TypeCategory::e_ARRAY_CATEGORY ==
              static_cast<bdlat_TypeCategory::Value>(
                  bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION)>
struct TestDynamicType_ArrayImpUtil {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by 'TestDynamicType' to implement the 'bdlat' 'Array' concept for
    // 'VALUE_TYPE' template parameters that themselves satisfy the 'bdlat'
    // 'Array' concept.

    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS METHODS
    template <class ACCESSOR>
    static int accessElement(const Value& value, ACCESSOR& accessor, int index);

    template <class MANIPULATOR>
    static int manipulateElement(Value        *value,
                                 MANIPULATOR&  manipulator,
                                 int           index);

    static void resize(Value *value, int newSize);

    static bsl::size_t size(const Value& value);
};

template <class VALUE_TYPE>
struct TestDynamicType_ArrayImpUtil<VALUE_TYPE, false> {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by 'TestDynamicType' to provide a stub implementation of the 'bdlat'
    // 'Array' concept for 'VALUE_TYPE' template parameters that do not satisfy
    // the 'bdlat' 'Array' concept.

    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS METHODS
    template <class ACCESSOR>
    static int accessElement(const Value&, ACCESSOR, int);

    template <class MANIPULATOR>
    static int manipulateElement(Value *, MANIPULATOR&, int);

    static void resize(Value *, int);

    static bsl::size_t size(const Value&);
};

                    // ===================================
                    // class TestDynamicType_ChoiceImpUtil
                    // ===================================

template <class VALUE_TYPE,
          bool IS_CHOICE =
              bdlat_TypeCategory::e_CHOICE_CATEGORY ==
              static_cast<bdlat_TypeCategory::Value>(
                  bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION)>
struct TestDynamicType_ChoiceImpUtil {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by 'TestDynamicType' to implement the 'bdlat' 'Choice' concept for
    // 'VALUE_TYPE' template parameters that themselves satisfy the 'bdlat'
    // 'Choice' concept.

    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS METHODS
    template <class ACCESSOR>
    static int accessSelection(const Value& value, ACCESSOR& accessor);

    static bool hasSelection(const Value& value, int selectionId);

    static bool hasSelection(const Value&  value,
                             const char   *selectionName,
                             int           selectionNameLength);

    static int makeSelection(Value *value, int selectionId);

    static int makeSelection(Value      *value,
                             const char *selectionName,
                             int         selectionNameLength);

    template <class MANIPULATOR>
    static int manipulateSelection(Value *value, MANIPULATOR& manipulator);

    static int selectionId(const Value& value);
};

template <class VALUE_TYPE>
struct TestDynamicType_ChoiceImpUtil<VALUE_TYPE, false> {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by 'TestDynamicType' to provide a stub implementation of the 'bdlat'
    // 'Choice' concept for 'VALUE_TYPE' template parameters that do not
    // satisfy the 'bdlat' 'Choice' concept.

    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS METHODS
    template <class ACCESSOR>
    static int accessSelection(const Value&, ACCESSOR&);

    static bool hasSelection(const Value&, int);

    static bool hasSelection(const Value&, const char *, int);

    static int makeSelection(Value *, int);

    static int makeSelection(Value *, const char *, int);

    template <class MANIPULATOR>
    static int manipulateSelection(Value *, MANIPULATOR&);

    static int selectionId(const Value&);
};

                // ===========================================
                // class TestDynamicType_CustomizedTypeImpUtil
                // ===========================================

template <class VALUE_TYPE,
          bool IS_CUSTOMIZED_TYPE =
              bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY ==
              static_cast<bdlat_TypeCategory::Value>(
                  bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION)>
struct TestDynamicType_CustomizedTypeImpUtil {
    // This utility 'struct' provides a namespace for a suite of utility
    // functions used by 'TestDynamicType' to implement the 'bdlat'
    // 'CustomizedType' concept for 'VALUE_TYPE' template parameters that
    // themselves satisfy the 'bdlat' 'CustomizedType' concept.

    // TYPES
    typedef VALUE_TYPE Value;
    typedef
        typename bdlat_CustomizedTypeFunctions::template BaseType<Value>::Type
            BaseType;

    // CLASS METHODS
    template <class BASE_TYPE>
    static int convertFromBaseType(Value *value, const BASE_TYPE& object);

    static const BaseType& convertToBaseType(const Value& value);
};

template <class VALUE_TYPE>
struct TestDynamicType_CustomizedTypeImpUtil<VALUE_TYPE, false> {
    // This utility 'struct' provides a namespace for a suite of utility
    // functions used by 'TestDynamicType' to provide a stub implementation of
    // the 'bdlat' 'CustomizedType' concept for 'VALUE_TYPE' template
    // parameters that do not satisfy the 'bdlat' 'CustomizedType' concept.

    // TYPES
    typedef VALUE_TYPE Value;
    typedef TestNilValue BaseType;

    // CLASS METHODS
    template <class BASE_TYPE>
    static int convertFromBaseType(Value *, const BASE_TYPE&);

    static const BaseType& convertToBaseType(const Value&);
};

                  // ========================================
                  // class TestDynamicType_EnumerationImpUtil
                  // ========================================

template <class VALUE_TYPE,
          bool IS_ENUMERATION =
              bdlat_TypeCategory::e_ENUMERATION_CATEGORY ==
              static_cast<bdlat_TypeCategory::Value>(
                  bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION)>
struct TestDynamicType_EnumerationImpUtil {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by 'TestDynamicType' to implement the 'bdlat' 'Enumeration' concept for
    // 'VALUE_TYPE' template parameters that themselves satisfy the 'bdlat'
    // 'Enumeration' concept.

    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS METHODS
    static int fromInt(Value *value, int integer);

    static int fromString(Value *value, const char *string, int stringLength);

    static void toInt(int *result, const Value& value);

    static void toString(bsl::string *result, const Value& value);
};

template <class VALUE_TYPE>
struct TestDynamicType_EnumerationImpUtil<VALUE_TYPE, false> {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by 'TestDynamicType' to provide a stub implementation of the 'bdlat'
    // 'Enumeration' concept for 'VALUE_TYPE' template parameters that do not
    // satisfy the 'bdlat' 'Enumeration' concept.

    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS METHODS
    static int fromInt(Value *, int);

    static int fromString(Value *, const char *, int);

    static void toInt(int *, const Value&);

    static void toString(bsl::string *, const Value&);
};

                 // ==========================================
                 // class TestDynamicType_NullableValueImpUtil
                 // ==========================================

template <class VALUE_TYPE,
          bool IS_NULLABLE_VALUE =
              bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY ==
              static_cast<bdlat_TypeCategory::Value>(
                  bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION)>
struct TestDynamicType_NullableValueImpUtil {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by 'TestDynamicType' to implement the 'bdlat' 'NullableValue' concept
    // for 'VALUE_TYPE' template parameters that themselves satisfy the 'bdlat'
    // 'NullableValue' concept.

    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS METHODS
    static void makeValue(Value *value);

    template <class MANIPULATOR>
    static int manipulateValue(Value *value, MANIPULATOR& manipulator);

    template <class ACCESSOR>
    static int accessValue(const Value& value, ACCESSOR& accessor);

    static bool isNull(const Value& value);
};

template <class VALUE_TYPE>
struct TestDynamicType_NullableValueImpUtil<VALUE_TYPE, false> {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by 'TestDynamicType' to provide a stub implementation of the 'bdlat'
    // 'NullableValue' concept for 'VALUE_TYPE' template parameters that do not
    // satisfy the 'bdlat' 'NullableValue' concept.

    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS METHODS
    static void makeValue(Value *);

    template <class MANIPULATOR>
    static int manipulateValue(Value *, MANIPULATOR&);

    template <class ACCESSOR>
    static int accessValue(const Value&, ACCESSOR&);

    static bool isNull(const Value&);
};

                   // =====================================
                   // class TestDynamicType_SequenceImpUtil
                   // =====================================

template <class VALUE_TYPE,
          bool IS_SEQUENCE =
              bdlat_TypeCategory::e_SEQUENCE_CATEGORY ==
              static_cast<bdlat_TypeCategory::Value>(
                  bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION)>
struct TestDynamicType_SequenceImpUtil {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by 'TestDynamicType' to implement the 'bdlat' 'Sequence' concept for
    // 'VALUE_TYPE' template parameters that themselves satisfy the 'bdlat'
    // 'Sequence' concept.

    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS METHODS
    template <class ACCESSOR>
    static int accessAttribute(const Value&  value,
                               ACCESSOR&     accessor,
                               const char   *attributeName,
                               int           attributeNameLength);

    template <class ACCESSOR>
    static int accessAttribute(const Value& value,
                               ACCESSOR&    accessor,
                               int          attributeId);

    template <class ACCESSOR>
    static int accessAttributes(const Value& value, ACCESSOR& accessor);

    static bool hasAttribute(const Value&  value,
                             const char   *attributeName,
                             int           attributeNameLength);

    static bool hasAttribute(const Value& value, int attributeId);

    template <class MANIPULATOR>
    static int manipulateAttribute(Value        *value,
                                   MANIPULATOR&  manipulator,
                                   const char   *attributeName,
                                   int           attributeNameLength);

    template <class MANIPULATOR>
    static int manipulateAttribute(Value        *value,
                                   MANIPULATOR&  manipulator,
                                   int           attributeId);

    template <class MANIPULATOR>
    static int manipulateAttributes(Value *value, MANIPULATOR& manipulator);
};

template <class VALUE_TYPE>
struct TestDynamicType_SequenceImpUtil<VALUE_TYPE, false> {
    // This utility 'struct' provides a namespace for a suite of functions used
    // by 'TestDynamicType' to provide stub implementations of the 'bdlat'
    // 'Sequence' concept for 'VALUE_TYPE' template parameters that do not
    // satisfy the 'bdlat' 'Sequence' concept.

    // TYPES
    typedef VALUE_TYPE Value;

    // CLASS METHODS
    template <class ACCESSOR>
    static int accessAttribute(const Value&, ACCESSOR&, const char *, int);

    template <class ACCESSOR>
    static int accessAttribute(const Value&, ACCESSOR&, int);

    template <class ACCESSOR>
    static int accessAttributes(const Value&, ACCESSOR&);

    static bool hasAttribute(const Value&, const char *, int);

    static bool hasAttribute(const Value&, int);

    template <class MANIPULATOR>
    static int manipulateAttribute(Value *, MANIPULATOR&, const char *, int);

    template <class MANIPULATOR>
    static int manipulateAttribute(Value *, MANIPULATOR&, int);

    template <class MANIPULATOR>
    static int manipulateAttributes(Value *, MANIPULATOR&);
};

                           // =====================
                           // class TestDynamicType
                           // =====================

template <class VALUE_TYPE>
class TestDynamicType {
    // This in-core value-semantic class provides a basic implementation of the
    // 'bdlat' 'DynamicType' concept.  The template parameter 'VALUE_TYPE'
    // specifies the underlying value of this type, and further, specifies
    // which of the 'bdlat' attribute concepts this type implements.  This type
    // implements the same 'bdlat' concept as the 'VALUE_TYPE' through the
    // 'bdlat' 'DynamicType' interface.  The program is ill-formed unless
    // 'VALUE_TYPE' meets the requirements of at least one of the 'bdlat'
    // 'Array', 'Choice', 'CustomizedType', 'Enumeration', 'NullableValue', or
    // 'Sequence' concepts.

  public:
    // TYPES
    typedef VALUE_TYPE Value;
        // An alias to the type that defines the underlying value of this
        // object.

  private:
    // PRIVATE TYPES
    typedef TestDynamicType_ArrayImpUtil<VALUE_TYPE>  ArrayImpUtil;
    typedef TestDynamicType_ChoiceImpUtil<VALUE_TYPE> ChoiceImpUtil;
    typedef TestDynamicType_CustomizedTypeImpUtil<VALUE_TYPE>
                                                           CustomizedTypeImpUtil;
    typedef TestDynamicType_EnumerationImpUtil<VALUE_TYPE> EnumerationImpUtil;
    typedef TestDynamicType_NullableValueImpUtil<VALUE_TYPE>
                                                        NullableValueImpUtil;
    typedef TestDynamicType_SequenceImpUtil<VALUE_TYPE> SequenceImpUtil;

    // PRIVATE CLASS DATA
    enum {
        e_ARRAY           = bdlat_TypeCategory::e_ARRAY_CATEGORY,
        e_CHOICE          = bdlat_TypeCategory::e_CHOICE_CATEGORY,
        e_CUSTOMIZED_TYPE = bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY,
        e_ENUMERATION     = bdlat_TypeCategory::e_ENUMERATION_CATEGORY,
        e_NULLABLE_VALUE  = bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY,
        e_SEQUENCE        = bdlat_TypeCategory::e_SEQUENCE_CATEGORY
    };

    // PRIVATE CLASS FUNCTIONS
    static bool valueHasCategory(int category);

    // DATA
    bslalg::ConstructorProxy<Value> d_value;  // underlying value
    bslma::Allocator *d_allocator_p;  // memory supply (held, not owned)

  public:
    // CLASS METHODS
    static bool areEqual(const TestDynamicType& lhs,
                         const TestDynamicType& rhs);

    // CREATORS
    TestDynamicType();

    explicit TestDynamicType(bslma::Allocator *basicAllocator);

    TestDynamicType(const Value& value, bslma::Allocator *basicAllocator = 0);

    TestDynamicType(const TestDynamicType&  original,
                    bslma::Allocator       *basicAllocator = 0);

    // MANIPULATORS
    TestDynamicType& operator=(const TestDynamicType& original);

    template <class MANIPULATOR>
    int arrayManipulateElement(MANIPULATOR& manipulator, int index);

    void arrayResize(int newSize);

    int choiceMakeSelection(int selectionId);

    int choiceMakeSelection(const char *selectionName,
                            int         selectionNameLength);

    template <class MANIPULATOR>
    int choiceManipulateSelection(MANIPULATOR& manipulator);

    template <class BASE_TYPE>
    int customizedTypeConvertFromBaseType(const BASE_TYPE& base);

    int enumerationFromInt(int number);

    int enumerationFromString(const char *string, int stringLength);

    void nullableValueMakeValue();

    template <class MANIPULATOR>
    int nullableValueManipulateValue(MANIPULATOR& manipulator);

    template <class MANIPULATOR>
    int sequenceManipulateAttribute(MANIPULATOR&  manipulator,
                                    const char   *attributeName,
                                    int           attributeNameLength);

    template <class MANIPULATOR>
    int sequenceManipulateAttribute(MANIPULATOR& manipulator, int attributeId);

    template <class MANIPULATOR>
    int sequenceManipulateAttributes(MANIPULATOR& manipulator);

    void reset();

    void setValue(const Value& value);

    // ACCESSORS
    const char *className() const;

    template <class ACCESSOR>
    int arrayAccessElement(ACCESSOR& accessor, int index) const;

    bsl::size_t arraySize() const;

    template <class ACCESSOR>
    int choiceAccessSelection(ACCESSOR& accessor) const;

    bool choiceHasSelection(int selectionId) const;

    bool choiceHasSelection(const char *selectionName,
                            int         selectionNameLength) const;

    int choiceSelectionId() const;

    const typename CustomizedTypeImpUtil::BaseType&
    customizedTypeConvertToBaseType() const;

    void enumerationToInt(int *result) const;

    void enumerationToString(bsl::string *result) const;

    template <class ACCESSOR>
    int nullableValueAccessValue(ACCESSOR& accessor) const;

    bool nullableValueIsNull() const;

    template <class ACCESSOR>
    int sequenceAccessAttribute(ACCESSOR&   accessor,
                                const char *attributeName,
                                int         attributeNameLength) const;

    template <class ACCESSOR>
    int sequenceAccessAttribute(ACCESSOR& accessor, int attributeId) const;

    template <class ACCESSOR>
    int sequenceAccessAttributes(ACCESSOR& accessor) const;

    bool sequenceHasAttribute(const char *attributeName,
                              int         attributeNameLength) const;

    bool sequenceHasAttribute(int attributeId) const;

    bdlat_TypeCategory::Value select() const;

    const Value& value() const;
};

// FREE OPERATORS
template <class VALUE_TYPE>
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const TestDynamicType<VALUE_TYPE>& object);

template <class VALUE_TYPE>
bool operator==(const TestDynamicType<VALUE_TYPE>& lhs,
                const TestDynamicType<VALUE_TYPE>& rhs);


template <class VALUE_TYPE>
bool operator!=(const TestDynamicType<VALUE_TYPE>& lhs,
                const TestDynamicType<VALUE_TYPE>& rhs);

// TRAITS
template <class VALUE_TYPE>
const char *bdlat_TypeName_className(
                                    const TestDynamicType<VALUE_TYPE>& object);

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_arrayManipulateElement(TestDynamicType<VALUE_TYPE> *object,
                                 MANIPULATOR&                 manipulator,
                                 int                          index);

template <class VALUE_TYPE>
void bdlat_arrayResize(TestDynamicType<VALUE_TYPE> *object, int newSize);

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_arrayAccessElement(const TestDynamicType<VALUE_TYPE>& object,
                             ACCESSOR&                          accessor,
                             int                                index);

template <class VALUE_TYPE>
bsl::size_t bdlat_arraySize(const TestDynamicType<VALUE_TYPE>& object);

template <class VALUE_TYPE>
int bdlat_choiceMakeSelection(TestDynamicType<VALUE_TYPE> *object,
                              int                          selectionId);

template <class VALUE_TYPE>
int bdlat_choiceMakeSelection(
                             TestDynamicType<VALUE_TYPE> *object,
                             const char                  *selectionName,
                             int                          selectionNameLength);

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_choiceManipulateSelection(TestDynamicType<VALUE_TYPE> *object,
                                    MANIPULATOR&                 manipulator);

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_choiceAccessSelection(const TestDynamicType<VALUE_TYPE>& object,
                                ACCESSOR&                          accessor);

template <class VALUE_TYPE>
bool bdlat_choiceHasSelection(const TestDynamicType<VALUE_TYPE>& object,
                              int                                selectionId);

template <class VALUE_TYPE>
bool bdlat_choiceHasSelection(
                      const TestDynamicType<VALUE_TYPE>&  object,
                      const char                         *selectionName,
                      int                                 selectionNameLength);

template <class VALUE_TYPE, class BASE_TYPE>
int bdlat_customizedTypeConvertFromBaseType(
                                           TestDynamicType<VALUE_TYPE> *object,
                                           const BASE_TYPE&             value);

template <class VALUE_TYPE>
const typename TestDynamicType<VALUE_TYPE>::BaseType&
bdlat_customizedTypeConvertToBaseType(
                                    const TestDynamicType<VALUE_TYPE>& object);

template <class VALUE_TYPE>
int bdlat_enumFromInt(TestDynamicType<VALUE_TYPE> *object, int number);

template <class VALUE_TYPE>
int bdlat_enumFromString(TestDynamicType<VALUE_TYPE> *object,
                         const char                  *string,
                         int                          stringLength);

template <class VALUE_TYPE>
void bdlat_enumToInt(int *result, const TestDynamicType<VALUE_TYPE>& object);

template <class VALUE_TYPE>
void bdlat_enumToString(bsl::string                        *result,
                        const TestDynamicType<VALUE_TYPE>&  object);

template <class VALUE_TYPE>
void bdlat_nullableValueMakeValue(TestDynamicType<VALUE_TYPE> *object);

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_nullableValueManipulateValue(
                                     TestDynamicType<VALUE_TYPE> *object,
                                     MANIPULATOR&                 manipulator);

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_nullableValueAccessValue(
                                  const TestDynamicType<VALUE_TYPE>& object,
                                  ACCESSOR&                          accessor);

template <class VALUE_TYPE>
bool bdlat_nullableValueIsNull(const TestDynamicType<VALUE_TYPE>& object);

template <class VALUE_TYPE>
int bdlat_choiceSelectionId(const TestDynamicType<VALUE_TYPE>& object);

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_sequenceAccessAttribute(
                      const TestDynamicType<VALUE_TYPE>&  object,
                      ACCESSOR&                           accessor,
                      const char                         *attributeName,
                      int                                 attributeNameLength);

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_sequenceAccessAttribute(
                                const TestDynamicType<VALUE_TYPE>& object,
                                ACCESSOR&                          accessor,
                                int                                attributeId);

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_sequenceAccessAttributes(
                                  const TestDynamicType<VALUE_TYPE>& object,
                                  ACCESSOR&                          accessor);

template <class VALUE_TYPE>
int bdlat_sequenceHasAttribute(
                      const TestDynamicType<VALUE_TYPE>&  object,
                      const char                         *attributeName,
                      int                                 attributeNameLength);

template <class VALUE_TYPE>
int bdlat_sequenceHasAttribute(const TestDynamicType<VALUE_TYPE>& object,
                               int                                attributeId);

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(
                             TestDynamicType<VALUE_TYPE> *object,
                             MANIPULATOR&                 manipulator,
                             const char                  *attributeName,
                             int                          attributeNameLength);

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(
                                     TestDynamicType<VALUE_TYPE> *object,
                                     MANIPULATOR&                 manipulator,
                                     int                          attributeId);

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_sequenceManipulateAttributes(
                                     TestDynamicType<VALUE_TYPE> *object,
                                     MANIPULATOR&                 manipulator);

template <class VALUE_TYPE>
bdlat_TypeCategory::Value bdlat_typeCategorySelect(
                                    const TestDynamicType<VALUE_TYPE>& object);

}  // close package namespace

template <class VALUE_TYPE>
struct bdlat_TypeCategoryDeclareDynamic<
    s_baltst::TestDynamicType<VALUE_TYPE> > {
    enum { VALUE = 1 };
};

namespace s_baltst {

template <class VALUE_TYPE,
          bool IS_ARRAY =
              static_cast<bdlat_TypeCategory::Value>(
                  bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
              bdlat_TypeCategory::e_ARRAY_CATEGORY>
struct TestDynamicType_ElementTypeImpl {
    typedef typename bdlat_ArrayFunctions::ElementType<VALUE_TYPE>::Type Type;
};

template <class VALUE_TYPE>
struct TestDynamicType_ElementTypeImpl<VALUE_TYPE, false> {
    typedef struct {
    } Type;
};

}  // close package namespace

namespace bdlat_ArrayFunctions {

template <class VALUE_TYPE>
struct ElementType<s_baltst::TestDynamicType<VALUE_TYPE> > {
    typedef
        typename s_baltst::TestDynamicType_ElementTypeImpl<VALUE_TYPE>::Type
            Type;
};

template <class VALUE_TYPE>
struct IsArray<s_baltst::TestDynamicType<VALUE_TYPE> > {
    enum { VALUE = IsArray<VALUE_TYPE>::VALUE };
};

}  // close bdlat_ArrayFunctions namespace

template <class VALUE_TYPE>
struct bdlat_IsBasicChoice<s_baltst::TestDynamicType<VALUE_TYPE> >
: bdlat_IsBasicChoice<VALUE_TYPE> {
};

namespace bdlat_ChoiceFunctions {

template <class VALUE_TYPE>
struct IsChoice<s_baltst::TestDynamicType<VALUE_TYPE> > {
    enum { VALUE = IsChoice<VALUE_TYPE>::VALUE };
};

}  // close bdlat_ChoiceFunctions namespace

namespace s_baltst {

template <class VALUE_TYPE,
          bool IS_CUSTOMIZED_TYPE =
              static_cast<bdlat_TypeCategory::Value>(
                  bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
              bdlat_TypeCategory::e_CUSTOMIZED_TYPE_CATEGORY>
struct TestDynamicType_BaseTypeImpl {
    typedef typename bdlat_CustomizedTypeFunctions::BaseType<VALUE_TYPE>::Type
        Type;
};

template <class VALUE_TYPE>
struct TestDynamicType_BaseTypeImpl<VALUE_TYPE, false> {
    typedef struct {
    } Type;
};

}  // close package namespace

template <class VALUE_TYPE>
struct bdlat_IsBasicCustomizedType<s_baltst::TestDynamicType<VALUE_TYPE> >
: bdlat_IsBasicCustomizedType<VALUE_TYPE> {
};

namespace bdlat_CustomizedTypeFunctions {

template <class VALUE_TYPE>
struct BaseType<s_baltst::TestDynamicType<VALUE_TYPE> > {
    typedef
        typename s_baltst::TestDynamicType_BaseTypeImpl<VALUE_TYPE>::Type Type;
};

template <class VALUE_TYPE>
struct IsCustomizedType<s_baltst::TestDynamicType<VALUE_TYPE> > {
    enum { VALUE = IsCustomizedType<VALUE_TYPE>::VALUE };
};

}  // close bdlat_CustomizedTypeFunctions namespace

template <class VALUE_TYPE>
struct bdlat_IsBasicEnumeration<s_baltst::TestDynamicType<VALUE_TYPE> >
: bdlat_IsBasicEnumeration<VALUE_TYPE> {
};

namespace bdlat_EnumFunctions {

template <class VALUE_TYPE>
struct IsEnumeration<s_baltst::TestDynamicType<VALUE_TYPE> > {
    enum { VALUE = IsEnumeration<VALUE_TYPE>::VALUE };
};

}  // close bdlat_EnumFunctions namespace

namespace s_baltst {

template <class VALUE_TYPE,
          bool IS_NULLABLE_VALUE =
              static_cast<bdlat_TypeCategory::Value>(
                  bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION) ==
              bdlat_TypeCategory::e_NULLABLE_VALUE_CATEGORY>
struct TestDynamicType_ValueTypeImpl {
    typedef typename bdlat_NullableValueFunctions::ValueType<VALUE_TYPE>::Type
        Type;
};

template <class VALUE_TYPE>
struct TestDynamicType_ValueTypeImpl<VALUE_TYPE, false> {
    typedef struct {
    } Type;
};

}  // close package namespace

namespace bdlat_NullableValueFunctions {

template <class VALUE_TYPE>
struct ValueType<s_baltst::TestDynamicType<VALUE_TYPE> > {
    typedef typename s_baltst::TestDynamicType_ValueTypeImpl<VALUE_TYPE>::Type
        Type;
};

template <class VALUE_TYPE>
struct IsNullableValue<s_baltst::TestDynamicType<VALUE_TYPE> > {
    enum { VALUE = IsNullableValue<VALUE_TYPE>::VALUE };
};

}  // close bdlat_NullableValueFunctions namespace

template <class VALUE_TYPE>
struct bdlat_IsBasicSequence<s_baltst::TestDynamicType<VALUE_TYPE> >
: bdlat_IsBasicSequence<VALUE_TYPE> {
};

namespace bdlat_SequenceFunctions {

template <class VALUE_TYPE>
struct IsSequence<s_baltst::TestDynamicType<VALUE_TYPE> > {
    enum { VALUE = IsSequence<VALUE_TYPE>::VALUE };
};

}  // close bdlat_SequenceFunctions namespace

namespace s_baltst {

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

          // --------------------------------------------------------
          // class TestDynamicType_ArrayImpUtil<VALUE_TYPE, IS_ARRAY>
          // --------------------------------------------------------

// CLASS METHODS
template <class VALUE_TYPE, bool IS_ARRAY>
template <class ACCESSOR>
int TestDynamicType_ArrayImpUtil<VALUE_TYPE, IS_ARRAY>::accessElement(
                                                         const Value& value,
                                                         ACCESSOR&    accessor,
                                                         int          index)
{
    return bdlat_ArrayFunctions::accessElement(value, accessor, index);
}

template <class VALUE_TYPE, bool IS_ARRAY>
template <class MANIPULATOR>
int TestDynamicType_ArrayImpUtil<VALUE_TYPE, IS_ARRAY>::manipulateElement(
                                                     Value        *value,
                                                     MANIPULATOR&  manipulator,
                                                     int           index)
{
    return bdlat_ArrayFunctions::manipulateElement(value, manipulator, index);
}

template <class VALUE_TYPE, bool IS_ARRAY>
void TestDynamicType_ArrayImpUtil<VALUE_TYPE, IS_ARRAY>::resize(Value *value,
                                                                int    newSize)
{
    return bdlat_ArrayFunctions::resize(value, newSize);
}

template <class VALUE_TYPE, bool IS_ARRAY>
bsl::size_t TestDynamicType_ArrayImpUtil<VALUE_TYPE, IS_ARRAY>::size(
                                                            const Value& value)
{
    return bdlat_ArrayFunctions::size(value);
}

           // -----------------------------------------------------
           // class TestDynamicType_ArrayImpUtil<VALUE_TYPE, false>
           // -----------------------------------------------------

// CLASS METHODS
template <class VALUE_TYPE>
template <class ACCESSOR>
int TestDynamicType_ArrayImpUtil<VALUE_TYPE, false>::accessElement(
    const Value&,
    ACCESSOR,
    int)
{
    bsl::abort();
}

template <class VALUE_TYPE>
template <class MANIPULATOR>
int TestDynamicType_ArrayImpUtil<VALUE_TYPE, false>::manipulateElement(
    Value *,
    MANIPULATOR&,
    int)
{
    bsl::abort();
}

template <class VALUE_TYPE>
void TestDynamicType_ArrayImpUtil<VALUE_TYPE, false>::resize(Value *, int)
{
    bsl::abort();
}

template <class VALUE_TYPE>
bsl::size_t TestDynamicType_ArrayImpUtil<VALUE_TYPE, false>::size(const Value&)
{
    bsl::abort();
}

         // ----------------------------------------------------------
         // class TestDynamicType_ChoiceImpUtil<VALUE_TYPE, IS_CHOICE>
         // ----------------------------------------------------------

template <class VALUE_TYPE, bool IS_ARRAY>
template <class ACCESSOR>
int TestDynamicType_ChoiceImpUtil<VALUE_TYPE, IS_ARRAY>::accessSelection(
                                                         const Value& value,
                                                         ACCESSOR&    accessor)
{
    return bdlat_ChoiceFunctions::accessSelection(value, accessor);
}

template <class VALUE_TYPE, bool IS_ARRAY>
bool TestDynamicType_ChoiceImpUtil<VALUE_TYPE, IS_ARRAY>::hasSelection(
                                                      const Value& value,
                                                      int          selectionId)
{
    return bdlat_ChoiceFunctions::hasSelection(value, selectionId);
}

template <class VALUE_TYPE, bool IS_ARRAY>
bool TestDynamicType_ChoiceImpUtil<VALUE_TYPE, IS_ARRAY>::hasSelection(
                                             const Value&  value,
                                             const char   *selectionName,
                                             int           selectionNameLength)
{
    return bdlat_ChoiceFunctions::hasSelection(
        value, selectionName, selectionNameLength);
}

template <class VALUE_TYPE, bool IS_ARRAY>
int TestDynamicType_ChoiceImpUtil<VALUE_TYPE, IS_ARRAY>::makeSelection(
                                                            Value *value,
                                                            int    selectionId)
{
    return bdlat_ChoiceFunctions::makeSelection(value, selectionId);
}

template <class VALUE_TYPE, bool IS_ARRAY>
int TestDynamicType_ChoiceImpUtil<VALUE_TYPE, IS_ARRAY>::makeSelection(
                                               Value      *value,
                                               const char *selectionName,
                                               int         selectionNameLength)
{
    return bdlat_ChoiceFunctions::makeSelection(
        value, selectionName, selectionNameLength);
}

template <class VALUE_TYPE, bool IS_ARRAY>
template <class MANIPULATOR>
int TestDynamicType_ChoiceImpUtil<VALUE_TYPE, IS_ARRAY>::manipulateSelection(
                                                     Value        *value,
                                                     MANIPULATOR&  manipulator)
{
    return bdlat_ChoiceFunctions::manipulateSelection(value, manipulator);
}

template <class VALUE_TYPE, bool IS_ARRAY>
int TestDynamicType_ChoiceImpUtil<VALUE_TYPE, IS_ARRAY>::selectionId(
                                                            const Value& value)
{
    return bdlat_ChoiceFunctions::selectionId(value);
}

           // ------------------------------------------------------
           // class TestDynamicType_ChoiceImpUtil<VALUE_TYPE, false>
           // ------------------------------------------------------

// CLASS METHODS
template <class VALUE_TYPE>
template <class ACCESSOR>
int TestDynamicType_ChoiceImpUtil<VALUE_TYPE, false>::accessSelection(
    const Value&,
    ACCESSOR&)
{
    bsl::abort();
}

template <class VALUE_TYPE>
bool TestDynamicType_ChoiceImpUtil<VALUE_TYPE, false>::hasSelection(
    const Value&,
    int)
{
    bsl::abort();
}

template <class VALUE_TYPE>
bool TestDynamicType_ChoiceImpUtil<VALUE_TYPE, false>::hasSelection(
    const Value&,
    const char *,
    int)
{
    bsl::abort();
}

template <class VALUE_TYPE>
int TestDynamicType_ChoiceImpUtil<VALUE_TYPE, false>::makeSelection(Value *,
                                                                    int)
{
    bsl::abort();
}

template <class VALUE_TYPE>
int TestDynamicType_ChoiceImpUtil<VALUE_TYPE, false>::makeSelection(
    Value *,
    const char *,
    int)
{
    bsl::abort();
}

template <class VALUE_TYPE>
template <class MANIPULATOR>
int TestDynamicType_ChoiceImpUtil<VALUE_TYPE, false>::manipulateSelection(
    Value *,
    MANIPULATOR&)
{
    bsl::abort();
}

template <class VALUE_TYPE>
int TestDynamicType_ChoiceImpUtil<VALUE_TYPE, false>::selectionId(const Value&)
{
    bsl::abort();
}

// ---------------------------------------------------------------------------
// class TestDynamicType_CustomizedTypeImpUtil<VALUE_TYPE, IS_CUSTOMIZED_TYPE>
// ---------------------------------------------------------------------------

// CLASS METHODS
template <class VALUE_TYPE, bool IS_CUSTOMIZED_TYPE>
template <class BASE_TYPE>
int TestDynamicType_CustomizedTypeImpUtil<VALUE_TYPE, IS_CUSTOMIZED_TYPE>::
    convertFromBaseType(Value *value, const BASE_TYPE& object)
{
    return bdlat_CustomizedTypeFunctions::convertFromBaseType(value, object);
}

template <class VALUE_TYPE, bool IS_CUSTOMIZED_TYPE>
const typename bdlat_CustomizedTypeFunctions::template BaseType<
    VALUE_TYPE>::Type&
TestDynamicType_CustomizedTypeImpUtil<VALUE_TYPE, IS_CUSTOMIZED_TYPE>::
    convertToBaseType(const Value& value)
{
    return bdlat_CustomizedTypeFunctions::convertToBaseType(value);
}

       // --------------------------------------------------------------
       // class TestDynamicType_CustomizedTypeImpUtil<VALUE_TYPE, false>
       // --------------------------------------------------------------

// CLASS METHODS
template <class VALUE_TYPE>
template <class BASE_TYPE>
int TestDynamicType_CustomizedTypeImpUtil<VALUE_TYPE, false>::
    convertFromBaseType(Value *, const BASE_TYPE&)
{
    bsl::abort();
}

template <class VALUE_TYPE>
const TestNilValue&
TestDynamicType_CustomizedTypeImpUtil<VALUE_TYPE, false>::convertToBaseType(
    const Value&)
{
    bsl::abort();
}

       // -------------------------------------------------------------
       // class TestDynamicType_EnumerationImpUtil<VALUE_TYPE, IS_ENUM>
       // -------------------------------------------------------------

// CLASS METHODS
template <class VALUE_TYPE, bool IS_ENUMERATION>
int TestDynamicType_EnumerationImpUtil<VALUE_TYPE, IS_ENUMERATION>::fromInt(
                                                                Value *value,
                                                                int    integer)
{
    return bdlat_EnumFunctions::fromInt(value, integer);
}

template <class VALUE_TYPE, bool IS_ENUMERATION>
int TestDynamicType_EnumerationImpUtil<VALUE_TYPE, IS_ENUMERATION>::fromString(
                                                      Value      *value,
                                                      const char *string,
                                                      int         stringLength)
{
    return bdlat_EnumFunctions::fromString(value, string, stringLength);
}

template <class VALUE_TYPE, bool IS_ENUMERATION>
void TestDynamicType_EnumerationImpUtil<VALUE_TYPE, IS_ENUMERATION>::toInt(
                                                          int          *result,
                                                          const Value&  value)
{
    return bdlat_EnumFunctions::toInt(result, value);
}

template <class VALUE_TYPE, bool IS_ENUMERATION>
void TestDynamicType_EnumerationImpUtil<VALUE_TYPE, IS_ENUMERATION>::toString(
                                                          bsl::string  *result,
                                                          const Value&  value)
{
    return bdlat_EnumFunctions::toString(result, value);
}

        // -----------------------------------------------------------
        // class TestDynamicType_EnumerationImpUtil<VALUE_TYPE, false>
        // -----------------------------------------------------------

// CLASS METHODS
template <class VALUE_TYPE>
int TestDynamicType_EnumerationImpUtil<VALUE_TYPE, false>::fromInt(Value *,
                                                                   int)
{
    bsl::abort();
}

template <class VALUE_TYPE>
int TestDynamicType_EnumerationImpUtil<VALUE_TYPE, false>::fromString(
    Value *,
    const char *,
    int)
{
    bsl::abort();
}

template <class VALUE_TYPE>
void TestDynamicType_EnumerationImpUtil<VALUE_TYPE, false>::toInt(int *,
                                                                  const Value&)
{
    bsl::abort();
}

template <class VALUE_TYPE>
void TestDynamicType_EnumerationImpUtil<VALUE_TYPE, false>::toString(
    bsl::string *,
    const Value&)
{
    bsl::abort();
}

    // -------------------------------------------------------------------
    // class TestDynamicType_NullableValueImpUtil<VALUE_TYPE, IS_NULLABLE>
    // -------------------------------------------------------------------

// CLASS METHODS
template <class VALUE_TYPE, bool IS_NULLABLE_VALUE>
void
TestDynamicType_NullableValueImpUtil<VALUE_TYPE, IS_NULLABLE_VALUE>::makeValue(
                                                                  Value *value)
{
    bdlat_NullableValueFunctions::makeValue(value);
}

template <class VALUE_TYPE, bool IS_NULLABLE_VALUE>
template <class MANIPULATOR>
int TestDynamicType_NullableValueImpUtil<VALUE_TYPE, IS_NULLABLE_VALUE>::
    manipulateValue(Value *value, MANIPULATOR& manipulator)
{
    return bdlat_NullableValueFunctions::manipulateValue(value, manipulator);
}

template <class VALUE_TYPE, bool IS_NULLABLE_VALUE>
template <class ACCESSOR>
int TestDynamicType_NullableValueImpUtil<VALUE_TYPE, IS_NULLABLE_VALUE>::
    accessValue(const Value& value, ACCESSOR& accessor)
{
    return bdlat_NullableValueFunctions::accessValue(value, accessor);
}

template <class VALUE_TYPE, bool IS_NULLABLE_VALUE>
bool
TestDynamicType_NullableValueImpUtil<VALUE_TYPE, IS_NULLABLE_VALUE>::isNull(
                                                            const Value& value)
{
    return bdlat_NullableValueFunctions::isNull(value);
}

       // -------------------------------------------------------------
       // class TestDynamicType_NullableValueImpUtil<VALUE_TYPE, false>
       // -------------------------------------------------------------

// CLASS METHODS
template <class VALUE_TYPE>
void TestDynamicType_NullableValueImpUtil<VALUE_TYPE, false>::makeValue(
    Value *)
{
    bsl::abort();
}

template <class VALUE_TYPE>
template <class MANIPULATOR>
int TestDynamicType_NullableValueImpUtil<VALUE_TYPE, false>::manipulateValue(
    Value *,
    MANIPULATOR&)
{
    bsl::abort();
}

template <class VALUE_TYPE>
template <class ACCESSOR>
int TestDynamicType_NullableValueImpUtil<VALUE_TYPE, false>::accessValue(
    const Value&,
    ACCESSOR&)
{
    bsl::abort();
}

template <class VALUE_TYPE>
bool TestDynamicType_NullableValueImpUtil<VALUE_TYPE, false>::isNull(
    const Value&)
{
    bsl::abort();
}

       // --------------------------------------------------------------
       // class TestDynamicType_SequenceImpUtil<VALUE_TYPE, IS_SEQUENCE>
       // --------------------------------------------------------------

// CLASS METHODS
template <class VALUE_TYPE, bool IS_SEQUENCE>
template <class ACCESSOR>
int TestDynamicType_SequenceImpUtil<VALUE_TYPE, IS_SEQUENCE>::accessAttribute(
                                             const Value&  value,
                                             ACCESSOR&     accessor,
                                             const char   *attributeName,
                                             int           attributeNameLength)
{
    return bdlat_SequenceFunctions::accessAttribute(
        value, accessor, attributeName, attributeNameLength);
}

template <class VALUE_TYPE, bool IS_SEQUENCE>
template <class ACCESSOR>
int TestDynamicType_SequenceImpUtil<VALUE_TYPE, IS_SEQUENCE>::accessAttribute(
                                                      const Value& value,
                                                      ACCESSOR&    accessor,
                                                      int          attributeId)
{
    return bdlat_SequenceFunctions::accessAttribute(
        value, accessor, attributeId);
}

template <class VALUE_TYPE, bool IS_SEQUENCE>
template <class ACCESSOR>
int TestDynamicType_SequenceImpUtil<VALUE_TYPE, IS_SEQUENCE>::accessAttributes(
                                                         const Value& value,
                                                         ACCESSOR&    accessor)
{
    return bdlat_SequenceFunctions::accessAttributes(value, accessor);
}

template <class VALUE_TYPE, bool IS_SEQUENCE>
bool TestDynamicType_SequenceImpUtil<VALUE_TYPE, IS_SEQUENCE>::hasAttribute(
                                             const Value&  value,
                                             const char   *attributeName,
                                             int           attributeNameLength)
{
    return bdlat_SequenceFunctions::hasAttribute(
        value, attributeName, attributeNameLength);
}

template <class VALUE_TYPE, bool IS_SEQUENCE>
bool TestDynamicType_SequenceImpUtil<VALUE_TYPE, IS_SEQUENCE>::hasAttribute(
                                                      const Value& value,
                                                      int          attributeId)
{
    return bdlat_SequenceFunctions::hasAttribute(value, attributeId);
}

template <class VALUE_TYPE, bool IS_SEQUENCE>
template <class MANIPULATOR>
int
TestDynamicType_SequenceImpUtil<VALUE_TYPE, IS_SEQUENCE>::manipulateAttribute(
                                             Value        *value,
                                             MANIPULATOR&  manipulator,
                                             const char   *attributeName,
                                             int           attributeNameLength)
{
    return bdlat_SequenceFunctions::manipulateAttribute(
        value, manipulator, attributeName, attributeNameLength);
}

template <class VALUE_TYPE, bool IS_SEQUENCE>
template <class MANIPULATOR>
int
TestDynamicType_SequenceImpUtil<VALUE_TYPE, IS_SEQUENCE>::manipulateAttribute(
                                                     Value        *value,
                                                     MANIPULATOR&  manipulator,
                                                     int           attributeId)
{
    return bdlat_SequenceFunctions::manipulateAttribute(
        value, manipulator, attributeId);
}

template <class VALUE_TYPE, bool IS_SEQUENCE>
template <class MANIPULATOR>
int
TestDynamicType_SequenceImpUtil<VALUE_TYPE, IS_SEQUENCE>::manipulateAttributes(
                                                     Value        *value,
                                                     MANIPULATOR&  manipulator)
{
    return bdlat_SequenceFunctions::manipulateAttributes(value, manipulator);
}

          // --------------------------------------------------------
          // class TestDynamicType_SequenceImpUtil<VALUE_TYPE, false>
          // --------------------------------------------------------

// CLASS METHODS
template <class VALUE_TYPE>
template <class ACCESSOR>
int TestDynamicType_SequenceImpUtil<VALUE_TYPE, false>::accessAttribute(
    const Value&,
    ACCESSOR&,
    const char *,
    int)
{
    bsl::abort();
}

template <class VALUE_TYPE>
template <class ACCESSOR>
int TestDynamicType_SequenceImpUtil<VALUE_TYPE, false>::accessAttribute(
    const Value&,
    ACCESSOR&,
    int)
{
    bsl::abort();
}

template <class VALUE_TYPE>
template <class ACCESSOR>
int TestDynamicType_SequenceImpUtil<VALUE_TYPE, false>::accessAttributes(
    const Value&,
    ACCESSOR&)
{
    bsl::abort();
}

template <class VALUE_TYPE>
bool TestDynamicType_SequenceImpUtil<VALUE_TYPE, false>::hasAttribute(
    const Value&,
    const char *,
    int)
{
    bsl::abort();
}

template <class VALUE_TYPE>
bool TestDynamicType_SequenceImpUtil<VALUE_TYPE, false>::hasAttribute(
    const Value&,
    int)
{
    bsl::abort();
}

template <class VALUE_TYPE>
template <class MANIPULATOR>
int TestDynamicType_SequenceImpUtil<VALUE_TYPE, false>::manipulateAttribute(
    Value *,
    MANIPULATOR&,
    const char *,
    int)
{
    bsl::abort();
}

template <class VALUE_TYPE>
template <class MANIPULATOR>
int TestDynamicType_SequenceImpUtil<VALUE_TYPE, false>::manipulateAttribute(
    Value *,
    MANIPULATOR&,
    int)
{
    bsl::abort();
}

template <class VALUE_TYPE>
template <class MANIPULATOR>
int TestDynamicType_SequenceImpUtil<VALUE_TYPE, false>::manipulateAttributes(
    Value *,
    MANIPULATOR&)
{
    bsl::abort();
}

                           // ---------------------
                           // class TestDynamicType
                           // ---------------------

// PRIVATE CLASS FUNCTIONS
template <class VALUE_TYPE>
bool TestDynamicType<VALUE_TYPE>::valueHasCategory(int category)
{
    return category == bdlat_TypeCategory::Select<VALUE_TYPE>::e_SELECTION;
}

// CLASS METHODS
template <class VALUE_TYPE>
bool TestDynamicType<VALUE_TYPE>::areEqual(const TestDynamicType& lhs,
                                           const TestDynamicType& rhs)
{
    return lhs.d_value.object() == rhs.d_value.object();
}

// CREATORS
template <class VALUE_TYPE>
TestDynamicType<VALUE_TYPE>::TestDynamicType()
: d_value(bslma::Default::allocator())
, d_allocator_p(bslma::Default::allocator())
{
}

template <class VALUE_TYPE>
TestDynamicType<VALUE_TYPE>::TestDynamicType(bslma::Allocator *basicAllocator)
: d_value(basicAllocator)
, d_allocator_p(bslma::Default::allocator())
{
}

template <class VALUE_TYPE>
TestDynamicType<VALUE_TYPE>::TestDynamicType(const Value&      value,
                                             bslma::Allocator *basicAllocator)
: d_value(value, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

template <class VALUE_TYPE>
TestDynamicType<VALUE_TYPE>::TestDynamicType(
                                        const TestDynamicType&  original,
                                        bslma::Allocator       *basicAllocator)
: d_value(original.d_value.object(), basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

// MANIPULATORS
template <class VALUE_TYPE>
TestDynamicType<VALUE_TYPE>& TestDynamicType<VALUE_TYPE>::operator=(
                                               const TestDynamicType& original)
{
    d_value.object() = original.d_value.object();
    return *this;
}

template <class VALUE_TYPE>
template <class MANIPULATOR>
int TestDynamicType<VALUE_TYPE>::arrayManipulateElement(
                                                      MANIPULATOR& manipulator,
                                                      int          index)
{
    BSLS_ASSERT(valueHasCategory(e_ARRAY));

    return ArrayImpUtil::manipulateElement(
        &d_value.object(), manipulator, index);
}

template <class VALUE_TYPE>
void TestDynamicType<VALUE_TYPE>::arrayResize(int newSize)
{
    BSLS_ASSERT(valueHasCategory(e_ARRAY));

    return ArrayImpUtil::resize(&d_value.object(), newSize);
}

template <class VALUE_TYPE>
int TestDynamicType<VALUE_TYPE>::choiceMakeSelection(int selectionId)
{
    BSLS_ASSERT(valueHasCategory(e_CHOICE));

    return ChoiceImpUtil::makeSelection(&d_value.object(), selectionId);
}

template <class VALUE_TYPE>
int TestDynamicType<VALUE_TYPE>::choiceMakeSelection(
                                               const char *selectionName,
                                               int         selectionNameLength)
{
    BSLS_ASSERT(valueHasCategory(e_CHOICE));

    return ChoiceImpUtil::makeSelection(
        &d_value.object(), selectionName, selectionNameLength);
}

template <class VALUE_TYPE>
template <class MANIPULATOR>
int TestDynamicType<VALUE_TYPE>::choiceManipulateSelection(
                                                      MANIPULATOR& manipulator)
{
    BSLS_ASSERT(valueHasCategory(e_CHOICE));

    return ChoiceImpUtil::manipulateSelection(&d_value.object(), manipulator);
}

template <class VALUE_TYPE>
template <class BASE_TYPE>
int TestDynamicType<VALUE_TYPE>::customizedTypeConvertFromBaseType(
                                                         const BASE_TYPE& base)
{
    BSLS_ASSERT(valueHasCategory(e_CUSTOMIZED_TYPE));

    return CustomizedTypeImpUtil::convertFromBaseType(&d_value.object(), base);
}

template <class VALUE_TYPE>
int TestDynamicType<VALUE_TYPE>::enumerationFromInt(int number)
{
    BSLS_ASSERT(valueHasCategory(e_ENUMERATION));

    return EnumerationImpUtil::fromInt(&d_value.object(), number);
}

template <class VALUE_TYPE>
int TestDynamicType<VALUE_TYPE>::enumerationFromString(
                                                      const char *string,
                                                      int         stringLength)
{
    BSLS_ASSERT(valueHasCategory(e_ENUMERATION));

    return EnumerationImpUtil::fromString(
        &d_value.object(), string, stringLength);
}

template <class VALUE_TYPE>
void TestDynamicType<VALUE_TYPE>::nullableValueMakeValue()
{
    BSLS_ASSERT(valueHasCategory(e_NULLABLE_VALUE));

    return NullableValueImpUtil::makeValue(&d_value.object());
}

template <class VALUE_TYPE>
template <class MANIPULATOR>
int TestDynamicType<VALUE_TYPE>::nullableValueManipulateValue(
                                                      MANIPULATOR& manipulator)
{
    BSLS_ASSERT(valueHasCategory(e_NULLABLE_VALUE));

    return NullableValueImpUtil::manipulateValue(&d_value.object(),
                                                 manipulator);
}

template <class VALUE_TYPE>
template <class MANIPULATOR>
int TestDynamicType<VALUE_TYPE>::sequenceManipulateAttribute(
                                             MANIPULATOR&  manipulator,
                                             const char   *attributeName,
                                             int           attributeNameLength)
{
    BSLS_ASSERT(valueHasCategory(e_SEQUENCE));

    return SequenceImpUtil::manipulateAttribute(
        &d_value.object(), manipulator, attributeName, attributeNameLength);
}

template <class VALUE_TYPE>
template <class MANIPULATOR>
int TestDynamicType<VALUE_TYPE>::sequenceManipulateAttribute(
                                                      MANIPULATOR& manipulator,
                                                      int          attributeId)
{
    BSLS_ASSERT(valueHasCategory(e_SEQUENCE));

    return SequenceImpUtil::manipulateAttribute(
        &d_value.object(), manipulator, attributeId);
}

template <class VALUE_TYPE>
template <class MANIPULATOR>
int TestDynamicType<VALUE_TYPE>::sequenceManipulateAttributes(
                                                      MANIPULATOR& manipulator)
{
    BSLS_ASSERT(valueHasCategory(e_SEQUENCE));

    return SequenceImpUtil::manipulateAttributes(&d_value.object(),
                                                 manipulator);
}

template <class VALUE_TYPE>
void TestDynamicType<VALUE_TYPE>::reset()
{
    bdlat_ValueTypeFunctions::reset(&d_value.object());
}

template <class VALUE_TYPE>
void TestDynamicType<VALUE_TYPE>::setValue(const Value& value)
{
    d_value.object() = value;
}

// ACCESSORS
template <class VALUE_TYPE>
const char *TestDynamicType<VALUE_TYPE>::className() const
{
    return "MyDynamicType";
}

template <class VALUE_TYPE>
template <class ACCESSOR>
int TestDynamicType<VALUE_TYPE>::arrayAccessElement(ACCESSOR& accessor,
                                                    int       index) const
{
    BSLS_ASSERT(valueHasCategory(e_ARRAY));

    return ArrayImpUtil::accessElement(d_value.object(), accessor, index);
}

template <class VALUE_TYPE>
bsl::size_t TestDynamicType<VALUE_TYPE>::arraySize() const
{
    BSLS_ASSERT(valueHasCategory(e_ARRAY));

    return ArrayImpUtil::size(d_value.object());
}

template <class VALUE_TYPE>
template <class ACCESSOR>
int TestDynamicType<VALUE_TYPE>::choiceAccessSelection(
                                                      ACCESSOR& accessor) const
{
    BSLS_ASSERT(valueHasCategory(e_CHOICE));

    return ChoiceImpUtil::accessSelection(d_value.object(), accessor);
}

template <class VALUE_TYPE>
bool TestDynamicType<VALUE_TYPE>::choiceHasSelection(int selectionId) const
{
    BSLS_ASSERT(valueHasCategory(e_CHOICE));

    return ChoiceImpUtil::hasSelection(d_value.object(), selectionId);
}

template <class VALUE_TYPE>
bool TestDynamicType<VALUE_TYPE>::choiceHasSelection(
                                         const char *selectionName,
                                         int         selectionNameLength) const
{
    BSLS_ASSERT(valueHasCategory(e_CHOICE));

    return ChoiceImpUtil::hasSelection(
        d_value.object(), selectionName, selectionNameLength);
}

template <class VALUE_TYPE>
int TestDynamicType<VALUE_TYPE>::choiceSelectionId() const
{
    BSLS_ASSERT(valueHasCategory(e_CHOICE));

    return ChoiceImpUtil::selectionId(d_value.object());
}

template <class VALUE_TYPE>
const typename TestDynamicType_CustomizedTypeImpUtil<VALUE_TYPE>::BaseType&
TestDynamicType<VALUE_TYPE>::customizedTypeConvertToBaseType() const
{
    BSLS_ASSERT(valueHasCategory(e_CUSTOMIZED_TYPE));

    return CustomizedTypeImpUtil::convertToBaseType(d_value.object());
}

template <class VALUE_TYPE>
void TestDynamicType<VALUE_TYPE>::enumerationToInt(int *result) const
{
    BSLS_ASSERT(valueHasCategory(e_ENUMERATION));

    EnumerationImpUtil::toInt(result, d_value.object());
}

template <class VALUE_TYPE>
void TestDynamicType<VALUE_TYPE>::enumerationToString(
                                                     bsl::string *result) const
{
    BSLS_ASSERT(valueHasCategory(e_ENUMERATION));

    EnumerationImpUtil::toString(result, d_value.object());
}

template <class VALUE_TYPE>
template <class ACCESSOR>
int TestDynamicType<VALUE_TYPE>::nullableValueAccessValue(
                                                      ACCESSOR& accessor) const
{
    BSLS_ASSERT(valueHasCategory(e_NULLABLE_VALUE));

    return NullableValueImpUtil::accessValue(d_value.object(), accessor);
}

template <class VALUE_TYPE>
bool TestDynamicType<VALUE_TYPE>::nullableValueIsNull() const
{
    BSLS_ASSERT(valueHasCategory(e_NULLABLE_VALUE));

    return NullableValueImpUtil::isNull(d_value.object());
}

template <class VALUE_TYPE>
template <class ACCESSOR>
int TestDynamicType<VALUE_TYPE>::sequenceAccessAttribute(
                                         ACCESSOR&   accessor,
                                         const char *attributeName,
                                         int         attributeNameLength) const
{
    BSLS_ASSERT(valueHasCategory(e_SEQUENCE));

    return SequenceImpUtil::accessAttribute(
        d_value.object(), accessor, attributeName, attributeNameLength);
}

template <class VALUE_TYPE>
template <class ACCESSOR>
int TestDynamicType<VALUE_TYPE>::sequenceAccessAttribute(
                                                   ACCESSOR& accessor,
                                                   int       attributeId) const
{
    BSLS_ASSERT(valueHasCategory(e_SEQUENCE));

    return SequenceImpUtil::accessAttribute(
        d_value.object(), accessor, attributeId);
}

template <class VALUE_TYPE>
template <class ACCESSOR>
int TestDynamicType<VALUE_TYPE>::sequenceAccessAttributes(
                                                      ACCESSOR& accessor) const
{
    BSLS_ASSERT(valueHasCategory(e_SEQUENCE));

    return SequenceImpUtil::accessAttributes(d_value.object(), accessor);
}

template <class VALUE_TYPE>
bool TestDynamicType<VALUE_TYPE>::sequenceHasAttribute(
                                         const char *attributeName,
                                         int         attributeNameLength) const
{
    BSLS_ASSERT(valueHasCategory(e_SEQUENCE));

    return SequenceImpUtil::hasAttribute(
        d_value.object(), attributeName, attributeNameLength);
}

template <class VALUE_TYPE>
bool TestDynamicType<VALUE_TYPE>::sequenceHasAttribute(int attributeId) const
{
    BSLS_ASSERT(valueHasCategory(e_SEQUENCE));

    return SequenceImpUtil::hasAttribute(d_value.object(), attributeId);
}

template <class VALUE_TYPE>
bdlat_TypeCategory::Value TestDynamicType<VALUE_TYPE>::select() const
{
    return static_cast<bdlat_TypeCategory::Value>(
        bdlat_TypeCategory::Select<Value>::e_SELECTION);
}

template <class VALUE_TYPE>
const VALUE_TYPE& TestDynamicType<VALUE_TYPE>::value() const
{
    return d_value.object();
}

// FREE OPERATORS
template <class VALUE_TYPE>
bsl::ostream& operator<<(bsl::ostream&                      stream,
                         const TestDynamicType<VALUE_TYPE>& object)
{
    return stream << "[" << object.className() << " value " << object.value()
                  << "]";
}

template <class VALUE_TYPE>
inline
bool operator==(const TestDynamicType<VALUE_TYPE>& lhs,
                const TestDynamicType<VALUE_TYPE>& rhs)
{
    return TestDynamicType<VALUE_TYPE>::areEqual(lhs, rhs);
}

template <class VALUE_TYPE>
inline
bool operator!=(const TestDynamicType<VALUE_TYPE>& lhs,
                const TestDynamicType<VALUE_TYPE>& rhs)
{
    return !TestDynamicType<VALUE_TYPE>::areEqual(lhs, rhs);
}

// TRAITS
template <class VALUE_TYPE>
const char *bdlat_TypeName_className(const TestDynamicType<VALUE_TYPE>& object)
{
    return object.className();
}

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_arrayManipulateElement(TestDynamicType<VALUE_TYPE> *object,
                                 MANIPULATOR&                 manipulator,
                                 int                          index)
{
    return object->arrayManipulateElement(manipulator, index);
}

template <class VALUE_TYPE>
void bdlat_arrayResize(TestDynamicType<VALUE_TYPE> *object, int newSize)
{
    return object->arrayResize(newSize);
}

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_arrayAccessElement(const TestDynamicType<VALUE_TYPE>& object,
                             ACCESSOR&                          accessor,
                             int                                index)
{
    return object.arrayAccessElement(accessor, index);
}

template <class VALUE_TYPE>
bsl::size_t bdlat_arraySize(const TestDynamicType<VALUE_TYPE>& object)
{
    return object.arraySize();
}

template <class VALUE_TYPE>
int bdlat_choiceMakeSelection(TestDynamicType<VALUE_TYPE> *object,
                              int                          selectionId)
{
    return object->choiceMakeSelection(selectionId);
}

template <class VALUE_TYPE>
int bdlat_choiceMakeSelection(TestDynamicType<VALUE_TYPE> *object,
                              const char                  *selectionName,
                              int                          selectionNameLength)
{
    return object->choiceMakeSelection(selectionName, selectionNameLength);
}

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_choiceManipulateSelection(TestDynamicType<VALUE_TYPE> *object,
                                    MANIPULATOR&                 manipulator)
{
    return object->choiceManipulateSelection(manipulator);
}

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_choiceAccessSelection(const TestDynamicType<VALUE_TYPE>& object,
                                ACCESSOR&                          accessor)
{
    return object.choiceAccessSelection(accessor);
}

template <class VALUE_TYPE>
bool bdlat_choiceHasSelection(const TestDynamicType<VALUE_TYPE>& object,
                              int                                selectionId)
{
    return object.choiceHasSelection(selectionId);
}

template <class VALUE_TYPE>
bool bdlat_choiceHasSelection(
                       const TestDynamicType<VALUE_TYPE>&  object,
                       const char                         *selectionName,
                       int                                 selectionNameLength)
{
    return object.choiceHasSelection(selectionName, selectionNameLength);
}

template <class VALUE_TYPE, class BASE_TYPE>
int bdlat_customizedTypeConvertFromBaseType(
                                           TestDynamicType<VALUE_TYPE> *object,
                                           const BASE_TYPE&             value)
{
    return object->customizedTypeConvertFromBaseType(value);
}

template <class VALUE_TYPE>
const typename TestDynamicType<VALUE_TYPE>::BaseType&
bdlat_customizedTypeConvertToBaseType(
                                     const TestDynamicType<VALUE_TYPE>& object)
{
    return object.customizedTypeConvertToBaseType();
}

template <class VALUE_TYPE>
int bdlat_enumFromInt(TestDynamicType<VALUE_TYPE> *object, int number)
{
    return object->enumerationFromInt(number);
}

template <class VALUE_TYPE>
int bdlat_enumFromString(TestDynamicType<VALUE_TYPE> *object,
                         const char                  *string,
                         int                          stringLength)
{
    return object->enumerationFromString(string, stringLength);
}

template <class VALUE_TYPE>
void bdlat_enumToInt(int *result, const TestDynamicType<VALUE_TYPE>& object)
{
    return object.enumerationToInt(result);
}

template <class VALUE_TYPE>
void bdlat_enumToString(bsl::string                        *result,
                        const TestDynamicType<VALUE_TYPE>&  object)
{
    return object.enumerationToString(result);
}

template <class VALUE_TYPE>
void bdlat_nullableValueMakeValue(TestDynamicType<VALUE_TYPE> *object)
{
    object->nullableValueMakeValue();
}

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_nullableValueManipulateValue(
                                      TestDynamicType<VALUE_TYPE> *object,
                                      MANIPULATOR&                 manipulator)
{
    return object->nullableValueManipulateValue(manipulator);
}

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_nullableValueAccessValue(const TestDynamicType<VALUE_TYPE>& object,
                                   ACCESSOR&                          accessor)
{
    return object.nullableValueAccessValue(accessor);
}

template <class VALUE_TYPE>
bool bdlat_nullableValueIsNull(const TestDynamicType<VALUE_TYPE>& object)
{
    return object.nullableValueIsNull();
}

template <class VALUE_TYPE>
int bdlat_choiceSelectionId(const TestDynamicType<VALUE_TYPE>& object)
{
    return object.choiceSelectionId();
}

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_sequenceAccessAttribute(
                       const TestDynamicType<VALUE_TYPE>&  object,
                       ACCESSOR&                           accessor,
                       const char                         *attributeName,
                       int                                 attributeNameLength)
{
    return object.sequenceAccessAttribute(
        accessor, attributeName, attributeNameLength);
}

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_sequenceAccessAttribute(
                                const TestDynamicType<VALUE_TYPE>& object,
                                ACCESSOR&                          accessor,
                                int                                attributeId)
{
    return object.sequenceAccessAttribute(accessor, attributeId);
}

template <class VALUE_TYPE, class ACCESSOR>
int bdlat_sequenceAccessAttributes(const TestDynamicType<VALUE_TYPE>& object,
                                   ACCESSOR&                          accessor)
{
    return object.sequenceAccessAttributes(accessor);
}

template <class VALUE_TYPE>
int bdlat_sequenceHasAttribute(
                       const TestDynamicType<VALUE_TYPE>&  object,
                       const char                         *attributeName,
                       int                                 attributeNameLength)
{
    return object.sequenceHasAttribute(attributeName, attributeNameLength);
}

template <class VALUE_TYPE>
int bdlat_sequenceHasAttribute(const TestDynamicType<VALUE_TYPE>& object,
                               int                                attributeId)
{
    return object.sequenceHasAttribute(attributeId);
}

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(
                              TestDynamicType<VALUE_TYPE> *object,
                              MANIPULATOR&                 manipulator,
                              const char                  *attributeName,
                              int                          attributeNameLength)
{
    return object->sequenceManipulateAttribute(
        manipulator, attributeName, attributeNameLength);
}

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_sequenceManipulateAttribute(TestDynamicType<VALUE_TYPE> *object,
                                      MANIPULATOR&                 manipulator,
                                      int                          attributeId)
{
    return object->sequenceManipulateAttribute(manipulator, attributeId);
}

template <class VALUE_TYPE, class MANIPULATOR>
int bdlat_sequenceManipulateAttributes(
                                      TestDynamicType<VALUE_TYPE> *object,
                                      MANIPULATOR&                 manipulator)
{
    return object->sequenceManipulateAttributes(manipulator);
}

template <class VALUE_TYPE>
bdlat_TypeCategory::Value bdlat_typeCategorySelect(
                                     const TestDynamicType<VALUE_TYPE>& object)
{
    return object.select();
}

}  // close package namespace
}  // close enterprise namespace

#endif // INCLUDED_S_BALTST_TESTDYNAMICTYPE

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
