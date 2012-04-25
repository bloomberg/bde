// baea_serializableobjectproxy.cpp   -*-C++-*-
#include <baea_serializableobjectproxy.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baea_serializableobjectproxy_cpp,"$Id$ $CSID$")

#include <bsl_cstring.h>         // for 'bsl::memcmp'

namespace BloombergLP {

namespace baea {

namespace {

bool areEqual(const char* string1, int length1,
              const char* string2, int length2)
    // Return 'true' if the specified 'string1' and 'string2' contains the same
    // characters and have the same lengths as indicated by the specified
    // 'length1' and 'length2', and 'false' otherwise.
{
    if (length1 != length2) {
        return false;                                                 // RETURN
    }
    return 0 == length1 || 0 == bsl::memcmp(string1, string2, length1);
}

}  // close unnamed namespace

// IMPLEMENTATION NOTE: Overloads for non-templatized Visitors
//
// Some decoders, in some situations, use instantiations of
// bdef_Function<int(*)(TYPE*)> where TYPE is the ElementType of
// SerializableObjectProxy...which is SerializableObjectProxy.  This is in
// contrast to the usual approach of using an object with a function-call
// (operator()) method with a templatized argument.  In these situations,
// passing a SerializableObjectProxy_NullableAdapter breaks compilation -- but
// is unnecessary, given that they are not attempting to detect anything about
// the type via metaprogramming.  They can be passed the
// SerializableObjectProxy directly.

int SerializableObjectProxy::manipulateContainedElement(
                 SerializableObjectProxy  *proxy,
                 bdef_Function<int(*)(SerializableObjectProxy*)>&  manipulator)
{
    return manipulator(proxy);
}

int SerializableObjectProxy::manipulateContainedElement(
              SerializableObjectProxy  *proxy,
              bdef_Function<int(*)(SerializableObjectProxy*,
                                   const bdeat_SelectionInfo&)>&  manipulator,
              const bdeat_SelectionInfo& info)
{
    return manipulator(proxy, info);
}

int SerializableObjectProxy::manipulateContainedElement(
              SerializableObjectProxy  *proxy,
              bdef_Function<int(*)(SerializableObjectProxy*,
                                   const bdeat_AttributeInfo&)>&  manipulator,
              const bdeat_AttributeInfo& info)
{
    return manipulator(proxy, info);
}

// 'load' implementations

void SerializableObjectProxy::loadSimple(char* value)
{
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { value, SimplePointer::TYPE_CHAR };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void SerializableObjectProxy::loadSimple(unsigned char* value)
{
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { value, SimplePointer::TYPE_UCHAR };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void SerializableObjectProxy::loadSimple(short* value)
{
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { value, SimplePointer::TYPE_SHORT };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void SerializableObjectProxy::loadSimple(int* value)
{
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { value, SimplePointer::TYPE_INT };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void SerializableObjectProxy::loadSimple(bsls_Types::Int64* value)
{
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { value, SimplePointer::TYPE_INT64 };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void SerializableObjectProxy::loadSimple(unsigned short* value)
{
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { value, SimplePointer::TYPE_USHORT };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void SerializableObjectProxy::loadSimple(unsigned int* value)
{
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { value, SimplePointer::TYPE_UINT };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void SerializableObjectProxy::loadSimple(bsls_Types::Uint64* value)
{
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { value, SimplePointer::TYPE_UINT64 };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void SerializableObjectProxy::loadSimple(float* value)
{
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { value, SimplePointer::TYPE_FLOAT };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void SerializableObjectProxy::loadSimple(double* value)
{
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { value, SimplePointer::TYPE_DOUBLE };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void SerializableObjectProxy::loadSimple(bsl::string* value)
{
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { value, SimplePointer::TYPE_STRING };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void SerializableObjectProxy::loadSimple(bdet_Datetime* value)
{
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { value, SimplePointer::TYPE_DATETIME };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void SerializableObjectProxy::loadSimple(bdet_Date* value)
{
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { value, SimplePointer::TYPE_DATE };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void SerializableObjectProxy::loadSimple(bdet_Time* value)
{
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { value, SimplePointer::TYPE_TIME };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void SerializableObjectProxy::loadSimple(bool* value)
{
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { value, SimplePointer::TYPE_BOOL };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void SerializableObjectProxy::loadSimple(bdet_DatetimeTz* value)
{
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { value, SimplePointer::TYPE_DATETIMETZ };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void SerializableObjectProxy::loadSimple(bdet_DateTz* value)
{
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { value, SimplePointer::TYPE_DATETZ };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void SerializableObjectProxy::loadSimple(bdet_TimeTz* value)
{
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { value, SimplePointer::TYPE_TIMETZ };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void SerializableObjectProxy::loadChoice(
                       int selectionId,
                       void *object,
                       const bdeat_SelectionInfo *info,
                       const char *className,
                       SerializableObjectProxyFunctions::Accessor accessor)
{
    d_object_p = object;
    d_category = bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY;
    new (d_objectInfoArena.buffer()) ObjectInfo(
                                ChoiceEncodeInfo(selectionId, info,
                                                 accessor, className));
}

void SerializableObjectProxy::loadChoice(
               int numSelections,
               void *choice,
               const bdeat_SelectionInfo* selectionInfoArray,
               SerializableObjectProxyFunctions::AccessorAndExtractor accessor,
               SerializableObjectProxyFunctions::Chooser chooser)
{
    d_object_p = choice;
    d_category = bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY;
    new (d_objectInfoArena.buffer()) ObjectInfo(
                                ChoiceDecodeInfo(numSelections,
                                                 selectionInfoArray,
                                                 accessor,
                                                 chooser));
}

int SerializableObjectProxy::loadSequenceElementDecodeProxy(
                                       SerializableObjectProxy    *proxy,
                                       const bdeat_AttributeInfo **attrInfo,
                                       int                         elementId)
{
    BSLS_ASSERT(d_objectInfo.is<SequenceInfo>());

    const SequenceInfo& info = d_objectInfo.the<SequenceInfo>();
    for(int i = 0; i < info.d_numAttributes; ++i)
    {
        if (info.d_attributeInfo_p[i].id() == elementId) {
            info.d_accessor(proxy, *this, i);
            *attrInfo = info.d_attributeInfo_p + i;
            return 0;                                                 // RETURN
        }
    }
    return -1;
}

int SerializableObjectProxy::loadSequenceElementDecodeProxy(
                                 SerializableObjectProxy    *proxy,
                                 const bdeat_AttributeInfo **attrInfo,
                                 const char                 *elementName,
                                 int                         elementNameLength)
{
    BSLS_ASSERT(d_objectInfo.is<SequenceInfo>());

    const SequenceInfo& info = d_objectInfo.the<SequenceInfo>();

    for(int i = 0; i < info.d_numAttributes; ++i)
    {
        *attrInfo = info.d_attributeInfo_p + i;

        if (areEqual(elementName, elementNameLength,
                     (*attrInfo)->name(), (*attrInfo)->nameLength()))
        {
            info.d_accessor(proxy, *this, i);
            return 0;                                                 // RETURN
        }
    }
    return -1;
}

int SerializableObjectProxy::choiceMakeSelection(int selectionId)
{
    BSLS_ASSERT(d_objectInfo.is<ChoiceDecodeInfo>());

    ChoiceDecodeInfo& info = d_objectInfo.the<ChoiceDecodeInfo>();

    // save the selectionId in d_objectInfo.  At this writing, this is only
    // necessary in SAFE mode builds -- after invoking this function, some
    // decoders will then assert (in SAFE mode) that the selection ID matches
    // what they just set.  But it might be queried in this way in other
    // modes in the future and there's no good reason not to save it.
    //
    // Note that if the chooser fails, this value will be wrong.  We
    // could use an 'if' condition to prevent that, but it's not worth the
    // expense.

    info.d_currentSelection = selectionId;

    return info.d_chooser(d_object_p, selectionId);
}

int SerializableObjectProxy::choiceMakeSelection(
                                               const char *selectionName,
                                               int         selectionNameLength)
{
    BSLS_ASSERT(d_objectInfo.is<ChoiceDecodeInfo>());

    const ChoiceDecodeInfo& info = d_objectInfo.the<ChoiceDecodeInfo>();
    const bdeat_SelectionInfo* end =
        info.d_selectionInfoArray_p + info.d_numSelections;
    for (const bdeat_SelectionInfo *selectionInfo =
                                                   info.d_selectionInfoArray_p;
         selectionInfo < end;
         ++selectionInfo) {
        if (areEqual(selectionName, selectionNameLength,
                     selectionInfo->name(), selectionInfo->nameLength())) {
            return choiceMakeSelection(selectionInfo->id());          // RETURN
        }
    }
    return -1;
}

bool SerializableObjectProxy::isNull() const
{
    return d_object_p == 0 ||
        (d_objectInfo.is<NullableDecodeInfo>() &&
         d_objectInfo.the<NullableDecodeInfo>().d_fetcher(d_object_p) == 0);
}

bool SerializableObjectProxy::choiceHasSelection(int selectionId) const
{
    BSLS_ASSERT(d_objectInfo.is<ChoiceDecodeInfo>());

    const ChoiceDecodeInfo& info = d_objectInfo.the<ChoiceDecodeInfo>();
    const bdeat_SelectionInfo* end =
        info.d_selectionInfoArray_p + info.d_numSelections;
    for (const bdeat_SelectionInfo* selectionInfo =
                                                   info.d_selectionInfoArray_p;
         selectionInfo < end;
         ++selectionInfo) {
        if (selectionInfo->id() == selectionId) {
            return true;                                              // RETURN
        }
    }
    return false;
}

bool SerializableObjectProxy::choiceHasSelection(
                                     const char *selectionName,
                                     int         selectionNameLength) const
{
    BSLS_ASSERT(d_objectInfo.is<ChoiceDecodeInfo>());

    const ChoiceDecodeInfo& info = d_objectInfo.the<ChoiceDecodeInfo>();
    const bdeat_SelectionInfo* end =
        info.d_selectionInfoArray_p + info.d_numSelections;
    for (const bdeat_SelectionInfo* selectionInfo =
                                                   info.d_selectionInfoArray_p;
         selectionInfo < end;
         ++selectionInfo) {
        if (areEqual(selectionName, selectionNameLength,
                     selectionInfo->name(), selectionInfo->nameLength())) {
            return true;                                              // RETURN
        }
    }
    return false;
}

void SerializableObjectProxy::loadSequence(
                    int numAttributes,
                    void *object,
                    const bdeat_AttributeInfo *info,
                    const char *className,
                    SerializableObjectProxyFunctions::ElementAccessor accessor)
{
    d_object_p = object;
    d_category = bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY;
    new (d_objectInfoArena.buffer()) ObjectInfo(
                                SequenceInfo(numAttributes, info,
                                             className, accessor));
}

void SerializableObjectProxy::loadArray(
                  void                                            *array,
                  int                                              length,
                  int                                              elementSize,
                  void                                            *begin,
                  SerializableObjectProxyFunctions::Resizer        resizer,
                  SerializableObjectProxyFunctions::Accessor       accessor)
{
    d_object_p = array;
    d_category = bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY;
    new (d_objectInfoArena.buffer()) ObjectInfo(
                         ArrayDecodeInfo(length, elementSize, begin,
                                         resizer, accessor));
}

void SerializableObjectProxy::loadArray(
                       int length,
                       int elementSize,
                       void *begin,
                       SerializableObjectProxyFunctions::Accessor accessor)
{
    d_object_p = begin;
    d_category = bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY;
    new (d_objectInfoArena.buffer()) ObjectInfo(
                                         ArrayEncodeInfo(length, elementSize,
                                                         accessor));
}

void SerializableObjectProxy::loadEnumeration(
                       int value, const bdeat_EnumeratorInfo* infoArray,
                       int numEnumerators)
{
    EnumEncodeInfo info = { value, infoArray, numEnumerators};
    d_category = bdeat_TypeCategory::BDEAT_ENUMERATION_CATEGORY;
    new (d_objectInfoArena.buffer()) ObjectInfo(info);
}

void SerializableObjectProxy::loadEnumeration(
                void                                           *object,
                SerializableObjectProxyFunctions::IntSetter     intSetter,
                SerializableObjectProxyFunctions::StringSetter  stringSetter,
                const bdeat_EnumeratorInfo                     *infoArray,
                int                                             numEnumerators)
{
    d_object_p = object;
    EnumDecodeInfo info = { intSetter, stringSetter,
                            infoArray, numEnumerators };
    d_category = bdeat_TypeCategory::BDEAT_ENUMERATION_CATEGORY;
    new (d_objectInfoArena.buffer()) ObjectInfo(info);
}

void SerializableObjectProxy::loadNullable(
                       void *object,
                       SerializableObjectProxyFunctions::Accessor accessor)
{
    d_object_p = object; // 0 if null value
    d_category = bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY;
    new (d_objectInfoArena.buffer()) ObjectInfo(accessor);
}

void SerializableObjectProxy::loadNullable(
                 void *object,
                 SerializableObjectProxyFunctions::Accessor accessor,
                 SerializableObjectProxyFunctions::Manipulator valueMaker,
                 SerializableObjectProxyFunctions::ObjectFetcher valueFetcher)
{
    d_object_p = object;
    d_category = bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY;
    NullableDecodeInfo info = { valueMaker, valueFetcher, accessor };
    new (d_objectInfoArena.buffer()) ObjectInfo(info);
}

void SerializableObjectProxy::makeValue()
{
    BSLS_ASSERT(d_objectInfo.is<NullableDecodeInfo>());

    d_objectInfo.the<NullableDecodeInfo>().d_valueMaker(d_object_p);
}

int SerializableObjectProxy::enumFromInt(int value)
{
    BSLS_ASSERT(d_objectInfo.is<EnumDecodeInfo>());

    return d_objectInfo.the<EnumDecodeInfo>().d_intSetter(d_object_p,
                                                          value);
}

int SerializableObjectProxy::enumFromString(const char *value, int length)
{
    BSLS_ASSERT(d_objectInfo.is<EnumDecodeInfo>());

    return d_objectInfo.the<EnumDecodeInfo>().d_stringSetter(d_object_p,
                                                             value, length);
}

int SerializableObjectProxy::enumToInt() const
{
    BSLS_ASSERT(d_objectInfo.is<EnumEncodeInfo>());

    return d_objectInfo.the<EnumEncodeInfo>().d_intVal;
}

const char* SerializableObjectProxy::enumToString() const
{
    BSLS_ASSERT(d_objectInfo.is<EnumEncodeInfo>());

    const EnumEncodeInfo& info = d_objectInfo.the<EnumEncodeInfo>();
    const bdeat_EnumeratorInfo* end = info.d_infoArray_p +
                                      info.d_infoArraySize;
    for (const bdeat_EnumeratorInfo *enumInfo = info.d_infoArray_p;
         enumInfo < end; ++enumInfo) {
        if (enumInfo->d_value == info.d_intVal) {
            return enumInfo->d_name_p;                                // RETURN
        }
    }
    return 0;
}

bool SerializableObjectProxy::sequenceHasAttribute(int attributeId) const
{
    BSLS_ASSERT(d_objectInfo.is<SequenceInfo>());

    const SequenceInfo& info = d_objectInfo.the<SequenceInfo>();

    const bdeat_AttributeInfo *ptr = info.d_attributeInfo_p;
    const bdeat_AttributeInfo *end = ptr + info.d_numAttributes;

    for (; ptr < end; ++ptr) {
        if (ptr->id() == attributeId) {
            return true;                                              // RETURN
        }
    }
    return false;
}

bool SerializableObjectProxy::sequenceHasAttribute(const char* name,
                                                   int nameLength) const
{
    BSLS_ASSERT(d_objectInfo.is<SequenceInfo>());

    const SequenceInfo& info = d_objectInfo.the<SequenceInfo>();
    for(int i = 0; i < info.d_numAttributes; ++i) {
        if (areEqual(name, nameLength,
                     info.d_attributeInfo_p[i].name(),
                     info.d_attributeInfo_p[i].nameLength()))
        {
            return true;                                              // RETURN
        }
    }
    return false;
}

void SerializableObjectProxy::resize(bsl::size_t newSize)
{
    BSLS_ASSERT(d_objectInfo.is<ArrayDecodeInfo>());

    ArrayDecodeInfo& info = d_objectInfo.the<ArrayDecodeInfo>();

    info.d_resizer(d_object_p, &info.d_begin, newSize);
    info.d_length = newSize;
}

void SerializableObjectProxy::loadArrayElementEncodeProxy(
                                     SerializableObjectProxy *elementProxy,
                                     int                      index) const
{
    BSLS_ASSERT(d_objectInfo.is<ArrayEncodeInfo>());

    const ArrayEncodeInfo& info = d_objectInfo.the<ArrayEncodeInfo>();
    void* address = (char*)d_object_p + info.d_elementSize * index;
    info.d_accessor(elementProxy, address);
}

void SerializableObjectProxy::loadArrayElementDecodeProxy(
                                     SerializableObjectProxy *elementProxy,
                                     int                      index) const
{
    BSLS_ASSERT(d_objectInfo.is<ArrayDecodeInfo>());

    const ArrayDecodeInfo& info = d_objectInfo.the<ArrayDecodeInfo>();
    void* address = (char*)info.d_begin + info.d_elementSize * index;
    info.d_accessor(elementProxy, address);
}

bsl::size_t SerializableObjectProxy::size() const
{
    bool isEncode = d_objectInfo.is<ArrayEncodeInfo>();

    BSLS_ASSERT(isEncode || d_objectInfo.is<ArrayDecodeInfo>());

    if (isEncode) {
        return d_objectInfo.the<ArrayEncodeInfo>().d_length;          // RETURN
    }
    else {
        return d_objectInfo.the<ArrayDecodeInfo>().d_length;          // RETURN
    }
}

const char *SerializableObjectProxy::className() const
{
    if (d_objectInfo.is<ChoiceEncodeInfo>()) {
        return d_objectInfo.the<ChoiceEncodeInfo>().d_className_p;    // RETURN
    }
    if (d_objectInfo.is<SequenceInfo>()) {
        return d_objectInfo.the<SequenceInfo>().d_className_p;        // RETURN
    }

    // 'className' should only be required when encoding top-level elements,
    // which should in turn only be sequences/choices for BAS.  We don't bother
    // storing className otherwise.
    //
    // bdeat_TypeName documents that we're allowed to return 0.

    return 0;
}

int SerializableObjectProxy::selectionId() const
{
    if (d_objectInfo.is<ChoiceEncodeInfo>()) {
        return d_objectInfo.the<ChoiceEncodeInfo>().d_selectionId;    // RETURN
    }
    else if (d_objectInfo.is<ChoiceDecodeInfo>()) {
        return d_objectInfo.the<ChoiceDecodeInfo>().d_currentSelection;
                                                                      // RETURN
    }
    else {
        return bdeat_ChoiceFunctions::BDEAT_UNDEFINED_SELECTION_ID;   // RETURN
    }
}

}  // close namespace baea
}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
