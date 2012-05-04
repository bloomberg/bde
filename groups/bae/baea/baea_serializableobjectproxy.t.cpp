// baea_serializableobjectproxy.t.cpp   -*-C++-*-
#include <baea_serializableobjectproxy.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bslma_sequentialallocator.h>

#include <bdeut_nullablevalue.h>

#include <cstdlib>

using namespace BloombergLP;
using namespace BloombergLP::baea;
using namespace BloombergLP::bdeat_TypeCategoryFunctions;

using bsl::cout;
using bsl::endl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// CREATORS
// [  ] SerializableObjectProxy;
//
// NO-OP FUNCTIONS FOR INTEGRATION
// [  ] STREAM& bdexStreamIn(STREAM& stream, int);
// [  ] STREAM& bdexStreamOut(STREAM& stream, int) const;
//
// MANIPULATORS
// [ 6] void resize(size_t newSize);
// [ 7] int choiceMakeSelection(int selectionId);
// [ 7] int choiceMakeSelection(const char *name, int length);
// [ 7] int choiceManipulateSelection(MANIPULATOR& manipulator);
// [ 6] int arrayManipulateElement(MANIPULATOR& manipulator, int index);
// [ 8] int sequenceManipulateAttribute(MANIPULATOR& manipulator, int id);
// [ 8] int sequenceManipulateAttribute(MANIPULATOR& m, const char*  n, int l);
// [ 4] int manipulateSimple(MANIPULATOR& manipulator);
// [ 9] int manipulateNullable(MANIPULATOR& manipulator);
// [ 5] int enumFromInt(int value);
// [ 5] int enumFromString(const char *stringValue, int stringLength);
// [ 9] void makeValue();
// [ 4] void loadSimple(char               *value);
// [ 4] void loadSimple(unsigned char      *value);
// [ 4] void loadSimple(short              *value);
// [ 4] void loadSimple(int                *value);
// [ 4] void loadSimple(bsls_Types::Int64  *value);
// [ 4] void loadSimple(unsigned short     *value);
// [ 4] void loadSimple(unsigned int       *value);
// [ 4] void loadSimple(bsls_Types::Uint64 *value);
// [ 4] void loadSimple(float              *value);
// [ 4] void loadSimple(double             *value);
// [ 4] void loadSimple(bsl::string        *value);
// [ 4] void loadSimple(bdet_Datetime      *value);
// [ 4] void loadSimple(bdet_Date          *value);
// [ 4] void loadSimple(bdet_Time          *value);
// [ 4] void loadSimple(bool               *value);
// [ 4] void loadSimple(bdet_DatetimeTz    *value);
// [ 4] void loadSimple(bdet_DateTz        *value);
// [ 4] void loadSimple(bdet_TimeTz        *value);
// [ 7] loadChoice(int, void *, const SInfo *, const char *, Accessor);
// [ 7] loadChoice(int, void *, const SInfo *, AccssrNExtrctr, Chooser);
// [ 8] loadSequence(int, void *, const AInfo *, const char *, ElmtAccssr);
// [ 6] loadArray(int length, int elementSize, void *begin, Accessor a);
// [ 6] loadArray(void *, int, int, void *, Resizer, Accessor);
// [ 9] loadNullable(void *object, Accessor accessor);
// [ 9] loadNullable(void *obj, Accessor, Manipulator, ObjectFetcher);
// [ 5] loadEnumeration(int v, const EnumeratorInfo *a, int s);
// [ 5] loadEnumeration(void *, IntSetter, StrSetter, const EInfo *, int);
//
// ACCESSORS
// [  ] const char *className() const;
// [  ] bdeat_TypeCategory::Value category() const;
// [ 7] int selectionId() const;
// [ 6] bsl::size_t size() const;
// [  ] const void* object() const;
// [ 6] bool isByteArrayValue() const;
// [ 9] bool isNull() const;
// [ 7] bool choiceHasSelection(const char *name, int len) const;
// [ 7] bool choiceHasSelection(int selectionId) const;
// [ 8] bool sequenceHasAttribute(const char *name, int len) const;
// [ 8] bool sequenceHasAttribute(int attributeId) const;
// [ 7] int choiceAccessSelection(ACCESSOR& accessor) const;
// [ 8] int sequenceAccessAttribute(ACCESSOR& accessor, int attrId) const;
// [ 8] int sequenceAccessAttributes(ACCESSOR& accessor) const;
// [ 9] int accessNullable(ACCESSOR& accessor) const;
// [ 4] int accessSimple(ACCESSOR& accessor) const;
// [ 6] int arrayAccessElement(ACCESSOR& accessor, int index) const;
// [ 5] int enumToInt() const;
// [ 5] const char* enumToString() const;

// 'bdeat_typecategory' OVERLOADS
// [  ] Value bdeat_typeCategorySelect(const ObjectProxy& obj);
// [  ] int bdeat_typeCategoryAccessSimple(const ObjProxy&, ACCESSOR& a);
// [  ] int bdeat_typeCategoryManipulateSimple(ObjProxy *, MANIPULATOR&);
// [  ] int bdeat_typeCategoryManipulateArray(ObjProxy *, MANIPULATOR&);
// [  ] int bdeat_typeCategoryAccessArray(const ObjectProxy&, ACCESSOR&);
// [  ] int bdeat_typeCategoryAccessNullableValue(const ObjProxy&, ACC&);
// [  ] int bdeat_typeCategoryManipulateNullableValue(ObjProxy*, MANIP&);
//
// 'bdeat_EnumFunctions' OVERLOADS
//
// [  ] void bdeat_enumToInt(int *, const ObjectProxy& object)
// [  ] int bdeat_enumFromInt(ObjectProxy *result, int value)
// [  ] void bdeat_enumToString(bsl::string *, const ObjectProxy& object)
// [  ] int bdeat_enumFromString(ObjectProxy *, const char *str, int len)
//
// 'bdeat_arrayfunctions' OVERLOADS AND SPECIALIZATIONS
// [  ] bsl::size_t bdeat_arraySize(const ObjectProxy& object)
// [  ] void bdeat_arrayResize(ObjectProxy* object, int newSize)
// [  ] int bdeat_arrayAccessElement(const ObjectProxy&, ACCESSOR&, int)
// [  ] int bdeat_arrayManipulateElement(ObjectProxy*, MANIPULATOR&, int)
//
// 'bdeat_sequencefunctions' OVERLOADS AND SPECIALIZATIONS
// [  ] int bdeat_sequenceAccessAttributes(const ObjectProxy&, ACCESSOR&)
// [  ] int bdeat_sequenceAccessAttribute(const ObjProxy&, ACCESSOR&, int)
// [  ] bdeat_sequenceManipulateAttribute(ObjProxy *, MANIPULATOR&, int)
// [  ] bdeat_sequenceManipulateAttribute(ObjProxy *, MANIP&, const char*, int)
// [  ] bdeat_sequenceHasAttribute(const ObjectProxy&, int)
// [  ] bdeat_sequenceHasAttribute(const ObjProxy&, const char *, int)
//
// 'bdeat_nullablevaluefunctions' OVERLOADS AND SPECIALIZATIONS
// [  ] bool bdeat_nullableValueIsNull(const NullableAdapter& object)
// [  ] void bdeat_nullableValueMakeValue(NullableAdapter *object)
// [  ] int bdeat_nullableValueManipulateValue(Nullable*, MANIPULATOR&)
// [  ] int bdeat_nullableValueAccessValue(const Nullable&, ACCESSOR&)
//
// 'bdeat_typename' overloads
// [  ] const char *bdeat_TypeName_className(const ObjectProxy& object)
//
// 'bdeat_choicefunctions' overloads and specializations
// [  ] int bdeat_choiceSelectionId(const ObjectProxy& object)
// [  ] int bdeat_choiceAccessSelection(const ObjectProxy&, ACCESSOR&)
// [  ] bool bdeat_choiceHasSelection(const ObjProxy&, const char *, int)
// [  ] bool bdeat_choiceHasSelection(const ObjectProxy&, int)
// [  ] int bdeat_choiceMakeSelection(ObjectProxy *obj, int selectionId)
// [  ] int bdeat_choiceMakeSelection(ObjectProxy  *, const char *, int)
// [  ] int bdeat_choiceManipulateSelection(ObjectProxy *, MANIPULATOR&)

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { bsl::cout << #I << ": " << I << "\t"  \
                          << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { bsl::cout << #I << ": " << I << "\t" \
                         << #J << ": " << J << "\t" \
                         << #K << ": " << K << "\n";\
               aSsErT(1, #X, __LINE__); } }

#define LOOP0_ASSERT ASSERT
#define LOOP1_ASSERT LOOP_ASSERT

//=============================================================================
//                  STANDARD BDE VARIADIC ASSERT TEST MACROS
//-----------------------------------------------------------------------------

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)   N
#define NUM_ARGS(...) NUM_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1, 0, "")

#define LOOPN_ASSERT_IMPL(N, ...) LOOP ## N ## _ASSERT(__VA_ARGS__)
#define LOOPN_ASSERT(N, ...)      LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...) LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
                                              // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
                                              // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
                                              // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;
static int veryVeryVeryVerbose = 0;

const char LOG_CATEGORY[] = "BAEA_SERIALIZABLEOBJECTPROXY.TEST";

typedef baea::SerializableObjectProxy Obj;
typedef bdeat_TypeCategory            Category;

//=============================================================================
//                         TEST CALLBACK FUNCTIONS
//-----------------------------------------------------------------------------

namespace {

struct SimpleAddressExtractor {
    const void *d_address;

    template<typename TYPE>
    int operator()(const TYPE& object, bdeat_TypeCategory::Simple)
    {
        d_address = &object;
        return 0;
    }
};

template <class TYPE>
struct SimpleAccessor {
    TYPE d_value;
    int  d_rc;

    SimpleAccessor() : d_value(), d_rc(0) {};

    int operator()(const TYPE& value, bdeat_TypeCategory::Simple)
    {
        d_value = value;
        return d_rc;
    }

    template <class OTHER_TYPE, class OTHER_CATEGORY>
    int operator()(const OTHER_TYPE&, const OTHER_CATEGORY&)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERT(!"Should be unreachable");
        return -1;
    }
};

template <class TYPE>
struct SimpleManipulator {
    TYPE d_value;
    int  d_rc;

    SimpleManipulator() : d_value(), d_rc(0) {};

    int operator()(TYPE *value, bdeat_TypeCategory::Simple)
    {
        *value = d_value;
        return d_rc;
    }

    template <class OTHER_TYPE, class OTHER_CATEGORY>
    int operator()(OTHER_TYPE *, const OTHER_CATEGORY&)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERT(!"Should be unreachable");
        return -1;
    }
};

struct EnumManipulator {

    enum { ENUM_INFO_SIZE = 4 };
    static const bdeat_EnumeratorInfo d_enumInfo[ENUM_INFO_SIZE];

    static int intSetter(void *object, int value)
    {
        *(int *)object = value;
        return 0;
    }

    static int stringSetter(void *object, const char *value, int)
    {
        for (int i = 0; i < ENUM_INFO_SIZE; ++i) {
            if (0 == strcmp(d_enumInfo[i].name(), value)) {
                *(int *)object = d_enumInfo[i].value();
            }
        }
        return 0;
    }
};

const bdeat_EnumeratorInfo EnumManipulator::d_enumInfo[] = {
    { 0, "", 0, "" },
    { 1, "ENUM1", 5, "annotation for enum1" },
    { 2, "ENUM2", 5, "annotation for enum2" },
    { 5, "ENUMERATION5", 12, "" }
};


template <class TYPE>
struct ArrayObjectProxyUtil {
    static
    void accessor(SerializableObjectProxy *proxy, void *object)
    {
        proxy->loadSimple((TYPE *) object);
    }

    static 
    void resizer(void* object, void** newBegin, bsl::size_t newSize)
    {
        bsl::vector<TYPE> *array = (bsl::vector<TYPE> *)object;
        array->resize(newSize);
        *newBegin = newSize ? &(*array->begin()) : 0;
    }

};

template <class TYPE>
struct ArrayElementAccessor {
    TYPE d_value;
    int  d_rc;

    ArrayElementAccessor() : d_value(), d_rc(0) {}

    int operator() (const SerializableObjectProxy& object)
    {
        SimpleAccessor<TYPE> accessor;
        object.accessSimple(accessor);
        d_value = accessor.d_value;
        return d_rc;
    }

    int operator() (const SerializableObjectProxy_NullableAdapter&)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERT(!"Should be unreachable");
        return -1;
    }
};

template <class TYPE>
struct ArrayAccessor {
    bsl::vector<TYPE> d_value;
    int               d_rc;

    ArrayAccessor() : d_value(), d_rc(0) {}

    int operator() (const SerializableObjectProxy& object,
                    const bdeat_TypeCategory::Array&)
    {
        const int size = (int)bdeat_ArrayFunctions::size(object);
        d_value.reserve(size);

        for (int i = 0; i < size; ++i) {
            ArrayElementAccessor<TYPE> accessor;
            bdeat_ArrayFunctions::bdeat_arrayAccessElement(object,
                                                           accessor,
                                                           i);
            d_value.push_back(accessor.d_value);
        }
        return d_rc;
    }

    int operator() (const bsl::vector<char>& object,
                    const bdeat_TypeCategory::Array&)
    {
        if (bslmf_IsSame<TYPE, char>::VALUE) {

            // 'd_value' is guranteed to be 'bsl::vector<char>', but the cast
            // is necessary to compile if 'TYPE' is not 'char'.

            bsl::vector<char>& value =
                                 reinterpret_cast<bsl::vector<char>&>(d_value);
            value = object;
        }
        else {
            ASSERT(!"Should be unreachable");
        }
        return d_rc;
    }
};

template <class TYPE>
struct ArrayElementManipulator {
    TYPE d_value;
    int  d_rc;

    ArrayElementManipulator() : d_value(), d_rc(0) {}

    int operator() (SerializableObjectProxy *object)
    {
        SimpleManipulator<TYPE> manipulator;
        manipulator.d_value = d_value;
        object->manipulateSimple(manipulator);
        return d_rc;
    }

    int operator() (SerializableObjectProxy_NullableAdapter *)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERT(!"Should be unreachable");
        return -1;
    }
};

template <class TYPE>
struct ArrayManipulator {
    bsl::vector<TYPE> d_value;
    int               d_rc;

    ArrayManipulator() : d_value(), d_rc(0) {}

    int operator() (SerializableObjectProxy *object,
                    const bdeat_TypeCategory::Array&)
    {
        const int size = d_value.size();

        bdeat_ArrayFunctions::bdeat_arrayResize(object, size);

        for (int i = 0; i < size; ++i) {
            ArrayElementManipulator<TYPE> manipulator;
            manipulator.d_value = d_value[i];
            bdeat_ArrayFunctions::bdeat_arrayManipulateElement(object,
                                                               manipulator,
                                                               i);
        }
        return d_rc;
    }

    int operator() (bsl::vector<char> *object,
                    const bdeat_TypeCategory::Array&)
    {
        if (bslmf_IsSame<TYPE, char>::VALUE) {

            // 'd_value' is guranteed to be 'bsl::vector<char>', but the cast
            // is necessary to compile if 'TYPE' is not 'char'.

            bsl::vector<char>& value =
                                 reinterpret_cast<bsl::vector<char>&>(d_value);
            *object = value;
        }
        else {
            ASSERT(!"Should be unreachable");
        }
        return d_rc;
    }
};

struct SequenceAccessor {
    const SerializableObjectProxy *d_proxy;
    const void                    *d_address;
    bdeat_AttributeInfo            d_info;
    int                            d_rc;

    SequenceAccessor() : d_proxy(0), d_address(0), d_rc(0) {}

    int operator() (const SerializableObjectProxy& object,
                    const bdeat_AttributeInfo& info);

    template <typename TYPE>
    int operator() (const TYPE&, const bdeat_AttributeInfo&)
    {
        // needed to compile due to nullable adapter, but should not be called

        ASSERT(!"Should be unreachable");
        return -1;
    }
};

int SequenceAccessor::operator()(const SerializableObjectProxy& object,
                                 const bdeat_AttributeInfo& info) {
    d_proxy = &object;
    SimpleAddressExtractor extractor;
    object.accessSimple(extractor);

    d_address = extractor.d_address;
    d_info = info;
    return d_rc;
}

struct ChoiceAccessor {
    const SerializableObjectProxy *d_proxy;
    const void                    *d_address;
    bdeat_SelectionInfo            d_info;
    int                            d_rc;

    ChoiceAccessor() : d_proxy(0), d_address(0), d_rc(0) {}

    int operator() (const SerializableObjectProxy& object,
                    const bdeat_SelectionInfo& info)
    {
        d_proxy = &object;
        d_address = object.object();
        d_info = info;
        return d_rc;
    }

    template <typename TYPE>
    int operator() (const TYPE&, const bdeat_SelectionInfo&)
    {
        // needed to compile due to nullable adapter, but should not be called

        ASSERT(!"Should be unreachable");
        return -1;
    }
};

struct ChoiceManipulator
{
    SerializableObjectProxy *d_proxy;
    const void              *d_address;
    bdeat_SelectionInfo      d_info;
    int                      d_rc;

    ChoiceManipulator() : d_proxy(0), d_address(0), d_rc(0) {}

    int operator() (SerializableObjectProxy* object,
                    const bdeat_SelectionInfo& info)
    {
        d_proxy = object;
        d_address = object->object();
        d_info = info;
        return d_rc;
    }

    template <typename TYPE>
    int operator() (TYPE*, const bdeat_SelectionInfo&)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERT(!"Should be unreachable");
        return -1;
    }
};

SerializableObjectProxy *elementAccessorCallback_proxy;
const void              *elementAccessorCallback_object;
int                      elementAccessorCallback_int;
int                      elementAccessorCallback_index;
void elementAccessorCallback(SerializableObjectProxy       *proxy,
                             const SerializableObjectProxy &object,
                             int                            index)
{
    elementAccessorCallback_proxy = proxy;
    elementAccessorCallback_object = &object;
    elementAccessorCallback_index = index;
    proxy->loadSimple(&elementAccessorCallback_int);
}

SerializableObjectProxy *accessorCallback_proxy;
void                    *accessorCallback_object;

template<class TYPE>
void accessorCallback(SerializableObjectProxy *proxy, void* object)
{
    accessorCallback_proxy = proxy;
    accessorCallback_object = object;
    proxy->loadSimple((TYPE *)object);
}

SerializableObjectProxy   *accessorAndExtractorCallback_proxy;
void                      *accessorAndExtractorCallback_object;
const bdeat_SelectionInfo *accessorAndExtractorCallback_selectInfoPtr;
int                        accessorAndExtractorCallback_int;
void accessorAndExtractorCallback(SerializableObjectProxy *proxy,
                                  void* object,
                                  const bdeat_SelectionInfo **selectInfoPtr)
{
    accessorAndExtractorCallback_proxy = proxy;
    accessorAndExtractorCallback_object = object;
    *selectInfoPtr = accessorAndExtractorCallback_selectInfoPtr;
    proxy->loadSimple(&accessorAndExtractorCallback_int);
}

int chooserCallback_rc = 0;
void *chooserCallback_object;
int chooserCallback_id;
int chooserCallback(void *object, int selectionId)
{
    chooserCallback_object = object;
    chooserCallback_id = selectionId;
    return chooserCallback_rc;
}


template <typename NULLABLE>
void nullableValueMaker(void *object)
{
    ((NULLABLE*)object)->makeValue();
}

template<typename NULLABLE>
void* nullableValueFetcher(void *object)
{
    NULLABLE* nullableValue = (NULLABLE*)object;
    if (nullableValue->isNull()) {
        // We also use this function for testing for null.

        return 0;                                                     // RETURN
    }
    return &nullableValue->value();
}

}  // close unamed namespace

// ============================================================================
//                      TEST DRIVER
// ----------------------------------------------------------------------------

template <class TYPE>
void executeSimpleCategoryTest(const char *typeName, const TYPE& testValue)
{
    Obj mX; const Obj& X = mX;
    const TYPE DEFAULT = TYPE();
    TYPE value;
    mX.loadSimple(&value);

    ASSERTV(typeName, Category::BDEAT_SIMPLE_CATEGORY == X.category());

    // TBD: fix this!! object() is 0 instead of the address of the object as
    // documented.

    //ASSERTV(typeName, (void*)&value, X.object(), &value == (TYPE*) X.object());

    SimpleManipulator<TYPE> manipulator;
    manipulator.d_value = testValue;

    SimpleAccessor<TYPE> accessor;

    ASSERTV(typeName, 0 == mX.manipulateSimple(manipulator));
    ASSERTV(typeName, testValue == value);
    ASSERTV(typeName, 0 == mX.accessSimple(accessor));
    ASSERTV(typeName, testValue == accessor.d_value);

    manipulator.d_value = DEFAULT;
    manipulator.d_rc    = 1;
    accessor.d_rc       = 2;

    ASSERTV(typeName, 1 == mX.manipulateSimple(manipulator));
    ASSERTV(typeName, 2 == mX.accessSimple(accessor));
    ASSERTV(typeName, DEFAULT == accessor.d_value);
}


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? std::atoi(argv[1]) : 0;

    verbose = (argc > 2);
    veryVerbose = (argc >  3);
    veryVeryVerbose = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;

    switch (test) { case 0: // Zero is always the leading case.
      case 9: {
        // --------------------------------------------------------------------
        // TESTING Nullable
        //
        // Testing:
        //   int manipulateNullable(MANIPULATOR& manipulator);
        //   void makeValue();
        //   loadNullable(void *object, Accessor accessor);
        //   loadNullable(void *obj, Accessor, Manipulator, ObjectFetcher);
        //   bool isNull() const;
        //   int accessNullable(ACCESSOR& accessor) const;
        // --------------------------------------------------------------------

        {
            Obj mX; const Obj& X = mX;
            int value = 1;

            mX.loadNullable(&value, &accessorCallback<int>);

            SimpleAccessor<int> accessor;
            X.accessNullable(accessor);

            ASSERTV(1 == accessor.d_value);
        }

        {
            Obj mX; const Obj& X = mX;

            bdeut_NullableValue<int> value;

            mX.loadNullable(&value,
                            &accessorCallback<int>,
                            &nullableValueMaker<bdeut_NullableValue<int> >,
                            &nullableValueFetcher<bdeut_NullableValue<int> >);

            ASSERTV(true == X.isNull());

            mX.makeValue();
            ASSERTV(false == X.isNull());
            ASSERTV(false == value.isNull());
            ASSERTV(0     == value.value());

            SimpleManipulator<int> manipulator;
            manipulator.d_value = 1;
            ASSERTV(0 == mX.manipulateNullable(manipulator));
            ASSERTV(1 == value.value());
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING Sequence
        //
        // Testing:
        //   int sequenceManipulateAttribute(MANIPULATOR& manipulator, int id);
        //   int sequenceManipulateAttribute(MANIPULATOR&, const char*, int;
        //   loadSequence(int, void *, const AInfo *, const char *, ElmtAccssr)
        //   bool sequenceHasAttribute(const char *name, int len) const;
        //   bool sequenceHasAttribute(int attributeId) const;
        //   int sequenceAccessAttribute(ACCESSOR& accessor, int attrId) const;
        //   int sequenceAccessAttributes(ACCESSOR& accessor) const;
        // --------------------------------------------------------------------
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING Choice
        //
        // Testing:
        //   int choiceMakeSelection(int selectionId);
        //   int choiceMakeSelection(const char *name, int length);
        //   int choiceManipulateSelection(MANIPULATOR& manipulator);
        //   loadChoice(int, void *, const SInfo *, const char *, Accessor);
        //   loadChoice(int, void *, const SInfo *, AccssrNExtrctr, Chooser);
        //   int selectionId() const;
        //   bool choiceHasSelection(const char *name, int len) const;
        //   bool choiceHasSelection(int selectionId) const;
        //   int choiceAccessSelection(ACCESSOR& accessor) const;
        // --------------------------------------------------------------------
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING Array
        //   void resize(size_t newSize);
        //   int arrayManipulateElement(MANIPULATOR& manipulator, int index);
        //   loadArray(int length, int elementSize, void *begin, Accessor a);
        //   loadArray(void *, int, int, void *, Resizer, Accessor);
        //   bsl::size_t size() const;
        //   bool isByteArrayValue() const;
        //   int arrayAccessElement(ACCESSOR& accessor, int index) const;
        // --------------------------------------------------------------------

        {
            Obj mX; const Obj& X = mX;

            bsl::vector<int> value;
            value.push_back(0);
            value.push_back(1);
            value.push_back(2);
            value.push_back(3);
            value.push_back(4);

            mX.loadArray(value.size(),
                         sizeof(int),
                         value.data(),
                         &accessorCallback<int>);

            ASSERTV(false == X.isByteArrayValue());

            for (int i = 0; i < value.size(); ++i) {
                ArrayElementAccessor<int> accessor;
                ASSERTV(0        == mX.arrayAccessElement(accessor, i));
                ASSERTV(value[i] == accessor.d_value);
                ASSERTV(0        == bdeat_ArrayFunctions::
                                     bdeat_arrayAccessElement(X, accessor, i));
                ASSERTV(value[i] == accessor.d_value);
            }

            ArrayAccessor<int> accessor;
            ASSERTV(0 == bdeat_typeCategoryAccessArray(X, accessor));
            ASSERTV(value == accessor.d_value);

            mX.loadArray(&value,
                         value.size(),
                         sizeof(int),
                         value.data(),
                         &ArrayObjectProxyUtil<int>::resizer,
                         &accessorCallback<int>);

            ASSERTV(false == X.isByteArrayValue());

            for (int i = 0; i < value.size(); ++i) {
                ArrayElementManipulator<int> manipulator;
                manipulator.d_value = -i;
                ASSERTV(0  == mX.arrayManipulateElement(manipulator, i));
                ASSERTV(-i == value[i]);

                manipulator.d_value = i;
                ASSERTV(0 == bdeat_ArrayFunctions::
                            bdeat_arrayManipulateElement(&mX, manipulator, i));
                ASSERTV(i == value[i]);
            }

            ArrayManipulator<int> manipulator;
            ASSERTV(0  == bdeat_typeCategoryManipulateArray(&mX, manipulator));
            ASSERTV(bsl::vector<int>() == value);

            mX.resize(0);
            ASSERTV(0 == value.size());
        }

        // Test 'char' array

        {
            Obj mX; const Obj& X = mX;

            bsl::vector<char> value;
            value.push_back(0);
            value.push_back(1);
            value.push_back(2);
            value.push_back(3);
            value.push_back(4);

            // Load 'char' vector.  A byte array is the same for both encoding
            // and decoding.

            mX.loadArray(value.size(), sizeof(char), &value, 0);

            ASSERTV(true == X.isByteArrayValue());

            ArrayAccessor<char> accessor;
            ASSERTV(0  == bdeat_typeCategoryAccessArray(X, accessor));
            ASSERTV(value == accessor.d_value);

            ArrayManipulator<char> manipulator;
            ASSERTV(0  == bdeat_typeCategoryManipulateArray(&mX, manipulator));
            ASSERTV(bsl::vector<char>() == value);
        }

      }  break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING Enum
        //
        // Testing:
        //   int enumFromInt(int value);
        //   int enumFromString(const char *stringValue, int stringLength);
        //   loadEnumeration(int v, const EnumeratorInfo *a, int s);
        //   loadEnumeration(void *, IntSetter, StrSetter, const EInfo *, int);
        //   int enumToInt() const;
        //   const char* enumToString() const;
        // --------------------------------------------------------------------

        Obj mX; const Obj& X = mX;

        const bdeat_EnumeratorInfo *enumInfo = EnumManipulator::d_enumInfo;
        int enumInfoSize = EnumManipulator::ENUM_INFO_SIZE;
        mX.loadEnumeration(1, enumInfo, enumInfoSize);

        ASSERTV(1 == X.enumToInt());
        ASSERTV(0 == strcmp("ENUM1", X.enumToString()));

        int value;

        mX.loadEnumeration(&value,
                           &EnumManipulator::intSetter,
                           &EnumManipulator::stringSetter,
                           enumInfo,
                           enumInfoSize);

        mX.enumFromInt(2);
        ASSERTV(2 == value);

        mX.enumFromString("", 0);
        ASSERTV(0 == value);


      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING Simple Pointer
        //
        // Testing:
        //   int manipulateSimple(MANIPULATOR& manipulator);
        //   void loadSimple(char               *value);
        //   void loadSimple(unsigned char      *value);
        //   void loadSimple(short              *value);
        //   void loadSimple(int                *value);
        //   void loadSimple(bsls_Types::Int64  *value);
        //   void loadSimple(unsigned short     *value);
        //   void loadSimple(unsigned int       *value);
        //   void loadSimple(bsls_Types::Uint64 *value);
        //   void loadSimple(float              *value);
        //   void loadSimple(double             *value);
        //   void loadSimple(bsl::string        *value);
        //   void loadSimple(bdet_Datetime      *value);
        //   void loadSimple(bdet_Date          *value);
        //   void loadSimple(bdet_Time          *value);
        //   void loadSimple(bool               *value);
        //   void loadSimple(bdet_DatetimeTz    *value);
        //   void loadSimple(bdet_DateTz        *value);
        //   void loadSimple(bdet_TimeTz        *value);
        //   int accessSimple(ACCESSOR& accessor) const;
        // --------------------------------------------------------------------

        executeSimpleCategoryTest<char>("char", 1);
        executeSimpleCategoryTest<unsigned char>("uchar", 2);
        executeSimpleCategoryTest<short>("short", 3);
        executeSimpleCategoryTest<int>("int", 4);
        executeSimpleCategoryTest<bsls_Types::Uint64>("uint64", 5);
        executeSimpleCategoryTest<float>("float", 6.1);
        executeSimpleCategoryTest<double>("double", 7.2);
        executeSimpleCategoryTest<bsl::string>("string", "hello world");
        executeSimpleCategoryTest<bdet_Datetime>("datetime",
                                                 bdet_Datetime(2001, 9, 3));
        executeSimpleCategoryTest<bdet_Date>("date", bdet_Date(2002, 10, 4));
        executeSimpleCategoryTest<bdet_Time>("time", bdet_Time(3, 11, 5));
        executeSimpleCategoryTest<bool>("bool", true);
        executeSimpleCategoryTest<bdet_DatetimeTz>(
                               "datetimetz",
                               bdet_DatetimeTz(bdet_Datetime(2004, 12, 6), 1));
        executeSimpleCategoryTest<bdet_DateTz>(
                                        "datetz",
                                        bdet_DateTz(bdet_Date(2005, 1, 7), 2));
        executeSimpleCategoryTest<bdet_TimeTz>(
                                           "timetz",
                                           bdet_TimeTz(bdet_Time(6, 2, 8), 3));
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING Sequence callback
        //
        // Test that the callback function supplied to the 'loadSequence'
        // method is invoked properly.
        // --------------------------------------------------------------------

        bdeat_AttributeInfo someInfo;
        someInfo.id() = 10;
        someInfo.name() = "plugh";
        someInfo.nameLength() = 5;

        int someObject;

        Obj proxy;
        proxy.loadSequence(1, &someObject, &someInfo, "foo",
                           &elementAccessorCallback);

        SequenceAccessor sequenceAccessor;
        ASSERT(0 == proxy.sequenceAccessAttribute(sequenceAccessor, 10));

        ASSERT(elementAccessorCallback_proxy == sequenceAccessor.d_proxy);
        ASSERT(elementAccessorCallback_object == &proxy);
        ASSERT(elementAccessorCallback_index == 0);
        ASSERT(sequenceAccessor.d_address == &elementAccessorCallback_int);
        ASSERT(sequenceAccessor.d_info == someInfo);

        sequenceAccessor.d_rc = 5;
        ASSERT(5 == proxy.sequenceAccessAttribute(sequenceAccessor, 10));

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING Choice callbacks
        //
        // Test that the callback functions supplied to the 'loadChoice'
        // methods are invoked properly.
        // --------------------------------------------------------------------

        // accessor
        int someObject;
        bdeat_SelectionInfo someInfo;
        someInfo.id() = 10;
        someInfo.name() = "plugh";
        someInfo.nameLength() = 5;

        Obj proxy;

        proxy.loadChoice(10, &someObject, &someInfo, "foo", &accessorCallback<int>);

        ChoiceAccessor choiceAccessor;

        ASSERT(0 == proxy.choiceAccessSelection(choiceAccessor));

        ASSERT(accessorCallback_proxy == choiceAccessor.d_proxy);
        ASSERT(accessorCallback_object == &someObject);
        ASSERT(choiceAccessor.d_address = &someObject);
        ASSERT(choiceAccessor.d_info == someInfo);

        choiceAccessor.d_rc = 5;
        ASSERT(5 == proxy.choiceAccessSelection(choiceAccessor));

        // manipulator
        int someChoice; // different than someObject
        proxy.loadChoice(1, &someChoice, &someInfo,
                         &accessorAndExtractorCallback,
                         &chooserCallback);
        accessorAndExtractorCallback_selectInfoPtr = &someInfo;

        ASSERT(0 == proxy.choiceMakeSelection(2));
        ASSERT(2 == chooserCallback_id);
        ASSERT(&someChoice == chooserCallback_object);
        ASSERT(0 == proxy.choiceMakeSelection(10));


        ChoiceManipulator choiceManipulator;
        ASSERT(0 == proxy.choiceManipulateSelection(choiceManipulator));

        ASSERT(accessorAndExtractorCallback_proxy == choiceManipulator.d_proxy);
        ASSERT(accessorAndExtractorCallback_object == &someChoice);
        ASSERT(choiceManipulator.d_address = &accessorAndExtractorCallback_int);
        ASSERT(choiceManipulator.d_info == someInfo);

        choiceManipulator.d_rc = 7;
        ASSERT(7 == proxy.choiceManipulateSelection(choiceManipulator));

        chooserCallback_rc = 6;
        ASSERT(6 == proxy.choiceMakeSelection(2));

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING Constructor
        //
        // Test that SerializableObjectProxy is initialized to a valid empty
        // state.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << "Testing constructor" << endl
                 << "===================" << endl;
        }

        Obj mX; const Obj& X = mX;
        ASSERT(X.isNull());
        ASSERT(Category::BDEAT_DYNAMIC_CATEGORY == X.category());
        ASSERT(Category::BDEAT_DYNAMIC_CATEGORY ==
               bdeat_TypeCategoryFunctions::bdeat_typeCategorySelect(X));

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
