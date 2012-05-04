// baea_serializableobjectproxyutil.h                               -*-C++-*-
#ifndef INCLUDED_BAEA_SERIALIZABLEOBJECTPROXYUTIL
#define INCLUDED_BAEA_SERIALIZABLEOBJECTPROXYUTIL

#include <bdes_ident.h>
BDES_IDENT_RCSID(baea_serializableobjectproxyutil_h,"$Id$ $CSID$")
BDES_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide functions for populating a 'SerializableObjectProxy'.
//
//@AUTHOR: David Schumann (dschumann1)
//
//@CLASSES:
//  SerializableObjectProxyUtil: functions populating 'SerializableObjectProxy'
//
//@SEE_ALSO: baea-serializableobjectproxy, bdeat, bdem_berencoder
//
//@DESCRIPTION: This component provides a suite of functions for populating a
// 'baea::SerializableObjectProxy' object so that it represents some
// 'bas_codegen.pl'-generated object for the purpose of encoding or decoding
// a represented object.  Encoding/decoding a 'SerializableObjectProxy' using
// a bdeat-style encoder/decoder involves expanding the template functions of
// that encoder/decoder for only one type rather than for many types, and thus
// can be expected to produce substantially less object code at a lower
// compilation cost.  Instead, the type-specific logic is supplied by
// template expansions of functions in this component, which are generally
// much simpler and smaller.
//
// The type categories of the 'bdeat' package are used throughout this
// component.  The terms "Array", "Choice", "Customized", "Enumeration",
// "Nullable", "Sequence", and "Simple" all refer to the corresponding
// 'bdeat' categories.
//
///Proxy Object Structure
///----------------------
// 'SerializableObjectProxyUtil' provides functions that load a
// 'SerializableObjectProxy' with the necessary information to encode from,
// or decode into, the supplied 'bdeat'-compliant proxied object.  If the
// proxied object holds another object -- as is the case for Sequence, Choice,
// Nullable, and Array values -- the proxy holds a pointer to a function that
// can populate another a SerializableObjectProxy to describe that contained
// object.
//
// This can be thought of as a form of 'lazy evaluation'.  An example may
// help illustrate the approach.  We start by outlining the definition of
// a bas_codegen.pl-generated type 'MyChoiceType', having 'MySequenceType'
// as one of its selections, that we wish to encode/decode:
//..
//  class MyChoiceType {
//    union {
//      bsls_ObjectBuffer<MySequenceType> d_mySequence;  // selection 0
//      // ...
//    };
//    // ...
//    MySequenceType& makeMySequenceValue();
//    // ...
//  };
//..
// Now we define an instance of 'MyChoiceType', 'myChoice', and an initially
// empty 'SerializableObjectProxy', 'myProxy', that we will use for encoding
// 'myChoice'.
//..
//  MyChoiceType myChoice;
//  myChoice.makeMySequenceValue();
//  baea::SerializableObjectProxy myProxy;
//..
// If we then invoke 'SerializableObjectProxyUtil::makeEncodeProxy' on
// 'myChoice', it will populate 'myProxy' in the following way:
//..
//  .--------------------------------.
//  |   SerializableObjectProxy      |
//  |   void         *d_object_p;  --+-->[&myChoice.d_mySequence]
//  |   Variant<...>  d_objectInfo;--+--.
//  `--------------------------------'  |
//                                      |
//                                      V
//  .------------------------------------------.
//  |  ChoiceEncodeInfo                        |
//  |  int                  d_selectionId; ----+--> 0
//  |  bdeat_SelectionInfo *d_selection;   ----+--> [ selection description ]
//  |  const char          *d_classname_p; ----+--> "MyChoiceType"
//  |  Accessor             d_accessor;    --. |
//  `----------------------------------------|-'
//                                           |
//                                           V
//  .------------------------------------------------------------------.
//  |  SerializableObjectProxyUtil::makeEncodeProxyRaw<MySequenceType> |
//  |    A function that, when invoked, will load a                    |
//  |    SerializableObjectProxy for 'myChoice.d_mySequence'.          |
//  `------------------------------------------------------------------'
//..
// So for a Choice object, the proxy object populated for encoding that Choice
// contains: a pointer to the underlying selection, information about that
// choice, and a pointer to a function that, when invoked, will populate
// another proxy for the selection.  That function is not invoked until the
// object is later encoded.  A similar structure is created for
// encoding/decoding any container type, but the details vary slightly based
// on the type and the direction (encoding/decoding).
//
///Treatment of "Customized" Types
///===============================
// The 'bdeat' package has the concept of a Customized type.  Types in this
// category contain a restricted value of some other type.  A common example
// is a string of restricted length.  Customized types are required to have
// conversion methods for their underlying type -- for example, a Customized
// type based on 'bsl::string' is required to have a 'toString' method.
//
// 'SerializableObjectProxyUtil' makes the assumption that such methods
// return const references to a member variable of the underlying type, so
// that it is safe to take the address of the return value and store it for
// later use.  In this way, 'toString' is treated by this component as an
// accessor method for a member variable of type 'bsl::string' assumed to exist
// within the type.  This is not required by the 'bdeat' package -- a
// Customized type based on 'bsl::string' is allowed to return a 'bsl::string'
// by value from toString().  Such types cannot be used with this component,
// or undefined behavior will result.  'bas_codegen.pl' emits Customized types
// that return const references from their conversion methods and are thus
// compatible with this component.
//
///Usage Example
///=============
// In this section we show the intended usage of this component.
//
///Example 1: Serializing a Simple Request
///---------------------------------------
// In this example, we encode and decode a BAS message object in memory using
// SerializableObjectProxy to reduce the complexity of the resulting object
// code as compared with encoding and decoding the message object directly.
//..
// baea::Request message1, message2;
// message1.makeSimpleRequest();
// baexml_EncoderOptions eOptions;
// baexml_Encoder encoder(&eOptions);
// baexml_DecoderOptions dOptions;
// baexml_MiniReader reader;
// baexml_Decoder decoder(&dOptions, &reader);
//
// baea::SerializableObjectProxy encodeProxy;
// baea::SerializableObjectProxyUtil::makeEncodeProxy(&encodeProxy,
//                                                    &message1);
// bdesb_MemOutStreamBuf osb;
// encoder.encode(&osb, encodeProxy);
//
// baea::SerializableObjectProxy decodeProxy;
// baea::SerializableObjectProxyUtil::makeDecodeProxy(&decodeProxy,
//                                                      &message2);
//
// bdesb_FixedMemInStreamBuf isb(osb.data(), osb.length());
// decoder.decode(&isb, &decodeProxy);
//
// assert(message1 == message2);
//..
//

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEA_SERIALIZABLEOBJECTPROXY
#include <baea_serializableobjectproxy.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

namespace BloombergLP {

namespace baea {

class SerializableObjectProxyUtil;
class SerializableObjectProxyUtil_ChoiceAccessorProxy;
class SerializableObjectProxyUtil_SequenceAccessorProxy;
class SerializableObjectProxyUtil_SequenceManipulatorProxy;
class SerializableObjectProxyUtil_ChoiceManipulatorProxy;

template <typename TYPE, bool HAS_ELEMENTS>
class SerializableObjectProxyUtil_SequenceLoader;

                     // =============================
                     // class SerializableObjectProxy
                     // =============================

class SerializableObjectProxyUtil {
    // This type provides a namespace for free functions that populate
    // 'SerializableObjectProxy' objects.

    // PRIVATE CLASS METHODS
    template<typename VALUE_TYPE>
    static int enumSetter(void *value, int intVal);
        // Set the specified 'value' (assumed to be of the parameterized
        // 'VALUE_TYPE') to the specified 'intVal'.  Return 0 on success,
        // and a non-zero value otherwise.  Note that this method is an
        // implementation of 'SerializableObjectProxyFunctions::IntSetter'.

    template<typename VALUE_TYPE>
    static int enumStringSetter(void        *value,
                                const char  *stringValue,
                                int          stringLength);
        // Set the specified 'value' (assumed to be of the parameterized
        // 'VALUE_TYPE') to the specified 'stringValue' of the specified
        // 'stringLength'.  Return 0 on success, and a non-zero value
        // otherwise.  Note that this method is an implementation of
        // 'SerializableObjectProxyFunctions::StringSetter'.

    template<typename NULLABLE>
    static void makeValueFn(void *object);
        // Make specified 'object' (assumed to be of the parameterized
        // 'NULLABLE' type) non-null.  Note that this method is an
        // implementation of 'SerializableObjectProxyFunctions::Manipulator'.

    template<typename NULLABLE>
    static void* fetchValueFn(void *object);
        // Return the address of the object contained within the
        // specified 'object' (assumed to be of the parameterized
        // 'NULLABLE' type).  If the 'object' represents a null value,
        // return 0.  Note that this method is an implementation of
        // 'SerializableObjectProxyFunctions::ObjectFetcher'.

    template<typename VECTOR>
    static void vectorResizeFn(void         *object,
                               void        **newBegin,
                               bsl::size_t   newSize);
        // Resize the specified 'object' (assumed to be of the
        // parameterized 'VECTOR' type), to the specified 'newSize'.  Load
        // into the specified 'newBegin' the address of the first element in
        // the newly resized vector, or 0 if 'newSize' is 0.  Note that this is
        // an implementation of 'SerializableObjectProxyFunctions::Resizer'.

    template<typename TYPE>
    static void sequenceAccessorFn(SerializableObjectProxy       *proxy,
                                   const SerializableObjectProxy& object,
                                   int                            attributeId);
        // Populate the specified 'proxy' to represent the attribute with the
        // specified 'attributeId' within the Sequence (assumed to of the
        // parameterized 'TYPE') represented by the specified 'object', for
        // encoding.  Note that this is an implementation of
        // ''SerializableObjectProxyFunctions::ElementAccessor'.

    template<typename TYPE>
    static void sequenceManipulatorFn(
                                  SerializableObjectProxy        *proxy,
                                  const SerializableObjectProxy&  object,
                                  int                             attributeId);
        // Populate the specified 'proxy' to represent the attribute with the
        // specified 'attributeId' within the Sequence (assumed to of the
        // parameterized 'TYPE') represented by the specified 'object', for
        // decoding.  Note that this is an implementation of
        // 'SerializableObjectProxyFunctions::ElementAccessor'.

    template<typename TYPE>
    static void choiceManipulatorFn(
                                 SerializableObjectProxy    *proxy,
                                 void                       *object,
                                 const bdeat_SelectionInfo **selectionInfoPtr);
        // Populate the specified 'proxy' to represent the current selection of
        // the specified Choice 'object' (assumed to be of the parameterized
        // 'TYPE'), and load into the specified 'selectionInfoPtr' the address
        // of the bdeat_SelectionInfo for that selection.  Note that this is an
        // implementation of
        // 'SerializableObjectProxyFunctions::AccessorAndExtractor'.

    template<typename TYPE>
    static int choiceChooserFn(void* object, int selectionId);
        // Change the selection of the specified Choice 'object' (assumed to be
        // of the parameterized 'TYPE') to the specified 'selectionId'.  Return
        // 0 on success, and a non-zero value otherwise.  Note that this
        // function is an implementation of
        // 'SerializableObjectProxyFunctions::Chooser'.

    // Dispatch functions for makeEncodeProxy

    template<typename TYPE>
    static void makeEncodeProxy(SerializableObjectProxy            *proxy,
                                bdeut_NullableAllocatedValue<TYPE> *object,
                                bdeat_TypeCategory::NullableValue);
    template<typename TYPE>
    static void makeEncodeProxy(SerializableObjectProxy      *proxy,
                                bdeut_NullableValue<TYPE>    *object,
                                bdeat_TypeCategory::NullableValue);
    template <typename TYPE>
    static void makeEncodeProxy(SerializableObjectProxy *proxy,
                                TYPE* object,
                                bdeat_TypeCategory::Enumeration);
    template <typename TYPE>
    static void makeEncodeProxy(SerializableObjectProxy *proxy,
                                TYPE                    *object,
                                bdeat_TypeCategory::CustomizedType);
    template <typename TYPE>
    static void makeEncodeProxy(SerializableObjectProxy *proxy,
                                TYPE* object,
                                bdeat_TypeCategory::Choice);
    template <typename TYPE>
    static void makeEncodeProxy(SerializableObjectProxy *proxy,
                                TYPE                    *object,
                                bdeat_TypeCategory::Sequence);
    static void makeEncodeProxy(SerializableObjectProxy   *proxy,
                                bsl::vector<char>         *object,
                                bdeat_TypeCategory::Array);
    template <typename ELEMENT_TYPE>
    static void makeEncodeProxy(SerializableObjectProxy   *proxy,
                                bsl::vector<ELEMENT_TYPE> *object,
                                bdeat_TypeCategory::Array);
    template <typename TYPE>
    static void makeEncodeProxy(SerializableObjectProxy *proxy,
                                TYPE* object,
                                bdeat_TypeCategory::Simple);
        // Populate the specified 'proxy' to represent the specified
        // 'object' for encoding, based on the type of the unnamed "dispatch"
        // argument.

    template <typename TYPE>
    static void makeEncodeProxyRaw(SerializableObjectProxy *proxy,
                                   void* object);
        // Populate the specified 'proxy' to represent the specified 'object'
        // of the parameterized 'TYPE' for encoding.


    // Dispatch functions for makeDecodeProxy

    template<typename TYPE>
    static void makeDecodeProxy(SerializableObjectProxy *proxy,
                                TYPE                    *object,
                                bdeat_TypeCategory::Enumeration);
    template<typename TYPE>
    static void makeDecodeProxy(
                              SerializableObjectProxy            *proxy,
                              bdeut_NullableAllocatedValue<TYPE> *object,
                              bdeat_TypeCategory::NullableValue);
    template<typename TYPE>
    static void makeDecodeProxy(SerializableObjectProxy   *proxy,
                                bdeut_NullableValue<TYPE> *object,
                                bdeat_TypeCategory::NullableValue);
    template<typename TYPE>
    static void makeDecodeProxy(SerializableObjectProxy *proxy,
                                TYPE                    *object,
                                bdeat_TypeCategory::Choice);
    template<typename TYPE>
    static void makeDecodeProxy(SerializableObjectProxy *proxy,
                                TYPE                    *object,
                                bdeat_TypeCategory::Sequence);
    static void makeDecodeProxy(SerializableObjectProxy *proxy,
                                bsl::vector<char>       *object,
                                bdeat_TypeCategory::Array);
    template<typename ELEMENT_TYPE>
    static void makeDecodeProxy(SerializableObjectProxy   *proxy,
                                bsl::vector<ELEMENT_TYPE> *object,
                                bdeat_TypeCategory::Array);
    template<typename TYPE>
    inline
    static void makeDecodeProxy(SerializableObjectProxy *proxy,
                                TYPE                    *object,
                                bdeat_TypeCategory::CustomizedType);
    template<typename TYPE>
    inline
    static void makeDecodeProxy(SerializableObjectProxy *proxy,
                                TYPE                    *object,
                                bdeat_TypeCategory::Simple);
        // Populate the specified 'proxy' to represent the specified 'object'
        // for decoding, based on the type of the unnamed "dispatch" argument.

    template <typename TYPE>
    static void makeDecodeProxyRaw(SerializableObjectProxy *proxy,
                                   void                    *object);
        // Populate the specified 'proxy' to represent the specified 'object'
        // of the parameterized 'TYPE' for decoding.

    static void makeProxyForEmptySequence(SerializableObjectProxy *proxy,
                                          const char              *className);
        // Populate the specified 'proxy' to represent an empty sequence (i.e.,
        // one without any elements) having the specified 'className'.

    // FRIENDS
    friend class SerializableObjectProxyUtil_ChoiceAccessorProxy;
    friend class SerializableObjectProxyUtil_SequenceAccessorProxy;
    friend class SerializableObjectProxyUtil_SequenceManipulatorProxy;
    friend class SerializableObjectProxyUtil_ChoiceManipulatorProxy;
    template <typename TYPE, bool HAS_ELEMENTS>
    friend class SerializableObjectProxyUtil_SequenceLoader;

  public:

    // CLASS METHODS
    template <typename TYPE>
    inline
    static void makeEncodeProxy(SerializableObjectProxy *proxy,
                                TYPE                    *object);
        // Populate the specified 'proxy' to represent the specified
        // 'bdeat'-compliant 'object' of the parameterized 'TYPE' for encoding.

    template <typename TYPE>
    inline
    static void makeDecodeProxy(SerializableObjectProxy *proxy,
                                TYPE                    *object);
        // Populate the specified 'proxy' to represent the specified
        // 'bdeat'-compliant 'object' of the parameterized 'TYPE' for decoding.
        // Note that 'reset' may need to be invoked on 'object' prior to
        // decoding into it, because the 'reset' method of 'proxy' has no
        // effect.
};

      // ==============================================================
      // local class SerializableObjectProxyUtil_ChoiceManipulatorProxy
      // ==============================================================

class SerializableObjectProxyUtil_ChoiceManipulatorProxy {
    // Provide a type to extract the 'bdeat_SelectionInfo' corresponding to
    // the selection of a Choice object, and to populate a
    // SerializableObjectProxy for decoding into that selection.

    SerializableObjectProxy   *d_proxy_p;         // proxy to populate
    const bdeat_SelectionInfo *d_selectionInfo_p; // info for current selection

  public:
    explicit SerializableObjectProxyUtil_ChoiceManipulatorProxy(
                                                SerializableObjectProxy* proxy)
    : d_proxy_p(proxy)
    , d_selectionInfo_p(0)
    {
    }

    template <typename TYPE>
    int operator()(const TYPE& object, const bdeat_SelectionInfo& info);
        // Populate the proxy supplied at construction to represent the
        // specified 'object' for decoding, and save the address of 'info' for
        // later use.  Return 0.

    const bdeat_SelectionInfo *selectionInfoPtr() const;
        // Return the address of the 'bdeat_SelectionInfo' object that was
        // passed to the function-call operator of this object; or 0 if
        // the function-call operator has not been invoked.
};

      // ===========================================================
      // local class SerializableObjectProxyUtil_ChoiceAccessorProxy
      // ===========================================================

class SerializableObjectProxyUtil_ChoiceAccessorProxy {
    // Provide a type to populate a SerializableObjectProxy for encoding the
    // current selection of a Choice object.

    int                      d_selectionId;
    SerializableObjectProxy *d_proxy_p; // held
    const char              *d_className;

  public:
    SerializableObjectProxyUtil_ChoiceAccessorProxy(
                                         SerializableObjectProxy *proxy,
                                         int                      selectionId,
                                         const char              *className)
    : d_proxy_p(proxy)
    , d_selectionId(selectionId)
    , d_className(className)
    {
    }

    template <typename TYPE>
    int operator()(const TYPE& object, const bdeat_SelectionInfo& info);
        // Populate the proxy supplied at construction to represent
        // the specified 'object' (having the specified 'info') for encoding.
        // Return 0.
};

      // ======================================================
      // local class SerializableObjectProxyUtil_SequenceLoader
      // ======================================================

template<typename TYPE>
class SerializableObjectProxyUtil_SequenceLoader<TYPE, true> {
    // This class provides free functions that populate a
    // SerializableObjectProxy to represent a Sequence.  This specialization
    // is for normal sequences (those that have elements).

  public:
    static inline
    void loadAccessor(SerializableObjectProxy* proxy,
                      void*        object) {
        proxy->loadSequence(
                       static_cast<int>(TYPE::NUM_ATTRIBUTES),
                       object,
                       TYPE::ATTRIBUTE_INFO_ARRAY,
                       TYPE::CLASS_NAME,
                       &SerializableObjectProxyUtil::sequenceAccessorFn<TYPE>);
    }

    static inline
    void loadManipulator(SerializableObjectProxy* proxy,
                         void*        object) {
        proxy->loadSequence(
                    static_cast<int>(TYPE::NUM_ATTRIBUTES),
                    object,
                    TYPE::ATTRIBUTE_INFO_ARRAY,
                    TYPE::CLASS_NAME,
                    &SerializableObjectProxyUtil::sequenceManipulatorFn<TYPE>);
    }
};

template <typename TYPE>
class SerializableObjectProxyUtil_SequenceLoader<TYPE, false> {
    // This class provides free functions that populate a
    // SerializableObjectProxy to represent a Sequence.
    // This specialization is for sequences without any elements.  Such
    // sequences may not have an ATTRIBUTE_INFO_ARRAY class member defined,
    // and thus need special handling.
    //
    // Note that the implementation of loadAccessor() and loadManipulator()
    // are identical in this specialization, but not in the general case.

  public:
    static inline
    void loadAccessor(SerializableObjectProxy* proxy, void*)
    {
        SerializableObjectProxyUtil::makeProxyForEmptySequence
            (proxy, TYPE::CLASS_NAME);
    }

    static inline
    void loadManipulator(SerializableObjectProxy* proxy, void*)
    {
        SerializableObjectProxyUtil::makeProxyForEmptySequence
            (proxy, TYPE::CLASS_NAME);
    }
};

      // =============================================================
      // local class SerializableObjectProxyUtil_SequenceAccessorProxy
      // =============================================================

class SerializableObjectProxyUtil_SequenceAccessorProxy
{
    // Provide a type to populate a 'SerializableObjectProxy' for encoding
    // the elements of a Sequence object.

    SerializableObjectProxy *d_proxy_p; // held

  public:
    explicit SerializableObjectProxyUtil_SequenceAccessorProxy(
                                         SerializableObjectProxy* proxy)
    : d_proxy_p(proxy)
    {
    }

    template <typename TYPE>
    int operator()(const TYPE& object, const bdeat_AttributeInfo&);
        // Populate the proxy supplied at construction to represent the
        // specified 'object' for encoding.  Return 0.
};

class SerializableObjectProxyUtil_SequenceManipulatorProxy
{
    // Provide a type to populate a SerializableObjectProxy for decoding into
    // the elements of a Sequence object.

    SerializableObjectProxy *d_proxy_p; // held

  public:
    explicit SerializableObjectProxyUtil_SequenceManipulatorProxy(
                                          SerializableObjectProxy* proxy)
    : d_proxy_p(proxy)
    {
    }

    template <typename TYPE>
    int operator()(const TYPE& object, const bdeat_AttributeInfo&);
        // Populate the proxy supplied at construction to represent the
        // specified 'object' for decoding.  Return 0.
};

// ============================================================================
//                  INLINE AND TEMPLATE FUNCTION DEFINITIONS
// ============================================================================

template<typename VALUE_TYPE>
inline
int SerializableObjectProxyUtil::enumSetter(void* value, int intVal)
{
    return bslalg_TypeTraits<VALUE_TYPE>::Wrapper::fromInt((VALUE_TYPE*)value,
                                                           intVal);
}

template<typename VALUE_TYPE>
inline
int SerializableObjectProxyUtil::enumStringSetter(void* value,
                                                  const char* strVal,
                                                  int strLength)
{
    return bslalg_TypeTraits<VALUE_TYPE>::Wrapper::fromString(
                                                         (VALUE_TYPE*)value,
                                                         strVal,
                                                         strLength);
}

template <typename NULLABLE>
inline
void SerializableObjectProxyUtil::makeValueFn(void* object)
{
    ((NULLABLE*)object)->makeValue();
}

template<typename NULLABLE>
void* SerializableObjectProxyUtil::fetchValueFn(void* object)
{
    NULLABLE* nullableValue = (NULLABLE*)object;
    if (nullableValue->isNull()) {
        // We also use this function for testing for null.

        return 0;                                                     // RETURN
    }
    return &nullableValue->value();
}

template<typename TYPE>
void
SerializableObjectProxyUtil::sequenceAccessorFn(
                                  SerializableObjectProxy        *proxy,
                                  const SerializableObjectProxy&  object,
                                  int                             attributeId)
{
    SerializableObjectProxyUtil_SequenceAccessorProxy accessorProxy(proxy);
    bdeat_SequenceFunctions::accessAttribute(*(const TYPE*)object.object(),
                                             accessorProxy, attributeId);
}

template<typename TYPE>
void
SerializableObjectProxyUtil::sequenceManipulatorFn(
                                  SerializableObjectProxy        *proxy,
                                  const SerializableObjectProxy&  object,
                                  int                             attributeId)
{
    SerializableObjectProxyUtil_SequenceManipulatorProxy
                                                      manipulatorProxy(proxy);
    bdeat_SequenceFunctions::accessAttribute(*(const TYPE*)object.object(),
                                             manipulatorProxy, attributeId);
}

template<typename TYPE>
void
SerializableObjectProxyUtil::choiceManipulatorFn(
                                 SerializableObjectProxy    *proxy,
                                 void                       *object,
                                 const bdeat_SelectionInfo **selectionInfoPtr)
{
    SerializableObjectProxyUtil_ChoiceManipulatorProxy manipulatorProxy(proxy);
    bdeat_ChoiceFunctions::accessSelection(*(const TYPE*)object,
                                           manipulatorProxy);
    *selectionInfoPtr = manipulatorProxy.selectionInfoPtr();
}

template<typename TYPE>
int
SerializableObjectProxyUtil::choiceChooserFn(void* object, int selectionId)
{
    return ((TYPE*)object)->makeSelection(selectionId);
}

template<typename TYPE>
inline
void SerializableObjectProxyUtil::makeDecodeProxy(
                                               SerializableObjectProxy *proxy,
                                               TYPE                    *object,
                                               bdeat_TypeCategory::Enumeration)
{
    proxy->loadEnumeration(
                      object,
                      &enumSetter<TYPE>,
                      &enumStringSetter<TYPE>,
                      bslalg_TypeTraits<TYPE>::Wrapper::ENUMERATOR_INFO_ARRAY,
                      bslalg_TypeTraits<TYPE>::Wrapper::NUM_ENUMERATORS);
}

template<typename TYPE>
inline
void SerializableObjectProxyUtil::makeEncodeProxy(
                                    SerializableObjectProxy            *proxy,
                                    bdeut_NullableAllocatedValue<TYPE> *object,
                                    bdeat_TypeCategory::NullableValue)
{
    proxy->loadNullable((void*)(object->isNull() ? 0 : &object->value()),
                            &makeEncodeProxyRaw<TYPE>);
}

template<typename TYPE>
inline
void SerializableObjectProxyUtil::makeEncodeProxy(
                                          SerializableObjectProxy      *proxy,
                                          bdeut_NullableValue<TYPE>    *object,
                                          bdeat_TypeCategory::NullableValue)
{
    proxy->loadNullable((void*)(object->isNull() ? 0 : &object->value()),
                        &makeEncodeProxyRaw<TYPE>);
}


template<typename TYPE>
inline
void SerializableObjectProxyUtil::makeDecodeProxy(
                                    SerializableObjectProxy            *proxy,
                                    bdeut_NullableAllocatedValue<TYPE> *object,
                                    bdeat_TypeCategory::NullableValue)
{
    proxy->loadNullable(object,
                        &makeDecodeProxyRaw<TYPE>,
                        &makeValueFn<bdeut_NullableAllocatedValue<TYPE> >,
                        &fetchValueFn<bdeut_NullableAllocatedValue<TYPE> >);
}

template<typename TYPE>
inline
void SerializableObjectProxyUtil::makeDecodeProxy(
                                             SerializableObjectProxy   *proxy,
                                             bdeut_NullableValue<TYPE> *object,
                                             bdeat_TypeCategory::NullableValue)
{
    proxy->loadNullable(object,
                        &makeDecodeProxyRaw<TYPE>,
                        &makeValueFn<bdeut_NullableValue<TYPE> >,
                        &fetchValueFn<bdeut_NullableValue<TYPE> >);
}

template<typename TYPE>
inline
void SerializableObjectProxyUtil::makeDecodeProxy(
                                               SerializableObjectProxy *proxy,
                                               TYPE*                    object,
                                               bdeat_TypeCategory::Choice)
{
    proxy->loadChoice(static_cast<int>(TYPE::NUM_SELECTIONS),
                      object,
                      TYPE::SELECTION_INFO_ARRAY,
                      &choiceManipulatorFn<TYPE>,
                      &choiceChooserFn<TYPE>);
}

template<typename TYPE>
inline
void SerializableObjectProxyUtil::makeDecodeProxy(
                                               SerializableObjectProxy *proxy,
                                               TYPE                    *object,
                                               bdeat_TypeCategory::Sequence)
{
    SerializableObjectProxyUtil_SequenceLoader<TYPE,
                                              (TYPE::NUM_ATTRIBUTES > 0)>::
                                                loadManipulator(proxy, object);
}



template<typename VECTOR>
void SerializableObjectProxyUtil::vectorResizeFn(void* object,
                                                 void** newBegin,
                                                 bsl::size_t newSize)
{
    VECTOR* array = (VECTOR*)object;
    array->resize(newSize);
    *newBegin = newSize ? &(*array->begin()) : 0;
}

template <typename TYPE>
inline
void SerializableObjectProxyUtil::makeEncodeProxy(
                                               SerializableObjectProxy *proxy,
                                               TYPE                    *object)
{
    makeEncodeProxy(proxy, object,
                    typename bdeat_TypeCategory::Select<TYPE>::Type());
}

template <typename TYPE>
inline
void SerializableObjectProxyUtil::makeDecodeProxy(
                                               SerializableObjectProxy *proxy,
                                               TYPE                    *object)
{
    makeDecodeProxy(proxy, object,
                    typename bdeat_TypeCategory::Select<TYPE>::Type());
}

inline
void SerializableObjectProxyUtil::makeDecodeProxy(
                                               SerializableObjectProxy *proxy,
                                               bsl::vector<char>       *object,
                                               bdeat_TypeCategory::Array)
{
    // Our representation for decoding into a vector<char> is the
    // same as encoding into a vector<char> because all we need is
    // the address of the vector.

    makeEncodeProxy(proxy, object, bdeat_TypeCategory::Array());
}

template<typename ELEMENT_TYPE>
inline
void SerializableObjectProxyUtil::makeDecodeProxy(
                                             SerializableObjectProxy   *proxy,
                                             bsl::vector<ELEMENT_TYPE> *object,
                                             bdeat_TypeCategory::Array)
{
    proxy->loadArray(object,
                     object->size(),
                     sizeof(ELEMENT_TYPE),
                     object->size() ? &(*object->begin()) : 0,
                     &vectorResizeFn<bsl::vector<ELEMENT_TYPE> >,
                     &makeDecodeProxyRaw<ELEMENT_TYPE>);
}

template<typename TYPE>
inline
void SerializableObjectProxyUtil::makeDecodeProxy(
                                            SerializableObjectProxy *proxy,
                                            TYPE                    *object,
                                            bdeat_TypeCategory::CustomizedType)
{
    typedef typename
        bdeat_CustomizedTypeFunctions::BaseType<TYPE>::Type BaseType;

    // We rely on the knowledge that codegen's toString(), toInt(), etc
    // methods return references to the underlying base-type member
    // variable, so that we can take the address of that reference safely.
    // This is white-box knowledge that depends on the implementation of
    // codegen!  can we do it differently?

    const BaseType& baseRef =
        bdeat_CustomizedTypeFunctions::convertToBaseType(*object);

    makeDecodeProxy(proxy, (BaseType*)&baseRef);
}

template<typename TYPE>
inline
void SerializableObjectProxyUtil::makeDecodeProxy(
                                               SerializableObjectProxy *proxy,
                                               TYPE                    *object,
                                               bdeat_TypeCategory::Simple)
{
    proxy->loadSimple(object);
}

template <typename TYPE>
inline
void SerializableObjectProxyUtil::makeEncodeProxy(
                                               SerializableObjectProxy *proxy,
                                               TYPE                    *object,
                                               bdeat_TypeCategory::Enumeration)
{
    int value;
    bdeat_EnumFunctions::toInt(&value, *object);
    proxy->loadEnumeration(value,
                      bslalg_TypeTraits<TYPE>::Wrapper::ENUMERATOR_INFO_ARRAY,
                      bslalg_TypeTraits<TYPE>::Wrapper::NUM_ENUMERATORS);
}

template <typename TYPE>
inline
void SerializableObjectProxyUtil::makeEncodeProxy(
                                            SerializableObjectProxy *proxy,
                                            TYPE                    *object,
                                            bdeat_TypeCategory::CustomizedType)
{
    typedef typename
        bdeat_CustomizedTypeFunctions::BaseType<TYPE>::Type BaseType;

    // We rely on the knowledge that codegen's toString(), toInt(), etc
    // methods return references to the underlying base-type member
    // variable, so that we can take the address of that reference safely.
    // This is white-box knowledge that depends on the implementation of
    // codegen!  can we do it differently?

    const BaseType& baseRef =
        bdeat_CustomizedTypeFunctions::convertToBaseType(*object);

    makeEncodeProxy(proxy, (BaseType*)&baseRef);
}

template <typename TYPE>
inline
void SerializableObjectProxyUtil::makeEncodeProxy(
                                               SerializableObjectProxy *proxy,
                                               TYPE                    *object,
                                               bdeat_TypeCategory::Choice)
{
    SerializableObjectProxyUtil_ChoiceAccessorProxy accessorProxy(
                                                         proxy,
                                                         object->selectionId(),
                                                         TYPE::CLASS_NAME);

    object->accessSelection(accessorProxy);
}

template <typename TYPE>
inline
void SerializableObjectProxyUtil::makeEncodeProxy(
                                               SerializableObjectProxy *proxy,
                                               TYPE                    *object,
                                               bdeat_TypeCategory::Sequence)
{
    SerializableObjectProxyUtil_SequenceLoader<TYPE,
                                               (TYPE::NUM_ATTRIBUTES > 0)>::
        loadAccessor(proxy, object);
}

template <typename ELEMENT_TYPE>
inline
void SerializableObjectProxyUtil::makeEncodeProxy(
                                             SerializableObjectProxy   *proxy,
                                             bsl::vector<ELEMENT_TYPE> *object,
                                             bdeat_TypeCategory::Array)
{
    proxy->loadArray(object->size(),
                     sizeof(ELEMENT_TYPE),
                     object->size() ? &(*object->begin()) : 0,
                     &makeEncodeProxyRaw<ELEMENT_TYPE>);
}

template <typename TYPE>
inline
void SerializableObjectProxyUtil::makeEncodeProxy(
                                               SerializableObjectProxy *proxy,
                                               TYPE                    *object,
                                               bdeat_TypeCategory::Simple) {
    proxy->loadSimple(object);
}

template <typename TYPE>
inline
void SerializableObjectProxyUtil::makeEncodeProxyRaw(
                                               SerializableObjectProxy *proxy,
                                               void                    *object)
{
    makeEncodeProxy(proxy, reinterpret_cast<TYPE*>(object),
                    typename bdeat_TypeCategory::Select<TYPE>::Type());
    BSLS_ASSERT_SAFE(bdeat_TypeCategory::BDEAT_DYNAMIC_CATEGORY !=
                     proxy->category());
}

template <typename TYPE>
inline
void SerializableObjectProxyUtil::makeDecodeProxyRaw(
                                   SerializableObjectProxy *proxy,
                                   void                    *object)
{
    makeDecodeProxy(proxy, (TYPE*)object,
                        typename bdeat_TypeCategory::Select<TYPE>::Type());
    BSLS_ASSERT_SAFE(bdeat_TypeCategory::BDEAT_DYNAMIC_CATEGORY !=
                     proxy->category());
}

template <typename TYPE>
inline
int SerializableObjectProxyUtil_SequenceManipulatorProxy::operator() (
                                                    const TYPE& object,
                                                    const bdeat_AttributeInfo&)
{
    SerializableObjectProxyUtil::makeDecodeProxyRaw<TYPE>(d_proxy_p,
                                                          (void*)&object);
    return 0;
}

template <typename TYPE>
inline
int SerializableObjectProxyUtil_ChoiceManipulatorProxy::operator()(
                                            const TYPE&                object,
                                            const bdeat_SelectionInfo& info)
{
    SerializableObjectProxyUtil::makeDecodeProxy(d_proxy_p,
                                         const_cast<TYPE*>(&object));
    d_selectionInfo_p = &info;
    return 0;
}

template <typename TYPE>
inline
int SerializableObjectProxyUtil_SequenceAccessorProxy::operator()(
                                            const TYPE& object,
                                            const bdeat_AttributeInfo&)
{
    SerializableObjectProxyUtil::makeEncodeProxyRaw<TYPE>(d_proxy_p,
                                                          (void*)&object);
    return 0;
}

template <typename TYPE>
inline
int SerializableObjectProxyUtil_ChoiceAccessorProxy::operator()(
                                            const TYPE&                object,
                                            const bdeat_SelectionInfo& info)
{
    d_proxy_p->loadChoice(d_selectionId,
                          (void*)&object,
                          &info, d_className,
                          &SerializableObjectProxyUtil::
                                                  makeEncodeProxyRaw<TYPE>);

    return 0;
}

inline
const bdeat_SelectionInfo*
SerializableObjectProxyUtil_ChoiceManipulatorProxy::selectionInfoPtr() const
{
    return d_selectionInfo_p;
}


}  // close namespace baea
}  // close namespace BloombergLP


#endif


// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
