// balxml_element.cpp   -*-C++-*-
#include <balxml_element.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balxml_element_cpp,"$Id$ $CSID$")

#include <bdlmca_blob.h>
#include <bdlmca_blobutil.h>
#include <bdlmca_blobstreambuf.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

#include <bsl_list.h>
#include <bsl_vector.h>

namespace BloombergLP {

namespace {

                           // ======================
                           // class BlobCharIterator
                           // ======================

class BlobCharIterator {
    // Class for iterating over characters in a blob.

    // PRIVATE DATA MEMBERS
    const bdlmca::Blob       *d_data_p;   // Blob on which to iterate (held)
    int                     d_absPos;   // Absolute position within blob
    const bdlmca::BlobBuffer *d_buffer_p; // Current buffer within blob (held)
    int                     d_relPos;   // Position relative to current buffer

  private:
    // NOT IMPLEMENTED
    BlobCharIterator(const BlobCharIterator&);
    BlobCharIterator& operator=(const BlobCharIterator&);

  public:
    // CREATORS
    explicit BlobCharIterator(const bdlmca::Blob *data);

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

BlobCharIterator::BlobCharIterator(const bdlmca::Blob *data)
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

void initNodeDeepCopy(balxml::Element_Node        *dst,
                      const balxml::Element_Node&  src,
                      balxml::Element_Imp         *elementImp)
{
    bsl::list<int> indexStack;

    balxml::Element_Node       *parent     = 0;
    balxml::Element_Node       *currentDst = dst;
    const balxml::Element_Node *currentSrc = &src;

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

                    currentDst = currentDst->d_parent_p->d_subNodes[idx].get();
                    currentSrc = currentSrc->d_parent_p->d_subNodes[idx].get();
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
            currentDst = currentDst->d_subNodes[0].get();
            currentSrc = currentSrc->d_subNodes[0].get();
        }
    }
}

int loadTree(balxml::Element_Node      *rootNode,
             const bdlmca::Blob&  data,
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

    bslma::Allocator *allocator = rootNode->d_imp_p->d_allocator_p;

    balxml::Element_Node *node  = 0;
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

                        node = node->d_subNodes[idx].get();
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

namespace balxml {
// TBD: make the short functions inlined in the header file

                              // ----------------
                              // class ElementRef
                              // ----------------

// CREATORS

ElementRef::ElementRef()
: d_node_p()
{
}

ElementRef::ElementRef(const ElementRef& original)
: d_node_p(original.d_node_p)
{
}

ElementRef::~ElementRef()
{
}

// MANIPULATORS

ElementRef& ElementRef::operator=(const ElementRef& rhs)
{
    d_node_p = rhs.d_node_p;
    return *this;
}
}  // close package namespace

// ACCESSORS

balxml::ElementRef::operator bool() const
{
    return 0 != d_node_p.get();
}

namespace balxml {
ElementRef ElementRef::operator[](int index) const
{
    BSLS_ASSERT(d_node_p);
    BSLS_ASSERT(0     <= index);
    BSLS_ASSERT(index <  numSubElements());

    ElementRef ret;
    ret.d_node_p = d_node_p->d_subNodes[index];

    return ret;
}

void ElementRef::extractContent(bdlmca::Blob *content) const
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

    bdlmca::BlobUtil::append(content,
                           d_node_p->d_imp_p->d_data,
                           contentBegin,
                           contentLength);
}

void ElementRef::extractData(bdlmca::Blob *data) const
{
    BSLS_ASSERT(data);
    BSLS_ASSERT(d_node_p);

    bdlmca::Blob& src    = d_node_p->d_imp_p->d_data;
    int         begin  = d_node_p->d_openTagBegin
                                               + d_node_p->indexOffsetAmount();
    int         length = d_node_p->d_elementLength;

    data->removeAll();
    bdlmca::BlobUtil::append(data, src, begin, length);
}

void ElementRef::extractElementName(bsl::string *elementName) const
{
    BSLS_ASSERT(elementName);
    BSLS_ASSERT(d_node_p);

    int begin  = d_node_p->d_openTagBegin + d_node_p->indexOffsetAmount() + 1;
    int length = 0;

    bdlmca::Blob& src = d_node_p->d_imp_p->d_data;

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

void ElementRef::extractOpenTag(bsl::string *tag) const
{
    BSLS_ASSERT(tag);
    BSLS_ASSERT(d_node_p);

    int begin  = d_node_p->d_openTagBegin + d_node_p->indexOffsetAmount();
    int length = 0;

    bdlmca::Blob& src = d_node_p->d_imp_p->d_data;

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

void ElementRef::insertSubElement(
            int                            index,
            const ElementConstRef&  elementRef) const
{
    BSLS_ASSERT(d_node_p);
    BSLS_ASSERT(elementRef);
    BSLS_ASSERT(0     <= index);
    BSLS_ASSERT(index <= numSubElements());

    bslma::Allocator *allocator = d_node_p->d_imp_p->d_allocator_p;

    bdlmca::Blob data;
    elementRef.extractData(&data);

    bsl::shared_ptr<Element_Node> node;
    node.createInplace(allocator, allocator);

    initNodeDeepCopy(node.get(),
                     *elementRef.d_imp.d_node_p.get(),
                     d_node_p->d_imp_p);

    node->d_parent_p = d_node_p.get();

    int lengthDifference = 0;

    if (0 == index) {
        if (d_node_p->d_openTagLength == d_node_p->d_elementLength) {
            // open and close element in the same tag
            BSLS_ASSERT(!"Feature not yet implemented!");
            return;
        }
        else {
            node->d_openTagBegin = d_node_p->d_openTagLength;

            bdlmca::Blob *dest       = &d_node_p->d_imp_p->d_data;
            int         destOffset = d_node_p->indexOffsetAmount()
                                     + d_node_p->d_openTagBegin
                                     + node->d_openTagBegin;

            bdlmca::BlobUtil::insert(dest, destOffset, data);
            lengthDifference = data.length();
        }
    }
    else {
        int prev = index - 1;

        node->d_openTagBegin = d_node_p->d_subNodes[prev]->d_openTagBegin
                               + d_node_p->d_subNodes[prev]->d_elementLength;

        bdlmca::Blob *dest       = &d_node_p->d_imp_p->d_data;
        int         destOffset = d_node_p->indexOffsetAmount()
                                 + d_node_p->d_openTagBegin
                                 + node->d_openTagBegin;

        bdlmca::BlobUtil::insert(dest, destOffset, data);
        lengthDifference = data.length();
    }

    d_node_p->d_subNodes.insert(d_node_p->d_subNodes.begin() + index, node);

    // Update node information.

    {
        Element_Node *parent = node->d_parent_p;
        Element_Node *child  = node.get();

        while (parent) {
            parent->d_elementLength += lengthDifference;

            bool foundChild = false;

            for (int i = 0; i < (int) parent->d_subNodes.size(); ++i) {
                if (foundChild) {
                    parent->d_subNodes[i]->d_openTagBegin += lengthDifference;
                }
                else if (parent->d_subNodes[i].get() == child) {
                    foundChild = true;
                }
            }

            parent = child->d_parent_p->d_parent_p;
            child  = child->d_parent_p;
        }
    }
}

int ElementRef::numSubElements() const
{
    BSLS_ASSERT(d_node_p);

    return d_node_p->d_subNodes.size();
}

void ElementRef::removeAllSubElements() const
{
    setContent(bdlmca::Blob());
}

void ElementRef::removeSubElement(int index) const
{
    Element orig = *this;
    int numSubElem = orig.numSubElements();
    int numKeptSubElem = 0;

    // Clear the current content
    setContent(bdlmca::Blob());

    // Selectively insert back from the original.
    for (int i = 0; i < numSubElem; ++i) {
        if (i != index)
            insertSubElement(numKeptSubElem++, orig[i]);
    }
}

void ElementRef::setContent(const bdlmca::Blob& content) const
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

    bdlmca::BlobUtil::erase(&d_node_p->d_imp_p->d_data,
                          contentBegin,
                          prevContentLength);
    bdlmca::BlobUtil::insert(&d_node_p->d_imp_p->d_data,
                           contentBegin,
                           content);

    // Update node information.

    d_node_p->d_subNodes.clear();

    int lengthDifference = content.length() - prevContentLength;

    d_node_p->d_elementLength += lengthDifference;

    {
        Element_Node *parent = d_node_p->d_parent_p;
        Element_Node *child  = d_node_p.get();

        while (parent) {
            parent->d_elementLength += lengthDifference;

            bool foundChild = false;

            for (unsigned int i = 0; i < parent->d_subNodes.size(); ++i) {
                if (foundChild) {
                    parent->d_subNodes[i]->d_openTagBegin += lengthDifference;
                }
                else if (parent->d_subNodes[i].get() == child) {
                    foundChild = true;
                }
            }

            parent = child->d_parent_p->d_parent_p;
            child  = child->d_parent_p;
        }
    }
}

                       // ---------------------
                       // class ElementConstRef
                       // ---------------------

// CREATORS

ElementConstRef::ElementConstRef()
: d_imp()
{
}

ElementConstRef::ElementConstRef(
                const ElementConstRef& original)
: d_imp(original.d_imp)
{
}

ElementConstRef::ElementConstRef(
                const ElementRef& original)
: d_imp(original)
{
}

ElementConstRef::~ElementConstRef()
{
}

// MANIPULATORS

ElementConstRef& ElementConstRef::operator=(
                const ElementConstRef& rhs)
{
    d_imp = rhs.d_imp;
    return *this;
}
}  // close package namespace

// ACCESSORS

balxml::ElementConstRef::operator bool() const
{
    return d_imp;
}

namespace balxml {
ElementConstRef ElementConstRef::operator[](int index) const
{
    return d_imp[index];
}

void ElementConstRef::extractContent(bdlmca::Blob *content) const
{
    d_imp.extractContent(content);
}

void ElementConstRef::extractData(bdlmca::Blob *data) const
{
    d_imp.extractData(data);
}

void ElementConstRef::extractElementName(bsl::string *elementName) const
{
    d_imp.extractElementName(elementName);
}

void ElementConstRef::extractOpenTag(bsl::string *tag) const
{
    d_imp.extractOpenTag(tag);
}

int ElementConstRef::numSubElements() const
{
    return d_imp.numSubElements();
}

                           // -------------
                           // class Element
                           // -------------

// CREATORS

Element::Element(bslma::Allocator *basicAllocator)
: d_imp(basicAllocator)
, d_root()
{
}

Element::Element(const Element&   original,
                               bslma::Allocator       *basicAllocator)
: d_imp(basicAllocator)
, d_root()
{
    *this = original;
}

Element::Element(const ElementRef&  original,
                               bslma::Allocator         *basicAllocator)
: d_imp(basicAllocator)
, d_root()
{
    *this = original;
}

Element::Element(const ElementConstRef&  original,
                               bslma::Allocator              *basicAllocator)
: d_imp(basicAllocator)
, d_root()
{
    *this = original;
}

Element::~Element()
{
}

// MANIPULATORS

Element& Element::operator=(const Element& rhs)
{
    if (&rhs != this) {
        bslma::Allocator *allocator = d_imp.d_allocator_p;

        bdlmca::Blob tmpData(rhs.d_imp.d_data, allocator);
        bsl::shared_ptr<Element_Node> tmpNode;

        if (rhs.d_root.d_node_p.get()) {
            tmpNode.createInplace(allocator, allocator);
            initNodeDeepCopy(tmpNode.get(), *rhs.d_root.d_node_p, &d_imp);
        }

        d_imp.d_data    = tmpData;
        d_root.d_node_p = tmpNode;    // nothrow
    }

    return *this;
}

Element& Element::operator=(const ElementRef& rhs)
{
    if (rhs != d_root) {
        bslma::Allocator *allocator = d_imp.d_allocator_p;

        bdlmca::Blob                    tmpData(allocator);
        bsl::shared_ptr<Element_Node> tmpNode;

        rhs.extractData(&tmpData);
        tmpNode.createInplace(allocator, allocator);

        initNodeDeepCopy(tmpNode.get(), *rhs.d_node_p.get(), &d_imp);
        tmpNode->d_openTagBegin = 0;

        d_imp.d_data    = tmpData;
        d_root.d_node_p = tmpNode;    // nothrow
    }

    return *this;
}

Element& Element::operator=(const ElementConstRef& rhs)
{
    if (rhs != d_root) {
        bslma::Allocator *allocator = d_imp.d_allocator_p;

        bdlmca::Blob                    tmpData(allocator);
        bsl::shared_ptr<Element_Node> tmpNode;

        rhs.extractData(&tmpData);
        tmpNode.createInplace(allocator, allocator);

        initNodeDeepCopy(tmpNode.get(), *rhs.d_imp.d_node_p.get(), &d_imp);
        tmpNode->d_openTagBegin = 0;

        d_imp.d_data    = tmpData;
        d_root.d_node_p = tmpNode;    // nothrow
    }

    return *this;
}

ElementRef Element::operator[](int index)
{
    return d_root[index];
}

void Element::insertSubElement(int                    index,
                               const ElementConstRef& elementRef) const
{
    return d_root.insertSubElement(index, elementRef);
}

int Element::load(const bdlmca::Blob& data)
{
    return load(data, UNLIMITED_LEVELS);
}

int Element::load(const bdlmca::Blob& data, int numLevels)
{
    bslma::Allocator *allocator = d_imp.d_allocator_p;

    bsl::shared_ptr<Element_Node> rootNode;
    rootNode.createInplace(allocator, allocator);

    rootNode->d_imp_p = &d_imp;

    int ret = loadTree(rootNode.get(), data, 0, data.length(), numLevels);

    if (0 != ret) {
        return ret;
    }

    d_imp.d_data    = data;
    d_root.d_node_p = rootNode;

    return ret;
}

void Element::removeAllSubElements()
{
    d_root.removeAllSubElements();
}

void Element::removeSubElement(int index)
{
    d_root.removeSubElement(index);
}

void Element::setContent(const bdlmca::Blob& content)
{
    d_root.setContent(content);
}

// ACCESSORS

ElementConstRef Element::operator[](int index) const
{
    return d_root[index];
}
}  // close package namespace

balxml::Element::operator balxml::ElementConstRef() const
{
    return d_root;
}

namespace balxml {
void Element::extractContent(bdlmca::Blob *content) const
{
    d_root.extractContent(content);
}

void Element::extractData(bdlmca::Blob *data) const
{
    d_root.extractData(data);
}

void Element::extractElementName(bsl::string *elementName) const
{
    d_root.extractElementName(elementName);
}

void Element::extractOpenTag(bsl::string *tag) const
{
    d_root.extractOpenTag(tag);
}

int Element::numSubElements() const
{
    return d_root.numSubElements();
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
