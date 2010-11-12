// bdem_listimp.cpp                                                   -*-C++-*-
#include <bdem_listimp.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_listimp_cpp,"$Id$ $CSID$")

#include <bdem_functiontemplates.h>
#include <bdem_properties.h>

#include <bdeimp_bitwisecopy.h>

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
    bdem_RowLayout *d_rowLayout_p;  // row that contains managed elements
    int             d_startIndex;   // starting index
    int             d_length;       // number of objects to manage

  private:
    // NOT IMPLEMENTED
    bdem_ListImp_RowLayoutProctor(const bdem_ListImp_RowLayoutProctor&);
    bdem_ListImp_RowLayoutProctor& operator=(
                                         const bdem_ListImp_RowLayoutProctor&);

  public:
    // CREATORS
    bdem_ListImp_RowLayoutProctor(bdem_RowLayout *rowLayout,
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
                                                     bdem_RowLayout *rowLayout,
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
    // Namespace for static functions to store in a 'bdem_Descriptor'.

    // CLASS METHODS
    static
    void defaultConstruct(
                  void                                     *obj,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *alloc);
        // Construct a list object into raw memory.  The prototype for this
        // function must match the defaultConstruct function pointer in
        // bdem_Descriptor.

    static
    void copyConstruct(
                  void                                     *obj,
                  const void                               *rhs,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *alloc);
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
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *alloc)
{
    BSLS_ASSERT(obj);

    new (obj) bdem_ListImp(allocationStrategy, alloc);
}

void
bdem_ListImp_AttrFuncs::copyConstruct(
                  void                                     *obj,
                  const void                               *rhs,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *alloc)
{
    BSLS_ASSERT(obj);
    BSLS_ASSERT(rhs);

    const bdem_ListImp& rhsList = *static_cast<const bdem_ListImp *>(rhs);
    new (obj) bdem_ListImp(rhsList, allocationStrategy, alloc);
}

bsl::ostream& bdem_ListImp_AttrFuncs::print(const void    *obj,
                                            bsl::ostream&  stream,
                                            int            level,
                                            int            spacesPerLevel)
{
    BSLS_ASSERT(obj);

    static_cast<const bdem_ListImp *>(obj)->print(stream,
                                                  level,
                                                  spacesPerLevel);
    return stream;
}

                        // ------------------
                        // class bdem_ListImp
                        // ------------------

// CLASS DATA
const bdem_Descriptor bdem_ListImp::d_listAttr =
{
    bdem_ElemType::BDEM_LIST,
    sizeof(bdem_ListImp),
    bsls_AlignmentFromType<bdem_ListImp>::VALUE,
    &bdem_ListImp_AttrFuncs::defaultConstruct,
    &bdem_ListImp_AttrFuncs::copyConstruct,
    &bdem_FunctionTemplates::destroy<bdem_ListImp>,
    &bdem_FunctionTemplates::assign<bdem_ListImp>,
    &bdem_FunctionTemplates::bitwiseMove<bdem_ListImp>,
    &bdem_FunctionTemplates::removeAll<bdem_ListImp>,
    &bdem_FunctionTemplates::isEmpty<bdem_ListImp>,
    &bdem_FunctionTemplates::areEqual<bdem_ListImp>,
    &bdem_ListImp_AttrFuncs::print
};

// PRIVATE MANIPULATORS
void bdem_ListImp::init()
{
    bslma_Allocator *allocator = d_allocatorManager.internalAllocator();

    d_rowLayout_p = new (*allocator) bdem_RowLayout(allocator);

    bslma_RawDeleterProctor<bdem_RowLayout, bslma_Allocator> proctor(
                                                                 d_rowLayout_p,
                                                                 allocator);

    d_rowData_p = new (*allocator) bdem_RowData(
                                       d_rowLayout_p,
                                       d_allocatorManager.allocationStrategy(),
                                       allocator);

    proctor.release();
}

void bdem_ListImp::init(const bdem_ElemType::Type *elementTypes,
                        int                        numElements,
                        const bdem_Descriptor     *const *attrLookupTbl)
{
    BSLS_ASSERT(0 <= numElements);

    bslma_Allocator *allocator = d_allocatorManager.internalAllocator();

    d_rowLayout_p = new (*allocator) bdem_RowLayout(elementTypes,
                                                    numElements,
                                                    attrLookupTbl,
                                                    allocator);

    bslma_RawDeleterProctor<bdem_RowLayout, bslma_Allocator> proctor(
                                                                 d_rowLayout_p,
                                                                 allocator);

    d_rowData_p = new (*allocator) bdem_RowData(
                                       d_rowLayout_p,
                                       d_allocatorManager.allocationStrategy(),
                                       allocator);

    proctor.release();
}

void bdem_ListImp::init(const bdem_RowData& srcRowData)
{
    bslma_Allocator *allocator = d_allocatorManager.internalAllocator();

    d_rowLayout_p = new (*allocator) bdem_RowLayout(*srcRowData.rowLayout(),
                                                    allocator);

    d_rowLayout_p->compact();

    bslma_RawDeleterProctor<bdem_RowLayout, bslma_Allocator> proctor(
                                                                 d_rowLayout_p,
                                                                 allocator);

    d_rowData_p = new (*allocator) bdem_RowData(
                                       d_rowLayout_p,
                                       srcRowData,
                                       d_allocatorManager.allocationStrategy(),
                                       allocator);

    proctor.release();
}

// CREATORS
bdem_ListImp::bdem_ListImp(
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
{
    init();
}

bdem_ListImp::bdem_ListImp(
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemorySize,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, initialMemorySize, basicAllocator)
{
    BSLS_ASSERT(0 <= initialMemorySize);

    init();
}

bdem_ListImp::bdem_ListImp(
                  const bdem_ElemType::Type                *elementTypes,
                  int                                       numElements,
                  const bdem_Descriptor *const             *attrLookupTbl,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
{
    BSLS_ASSERT(0 <= numElements);

    init(elementTypes, numElements, attrLookupTbl);
}

bdem_ListImp::bdem_ListImp(
                  const bdem_ElemType::Type                *elementTypes,
                  int                                       numElements,
                  const bdem_Descriptor *const             *attrLookupTbl,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemorySize,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, initialMemorySize, basicAllocator)
{
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(0 <= initialMemorySize);

    init(elementTypes, numElements, attrLookupTbl);
}

bdem_ListImp::bdem_ListImp(
                  const bdem_RowData&                       original,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
{
    init(original);
}

bdem_ListImp::bdem_ListImp(
                  const bdem_RowData&                       original,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemorySize,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, initialMemorySize, basicAllocator)
{
    BSLS_ASSERT(0 <= initialMemorySize);

    init(original);
}

bdem_ListImp::bdem_ListImp(
                  const bdem_ListImp&                       original,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, basicAllocator)
{
    init(*original.d_rowData_p);
}

bdem_ListImp::bdem_ListImp(
                  const bdem_ListImp&                       original,
                  bdem_AggregateOption::AllocationStrategy  allocationStrategy,
                  const InitialMemory&                      initialMemorySize,
                  bslma_Allocator                          *basicAllocator)
: d_allocatorManager(allocationStrategy, initialMemorySize, basicAllocator)
{
    BSLS_ASSERT(0 <= initialMemorySize);

    init(*original.d_rowData_p);
}

bdem_ListImp::~bdem_ListImp()
{
    if (!(bdem_AggregateOption::BDEM_NODESTRUCT_FLAG &
                                    d_allocatorManager.allocationStrategy())) {
        bslma_Allocator *allocator = d_allocatorManager.internalAllocator();

        allocator->deleteObject(d_rowData_p);
        allocator->deleteObject(d_rowLayout_p);
    }
}

// MANIPULATORS
bdem_ListImp& bdem_ListImp::operator=(const bdem_RowData& rhs)
{
    // Create 'newList' as a copy of 'rhs', but using the same allocation
    // options as were used to construct '*this'.  Then swap '*this' with the
    // temporary list.  The old value of '*this' will be destroyed when
    // 'newList' goes out of scope.

    if (&rhs != d_rowData_p) {
        bdem_ListImp newList(rhs,
                             d_allocatorManager.allocationStrategy(),
                             d_allocatorManager.originalAllocator());

        bdeimp_BitwiseCopy<bdem_ListImp>::swap(this, &newList);
    }

    return *this;
}

void *bdem_ListImp::insertElementRaw(int                    dstIndex,
                                     const bdem_Descriptor *elemAttr)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());

    d_rowLayout_p->insert(dstIndex, elemAttr);

    bdem_ListImp_RowLayoutProctor proctor(d_rowLayout_p, dstIndex, 1);

    void *newElement = d_rowData_p->insertElementRaw(dstIndex);

    proctor.release();

    return newElement;
}

void *bdem_ListImp::insertElement(int                    dstIndex,
                                  const void            *value,
                                  const bdem_Descriptor *elemAttr)
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
        bdem_ListImp& list = insertList(
                                   dstIndex,
                                  *((const bdem_ListImp *)value)->d_rowData_p);
        return (void *) &list;
    }
}

void bdem_ListImp::insertElements(int                 dstIndex,
                                  const bdem_RowData& srcRowData,
                                  int                 srcIndex,
                                  int                 numElements)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());
    BSLS_ASSERT(0 <= srcIndex);
    BSLS_ASSERT(0 <= numElements);
    BSLS_ASSERT(srcIndex + numElements <= srcRowData.length());

    bdem_RowLayout tmpLayout(*srcRowData.rowLayout(),
                             srcIndex,
                             numElements);

    bdem_RowData tmpRowData(&tmpLayout,
                            srcRowData,
                            srcIndex,
                            numElements,
                            bdem_AggregateOption::BDEM_PASS_THROUGH);

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

void *bdem_ListImp::insertNullElement(int                    dstIndex,
                                      const bdem_Descriptor *elemAttr)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());

    d_rowLayout_p->insert(dstIndex, elemAttr);

    bdem_ListImp_RowLayoutProctor proctor(d_rowLayout_p, dstIndex, 1);

    void *newElement = d_rowData_p->insertNullElement(dstIndex);

    proctor.release();

    return newElement;
}

void bdem_ListImp::insertNullElements(
                              int                        dstIndex,
                              const bdem_ElemType::Type *elementTypes,
                              int                        numElements,
                              const bdem_Descriptor     *const *attrLookupTbl)
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

void bdem_ListImp::insertElement(int                      dstIndex,
                                 const bdem_ConstElemRef& srcElement)
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

bsl::string& bdem_ListImp::insertString(int dstIndex, const char *value)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());

    bsls_ObjectBuffer<bsl::string> stringBuffer;

    new (stringBuffer.buffer()) bsl::string(
                                       value,
                                       d_allocatorManager.internalAllocator());

    bslma_AutoDestructor<bsl::string> autoElemDestr(&stringBuffer.object(), 1);
    void *elemData = insertElementRaw(dstIndex,
                                      &bdem_Properties::d_stringAttr);
    autoElemDestr.release();

    bdeimp_BitwiseCopy<bsl::string>::copy(static_cast<bsl::string *>(elemData),
                                          &stringBuffer.object());
    return *static_cast<bsl::string *>(elemData);
}

bdem_ListImp& bdem_ListImp::insertList(int                 dstIndex,
                                       const bdem_RowData& srcRow)
{
    BSLS_ASSERT(0 <= dstIndex);
    BSLS_ASSERT(     dstIndex <= length());

    // Must construct list in temporary buffer to avoid overlap/recursion
    // potential.

    bsls_ObjectBuffer<bdem_ListImp> listBuffer;

    new (listBuffer.buffer()) bdem_ListImp(
                                       srcRow,
                                       d_allocatorManager.allocationStrategy(),
                                       d_allocatorManager.originalAllocator());

    bslma_AutoDestructor<bdem_ListImp> autoElemDestr(&listBuffer.object(), 1);
    void *elemData = insertElementRaw(dstIndex, &bdem_ListImp::d_listAttr);
    autoElemDestr.release();

    bdeimp_BitwiseCopy<bdem_ListImp>::copy(
                                         static_cast<bdem_ListImp *>(elemData),
                                         &listBuffer.object());
    return *static_cast<bdem_ListImp *>(elemData);
}

void bdem_ListImp::replaceElement(int index, const bdem_ConstElemRef& srcElem)
{
    BSLS_ASSERT(0 <= index);
    BSLS_ASSERT(     index < length());

    const bdem_Descriptor *descriptor = (*d_rowLayout_p)[index].attributes();
    if (srcElem.descriptor() == descriptor
     && ! (bdem_ElemType::BDEM_LIST == srcElem.type()
        && this == (bdem_ListImp *) srcElem.data())) {
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

void bdem_ListImp::swap(bdem_ListImp& rhs)
{
    if (this != &rhs) {
        if (d_allocatorManager.allocationStrategy() ==
                                    rhs.d_allocatorManager.allocationStrategy()
         && d_allocatorManager.internalAllocator() ==
                                  rhs.d_allocatorManager.internalAllocator()) {
            bdeimp_BitwiseCopy<bdem_ListImp>::swap(this, &rhs);
        }
        else {
            bdem_ListImp newList(rhs,
                                 d_allocatorManager.allocationStrategy(),
                                 d_allocatorManager.originalAllocator());
            bdem_ListImp newListRhs(
                                   *this,
                                   rhs.d_allocatorManager.allocationStrategy(),
                                   rhs.d_allocatorManager.originalAllocator());

            bdeimp_BitwiseCopy<bdem_ListImp>::swap(this, &newList);
            bdeimp_BitwiseCopy<bdem_ListImp>::swap(&rhs, &newListRhs);
        }
    }
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
