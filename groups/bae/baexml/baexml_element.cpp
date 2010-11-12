// baexml_element.cpp   -*-C++-*-
#include <baexml_element.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baexml_element_cpp,"$Id$ $CSID$")

#include <bcema_blob.h>
#include <bcema_blobutil.h>
#include <bcesb_blobstreambuf.h>
#include <bcema_sharedptr.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bsl_list.h>
#include <bsl_vector.h>

#include <bsls_assert.h>

namespace BloombergLP {

namespace {

                           // ======================
                           // class BlobCharIterator
                           // ======================

class BlobCharIterator {
    // Class for iterating over characters in a blob.

    // PRIVATE DATA MEMBERS
    const bcema_Blob       *d_data_p;   // Blob on which to iterate (held)
    int                     d_absPos;   // Absolute position within blob
    const bcema_BlobBuffer *d_buffer_p; // Current buffer within blob (held)
    int                     d_relPos;   // Position relative to current buffer

  private:
    // NOT IMPLEMENTED
    BlobCharIterator(const BlobCharIterator&);
    BlobCharIterator& operator=(const BlobCharIterator&);

  public:
    // CREATORS
    explicit BlobCharIterator(const bcema_Blob *data);

    ~BlobCharIterator();

    // MANIPULATORS
    void setPos(int newPos);

    BlobCharIterator& operator++();

    BlobCharIterator& operator--();

    // ACCESSORS
    char operator*() const;

    int pos() const;
};

                           // ----------------------
                           // class BlobCharIterator
                           // ----------------------
// CREATORS

BlobCharIterator::BlobCharIterator(const bcema_Blob *data)
: d_data_p(data)
, d_absPos(0)
, d_buffer_p(&data->buffer(0))
, d_relPos(0)
{
}

inline
BlobCharIterator::~BlobCharIterator()
{
}

// MANIPULATORS

void BlobCharIterator::setPos(int newPos)
{
    BSLS_ASSERT(0      <= newPos);
    BSLS_ASSERT(newPos <= d_data_p->length());

    d_absPos = newPos;
    d_relPos = newPos;

    d_buffer_p = &d_data_p->buffer(0);

    // Testing that d_relPos is not zero before checking the buffer size
    // prevents dereferencing d_buffer_p if it is off the end of the buffer
    // array.
    while (d_relPos && d_buffer_p->size() <= d_relPos) {
        d_relPos -= d_buffer_p->size();
        ++d_buffer_p;
    }
}

inline
BlobCharIterator& BlobCharIterator::operator++()
{
    BSLS_ASSERT_SAFE(d_absPos < d_data_p->length());

    ++d_absPos;
    ++d_relPos;

    if (d_relPos >= d_buffer_p->size()) {
        d_relPos = 0;
        ++d_buffer_p;
    }

    return *this;
}

inline
BlobCharIterator& BlobCharIterator::operator--()
{
    BSLS_ASSERT_SAFE(0 < d_absPos);

    --d_absPos;
    --d_relPos;

    if (d_relPos < 0) {
        --d_buffer_p;
        d_relPos = d_buffer_p->size() - 1;
    }

    return *this;
}

// ACCESSORS

inline
char BlobCharIterator::operator*() const
{
    BSLS_ASSERT_SAFE(d_absPos < d_data_p->length());

    return d_buffer_p->data()[d_relPos];
}

inline
int BlobCharIterator::pos() const
{
    return d_absPos;
}

// CONSTANTS

enum { UNLIMITED_LEVELS };

// HELPER FUNCTIONS

void initNodeDeepCopy(baexml_Element_Node        *dst,
                      const baexml_Element_Node&  src,
                      baexml_Element_Imp         *elementImp)
{
    bsl::list<int> indexStack;

    baexml_Element_Node       *parent     = 0;
    baexml_Element_Node       *currentDst = dst;
    const baexml_Element_Node *currentSrc = &src;

    while (true) {
        currentDst->d_imp_p         = elementImp;
        currentDst->d_parent_p      = parent;
        currentDst->d_openTagBegin  = currentSrc->d_openTagBegin;
        currentDst->d_openTagLength = currentSrc->d_openTagLength;
        currentDst->d_elementLength = currentSrc->d_elementLength;

        int numSubNodes = currentSrc->d_subNodes.size();

        currentDst->d_subNodes.resize(numSubNodes);

        for (int i = 0; i < numSubNodes; ++i) {
            currentDst->d_subNodes[i].createInplace(elementImp->d_allocator_p,
                                                    elementImp->d_allocator_p);
        }

        if (0 == numSubNodes) {
            while (parent) {
                BSLS_ASSERT(!indexStack.empty());

                ++*indexStack.begin();

                int idx         = *indexStack.begin();
                int numSiblings = currentDst->d_parent_p->d_subNodes.size();

                if (idx == numSiblings) {
                    indexStack.pop_front();
                    parent     = currentDst->d_parent_p->d_parent_p;
                    currentDst = currentDst->d_parent_p;
                    currentSrc = currentSrc->d_parent_p;
                }
                else {
                    int srcNumSiblings
                                   = currentSrc->d_parent_p->d_subNodes.size();

                    BSLS_ASSERT(idx         <  numSiblings);
                    BSLS_ASSERT(idx         <  srcNumSiblings);
                    BSLS_ASSERT(numSiblings == srcNumSiblings);

                    currentDst = currentDst->d_parent_p->d_subNodes[idx].ptr();
                    currentSrc = currentSrc->d_parent_p->d_subNodes[idx].ptr();
                    break;
                }
            }

            if (indexStack.empty()) {
                return;
            }
        }
        else {
            indexStack.push_front(0);
            parent     = currentDst;
            currentDst = currentDst->d_subNodes[0].ptr();
            currentSrc = currentSrc->d_subNodes[0].ptr();
        }
    }
}

int loadTree(baexml_Element_Node      *rootNode,
             const bcema_Blob&  data,
             int                startPos,
             int                endPos,
             int                numLevels)
{
    enum { BAEXML_SUCCESS = 0, BAEXML_FAILURE = -1 };

    BlobCharIterator it(&data);

    it.setPos(startPos);

    enum {
        STATE_INIT,
        STATE_HEADER,
        STATE_COMMENT,
        STATE_IN_TAG,
        STATE_OUT_OF_TAG
    };

    int state = STATE_INIT;
    int stateBeforeComment;

    bslma_Allocator *allocator = rootNode->d_imp_p->d_allocator_p;

    baexml_Element_Node *node         = 0;
    bool          foundRoot    = false;
    bool          finished     = false;
    int           currentLevel = 0;

    bool isUnlimitedLevels = numLevels == UNLIMITED_LEVELS;

    while (!finished) {
        switch (state) {
          case STATE_INIT: {
            while (true) {
                if (it.pos() == endPos) {
                    return BAEXML_FAILURE;
                }

                char c = *it;

                if ('<' == c) {
                    ++it;

                    if (it.pos() == endPos) {
                        return BAEXML_FAILURE;
                    }

                    c = *it;

                    if ('/' == c || '>' == c) {
                        return BAEXML_FAILURE;
                    }

                    if ('!' == c) {
                        ++it;
                        if (it.pos() == endPos
                         || '-'      != *it) {
                            return BAEXML_FAILURE;
                        }
                        ++it;
                        if (it.pos() == endPos
                         || '-'      != *it) {
                            return BAEXML_FAILURE;
                        }
                        ++it;
                        stateBeforeComment = state;
                        state              = STATE_COMMENT;
                        break;
                    }

                    if ('?' == c) {
                        state = STATE_HEADER;
                        break;
                    }
                    else {
                        --it;
                        state = STATE_IN_TAG;
                        break;
                    }
                }

                ++it;
            }
          } break;
          case STATE_HEADER: {
            while (true) {
                if (it.pos() == endPos) {
                    return BAEXML_FAILURE;
                }

                char c = *it;

                if ('>' == c) {
                    ++it;
                    state = STATE_OUT_OF_TAG;
                    break;
                }

                ++it;
            }
          } break;
          case STATE_COMMENT: {
            while (true) {
                if (it.pos() == endPos) {
                    return BAEXML_FAILURE;
                }
                char c = *it;
                if ('-' == c) {
                    ++it;
                    if (it.pos() == endPos) {
                        return BAEXML_FAILURE;
                    }
                    if ('-' != *it) {
                        ++it;
                        continue;
                    }
                    ++it;
                    if (it.pos() == endPos) {
                        return BAEXML_FAILURE;
                    }
                    if ('>' != *it) {
                        ++it;
                        continue;
                    }
                    ++it;
                    state = stateBeforeComment;
                    break;
                }
                else {
                    ++it;
                }
            }
          } break;
          case STATE_IN_TAG: {
            BSLS_ASSERT(it.pos() != endPos);
            BSLS_ASSERT('<'      == *it);

            int tagBeginPos = it.pos();

            ++it;

            if (it.pos() == endPos) {
                return BAEXML_FAILURE;
            }

            char c         = *it;
            bool isOpenTag = ('/' != c);

            if (isOpenTag) {
                if (!node) {
                    // root
                    if (foundRoot) {
                        return BAEXML_FAILURE;
                    }

                    node                 = rootNode;
                    node->d_openTagBegin = tagBeginPos
                                                   - node->indexOffsetAmount();
                    foundRoot            = true;
                }
                else {
                    ++currentLevel;

                    if (isUnlimitedLevels || currentLevel <= numLevels) {
                        // new child node
                        int idx = node->d_subNodes.size();

                        node->d_subNodes.resize(idx + 1);
                        node->d_subNodes[idx].createInplace(allocator,
                                                            allocator);

                        node->d_subNodes[idx]->d_parent_p = node;
                        node->d_subNodes[idx]->d_imp_p    = node->d_imp_p;
                        node->d_subNodes[idx]->d_openTagBegin
                                = tagBeginPos
                                  - node->d_subNodes[idx]->indexOffsetAmount();

                        node = node->d_subNodes[idx].ptr();
                    }
                }
            }
            else {
                if (!node) {
                    return BAEXML_FAILURE;
                }
            }

            bool isAlsoCloseTag = false;

            while (true) {
                if (it.pos() == endPos) {
                    return BAEXML_FAILURE;
                }

                char c = *it;

                if ('<' == c) {
                    return BAEXML_FAILURE;
                }

                if ('>' == c) {
                    --it;
                    if ('/' == *it) {
                        isAlsoCloseTag = true;
                    }
                    ++it;

                    state = STATE_OUT_OF_TAG;
                    ++it;
                    break;
                }

                ++it;
            }

            int tagLength = it.pos() - tagBeginPos;

            if (isOpenTag) {
                if (isUnlimitedLevels || currentLevel <= numLevels) {
                    node->d_openTagLength = tagLength;
                }
            }

            if (!isOpenTag || isAlsoCloseTag) {
                if (isUnlimitedLevels || currentLevel <= numLevels) {
                    node->d_elementLength = it.pos()
                                            - node->d_openTagBegin
                                            - node->indexOffsetAmount();

                    if (isAlsoCloseTag
                     && node->d_elementLength != node->d_openTagLength) {
                        return BAEXML_FAILURE;
                    }

                    node = node->d_parent_p;
                }

                --currentLevel;
            }
          } break;
          case STATE_OUT_OF_TAG: {
            while (true) {
                if (it.pos() == endPos) {
                    if (node != rootNode->d_parent_p) {
                        return BAEXML_FAILURE;
                    }

                    finished = true;
                    break;
                }

                char c = *it;

                if ('<' == c) {
                    ++it;

                    if (it.pos() == endPos) {
                        return BAEXML_FAILURE;
                    }

                    if ('!' == *it) {
                        ++it;
                        if (it.pos() == endPos
                         || '-'      != *it) {
                            return BAEXML_FAILURE;
                        }
                        ++it;
                        if (it.pos() == endPos
                         || '-'      != *it) {
                            return BAEXML_FAILURE;
                        }
                        ++it;
                        stateBeforeComment = state;
                        state              = STATE_COMMENT;
                        break;
                    }

                    --it;

                    state = STATE_IN_TAG;
                    break;
                }

                ++it;
            }
          } break;
        }
    }

    return BAEXML_SUCCESS;
}

}  // close unnamed namespace

// TBD: make the short functions inlined in the header file

                              // ----------------
                              // class baexml_ElementRef
                              // ----------------

// CREATORS

baexml_ElementRef::baexml_ElementRef()
: d_node_p()
{
}

baexml_ElementRef::baexml_ElementRef(const baexml_ElementRef& original)
: d_node_p(original.d_node_p)
{
}

baexml_ElementRef::~baexml_ElementRef()
{
}

// MANIPULATORS

baexml_ElementRef& baexml_ElementRef::operator=(const baexml_ElementRef& rhs)
{
    d_node_p = rhs.d_node_p;
    return *this;
}

// ACCESSORS

baexml_ElementRef::operator bool() const
{
    return 0 != d_node_p.ptr();
}

baexml_ElementRef baexml_ElementRef::operator[](int index) const
{
    BSLS_ASSERT(d_node_p);
    BSLS_ASSERT(0     <= index);
    BSLS_ASSERT(index <  numSubElements());

    baexml_ElementRef ret;
    ret.d_node_p = d_node_p->d_subNodes[index];

    return ret;
}

void baexml_ElementRef::extractContent(bcema_Blob *content) const
{
    BSLS_ASSERT(content);
    BSLS_ASSERT(d_node_p);

    content->removeAll();

    if (d_node_p->d_openTagLength == d_node_p->d_elementLength) {
        return;
    }

    int begin = d_node_p->d_openTagBegin + d_node_p->indexOffsetAmount();
    int contentBegin  = begin + d_node_p->d_openTagLength;
    int contentLength = 0;

    {
        BlobCharIterator it(&d_node_p->d_imp_p->d_data);
        it.setPos(begin + d_node_p->d_elementLength);
        --it;
        while ('<' != *it) {
            --it;
        }
        contentLength = it.pos() - contentBegin;
    }

    bcema_BlobUtil::append(content,
                           d_node_p->d_imp_p->d_data,
                           contentBegin,
                           contentLength);
}

void baexml_ElementRef::extractData(bcema_Blob *data) const
{
    BSLS_ASSERT(data);
    BSLS_ASSERT(d_node_p);

    bcema_Blob& src    = d_node_p->d_imp_p->d_data;
    int         begin  = d_node_p->d_openTagBegin
                                               + d_node_p->indexOffsetAmount();
    int         length = d_node_p->d_elementLength;

    data->removeAll();
    bcema_BlobUtil::append(data, src, begin, length);
}

void baexml_ElementRef::extractElementName(bsl::string *elementName) const
{
    BSLS_ASSERT(elementName);
    BSLS_ASSERT(d_node_p);

    int begin  = d_node_p->d_openTagBegin + d_node_p->indexOffsetAmount() + 1;
    int length = 0;

    bcema_Blob& src = d_node_p->d_imp_p->d_data;

    BlobCharIterator it(&src);

    it.setPos(begin);

    while (it.pos() != src.length()) {
        if (' ' == *it || '>' == *it || '/' == *it) {
            break;
        }
        ++it;
        ++length;
    }

    elementName->resize(length);
    int i = 0;
    it.setPos(begin);

    while (i != length) {
        (*elementName)[i] = *it;

        ++i;
        ++it;
    }
}

void baexml_ElementRef::extractOpenTag(bsl::string *tag) const
{
    BSLS_ASSERT(tag);
    BSLS_ASSERT(d_node_p);

    int begin  = d_node_p->d_openTagBegin + d_node_p->indexOffsetAmount();
    int length = 0;

    bcema_Blob& src = d_node_p->d_imp_p->d_data;

    BlobCharIterator it(&src);

    it.setPos(begin);

    while (it.pos() != src.length()) {
        if ('>' == *it) {
            ++length;
            break;
        }
        ++it;
        ++length;
    }

    tag->resize(length);
    int i = 0;
    it.setPos(begin);

    while (i != length) {
        (*tag)[i] = *it;

        ++i;
        ++it;
    }
}

void baexml_ElementRef::insertSubElement(
            int                            index,
            const baexml_ElementConstRef&  elementRef) const
{
    BSLS_ASSERT(d_node_p);
    BSLS_ASSERT(elementRef);
    BSLS_ASSERT(0     <= index);
    BSLS_ASSERT(index <= numSubElements());

    bslma_Allocator *allocator = d_node_p->d_imp_p->d_allocator_p;

    bcema_Blob data;
    elementRef.extractData(&data);

    bcema_SharedPtr<baexml_Element_Node> node;
    node.createInplace(allocator, allocator);

    initNodeDeepCopy(node.ptr(),
                     *elementRef.d_imp.d_node_p.ptr(),
                     d_node_p->d_imp_p);

    node->d_parent_p = d_node_p.ptr();

    int lengthDifference = 0;

    if (0 == index) {
        if (d_node_p->d_openTagLength == d_node_p->d_elementLength) {
            // open and close element in the same tag
            BSLS_ASSERT(!"Feature not yet implemented!");
            return;
        }
        else {
            node->d_openTagBegin = d_node_p->d_openTagLength;

            bcema_Blob *dest       = &d_node_p->d_imp_p->d_data;
            int         destOffset = d_node_p->indexOffsetAmount()
                                     + d_node_p->d_openTagBegin
                                     + node->d_openTagBegin;

            bcema_BlobUtil::insert(dest, destOffset, data);
            lengthDifference = data.length();
        }
    }
    else {
        int prev = index - 1;

        node->d_openTagBegin = d_node_p->d_subNodes[prev]->d_openTagBegin
                               + d_node_p->d_subNodes[prev]->d_elementLength;

        bcema_Blob *dest       = &d_node_p->d_imp_p->d_data;
        int         destOffset = d_node_p->indexOffsetAmount()
                                 + d_node_p->d_openTagBegin
                                 + node->d_openTagBegin;

        bcema_BlobUtil::insert(dest, destOffset, data);
        lengthDifference = data.length();
    }

    d_node_p->d_subNodes.insert(d_node_p->d_subNodes.begin() + index, node);

    // Update node information.

    {
        baexml_Element_Node *parent = node->d_parent_p;
        baexml_Element_Node *child  = node.ptr();

        while (parent) {
            parent->d_elementLength += lengthDifference;

            bool foundChild = false;

            for (int i = 0; i < (int) parent->d_subNodes.size(); ++i) {
                if (foundChild) {
                    parent->d_subNodes[i]->d_openTagBegin += lengthDifference;
                }
                else if (parent->d_subNodes[i].ptr() == child) {
                    foundChild = true;
                }
            }

            parent = child->d_parent_p->d_parent_p;
            child  = child->d_parent_p;
        }
    }
}

int baexml_ElementRef::numSubElements() const
{
    BSLS_ASSERT(d_node_p);

    return d_node_p->d_subNodes.size();
}

void baexml_ElementRef::removeAllSubElements() const
{
    setContent(bcema_Blob());
}

void baexml_ElementRef::removeSubElement(int index) const
{
    baexml_Element orig = *this;
    int numSubElem = orig.numSubElements();
    int numKeptSubElem = 0;

    // Clear the current content
    setContent(bcema_Blob());

    // Selectively insert back from the original.
    for (int i = 0; i < numSubElem; ++i) {
        if (i != index)
            insertSubElement(numKeptSubElem++, orig[i]);
    }
}

void baexml_ElementRef::setContent(const bcema_Blob& content) const
{
    BSLS_ASSERT(d_node_p);

    if (d_node_p->d_openTagLength == d_node_p->d_elementLength) {
        // Open and close element on the same tag.
        BSLS_ASSERT(!"Not yet supported!");
        return;
    }

    int begin = d_node_p->d_openTagBegin + d_node_p->indexOffsetAmount();
    int contentBegin = begin + d_node_p->d_openTagLength;

    int prevContentLength = 0;

    {
        BlobCharIterator it(&d_node_p->d_imp_p->d_data);
        it.setPos(begin + d_node_p->d_elementLength);
        --it;
        while ('<' != *it) {
            --it;
        }
        prevContentLength = it.pos() - contentBegin;
    }

    bcema_BlobUtil::erase(&d_node_p->d_imp_p->d_data,
                          contentBegin,
                          prevContentLength);
    bcema_BlobUtil::insert(&d_node_p->d_imp_p->d_data,
                           contentBegin,
                           content);

    // Update node information.

    d_node_p->d_subNodes.clear();

    int lengthDifference = content.length() - prevContentLength;

    d_node_p->d_elementLength += lengthDifference;

    {
        baexml_Element_Node *parent = d_node_p->d_parent_p;
        baexml_Element_Node *child  = d_node_p.ptr();

        while (parent) {
            parent->d_elementLength += lengthDifference;

            bool foundChild = false;

            for (unsigned int i = 0; i < parent->d_subNodes.size(); ++i) {
                if (foundChild) {
                    parent->d_subNodes[i]->d_openTagBegin += lengthDifference;
                }
                else if (parent->d_subNodes[i].ptr() == child) {
                    foundChild = true;
                }
            }

            parent = child->d_parent_p->d_parent_p;
            child  = child->d_parent_p;
        }
    }
}

                       // ---------------------
                       // class baexml_ElementConstRef
                       // ---------------------

// CREATORS

baexml_ElementConstRef::baexml_ElementConstRef()
: d_imp()
{
}

baexml_ElementConstRef::baexml_ElementConstRef(
                const baexml_ElementConstRef& original)
: d_imp(original.d_imp)
{
}

baexml_ElementConstRef::baexml_ElementConstRef(
                const baexml_ElementRef& original)
: d_imp(original)
{
}

baexml_ElementConstRef::~baexml_ElementConstRef()
{
}

// MANIPULATORS

baexml_ElementConstRef& baexml_ElementConstRef::operator=(
                const baexml_ElementConstRef& rhs)
{
    d_imp = rhs.d_imp;
    return *this;
}

// ACCESSORS

baexml_ElementConstRef::operator bool() const
{
    return d_imp;
}

baexml_ElementConstRef baexml_ElementConstRef::operator[](int index) const
{
    return d_imp[index];
}

void baexml_ElementConstRef::extractContent(bcema_Blob *content) const
{
    d_imp.extractContent(content);
}

void baexml_ElementConstRef::extractData(bcema_Blob *data) const
{
    d_imp.extractData(data);
}

void baexml_ElementConstRef::extractElementName(bsl::string *elementName) const
{
    d_imp.extractElementName(elementName);
}

void baexml_ElementConstRef::extractOpenTag(bsl::string *tag) const
{
    d_imp.extractOpenTag(tag);
}

int baexml_ElementConstRef::numSubElements() const
{
    return d_imp.numSubElements();
}

                           // -------------
                           // class baexml_Element
                           // -------------

// CREATORS

baexml_Element::baexml_Element(bslma_Allocator *basicAllocator)
: d_imp(basicAllocator)
, d_root()
{
}

baexml_Element::baexml_Element(const baexml_Element&   original,
                               bslma_Allocator        *basicAllocator)
: d_imp(basicAllocator)
, d_root()
{
    *this = original;
}

baexml_Element::baexml_Element(const baexml_ElementRef&  original,
                               bslma_Allocator          *basicAllocator)
: d_imp(basicAllocator)
, d_root()
{
    *this = original;
}

baexml_Element::baexml_Element(const baexml_ElementConstRef&  original,
                 bslma_Allocator        *basicAllocator)
: d_imp(basicAllocator)
, d_root()
{
    *this = original;
}

baexml_Element::~baexml_Element()
{
}

// MANIPULATORS

baexml_Element& baexml_Element::operator=(const baexml_Element& rhs)
{
    if (&rhs != this) {
        bslma_Allocator *allocator = d_imp.d_allocator_p;

        bcema_Blob tmpData(rhs.d_imp.d_data, allocator);
        bcema_SharedPtr<baexml_Element_Node> tmpNode;

        if (rhs.d_root.d_node_p.ptr()) {
            tmpNode.createInplace(allocator, allocator);
            initNodeDeepCopy(tmpNode.ptr(), *rhs.d_root.d_node_p, &d_imp);
        }

        d_imp.d_data    = tmpData;
        d_root.d_node_p = tmpNode;    // nothrow
    }

    return *this;
}

baexml_Element& baexml_Element::operator=(const baexml_ElementRef& rhs)
{
    if (rhs != d_root) {
        bslma_Allocator *allocator = d_imp.d_allocator_p;

        bcema_Blob                    tmpData(allocator);
        bcema_SharedPtr<baexml_Element_Node> tmpNode;

        rhs.extractData(&tmpData);
        tmpNode.createInplace(allocator, allocator);

        initNodeDeepCopy(tmpNode.ptr(), *rhs.d_node_p.ptr(), &d_imp);
        tmpNode->d_openTagBegin = 0;

        d_imp.d_data    = tmpData;
        d_root.d_node_p = tmpNode;    // nothrow
    }

    return *this;
}

baexml_Element& baexml_Element::operator=(const baexml_ElementConstRef& rhs)
{
    if (rhs != d_root) {
        bslma_Allocator *allocator = d_imp.d_allocator_p;

        bcema_Blob                    tmpData(allocator);
        bcema_SharedPtr<baexml_Element_Node> tmpNode;

        rhs.extractData(&tmpData);
        tmpNode.createInplace(allocator, allocator);

        initNodeDeepCopy(tmpNode.ptr(), *rhs.d_imp.d_node_p.ptr(), &d_imp);
        tmpNode->d_openTagBegin = 0;

        d_imp.d_data    = tmpData;
        d_root.d_node_p = tmpNode;    // nothrow
    }

    return *this;
}

baexml_ElementRef baexml_Element::operator[](int index)
{
    return d_root[index];
}

void baexml_Element::insertSubElement(int                    index,
                               const baexml_ElementConstRef& elementRef) const
{
    return d_root.insertSubElement(index, elementRef);
}

int baexml_Element::load(const bcema_Blob& data)
{
    return load(data, UNLIMITED_LEVELS);
}

int baexml_Element::load(const bcema_Blob& data, int numLevels)
{
    bslma_Allocator *allocator = d_imp.d_allocator_p;

    bcema_SharedPtr<baexml_Element_Node> rootNode;
    rootNode.createInplace(allocator, allocator);

    rootNode->d_imp_p = &d_imp;

    int ret = loadTree(rootNode.ptr(), data, 0, data.length(), numLevels);

    if (0 != ret) {
        return ret;
    }

    d_imp.d_data    = data;
    d_root.d_node_p = rootNode;

    return ret;
}

void baexml_Element::removeAllSubElements()
{
    d_root.removeAllSubElements();
}

void baexml_Element::removeSubElement(int index)
{
    d_root.removeSubElement(index);
}

void baexml_Element::setContent(const bcema_Blob& content)
{
    d_root.setContent(content);
}

// ACCESSORS

baexml_ElementConstRef baexml_Element::operator[](int index) const
{
    return d_root[index];
}

baexml_Element::operator baexml_ElementConstRef() const
{
    return d_root;
}

void baexml_Element::extractContent(bcema_Blob *content) const
{
    d_root.extractContent(content);
}

void baexml_Element::extractData(bcema_Blob *data) const
{
    d_root.extractData(data);
}

void baexml_Element::extractElementName(bsl::string *elementName) const
{
    d_root.extractElementName(elementName);
}

void baexml_Element::extractOpenTag(bsl::string *tag) const
{
    d_root.extractOpenTag(tag);
}

int baexml_Element::numSubElements() const
{
    return d_root.numSubElements();
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
