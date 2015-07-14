// bdlmxxx_listimp.cpp                                                   -*-C++-*-
#include <bdlmxxx_listimp.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_listimp_cpp,"$Id$ $CSID$")

#include <bdlmxxx_functiontemplates.h>
#include <bdlmxxx_properties.h>

#include <bdlimpxxx_bitwisecopy.h>

#include <bslma_allocator.h>
#include <bslma_autodestructor.h>
#include <bslma_rawdeleterproctor.h>

#include <bsls_alignmentfromtype.h>
#include <bsls_assert.h>
#include <bsls_objectbuffer.h>

#include <bsl_ostream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

namespace BloombergLP {

                        // ===================================
                        // class bdem_ListImp_RowLayoutProctor
                        // ===================================

class bdem_ListImp_RowLayoutProctor {
    // This 'class' implements a proctor that, unless its 'release' method is
    // invoked, automatically removes the row layout elements under management.

    // DATA
    bdlmxxx::RowLayout *d_rowLayout_p;  // row that contains managed elements
    int             d_startIndex;   // starting index
    int             d_length;       // number of objects to manage

  private:
    // NOT IMPLEMENTED
    bdem_ListImp_RowLayoutProctor(const bdem_ListImp_RowLayoutProctor&);
    bdem_ListImp_RowLayoutProctor& operator=(
                                         const bdem_ListImp_RowLayoutProctor&);

  public:
    // CREATORS
    bdem_ListImp_RowLayoutProctor(bdlmxxx::RowLayout *rowLayout,
                                  int             index,
                                  int             length);
        // Create a proctor to manage the specified 'length' elements in the
        // specified 'rowLayout' starting at the specified 'index'.

    ~bdem_ListImp_RowLayoutProctor();
        // Remove all managed elements.

    // MANIPULATORS
    void release();
        // Release from management all elements currently managed by this
        // proctor.

    bdem_ListImp_RowLayoutProctor& operator++();
        // Increase by one the length of the sequence of objects
        // managed by this proctor.
};

                        // -----------------------------------
                        // class bdem_ListImp_RowLayoutProctor
                        // -----------------------------------

// CREATORS
inline
bdem_ListImp_RowLayoutProctor::bdem_ListImp_RowLayoutProctor(
                                                     bdlmxxx::RowLayout *rowLayout,
                                                     int             index,
                                                     int             length)
: d_rowLayout_p(rowLayout)
, d_startIndex(index)
, d_length(length)
{
}

bdem_ListImp_RowLayoutProctor::~bdem_ListImp_RowLayoutProctor()
{
    d_rowLayout_p->remove(d_startIndex, d_length);
}

// MANIPULATORS
inline
void bdem_ListImp_RowLayoutProctor::release()
{
    d_length = 0;
}

inline
bdem_ListImp_RowLayoutProctor& bdem_ListImp_RowLayoutProctor::operator++()
{
    ++d_length;
    return *this;
}

                        // ============================
                        // class bdem_ListImp_AttrFuncs
                        // ============================

struct bdem_ListImp_AttrFuncs {
    // Namespace for static functions to store in a 'bdlmxxx::Descriptor'.

    // CLASS METHODS
    static
    void defaultConstruct(
                  void                                     *obj,
                  bdlmxxx::AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *alloc);
        // Construct a list object into raw memory.  The prototype for this
        // function must match the defaultConstruct function pointer in
        // bdem_Descriptor.

    static
    void copyConstruct(
                  void                                     *obj,
                  const void                               *rhs,
                  bdlmxxx::AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *alloc);
        // Copy construct a list object into raw memory.  The prototype for
        // this function must match the copyConstruct function pointer in
        // bdem_Descriptor.

    static
    bsl::ostream& print(const void    *obj,
                        bsl::ostream&  stream,
                        int            level,
                        int            spacesPerLevel);
        // Print this list.
};

                        // ============================
                        // class bdem_ListImp_AttrFuncs
                        // ============================

// CLASS METHODS
void
bdem_ListImp_AttrFuncs::defaultConstruct(
                  void                                     *obj,
                  bdlmxxx::AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *alloc)
{
    BSLS_ASSERT(obj);

    new (obj) bdlmxxx::ListImp(allocationStrategy, alloc);
}

void
bdem_ListImp_AttrFuncs::copyConstruct(
                  void                                     *obj,
                  const void                               *rhs,
                  bdlmxxx::AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *alloc)
{
    BSLS_ASSERT(obj);
    BSLS_ASSERT(rhs);

    const bdlmxxx::ListImp& rhsList = *static_cast<const bdlmxxx::ListImp *>(rhs);
    new (obj) bdlmxxx::ListImp(rhsList, allocationStrategy, alloc);
}

bsl::ostream& bdem_ListImp_AttrFuncs::print(const void    *obj,
                                            bsl::ostream&  stream,
                                            int            level,
                                            int            spacesPerLevel)
{
    BSLS_ASSERT(obj);

    static_cast<const bdlmxxx::ListImp *>(obj)->print(stream,
                                                  level,
                                                  spacesPerLevel);
    return stream;
}

                        // ------------------
                        // class bdlmxxx::ListImp
                        // ------------------

// CLASS DATA
const bdlmxxx::Descriptor bdlmxxx::ListImp::s_listAttr =
{
    bdlmxxx::ElemType::BDEM_LIST,
    sizeof(bdlmxxx::ListImp),
    bsls::AlignmentFromType<bdlmxxx::ListImp>::VALUE,
    &bdem_ListImp_AttrFuncs::defaultConstruct,
    &bdem_ListImp_AttrFuncs::copyConstruct,
    &bdlmxxx::FunctionTemplates::destroy<bdlmxxx::ListImp>,
    &bdlmxxx::FunctionTemplates::assign<bdlmxxx::ListImp>,
    &bdlmxxx::FunctionTemplates::bitwiseMove<bdlmxxx::ListImp>,
    &bdlmxxx::FunctionTemplates::removeAll<bdlmxxx::ListImp>,
    &bdlmxxx::FunctionTemplates::isEmpty<bdlmxxx::ListImp>,
    &bdlmxxx::FunctionTemplates::areEqual<bdlmxxx::ListImp>,
    &bdem_ListImp_AttrFuncs::print
};

namespace bdlmxxx {
// PRIVATE MANIPULATORS
void ListImp::init()
{
    bslma::Allocator *allocator = d_allocatorManager.internalAllocator();

    d_rowLayout_p = new (*allocator) RowLayout(allocator);

    bslma::RawDeleterProctor<RowLayout, bslma::Allocator> proctor(
                                                                 d_rowLayout_p,
                                                                 allocator);

    d_rowData_p = new (*allocator) RowData(
                                       d_rowLayout_p,
                                       d_allocatorManager.allocationStrategy(),
                                       allocator);

    proctor.release();
}

void ListImp::init(const ElemType::Type *elementTypes,
                        int                        numElements,
                        const Descriptor     *const *attrLookupTbl)
{
    BSLS_ASSERT(0 <= numElements);

    bslma::Allocator *allocator = d_allocatorManager.internalAllocator();

    d_rowLayout_p = new (*allocator) RowLayout(elementTypes,
                                                    numElements,
                                                    attrLookupTbl,
                                                    allocator);

    bslma::RawDeleterProctor<RowLayout, bslma::Allocator> proctor(
                                                                 d_rowLayout_p,
                                                                 allocator);

    d_rowData_p = new (*allocator) RowData(
                                       d_rowLayout_p,
                                       d_allocatorManager.allocationStrategy(),
                                       allocator);

    proctor.release();
}

void ListImp::init(const RowData& srcRowData)
{
    bslma::Allocator *allocator = d_allocatorManager.internalAllocator();

    d_rowLayout_p = new (*allocator) RowLayout(*srcRowData.rowLayout(),
                                                    allocator);

    d_rowLayout_p->compact();

    bslma::RawDeleterProctor<RowLayout, bslma::Allocator> proctor(
                                                                 d_rowLayout_p,
                                                                 allocator);

    d_rowData_p = new (*allocator) RowData(
                                       d_rowLayout_p,
                                       srcRowData,
                                       d_allocatorManager.allocationStrategy(),
                                       allocator);

    proctor.release();
}

// CREATORS
ListImp::ListImp(
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
{
    init();
}

ListImp::ListImp(
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemorySize,
                  bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, initialMemorySize, basicAllocator)
{
    BSLS_ASSERT(0 <= initialMemorySize);

    init();
}

ListImp::ListImp(
                  const ElemType::Type                *elementTypes,
                  int                                       numElements,
                  const Descriptor *const             *attrLookupTbl,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
{
    BSLS_ASSERT(0 <= numElements);

    init(elementTypes, numElements, attrLookupTbl);
}

ListImp::ListImp(
                  const ElemType::Type                *elementTypes,
                  int                                       numElements,
                  const Descriptor *const             *attrLookupTbl,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemorySize,
                  bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, initialMemorySize, basicAllocator)
{
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(0 <= initialMemorySize);

    init(elementTypes, numElements, attrLookupTbl);
}

ListImp::ListImp(
                  const RowData&                       original,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
{
    init(original);
}

ListImp::ListImp(
                  const RowData&                       original,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemorySize,
                  bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, initialMemorySize, basicAllocator)
{
    BSLS_ASSERT(0 <= initialMemorySize);

    init(original);
}

ListImp::ListImp(
                  const ListImp&                       original,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
{
    init(*original.d_rowData_p);
}

ListImp::ListImp(
                  const ListImp&                       original,
                  AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemorySize,
                  bslma::Allocator                         *basicAllocator)
: d_allocatorManager(allocationStrategy, initialMemorySize, basicAllocator)
{
    BSLS_ASSERT(0 <= initialMemorySize);

    init(*original.d_rowData_p);
}

ListImp::~ListImp()
{
    if (!(AggregateOption::BDEM_NODESTRUCT_FLAG &
                                    d_allocatorManager.allocationStrategy())) {
        bslma::Allocator *allocator = d_allocatorManager.internalAllocator();

        allocator->deleteObject(d_rowData_p);
        allocator->deleteObject(d_rowLayout_p);
    }
}

// MANIPULATORS
ListImp& ListImp::operator=(const RowData& rhs)
{
    // Create 'newList' as a copy of 'rhs', but using the same allocation
    // options as were used to construct '*this'.  Then swap '*this' with the
    // temporary list.  The old value of '*this' will be destroyed when
    // 'newList' goes out of scope.

    if (&rhs != d_rowData_p) {
        ListImp newList(rhs,
                             d_allocatorManager.allocationStrategy(),
                             d_allocatorManager.originalAllocator());

        bdlimpxxx::BitwiseCopy<ListImp>::swap(this, &newList);
    }

    return *this;
}

void *ListImp::insertElementRaw(int                    dstIndex,
                                     const Descriptor *elemAttr)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());

    d_rowLayout_p->insert(dstIndex, elemAttr);

    bdem_ListImp_RowLayoutProctor proctor(d_rowLayout_p, dstIndex, 1);

    void *newElement = d_rowData_p->insertElementRaw(dstIndex);

    proctor.release();

    return newElement;
}

void *ListImp::insertElement(int                    dstIndex,
                                  const void            *value,
                                  const Descriptor *elemAttr)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());

    if (this != value) {
        d_rowLayout_p->insert(dstIndex, elemAttr);

        bdem_ListImp_RowLayoutProctor proctor(d_rowLayout_p, dstIndex, 1);

        void *newElement = d_rowData_p->insertElement(dstIndex, value);

        proctor.release();

        return newElement;                                            // RETURN
    }
    else {
        ListImp& list = insertList(
                                   dstIndex,
                                  *((const ListImp *)value)->d_rowData_p);
        return (void *) &list;
    }
}

void ListImp::insertElements(int                 dstIndex,
                                  const RowData& srcRowData,
                                  int                 srcIndex,
                                  int                 numElements)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(srcIndex + numElements <= srcRowData.length());

    RowLayout tmpLayout(*srcRowData.rowLayout(),
                             srcIndex,
                             numElements);

    RowData tmpRowData(&tmpLayout,
                            srcRowData,
                            srcIndex,
                            numElements,
                            AggregateOption::BDEM_PASS_THROUGH);

    d_rowLayout_p->insert(dstIndex,
                          tmpLayout,
                          0,
                          numElements);

    bdem_ListImp_RowLayoutProctor proctor(d_rowLayout_p,
                                          dstIndex,
                                          numElements);

    d_rowData_p->insertElements(dstIndex, tmpRowData, 0, numElements);

    proctor.release();
}

void *ListImp::insertNullElement(int                    dstIndex,
                                      const Descriptor *elemAttr)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());

    d_rowLayout_p->insert(dstIndex, elemAttr);

    bdem_ListImp_RowLayoutProctor proctor(d_rowLayout_p, dstIndex, 1);

    void *newElement = d_rowData_p->insertNullElement(dstIndex);

    proctor.release();

    return newElement;
}

void ListImp::insertNullElements(
                              int                        dstIndex,
                              const ElemType::Type *elementTypes,
                              int                        numElements,
                              const Descriptor     *const *attrLookupTbl)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());
    BSLS_ASSERT(0 <= numElements);

    d_rowLayout_p->insert(dstIndex, elementTypes, numElements, attrLookupTbl);

    bdem_ListImp_RowLayoutProctor proctor(d_rowLayout_p,
                                          dstIndex,
                                          numElements);

    d_rowData_p->insertNullElements(dstIndex, numElements);

    proctor.release();
}

void ListImp::insertElement(int                      dstIndex,
                                 const ConstElemRef& srcElement)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());

    if (srcElement.isNull()) {
        insertNullElement(dstIndex, srcElement.descriptor());
    }
    else {
        insertElement(dstIndex, srcElement.data(), srcElement.descriptor());
    }
}

bsl::string& ListImp::insertString(int dstIndex, const char *value)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());

    bsls::ObjectBuffer<bsl::string> stringBuffer;

    new (stringBuffer.buffer()) bsl::string(
                                       value,
                                       d_allocatorManager.internalAllocator());

    bslma::AutoDestructor<bsl::string> autoElemDestr(&stringBuffer.object(),
                                                     1);
    void *elemData = insertElementRaw(dstIndex,
                                      &Properties::s_stringAttr);
    autoElemDestr.release();

    bdlimpxxx::BitwiseCopy<bsl::string>::copy(static_cast<bsl::string *>(elemData),
                                          &stringBuffer.object());
    return *static_cast<bsl::string *>(elemData);
}

ListImp& ListImp::insertList(int                 dstIndex,
                                       const RowData& srcRow)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());

    // Must construct list in temporary buffer to avoid overlap/recursion
    // potential.

    bsls::ObjectBuffer<ListImp> listBuffer;

    new (listBuffer.buffer()) ListImp(
                                       srcRow,
                                       d_allocatorManager.allocationStrategy(),
                                       d_allocatorManager.originalAllocator());

    bslma::AutoDestructor<ListImp> autoElemDestr(&listBuffer.object(), 1);
    void *elemData = insertElementRaw(dstIndex, &ListImp::s_listAttr);
    autoElemDestr.release();

    bdlimpxxx::BitwiseCopy<ListImp>::copy(
                                         static_cast<ListImp *>(elemData),
                                         &listBuffer.object());
    return *static_cast<ListImp *>(elemData);
}

void ListImp::replaceElement(int index, const ConstElemRef& srcElem)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < length());

    const Descriptor *descriptor = (*d_rowLayout_p)[index].attributes();
    if (srcElem.descriptor() == descriptor
     && ! (ElemType::BDEM_LIST == srcElem.type()
        && this == (const ListImp *) srcElem.data())) {
        d_rowData_p->elemRef(index).replaceValue(srcElem);
    }
    else {
        if (srcElem.isNull()) {
            insertNullElement(index, srcElem.descriptor());
        }
        else {
            insertElement(index, srcElem.data(), srcElem.descriptor());
        }
        removeElement(index + 1);
    }
}

void ListImp::swap(ListImp& rhs)
{
    if (this != &rhs) {
        if (d_allocatorManager.allocationStrategy() ==
                                    rhs.d_allocatorManager.allocationStrategy()
         && d_allocatorManager.internalAllocator() ==
                                  rhs.d_allocatorManager.internalAllocator()) {
            bdlimpxxx::BitwiseCopy<ListImp>::swap(this, &rhs);
        }
        else {
            ListImp newList(rhs,
                                 d_allocatorManager.allocationStrategy(),
                                 d_allocatorManager.originalAllocator());
            ListImp newListRhs(
                                   *this,
                                   rhs.d_allocatorManager.allocationStrategy(),
                                   rhs.d_allocatorManager.originalAllocator());

            bdlimpxxx::BitwiseCopy<ListImp>::swap(this, &newList);
            bdlimpxxx::BitwiseCopy<ListImp>::swap(&rhs, &newListRhs);
        }
    }
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
