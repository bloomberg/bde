// baea_serializableobjectproxy.cpp                                   -*-C++-*-
#include <baea_serializableobjectproxy.h>

#include <bdeat_formattingmode.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baea_serializableobjectproxy_cpp,"$Id$ $CSID$")

#include <bsl_cstring.h>         // for 'bsl::memcmp'

namespace BloombergLP {

namespace {

bool areEqual(const char* string1, int length1,
              const char* string2, int length2)
    // Return 'true' if the specified 'string1' and 'string2' contain the same
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
// 'baea_SerializableObjectProxy', which is 'baea_SerializableObjectProxy'.
// This is in contrast to the usual approach of using a functor.  In these
// situations, it is unnecessary to pass in a
// 'baea_SerializableObjectProxy_NullableAdapter', but the
// 'manipulateContainedElement' methods that takes in a functor
// will still pass in a 'baea_SerializableObjectProxy_NullableAdapter'.  This
// will break compilation, which is why an overload for 'bdef_Function' is
// necessary.

// PRIVATE CLASS METHODS
int baea_SerializableObjectProxy::manipulateContainedElement(
            baea_SerializableObjectProxy                          *proxy,
            bdef_Function<int(*)(baea_SerializableObjectProxy*)>&  manipulator)
{
    return manipulator(proxy);
}

int baea_SerializableObjectProxy::manipulateContainedElement(
              baea_SerializableObjectProxy                       *proxy,
              bdef_Function<int(*)(baea_SerializableObjectProxy*,
                                   const bdeat_SelectionInfo&)>&  manipulator,
              const bdeat_SelectionInfo&                          info)
{
    return manipulator(proxy, info);
}

int baea_SerializableObjectProxy::manipulateContainedElement(
              baea_SerializableObjectProxy                       *proxy,
              bdef_Function<int(*)(baea_SerializableObjectProxy*,
                                   const bdeat_AttributeInfo&)>&  manipulator,
              const bdeat_AttributeInfo&                          info)
{
    return manipulator(proxy, info);
}

// PRIVATE ACCESSORS
void baea_SerializableObjectProxy::loadArrayElementEncodeProxy(
                                    baea_SerializableObjectProxy *elementProxy,
                                    int                           index) const
{
    BSLS_ASSERT(d_objectInfo.is<ArrayEncodeInfo>());

    const ArrayEncodeInfo& info = d_objectInfo.the<ArrayEncodeInfo>();
    void *address = (char*)d_object_p + info.d_elementSize * index;
    info.d_loader(elementProxy, address);
}

void baea_SerializableObjectProxy::loadArrayElementDecodeProxy(
                                    baea_SerializableObjectProxy *elementProxy,
                                    int                           index) const
{
    BSLS_ASSERT(d_objectInfo.is<ArrayDecodeInfo>());

    const ArrayDecodeInfo& info = d_objectInfo.the<ArrayDecodeInfo>();
    void* address = (char*)info.d_begin + info.d_elementSize * index;
    info.d_loader(elementProxy, address);
}

int baea_SerializableObjectProxy::loadSequenceElementProxy(
                                baea_SerializableObjectProxy  *proxy,
                                const bdeat_AttributeInfo    **attrInfo,
                                int                            elementId) const
{
    BSLS_ASSERT(d_objectInfo.is<SequenceInfo>());

    const SequenceInfo& info = d_objectInfo.the<SequenceInfo>();
    for(int i = 0; i < info.d_numAttributes; ++i) {
        if (info.d_attributeInfo_p[i].id() == elementId) {
            info.d_loader(proxy, *this, elementId);
            *attrInfo = info.d_attributeInfo_p + i;
            return 0;                                                 // RETURN
        }
    }
    return -1;
}

int baea_SerializableObjectProxy::loadSequenceElementProxy(
                        baea_SerializableObjectProxy  *proxy,
                        const bdeat_AttributeInfo    **attrInfo,
                        const char                    *elementName,
                        int                            elementNameLength) const
{
    BSLS_ASSERT(d_objectInfo.is<SequenceInfo>());

    const SequenceInfo& info = d_objectInfo.the<SequenceInfo>();

    const bdeat_AttributeInfo *untaggedAttrInfo = 0;
    for(int i = 0; i < info.d_numAttributes; ++i) {
        *attrInfo = info.d_attributeInfo_p + i;

        if (areEqual(elementName,
                     elementNameLength,
                     (*attrInfo)->name(),
                     (*attrInfo)->nameLength())) {
            info.d_loader(proxy, *this, (*attrInfo)->id());
            return 0;                                                 // RETURN
        }
        if ((*attrInfo)->formattingMode() 
            & bdeat_FormattingMode::BDEAT_UNTAGGED) {
            untaggedAttrInfo = *attrInfo;

            // maybe an anonymous choice having a selection with this name?

            info.d_loader(proxy, *this, untaggedAttrInfo->id());
            if (proxy->d_objectInfo.is<ChoiceDecodeInfo>() &&
                proxy->choiceHasSelection(elementName, elementNameLength)) {
                // We don't need to "make" the selection here
                *attrInfo = untaggedAttrInfo;
                return 0;                                             // RETURN
            }
        }
    }

    if (untaggedAttrInfo) {
        // maybe a nullable anonymous choice?

        info.d_loader(proxy, *this, untaggedAttrInfo->id());

        if (proxy->d_objectInfo.is<NullableDecodeInfo>()) {
            // No way to check whether this is a nullable choice
            // without disrupting the decoder, but just return this to 
            // the decoder and let it try to decode
            *attrInfo = untaggedAttrInfo;
            return 0;                                                // RETURN
        }
    }
    return -1;

}

// MANIPULATORS
void baea_SerializableObjectProxy::resize(bsl::size_t newSize)
{
    BSLS_ASSERT(d_objectInfo.is<ArrayDecodeInfo>());

    ArrayDecodeInfo& info = d_objectInfo.the<ArrayDecodeInfo>();

    info.d_resizer(d_object_p, &info.d_begin, newSize);
    info.d_length = newSize;
}

int baea_SerializableObjectProxy::choiceMakeSelection(int selectionId)
{
    BSLS_ASSERT(d_objectInfo.is<ChoiceDecodeInfo>());

    ChoiceDecodeInfo& info = d_objectInfo.the<ChoiceDecodeInfo>();

    // Save the selectionId in d_objectInfo.  At this writing, this is only
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

int baea_SerializableObjectProxy::choiceMakeSelection(
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

int baea_SerializableObjectProxy::enumFromInt(int value)
{
    BSLS_ASSERT(d_objectInfo.is<EnumDecodeInfo>());

    return d_objectInfo.the<EnumDecodeInfo>().d_intSetter(d_object_p, value);
}

int baea_SerializableObjectProxy::enumFromString(const char *value, int length)
{
    BSLS_ASSERT(d_objectInfo.is<EnumDecodeInfo>());

    return d_objectInfo.the<EnumDecodeInfo>().d_stringSetter(d_object_p,
                                                             value,
                                                             length);
}

void baea_SerializableObjectProxy::makeValue()
{
    BSLS_ASSERT(d_objectInfo.is<NullableDecodeInfo>());

    d_objectInfo.the<NullableDecodeInfo>().d_valueMaker(d_object_p);
}

// 'load' implementations

void baea_SerializableObjectProxy::loadSimple(char* value)
{
    d_object_p = value;
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { SimplePointer::TYPE_CHAR };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void baea_SerializableObjectProxy::loadSimple(unsigned char* value)
{
    d_object_p = value;
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { SimplePointer::TYPE_UCHAR };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void baea_SerializableObjectProxy::loadSimple(short* value)
{
    d_object_p = value;
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { SimplePointer::TYPE_SHORT };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void baea_SerializableObjectProxy::loadSimple(int* value)
{
    d_object_p = value;
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { SimplePointer::TYPE_INT };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void baea_SerializableObjectProxy::loadSimple(bsls_Types::Int64* value)
{
    d_object_p = value;
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { SimplePointer::TYPE_INT64 };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void baea_SerializableObjectProxy::loadSimple(unsigned short* value)
{
    d_object_p = value;
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { SimplePointer::TYPE_USHORT };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void baea_SerializableObjectProxy::loadSimple(unsigned int* value)
{
    d_object_p = value;
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { SimplePointer::TYPE_UINT };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void baea_SerializableObjectProxy::loadSimple(bsls_Types::Uint64* value)
{
    d_object_p = value;
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { SimplePointer::TYPE_UINT64 };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void baea_SerializableObjectProxy::loadSimple(float* value)
{
    d_object_p = value;
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { SimplePointer::TYPE_FLOAT };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void baea_SerializableObjectProxy::loadSimple(double* value)
{
    d_object_p = value;
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { SimplePointer::TYPE_DOUBLE };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void baea_SerializableObjectProxy::loadSimple(bsl::string* value)
{
    d_object_p = value;
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { SimplePointer::TYPE_STRING };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void baea_SerializableObjectProxy::loadSimple(bdet_Datetime* value)
{
    d_object_p = value;
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { SimplePointer::TYPE_DATETIME };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void baea_SerializableObjectProxy::loadSimple(bdet_Date* value)
{
    d_object_p = value;
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { SimplePointer::TYPE_DATE };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void baea_SerializableObjectProxy::loadSimple(bdet_Time* value)
{
    d_object_p = value;
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { SimplePointer::TYPE_TIME };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void baea_SerializableObjectProxy::loadSimple(bool* value)
{
    d_object_p = value;
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { SimplePointer::TYPE_BOOL };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void baea_SerializableObjectProxy::loadSimple(bdet_DatetimeTz* value)
{
    d_object_p = value;
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { SimplePointer::TYPE_DATETIMETZ };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void baea_SerializableObjectProxy::loadSimple(bdet_DateTz* value)
{
    d_object_p = value;
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { SimplePointer::TYPE_DATETZ };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void baea_SerializableObjectProxy::loadSimple(bdet_TimeTz* value)
{
    d_object_p = value;
    d_category = bdeat_TypeCategory::BDEAT_SIMPLE_CATEGORY;
    SimplePointer pointer = { SimplePointer::TYPE_TIMETZ };
    new (d_objectInfoArena.buffer()) ObjectInfo(pointer);
}

void baea_SerializableObjectProxy::loadChoiceForEncoding(
                      void                                          *object,
                      const bdeat_SelectionInfo                     *info,
                      const char                                    *className,
                      baea_SerializableObjectProxyFunctions::Loader  loader)
{
    d_object_p = object;
    d_category = bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY;
    new (d_objectInfoArena.buffer()) ObjectInfo(ChoiceEncodeInfo(info,
                                                                 loader,
                                                                 className));
}

void baea_SerializableObjectProxy::loadChoiceForDecoding(
    int                                                     numSelections,
    void                                                   *object,
    const bdeat_SelectionInfo                              *selectionInfoArray,
    baea_SerializableObjectProxyFunctions::SelectionLoader  loader,
    baea_SerializableObjectProxyFunctions::Chooser          chooser)
{
    d_object_p = object;
    d_category = bdeat_TypeCategory::BDEAT_CHOICE_CATEGORY;
    new (d_objectInfoArena.buffer()) ObjectInfo(
                                           ChoiceDecodeInfo(numSelections,
                                                            selectionInfoArray,
                                                            loader,
                                                            chooser));
}

void baea_SerializableObjectProxy::loadSequence(
           int                                                   numAttributes,
           void                                                 *object,
           const bdeat_AttributeInfo                            *info,
           const char                                           *className,
           baea_SerializableObjectProxyFunctions::ElementLoader  loader)
{
    d_object_p = object;
    d_category = bdeat_TypeCategory::BDEAT_SEQUENCE_CATEGORY;
    new (d_objectInfoArena.buffer()) ObjectInfo(SequenceInfo(numAttributes,
                                                             info,
                                                             className,
                                                             loader));
}

void baea_SerializableObjectProxy::loadByteArray(bsl::vector<char> *object)
{
    loadArrayForEncoding(object->size(), sizeof(char), object, 0);
}

void baea_SerializableObjectProxy::loadArrayForEncoding(
                     int                                           length,
                     int                                           elementSize,
                     void                                         *begin,
                     baea_SerializableObjectProxyFunctions::Loader loader)
{
    d_object_p = begin;
    d_category = bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY;
    new (d_objectInfoArena.buffer()) ObjectInfo(ArrayEncodeInfo(length,
                                                                elementSize,
                                                                loader));
}

void baea_SerializableObjectProxy::loadArrayForDecoding(
                    void                                          *object,
                    int                                            length,
                    int                                            elementSize,
                    void                                          *begin,
                    baea_SerializableObjectProxyFunctions::Resizer resizer,
                    baea_SerializableObjectProxyFunctions::Loader  loader)
{
    d_object_p = object;
    d_category = bdeat_TypeCategory::BDEAT_ARRAY_CATEGORY;
    new (d_objectInfoArena.buffer()) ObjectInfo(ArrayDecodeInfo(length,
                                                                elementSize,
                                                                begin,
                                                                resizer,
                                                                loader));
}

void baea_SerializableObjectProxy::loadNullableForEncoding(
                         void                                          *object,
                         baea_SerializableObjectProxyFunctions::Loader  loader)
{
    d_object_p = object; // 0 if null value
    d_category = bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY;
    new (d_objectInfoArena.buffer()) ObjectInfo(loader);
}

void baea_SerializableObjectProxy::loadNullableForDecoding(
            void                                                 *object,
            baea_SerializableObjectProxyFunctions::Loader         loader,
            baea_SerializableObjectProxyFunctions::ValueMaker     valueMaker,
            baea_SerializableObjectProxyFunctions::ObjectFetcher  valueFetcher)
{
    d_object_p = object;
    d_category = bdeat_TypeCategory::BDEAT_NULLABLE_VALUE_CATEGORY;
    NullableDecodeInfo info = { valueMaker, valueFetcher, loader };
    new (d_objectInfoArena.buffer()) ObjectInfo(info);
}

void baea_SerializableObjectProxy::loadEnumerationForEncoding(
                                    int                         value,
                                    const bdeat_EnumeratorInfo *infoArray,
                                    int                         numEnumerators)
{
    d_object_p = 0;
    EnumEncodeInfo info = { value, infoArray, numEnumerators};
    d_category = bdeat_TypeCategory::BDEAT_ENUMERATION_CATEGORY;
    new (d_objectInfoArena.buffer()) ObjectInfo(info);
}

void baea_SerializableObjectProxy::loadEnumerationForDecoding(
           void                                                *object,
           baea_SerializableObjectProxyFunctions::IntSetter     intSetter,
           baea_SerializableObjectProxyFunctions::StringSetter  stringSetter,
           const bdeat_EnumeratorInfo                          *infoArray,
           int                                                  numEnumerators)
{
    d_object_p = object;
    EnumDecodeInfo info = { intSetter, stringSetter,
                            infoArray, numEnumerators };
    d_category = bdeat_TypeCategory::BDEAT_ENUMERATION_CATEGORY;
    new (d_objectInfoArena.buffer()) ObjectInfo(info);
}

// ACCESSORS
const char *baea_SerializableObjectProxy::className() const
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

int baea_SerializableObjectProxy::selectionId() const
{
    if (d_objectInfo.is<ChoiceEncodeInfo>()) {
        return d_objectInfo.the<ChoiceEncodeInfo>().d_selectionInfo_p->id();
                                                                      // RETURN
    }
    else if (d_objectInfo.is<ChoiceDecodeInfo>()) {
        return d_objectInfo.the<ChoiceDecodeInfo>().d_currentSelection;
                                                                      // RETURN
    }
    else {
        return bdeat_ChoiceFunctions::BDEAT_UNDEFINED_SELECTION_ID;   // RETURN
    }
}

bsl::size_t baea_SerializableObjectProxy::size() const
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

bool baea_SerializableObjectProxy::isNull() const
{
    BSLS_ASSERT_SAFE(d_objectInfo.is<NullableEncodeInfo>() ||
                     d_objectInfo.is<NullableDecodeInfo>());

    return d_object_p == 0 ||
        (d_objectInfo.is<NullableDecodeInfo>() &&
         d_objectInfo.the<NullableDecodeInfo>().d_fetcher(d_object_p) == 0);
}

bool baea_SerializableObjectProxy::choiceHasSelection(int selectionId) const
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

bool baea_SerializableObjectProxy::choiceHasSelection(
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

bool baea_SerializableObjectProxy::sequenceHasAttribute(int attributeId) const
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

bool baea_SerializableObjectProxy::sequenceHasAttribute(
                                                  const char* name,
                                                  int         nameLength) const
{
    BSLS_ASSERT(d_objectInfo.is<SequenceInfo>());

    const SequenceInfo& info = d_objectInfo.the<SequenceInfo>();
    int untaggedAttributeId = -1;
    for(int i = 0; i < info.d_numAttributes; ++i) {
        if (areEqual(name, nameLength,
                     info.d_attributeInfo_p[i].name(),
                     info.d_attributeInfo_p[i].nameLength()))
        {
            return true;                                              // RETURN
        }
        if (info.d_attributeInfo_p[i].formattingMode() 
            & bdeat_FormattingMode::BDEAT_UNTAGGED) {
            // maybe an anonymous choice having a selection with this name?

            untaggedAttributeId = info.d_attributeInfo_p[i].id();

            baea_SerializableObjectProxy untaggedInfo;
            info.d_loader(&untaggedInfo, *this, untaggedAttributeId);
            if (untaggedInfo.d_objectInfo.is<ChoiceDecodeInfo>() &&
                untaggedInfo.choiceHasSelection(name, nameLength)) {
                return true; // RETURN
            }
        }
    }

    if (0 <= untaggedAttributeId) {

        // If it's an untagged nullable, just return 'true' and let the
        // decoder give it a shot.  We don't have enough info to check whether
        // the nullable is a Choice and has this selection.  

        baea_SerializableObjectProxy untaggedInfo;
        info.d_loader(&untaggedInfo, *this, untaggedAttributeId);
            
        if (untaggedInfo.d_objectInfo.is<NullableDecodeInfo>()) {
            return true;                                             // RETURN
        }                                                     
    }
    return false;
}

int baea_SerializableObjectProxy::enumToInt() const
{
    BSLS_ASSERT(d_objectInfo.is<EnumEncodeInfo>());

    return d_objectInfo.the<EnumEncodeInfo>().d_intVal;
}

const char* baea_SerializableObjectProxy::enumToString() const
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

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
