// baea_serializableobjectproxy.t.cpp   -*-C++-*-
#include <baea_serializableobjectproxy.h>
#include <baea_serializableobjectproxy.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bslma_sequentialallocator.h>
#include <cstdlib>

using namespace BloombergLP;
using namespace BloombergLP::baea;

using bsl::cout;
using bsl::endl;

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

//=============================================================================
//                         TEST CALLBACK FUNCTIONS
//-----------------------------------------------------------------------------

struct SimpleAddressExtractor
{
    const void *d_address;

    template<typename TYPE>
    int operator()(const TYPE& object, bdeat_TypeCategory::Simple)
    {
        d_address = &object;
    }
};

struct SequenceAccessor
{
    const SerializableObjectProxy    *d_proxy;
    const void                       *d_address;
    bdeat_AttributeInfo               d_info;
    int                               d_rc;

    SequenceAccessor() : d_proxy(0), d_address(0), d_rc(0) {}

    int operator() (const SerializableObjectProxy& object,
                    const bdeat_AttributeInfo& info);

    template <typename TYPE>
    int operator() (const TYPE& object,
                    const bdeat_AttributeInfo& info)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERT(!"Should be unreachable");
        return -1;
    }
};

int SequenceAccessor::operator()(const SerializableObjectProxy& object,
                                 const bdeat_AttributeInfo& info)
{
    d_proxy = &object;
    SimpleAddressExtractor extractor;
    object.accessSimple(extractor);

    d_address = extractor.d_address;
    d_info = info;
    return d_rc;
}

struct ChoiceAccessor
{
    const SerializableObjectProxy    *d_proxy;
    const void                       *d_address;
    bdeat_SelectionInfo               d_info;
    int                               d_rc;

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
    int operator() (const TYPE& object,
                    const bdeat_SelectionInfo& info)
    {
        // needed to compile due to nullable adapter, but should not be called
        ASSERT(!"Should be unreachable");
        return -1;
    }
};

struct ChoiceManipulator
{
    SerializableObjectProxy          *d_proxy;
    const void                       *d_address;
    bdeat_SelectionInfo               d_info;
    int                               d_rc;

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
    int operator() (TYPE* object,
                    const bdeat_SelectionInfo& info)
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
int                      accessorCallback_int;
void accessorCallback(SerializableObjectProxy *proxy, void* object)
{
    accessorCallback_proxy = proxy;
    accessorCallback_object = object;
    proxy->loadSimple(&accessorCallback_int);
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

        SerializableObjectProxy proxy;
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

        SerializableObjectProxy proxy;

        proxy.loadChoice(10, &someObject, &someInfo, "foo", &accessorCallback);

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

        baea::SerializableObjectProxy mX;
        const baea::SerializableObjectProxy& X = mX;
        ASSERT(X.isNull());
        ASSERT(bdeat_TypeCategory::BDEAT_DYNAMIC_CATEGORY == X.category());
        ASSERT(bdeat_TypeCategory::BDEAT_DYNAMIC_CATEGORY ==
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
