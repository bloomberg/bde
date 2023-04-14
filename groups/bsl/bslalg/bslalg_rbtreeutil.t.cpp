// bslalg_rbtreeutil.t.cpp                                            -*-C++-*-
#include <bslalg_rbtreeutil.h>

#include <bslalg_arrayprimitives.h>
#include <bslalg_arraydestructionprimitives.h>
#include <bslalg_rangecompare.h>

#include <bslma_allocator.h>
#include <bslma_stdallocator.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_libraryfeatures.h>

#include <algorithm>

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
#include <random>
#endif

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#endif

#ifdef BSLS_PLATFORM_OS_UNIX
#include <unistd.h>
#endif

using namespace BloombergLP;
using namespace std;
using namespace bslalg;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// 'RbTreeUtil' provides a suite of functions for managing red-black binary
// search trees.  These tests must verify that each of the functions behaves
// as documented, and works correctly on the full range of data that a
// red-black tree may be created for.  Within this test driver we develop a
// test-framework (similar to 'bsltf') for testing the templatized methods of
// this type on a variety of types.  Note that because of package dependencies
// this test-driver cannot depend on 'bsltf'.
//-----------------------------------------------------------------------------
//                              CLASS METHODS
// ----------------------------------------------------------------------------
// Navigation
// [ 4] const RbTreeNode *leftmost(const RbTreeNode *);
// [ 4]       RbTreeNode *leftmost(      RbTreeNode *);
// [ 4] const RbTreeNode *rightmost(const RbTreeNode *);
// [ 4]       RbTreeNode *rightmost(      RbTreeNode *);
// [ 5] const RbTreeNode *next(const RbTreeNode *);
// [ 5]       RbTreeNode *next(      RbTreeNode *);
// [ 6] const RbTreeNode *previous(const RbTreeNode *);
// [ 6]       RbTreeNode *previous(      RbTreeNode *);
// Search
// [10] const RbTreeNode *find(const Anchor&, const COMP&, const VALUE&);
// [10]       RbTreeNode *find(Anchor&, const COMP&, const VALUE&);
// [11] const RbTreeNode *lowerBound(const Anchor&, const COMP&, const VAL&);
// [11]       RbTreeNode *lowerBound(Anchor&, const COMP&, const VALUE&);
// [12] const RbTreeNode *upperBound(const Anchor&, const COMP&, const VALUE&);
// [12]       RbTreeNode *upperBound(Anchor&, const COMP&, const VALUE&);
// Modification
// [20] void copyTree(RbTreeAnchor *, const RbTreeAnchor& , FACTORY *);
// [19] void deleteTree(RbTreeAnchor *, FACTORY *);
// [14] RbTreeNode *findInsertLocation(bool*,Anchor*,COMP&,const VALUE&);
// [14] RbTreeNode *findInsertLocation(bool*,Anchor*,COMP&,const VALUE&,Node*);
// [16] RbTreeNode *findUniqueInsertLocation(int *,Anchor*,COMP&,VALUE&);
// [16] RbTreeNode *findUniqueInsertLocation(int *,Anchor*,COMP&,VALUE&,Node*);
// [ 9] void insert(RbTreeAnchor *, const COMP& , RbTreeNode *);
// [17] void insertAt(RbTreeAnchor *,RbTreeNode *, bool, RbTreeNode *);
// [18] void remove(RbTreeAnchor *, RbTreeNode *);
// [21] void swap(RbTreeAnchor *, RbTreeAnchor *);
// [22] bool isLeftChild(const RbTreeNode *);
// [22] bool isRightChild(const RbTreeNode *);
// [23] void rotateLeft(RbTreeNode *);
// [23] void rotateRight(RbTreeNode *);
// Testing
// [24] void printTreeStructure(FILE *, const Node *, Callback, int, int);
// [ 7] int validateRbTree(const RbTreeNode *, const COMP& );
// [ 7] int validateRbTree(const RbTreeNode **, const char **, Node *, COMP&);
// [ 8] bool isWellFormed(const RbTreeAnchor& ,const COMPR& );
// [ 2] Validator::isWellFormedAnchor(const RbTreeAnchor& ,const COMPR& );
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [26] USAGE EXAMPLE
// [ 3] CONCERN: gg Generator
// [25] CONCERN: Additional verification of exception safety of 'copyTree'

//=============================================================================

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef RbTreeUtil        Obj;
typedef RbTreeNode::Color Color;

const RbTreeNode::Color RED   = RbTreeNode::BSLALG_RED;
const RbTreeNode::Color BLACK = RbTreeNode::BSLALG_BLACK;

static bool         verbose = false;
static bool     veryVerbose = false;
static bool veryVeryVerbose = false;

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
static std::default_random_engine g_randomSource;
    // A global source of random numbers to be used by 'shuffle' algorithms
    // throughout this test driver.  As multiple shuffles are required, and
    // only one thread is running, a single global source of randomness should
    // provide the best guarantees to avoid accidentally repeating the same
    // shuffle with the same seeds.
#endif

// ============================================================================
//                         GLOBAL CLASSES FOR TESTING
// ----------------------------------------------------------------------------

class IntNode : public RbTreeNode {
    // A trivial node type holding an integer payload.

    // DATA
    int d_value;

  public:
    // MANIPULATORS
    int& value() { return d_value; }
        // Return a reference providing modifiable access to the 'value' of
        // this object.

    // ACCESSORS
    const int& value() const {return d_value; }
        // Return a reference providing non-modifiable access to the
        // 'value' of this object.
};

void printIntNodeValue(FILE *file, const RbTreeNode *node)
   // Print the integer value of the specified 'node' to the specified 'file'.
   // The behavior is undefined unless 'node' is a 'IntNode' object.
{
    fprintf(file, "%d", static_cast<const IntNode *>(node)->value());
}

void printIntTree(const RbTreeAnchor& tree)
   // Print the specified 'tree' to 'stdout'.  The behavior is undefined
   // unless 'tree' is comprised of 'IntNode' objects.

{
    Obj::printTreeStructure(stdout, tree.rootNode(), printIntNodeValue, 0, 3);
    printf("===================\n");
}

struct IntNodeComparator {
    // A 'RbTreeUtil' complaint node comparison functor for 'IntNode' objects.

    bool operator()(const RbTreeNode& lhs, const RbTreeNode& rhs) const {
        // Return 'true' if the integer value in the specified 'lhs' node is
        // less than that of the specified 'rhs' node.

        return static_cast<const IntNode&>(lhs).value() <
               static_cast<const IntNode&>(rhs).value();
    }
};

struct IntNodeValueComparator {
    // A 'RbTreeUtil' complaint node-value comparison functor for 'IntNode'
    // objects.

    bool operator()(const RbTreeNode& node, int value) const {
        // Return 'true' if the integer value in the specified 'node' is
        // less than the specified 'value'.

        return static_cast<const IntNode&>(node).value() < value;
    }

    bool operator()(int value, const RbTreeNode& node) const {
        // Return 'true' if the specified 'value' is less than the integer
        // value in the specified 'rhs' node.

        return value < static_cast<const IntNode&>(node).value();
    }
};

void assignIntNodeValue(IntNode *node, int value)
    // Assign to the specified 'node' the specified 'value'.  Note that this
    // function signature is designed to be used by the 'gg' generator function
    // below.
{
    node->value() = value;
}

IntNode *toIntNode(RbTreeNode *node)
{
    return static_cast<IntNode *>(node);
}

int prevIntNodeValue(const RbTreeAnchor& tree, RbTreeNode *node)
{
    if (tree.firstNode() == node) {
        return INT_MIN;                                               // RETURN
    }
    RbTreeNode *prev = Obj::previous(node);
    return toIntNode(prev)->value();
}

int nextIntNodeValue(const RbTreeAnchor& tree, RbTreeNode *node)
{
    RbTreeNode *next = Obj::next(node);
    if (tree.sentinel() == next) {
        return INT_MAX;                                               // RETURN
    }
    return toIntNode(next)->value();
}

class IntNodeAllocator{
    // Provide a factory for allocating 'IntNode' objects.  Note that the
    // signatures of this type match those required by 'FACTORY' template
    // parameters of 'RbTreeUtil' methods.

    // DATA
    IntNode     *d_nodes;
    mutable int  d_next;
    int          d_numNodes;

  public:
    IntNodeAllocator(IntNode *nodes, int numNodes)
    : d_nodes(nodes)
    , d_next(0)
    , d_numNodes(numNodes)
    {}

    RbTreeNode *createNode()
    {
        ASSERT(d_next < d_numNodes);
        IntNode *newNode = &d_nodes[d_next++];
        newNode->value() = 0;
        return newNode;
    }

    RbTreeNode *cloneNode(const RbTreeNode& value)
    {
        ASSERT(d_next < d_numNodes);
        IntNode *newNode = &d_nodes[d_next++];
        newNode->value() = static_cast<const IntNode&>(value).value();
        return newNode;
    }

    void deleteNode(RbTreeNode *)
    {
        --d_next;
    }
};

class ThrowableIntNodeAllocator{
    bslma::Allocator *d_allocator_p;

  public:
    ThrowableIntNodeAllocator(bslma::Allocator *basicAllocator)
    : d_allocator_p(basicAllocator)
    {}

    RbTreeNode *createNode()
    {
        IntNode *newNode = new (*d_allocator_p) IntNode;
        newNode->value() = 0;
        return newNode;
    }

    RbTreeNode *cloneNode(const RbTreeNode& value)
    {
        IntNode *newNode = new (*d_allocator_p) IntNode;
        newNode->value() = static_cast<const IntNode&>(value).value();
        return newNode;
    }

    void deleteNode(RbTreeNode *node)
    {
        //delete (*d_allocator_p, static_cast<IntNode *>(node));
        d_allocator_p->deallocate(static_cast<IntNode *>(node));
    }
};

void createBreathingTestTree(RbTreeAnchor *tree)
    // Load into the specified 'tree' a static tree of nodes matching the
    // following description.  The resulting 'tree' is built from a contiguous
    // array of 'IntNode', where the element with 0 index is at
    // 'tree->rootNode()'.
    //..
    // {N : [R|B]} - N indicates the index of the node in 'nodes'
    //             - [R|B] indicates whether the node is Red or Black
    //
    //                          20{0:B}
    //                        /        \.
    //                       /          \.
    //                    10{1:R}        30{5:R}
    //                   /    \         /    \.
    //                5{2:B}  15{4:B} 25{6:B}  35{8:B}
    //                /                \.
    //               1{3:R}            27{7:R}
    //..
    // The behavior is undefined if the returned 'tree' is modified.
{
    static IntNode nodes[9];
    memset(nodes, 0, sizeof(IntNode) * 9);

    nodes[0].setColor(BLACK);
    nodes[0].setLeftChild(&nodes[1]);
    nodes[0].setRightChild(&nodes[5]);
    nodes[0].setParent(tree->sentinel());
    nodes[0].value()  = 20;

    nodes[1].setColor(RED);
    nodes[1].setLeftChild(&nodes[2]);
    nodes[1].setRightChild(&nodes[4]);
    nodes[1].setParent(&nodes[0]);
    nodes[1].value()  = 10;

    nodes[2].setColor(BLACK);
    nodes[2].setLeftChild(&nodes[3]);
    nodes[2].setRightChild(0);
    nodes[2].setParent(&nodes[1]);
    nodes[2].value()  = 5;

    nodes[3].setColor(RED);
    nodes[3].setLeftChild(0);
    nodes[3].setRightChild(0);
    nodes[3].setParent(&nodes[2]);
    nodes[3].value()  = 1;

    nodes[4].setColor(BLACK);
    nodes[4].setLeftChild(0);
    nodes[4].setRightChild(0);
    nodes[4].setParent(&nodes[1]);
    nodes[4].value()  = 15;

    nodes[5].setColor(RED);
    nodes[5].setLeftChild(&nodes[6]);
    nodes[5].setRightChild(&nodes[8]);
    nodes[5].setParent(&nodes[0]);
    nodes[5].value()  = 30;

    nodes[6].setColor(BLACK);
    nodes[6].setLeftChild(0);
    nodes[6].setRightChild(&nodes[7]);
    nodes[6].setParent(&nodes[5]);
    nodes[6].value()  = 25;

    nodes[7].setColor(RED);
    nodes[7].setLeftChild(0);
    nodes[7].setRightChild(0);
    nodes[7].setParent(&nodes[6]);
    nodes[7].value()  = 27;

    nodes[8].setColor(BLACK);
    nodes[8].setLeftChild(0);
    nodes[8].setRightChild(0);
    nodes[8].setParent(&nodes[5]);
    nodes[8].value()  = 35;

    tree->reset(&nodes[0], &nodes[3], 9);
}

bool intLess(int lhs, int rhs)
{
    return lhs < rhs;
}

inline
const RbTreeNode *testNodeAtIndex(const IntNode       *nodes,
                                  const RbTreeAnchor&  tree,
                                  int                  nodeIndex)
   // Return the node at the specified 'nodeIndex' in the specified array of
   // 'nodes' if 'nodeIndex >= 0', and the sentinel node of the specified
   // 'tree' otherwise.
{
    return nodeIndex >= 0 ? &nodes[nodeIndex] : tree.sentinel();
}

template <class NODE_COMPARATOR, class NODE_PRINT_FUNC>
int validateTestRbTree(const RbTreeNode       *rootNode,
                       const NODE_COMPARATOR&  comparator,
                       const NODE_PRINT_FUNC&  nodePrintFunction)
     // Return the value of 'RbTreeUtil::validateRbTree' for the
     // specified 'node' using the specified 'comparator', and the specified
     // 'nodePrintFunction' to record the nodes value.  Note that this method
     // is provided to record additional error information regarding an
     // invalid tree.
{
    const char       *errorMessage;
    const RbTreeNode *errorNode;

    int ret = Obj::validateRbTree(&errorNode,
                                  &errorMessage,
                                  rootNode,
                                  comparator);
    if (ret < 0) {
        P_(errorMessage);
        printf(" ");
        nodePrintFunction(stdout, rootNode);
        printf("\n");
    }
    return ret;
}

int validateIntRbTree(const RbTreeNode *rootNode)
    // Return the value of 'RbTreeUtil::validateRbTree' and log any
    // error to the console.  Note that this method is provided to record
    // additional error information regarding an invalid tree.
{
    IntNodeComparator nodeComparator;
    return validateTestRbTree(rootNode, nodeComparator, &printIntNodeValue);
}

template <class VALUE, class ALLOCATOR = bsl::allocator<VALUE> >
class Array {
    // This class provides an array of objects of the parameterized 'VALUE'
    // type.  The size of the array is initialized by a call to 'reset'.  Note
    // that this provides a replacement for 'vector', which cannot be used
    // below 'bslstl', but is considerably simpler to implement because it is
    // no exception safe and does not grow dynamically.

    // DATA
    VALUE           *d_data_p;
    int              d_size;
    ALLOCATOR        d_allocator;
    // bslma::Allocator *d_allocator_p;

    Array(const Array&);
    Array& operator=(const Array&);

  public:

    // CREATORS
    Array(const ALLOCATOR& basicAllocator)
        // Create a new 'Array' for holding objects of the parameterized
        // 'VALUE' type, using the specified 'allocator' to supply memory.
    : d_data_p(0)
    , d_size(0)
    , d_allocator(basicAllocator)
    {
    }

    ~Array()
        // Destroy this object.
    {
        clear();
    }

    // MANIPULATORS
    void clear() {
        // Destroy all the elements in the array and set its size to 0.
        if (d_data_p) {
            bslalg::ArrayDestructionPrimitives::destroy(d_data_p,
                                                        d_data_p + d_size,
                                                        d_allocator);
            bsl::allocator_traits<ALLOCATOR>::deallocate(d_allocator,
                                                         d_data_p,
                                                         d_size);
            d_size = 0;
        }
    }

    void reset(int size) {
        // Destroy all the any elements in the array, and reallocate a
        // contiguous sequence of 'VALUE' objects of the specified 'size'
        clear();
        if (0 != size) {
            d_data_p = bsl::allocator_traits<ALLOCATOR>::allocate(d_allocator,
                                                                  size);
            bslalg::ArrayPrimitives::defaultConstruct(d_data_p,
                                                      size,
                                                      d_allocator);
        }
        d_size = size;
    }

    VALUE& operator[](int offset) { return d_data_p[offset]; }
        // Return the element at the specified 'offset'.

    VALUE *data() { return d_data_p; }
        // Return the address of the first data element in this array.

    // ACCESSORS
    const VALUE *data() const { return d_data_p; }
        // Return the address of the first data element in this array.


    int size() const { return d_size; }
        // Return the address of the first data element in this array.
};

struct DeleteTestNode : public RbTreeNode {
   // This 'struct' provides a test node type used to verify a deleter
   // has been called on a node.

   // DATA
   bool d_deleted;  // whether the deleter has been called on this node.
   int  d_value;
   DeleteTestNode() : d_deleted(false) {}
};

                       // ==============================
                       // class DeleteTestNodeComparator
                       // ==============================


class DeleteTestNodeComparator {
    // Provide a comparison functor for 'DeleteTestNode' objects.

  public:

    // CREATORS
    DeleteTestNodeComparator() {}

    // ACCESSORS
    bool operator()(int                       lhs,
                    const bslalg::RbTreeNode& rhs) const {
        return lhs < static_cast<const DeleteTestNode&>(rhs).d_value;
    }

    bool operator()(const bslalg::RbTreeNode& lhs,
                    int                       rhs) const {
        return static_cast<const DeleteTestNode&>(lhs).d_value < rhs;
    }

    bool operator()(const bslalg::RbTreeNode& lhs,
                    const bslalg::RbTreeNode& rhs) const {
        return static_cast<const DeleteTestNode&>(lhs).d_value <
               static_cast<const DeleteTestNode&>(rhs).d_value;
    }
};


void printDeleteTestNode(FILE *file, const RbTreeNode *node)
    // Write the value of 'node' to the specified 'file'.
{
    fprintf(file,
            "%d",
            static_cast<const DeleteTestNode *>(node)->d_deleted);
}

void deleteTestNodeAssign(DeleteTestNode *node, int value)
{
    node->d_value = value;
}

class DeleteTestNodeFactory {
    // Create a factory for 'DeleteTestNode' objects whose signatures
    // match the requirements of 'RbTreeUtil' methods accepting a 'FACTORY'
    // template parameter.

    // DATA
    int d_invocationCount;

    // NOT IMPLEMENTED
    DeleteTestNodeFactory(const DeleteTestNodeFactory&);
    DeleteTestNodeFactory& operator=(const DeleteTestNodeFactory&);

  public:
    // CREATORS
    DeleteTestNodeFactory() : d_invocationCount(0) {}

    // MANIPULATORS
    RbTreeNode *createNode()
    {
        BSLS_ASSERT(false);
        return 0;
    }

    RbTreeNode *cloneNode(const RbTreeNode& )
    {
        BSLS_ASSERT(false);
        return 0;
    }

    void deleteNode(RbTreeNode *node)
        // Mark the specified 'node' as deleted, and set its left, right, and
        // parent pointers to invalid pointer values.
    {
        ++d_invocationCount;
        DeleteTestNode *removedNode = static_cast<DeleteTestNode *>(node);
        ASSERT(false == removedNode->d_deleted);
        removedNode->d_deleted = true;
        removedNode->setParent(reinterpret_cast<RbTreeNode *>(0xdeadc0de));
        removedNode->setLeftChild(reinterpret_cast<RbTreeNode *>(0xdeadbeef));
        removedNode->setRightChild(reinterpret_cast<RbTreeNode *>(0xdeadbeef));
    }

    int numInvocations() const { return d_invocationCount; }
};

template <class NODE_COMPARATOR, class NODE_PRINT_FUNC>
bool isValidSearchTree(const RbTreeNode       *rootNode,
                       const NODE_COMPARATOR&  comparator,
                       const NODE_PRINT_FUNC&  nodePrinter)
    // Return 'true' if the specified 'node' refers to a valid binary search
    // tree ordered according to the specified 'comparator'.  This method
    // returns 'true' if and only if:
    //: 1 Both non-null children of every node refer to that node as their
    //:
    //: 2 All nodes to the left of 'node' are ordered at or before 'node',
    //:   and all nodes to the right of 'node' are ordered at or after
    //:   'node', as determined by the specified 'comparator'.
    //:
    //: 3 Rule (2) is recursively true of the left and right sub-trees of
    //:   'node'.
    // Note that this method explicitly doesn't verify that the tree is
    // valid red-black tree.
{
    if (0 == rootNode) {
        return true;                                                  // RETURN
    }

    const RbTreeNode *left  = rootNode->leftChild();
    const RbTreeNode *right = rootNode->rightChild();

    if ((left != 0 || right != 0) && left == right) {
        printf("Two equal children: ");
        nodePrinter(stdout, rootNode);
        printf("\n");
        return false;                                                 // RETURN
    }
    if ((left  && left->parent()  != rootNode) ||
        (right && right->parent() != rootNode)) {
        printf("Problem with childrens parent pointer: ");
        nodePrinter(stdout, rootNode);
        printf("\n");
        return false;                                                 // RETURN
    }

    if ((left  && comparator(*rootNode, *left)) ||
        (right && comparator(*right, *rootNode))) {
        printf("Invalid search tree: ");
        nodePrinter(stdout, rootNode);
        printf("\n");

        return false;                                                 // RETURN
    }

    return isValidSearchTree(left,  comparator, nodePrinter)
         ? isValidSearchTree(right, comparator, nodePrinter)
         : false;
}

template <class NODE_COMPARATOR>
bool areTreesEqual(const RbTreeNode       *left,
                   const RbTreeNode       *right,
                   const NODE_COMPARATOR&  comparator)
    // Return 'true' if the trees rooted at the specified 'left' and
    // 'specified' right compare equal using the specified 'nodeComparator'.
    // The behavior is undefined unless 'nodeComparator' provides a weak
    // ordering on the elements of both 'left' and 'right' trees.  Note that
    // this implementation uses recursion for simplicity.
{

    if (0 == left && 0 == right) {
        return true;                                                  // RETURN
    }
    if (0 == left || 0 == right) {
        return false;                                                 // RETURN
    }
    return (!comparator(*left, *right) && !comparator(*right, *left))
        && areTreesEqual(left->leftChild(), right->leftChild(), comparator)
        && areTreesEqual(left->rightChild(), right->rightChild(), comparator);
}

int countNodes(RbTreeNode *node)
    // Return the count of nodes under (and including) the specified 'node'.
{
    if (0 == node) {
        return 0;                                                     // RETURN
    }
    return 1 + countNodes(node->leftChild()) + countNodes(node->rightChild());
}


class RbTreeNodeRangeIterator {
    // This class provides a trivial iterator to simplify the process of
    // iterating over an inclusive range of 'RbTreeNode' objects.

     const RbTreeNode *d_it;  // current position
     const RbTreeNode *d_end; // end of the range (inclusive)

  public:

    RbTreeNodeRangeIterator(const RbTreeNode *begin,
                            const RbTreeNode *end)
        // Create a 'RbTreeNodeRangeIterator' that iterates over the range of
        // nodes between the specified 'begin' and 'end', inclusive.
    : d_it(begin)
    , d_end(end)
    {
    }

    RbTreeNode *next()
       // Return the next node in the range of nodes, or 0 if there are no more
       // nodes in the range.
    {
        const RbTreeNode *result = d_it;
        d_it = ((d_it == 0) || (d_it == d_end)) ? 0 : Obj::next(d_it);
        return const_cast<RbTreeNode*>(result);
    }
};

bool isInRange(const RbTreeNode *node,
               const RbTreeNode *first,
               const RbTreeNode *last)
    // Return 'true' if the specified 'node' is between the 'first' and 'last'
    // node inclusive.
{
    RbTreeNodeRangeIterator it(first, last);
    for (RbTreeNode *itNode = it.next(); 0 != itNode; itNode = it.next()) {
        if (node == itNode) {
            return true;                                              // RETURN
        }
    }
    return false;
}

RbTreeNode *nodeAtOffset(RbTreeAnchor *tree, int offset)
    // Return the node at the specified 'offset', according to an infix tree
    // traversal starting from the left-most node in 'tree'.
{
    RbTreeNode *node = tree->firstNode();
    while (offset > 0) {
        node = Obj::next(node);
        --offset;
    }
    return node;
}


const char *tempFileName(bool verboseFlag)
   // Return the potential name for a temporary file.  The returned C-string
   // refers to a static memory buffer (so this method is not thread safe).
{
    enum { MAX_LENGTH = 4096 };
    static char result[MAX_LENGTH];

#ifdef BSLS_PLATFORM_OS_WINDOWS
    BSLMF_ASSERT(MAX_LENGTH > MAX_PATH);

    char tmpPathBuf[MAX_PATH], tmpNameBuf[MAX_PATH];
    GetTempPath(MAX_PATH, tmpPathBuf);
    GetTempFileName(tmpPathBuf, "bael", 0, result);
#else
    char *temp = tempnam(0, "bael");
    strncpy(result, temp, MAX_LENGTH);
    free(temp);
#endif

    if (verboseFlag) printf("\tUse %s as a base filename.\n", result);

    return result;
}

void removeFile(const char *fileName)
    // Remove the file having the specified 'fileName' from the file-system.
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    DeleteFile(fileName);
#else
    unlink(fileName);
#endif
}

//=============================================================================
//                           TEMPLATE TEST FACILITIES
//=============================================================================

// This section provides a facility analogous to 'bsltf_templatetestfacility'
// (which cannot be directly included here for dependency reasons).
//
///Test Types and Associated Policies
///----------------------------------
//: o PrimitiveTestTypes         - a set of typedefs for primitive types
//: o SimpleTestType             - a simple non-allocating test object type
//: o AllocTestType              - a simple allocating test object
//: o TestTypeValue<VALUE>       - a policy for creating a test type value
//: o TestTypeComparator<VALUE>  - a policy for comparing test type objects
//
// The above allows support for primitive and two non-primitive types as the
// payload for test trees.  On top of the above types we also provide a
// framework for managing 'RbTreeNode' objects whose payload is one of
// those test types.  These types are analogous to 'bslstl_treenode',
// 'bslstl_treenodepool', and 'bslstl_setcomparator'.
//
///'RbTreeNode' Facilities
///-----------------------
//: o TestTreeNode<VALUE>         - a test node parameterized on payload value
//:                                 type.
//: o TestTreeNodeFactory<VALUE>  - a factory for creating and destroying
//:                                'TestNode' objects.
//: o TestNodeComparator<VALUE>   - a comparator for comparing 'TestNode'
//:                                 objects.
//: o TestNodeAssign<VALUE>       - a functor for assigning a value to a node
//
// Finally, a macros are provided to run a test on a series of test types:
//: o RUN_ON_ALL_TEST_TYPES(TestClass, TestFunction);



                        // =========================
                        // struct PrimitiveTestTypes
                        // =========================

struct PrimitiveTestTypes {
    // This 'struct' provides a namespace for utility functions that

    // PUBLIC TYPES
    typedef PrimitiveTestTypes* ObjectPtr;
        // This 'typedef' is an alias for a object pointer type.

    typedef void (*FunctionPtr) ();
        // This 'typedef' is an alias for a function pointer type.

    typedef std::size_t SizeT;
        // This 'typedef' is an alias for the 'size_t' type.
};


                        // ====================
                        // class SimpleTestType
                        // ====================

class SimpleTestType {
    // This unconstrained (value-semantic) attribute class does not allocate
    // memory and does not define any traits.  See the Attributes section under
    // @DESCRIPTION in the component-level documentation for information on the
    // class attributes.

    // DATA
    int d_data;  // integer class value

  public:

    // CREATORS
    SimpleTestType();
        // Create a 'SimpleTestType' object having the (default) attribute
        // values:
        //..
        //  data() == 0
        //..

    explicit SimpleTestType(int data);
        // Create a 'SimpleTestType' object having the specified 'data'
        // attribute value.

    SimpleTestType(const SimpleTestType& original);
        // Create a 'SimpleTestType' object having the same value
        // as the specified 'original' object.

    // ~SimpleTestType() = default;
        // Destroy this object.

    // MANIPULATORS
    SimpleTestType& operator=(const SimpleTestType& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.
};

// FREE OPERATORS
bool operator==(const SimpleTestType& lhs, const SimpleTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'SimpleTestType' objects
    // have the same if their 'data' attributes are the same.

bool operator!=(const SimpleTestType& lhs, const SimpleTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'SimpleTestType' objects do not
    // have the same value if their 'data' attributes are not the same.

                        // --------------------
                        // class SimpleTestType
                        // --------------------

// CREATORS
inline
SimpleTestType::SimpleTestType()
: d_data(0)
{
}

inline
SimpleTestType::SimpleTestType(int data)
: d_data(data)
{
}

inline
SimpleTestType::SimpleTestType(const SimpleTestType& original)
: d_data(original.d_data)
{
}

// MANIPULATORS
inline
SimpleTestType& SimpleTestType::operator=(const SimpleTestType& rhs)
{
    if (&rhs != this) {
        d_data = rhs.d_data;
    }
    return *this;
}

inline
void SimpleTestType::setData(int value)
{
    d_data = value;
}

// ACCESSORS
inline
int SimpleTestType::data() const
{
    return d_data;
}

// FREE OPERATORS
inline
bool operator==(const SimpleTestType& lhs, const SimpleTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool operator!=(const SimpleTestType& lhs, const SimpleTestType& rhs)
{
    return lhs.data() != rhs.data();
}

                        // ===================
                        // class AllocTestType
                        // ===================

class AllocTestType {
    // This unconstrained (value-semantic) attribute class that uses a
    // 'bslma::Allocator' to allocate memory and defines the type trait
    // 'bslma::UsesBslmaAllocator'.  See the Attributes section under
    // @DESCRIPTION in the component-level documentation for information on the
    // class attributes.

    // DATA
    int             *d_data_p;       // pointer to the integer class value
    bslma::Allocator *d_allocator_p;  // allocator, (held, not owned)
    AllocTestType   *d_self_p;       // pointer to self

  public:
    // CREATORS
    explicit AllocTestType(bslma::Allocator *basicAllocator = 0);
        // Create a 'AllocTestType' object having the (default) attribute
        // values:
        //..
        //  data() == 0
        //..
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    explicit AllocTestType(int data, bslma::Allocator *basicAllocator = 0);
        // Create a 'AllocTestType' object having the specified 'data'
        // attribute value.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    AllocTestType(const AllocTestType&  original,
                  bslma::Allocator      *basicAllocator = 0);
        // Create a 'AllocTestType' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    ~AllocTestType();
        // Destroy this object.

    // MANIPULATORS
    AllocTestType& operator=(const AllocTestType& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setData(int value);
        // Set the 'data' attribute of this object to the specified 'value'.

    // ACCESSORS
    int data() const;
        // Return the value of the 'data' attribute of this object.
};

// TRAITS
namespace BloombergLP {
namespace bslma {
template <> struct UsesBslmaAllocator<AllocTestType> : bsl::true_type {};
}  // close namespace bslma
}  // close enterprise namespace

// FREE OPERATORS
bool operator==(const AllocTestType& lhs, const AllocTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'AllocTestType' objects have the same
    // if their 'data' attributes are the same.

bool operator!=(const AllocTestType& lhs, const AllocTestType& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'AllocTestType' objects do not
    // have the same value if their 'data' attributes are not the same.

                        // -------------------
                        // class AllocTestType
                        // -------------------

// CREATORS
inline
AllocTestType::AllocTestType(bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_self_p(this)
{
    d_data_p = (int *) d_allocator_p->allocate(sizeof(int));
    *d_data_p = 0;
}

inline
AllocTestType::AllocTestType(int data, bslma::Allocator *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_self_p(this)
{
    d_data_p = (int *) d_allocator_p->allocate(sizeof(int));
    *d_data_p = data;
}

inline
AllocTestType::AllocTestType(const AllocTestType& original,
                             bslma::Allocator     *basicAllocator)
: d_allocator_p(bslma::Default::allocator(basicAllocator))
, d_self_p(this)
{
    d_data_p = (int *) d_allocator_p->allocate(sizeof(int));
    *d_data_p = *original.d_data_p;
}

inline
AllocTestType::~AllocTestType()
{
    // Ensure that this objects has not been bitwise moved.
    BSLS_ASSERT_OPT(this == d_self_p);
    d_allocator_p->deallocate(d_data_p);
}

// MANIPULATORS
inline
AllocTestType& AllocTestType::operator=(const AllocTestType& rhs)
{
    if (&rhs != this)
    {
        int *newData = (int *) d_allocator_p->allocate(sizeof(int));
        d_allocator_p->deallocate(d_data_p);
        d_data_p = newData;
        *d_data_p = *rhs.d_data_p;
    }
    return *this;
}

inline
void AllocTestType::setData(int value)
{
    *d_data_p = value;
}

// ACCESSORS
inline
int AllocTestType::data() const
{
    return *d_data_p;
}

// FREE OPERATORS
inline
bool operator==(const AllocTestType& lhs, const AllocTestType& rhs)
{
    return lhs.data() == rhs.data();
}

inline
bool operator!=(const AllocTestType& lhs, const AllocTestType& rhs)
{
    return lhs.data() != rhs.data();
}

                        // ===================
                        // class TestTypeValue
                        // ===================

template <class VALUE>
struct TestTypeValue {
    // Provide a functor for creating values of the parameterized 'VALUE'
    // type from an int, and to load an int from an object of the
    // parameterized 'VALUE' type.  Note that this primary template is
    // provided for fundamental types, while specializations provide support
    // for non-fundamental types (e.g., 'AllocTestType', 'SimpleTestType').

    VALUE create(int value) const {
        BSLS_ASSERT(0 <= value);

        return (VALUE)(value);
    }

    int getValue(const VALUE& value) const {
        return (int)(value);
    }
};

template <>
struct TestTypeValue<PrimitiveTestTypes::ObjectPtr>
{
    PrimitiveTestTypes::ObjectPtr create(int value) const {
        BSLS_ASSERT(0 <= value);

        return reinterpret_cast<PrimitiveTestTypes::ObjectPtr>(value);
    }

    int getValue(const PrimitiveTestTypes::ObjectPtr& value) const {
        bsls::Types::IntPtr v =
            reinterpret_cast<bsls::Types::IntPtr>(value);
        return static_cast<int>(v);
    }
};

template <>
struct TestTypeValue<PrimitiveTestTypes::FunctionPtr>
{
    PrimitiveTestTypes::FunctionPtr create(int value) const {
        BSLS_ASSERT(0 <= value);

        return reinterpret_cast<PrimitiveTestTypes::FunctionPtr>(value);
    }

    int getValue(const PrimitiveTestTypes::FunctionPtr& value) const {
        bsls::Types::IntPtr v =
            reinterpret_cast<bsls::Types::IntPtr>(value);
        return static_cast<int>(v);
    }
};


template <>
struct TestTypeValue<AllocTestType> {
    // Provide a specialization of 'TestTypeValue' to create, and obtain
    // the value of an 'AllocTestType'.

    AllocTestType create(int value) const {
        BSLS_ASSERT(0 <= value);
        AllocTestType result;
        result.setData(value);
        return result;
    }

    int getValue(const AllocTestType& value) const {
        return value.data();
    }
};

template <>
struct TestTypeValue<SimpleTestType> {
    // Provide a specialization of 'TestTypeValue' to create, and obtain
    // the value of an 'SimpleTestType'.

    SimpleTestType create(int value) const {
        BSLS_ASSERT(0 <= value);
        SimpleTestType result;
        result.setData(value);
        return result;
    }

    int getValue(const SimpleTestType& value) const {
        return value.data();
    }

};

                        // ========================
                        // class TestTypeComparator
                        // ========================

template <class VALUE>
struct TestTypeComparator {
    bool operator()(const VALUE& lhs, const VALUE& rhs) const {
        return lhs < rhs;
    }
};

template <>
struct TestTypeComparator<SimpleTestType>{
    bool operator()(const SimpleTestType& lhs, const SimpleTestType& rhs) const
    {
        return lhs.data() < rhs.data();
    }
};

template <>
struct TestTypeComparator<AllocTestType>{
    bool operator()(const AllocTestType& lhs, const AllocTestType& rhs) const {
        return lhs.data() < rhs.data();
    }
};

                        // ==================
                        // class TestTreeNode
                        // ==================

template <class VALUE>
class TestTreeNode : public bslalg::RbTreeNode {
    // This POD-like 'class' describes a node suitable for use in a red-black
    // binary search tree of values of the parameterized 'VALUE'.  This class
    // is a "POD-like" to facilitate efficient allocation and use in the
    // context of a container implementation.  In order to meet the essential
    // requirements of a POD type, this 'class' does not define a constructor
    // or destructor.  The manipulator, 'value', returns a modifiable reference
    // to 'd_value' so that it may be constructed in-place by the appropriate
    // 'bsl::allocator_traits' object.

    // DATA
    VALUE d_value;  // payload value

  private:
    // The following functions are not defined because a 'TestTreeNode' should
    // never be constructed, destructed, or assigned.  The 'd_value' member
    // should be separately constructed and destroyed using an appropriate
    // 'bsl::allocator_traits' object.

    TestTreeNode(const TestTreeNode&);             // Declared but not defined
    TestTreeNode& operator=(const TestTreeNode&);  // Declared but not defined

  public:
    // CREATORS
    TestTreeNode() : d_value() {}

    // MANIPULATORS
    VALUE& value() { return d_value; }
        // Return a reference providing modifiable access to the 'value' of
        // this object.

    // ACCESSORS
    const VALUE& value() const { return d_value; }
        // Return a reference providing non-modifiable access to the 'value' of
        // this object.
};

                       // ============================
                       // class TestTreeNodeComparator
                       // ============================

template <class VALUE>
class TestTreeNodeComparator {

     // DATA
     mutable int d_numComparisons;

     // NOT IMPLEMENTED
     TestTreeNodeComparator(const TestTreeNodeComparator &);
     TestTreeNodeComparator& operator=(const TestTreeNodeComparator &);
  public:
    // TYPES
    typedef TestTreeNode<VALUE> NodeType;
        // This 'typedef' is an alias for the node type, holding 'KEY' values,
        // on which comparisons are performed.

    // CREATORS
    TestTreeNodeComparator() : d_numComparisons(0) {}

    // ACCESSORS
    bool operator()(const VALUE&              lhs,
                    const bslalg::RbTreeNode& rhs) const {
        ++d_numComparisons;
        TestTypeComparator<VALUE> comparator;
        return comparator(lhs, static_cast<const NodeType&>(rhs).value());
    }

    bool operator()(const bslalg::RbTreeNode& lhs,
                    const VALUE&              rhs) const {
        ++d_numComparisons;
        TestTypeComparator<VALUE> comparator;
        return comparator(static_cast<const NodeType&>(lhs).value(), rhs);
    }

    bool operator()(const bslalg::RbTreeNode& lhs,
                    const bslalg::RbTreeNode& rhs) const {
        ++d_numComparisons;
        TestTypeComparator<VALUE> comparator;
        return comparator(static_cast<const NodeType&>(lhs).value(),
                          static_cast<const NodeType&>(rhs).value());
    }

    int numComparisons() const { return d_numComparisons; }
};


                       // ========================
                       // class TestTreeNodeAssign
                       // ========================


template <class VALUE>
class TestTreeNodeAssign {

  public:
    // TYPES
    typedef TestTreeNode<VALUE> NodeType;
        // This 'typedef' is an alias for the node type, holding 'KEY' values,
        // on which comparisons are performed.

    // CREATORS
    void operator()(NodeType *node, int value) const {
        node->value() = TestTypeValue<VALUE>().create(value);
    }

};


                        // ======================
                        // struct TestNodeFactory
                        // ======================

template <class VALUE>
class TestNodeFactory {

  public:
    // PUBLIC TYPES
    typedef TestTreeNode<VALUE> NodeType;
        // This 'typedef' is an alias for the node type, holding 'KEY' values,
        // on which comparisons are performed.

    enum {
        STATE_INVALID,
        STATE_CREATED,
        STATE_DELETED
    };

    struct TestException {
    };

  private:
    // DATA
    Array<NodeType> d_nodes;           // array of nodes to be returned
    Array<int>      d_nodeState;       // state of each node in 'd_nodes'
    int             d_numCreated;      // number of nodes allocated
    int             d_numDeleted;      // number of nodes deleted
    int             d_creationLimit;   // limit after which 'createNode' throws

    // NOT IMPLEMENTED
    TestNodeFactory(const TestNodeFactory&);
    TestNodeFactory& operator=(const TestNodeFactory&);

  public:
    TestNodeFactory(int              numNodes,
                    bslma::Allocator *allocator)
    : d_nodes(allocator)
    , d_nodeState(allocator)
    , d_numCreated(0)
    , d_numDeleted(0)
    , d_creationLimit(INT_MAX)
    {
        d_nodes.reset(numNodes);
        d_nodeState.reset(numNodes);
        for (int i = 0; i < numNodes; ++i) {
            d_nodeState[i] = STATE_INVALID;
        }
    }

    TestNodeFactory(int              numNodes,
                    int              creationLimit,
                    bslma::Allocator *allocator)
    : d_nodes(allocator)
    , d_nodeState(allocator)
    , d_numCreated(0)
    , d_numDeleted(0)
    , d_creationLimit(creationLimit)
    {
        d_nodes.reset(numNodes);
        d_nodeState.reset(numNodes);
        for (int i = 0; i < numNodes; ++i) {
            d_nodeState[i] = STATE_INVALID;
        }
    }


    RbTreeNode *cloneNode(const RbTreeNode& value) {

        const NodeType& nodeValue = static_cast<const NodeType&>(value);

        ASSERTV(d_numCreated, d_nodes.size(), d_numCreated < d_nodes.size());
        ASSERTV(d_nodeState[d_numCreated],
                STATE_INVALID == d_nodeState[d_numCreated]);

        if (d_numCreated >= d_creationLimit) {
#ifdef BDE_BUILD_TARGET_EXC
            throw TestException();
#else
            BSLS_ASSERT(false);
#endif
        }
        NodeType *resultNode      = &d_nodes[d_numCreated];
        d_nodeState[d_numCreated] = STATE_CREATED;

        resultNode->value() = nodeValue.value();

        ++d_numCreated;
        return resultNode;
    }

    void deleteNode(RbTreeNode *value) {
        NodeType *nodeValue = static_cast<NodeType *>(value);

        ASSERT(&d_nodes[0] <= nodeValue);
        ASSERT(nodeValue   <  &d_nodes[d_nodes.size()]);

        const int offset = static_cast<int>(nodeValue - &d_nodes[0]);

        ASSERT(0             <= offset);
        ASSERT(STATE_CREATED == d_nodeState[offset]);

        d_nodeState[offset] = STATE_DELETED;

        nodeValue->reset(reinterpret_cast<RbTreeNode *>(0xdeadc0de),
                         reinterpret_cast<RbTreeNode *>(0xdeadbeef),
                         reinterpret_cast<RbTreeNode *>(0xdeadbeef),
                         BLACK);
        ++d_numDeleted;
    }

    // ACCESSORS
    const Array<NodeType>& nodes() const { return d_nodes; }
    const Array<int>& nodeStates() const { return d_nodeState; }
    int numCreated() const { return d_numCreated; }
    int numDeleted() const { return d_numDeleted; }
};


#define RUN_ON_ALL_TEST_TYPES(TEST_DRIVER, TEST_FUNCTION) \
    TEST_DRIVER<int>::TEST_FUNCTION(); \
    TEST_DRIVER<bsls::Types::Int64>::TEST_FUNCTION(); \
    TEST_DRIVER<PrimitiveTestTypes::ObjectPtr>::TEST_FUNCTION(); \
    TEST_DRIVER<PrimitiveTestTypes::FunctionPtr>::TEST_FUNCTION(); \
    TEST_DRIVER<PrimitiveTestTypes::SizeT>::TEST_FUNCTION();   \
    TEST_DRIVER<SimpleTestType>::TEST_FUNCTION(); \
    TEST_DRIVER<AllocTestType>::TEST_FUNCTION(); \


//=============================================================================
//                        GENERATOR LANGUAGE FOR gg
//=============================================================================
//
// The gg function interprets a given 'spec' in order from left to right to
// build up a schedule according to a custom language.
//
//  Spec := NodeOrNull
//
//  NodeOrNull  := '.' | Node
//
//  Node :=  '{' NodeValue (NodeOrNull,NodeOrNull)? '}'
//
//  NodeValue := IntValue ':' RedOrBlack
//
//  IntValue := ['0'-'9']*
//
//  RedOrBlack := 'R' | 'B'
//
//
// 'gg' syntax usage examples:
//
// Spec String                                Resulting Tree
// -----------                                -------------
// "."                                        [empty tree]
//
//
// "{12:R}"                                     12(R)
//
//
// "{12:R {4:B, .}}                             12(R)
//                                              /
//                                           4(B)
//
//
// "{12:R {4:B {1:R}, {1:B}}, .}"               12(R)
//                                             /
//                                           4(B)
//                                         /    \.
//                                        1(R)   1(B)
//..

// PARSE_ASSERT(CONDITION, SPEC)
//      Abort the task if the specified 'CONDITION' is false,  and provide a
//      helpful error message include the specified tree 'SPEC'.
#define PARSE_ASSERT(CONDITION, SPEC)                                         \
    if (!(CONDITION)) {                                                       \
        printf("Parse error " #CONDITION ": %s\n", (SPEC));                   \
        BSLS_ASSERT(false);                                                   \
    }                                                                         \

const char *ltrim(const char *spec)
    // Return the first non-white space character in the specified 'spec'.
{
    while (*spec && isspace(*spec)) {
        ++spec;
    }
    return spec;
}


template <class NODE_TYPE, class NODE_ASSIGN_FUNCTOR>
const char *ggParseNode(
               NODE_TYPE                  **nextNode,
               const char                  *spec,
               const NODE_ASSIGN_FUNCTOR&   assignFunction)
    // Populate the specified 'nextNode' with the first node described in the
    // specified 'spec' and advance 'nextNode' to the next free node in the
    // sequence of nodes after parsing the rest of the sub-tree described by
    // the first node in 'spec'.  Return the first character in 'spec' that
    // does not refer to the sub-tree of the first element described by
    // 'spec'.  The behavior is undefined unless 'spec' is a valid 'gg'
    // specification for a red-black tree, and 'nextNode' is not an address
    // of an address of an array of nodes large enough to contain the tree
    // described by 'spec'.
{
    // Pop the first node off the 'nextNode' to use for the root of this
    // sub-tree.

    NODE_TYPE *newNode = *nextNode;
    *nextNode = *nextNode + 1;

    const char *currentChar = ltrim(spec);

    PARSE_ASSERT(*currentChar == '{', spec);

    currentChar = ltrim(currentChar + 1);

    PARSE_ASSERT(isdigit(*currentChar), spec);

    assignFunction(newNode, atoi(currentChar));
    while (isdigit((int)*currentChar)) {
        ++currentChar;
    }
    currentChar = ltrim(currentChar);

    PARSE_ASSERT(':' == *currentChar, spec);
    currentChar = ltrim(currentChar+1);

    PARSE_ASSERT('R' == *currentChar || 'B' == *currentChar, spec);
    newNode->setColor(('R' == *currentChar) ? RED : BLACK);
    newNode->setParent(0);
    newNode->setLeftChild(0);
    newNode->setRightChild(0);

    currentChar = ltrim(currentChar+1);

    PARSE_ASSERT('{' != *currentChar ||
                 '}' != *currentChar ||
                 '.' != *currentChar,
                 spec);

    if ('}' == *currentChar) {
        return currentChar + 1;                                       // RETURN
    }

    if ('.' == *currentChar) {
        ++currentChar;
        newNode->setLeftChild(0);
    }
    else {
        newNode->setLeftChild(*nextNode);
        currentChar = ggParseNode(nextNode, currentChar, assignFunction);
        newNode->leftChild()->setParent(newNode);
    }

    currentChar = ltrim(currentChar);
    PARSE_ASSERT(',' == *currentChar, spec);
    currentChar = ltrim(currentChar+1);

    PARSE_ASSERT('{' != *currentChar ||
                 '.' != *currentChar,
                 spec);

    if ('.' == *currentChar) {
        ++currentChar;
        newNode->setRightChild(0);
    }
    else {
        newNode->setRightChild(*nextNode);
        currentChar = ggParseNode(nextNode, currentChar, assignFunction);
        newNode->rightChild()->setParent(newNode);
    }
    currentChar = ltrim(currentChar);

    PARSE_ASSERT('}' == *currentChar, spec);
    return currentChar + 1;
}

// CLASS METHODS
bool isWellFormedAnchor(const RbTreeAnchor& tree)
   // Return 'true' if the specified 'tree' is well-formed, and 'false'
   // otherwise.  This enforces the rules required by a well formed anchor, as
   // documented in 'RbTreeUtil_Validator::isWellFormedAnchor' *except* that
   // the root node of the tree may be red.  Note that this method is provided
   // for use by 'gg' to enable the creation of test tree's with a red root
   // node in order to, for example, test 'RbTreeUtil::validateRbTree'.
{
    // We will test rule 2 last, as iterating over the nodes in tee results in
    // undefined behavior if the other rules are not met.

    //: 1 'tree.firstNode()' must refer to 'tree.sentinel()' if
    //:   'tree.rootNode()' is 0, and leftmost(tree.rootNode())' otherwise.

    if (!tree.rootNode() && tree.firstNode() != tree.sentinel()) {
        return false;                                                 // RETURN
    }
    if (tree.rootNode() &&
        tree.firstNode() != RbTreeUtil::leftmost(tree.rootNode())) {
        return false;                                                 // RETURN
    }

    //: 3 'tree.sentinel()->leftchild()' is 'tree.rootNode()', and (if
    //:   'tree.rootNode()' is not 0), 'tree.rootNode()->parent()' is
    //:   'tree.sentinel().

    if ((tree.sentinel()->leftChild() != tree.rootNode()) ||
        (tree.rootNode() && (tree.rootNode()->parent() != tree.sentinel()))) {
        return false;                                                 // RETURN
    }

    //: 2 'tree.nodeCount()' must be the count of nodes in 'tree' (not
    //:    including the sentinel node).

    int count = 0;
    const RbTreeNode *node = tree.firstNode();
    while (tree.sentinel() != node) {
        ++count;
        node = RbTreeUtil::next(node);
    }
    return count == tree.numNodes();
}

template <class NODE_TYPE, class NODE_ASSIGN_FUNCTOR>
void gg(RbTreeAnchor                *tree,
        Array<NODE_TYPE>            *nodes,
        const char                  *spec,
        const NODE_ASSIGN_FUNCTOR&   assignFunction)
   // Return the root of a red-black tree described by the specified 'spec',
   // and populate 'nodes' with the array of nodes that make up that tree,
   // using the specified 'nodeAssignValueFunc' to assign a value for a node
   // from an integer value.
{
    // Count the number of nodes.
    int numNodes = 0;
    for (const char *c = spec; *c; ++c) {
        if (':' == *c) {
            ++numNodes;
        }
    }

    if (numNodes == 0) {
        PARSE_ASSERT('.' == *ltrim(spec), spec);
        tree->reset(0, tree->sentinel(), 0);
        return;                                                       // RETURN
    }

    nodes->reset(numNodes);

    spec = ltrim(spec);

    NODE_TYPE *nextNode = nodes->data();
    spec = ggParseNode(&nextNode, spec, assignFunction);

    PARSE_ASSERT(!*ltrim(spec), spec);

    RbTreeNode *root = nodes->data();
    tree->reset(root, Obj::leftmost(root), numNodes);
    root->setParent(tree->sentinel());

    ASSERT(isWellFormedAnchor(*tree));
}

void intNodeTreeToSpec(IntNode *root)
{
    if (0 == root) {
        printf(".");
        return;                                                       // RETURN
    }

    printf("{%d:%c", root->value(), (root->isRed() ? 'R' : 'B'));
    if (0 == root->leftChild() && 0 == root->rightChild()) {
        printf("}");
        return;                                                       // RETURN
    }
    printf(" ");
    intNodeTreeToSpec(static_cast<IntNode*>(root->leftChild()));
    printf(",");
    intNodeTreeToSpec(static_cast<IntNode*>(root->rightChild()));
    printf("}");
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating and Using a Tree with 'RbTreeUtil'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create a tree of integers using
// 'RbTreeUtil'.
//
// First, we define a type 'SimpleIntNode' that will represent a nodes in our
// tree of integer values.  'SimpleIntNode' contains an 'int' payload, and
// inherits from 'RbTreeNode', allowing it to be operated on by
// 'RbTreeUtil'.
//..
    struct SimpleIntNode : public RbTreeNode {
        int d_value;
    };
//..
// Then, we define a comparison function for 'SimpleIntNode' objects (note
// that we static-cast 'RBTreeNode' objects to the actual node type,
// 'SimpleIntNode', for comparison purposes):
//..
    struct SimpleIntNodeValueComparator {
        // This class defines a comparator providing comparison operations
        // between 'SimpleIntNode' objects, and 'int' values.
//
        bool operator()(const RbTreeNode& lhs, int rhs) const
        {
            return static_cast<const SimpleIntNode&>(lhs).d_value < rhs;
        }
//
        bool operator()(int lhs, const RbTreeNode& rhs) const
        {
            return lhs < static_cast<const SimpleIntNode&>(rhs).d_value;
        }
    };
//
//..
// Next, we begin to define the example function that will build a tree of
// nodes holding integer values:
//..
    void createTestTreeExample()
    {
//..
// Then, within this function, we define a 'RbTreeAnchor' object that will
// hold the root, first, last, and sentinel nodes of tree, as well a count of
// the number of nodes in the tree:
//..
        RbTreeAnchor tree;
//..
// Next, we define an array of 5 'SimpleIntNode' objects that we will insert
// into the tree; in practice, nodes are more often allocated on the heap (see
// example 2):
//..
        const int NUM_NODES = 5;
        SimpleIntNode nodes[NUM_NODES];
//..
// Then, we assign unique values to each of the 'nodes':
//..
        for (int i = 0; i < NUM_NODES; ++i) {
            nodes[i].d_value = i;
        }
//..
// Now, for each node in the tree, we use 'RbTreeUtil' to first find the
// location at which the node should be inserted, and then insert that node
// into the tree:
//..
        for (int i = 0; i < NUM_NODES; ++i) {
            int comparisonResult;
            SimpleIntNodeValueComparator comparator;
            RbTreeNode *insertLocation = RbTreeUtil::findUniqueInsertLocation(
                                                &comparisonResult,
                                                &tree,
                                                comparator,
                                                nodes[i].d_value);
            BSLS_ASSERT(comparisonResult);
            RbTreeUtil::insertAt(&tree,
                                 insertLocation,
                                 comparisonResult < 0,
                                 &nodes[i]);
        }
//..
// And verify the resulting 'tree' holds 5 nodes, and the first node has
// the value 0:
//..
        ASSERT(5 == tree.numNodes());
        ASSERT(0 == static_cast<SimpleIntNode *>(tree.firstNode())->d_value);
//..
// Finally, we use 'RbTreeUtil' to iterate through the nodes of 'tree', and
// write the value of each node to the console:
//..
        const RbTreeNode *nodeIterator = tree.firstNode();
        while (tree.sentinel() != nodeIterator) {
if (verbose) {
            printf("Node value: %d\n",
                   static_cast<const SimpleIntNode *>(nodeIterator)->d_value);
}
            nodeIterator = RbTreeUtil::next(nodeIterator);
        }
    }
//..
// Notice that each of the 'RbTreeNode' objects must be 'static_cast' to the
// derived type, 'SimpleIntNode', in order to access their values.
//
// The resulting output is displayed on the console:
//..
//  Node value: 0
//  Node value: 1
//  Node value: 2
//  Node value: 3
//  Node value: 4
//..
//
///Example 2: Implementing a Set of Integers
///- - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to use 'RbTreeUtil' to implement a simple
// container holding a set of (unique) integer values as a red-black binary
// search tree.
//
// Before defining the 'IntSet' class, we need to define a series of
// associated helper types:
//: 1 The node-type, for the nodes in the tree.
//: 2 An iterator, for iterating over nodes in the tree.
//: 3 A comparison functor for comparing nodes and values.
//: 4 A factory for creating and destroying nodes.
//
// First, we define a type, 'IntSet_Node', that will represent the nodes in our
// tree of integer values; it contains an 'int' payload, and inherits from
// 'RbTreeNode', allowing it to be operated on by 'RbTreeUtil' (note that the
// underscore "_" indicates that this type is a private implementation type of
// 'IntSet', and not for use by clients of 'IntSet'):
//..
    class IntSet_Node : public RbTreeNode {
        // A red-black tree node containing an integer data-value.
//
        // DATA
        int d_value;  // actual value represented by the node
//
      public:
        // MANIPULATORS
        int& value() { return d_value; }
            // Return a reference providing modifiable access to the 'value' of
            // this object.
//
        // ACCESSORS
        const int& value() const { return d_value; }
            // Return a reference providing non-modifiable access to the
            // 'value' of this object.
    };
//..
// Then, we define a iterator over 'IntSet_Node' objects.  We use the 'next'
// function of 'RbTreeUtil' to increment the iterator (note that, for
// simplicity, this iterator is *not* a fully STL compliant iterator
// implementation):
//..
    class IntSetConstIterator {
        // This class defines an STL-style iterator over a non-modifiable tree
        // of 'IntSet_Node' objects.
//
        // DATA
        const RbTreeNode *d_node_p;  // current location of this iterator
//
      public:
        IntSetConstIterator() : d_node_p(0) {}
            // Create an iterator that does not refer to a node.
//
        IntSetConstIterator(const RbTreeNode *node) : d_node_p(node) {}
            // Create an iterator referring to the specified 'node'.
//
    //  IntSetConstIterator(const IntSetConstIterator&) = default;
//
        // MANIPULATOR
    //  IntSetConstIterator& operator=(const IntSetConstIterator&) = default;
//
//..
// Here, we implement the prefix-increment operator using the 'next' function
// of 'RbTreeUtil:
//..
        IntSetConstIterator& operator++()
           // Advance this iterator to the subsequent value it the 'IntSet',
           // and return a reference providing modifiable access to this
           // iterator.   The behavior is undefined unless this iterator
           // refers to a element in an 'IntSet'.
        {
            d_node_p = RbTreeUtil::next(d_node_p);
            return *this;
        }
//
        // ACCESSORS
        const int& operator*() const
            // Return a reference providing non-modifiable access to the value
            // referred to by this iterator.
        {
            return static_cast<const IntSet_Node *>(d_node_p)->value();
        }
//
        const int *operator->() const
            // Return an address providing non-modifiable access to the value
            // referred to by this iterator.
        {
            return &(static_cast<const IntSet_Node *>(d_node_p)->value());
        }
//
        const IntSet_Node *nodePtr() const
            // Return the address of the non-modifiable int-set node referred
            // to by this iterator
        {
            return static_cast<const IntSet_Node *>(d_node_p);
        }
    };
//
    // FREE OPERATORS
    bool operator==(const IntSetConstIterator &lhs,
                    const IntSetConstIterator &rhs)
        // Return 'true' if the 'lhs' and 'rhs' objects have the same value,
        // and 'false' otherwise.  Two 'IntSetConstIterator' objects have the
        // same value if they refer to the same node.
    {
        return lhs.nodePtr() == rhs.nodePtr();
    }
//
    bool operator!=(const IntSetConstIterator &lhs,
                    const IntSetConstIterator &rhs)
        // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
        // the same value, and 'false' otherwise.  Two 'IntSetConstIterator'
        // objects do not have the same value if they refer to different nodes.
    {
        return lhs.nodePtr() != rhs.nodePtr();
    }
//..
// Next, we define a comparison functor for 'IntSet_Node' objects, which will
// be supplied to 'RbTreeUtil' functions that must compare nodes with values
// -- i.e., those with a 'NODE_VALUE_COMPARATOR' template parameter (e.g.,
// 'find' and 'findInsertLocation'):
//..
    struct IntSet_NodeValueComparator {
        // This class defines a comparator providing comparison operations
        // between 'IntSet_Node' objects, and 'int' values.
//
        bool operator()(const RbTreeNode& lhs, int rhs) const
        {
            return static_cast<const IntSet_Node&>(lhs).value() < rhs;
        }
//
        bool operator()(int lhs, const RbTreeNode& rhs) const
        {
            return lhs < static_cast<const IntSet_Node&>(rhs).value();
        }
    };
//..
// Notice that we static-cast 'RbTreeNode' objects to the actual node type,
// 'IntSet_Node' for comparison.
//
// Next, we define a factory for creating and destroying 'IntSet_Node'
// objects.  This factory provides the operations 'createNode' and
// 'deleteNode'.  These operations will be used directly by our container
// implementation, and they are also required by 'RbTreeUtil' functions taking
// a 'FACTORY' template parameter (e.g., 'copyTree' and 'deleteTree'):
//..
    class IntSet_NodeFactory {
        // This class defines a creator object, that when invoked, creates a
        // new 'IntSet_Node' (either from a int value, or an existing
        // 'IntSet_Node' object) using the allocator supplied at construction.
//
        bslma::Allocator *d_allocator_p;  // allocator, (held, not owned)
//
      public:
//
        IntSet_NodeFactory(bslma::Allocator *allocator)
        : d_allocator_p(allocator)
        {
            BSLS_ASSERT_SAFE(allocator);
        }
//
        RbTreeNode *createNode(int value) const
        {
            IntSet_Node *newNode = new (*d_allocator_p) IntSet_Node;
            newNode->value() = value;
            return newNode;
        }
//
        RbTreeNode *cloneNode(const RbTreeNode& node) const
        {
            IntSet_Node *newNode = new (*d_allocator_p) IntSet_Node;
            newNode->value() = static_cast<const IntSet_Node&>(node).value();
            return newNode;
        }
        void deleteNode(RbTreeNode *node) const
        {
            d_allocator_p->deleteObject(static_cast<IntSet_Node *>(node));
        }
//
        bslma::Allocator *allocator() const
        {
            return d_allocator_p;
        }
    };
//..
// Then, having defined the requisite helper types, we define the public
// interface for our 'IntSet' type.  Note that for the purposes of
// illustrating the use of 'RbTreeUtil' a number of simplifications have been
// made.  For example, this implementation provides only a minimal set of
// critical operations, and it does not use the empty base-class optimization
// for the comparator, etc.  We define the interface of 'IntSet' as follows:
//..
    class IntSet {
        // This class implements a set of (unique) 'int' values.
//
        // DATA
        RbTreeAnchor           d_tree;         // root, first, and last tree
                                               // nodes
//
        IntSet_NodeValueComparator
                               d_comparator;   // comparison functor for 'int's
//
        IntSet_NodeFactory     d_nodeFactory;  // factory for creating and
                                               // destroying nodes
//
        // FRIENDS
        friend bool operator==(const IntSet& lhs, const IntSet& rhs);
//
      public:
        // PUBLIC TYPES
        typedef IntSetConstIterator const_iterator;
//
        // CREATORS
        IntSet(bslma::Allocator *basicAllocator = 0);
            // Create a empty 'IntSet'.  Optionally specify a 'basicAllocator'
            // used to supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.
//
        IntSet(const IntSet& original, bslma::Allocator *basicAllocator = 0);
            // Create a 'IntSet' object having the same value as the specified
            // 'original' object.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.
//
        ~IntSet();
            // Destroy this object.
//
        // MANIPULATORS
        IntSet& operator=(const IntSet& rhs);
            // Assign to this object the value of the specified 'rhs' object,
            // and return a reference providing modifiable access to this
            // object.
//
        const_iterator insert(int value);
            // If the specified 'value' is not already a member of this set,
            // insert it into this set, returning an iterator referring to the
            // newly added value, and return an iterator referring to the
            // existing instance of 'value' in this set, with no other effect,
            // otherwise.
//
        const_iterator erase(const_iterator iterator);
            // Remove the value referred to by the specified 'iterator' from
            // this set, and return an iterator referring to the value
            // subsequent to 'iterator' (prior to its removal).  The behavior
            // is undefined unless 'iterator' refers to a valid value in this
            // set.
//
        void clear();
            // Remove all the elements from this set.
//
        void swap(IntSet& other);
            // Efficiently exchange the value of this object with the value of
            // the specified 'other' object.
//
        // ACCESSORS
        const_iterator begin() const;
            // Return an iterator referring leftmost node value in this set, or
            // 'end()' if this set is empty.
//
        const_iterator end() const;
            // Return an iterator referring to the value one past the
            // rightmost value in this set.
//
        const_iterator find(int value) const;
            // Return a iterator referring to the specified 'value' in this
            // set, or 'end()' if 'value' is not a member of this set.
//
        int size() const;
            // Return the number of elements in this set.
    };
//
    // FREE OPERATORS
    bool operator==(const IntSet& lhs, const IntSet& rhs);
        // Return 'true' if the 'lhs' and 'rhs' objects have the same value,
        // and 'false' otherwise.  Two 'IntSet' objects have the same value if
        // they contain the same number of elements, and if for each element
        // in 'lhs' there is a corresponding element in 'rhs' with the same
        // value.
//
    bool operator!=(const IntSet& lhs, const IntSet& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
        // the same value, and 'false' otherwise.  Two 'IntSet' objects do not
        // have the same value if they differ in the number of elements they
        // contain, or if for any element in 'lhs' there is not a
        // corresponding element in 'rhs' with the same value.
//..
// Now, we implement the methods of 'IntSet' using 'RbTreeUtil' and the
// helper types we defined earlier:
//..
    // CREATORS
    IntSet::IntSet(bslma::Allocator *basicAllocator)
    : d_tree()
    , d_comparator()
    , d_nodeFactory(bslma::Default::allocator(basicAllocator))
    {
    }
//
    IntSet::IntSet(const IntSet& original, bslma::Allocator *basicAllocator)
    : d_tree()
    , d_comparator()
    , d_nodeFactory(bslma::Default::allocator(basicAllocator))
    {
        if (original.d_tree.rootNode()) {
            RbTreeUtil::copyTree(&d_tree, original.d_tree, &d_nodeFactory);
        }
    }
//
    IntSet::~IntSet()
    {
        clear();
    }
//
    // MANIPULATORS
    IntSet& IntSet::operator=(const IntSet& rhs)
    {
        IntSet temp(rhs, d_nodeFactory.allocator());
        swap(temp);
        return *this;
    }
//
//..
// Here, we implement 'insert' by using the 'RbTreeUtil' algorithms
// 'findUniqueInsertLocation' and 'insertAt':
//..
    IntSet::const_iterator IntSet::insert(int value)
    {
        // To insert a value into the tree, we first find the location where
        // the node would be added, and whether 'value' is unique.  If 'value'
        // is not unique we do not want to incur the expense of allocating
        // memory for a node.
//
        int comparisonResult;
        RbTreeNode *insertLocation =
                      RbTreeUtil::findUniqueInsertLocation(&comparisonResult,
                                                           &d_tree,
                                                           d_comparator,
                                                           value);
        if (0 == comparisonResult) {
            // 'value' already exists in 'd_tree'.
//
            return const_iterator(insertLocation);                    // RETURN
        }
//
        // If 'value' is unique, we create a new node and supply it to
        // 'insertAt', along with the tree location returned by
        // 'findUniqueInsertLocation'.
//
        RbTreeNode *newNode = d_nodeFactory.createNode(value);
        RbTreeUtil::insertAt(&d_tree,
                             insertLocation,
                             comparisonResult < 0,
                             newNode);
        return const_iterator(newNode);
    }
//
    IntSet::const_iterator IntSet::erase(const_iterator iterator)
    {
        BSLS_ASSERT(iterator.nodePtr());
        IntSet_Node *node = const_cast<IntSet_Node *>(iterator.nodePtr());
//
        // Before removing the node, we first find the subsequent node to which
        // we will return an iterator.
//
        RbTreeNode *next = RbTreeUtil::next(node);
        RbTreeUtil::remove(&d_tree, node);
        d_nodeFactory.deleteNode(node);
        return const_iterator(next);
    }
//
    void IntSet::clear()
    {
        if (d_tree.rootNode()) {
            RbTreeUtil::deleteTree(&d_tree, &d_nodeFactory);
        }
    }
//
    void IntSet::swap(IntSet& other) {
        BSLS_ASSERT(d_nodeFactory.allocator() ==
                    other.d_nodeFactory.allocator());
        RbTreeUtil::swap(&d_tree, &other.d_tree);
    }
//
    // ACCESSORS
    IntSet::const_iterator IntSet::begin() const
    {
        return const_iterator(d_tree.firstNode());
    }
//
    IntSet::const_iterator IntSet::end() const
    {
        return const_iterator(d_tree.sentinel());
    }
//
    IntSet::const_iterator IntSet::find(int value) const
    {
        return const_iterator(RbTreeUtil::find(d_tree, d_comparator, value));
    }
//
    int IntSet::size() const
    {
        return d_tree.numNodes();
    }
//..
// Finally, we implement the free operators on 'IntSet':
//..
    // FREE OPERATORS
    bool operator==(const IntSet& lhs, const IntSet& rhs)
    {
        return bslalg::RangeCompare::equal(lhs.begin(),
                                          lhs.end(),
                                          lhs.size(),
                                          rhs.begin(),
                                          rhs.end(),
                                          rhs.size());
    }
//
    bool operator!=(const IntSet& lhs, const IntSet& rhs)
    {
        return !(lhs == rhs);
    }
//..

void printIntSetNode(FILE *file, const RbTreeNode *node)
{
    fprintf(file, "%d", static_cast<const IntSet_Node *>(node)->value());
}


//=============================================================================
//                            GLOBAL TEST-DATA
//-----------------------------------------------------------------------------

struct TreeSpec {
    int         d_line;
    const char *d_spec;
    int         d_blackDepth;
} TREE_VALUES[] = {
        // Every possible tree with 1, 2, 3, 4, 5 unique nodes.
{ L_, "{5:B}" ,             1 },

{ L_, "{4:B {2:R},.}" ,     1 },
{ L_, "{2:B .,{4:R}}" ,     1 },

{ L_, "{4:B {2:B},{6:B}}" , 2 },
{ L_, "{4:B {2:R},{6:R}}" , 1 },

{ L_, "{4:B {2:B},{6:B .,{8:R}}}", 2 },
{ L_, "{4:B {2:B},{8:B {6:R},.}}", 2 },
{ L_, "{6:B {4:B {2:R},.},{8:B}}", 2 },
{ L_, "{6:B {2:B .,{4:R}},{8:B}}", 2 },

{ L_, "{4:B {2:B},{8:B {6:R},{10:R}}}", 2},
{ L_, "{6:B {2:B .,{4:R}},{10:B {8:R},.}}", 2},
{ L_, "{6:B {2:B .,{4:R}},{8:B .,{10:R}}}", 2},
{ L_, "{6:B {4:B {2:R},.},{10:B {8:R},.}}", 2},
{ L_, "{6:B {4:B {2:R},.},{8:B .,{10:R}}}", 2},
{ L_, "{8:B {4:B {2:R},{6:R}},{10:B}}", 2},

// Test 4, 5 nodes with duplicate nodes
{ L_, "{2:B {2:B},{4:B .,{4:R}}}", 2 },
{ L_, "{4:B {2:B .,{2:R}},{4:B}}", 2 },
{ L_, "{4:B {2:B},{6:B {4:R},{6:R}}}", 2},
{ L_, "{4:B {4:B {2:R},.},{6:B .,{6:R}}}", 2},
{ L_, "{6:B {4:B {2:R},{4:R}},{6:B}}", 2},

// Test 3 random unique combinations of 10-15 nodes
{ L_, "{10:B {6:R {4:B {2:R},.},{8:B}},"
      "{16:R {12:B .,{14:R}},{18:B .,{20:R}}}}", 2},
{ L_, "{6:B {4:B {2:R},.},{14:R {10:B {8:R},"
      "{12:R}},{18:B {16:R},{20:R}}}}", 2},
{ L_, "{8:B {4:B {2:R},{6:R}},{16:R {12:B "
      "{10:R},{14:R}},{18:B .,{20:R}}}}", 2},
{ L_, "{10:B {4:R {2:B},{8:B {6:R},.}},{18:R {14:B "
      "{12:R},{16:R}},{22:B {20:R},.}}}", 2},
{ L_, "{14:B {6:R {2:B .,{4:R}},{10:B {8:R},{12:R}}},{18:R "
      "{16:B},{20:B .,{22:R}}}}", 2},
{ L_, "{14:B {8:R {4:B {2:R},{6:R}},{12:B {10:R},.}},{20:R "
      "{18:B {16:R},.},{22:B}}}", 2},
{ L_, "{12:B {8:B {4:B {2:R},{6:R}},{10:B}},{16:B "
      " {14:B},{22:R {18:B .,{20:R}},{24:B}}}}", 3},
{ L_, "{16:B {8:R {4:B {2:R},{6:R}},{12:B {10:R},{14:R}}},"
      "{20:R {18:B},{22:B .,{24:R}}}}", 2},
{ L_, "{14:B {8:R {4:B {2:R},{6:R}},{10:B .,{12:R}}},"
      "{20:R {18:B {16:R},.},{24:B {22:R},.}}}", 2},
{ L_, "{14:B {8:B {4:B {2:R},{6:R}},{10:B .,{12:R}}},"
      "{18:B {16:B},{24:R {20:B .,{22:R}},{26:B}}}}", 3},
{ L_, "{12:B {6:B {2:B .,{4:R}},{8:B .,{10:R}}},{16:B"
      " {14:B},{24:R {20:B {18:R},{22:R}},{26:B}}}}", 3},
{ L_, "{10:B {4:B {2:B},{6:B .,{8:R}}},{16:B {12:B "
      ".,{14:R}},{22:R {18:B .,{20:R}},{24:B .,{26:R}}}}}", 3},
{ L_, "{16:B {12:B {8:R {4:B {2:R},{6:R}},{10:B}},{14:B}},"
      "{22:B {18:B .,{20:R}},{26:B {24:R},{28:R}}}}", 3},
{ L_, "{14:B {8:R {4:B {2:R},{6:R}},{12:B {10:R},.}},{22:R "
      "{18:B {16:R},{20:R}},{26:B {24:R},{28:R}}}}", 2},
{ L_, "{14:B {6:B {2:B .,{4:R}},{10:B {8:R},{12:R}}},"
      "{26:B {22:R {18:B {16:R},{20:R}},{24:B}},{28:B}}}", 3},

// Test random combination of 10-15 nodes  with duplicates
{ L_, "{4:B {2:B {2:B},{2:B}},{4:B {4:B},"
      "{6:R {6:B},{8:B {6:R},.}}}}", 3},
{ L_, "{6:B {4:R {2:B {2:R},{2:R}},{4:B .,{4:R}}},"
      "{8:R {6:B .,{6:R}},{8:B}}}", 2},
{ L_, "{6:B {4:B {2:R {2:B},{4:B {2:R},.}},{4:B}},"
      "{8:B {6:B .,{6:R}},{8:B .,{8:R}}}}", 3},
{ L_, "{4:B {2:B {2:B},{4:B {2:R},.}},{8:B {6:R {4:B},"
      "{6:B .,{6:R}}},{8:B {8:R},{10:R}}}}", 3},
{ L_, "{6:B {4:B {2:B {2:R},{2:R}},{4:B .,{4:R}}},"
      "{8:B {6:B .,{6:R}},{8:R {8:B},{10:B .,{10:R}}}}}", 3},

// Test random unique combination of 100 nodes
{ L_, "{122:B {58:R {32:B {16:R {6:B {4:B {2:R},.},{12:R {8:B .,{10:R}}"
      ",{14:B}}},{26:B {22:R {18:B .,{20:R}},{24:B}},{30:B {28:R},.}}},"
      "{48:R {42:B {38:R {34:B .,{36:R}},{40:B}},{46:B {44:R},.}},{52:B "
      "{50:B},{56:B {54:R},.}}}},{86:B {74:R {70:B {66:R {62:B "
      "{60:R},{64:R}},{68:B}},{72:B}},{80:B {76:B .,{78:R}},{82:B "
      ".,{84:R}}}},{100:R {96:B {90:R {88:B},{94:B {92:R},.}},"
      "{98:B}},{114:B {108:R {104:B {102:R},{106:R}},{110:B .,{112:R}}},"
      "{118:R {116:B},{120:B}}}}}},{152:R {132:B {128:B {126:B {124:R},.},"
      "{130:B}},{140:B {136:B {134:R},{138:R}},{148:R {144:B {142:R},"
      "{146:R}},{150:B}}}},{178:B {168:R {162:B {156:R {154:B},{158:B "
      ".,{160:R}}},{164:B .,{166:R}}},{174:B {172:B {170:R},.},{176:B}}}"
      ",{192:R {182:B {180:B},{186:R {184:B},{188:B .,{190:R}}}},{198:B "
      "{196:B {194:R},.},{200:B}}}}}}", 4},

// Test random combination of 100 nodes with many duplicates
{L_, "{32:B {16:B {8:R {4:B {2:R {2:B},{4:B {2:R},.}},{6:R {4:B},"
     "{6:B .,{6:R}}}},{12:B {10:R {8:B .,{8:R}},{10:B .,{10:R}}},{14:R "
     "{12:B .,{12:R}},{14:B .,{14:R}}}}},{24:R {20:B {18:R {16:B "
     "{16:R},{18:R}},{18:B}},{20:R {20:B},{22:B {22:R},{22:R}}}},{28:B "
     "{26:R {24:B .,{24:R}},{26:B .,{26:R}}},{30:R {28:B .,{28:R}},{30:B "
     ".,{30:R}}}}}},{52:R {42:B {36:R {34:B {32:B .,{32:R}},{34:B "
     ".,{34:R}}},{38:B {36:B .,{36:R}},{40:R {38:B .,{38:R}},{40:B .,"
     "{40:R}}}}},{48:R {44:B {42:B .,{42:R}},{46:R {44:B .,{44:R}},{46:B "
     ".,{46:R}}}},{48:B {48:B},{50:B {50:R},{50:R}}}}},{60:B {56:B {54:R "
     " {52:B {52:R},{54:R}},{56:B {54:R},.}},{58:R {56:B},{58:B .,{58:R}}}}"
     ",{62:R {60:B {60:B},{62:B}},{66:B {64:R {62:B},{64:B .,{64:R}}},"
     "{66:B {66:R},{68:R}}}}}}}", 4},

// Make the empty tree the last test case, as many tests are not applied to
// empty trees.
{ L_, "." ,                 0 },

};
const int NUM_TREE_VALUES_W_EMPTY = sizeof(TREE_VALUES) / sizeof(*TREE_VALUES);
const int NUM_TREE_VALUES         = NUM_TREE_VALUES_W_EMPTY - 1;


//=============================================================================
//                            GLOBAL TEST-DRIVER
//-----------------------------------------------------------------------------

template <class VALUE>
class TestDriver {
    // This templatized struct provide a namespace for testing 'RbTreeUtil'
    // functions having template parameters.  The parameterized 'VALUE'
    // specifies the type of the nodes held within the tree.  Each
    // "testCase*" method tests a specific aspect of 'RbTreeUtil', and should
    // be invoked with various parameterized types to full test 'RbTreeUtil'.

  private:
    typedef TestTypeValue<VALUE>          ValueConverter;
    typedef TestTreeNode<VALUE>           Node;
    typedef TestTreeNodeAssign<VALUE>     Assigner;
    typedef TestTreeNodeComparator<VALUE> Comparator;
    typedef TestNodeFactory<VALUE>        Factory;
    typedef Array<Node>                   NodeArray;

  private:
    // PRIVATE UTILITIES
    static Node *toNode(RbTreeNode *node) {
        return static_cast<Node *>(node);
    }

    static const Node *toNode(const RbTreeNode *node) {
        return static_cast<const Node *>(node);
    }

  public:
    static void testCase7();
        // Test 'validateRbTree'.

    static void testCase8();
        // Test 'isWellFormed'.

    static void testCase9();
        // Test 'insert'.

    static void testCase10();
        // Test 'find'.

    static void testCase11();
        // Test 'lowerBound'

    static void testCase12();
        // Test 'upperBound'

    static void testCase13();
        // Test 'findInsertLocation'

    static void testCase14();
        // Test 'findInsertLocation w/ hint'

    static void testCase15();
        // Test 'findUniqueInsertLocation'

    static void testCase16();
        // Test 'findUniqueInsertLocation w/ Hint'

    static void testCase17();
        // Test 'insertAt'

    static void testCase18();
        // Test 'remove'

    static void testCase20();
        // Test 'copyTree'

    static void testCase21();
        // Test 'swapTree'


};


template <class VALUE>
void TestDriver<VALUE>::testCase7()
{
    // ------------------------------------------------------------------------
    // CLASS METHODS: RbTreeUtil::validateRbTree
    //
    // Concerns:
    //:  1 That 'validateRbTree' returns 0 for an empty tree.
    //:
    //:  2 That 'validateRbTree' returns a negative value if any node in
    //:    the tree has children whose parent pointer do not refer back to
    //:    that node.
    //:
    //:  3 That 'validateRbTree' returns a negative value if the supplied
    //:    binary tree is not correctly ordered according to the
    //:    comparator.
    //:
    //:  4 That 'validateRbTree' returns a negative value if a RED colored
    //:    node has a red colored child.
    //:
    //:  5 That 'validateRbTree' returns a negative value if the path from
    //:    each leaf node to the root of the tree does not contain the same
    //:    number of BLACK colored nodes (where null children are
    //:    considered BLACK nodes).
    //:
    //:  6 That 'validateRbTree' returns the depth of the tree if none of
    //:    the above criteria are violated.
    //:
    //:  7 That 'validateRbTree' returns the 'true' for valid red-black tree's
    //:    with a red root node.  Note that such a tree is a valid red-black
    //:    tree, by the canonical definition, but does not meet the invariants
    //:    of this component (as expressed by 'isWellFormed').
    //:
    //:  8 That 'validateRbTree' optionally returns a description and node
    //:    related to the error.
    //:
    //:  9 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //:  1 Create an empty tree and verify 'validateRbTree' returns true. (C-1)
    //:
    //:  2 Create a tree for each test spec and verify the 'validateRbTree'
    //:    returns the correct depth. (C-6)
    //:
    //:  3 Create a tree for each test spec, for each node adjust its parent
    //:    pointer to an invalid parent, then verify 'validateRbTree' returns
    //:    a negative value. (C-2)
    //:
    //:  4 Create a tree for each test spec, for each node adjust the next
    //:    nodes value to be less than that node, verify that 'validateRbTree'
    //:    returns a negative value, then adjust the current nodes value to be
    //:    greater than the next node, and verify 'validateRbTree' returns a
    //:    negative value. (C-3)
    //:
    //:  5 Create a tree for each test spec, for each red node adjust its
    //:    parent pointer to be red, then verify 'validateRbTree' returns
    //:    a negative value. (C-4)
    //:
    //:  6 Create a tree for each test spec, for each red node adjust its color
    //:    to be black, then verify 'validateRbTree' returns a negative
    //:    value. (C-5)
    //:
    //:  7 Create a tree for each test spec, for each red node adjust its
    //:    parent to be red, then verify 'validateRbTree' returns an error
    //:    node and message. (C-8)
    //:
    //:  8 Call 'validateRbTree' with 0 addresses and verify assertions are
    //:    invoked. (C-9)
    //
    // Testing:
    //   int validateRbTree(const RbTreeNode *, const NODE_COMPARATOR& );
    //   int validateRbTree(const RbTreeNode **,
    //                      const char **,
    //                      const RbTreeNode *,
    //                      const NODE_COMPARATOR& );
    // ------------------------------------------------------------------------

    const TreeSpec *VALUES     = TREE_VALUES;
    const int       NUM_VALUES = NUM_TREE_VALUES;

    bslma::TestAllocator   ta;
    Comparator C; ValueConverter V; Assigner A;

    const RbTreeNode *en;
    const char       *em;

    // Concern 1 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\tTest an empty tree is valid\n");

    {
        NodeArray nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;

        gg(&x, &nodes, ".", A);
        ASSERT(0 <= Obj::validateRbTree(X.rootNode(), C));
        ASSERT(0 <= Obj::validateRbTree(&en, &em, X.rootNode(), C));

    }

    // Concern 6 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\tTest black depth is correctly returned.\n");

    // Note that concern 6 is tested out of order to ensure that invalid
    // test-input is reported early in the test process.
    for (int i = 0; i < NUM_VALUES; ++i) {
        const int   LINE        = VALUES[i].d_line;
        const char *SPEC        = VALUES[i].d_spec;
        const int   BLACK_DEPTH = VALUES[i].d_blackDepth;

        if (veryVeryVerbose) {
            P_(LINE); P_(BLACK_DEPTH); P(SPEC);
        }

        NodeArray nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;

        gg(&x, &nodes, SPEC, A);

        int depthA = Obj::validateRbTree(X.rootNode(), C);
        int depthB = Obj::validateRbTree(&en, &em, X.rootNode(), C);

        ASSERTV(LINE, BLACK_DEPTH, depthA, BLACK_DEPTH == depthA);
        ASSERTV(LINE, BLACK_DEPTH, depthB, BLACK_DEPTH == depthB);
    }

    // Concern 2 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\tTest invalid parent pointers.\n");

    for (int i = 0; i < NUM_VALUES; ++i) {
        const int   LINE        = VALUES[i].d_line;
        const char *SPEC        = VALUES[i].d_spec;
        const int   BLACK_DEPTH = VALUES[i].d_blackDepth;

        if (veryVeryVerbose) {
            P_(LINE); P_(BLACK_DEPTH); P(SPEC);
        }

        NodeArray nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;

        gg(&x, &nodes, SPEC, A);

        int depthA = Obj::validateRbTree(X.rootNode(), C);
        int depthB = Obj::validateRbTree(&en, &em, X.rootNode(), C);

        ASSERTV(LINE, BLACK_DEPTH, depthA, BLACK_DEPTH == depthA);
        ASSERTV(LINE, BLACK_DEPTH, depthB, BLACK_DEPTH == depthB);


        // Invalidate the parent of all nodes.
        for (int j = 0; j < nodes.size(); ++j) {
            ASSERT(0 <= Obj::validateRbTree(X.rootNode(), C));
            ASSERT(0 <= Obj::validateRbTree(&en, &em, X.rootNode(), C));

            Node *node = &nodes[j];
            if (X.sentinel() == node->parent()) { continue; }

            RbTreeNode *parent = node->parent();

            node->setParent(0);

            ASSERT(0 > Obj::validateRbTree(X.rootNode(), C));
            ASSERT(0 > Obj::validateRbTree(&en, &em, X.rootNode(), C));

            node->setParent(parent);
        }
    }

    // Concern 3 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\tTest invalid binary search trees\n");

    for (int i = 0; i < NUM_VALUES; ++i) {
        const int   LINE        = VALUES[i].d_line;
        const char *SPEC        = VALUES[i].d_spec;
        const int   BLACK_DEPTH = VALUES[i].d_blackDepth;

        if (veryVeryVerbose) {
            P_(LINE); P_(BLACK_DEPTH); P(SPEC);
        }

        NodeArray nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;

        gg(&x, &nodes, SPEC, A);

        // Set each node to a value less-than then greater-than the next node
        for (int j = 0; j < nodes.size(); ++j) {
            ASSERT(0 <= Obj::validateRbTree(X.rootNode(), C));
            ASSERT(0 <= Obj::validateRbTree(&en, &em, X.rootNode(), C));

            RbTreeNode *nextRbTreeNode = Obj::next(&nodes[j]);
            if (X.sentinel() == nextRbTreeNode) { continue; }

            Node *node     = &nodes[j];
            Node *nextNode = static_cast<Node *>(nextRbTreeNode);

            int nextNodeValue = V.getValue(nextNode->value());
            int nodeValue     = V.getValue(node->value());

            nextNode->value() = V.create(nodeValue - 1);

            ASSERT(0 > Obj::validateRbTree(X.rootNode(), C));
            ASSERT(0 > Obj::validateRbTree(&en, &em, X.rootNode(), C));

            nextNode->value() = V.create(nextNodeValue);
            node->value()     = V.create(nextNodeValue + 1);

            ASSERT(0 > Obj::validateRbTree(X.rootNode(), C));
            ASSERT(0 > Obj::validateRbTree(&en, &em, X.rootNode(), C));

            node->value()     = V.create(nodeValue);
            ASSERT(0 <= Obj::validateRbTree(X.rootNode(), C));
            ASSERT(0 <= Obj::validateRbTree(&en, &em, X.rootNode(), C));
        }
    }

    // Concern 4 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\tTest red nodes cannot have red children.\n");

    for (int i = 0; i < NUM_VALUES; ++i) {
        const int   LINE        = VALUES[i].d_line;
        const char *SPEC        = VALUES[i].d_spec;
        const int   BLACK_DEPTH = VALUES[i].d_blackDepth;

        if (veryVeryVerbose) {
            P_(LINE); P_(BLACK_DEPTH); P(SPEC);
        }

        NodeArray nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;

        gg(&x, &nodes, SPEC, A);

        // Set the parent of every red node to be not red
        for (int j = 0; j < nodes.size(); ++j) {
            ASSERT(0 <= Obj::validateRbTree(X.rootNode(), C));
            ASSERT(0 <= Obj::validateRbTree(&en, &em, X.rootNode(), C));

            Node *node = &nodes[j];
            if (X.sentinel() == node->parent() || node->isBlack()) {
                continue;
            }

            ASSERT(node->parent()->isBlack());

            node->parent()->toggleColor();

            ASSERT(0 > Obj::validateRbTree(X.rootNode(), C));
            ASSERT(0 > Obj::validateRbTree(&en, &em, X.rootNode(), C));

            node->parent()->toggleColor();
        }
    }

    // Concern 5 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\tTest that the black depth of all leaves is "
                            "the same.\n");

    for (int i = 0; i < NUM_VALUES; ++i) {
        const int   LINE        = VALUES[i].d_line;
        const char *SPEC        = VALUES[i].d_spec;
        const int   BLACK_DEPTH = VALUES[i].d_blackDepth;

        if (veryVeryVerbose) {
            P_(LINE); P_(BLACK_DEPTH); P(SPEC);
        }

        NodeArray nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;

        gg(&x, &nodes, SPEC, A);

        // Set the parent of every red node to be not red
        for (int j = 0; j < nodes.size(); ++j) {
            ASSERT(0 <= Obj::validateRbTree(X.rootNode(), C));
            ASSERT(0 <= Obj::validateRbTree(&en, &em, X.rootNode(), C));

            Node *node = &nodes[j];
            if (X.sentinel() == node->parent() || node->isBlack()) {
                continue;
            }

            ASSERT(node->isRed());

            node->toggleColor();

            ASSERT(0 > Obj::validateRbTree(X.rootNode(), C));
            ASSERT(0 > Obj::validateRbTree(&en, &em, X.rootNode(), C));

            node->toggleColor();
        }
    }

    // Concern 7 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose)
        printf("\tTest valid red-black trees w/ a red root node.\n");
    {
        struct TreeSpec {
            int         d_line;
            const char *d_spec;
            int         d_blackDepth;
        } VALUES[] = {
            { L_, "{5:R}" ,                                      0 },
            { L_, "{5:R {3:B},{7:B}}",                           1 },
            { L_, "{5:R {3:B {1:R},{4:R}},{7:B {6:R},{9:R}}}",   1 },
            { L_, "{5:R {3:B {1:B},{4:B}},{7:B {6:B},{9:B}}}",   2 },
        };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            const int   LINE        = VALUES[i].d_line;
            const char *SPEC        = VALUES[i].d_spec;
            const int   BLACK_DEPTH = VALUES[i].d_blackDepth;
            if (veryVeryVerbose) {
                P_(LINE); P_(BLACK_DEPTH); P(SPEC);
            }

            NodeArray nodes(&ta);
            RbTreeAnchor x; const RbTreeAnchor& X = x;

            gg(&x, &nodes, SPEC, A);

            ASSERT(BLACK_DEPTH == Obj::validateRbTree(X.rootNode(), C));
        }
    }
    // Concern 8 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\tTest error node and description.\n");

    for (int i = 0; i < NUM_VALUES; ++i) {
        const int   LINE        = VALUES[i].d_line;
        const char *SPEC        = VALUES[i].d_spec;
        const int   BLACK_DEPTH = VALUES[i].d_blackDepth;

        if (veryVeryVerbose) {
            P_(LINE); P_(BLACK_DEPTH); P(SPEC);
        }

        NodeArray nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;

        const RbTreeNode *en = 0;
        const char       *em = 0;

        gg(&x, &nodes, SPEC, A);

        // Set the parent of every red node to be not red
        for (int j = 0; j < nodes.size(); ++j) {
            ASSERT(0 <= Obj::validateRbTree(X.rootNode(), C));
            ASSERT(0 <= Obj::validateRbTree(&en, &em, X.rootNode(), C));

            Node *node = &nodes[j];
            if (X.sentinel() == node->parent() || node->isBlack()) {
                continue;
            }
            ASSERT(node->parent()->isBlack());

            node->parent()->toggleColor();

            ASSERT(0 == en);
            ASSERT(0 == em);
            ASSERT(0 > Obj::validateRbTree(&en, &em, X.rootNode(), C));
            ASSERT(0 != en);
            ASSERT(0 != em);

            node->parent()->toggleColor();
            en = 0;
            em = 0;
        }
    }

    // Concern 9 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    if (veryVerbose) printf("\tNegative testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        ASSERT_PASS(Obj::validateRbTree(&en, &em, 0, C));
        ASSERT_FAIL(Obj::validateRbTree(&en,   0, 0, C));
        ASSERT_FAIL(Obj::validateRbTree(0,   &em, 0, C));
        ASSERT_FAIL(Obj::validateRbTree(0,     0, 0, C));
    }
    if (veryVerbose) printf("---------------------\n");
}

template <class VALUE>
void TestDriver<VALUE>::testCase8()
{
    // ------------------------------------------------------------------------
    // CLASS METHODS: RbTreeUtil::isWellFormed()
    //
    // Concerns:
    //:  1 That 'isWellFormed' returns 'false' for an empty tree if the first
    //:    node of the tree is not the sentinel node.
    //:
    //:  2 That 'isWellFormed' returns 'false' for an empty tree if the node
    //:    count is not 0.
    //:
    //:  3 That 'isWellFormed' returns 'false' for an empty tree if the
    //:    sentinel node's left child is not 0.
    //:
    //:  4 That 'isWellFormed' returns 'true' for an empty tree if concerns
    //:    1-3 are not violated.
    //:
    //:  5 That 'isWellFormed' returns 'false' if the supplied tree is not a
    //:    valid red-black tree.  Note that we have already thoroughly tested
    //:    'validateRbTree', to which this method delegates.
    //:
    //:  6 That 'isWellFormed' returns 'false' if the first node of the tree
    //:    does not refer to the left most node of the root.
    //:
    //:  7 That 'isWellFormed' returns 'false' if the node count is not the
    //:    number of nodes in the tree.
    //:
    //:  8 That 'isWellFormed' returns 'false' if the sentinel nodes left
    //:    child is not the root node, and the root node's left child is not
    //:    the sentinel node.
    //:
    //:  9 That 'isWellFormed' returns 'true' if none of the above criteria
    //:    are violated.
    //
    // Plan:
    //:  1 Create an empty tree, modify its first node, node-count, and
    //:    sentinel node and verify 'isWellFormed' is 'false'.  (C-1, C-2, C-3,
    //:    C-4).
    //:
    //:  2 For a set of valid test tree's, adjust the parent of each red node
    //:    to be red and verify 'isWellFormed' is 'false'.  Note that this is
    //:    a white-box test as 'validateRbTree' has already been tested. (C-5)
    //:
    //:  3 For a set of valid test tree's, adjust the first node of the tree
    //:    and verify 'isWellFormed' is 'false'.  (C-6)
    //:
    //:  4 For a set of valid test tree's, adjust the node count of the tree
    //:    and verify 'isWellFormed' is 'false'.  (C-7)
    //:
    //:  5 For a set of valid test tree's, adjust the sentinel node's left
    //:    child and then the root node's parent, and and verify
    //:    'isWellFormed' is 'false'.  (C-8, C-9).
    //
    // Testing:
    //   bool isWellFormed(const RbTreeAnchor&, const NODE_COMPARATOR&);
    // ------------------------------------------------------------------------

    const TreeSpec *VALUES     = TREE_VALUES;
    const int       NUM_VALUES = NUM_TREE_VALUES;

    bslma::TestAllocator   ta;
    Comparator C; Assigner A;

    if (veryVerbose) printf("\tTest 'isWellFormed' on an empty node.\n");
    {
        RbTreeAnchor x; const RbTreeAnchor& X = x;
        Node node;

        node.reset(0, 0, 0, BLACK);

        ASSERT(Obj::isWellFormed(X, C));

        // Concern 1 - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        //   1 That 'isWellFormed' returns 'false' for an empty tree if the
        //   first node of the tree is not the sentinel node.

        x.setFirstNode(0);
        ASSERT(!Obj::isWellFormed(X, C));
        x.setFirstNode(&node);
        ASSERT(!Obj::isWellFormed(X, C));
        x.setFirstNode(x.sentinel());
        ASSERT(Obj::isWellFormed(X, C));

        // Concern 2 - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        //   2 That 'isWellFormed' returns 'false' for an empty tree if the
        //      node count is not 0.

        for (int i = 0; i < 10; ++i) {
            x.setNumNodes(i);
            ASSERT((0 == i) == Obj::isWellFormed(X, C));
        }
        x.setNumNodes(0);
        ASSERT(Obj::isWellFormed(X, C));

        // Concern 3 - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        //   3 That 'isWellFormed' returns 'false' for an empty tree if the
        //     sentinel node's left child is not 0.

        x.sentinel()->setLeftChild(&node);
        ASSERT(!Obj::isWellFormed(X, C));
        x.sentinel()->setLeftChild(0);

        // Concern 4 - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        //   4 That 'isWellFormed' returns 'true' for an empty tree if concerns
        //     1-3 are not violated.

        ASSERT(Obj::isWellFormed(X, C));
    }

    // Concern 5 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //   5 That 'isWellFormed' returns 'false' if the supplied tree is not a
    //     valid red-black tree.  Note that we have already thoroughly tested
    //     'validateRbTree', to which this method delegates.

    // Note that this is a white-box test as 'validateRbTree' has already
    // been tested extensively.  This test sets the parents of red-nodes
    // to red.

    if (veryVerbose) printf("\tTest invalid rbtrees.\n");

    for (int i = 0; i < NUM_VALUES; ++i) {
        const int   LINE        = VALUES[i].d_line;
        const char *SPEC        = VALUES[i].d_spec;
        const int   BLACK_DEPTH = VALUES[i].d_blackDepth;

        if (veryVeryVerbose) {
            P_(LINE); P_(BLACK_DEPTH); P(SPEC);
        }

        NodeArray nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;

        gg(&x, &nodes, SPEC, A);

        for (int j = 0; j < nodes.size(); ++j) {
            ASSERT(true == Obj::isWellFormed(X, C));

            Node *node = &nodes[j];

            if (X.sentinel() == node->parent() || node->isBlack()) {
                continue;
            }

            ASSERT(node->parent()->isBlack());
            node->parent()->toggleColor();
            ASSERT(false == Obj::isWellFormed(X, C));
            node->parent()->toggleColor();
            ASSERT(true  == Obj::isWellFormed(X, C));
        }
    }

    // Concern 6 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //   6 That 'isWellFormed' returns 'false' if the first node of the tree
    //     does not refer to the left most node of the root.

    if (veryVerbose) printf("\tTest first node is not the leftmost node.\n");

    for (int i = 0; i < NUM_VALUES; ++i) {
        const int   LINE        = VALUES[i].d_line;
        const char *SPEC        = VALUES[i].d_spec;
        const int   BLACK_DEPTH = VALUES[i].d_blackDepth;

        if (veryVeryVerbose) {
            P_(LINE); P_(BLACK_DEPTH); P(SPEC);
        }

        NodeArray nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;

        gg(&x, &nodes, SPEC, A);

        RbTreeNode *leftmostNode = Obj::leftmost(x.rootNode());

        ASSERT(leftmostNode == X.firstNode());

        for (int j = 0; j < X.numNodes(); ++j) {
            Node *node = &nodes[j];

            if (node == leftmostNode) {
                continue;
            }

            ASSERT(true  == Obj::isWellFormed(X, C));
            x.setFirstNode(node);
            ASSERT(false == Obj::isWellFormed(X, C));
            x.setFirstNode(leftmostNode);
        }
        x.setFirstNode(0);
        ASSERT(false == Obj::isWellFormed(X, C));
    }

    // Concern 7 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //   7 That 'isWellFormed' returns 'false' if the node count is not the
    //     number of nodes in the tree.

    if (veryVerbose) printf(
                             "\tTest that numNodes is not a correct count.\n");

    for (int i = 0; i < NUM_VALUES; ++i) {
        const int   LINE        = VALUES[i].d_line;
        const char *SPEC        = VALUES[i].d_spec;
        const int   BLACK_DEPTH = VALUES[i].d_blackDepth;

        if (veryVeryVerbose) {
            P_(LINE); P_(BLACK_DEPTH); P(SPEC);
        }

        NodeArray nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;

        gg(&x, &nodes, SPEC, A);

        const int NUM_NODES = X.numNodes();
        for (int j = 0; j <= 100; j+=10) {
            if (j == NUM_NODES) {
                continue;
            }

            ASSERT(true  == Obj::isWellFormed(X, C));
            x.setNumNodes(j);
            ASSERT(false == Obj::isWellFormed(X, C));
            x.setNumNodes(NUM_NODES);
        }
    }

    // Concern 8 - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    //   8 That 'isWellFormed' returns 'false' if the sentinel nodes left
    //     child is not the root node, and the root node's left child is not
    //     the sentinel node.

    if (veryVerbose) printf("\tTest the sentinel node's left child .\n");

    for (int i = 0; i < NUM_VALUES; ++i) {
        const int   LINE        = VALUES[i].d_line;
        const char *SPEC        = VALUES[i].d_spec;
        const int   BLACK_DEPTH = VALUES[i].d_blackDepth;

        if (veryVeryVerbose) {
            P_(LINE); P_(BLACK_DEPTH); P(SPEC);
        }

        NodeArray nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;

        gg(&x, &nodes, SPEC, A);

        RbTreeNode *ROOT_NODE = x.rootNode();

        for (int j = 0; j < X.numNodes(); ++j) {
            Node *node = &nodes[j];

            if (node == ROOT_NODE) {
                continue;
            }

            ASSERT(true  == Obj::isWellFormed(X, C));
            x.sentinel()->setLeftChild(node);
            ASSERT(node == X.rootNode());
            ASSERT(false  == Obj::isWellFormed(X, C));
            x.sentinel()->setLeftChild(ROOT_NODE);
            ASSERT(true  == Obj::isWellFormed(X, C));

            x.rootNode()->setParent(node);
            ASSERT(false  == Obj::isWellFormed(X, C));
            x.rootNode()->setParent(x.sentinel());
            ASSERT(true  == Obj::isWellFormed(X, C));
        }
    }
    if (veryVerbose) printf("---------------------\n");
}

template <class VALUE>
void TestDriver<VALUE>::testCase9()
{
    // ------------------------------------------------------------------------
    // CLASS METHODS: RbTreeUtil::insert
    //
    // Concerns:
    //:  1 That inserting a node in an empty tree returns a well-formed
    //:    red-black tree.
    //:
    //:  2 That inserting any value in a valid red-black tree results in a
    //:    valid red-black tree containing the original set of nodes plus the
    //:    additional node.
    //:
    //:  3 Inserting a value into a large red-black tree results in a valid
    //:    red-black tree containing the values in the original tree and the
    //:    new value.
    //:
    //:  4 Inserting a duplicate value into a tree results in a valid
    //:    red-black tree containing the values in the original tree and the
    //:    new duplicate value.
    //:
    //:  5 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //:  1 Insert a node into an empty tree and verify the properties of the
    //:    resulting tree.  (C-1)
    //:
    //:  2 For the series of pre-defined test trees, insert a node and verify
    //:    that its value appears in the tree. (C-2)
    //:
    //:  3 Insert every possible permutation of 6 nodes, ensure the tree is
    //:    well-formed at all times, and contains the expected nodes. (C-2)
    //:
    //:  4 Insert random shuffles of 200 nodes, ensure the tree is well
    //:    formed at all times, and contains the expected nodes when
    //:    finished. (C-2)
    //:
    //:  5 Insert a random shuffle of 50000 nodes, ensure that when finished
    //:    the tree is well formed and contains the expected nodes. (C-3)
    //:
    //:  6 Insert random shuffles of 100 nodes with many duplicates, and
    //:    ensure the tree is well formed at all times, and contains the
    //:    expected nodes when finished. (C-4)
    //:
    //:  8 Call 'insert' with 0 for the tree-anchor and node respectively,
    //:    verify that assertion is invoked.  Call insert with a not-well
    //:    formed binary tree and verify an assertion is invoked. (C-5)
    //
    // Testing:
    //   void insert(RbTreeAnchor *, const NODE_COMPARATOR& , RbTreeNode *);
    // ------------------------------------------------------------------------
    bslma::TestAllocator   ta;
    Comparator C; ValueConverter V; Assigner A;

    if (veryVerbose) printf("\tTest inserting into an empty tree\n");

    for (int i = 0; i < 10; ++i) {
        RbTreeAnchor x; const RbTreeAnchor& X = x;
        ASSERT(Obj::isWellFormed(X, C));

        Node node; const Node& NODE = node;
        A(&node, i);

        Obj::insert(&x, C, &node);

        ASSERT(Obj::isWellFormed(x, C));
        ASSERT(&NODE        == X.rootNode());
        ASSERT(&NODE        == X.firstNode());
        ASSERT(1            == X.numNodes());
        ASSERT(X.sentinel() == NODE.parent());
        ASSERT(&NODE        == X.sentinel()->leftChild());
    }

    if (veryVerbose) printf("\tinsert a node into a series of test trees\n");
    {
        const TreeSpec *VALUES     = TREE_VALUES;
        const int       NUM_VALUES = NUM_TREE_VALUES_W_EMPTY;

        // Iterate over the set of test trees 'TREE_VALUES'
        for (int i = 0; i < NUM_VALUES; ++i) {
            for (int j = 0; j < 5; j+=5) {
                const char *SPEC = VALUES[i].d_spec;
                NodeArray  nodes(&ta);
                RbTreeAnchor x; const RbTreeAnchor& X = x;

                gg(&x, &nodes, SPEC, A);

                Node node;
                node.value() = V.create(rand() % 100);
                ASSERT(Obj::isWellFormed(X, C));
                Obj::insert(&x, C, &node);
                ASSERT(Obj::isWellFormed(X, C));
                ASSERT(nodes.size() + 1 == X.numNodes());
            }
        }
    }

    if (veryVerbose) printf(
                            "\tinsert every possible combination of [0..6]\n");
    {
        int       VALUES[]   = { 0, 1, 2, 3, 4, 5, 6 };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);
        do {
            bslma::TestAllocator   ta;
            NodeArray             nodes(&ta);

            RbTreeAnchor x; const RbTreeAnchor& X = x;
            nodes.reset(NUM_VALUES);
            for (int i = 0; i < NUM_VALUES; ++i) {
                nodes[i].setLeftChild((RbTreeNode *)0xdeadbeef);
                nodes[i].setRightChild((RbTreeNode *)0xdeadbeef);
                nodes[i].setParent((RbTreeNode *)0xdeadc0de);
                nodes[i].value()  = V.create(VALUES[i]);

                ASSERT(Obj::isWellFormed(X, C));
                Obj::insert(&x, C, &nodes[i]);
                ASSERT(Obj::isWellFormed(X, C));
                ASSERT(i + 1 == X.numNodes());
            }
            int counter = 0;
            for (const RbTreeNode *n =  X.firstNode();
                                   n != X.sentinel();
                                   n =  Obj::next(n)) {
                const Node& node = static_cast<const Node&>(*n);
                ASSERT(counter == V.getValue(node.value()));
                ++counter;
            }
        } while (std::next_permutation(VALUES,
                                       VALUES + NUM_VALUES,
                                       intLess));
    }

    if (veryVerbose) printf("\tinsert two possible shuffles of [0..200]\n");
    {
        enum {
            NUM_VALUES = 200
        };
        int VALUES[NUM_VALUES];
        for (int i = 0; i < NUM_VALUES; ++i) {
            VALUES[i] = i;
        }

        for (int i = 0; i < 2; ++i) {
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
            std::shuffle(VALUES, VALUES + NUM_VALUES, g_randomSource);
#else  // fall-back for C++03, potentially unsupported in C++17
            std::random_shuffle(VALUES, VALUES + NUM_VALUES);
#endif


            NodeArray             nodes(&ta);

            RbTreeAnchor x; const RbTreeAnchor& X = x;
            nodes.reset(NUM_VALUES);

            for (int j = 0; j < NUM_VALUES; ++j) {
                nodes[j].value()  = V.create(VALUES[j]);

                ASSERT(Obj::isWellFormed(X, C));
                Obj::insert(&x, C, &nodes[j]);
                ASSERT(Obj::isWellFormed(X, C));
                ASSERT(j + 1 == X.numNodes());
            }
            int counter = 0;
            for (const RbTreeNode *n =  X.firstNode();
                                   n != X.sentinel();
                                   n =  Obj::next(n)) {
                const Node& node = static_cast<const Node&>(*n);
                ASSERT(counter == V.getValue(node.value()));
                ++counter;
            }
        }
    }

    if (veryVerbose) printf("\tinsert 50000 nodes\n");
    {
        enum {
            NUM_VALUES = 50000
        };
        int VALUES[NUM_VALUES];
        for (int i = 0; i < NUM_VALUES; ++i) {
            VALUES[i] = i;
        }

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
        std::shuffle(VALUES, VALUES + NUM_VALUES, g_randomSource);
#else  // fall-back for C++03, potentially unsupported in C++17
        std::random_shuffle(VALUES, VALUES + NUM_VALUES);
#endif

        bslma::TestAllocator   ta;
        NodeArray             nodes(&ta);
        Comparator C; ValueConverter V;

        RbTreeAnchor x; const RbTreeAnchor& X = x;
        nodes.reset(NUM_VALUES);

        for (int j = 0; j < NUM_VALUES; ++j) {
            nodes[j].value() = V.create(VALUES[j]);
            Obj::insert(&x, C, &nodes[j]);
            ASSERT(j + 1 == X.numNodes());
        }
        ASSERT(Obj::isWellFormed(X, C));
        int counter = 0;
        for (const RbTreeNode *n =  X.firstNode();
                               n != X.sentinel();
                               n =  Obj::next(n)) {
            const Node& node = static_cast<const Node&>(*n);
            ASSERT(counter == V.getValue(node.value()));
            ++counter;
        }
    }

    if (veryVerbose)
        printf("\tinsert 10 shuffles of 100 elements with duplicates\n");

    {
        enum {
            NUM_VALUES = 100
        };
        int VALUES[NUM_VALUES];
        for (int i = 0; i < NUM_VALUES; ++i) {
            VALUES[i] = i % 10;
        }

        for (int i = 0; i < 10; ++i) {
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
            std::shuffle(VALUES, VALUES + NUM_VALUES, g_randomSource);
#else  // fall-back for C++03, potentially unsupported in C++17
            std::random_shuffle(VALUES, VALUES + NUM_VALUES);
#endif

            NodeArray             nodes(&ta);

            RbTreeAnchor x; const RbTreeAnchor& X = x;
            nodes.reset(NUM_VALUES);
            for (int j = 0; j < NUM_VALUES; ++j) {
                nodes[j].value()  = V.create(VALUES[j]);

                ASSERT(Obj::isWellFormed(X, C));
                Obj::insert(&x, C, &nodes[j]);
                ASSERT(Obj::isWellFormed(X, C));
                ASSERT(j + 1 == X.numNodes());
            }
            int counter = 0;
            for (const RbTreeNode *n =  X.firstNode();
                                   n != X.sentinel();
                                   n =  Obj::next(n)) {
                const Node& node = static_cast<const Node&>(*n);
                ASSERT(counter / 10 == V.getValue(node.value()));
                ++counter;
            }
        }
    }

    if (veryVerbose) printf("\tNegative testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        RbTreeAnchor x; const RbTreeAnchor& X = x;
        ASSERT(Obj::isWellFormed(X, C));

        Node node;
        A(&node, 1);

        ASSERT_SAFE_FAIL(Obj::insert(&x, C, 0));
        ASSERT_SAFE_FAIL(Obj::insert(0, C, &node));
        ASSERT_PASS(Obj::insert(&x, C, &node));
    }
    if (veryVerbose) printf("---------------------\n");
}

template <class VALUE>
void TestDriver<VALUE>::testCase10()
{
    // ------------------------------------------------------------------------
    // CLASS METHODS: RbTreeUtil::find
    //
    // Concerns:
    //:  1 That finding a node value in an empty tree returns the tree's
    //:    sentinel node.
    //:
    //:  2 That, for all well-formed trees, finding a value that is in the
    //:    tree returns the address of the node containing that value.
    //:
    //:  2 That, for all well-formed trees, finding a value that is not in the
    //:    returns the address of tree's sentinel node.
    //:
    //:  4 That, for all well-formed trees containing duplicate values, finding
    //:    a value that occurs multiple times returns the leftmost value.
    //:
    //:  5 That 'find' behaves correctly on both 'const' and non-'const' trees
    //:
    //:  6 That 'find' behaves correctly on very large trees.
    //
    // Plan:
    //:  1 Create an empty tree and verify that 'find' returns the sentinel
    //:    node (C-1).
    //:
    //:  2 For a series of pre-defined test trees, test all the values from
    //:    the 0 value through the maximum value in the tree plus 1, verify the
    //:    result of 'find' matches the result of linearly searching the
    //:    tree. (C-2, C-3, C-4)
    //:
    //:  3 For a series of random trees containing duplicates, test all values
    //:    from 0 through a maximum possible value plus 1, verify the
    //:    result of 'find' matches the result of linearly searching the tree.
    //:
    //:  4 For a very large tree of random values, test all possible values in
    //:    the tree and verify the results of find against a linear search.
    //
    // Testing:
    //   const RbTreeNode *find(const RbTreeAnchor&
    //                          const NODE_VALUE_COMPARATOR& ,
    //                          const VALUE&                 );
    //   RbTreeNode       *find(RbTreeAnchor&                ,
    //                           const NODE_VALUE_COMPARATOR& ,
    //                           const VALUE&                 );
    // ------------------------------------------------------------------------

    bslma::TestAllocator   ta;
    Comparator C; ValueConverter V; Assigner A;

    if (veryVerbose) printf("\tTest finding nodes in an empty tree\n");
    {
        const int VALUES[]   = { 0, 1, 2, 4, 5, 6 };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        RbTreeAnchor x; const RbTreeAnchor& X = x;
        for (int i = 0; i < NUM_VALUES; ++i) {
            ASSERT(X.sentinel() == Obj::find(X, C, V.create(VALUES[i])));
            ASSERT(X.sentinel() == Obj::find(x, C, V.create(VALUES[i])));
        }
    }

    if (veryVerbose) printf(
                            "\tTest finding nodes in the set of test trees\n");
    {
        const TreeSpec *VALUES     = TREE_VALUES;
        const int       NUM_VALUES = NUM_TREE_VALUES;

        // Iterate over the set of test trees 'TREE_VALUES'
        for (int i = 0; i < NUM_VALUES; ++i) {
            const char *SPEC = VALUES[i].d_spec;
            const int   LINE = VALUES[i].d_line;

            NodeArray  nodes(&ta);
            RbTreeAnchor x; const RbTreeAnchor& X = x;

            gg(&x, &nodes, SPEC, A);

            const RbTreeNode *maxNode = Obj::rightmost(X.rootNode());
            int max = V.getValue(toNode(maxNode)->value());

            for (int j = 0; j <= max + 1; ++j) {
                bool found = false;
                const RbTreeNode *node = X.firstNode();
                for (; node != X.sentinel(); node = Obj::next(node)) {
                    if (j == V.getValue(toNode(node)->value())) {
                        found = true;
                        break;
                    }
                }
                RbTreeNode       *result = Obj::find(x, C, V.create(j));
                const RbTreeNode *RESULT = Obj::find(X, C, V.create(j));

                ASSERTV(LINE, result == RESULT);
                ASSERTV(LINE, found  == (X.sentinel() != RESULT));
                if (X.sentinel() != RESULT) {
                    ASSERTV(LINE, j, j == V.getValue(toNode(RESULT)->value()));
                }

            }
        }
    }

    if (veryVerbose)
        printf("\tTest find on sets of 1000 random values w/ duplicates\n");
    for (int i = 0; i < 10; ++i) {
        enum {
            NUM_VALUES  = 1000,
            VALUE_LIMIT = 500
        };
        int VALUES[NUM_VALUES];
        for (int i = 0; i < NUM_VALUES; ++i) {
            VALUES[i] = rand() % VALUE_LIMIT;
        }
        NodeArray             nodes(&ta);

        RbTreeAnchor x; const RbTreeAnchor& X = x;
        nodes.reset(NUM_VALUES);
        for (int j = 0; j < NUM_VALUES; ++j) {
            nodes[j].value()  = V.create(VALUES[j]);
            Obj::insert(&x, C, &nodes[j]);
        }
        ASSERT(Obj::isWellFormed(X, C));
        ASSERT(NUM_VALUES == X.numNodes());

        for (int j = 0; j < VALUE_LIMIT+2; ++j) {
            bool found = false;
            const RbTreeNode *node = X.firstNode();
            for (; node != X.sentinel(); node = Obj::next(node)) {
                if (j == V.getValue(toNode(node)->value())) {
                    found = true;
                    break;
                }
            }
            RbTreeNode       *result = Obj::find(x, C, V.create(j));
            const RbTreeNode *RESULT = Obj::find(X, C, V.create(j));

            ASSERTV(result == RESULT);
            ASSERTV(found  == (X.sentinel() != RESULT));
            if (X.sentinel() != RESULT) {
                ASSERTV(j == V.getValue(toNode(RESULT)->value()));
            }
        }
    }


    if (veryVerbose)
        printf("\tTest find on a 50000 node tree \n");
    {
        enum {
            NUM_VALUES = 50000
        };
        int VALUES[NUM_VALUES];
        for (int i = 0; i < NUM_VALUES; ++i) {
            VALUES[i] = rand() % NUM_VALUES;
        }
        NodeArray             nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;
        nodes.reset(NUM_VALUES);

        for (int j = 0; j < NUM_VALUES; ++j) {
            nodes[j].value()  = V.create(VALUES[j]);
            Obj::insert(&x, C, &nodes[j]);
        }
        ASSERT(Obj::isWellFormed(X, C));
        ASSERT(NUM_VALUES == X.numNodes());

        for (int j = 0; j < NUM_VALUES; ++j) {
            RbTreeNode       *result = Obj::find(x, C, V.create(VALUES[j]));
            const RbTreeNode *RESULT = Obj::find(X, C, V.create(VALUES[j]));
            ASSERT(result       == RESULT);
            ASSERT(X.sentinel() != RESULT);
            ASSERT(VALUES[j]    == V.getValue(toNode(RESULT)->value()));
        }
        ASSERT(X.sentinel() == Obj::find(x, C, V.create(NUM_VALUES+1)));
        ASSERT(X.sentinel() == Obj::find(X, C, V.create(NUM_VALUES+1)));
    }
    if (veryVerbose) printf("---------------------\n");
}

template <class VALUE>
void TestDriver<VALUE>::testCase11()
{
    // ------------------------------------------------------------------------
    // CLASS METHODS: RbTreeUtil::lowerBound
    //
    // Concerns:
    //:  1 That lowerBound returns sentinel node for an empty tree.
    //:
    //:  2 That, for all well-formed trees, when searching for a value that
    //:    is in the tree, return the leftmost matching value.
    //:
    //:  3 That, for all well-formed trees, when searching for a value not in
    //:    the tree, return the first value greater than the value being
    //:    searched for.
    //:
    //:  4 That, for all well-formed trees, when searching for a value greater
    //:    than any node in the tree, lowerBound returns the sentinel node.
    //:
    //:  5 That, for all well-formed trees, lowerBound returns the first node
    //:    less than or equal to the value being searched for.
    //:
    //:  6 That 'lowerBound' behaves correctly on both 'const' and non-'const'
    //:    trees
    //:
    //:  7 That 'lowerBound' behaves correctly on very large trees.
    //
    // Plan:
    //:  1 Create an empty tree and verify that 'lowerBound' returns the
    //:    sentinel node (C-1).
    //:
    //:  2 For a series of pre-defined test trees, test all the values from
    //:    the 0 value through the maximum value in the tree plus 1, verify the
    //:    result of 'lowerBound' is the first node whose value is greater
    //:    than or equal to the value being searched for. (C-2, C-3, C-4, C-5,
    //:    C-6)
    //:
    //:  3 For a series of random trees containing duplicates, test all values
    //:    from 0 through a maximum possible value plus 1, verify the
    //:    result of 'lowerBound' is the first node whose value is greater
    //:    than or equal to the value being searched for. (C-2, C-3, C-4, C-5,
    //:    C-6)
    //:
    //:  4 For a very large tree of random values, test all possible values in
    //:    the tree and verify that 'lowerBound' returns the first node whose
    //:    value is greater than or equal to the value being searched for.
    //:    (C-7)
    //
    // Testing:
    //   const RbTreeNode *lowerBound(const RbTreeAnchor&          ,
    //                                const NODE_VALUE_COMPARATOR& ,
    //                                const VALUE&                 );
    //   RbTreeNode       *lowerBound(RbTreeAnchor&                ,
    //                                const NODE_VALUE_COMPARATOR& ,
    //                                const VALUE&                 );
    // ------------------------------------------------------------------------

    bslma::TestAllocator   ta;
    Comparator C; ValueConverter V;  Assigner A;

    if (veryVerbose) printf("\tTest lowerBound on an empty tree\n");
    {
        const int VALUES[]   = { 0, 1, 2, 4, 5, 6 };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        RbTreeAnchor x; const RbTreeAnchor& X = x;
        for (int i = 0; i < NUM_VALUES; ++i) {
            ASSERT(X.sentinel() == Obj::lowerBound(X, C, V.create(VALUES[i])));
            ASSERT(X.sentinel() == Obj::lowerBound(x, C, V.create(VALUES[i])));
        }
    }

    if (veryVerbose) printf("\tTest lowerBound on a set of test trees\n");
    {
        const TreeSpec *VALUES     = TREE_VALUES;
        const int       NUM_VALUES = NUM_TREE_VALUES;

        // Iterate over the set of test trees 'TREE_VALUES'
        for (int i = 0; i < NUM_VALUES; ++i) {
            const char *SPEC = VALUES[i].d_spec;
            const int   LINE = VALUES[i].d_line;

            NodeArray  nodes(&ta);
            RbTreeAnchor x; const RbTreeAnchor& X = x;

            gg(&x, &nodes, SPEC, A);

            const RbTreeNode *maxNode = Obj::rightmost(X.rootNode());
            int max = V.getValue(toNode(maxNode)->value());

            for (int j = 0; j <= max + 1; ++j) {
                RbTreeNode       *result = Obj::lowerBound(x, C, V.create(j));
                const RbTreeNode *RESULT = Obj::lowerBound(X, C, V.create(j));

                ASSERTV(LINE, result == RESULT);
                ASSERTV(LINE, 0      != RESULT);

                if (j > max) {
                    ASSERTV(LINE, X.sentinel() == RESULT);
                    continue;
                }
                ASSERTV(LINE, j, X.sentinel() != RESULT);
                int nodeValue     = V.getValue(toNode(RESULT)->value());
                int prevNodeValue = INT_MIN;
                if (RESULT != X.firstNode()) {
                    const RbTreeNode *prevNode = Obj::previous(RESULT);
                    prevNodeValue = V.getValue(toNode(prevNode)->value());
                }

                ASSERTV(LINE, j, prevNodeValue, prevNodeValue < j);
                ASSERTV(LINE, j, nodeValue,     j <= nodeValue);
            }
        }
    }

    if (veryVerbose)
        printf("\tTest find on sets of 1000 random values w/ duplicates\n");
    for (int i = 0; i < 15; ++i) {
        enum {
            NUM_VALUES  = 1000,
            VALUE_LIMIT = 500
        };

        int VALUES[NUM_VALUES];
        for (int i = 0; i < NUM_VALUES; ++i) {
            VALUES[i] = rand() % VALUE_LIMIT;
        }
        NodeArray nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;
        nodes.reset(NUM_VALUES);
        for (int j = 0; j < NUM_VALUES; ++j) {
            nodes[j].value()  = V.create(VALUES[j]);
            Obj::insert(&x, C, &nodes[j]);
        }
        ASSERT(Obj::isWellFormed(X, C));
        ASSERT(NUM_VALUES == X.numNodes());

        const RbTreeNode *maxNode = Obj::rightmost(X.rootNode());
        const int MAX_VALUE = V.getValue(toNode(maxNode)->value());

        for (int j = 0; j < VALUE_LIMIT + 2; ++j) {
            RbTreeNode       *result = Obj::lowerBound(x, C, V.create(j));
            const RbTreeNode *RESULT = Obj::lowerBound(X, C, V.create(j));

            ASSERT(result == RESULT);
            if (j > MAX_VALUE) {
                ASSERTV(j, MAX_VALUE, X.sentinel() == RESULT);
                continue;
            }
            ASSERTV(j, X.sentinel() != RESULT);
            int nodeValue     = V.getValue(toNode(RESULT)->value());
            int prevNodeValue = INT_MIN;
            if (RESULT != X.firstNode()) {
                const RbTreeNode *prevNode = Obj::previous(RESULT);
                prevNodeValue = V.getValue(toNode(prevNode)->value());
            }
            ASSERTV(j, prevNodeValue, prevNodeValue < j);
            ASSERTV(j, nodeValue,     j <= nodeValue);
        }
    }

    if (veryVerbose)
        printf("\tTest lowerBound on a 50000 node tree \n");
    {
        enum {
            NUM_VALUES = 50000
        };
        int VALUES[NUM_VALUES];
        for (int i = 0; i < NUM_VALUES; ++i) {
            VALUES[i] = rand() % NUM_VALUES;
        }
        NodeArray             nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;
        nodes.reset(NUM_VALUES);

        for (int j = 0; j < NUM_VALUES; ++j) {
            nodes[j].value()  = V.create(VALUES[j]);
            Obj::insert(&x, C, &nodes[j]);
        }
        ASSERT(Obj::isWellFormed(X, C));
        ASSERT(NUM_VALUES == X.numNodes());

        const RbTreeNode *maxNode = Obj::rightmost(X.rootNode());
        const int MAX_VALUE = V.getValue(toNode(maxNode)->value());

        for (int j = 0; j <= NUM_VALUES; ++j) {
            RbTreeNode       *result = Obj::lowerBound(x, C, V.create(j));
            const RbTreeNode *RESULT = Obj::lowerBound(X, C, V.create(j));

            ASSERT(result == RESULT);
            if (j > MAX_VALUE) {
                ASSERT(X.sentinel() == RESULT);
                continue;
            }
            ASSERTV(j, X.sentinel() != RESULT);
            int nodeValue     = V.getValue(toNode(RESULT)->value());
            int prevNodeValue = INT_MIN;
            if (RESULT != X.firstNode()) {
                const RbTreeNode *prevNode = Obj::previous(RESULT);
                prevNodeValue = V.getValue(toNode(prevNode)->value());
            }
            ASSERTV(j, prevNodeValue, prevNodeValue < j);
            ASSERTV(j, nodeValue,     j <= nodeValue);
        }
    }

    if (veryVerbose) printf("---------------------\n");
}

template <class VALUE>
void TestDriver<VALUE>::testCase12()
{
    // ------------------------------------------------------------------------
    // CLASS METHODS: RbTreeUtil::upperBound
    //
    // Concerns:
    //:  1 That upperBound returns sentinel node for an empty tree.
    //:
    //:  2 That, for all well-formed trees, when searching for a value that
    //:    is in the tree, return the rightmost matching value.
    //:
    //:  3 That, for all well-formed trees, when searching for a value not in
    //:    the tree, return the first value greater than the value being
    //:    searched for.
    //:
    //:  4 That, for all well-formed trees, when searching for a value greater
    //:    than or equal to the last node in the tree, upperBound returns the
    //:    sentinel node.
    //:
    //:  5 That, for all well-formed trees, upperBound returns the last node
    //:    less than or equal to the value being searched for.
    //:
    //:  6 That 'upperBound' behaves correctly on both 'const' and non-'const'
    //:    trees
    //:
    //:  7 That 'upperBound' behaves correctly on very large trees.
    //
    // Plan:
    //:  1 Create an empty tree and verify that 'upperBound' returns the
    //:    sentinel node (C-1).
    //:
    //:  2 For a series of pre-defined test trees, test all the values from
    //:    the 0 value through the maximum value in the tree plus 1, verify the
    //:    result of 'upperBound' is the first node whose value is greater
    //:    than the value being searched for. (C-2, C-3, C-4, C-5, C-6)
    //:
    //:  3 For a series of random trees containing duplicates, test all values
    //:    from 0 through a maximum possible value plus 1, verify the
    //:    result of 'upperBound' is the first node whose value is greater
    //:    than the value being searched for. (C-2, C-3, C-4, C-5, C-6)
    //:
    //:  4 For a very large tree of random values, test all possible values in
    //:    the tree and verify that 'upperBound' returns the first node whose
    //:    value is greater than value being searched for. (C-7)
    //
    // Testing:
    //   const RbTreeNode *upperBound(const RbTreeAnchor&          ,
    //                                const NODE_VALUE_COMPARATOR& ,
    //                                const VALUE&                 );
    //   RbTreeNode       *upperBound(RbTreeAnchor&                ,
    //                                const NODE_VALUE_COMPARATOR& ,
    //                                const VALUE&                 );
    // ------------------------------------------------------------------------

    bslma::TestAllocator   ta;
    Comparator C; ValueConverter V;  Assigner A;

    if (veryVerbose) printf("\tTest upperBound on an empty tree\n");
    {
        const int VALUES[]   = { 0, 1, 2, 4, 5, 6 };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        RbTreeAnchor x; const RbTreeAnchor& X = x;
        for (int i = 0; i < NUM_VALUES; ++i) {
            ASSERT(X.sentinel() == Obj::upperBound(X, C, V.create(VALUES[i])));
            ASSERT(X.sentinel() == Obj::upperBound(x, C, V.create(VALUES[i])));
        }
    }

    if (veryVerbose) printf("\tTest upperBound on a set of test trees\n");
    {
        const TreeSpec *VALUES     = TREE_VALUES;
        const int       NUM_VALUES = NUM_TREE_VALUES;

        // Iterate over the set of test trees 'TREE_VALUES'
        for (int i = 0; i < NUM_VALUES; ++i) {
            const char *SPEC = VALUES[i].d_spec;
            const int   LINE = VALUES[i].d_line;

            NodeArray  nodes(&ta);
            RbTreeAnchor x; const RbTreeAnchor& X = x;

            gg(&x, &nodes, SPEC, A);

            const RbTreeNode *maxNode = Obj::rightmost(X.rootNode());
            int max = V.getValue(toNode(maxNode)->value());

            for (int j = 0; j <= max + 1; ++j) {
                RbTreeNode       *result = Obj::upperBound(x, C, V.create(j));
                const RbTreeNode *RESULT = Obj::upperBound(X, C, V.create(j));

                ASSERTV(LINE, result == RESULT);
                ASSERTV(LINE, 0      != RESULT);

                if (j >= max) {
                    ASSERTV(LINE, X.sentinel() == RESULT);
                    continue;
                }
                ASSERTV(LINE, j, X.sentinel() != RESULT);

                int nodeValue     = V.getValue(toNode(RESULT)->value());
                int prevNodeValue = INT_MIN;
                if (RESULT != X.firstNode()) {
                    const RbTreeNode *prevNode = Obj::previous(RESULT);
                    prevNodeValue = V.getValue(toNode(prevNode)->value());
                }

                ASSERTV(LINE, j, prevNodeValue, prevNodeValue <= j);
                ASSERTV(LINE, j, nodeValue,     j < nodeValue);
            }
        }
    }

    if (veryVerbose)
        printf("\tTest find on sets of 1000 random values w/ duplicates\n");
    for (int i = 0; i < 15; ++i) {
        enum {
            NUM_VALUES  = 1000,
            VALUE_LIMIT = 500
        };

        int VALUES[NUM_VALUES];
        for (int i = 0; i < NUM_VALUES; ++i) {
            VALUES[i] = rand() % VALUE_LIMIT;
        }
        NodeArray nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;
        nodes.reset(NUM_VALUES);
        for (int j = 0; j < NUM_VALUES; ++j) {
            nodes[j].value()  = V.create(VALUES[j]);
            Obj::insert(&x, C, &nodes[j]);
        }
        ASSERT(Obj::isWellFormed(X, C));
        ASSERT(NUM_VALUES == X.numNodes());

        const RbTreeNode *maxNode = Obj::rightmost(X.rootNode());
        const int MAX_VALUE = V.getValue(toNode(maxNode)->value());

        for (int j = 0; j < VALUE_LIMIT + 2; ++j) {
            RbTreeNode       *result = Obj::upperBound(x, C, V.create(j));
            const RbTreeNode *RESULT = Obj::upperBound(X, C, V.create(j));

            ASSERT(result == RESULT);
            if (j >= MAX_VALUE) {
                ASSERTV(j, MAX_VALUE, X.sentinel() == RESULT);
                continue;
            }
            ASSERTV(j, X.sentinel() != RESULT);
            int nodeValue     = V.getValue(toNode(RESULT)->value());
            int prevNodeValue = INT_MIN;
            if (RESULT != X.firstNode()) {
                const RbTreeNode *prevNode = Obj::previous(RESULT);
                prevNodeValue = V.getValue(toNode(prevNode)->value());
            }
            ASSERTV(j, prevNodeValue, prevNodeValue <= j);
            ASSERTV(j, nodeValue,     j < nodeValue);
        }
    }

    if (veryVerbose)
        printf("\tTest upperBound on a 50000 node tree \n");
    {
        enum {
            NUM_VALUES = 50000
        };
        int VALUES[NUM_VALUES];
        for (int i = 0; i < NUM_VALUES; ++i) {
            VALUES[i] = rand() % NUM_VALUES;
        }
        NodeArray             nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;
        nodes.reset(NUM_VALUES);

        for (int j = 0; j < NUM_VALUES; ++j) {
            nodes[j].value()  = V.create(VALUES[j]);
            Obj::insert(&x, C, &nodes[j]);
        }
        ASSERT(Obj::isWellFormed(X, C));
        ASSERT(NUM_VALUES == X.numNodes());

        const RbTreeNode *maxNode = Obj::rightmost(X.rootNode());
        const int MAX_VALUE = V.getValue(toNode(maxNode)->value());

        for (int j = 0; j <= NUM_VALUES; ++j) {
            RbTreeNode       *result = Obj::upperBound(x, C, V.create(j));
            const RbTreeNode *RESULT = Obj::upperBound(X, C, V.create(j));

            ASSERT(result       == RESULT);
            if (j >= MAX_VALUE) {
                ASSERT(X.sentinel() == RESULT);
                continue;
            }
            ASSERTV(j, X.sentinel() != RESULT);
            int nodeValue     = V.getValue(toNode(RESULT)->value());
            int prevNodeValue = INT_MIN;
            if (RESULT != X.firstNode()) {
                const RbTreeNode *prevNode = Obj::previous(RESULT);
                prevNodeValue = V.getValue(toNode(prevNode)->value());
            }
            ASSERTV(j, prevNodeValue, prevNodeValue <= j);
            ASSERTV(j, nodeValue,     j < nodeValue);
        }
    }

    if (veryVerbose) printf("---------------------\n");
}

template <class VALUE>
void TestDriver<VALUE>::testCase13()
{
    // ------------------------------------------------------------------------
    // CLASS METHODS: RbTreeUtil::findInsertLocation
    //
    // Concerns:
    //:  1 That 'findInsertLocation' called on an empty tree returns the
    //:    sentinel node, and 'isLeftChild' is 'true'.
    //:
    //:  2 That 'findInsertLocation' always returns a leaf node of the tree.
    //:
    //:  3 If the returned 'isLeftChild' is 'true' then the returned node
    //:    must be first node greater than the supplied value.
    //:
    //:  4 If the returned 'isLeftChild' is 'false'. then the returned node
    //:    must be the last node less than the supplied value.
    //:
    //:  5 'findInsertLocation' behaves correctly for a very large tree.
    //
    // Plan:
    //:  1 Create an empty tree and verify that 'findInsertLocation' returns
    //:    the sentinel node and 'isLeftChild' is 'true' (C-1).
    //:
    //:  2 For a series of pre-defined test trees, test all the values from
    //:    the 0 value through the maximum value in the tree plus 1, verify
    //:    either (1) 'isLeftChild' is 'true' and the returned node is the
    //:    'upperBound' for the value being searched for, or (2) 'isLeftChild'
    //:    is 'false' and the subsequent node from the returned node is the
    //:    'upperBound' of the value being searched for. (C-2, C-3, C-4)
    //:
    //:  3 For a series of random trees containing duplicates, test all values
    //:    from 0 through a maximum possible value plus 1, verify either (1)
    //:    'isLeftChild' is 'true' and the returned node is the 'upperBound'
    //:    for the value being searched for, or (2) 'isLeftChild' is 'false'
    //:    and the subsequent node from the returned node is the 'upperBound'
    //:    of the value being searched for. (C-2, C-3, C-4)
    //:
    //:  4 For a very large tree of random values, test all possible values in
    //:    the tree and verify either (1) 'isLeftChild' is 'true' and the
    //:    returned node is the 'upperBound' for the value being searched for,
    //:    or (2) 'isLeftChild' is 'false' and the subsequent node from the
    //:    returned node is the 'upperBound' of the value being searched
    //:    for. (C-5)
    //
    // Testing:
    //   RbTreeNode *findInsertLocation(bool *,
    //                                  RbTreeAnchor *,
    //                                  const NODE_VALUE_COMPARATOR&
    //                                  const VALUE& );
    // ------------------------------------------------------------------------

    bslma::TestAllocator   ta;
    Comparator C; ValueConverter V;  Assigner A;

    if (veryVerbose) printf("\tTest findInsertLocation on an empty tree\n");
    {
        const int VALUES[]   = { 0, 1, 2, 4, 5, 6 };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        RbTreeAnchor x; const RbTreeAnchor& X = x;
        for (int i = 0; i < NUM_VALUES; ++i) {
            bool left = false;
            ASSERT(X.sentinel() ==
                   Obj::findInsertLocation(&left, &x, C, V.create(VALUES[i])));
            ASSERT(true == left);
        }
    }

    if (veryVerbose)
        printf("\tTest findInsertLocation on a set of test trees\n");
    {
        const TreeSpec *VALUES     = TREE_VALUES;
        const int       NUM_VALUES = NUM_TREE_VALUES;

        // Iterate over the set of test trees 'TREE_VALUES'
        for (int i = 0; i < NUM_VALUES; ++i) {
            const char *SPEC = VALUES[i].d_spec;
            const int   LINE = VALUES[i].d_line;

            NodeArray  nodes(&ta);
            RbTreeAnchor x; const RbTreeAnchor& X = x;

            gg(&x, &nodes, SPEC, A);

            const RbTreeNode *maxNode = Obj::rightmost(X.rootNode());
            int max = V.getValue(toNode(maxNode)->value());

            for (int j = 0; j <= max + 1; ++j) {
                bool leftChild;
                const RbTreeNode *RESULT =
                    Obj::findInsertLocation(&leftChild, &x, C, V.create(j));

                ASSERTV(LINE, 0 != RESULT);
                ASSERTV(LINE, j, X.sentinel() != RESULT);

                const RbTreeNode *EXP_LOC = Obj::upperBound(X, C, V.create(j));
                if (leftChild) {
                    ASSERTV(LINE, j, 0 == RESULT->leftChild());
                    ASSERTV(LINE, j, EXP_LOC == RESULT);
                }
                else {
                    ASSERTV(LINE, j, EXP_LOC == Obj::next(RESULT));
                    ASSERTV(LINE, j, 0 == RESULT->rightChild());
                }
            }
        }
    }

    if (veryVerbose)
        printf("\tTest find on sets of 1000 random values w/ duplicates\n");
    for (int i = 0; i < 15; ++i) {
        enum {
            NUM_VALUES  = 1000,
            VALUE_LIMIT = 500
        };

        int VALUES[NUM_VALUES];
        for (int i = 0; i < NUM_VALUES; ++i) {
            VALUES[i] = rand() % VALUE_LIMIT;
        }
        NodeArray nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;
        nodes.reset(NUM_VALUES);
        for (int j = 0; j < NUM_VALUES; ++j) {
            nodes[j].value()  = V.create(VALUES[j]);
            Obj::insert(&x, C, &nodes[j]);
        }
        ASSERT(Obj::isWellFormed(X, C));
        ASSERT(NUM_VALUES == X.numNodes());

        for (int j = 0; j < VALUE_LIMIT + 2; ++j) {
            bool leftChild;
            const RbTreeNode *RESULT =
                    Obj::findInsertLocation(&leftChild, &x, C, V.create(j));
            const RbTreeNode *EXP_LOC = Obj::upperBound(X, C, V.create(j));

            ASSERTV(0 != RESULT);
            ASSERTV(j, X.sentinel() != RESULT);
            if (leftChild) {
                ASSERTV(j, 0 == RESULT->leftChild());
                ASSERTV(j, EXP_LOC == RESULT);
            }
            else {
                ASSERTV(j, EXP_LOC == Obj::next(RESULT));
                ASSERTV(j, 0 == RESULT->rightChild());
            }
        }
    }

    if (veryVerbose)
        printf("\tTest findInsertLocation on a 50000 node tree \n");
    {
        enum {
            NUM_VALUES = 50000
        };
        int VALUES[NUM_VALUES];
        for (int i = 0; i < NUM_VALUES; ++i) {
            VALUES[i] = rand() % NUM_VALUES;
        }
        NodeArray             nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;
        nodes.reset(NUM_VALUES);

        for (int j = 0; j < NUM_VALUES; ++j) {
            nodes[j].value()  = V.create(VALUES[j]);
            Obj::insert(&x, C, &nodes[j]);
        }
        ASSERT(Obj::isWellFormed(X, C));
        ASSERT(NUM_VALUES == X.numNodes());

        for (int j = 0; j <= NUM_VALUES; ++j) {
            bool leftChild;
            const RbTreeNode *RESULT =
                    Obj::findInsertLocation(&leftChild, &x, C, V.create(j));
            const RbTreeNode *EXP_LOC = Obj::upperBound(X, C, V.create(j));

            ASSERTV(0 != RESULT);
            ASSERTV(j, X.sentinel() != RESULT);
            if (leftChild) {
                ASSERTV(j, 0 == RESULT->leftChild());
                ASSERTV(j, EXP_LOC == RESULT);
            }
            else {
                ASSERTV(j, EXP_LOC == Obj::next(RESULT));
                ASSERTV(j, 0 == RESULT->rightChild());
            }
        }
    }

    if (veryVerbose) printf("---------------------\n");
}


template <class VALUE>
void TestDriver<VALUE>::testCase14()
{
    // ------------------------------------------------------------------------
    // CLASS METHODS: RbTreeUtil::findInsertLocation w/ Hint
    //
    // Concerns:
    //:  1 That 'findInsertLocation' called on an empty tree returns the
    //:    sentinel node, and 'isLeftChild' is 'true'.
    //:
    //:  2 That 'findInsertLocation' called with an invalid hint returns the
    //:    same result as 'findInsertLocation' without a hint.
    //:
    //:  3 That 'findInsertLocation' is called with a valid hint, and 'isLeft'
    //:    child is 'true', the returned node is the hint node.
    //:
    //:  4 That 'findInsertLocation' is called with a valid hint, and 'isLeft'
    //:    child is 'false', the returned node's successor is the hint node.
    //:
    //:  5 That 'findInsertLocation' is called with a valid hint, the number
    //:    of comparisons performed is less than or equal to 2.
    //
    // Plan:
    //:  1 Create an empty tree and verify that 'findInsertLocation' returns
    //:    the sentinel node and 'isLeftChild' is 'true' with the sentinel
    //:    node as the hint (C-1).
    //:
    //:  2 For a series of pre-defined test trees, values from the 0 value
    //:    through the maximum value in the tree plus 1.
    //:    1 For each node in the tree, call 'findInsertLocation' for the
    //:      selected value supplying that node as a hint.
    //:
    //:    2 If the hint node is in the range between the lowerBound and
    //:       upperBound of the value being inserted, ensure that the hint
    //:       would be the subsequent node of the insert location, and that
    //:       only 2 comparisons are performed. (C-3, C-4, C-5)
    //:
    //:    3 If the hint node is not in the range between the lowerBound and
    //:       upperBound of the value being inserted, ensure that the result
    //:       is equivalent to calling 'findInsertLocation' without a hint.
    //:       (C-2).
    //:
    //:  3 For a series of random trees containing duplicates, test all values
    //:    from 0 through a maximum possible value plus 1.
    //:    1 For each node in the tree, call 'findInsertLocation' for the
    //:      selected value supplying that node as a hint.
    //:
    //:    2 If the hint node is in the range between the lowerBound and
    //:       upperBound of the value being inserted, ensure that the hint
    //:       would be the subsequent node of the insert location, and that
    //:       only 2 comparisons are performed. (C-3, C-4, C-5)
    //:
    //:    3 If the hint node is not in the range between the lowerBound and
    //:       upperBound of the value being inserted, ensure that the result
    //:       is equivalent to calling 'findInsertLocation' without a hint.
    //:       (C-2).
    //
    // Testing:
    //   RbTreeNode *findInsertLocation(bool *,
    //                                  RbTreeAnchor *,
    //                                  const NODE_VALUE_COMPARATOR&
    //                                  const VALUE&,
    //                                  RbTreeNode *);
    // ------------------------------------------------------------------------

    bslma::TestAllocator   ta;
    Comparator C; ValueConverter V;  Assigner A;

    if (veryVerbose)
        printf("\tTest findInsertLocation w/ hint on an empty tree\n");
    {
        const int VALUES[]   = { 0, 1, 2, 4, 5, 6 };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        RbTreeAnchor x; const RbTreeAnchor& X = x;
        for (int i = 0; i < NUM_VALUES; ++i) {
            bool left = false;
            VALUE value = V.create(VALUES[i]);
            ASSERT(X.sentinel() ==
                   Obj::findInsertLocation(&left, &x, C, value, x.sentinel()));
            ASSERT(true == left);
        }
    }

    if (veryVerbose)
        printf("\tTest all possible hints on a set of test trees\n");
    {
        const TreeSpec *VALUES     = TREE_VALUES;
        const int       NUM_VALUES = NUM_TREE_VALUES;

        // Iterate over the set of test trees 'TREE_VALUES'
        for (int i = 0; i < NUM_VALUES; ++i) {
            const char *SPEC = VALUES[i].d_spec;
            const int   LINE = VALUES[i].d_line;

            NodeArray  nodes(&ta);
            RbTreeAnchor x; const RbTreeAnchor& X = x;

            gg(&x, &nodes, SPEC, A);

            const RbTreeNode *maxNode = Obj::rightmost(X.rootNode());
            int max = V.getValue(toNode(maxNode)->value());

            // Iterate over the range of all values (+2) in the tree.
            for (int j = 0; j <= max + 2; ++j) {
                VALUE value = V.create(j);
                RbTreeNode *lower = Obj::lowerBound(x, C, value);
                RbTreeNode *upper = Obj::upperBound(x, C, value);

                bool left  = false;
                const RbTreeNode *RESULT;

                // Iterate over all possible hints.
                RbTreeNodeRangeIterator it(X.firstNode(), X.sentinel());
                for (RbTreeNode *hint =  it.next();
                                 0    != hint;
                                 hint  = it.next()) {
                    Comparator C;
                    RESULT =
                        Obj::findInsertLocation(&left, &x, C, value, hint);
                    if (isInRange(hint, lower, upper)) {
                        // If the hint is valid (i.e., between the upper and
                        // lower bound for the value).

                        if (left) {
                            ASSERTV(LINE, j, 0 == RESULT->leftChild());
                            ASSERTV(LINE, j, RESULT == hint);
                        }
                        else {
                            ASSERTV(LINE, j, 0 == RESULT->rightChild());
                            ASSERTV(LINE, j, RESULT == Obj::previous(hint));
                        }
                        ASSERTV(LINE, j, C.numComparisons(),
                                C.numComparisons() <= 2);
                    }
                    else {
                        bool EXP_LEFT;
                        const RbTreeNode *EXP_NODE =
                            Obj::findInsertLocation(&EXP_LEFT, &x, C, value);
                        ASSERTV(LINE, j, EXP_NODE == RESULT);
                        ASSERTV(LINE, j, EXP_LEFT == left);
                    }
                }
            }
        }
    }

    if (veryVerbose)
        printf("\tTest find on sets of 1000 random values w/ duplicates\n");
    for (int i = 0; i < 3; ++i) {
        enum {
            NUM_VALUES  = 1000,
            VALUE_LIMIT = 500
        };

        int VALUES[NUM_VALUES];
        for (int i = 0; i < NUM_VALUES; ++i) {
            VALUES[i] = rand() % VALUE_LIMIT;
        }
        NodeArray nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;
        nodes.reset(NUM_VALUES);
        for (int j = 0; j < NUM_VALUES; ++j) {
            nodes[j].value()  = V.create(VALUES[j]);
            Obj::insert(&x, C, &nodes[j]);
        }
        ASSERT(Obj::isWellFormed(X, C));
        ASSERT(NUM_VALUES == X.numNodes());

        for (int j = 0; j < VALUE_LIMIT + 2; ++j) {
            VALUE value = V.create(j);
            RbTreeNode *lower = Obj::lowerBound(x, C, value);
            RbTreeNode *upper = Obj::upperBound(x, C, value);
            bool left  = false;
            const RbTreeNode *RESULT;

            // Iterate over all possible hints.
            RbTreeNodeRangeIterator it(X.firstNode(), X.sentinel());
            for (RbTreeNode *hint =  it.next();
                             0    != hint;
                             hint  = it.next()) {
                Comparator C;
                RESULT =
                    Obj::findInsertLocation(&left, &x, C, value, hint);
                if (isInRange(hint, lower, upper)) {
                    // If the hint is valid (i.e., between the upper and lower
                    // bound for the value).

                    if (left) {
                        ASSERTV( j, 0 == RESULT->leftChild());
                        ASSERTV( j, RESULT == hint);
                    }
                    else {
                        ASSERTV( j, 0 == RESULT->rightChild());
                        ASSERTV( j, RESULT == Obj::previous(hint));
                    }
                    ASSERTV( j, C.numComparisons(),
                            C.numComparisons() <= 2);
                }
                else {
                    bool EXP_LEFT;
                    const RbTreeNode *EXP_NODE =
                        Obj::findInsertLocation(&EXP_LEFT, &x, C, value);
                    ASSERTV( j, EXP_NODE == RESULT);
                    ASSERTV( j, EXP_LEFT == left);
                }
            }
        }
    }

    if (veryVerbose) printf("---------------------\n");
}


template <class VALUE>
void TestDriver<VALUE>::testCase15()
{
    // ------------------------------------------------------------------------
    // CLASS METHODS: RbTreeUtil::findUniqueInsertLocation
    //
    // Concerns:
    //:  1 That 'findUniqueInsertLocation' called on an empty tree returns the
    //:    sentinel node, and 'comparisonResult' is negative.
    //:
    //:  2 If the tree is not empty, and the value does not exist in the tree,
    //:    that a leaf node is always returned.
    //:
    //:  3 If the value already exists in the tree 'comparisonResult' will be
    //:    0, and node returned will contain the matching value.
    //:
    //:  3 If the value is not in the tree 'comparisonResult' will be non-0.
    //:
    //:  4 If 'comparisonResult' is negative, the returned node
    //:    must be first node greater than the supplied value.
    //:
    //:  5 If 'comparisonResult' is positive, the returned node
    //:    must be first node less than the supplied value.
    //:
    //:  6 'findUniqueInsertLocation' behaves correctly for a very large tree.
    //
    // Plan:
    //:  1 Create an empty tree and verify that 'findUniqueInsertLocation'
    //:    returns the sentinel node and a negative 'comparisonResult' value
    //:    (C-1).
    //:
    //:  2 For a series of pre-defined test trees, test all the values from
    //:    the 0 value through the maximum value in the tree plus 1, verify
    //:    either (1) the value is in the tree, 'comparisonResult' is 0, and
    //:    the returned node is a node containing the value, (2)
    //:    'comparisonResult' is negative, and the returned node is the
    //:    'upperBound' of the value being searched for, or (3)
    //:    'comparisonResult' is positive, and the returned node is the node
    //:    prior to the 'upperBound' if the value being searched for.  (C-2,
    //:    C-3, C-3, C-4, C-5)
    //:
    //:  3 For a series of random trees containing duplicates, test all values
    //:    from 0 through a maximum possible value plus 1, verify either (1)
    //:    the value is in the tree, 'comparisonResult' is 0, and the returned
    //:    node is a node containing the value, (2) 'comparisonResult' is
    //:    negative, and the returned node is the 'upperBound' of the value
    //:    being searched for, or (3) 'comparisonResult' is positive, and the
    //:    returned node is the node prior to the 'upperBound' if the value
    //:    being searched for.  (C-2, C-3, C-3, C-4, C-5)
    //:
    //:  4 For a very large tree of random values, test all possible values in
    //:    the tree and verify either (1) the value is in the tree,
    //:    'comparisonResult' is 0, and the returned node is a node containing
    //:    the value, (2) 'comparisonResult' is negative, and the returned
    //:    node is the 'upperBound' of the value being searched for, or (3)
    //:    'comparisonResult' is positive, and the returned node is the node
    //:    prior to the 'upperBound' if the value being searched for.  (C-6)
    //
    // Testing:
    //   RbTreeNode *findUniqueInsertLocation(int *,
    //                                        RbTreeAnchor *,
    //                                        const NODE_VALUE_COMPARATOR&
    //                                        const VALUE& );
    // ------------------------------------------------------------------------

    bslma::TestAllocator   ta;
    Comparator C; ValueConverter V;  Assigner A;

    if (veryVerbose)
        printf("\tTest findUniqueInsertLocation on an empty tree\n");
    {
        const int VALUES[]   = { 0, 1, 2, 4, 5, 6 };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        RbTreeAnchor x; const RbTreeAnchor& X = x;
        for (int i = 0; i < NUM_VALUES; ++i) {
            int comp = 0;
            ASSERT(X.sentinel() ==
                   Obj::findUniqueInsertLocation(&comp,
                                                 &x,
                                                 C,
                                                 V.create(VALUES[i])));
        }
    }

    if (veryVerbose)
        printf("\tTest findUniqueInsertLocation on a set of test trees\n");
    {
        const TreeSpec *VALUES     = TREE_VALUES;
        const int       NUM_VALUES = NUM_TREE_VALUES;

        // Iterate over the set of test trees 'TREE_VALUES'
        for (int i = 0; i < NUM_VALUES; ++i) {
            const char *SPEC = VALUES[i].d_spec;
            const int   LINE = VALUES[i].d_line;

            NodeArray  nodes(&ta);
            RbTreeAnchor x; const RbTreeAnchor& X = x;

            gg(&x, &nodes, SPEC, A);

            const RbTreeNode *maxNode = Obj::rightmost(X.rootNode());
            int max = V.getValue(toNode(maxNode)->value());

            for (int j = 0; j <= max + 1; ++j) {
                int comp = INT_MIN;
                const RbTreeNode *RESULT =
                    Obj::findUniqueInsertLocation(&comp, &x, C, V.create(j));

                ASSERTV(LINE, 0 != RESULT);
                ASSERTV(LINE, j, X.sentinel() != RESULT);

                if (X.sentinel() != Obj::find(X, C, V.create(j))) {
                    const RbTreeNode* EXP_RESULT =
                                            Obj::upperBound(X, C, V.create(j));
                    ASSERTV(LINE, j, RESULT == Obj::previous(EXP_RESULT));
                    ASSERTV(LINE, j, 0 == comp);
                    continue;
                }
                ASSERTV(LINE, j, 0 != comp);
                const RbTreeNode *EXP_LOC = Obj::upperBound(X, C, V.create(j));
                if (0 > comp) {
                    ASSERTV(LINE, j, 0 == RESULT->leftChild());
                    ASSERTV(LINE, j, EXP_LOC == RESULT);
                }
                else {
                    ASSERTV(LINE, j, EXP_LOC == Obj::next(RESULT));
                    ASSERTV(LINE, j, 0 == RESULT->rightChild());
                }
            }
        }
    }

    if (veryVerbose)
        printf("\tTest find on sets of 1000 random values w/ duplicates\n");
    for (int i = 0; i < 5; ++i) {
        enum {
            NUM_VALUES  = 1000,
            VALUE_LIMIT = 900
        };

        int VALUES[NUM_VALUES];
        for (int i = 0; i < NUM_VALUES; ++i) {
            VALUES[i] = rand() % VALUE_LIMIT;
        }

        NodeArray nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;
        nodes.reset(NUM_VALUES);
        for (int j = 0; j < NUM_VALUES; ++j) {
            nodes[j].value()  = V.create(VALUES[j]);
            Obj::insert(&x, C, &nodes[j]);
        }
        ASSERT(Obj::isWellFormed(X, C));
        ASSERT(NUM_VALUES == X.numNodes());

        for (int j = 0; j < VALUE_LIMIT + 2; ++j) {
            int comp;
            const RbTreeNode *RESULT =
                    Obj::findUniqueInsertLocation(&comp, &x, C, V.create(j));
            ASSERTV(0 != RESULT);
            ASSERTV(j, X.sentinel() != RESULT);

            if (X.sentinel() != Obj::find(X, C, V.create(j))) {
                const RbTreeNode* EXP_RESULT =
                                    Obj::upperBound(X, C, V.create(j));
                ASSERTV(j, RESULT == Obj::previous(EXP_RESULT));
                ASSERTV(j, 0 == comp);
                continue;
            }
            ASSERTV(j, 0 != comp);
            const RbTreeNode *EXP_LOC = Obj::upperBound(X, C, V.create(j));
            if (0 > comp) {
                ASSERTV(j, 0 == RESULT->leftChild());
                ASSERTV(j, EXP_LOC == RESULT);
            }
            else {
                ASSERTV(j, EXP_LOC == Obj::next(RESULT));
                ASSERTV(j, 0 == RESULT->rightChild());
            }
        }
    }

    if (veryVerbose)
        printf("\tTest findInsertLocation on a 50000 node tree \n");
    {
        enum {
            NUM_VALUES = 50000
        };
        int VALUES[NUM_VALUES];
        for (int i = 0; i < NUM_VALUES; ++i) {
            VALUES[i] = rand() % NUM_VALUES;
        }
        NodeArray             nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;
        nodes.reset(NUM_VALUES);

        for (int j = 0; j < NUM_VALUES; ++j) {
            nodes[j].value()  = V.create(VALUES[j]);
            Obj::insert(&x, C, &nodes[j]);
        }
        ASSERT(Obj::isWellFormed(X, C));
        ASSERT(NUM_VALUES == X.numNodes());

        for (int j = 0; j <= NUM_VALUES; ++j) {
            int comp;
            const RbTreeNode *RESULT =
                    Obj::findUniqueInsertLocation(&comp, &x, C, V.create(j));
            ASSERTV(0 != RESULT);
            ASSERTV(j, X.sentinel() != RESULT);

            if (X.sentinel() != Obj::find(X, C, V.create(j))) {
                const RbTreeNode* EXP_RESULT =
                                            Obj::upperBound(X, C, V.create(j));
                ASSERTV(j, RESULT == Obj::previous(EXP_RESULT));
                ASSERTV(j, 0 == comp);
                continue;
            }
            ASSERTV(j, 0 != comp);
            const RbTreeNode *EXP_LOC = Obj::upperBound(X, C, V.create(j));
            if (0 > comp) {
                ASSERTV(j, 0 == RESULT->leftChild());
                ASSERTV(j, EXP_LOC == RESULT);
            }
            else {
                ASSERTV(j, EXP_LOC == Obj::next(RESULT));
                ASSERTV(j, 0 == RESULT->rightChild());
            }
        }
    }
    if (veryVerbose) printf("---------------------\n");
}

template <class VALUE>
void TestDriver<VALUE>::testCase16()
{
    // ------------------------------------------------------------------------
    // CLASS METHODS: RbTreeUtil::findUniqueInsertLocation w/ Hint
    //
    // Concerns:
    //:  1 That 'findUniqueInsertLocation' called on an empty tree returns the
    //:    sentinel node, and 'comparisonResult' is negative.
    //:
    //:  2 That 'findUniqueInsertLocation' called with an invalid hint returns
    //:    the same result as 'findUniqueInsertLocation' without a hint.
    //:
    //:  3 That 'findUniqueInsertLocation' is called with a valid hint, and
    //:    'comparisonResult' is 0, the returned node has the value being
    //:    searched for and is either the hint node or the predecessor of the
    //:    hint node.
    //:
    //:  4 That 'findUniqueInsertLocation' is called with a valid hint, and
    //:    'comparisonResult' is negative, the returned node is the hint node.
    //:
    //:  5 That 'findUniqueInsertLocation' is called with a valid hint, and
    //:    'comparisonResult' is positive, the returned node's successor is
    //:    the hint node.
    //:
    //:  6 That 'findUniqueInsertLocation' is called with a valid hint, the
    //:    number of comparisons performed is less than or equal to 4.
    //
    // Plan:
    //:  1 Create an empty tree and verify that 'findUniqueInsertLocation'
    //:    returns the sentinel node and 'comparisonResult' is negative with
    //:    the sentinel node as the hint (C-1).
    //:
    //:  2 For a series of pre-defined test trees, values from the 0 value
    //:    through the maximum value in the tree plus 1.
    //:    1 For each node in the tree, call 'findUniqueInsertLocation' for the
    //:      selected value supplying that node as a hint.
    //:
    //:    2 If the hint node is in the range between the lowerBound and
    //:       upperBound of the value being inserted, ensure either that the
    //:       value is a duplicate, and a matching node is returned, or that
    //:       the hint would be the subsequent node of the insert location.
    //:       In all cases only 4 comparisons are performed. (C-2, C-3, C-4,
    //:       C-6)
    //:
    //:    3 If the hint node is not in the range between the lowerBound and
    //:       upperBound of the value being inserted, ensure that the result
    //:       is equivalent to calling 'findUniqueInsertLocation' without a
    //:       hint. (C-5).
    //:
    //:  3 For a series of random trees containing duplicates, test all values
    //:    from 0 through a maximum possible value plus 1.
    //:    1 For each node in the tree, call 'findUniqueInsertLocation' for the
    //:      selected value supplying that node as a hint.
    //:
    //:    2 If the hint node is in the range between the lowerBound and
    //:       upperBound of the value being inserted, ensure either that the
    //:       value is a duplicate, and a matching node is returned, or that
    //:       the hint would be the subsequent node of the insert location.
    //:       In all cases only 4 comparisons are performed. (C-2, C-3, C-4,
    //:       C-6)
    //:
    //:    3 If the hint node is not in the range between the lowerBound and
    //:       upperBound of the value being inserted, ensure that the result
    //:       is equivalent to calling 'findUniqueInsertLocation' without a
    //:       hint. (C-5).
    //
    // Testing:
    //   RbTreeNode *findUniqueInsertLocation(int *,
    //                                        RbTreeAnchor *,
    //                                        const NODE_VALUE_COMPARATOR&
    //                                        const VALUE&,
    //                                        RbTreeNode *);
    // ------------------------------------------------------------------------

    bslma::TestAllocator   ta;
    Comparator C; ValueConverter V;  Assigner A;

    if (veryVerbose)
        printf("\tTest findUniqueInsertLocation w/ hint on an empty tree\n");
    {
        const int VALUES[]   = { 0, 1, 2, 4, 5, 6 };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        RbTreeAnchor x; const RbTreeAnchor& X = x;
        for (int i = 0; i < NUM_VALUES; ++i) {
            int comp = 0;
            ASSERT(X.sentinel() ==
                   Obj::findUniqueInsertLocation(&comp,
                                                 &x,
                                                 C,
                                                 V.create(VALUES[i]),
                                                 x.sentinel()));
            ASSERT(comp < 0);
        }
    }

    if (veryVerbose)
        printf("\tTest all possible hints on a set of test trees\n");
    {
        const TreeSpec *VALUES     = TREE_VALUES;
        const int       NUM_VALUES = NUM_TREE_VALUES;

        // Iterate over the set of test trees 'TREE_VALUES'
        for (int i = 0; i < NUM_VALUES; ++i) {
            const char *SPEC = VALUES[i].d_spec;
            const int   LINE = VALUES[i].d_line;

            NodeArray  nodes(&ta);
            RbTreeAnchor x; const RbTreeAnchor& X = x;

            gg(&x, &nodes, SPEC, A);

            const RbTreeNode *maxNode = Obj::rightmost(X.rootNode());
            int max = V.getValue(toNode(maxNode)->value());

            // Iterate over the range of all values (+2) in the tree.
            for (int j = 0; j <= max + 2; ++j) {
                VALUE value = V.create(j);
                RbTreeNode *lower = Obj::lowerBound(x, C, value);
                RbTreeNode *upper = Obj::upperBound(x, C, value);

                int comp;
                const RbTreeNode *RESULT;

                // Iterate over all possible hints.
                RbTreeNodeRangeIterator it(X.firstNode(), X.sentinel());
                for (RbTreeNode *hint =  it.next();
                                 0    != hint;
                                 hint  = it.next()) {
                    Comparator C;
                    RESULT = Obj::findUniqueInsertLocation(&comp,
                                                           &x,
                                                           C,
                                                           value,
                                                           hint);
                    if (isInRange(hint,lower, upper)) {
                        // If the hint is valid (i.e., between the upper and
                        // lower bound for the value).

                        Comparator Cfind;
                        if (X.sentinel() != Obj::find(X, Cfind, value)) {
                            ASSERTV(LINE, j,
                                    j == V.getValue(toNode(RESULT)->value()));
                            ASSERTV(LINE, j, RESULT == hint ||
                                             RESULT == Obj::previous(hint));
                            ASSERTV(LINE, j, 0 == comp);
                        }
                        else if (comp < 0) {
                            ASSERTV(LINE, j, 0 == RESULT->leftChild());
                            ASSERTV(LINE, j, RESULT == hint);
                        }
                        else {
                            ASSERTV(LINE, j, 0 == RESULT->rightChild());
                            ASSERTV(LINE, j, RESULT == Obj::previous(hint));
                        }
                        ASSERTV(LINE, j, C.numComparisons(),
                                C.numComparisons() <= 4);
                    }
                    else {
                        int EXP_COMP;
                        const RbTreeNode *EXP_NODE =
                            Obj::findUniqueInsertLocation(&EXP_COMP,
                                                          &x,
                                                          C,
                                                          value);
                        ASSERTV(LINE, j, EXP_NODE == RESULT);
                        ASSERTV(LINE, j, EXP_COMP == comp);
                    }
                }
            }
        }
    }

    if (veryVerbose)
        printf("\tTest find on sets of 1000 random values w/ duplicates\n");
    for (int i = 0; i < 2; ++i) {
        enum {
            NUM_VALUES  = 800,
            VALUE_LIMIT = 500
        };

        int VALUES[NUM_VALUES];
        for (int i = 0; i < NUM_VALUES; ++i) {
            VALUES[i] = rand() % VALUE_LIMIT;
        }
        NodeArray nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;
        nodes.reset(NUM_VALUES);
        for (int j = 0; j < NUM_VALUES; ++j) {
            nodes[j].value()  = V.create(VALUES[j]);
            Obj::insert(&x, C, &nodes[j]);
        }
        ASSERT(Obj::isWellFormed(X, C));
        ASSERT(NUM_VALUES == X.numNodes());

        for (int j = 0; j < VALUE_LIMIT + 2; ++j) {
            VALUE value = V.create(j);
            RbTreeNode *lower = Obj::lowerBound(x, C, value);
            RbTreeNode *upper = Obj::upperBound(x, C, value);
            int comp;
            const RbTreeNode *RESULT;

            // Iterate over all possible hints.
            RbTreeNodeRangeIterator it(X.firstNode(), X.sentinel());
            for (RbTreeNode *hint =  it.next();
                             0    != hint;
                             hint  = it.next()) {
                Comparator C;
                RESULT = Obj::findUniqueInsertLocation(&comp,
                                                       &x,
                                                       C,
                                                       value,
                                                       hint);
                if (isInRange(hint, lower, upper)) {
                    // If the hint is valid (i.e., between the upper and lower
                    // bound for the value).

                    Comparator Cfind;
                    if (X.sentinel() != Obj::find(X, Cfind, value)) {
                        ASSERTV(j, j == V.getValue(toNode(RESULT)->value()));
                        ASSERTV(j, RESULT == hint ||
                                   RESULT == Obj::previous(hint));
                        ASSERTV(j, 0 == comp);
                    }
                    else if (comp < 0) {
                        ASSERTV(j, 0 == RESULT->leftChild());
                        ASSERTV(j, RESULT == hint);
                    }
                    else {
                        ASSERTV(j, 0 == RESULT->rightChild());
                        ASSERTV(j, RESULT == Obj::previous(hint));
                    }
                    ASSERTV(j, C.numComparisons(),
                            C.numComparisons() <= 4);
                }
                else {
                    int EXP_COMP;
                    const RbTreeNode *EXP_NODE =
                        Obj::findUniqueInsertLocation(&EXP_COMP,
                                                      &x,
                                                      C,
                                                      value);
                    ASSERTV(j, EXP_NODE == RESULT);
                    ASSERTV(j, EXP_COMP == comp);
                }
            }

        }
    }
    if (veryVerbose) printf("---------------------\n");
}



template <class VALUE>
void TestDriver<VALUE>::testCase17()
{
    // ------------------------------------------------------------------------
    // CLASS METHODS: RbTreeUtil::insertAt
    //
    // Concerns:
    //:  1 That 'insertAt' will successfully insert a node into an empty tree,
    //:    regardless if 'parentNode' is the sentinel node, and
    //:    'isLeftChildFlag' is true.
    //:
    //:  2 That 'insertAt' will successfully insert a node and rebalance the
    //:    tree, if the 'parentNode' and 'isLeftChildFlag' indicate a node
    //:    position at which the node could be inserted and still for a
    //:    binary tree.
    //:
    //:  3 That 'insertAt' will successfully insert a node regardless of its
    //:    state.
    //:
    //:  4 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //:  1 Create an empty tree and verify that 'insertAt' correctly inserts a
    //:    node if supplied a sentinel node, and 'true' for
    //:    'isLeftChildFlag'. (C-1, C-3)
    //:
    //:  2 For a series of pre-defined test trees:
    //:    1 For the set of values between 0 and the max value in the tree +2:
    //:      1 For every node in the tree:
    //:        1 Test if the left child of that node is a valid insertion
    //:          location, and if so, call 'insertAt' with that node as the
    //:          'parent' and 'isLeftChildFlag' as 'true' and verify the
    //:          resulting tree is well formed and includes the node. (C-2,
    //:          C-3)
    //:
    //:        2 Test if the right child of that node is a valid insertion
    //:          location, and if so, call 'insertAt' with that node as the
    //:          'parent' and 'isLeftChildFlag' as 'false' and verify the
    //:          resulting tree is well formed and includes the node. (C-2,
    //:          C-3).
    //:
    //:  3 For a series of random trees containing duplicate values:
    //:    1 For the set of values between 0 and the max value in the tree +2:
    //:      1 For every node in the tree:
    //:        1 Test if the left child of that node is a valid insertion
    //:          location, and if so, call 'insertAt' with that node as the
    //:          'parent' and 'isLeftChildFlag' as 'true' and verify the
    //:          resulting tree is well formed and includes the node. (C-2,
    //:          C-3)
    //:
    //:        2 Test if the right child of that node is a valid insertion
    //:          location, and if so, call 'insertAt' with that node as the
    //:          'parent' and 'isLeftChildFlag' as 'false' and verify the
    //:          resulting tree is well formed and includes the node. (C-2,
    //:          C-3)
    //:
    //:  4 Call 'insertAt' with 0 for 'tree', 'parentNode', and 'newNode',
    //:    respectively and ensure that assertions are thrown from appropriate
    //:    build modes.
    //
    // Testing:
    //    void insertAt(RbTreeAnchor *, RbTreeNode *,bool, RbTreeNode *);
    // ------------------------------------------------------------------------

    bslma::TestAllocator   ta;
    Comparator C; ValueConverter V;  Assigner A;

    if (veryVerbose)
        printf("\tTest insertAt an empty tree\n");
    {
        const int VALUES[]   = { 0, 1, 2, 4, 5, 6 };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            Node node;
            node.reset(&node, &node, &node, RED);
            node.value() = V.create(VALUES[i]);

            RbTreeAnchor x; const RbTreeAnchor& X = x;
            Obj::insertAt(&x, x.sentinel(), true, &node);
            ASSERT(Obj::isWellFormed(X, C));
            ASSERT(1 == X.numNodes());
            ASSERT(X.sentinel() == node.parent());
            ASSERT(&node        == X.firstNode());
            ASSERT(&node        == X.rootNode());
        }
    }

    if (veryVerbose)
        printf("\tTest all valid insert positions in the set of test trees\n");
    {
        const TreeSpec *VALUES     = TREE_VALUES;
        const int       NUM_VALUES = NUM_TREE_VALUES;

        // Iterate over the set of test trees 'TREE_VALUES'
        for (int i = 0; i < NUM_VALUES; ++i) {
            const char *SPEC = VALUES[i].d_spec;
            const int   LINE = VALUES[i].d_line;

            NodeArray  nodes(&ta);
            RbTreeAnchor x; const RbTreeAnchor& X = x;

            gg(&x, &nodes, SPEC, A);
            const RbTreeNode *maxNode = Obj::rightmost(X.rootNode());
            const int MAX_VALUE = V.getValue(toNode(maxNode)->value());
            const int NUM_NODES = X.numNodes();

            // Iterate over the range of all values (+2) in the tree.
            for (int value = 0; value <= MAX_VALUE + 2; ++value) {
                // Test whether each node in the tree is a valid parent for
                // 'value'.

                for (int nodeIdx = 0; nodeIdx < X.numNodes(); ++nodeIdx) {
                    RbTreeNode *node = nodeAtOffset(&x, nodeIdx);
                    int nodeValue = V.getValue(toNode(node)->value());
                    int prevValue = INT_MIN;
                    int nextValue = INT_MAX;
                    if (nodeIdx > 0) {
                        const RbTreeNode *prevNode = Obj::previous(node);
                        prevValue = V.getValue(toNode(prevNode)->value());
                    }
                    if (nodeIdx < NUM_NODES - 1) {
                        const RbTreeNode *nextNode = Obj::next(node);
                        nextValue = V.getValue(toNode(nextNode)->value());
                    }

                    // Test if value could be inserted as the leftChild of
                    // 'node'.
                    if ((prevValue <= value)     &&
                        (value     <= nodeValue) &&
                        (!node->leftChild())) {
                        if (veryVeryVerbose) {
                            P_(SPEC); P_(value); P_(nodeIdx); P('L');
                        }
                        Node testNode;
                        testNode.reset(&testNode, &testNode, &testNode, BLACK);
                        testNode.value() = V.create(value);
                        Obj::insertAt(&x, node, true, &testNode);
                        ASSERTV(LINE, value, nodeIdx, Obj::isWellFormed(X, C));
                        ASSERTV(LINE, value, nodeIdx, X.numNodes(),
                                NUM_NODES + 1 == X.numNodes());
                        ASSERTV(LINE, value, nodeIdx,
                                &testNode != testNode.parent());
                        ASSERTV(LINE, value, nodeIdx,
                                Obj::isLeftChild(&testNode) ||
                                Obj::isRightChild(&testNode));


                        // We must reset the tree.
                        gg(&x, &nodes, SPEC, A);
                        node = nodeAtOffset(&x, nodeIdx);
                    }
                    if ((nodeValue <= value) && (value <= nextValue) &&
                        (!node->rightChild())) {
                        if (veryVeryVerbose) {
                            P_(SPEC); P_(value); P_(nodeIdx); P('R');
                        }
                        Node testNode;
                        testNode.reset(&testNode, &testNode, &testNode, BLACK);
                        testNode.value() = V.create(value);
                        Obj::insertAt(&x, node, false, &testNode);
                        ASSERTV(LINE, value, nodeIdx, Obj::isWellFormed(X, C));
                        ASSERTV(LINE, value, nodeIdx, X.numNodes(),
                                NUM_NODES + 1 == X.numNodes());
                        ASSERTV(LINE, value, nodeIdx,
                                &testNode != testNode.parent());
                        ASSERTV(LINE, value, nodeIdx,
                                Obj::isLeftChild(&testNode) ||
                                Obj::isRightChild(&testNode));

                        // We must reset the tree.
                        gg(&x, &nodes, SPEC, A);
                        node = nodeAtOffset(&x, nodeIdx);
                    }

                }
            }
        }
    }

    if (veryVerbose)
        printf("\tTest find on sets of 300 random values w/ duplicates\n");
    for (int i = 0; i < 2; ++i) {
        enum {
            NUM_VALUES  = 200,
            VALUE_LIMIT = 300
        };

        int VALUES[NUM_VALUES];
        for (int i = 0; i < NUM_VALUES; ++i) {
            VALUES[i] = rand() % VALUE_LIMIT;
        }

        NodeArray nodes(&ta);
        RbTreeAnchor x; const RbTreeAnchor& X = x;
        nodes.reset(NUM_VALUES);
        for (int j = 0; j < NUM_VALUES; ++j) {
            nodes[j].value()  = V.create(VALUES[j]);
            Obj::insert(&x, C, &nodes[j]);
        }
        ASSERT(Obj::isWellFormed(X, C));


        // Iterate over the range of all values (+2) in the tree.
        for (int value = 0; value <= VALUE_LIMIT + 2; ++value) {
            // Test whether each node in the tree is a valid parent for
            // 'value'.

            for (int nodeIdx = 0; nodeIdx < NUM_VALUES; ++nodeIdx) {
                RbTreeNode *node = nodeAtOffset(&x, nodeIdx);
                int nodeValue = V.getValue(toNode(node)->value());
                int prevValue = INT_MIN;
                int nextValue = INT_MAX;
                if (nodeIdx > 0) {
                    const RbTreeNode *prevNode = Obj::previous(node);
                    prevValue = V.getValue(toNode(prevNode)->value());
                }
                if (nodeIdx < NUM_VALUES - 1) {
                    const RbTreeNode *nextNode = Obj::next(node);
                    nextValue = V.getValue(toNode(nextNode)->value());
                }

                // Test if value could be inserted as the leftChild of
                // 'node'.
                if ((prevValue <= value)     &&
                    (value     <= nodeValue) &&
                    (!node->leftChild())) {
                    Node testNode;
                    testNode.reset(&testNode, &testNode, &testNode, BLACK);
                    testNode.value() = V.create(value);

                    Obj::insertAt(&x, node, true, &testNode);

                    ASSERTV(value, nodeIdx, Obj::isWellFormed(X, C));
                    ASSERTV(value, nodeIdx, X.numNodes(),
                            NUM_VALUES + 1 == X.numNodes());
                    ASSERTV(value, nodeIdx, &testNode != testNode.parent());
                    ASSERTV(value, nodeIdx, Obj::isLeftChild(&testNode) ||
                                            Obj::isRightChild(&testNode));


                    // We must reset the tree.
                    nodes.reset(NUM_VALUES);
                    x.reset(0, x.sentinel(), 0);
                    for (int j = 0; j < NUM_VALUES; ++j) {
                        nodes[j].value()  = V.create(VALUES[j]);
                        Obj::insert(&x, C, &nodes[j]);
                    }
                    node = nodeAtOffset(&x, nodeIdx);
                }
                if ((nodeValue <= value) && (value <= nextValue) &&
                    (!node->rightChild())) {
                    Node testNode;
                    testNode.reset(&testNode, &testNode, &testNode, BLACK);
                    testNode.value() = V.create(value);

                    Obj::insertAt(&x, node, false, &testNode);

                    ASSERTV(value, nodeIdx, Obj::isWellFormed(X, C));
                    ASSERTV(value, nodeIdx, X.numNodes(),
                            NUM_VALUES + 1 == X.numNodes());
                    ASSERTV(value, nodeIdx, &testNode != testNode.parent());
                    ASSERTV(value, nodeIdx, Obj::isLeftChild(&testNode) ||
                                            Obj::isRightChild(&testNode));


                    // We must reset the tree.
                    nodes.reset(NUM_VALUES);
                    x.reset(0, x.sentinel(), 0);
                    for (int j = 0; j < NUM_VALUES; ++j) {
                        nodes[j].value()  = V.create(VALUES[j]);
                        Obj::insert(&x, C, &nodes[j]);
                    }
                    node = nodeAtOffset(&x, nodeIdx);
                }

            }
        }
    }

    if (veryVerbose) printf("\tNegative testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        RbTreeAnchor x;
        RbTreeNode   n;
        ASSERT_FAIL(Obj::insertAt( 0, x.sentinel(), true, &n));
        ASSERT_FAIL(Obj::insertAt(&x,            0, true, &n));
        ASSERT_FAIL(Obj::insertAt(&x, x.sentinel(), true,  0));
        ASSERT_PASS(Obj::insertAt(&x, x.sentinel(), true, &n));
    }

    if (veryVerbose) printf("---------------------\n");
}

template <class VALUE>
void TestDriver<VALUE>::testCase18()
{
    // ------------------------------------------------------------------------
    // CLASS METHODS: RbTreeUtil::remove
    //
    // Concerns:
    //:  1 That 'remove' removes the indicated node from the tree, and the
    //:    resulting tree is a balanced red-black tree.
    //:
    //:  2 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //:  1 For a series of pre-defined test trees (C-1):
    //:    1 Iterate over the number of nodes in the tree, selecting that node
    //:      as a starting index:
    //:      1 Remove all the nodes from the tree starting from the starting
    //:        index.
    //:
    //:      2 For each removed node, verify the node is no longer in the tree
    //:        and the tree is still well-formed.
    //:
    //:      3 After all the nodes are removed, verify the tree is empty.
    //:
    //:  2 For a series of random trees containing duplicate values (C-1):
    //:    1 Iterate over the number of nodes in the tree, selecting that node
    //:      as a starting index:
    //:      1 Remove all the nodes from the tree starting from the starting
    //:        index.
    //:
    //:      2 For each removed node, verify the node is no longer in the tree
    //:        and the tree is still well-formed.
    //:
    //:      3 After all the nodes are removed, verify the tree is empty.
    //:
    //:  3 Call 'remove' with 0 for 'tree' 'newNode', respectively and ensure
    //:    that assertions are thrown from appropriate build modes.
    //
    // Testing:
    //    void remove(RbTreeAnchor *, RbTreeNode *);
    // ------------------------------------------------------------------------

    bslma::TestAllocator   ta;
    Comparator C; ValueConverter V;  Assigner A;

    if (veryVerbose)
        printf("\tRemove each node from a set of test trees\n");
    {
        const TreeSpec *VALUES     = TREE_VALUES;
        const int       NUM_VALUES = NUM_TREE_VALUES;

        // Iterate over the set of test trees 'TREE_VALUES'
        for (int i = 0; i < NUM_VALUES; ++i) {
            const char *SPEC = VALUES[i].d_spec;
            const int   LINE = VALUES[i].d_line;

            NodeArray  nodes(&ta);
            RbTreeAnchor x; const RbTreeAnchor& X = x;
            gg(&x, &nodes, SPEC, A);
            const int NUM_NODES = X.numNodes();

            // Iterate over every node in the tree, and remove it.
            for (int startIdx = 0; startIdx < NUM_NODES; ++startIdx) {
                gg(&x, &nodes, SPEC, A);
                for (int idxOffset = 0; idxOffset < NUM_NODES; ++idxOffset) {
                    RbTreeNode *node = &nodes[(startIdx+idxOffset)%NUM_NODES];
                    ASSERTV(LINE, startIdx, idxOffset, Obj::isWellFormed(X,C));

                    Obj::remove(&x, node);

                    ASSERTV(LINE, startIdx, idxOffset, Obj::isWellFormed(X,C));
                    ASSERTV(LINE, startIdx, idxOffset,
                            NUM_NODES - idxOffset - 1 == X.numNodes());
                    ASSERTV(!isInRange(node, X.firstNode(), X.sentinel()));
                }
            }
        }
    }

    if (veryVerbose)
        printf("\tRemove each node from a set of random trees\n");
    for (int i = 0; i < 3; ++i) {
        enum {
            NUM_VALUES  = 150,
            VALUE_LIMIT = 100
        };

        int VALUES[NUM_VALUES];
        for (int i = 0; i < NUM_VALUES; ++i) {
            VALUES[i] = rand() % VALUE_LIMIT;
        }


        // Iterate over every node in the tree, and remove it.
        for (int startIdx = 0; startIdx < NUM_VALUES; ++startIdx) {
            NodeArray nodes(&ta);
            RbTreeAnchor x; const RbTreeAnchor& X = x;
            nodes.reset(NUM_VALUES);
            for (int j = 0; j < NUM_VALUES; ++j) {
                nodes[j].value()  = V.create(VALUES[j]);
                Obj::insert(&x, C, &nodes[j]);
            }
            ASSERT(Obj::isWellFormed(X, C));

            for (int idxOffset = 0; idxOffset < NUM_VALUES; ++idxOffset) {
                RbTreeNode *node = &nodes[(startIdx + idxOffset) % NUM_VALUES];
                ASSERTV(startIdx, idxOffset, Obj::isWellFormed(X,C));

                Obj::remove(&x, node);

                ASSERTV(startIdx, idxOffset, Obj::isWellFormed(X,C));
                ASSERTV(startIdx, idxOffset,
                        NUM_VALUES - idxOffset - 1 == X.numNodes());
                ASSERTV(!isInRange(node, X.firstNode(), X.sentinel()));
            }
        }
    }

    if (veryVerbose) printf("\tNegative testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        RbTreeAnchor x;
        RbTreeNode   n;
        Obj::insertAt(&x, x.sentinel(), true, &n);

        ASSERT_FAIL(Obj::remove( 0, &n));
        ASSERT_FAIL(Obj::remove(&x,  0));
        ASSERT_PASS(Obj::remove(&x, &n));
        ASSERT_FAIL(Obj::remove(&x, &n));
    }
    if (veryVerbose) printf("---------------------\n");
}

template <class VALUE>
void TestDriver<VALUE>::testCase20()
{
    // ------------------------------------------------------------------------
    // CLASS METHODS: RbTreeUtil::copyTree
    //
    // Concerns:
    //:  1 That 'copyTree' does nothing on an empty tree.
    //:
    //:  2 That 'copyTree' constructs a well-formed tree having the same
    //:    structure as the source tree.
    //:
    //:  3 For each node in the source tree, the corresponding node in the
    //:    result tree is created by invoking the factory 'createNode' function
    //:    on the source tree.
    //:
    //:  4 If an exception is thrown by 'createNode', then 'deleteNode' is
    //:    called on every newly created node.
    //:
    //:  5 QoI: Asserted precondition violations are detected when enabled.
    //
    //Plan:
    //:  1 Create an empty tree, call 'copyTree' on it, and verify the
    //:    resulting tree is empty and no nodes were allocated or deleted by
    //:    the factory. (C-1)
    //:
    //:  2 For a series of pre-defined test trees, call 'copyTree' and verify
    //:    the factory created the correct number of nodes, and that the
    //:    resulting tree is a well-formed tree having the same set of node
    //:    values (but different node addresses). (C-2, C-3)
    //:
    //:  3 For a series of random trees containing some duplicate values (C-1),
    //:    call 'copyTree' and verify the factory created the correct number
    //:    of nodes, and that the resulting tree is a well-formed tree having
    //:    the same set of node values (but different node addresses). (C-2,
    //:    C-3)
    //:
    //:  3 For a series of random trees containing some duplicate values (C-1),
    //:    create a test factory having a series of node-creation limits, then
    //:    call 'copyTree' and verify that an exception is passed to the
    //:    caller, the resulting tree is a well-formed empty tree, and that
    //:    the node-creation and node-deletion factory function have each been
    //:    correctly called node-limit times.
    //:
    //:  4 Call 'copyTree' with 0 for the factory, 0 for the result, and a
    //:    non-empty tree for the result, respectively, and ensure
    //:    that assertions are thrown from appropriate build modes.
    //
    // Testing:
    //   void RbTreeUtil::copyTree(RbTreeAnchor *, RbTreeAnchor &, FACTORY *);
    // ------------------------------------------------------------------------

    bslma::TestAllocator   ta;
    Comparator C; ValueConverter V;  Assigner A;

    if (veryVerbose)
        printf("\tCopy an empty tree.\n");
    {
        RbTreeAnchor x; const RbTreeAnchor& X = x;
        RbTreeAnchor y; const RbTreeAnchor& Y = y;
        Factory f(0, 0, &ta);

        Obj::copyTree(&y, X, &f);

        ASSERTV(0 == X.numNodes());
        ASSERTV(0 == Y.numNodes());
        ASSERTV(0 == Y.rootNode());
        ASSERTV(0 == f.numCreated());
        ASSERTV(0 == f.numDeleted());
        ASSERTV(Obj::isWellFormed(Y, C));
    }

    if (veryVerbose)
        printf("\tCopy a set of test trees\n");
    {
        const TreeSpec *VALUES     = TREE_VALUES;
        const int       NUM_VALUES = NUM_TREE_VALUES_W_EMPTY;

        // Iterate over the set of test trees 'TREE_VALUES'
        for (int i = 0; i < NUM_VALUES; ++i) {
            const char *SPEC = VALUES[i].d_spec;
            const int   LINE = VALUES[i].d_line;

            NodeArray  nodes(&ta);
            RbTreeAnchor x; const RbTreeAnchor& X = x;
            gg(&x, &nodes, SPEC, A);
            const int NUM_NODES = X.numNodes();

            Factory f(NUM_NODES, &ta);

            RbTreeAnchor y; const RbTreeAnchor& Y = y;

            Obj::copyTree(&y, X, &f);
            ASSERTV(LINE, Obj::isWellFormed(Y, C));
            ASSERTV(LINE, X.numNodes() == Y.numNodes());
            ASSERTV(LINE, X.numNodes() == f.numCreated());
            ASSERTV(LINE, 0            == f.numDeleted());

            const RbTreeNode *xNode = X.firstNode();
            const RbTreeNode *yNode = Y.firstNode();

            while (X.sentinel() != xNode) {
                ASSERTV(LINE, xNode != yNode);
                ASSERTV(LINE, V.getValue(toNode(xNode)->value()) ==
                              V.getValue(toNode(yNode)->value()));
                xNode = Obj::next(xNode);
                yNode = Obj::next(yNode);
            }
        }
    }

    if (veryVerbose)
        printf("\tCopy several random trees\n");
    {

        for (int i = 0; i < 3; ++i) {
            enum {
                NUM_VALUES  = 150,
                VALUE_LIMIT = 100
            };

            int VALUES[NUM_VALUES];
            for (int i = 0; i < NUM_VALUES; ++i) {
                VALUES[i] = rand() % VALUE_LIMIT;
            }


            NodeArray nodes(&ta);
            RbTreeAnchor x; const RbTreeAnchor& X = x;
            nodes.reset(NUM_VALUES);
            for (int j = 0; j < NUM_VALUES; ++j) {
                nodes[j].value()  = V.create(VALUES[j]);
                Obj::insert(&x, C, &nodes[j]);
            }
            ASSERT(Obj::isWellFormed(X, C));

            Factory f(NUM_VALUES, &ta);
            RbTreeAnchor y; const RbTreeAnchor& Y = y;

            Obj::copyTree(&y, X, &f);

            ASSERTV(Obj::isWellFormed(Y, C));
            ASSERTV(X.numNodes() == Y.numNodes());
            ASSERTV(X.numNodes() == f.numCreated());
            ASSERTV(0            == f.numDeleted());

            const RbTreeNode *xNode = X.firstNode();
            const RbTreeNode *yNode = Y.firstNode();

            while (X.sentinel() != xNode) {
                ASSERTV(xNode != yNode);
                ASSERTV(V.getValue(toNode(xNode)->value()) ==
                        V.getValue(toNode(yNode)->value()));
                xNode = Obj::next(xNode);
                yNode = Obj::next(yNode);
            }
        }
    }

#ifdef BDE_BUILD_TARGET_EXC
    if (veryVerbose)
        printf("\tTest exception safety\n");
    {
        for (int i = 0; i < 3; ++i) {
            enum {
                NUM_VALUES  = 30,
                VALUE_LIMIT = 25
            };

            int VALUES[NUM_VALUES];
            for (int i = 0; i < NUM_VALUES; ++i) {
                VALUES[i] = rand() % VALUE_LIMIT;
            }

            NodeArray nodes(&ta);
            RbTreeAnchor x; const RbTreeAnchor& X = x;
            nodes.reset(NUM_VALUES);
            for (int j = 0; j < NUM_VALUES; ++j) {
                nodes[j].value()  = V.create(VALUES[j]);
                Obj::insert(&x, C, &nodes[j]);
            }
            ASSERT(Obj::isWellFormed(X, C));

            for (int nodeLimit = 0; nodeLimit < NUM_VALUES - 1; ++nodeLimit) {
                Factory f(NUM_VALUES, nodeLimit, &ta);
                RbTreeAnchor y; const RbTreeAnchor& Y = y;

                try {
                    Obj::copyTree(&y, X, &f);
                    ASSERTV(nodeLimit, false);
                }
                catch (typename Factory::TestException& ) {
                    ASSERTV(nodeLimit, Obj::isWellFormed(X, C));
                    ASSERTV(nodeLimit, Obj::isWellFormed(Y, C));
                    ASSERTV(nodeLimit, NUM_VALUES == X.numNodes());
                    ASSERTV(nodeLimit, 0          == Y.numNodes());
                    ASSERTV(nodeLimit, nodeLimit  == f.numCreated());
                    ASSERTV(nodeLimit, nodeLimit  == f.numDeleted());
                }
                catch (...)
                {
                    ASSERTV(nodeLimit, false);
                }
            }
        }
    }
#endif

    if (veryVerbose) printf("\tNegative testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        RbTreeAnchor x, y;
        Factory f(0, 0, &ta);
        ASSERT_PASS(Obj::copyTree(&x, y, &f));
        ASSERT_SAFE_FAIL(Obj::copyTree( 0, y, &f));
        ASSERT_SAFE_FAIL(Obj::copyTree(&x, y, (Factory *) 0));

        RbTreeNode   n;
        Obj::insertAt(&x, x.sentinel(), true, &n);
        ASSERT_SAFE_FAIL(Obj::copyTree(&x, y, &f));
    }

    if (veryVerbose) printf("---------------------\n");
}

template <class VALUE>
void TestDriver<VALUE>::testCase21()
{
    // ------------------------------------------------------------------------
    // CLASS METHODS: RbTreeUtil::swapTree
    //
    // Concerns:
    //   1 That 'swap' does nothing on two empty trees.
    //
    //   2 That after a call to 'swap' the lhs contains the equivalent
    //     tree as rhs, and the rhs contains a tree equivalent to the lhs.
    //
    //   3 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //   1 Call swap on two empty trees, and verify that the
    // Testing:
    //   void RbTreeUtil::swap(RbTreeAnchor *, RbTreeAnchor *);
    // ------------------------------------------------------------------------

    bslma::TestAllocator   ta;
    Comparator C; ValueConverter V;  Assigner A;

    if (veryVerbose)
        printf("\tSwap two empty trees.\n");
    {
        RbTreeAnchor x; const RbTreeAnchor& X = x;
        RbTreeAnchor y; const RbTreeAnchor& Y = y;

        Obj::swap(&x, &y);

        ASSERTV(0 == X.numNodes());
        ASSERTV(0 == Y.numNodes());
        ASSERTV(0 == X.rootNode());
        ASSERTV(0 == Y.rootNode());
        ASSERTV(Obj::isWellFormed(X, C));
        ASSERTV(Obj::isWellFormed(Y, C));
    }

    if (veryVerbose)
        printf("\tSwap two test trees.\n");
    {
        const TreeSpec *VALUES     = TREE_VALUES;
        const int       NUM_VALUES = NUM_TREE_VALUES_W_EMPTY;

        // Iterate over the set of test trees 'TREE_VALUES'
        for (int i = 0; i < NUM_VALUES; ++i) {
            const char *SPECX = VALUES[i].d_spec;
            const int   LINEX = VALUES[i].d_line;

            // Iterate over the set of test trees 'TREE_VALUES'
            for (int j = 0; j < NUM_VALUES; ++j) {
                const char *SPECY = VALUES[j].d_spec;
                const int   LINEY = VALUES[j].d_line;

                NodeArray nodesX(&ta); const NodeArray& NODESX = nodesX;
                RbTreeAnchor x; const RbTreeAnchor& X = x;
                gg(&x, &nodesX, SPECX, A);

                NodeArray nodesY(&ta); const NodeArray& NODESY = nodesY;
                RbTreeAnchor y; const RbTreeAnchor& Y = y;
                gg(&y, &nodesY, SPECY, A);

                ASSERTV(LINEX, Obj::isWellFormed(X, C));
                ASSERTV(LINEY, Obj::isWellFormed(Y, C));

                ASSERTV(NODESX.size() == X.numNodes());
                ASSERTV(NODESY.size() == Y.numNodes());

                // Create copies of both X and Y
                Factory fX(X.numNodes(), &ta);
                Factory fY(Y.numNodes(), &ta);

                RbTreeAnchor x2; const RbTreeAnchor& X2 = x2;
                RbTreeAnchor y2; const RbTreeAnchor& Y2 = y2;

                Obj::copyTree(&x2, X, &fX);
                Obj::copyTree(&y2, Y, &fY);

                // Swap X and Y and then compare them to their copies.
                Obj::swap(&x, &y);

                ASSERTV(LINEX, Obj::isWellFormed(X, C));
                ASSERTV(LINEY, Obj::isWellFormed(Y, C));

                ASSERTV(LINEX, LINEY, Y2.numNodes() == X.numNodes());
                ASSERTV(LINEX, LINEY, X2.numNodes() == Y.numNodes());
                ASSERTV(NODESY.size() == X.numNodes());
                ASSERTV(NODESX.size() == Y.numNodes());

                const RbTreeNode *xNode = X.firstNode();
                const RbTreeNode *yNode = Y2.firstNode();

                while (X.sentinel() != xNode) {
                    ASSERTV(LINEX, LINEY, xNode != yNode);
                    ASSERTV(LINEX, LINEY,
                            V.getValue(toNode(xNode)->value()) ==
                            V.getValue(toNode(yNode)->value()));
                    ASSERTV(LINEX, LINEY, nodesY.data() <= xNode);
                    ASSERTV(LINEX, LINEY,
                            xNode < nodesY.data() + nodesY.size());
                    xNode = Obj::next(xNode);
                    yNode = Obj::next(yNode);
                }

                yNode = Y.firstNode();
                xNode = X2.firstNode();

                while (Y.sentinel() != yNode) {
                    ASSERTV(LINEX, LINEY, xNode != yNode);
                    ASSERTV(LINEX, LINEY,
                            V.getValue(toNode(xNode)->value()) ==
                            V.getValue(toNode(yNode)->value()));
                    ASSERTV(LINEX, LINEY, NODESX.data() <= yNode);
                    ASSERTV(LINEX, LINEY,
                            yNode < NODESX.data() + NODESX.size());
                    xNode = Obj::next(xNode);
                    yNode = Obj::next(yNode);
                }

            }
        }
    }

    if (veryVerbose) printf("\tNegative testing.\n");
    {
        bsls::AssertTestHandlerGuard hG;

        RbTreeAnchor x, y;
        Factory f(0, 0, &ta);
        ASSERT_PASS(Obj::swap(&x, &y));
        ASSERT_FAIL(Obj::swap(&x,  0));
        ASSERT_FAIL(Obj::swap( 0, &y));
    }

    if (veryVerbose) printf("---------------------\n");
}
//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
                     verbose = argc > 2;
                 veryVerbose = argc > 3;
             veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVeryVeryVerbose;  // suppress warning

    setbuf(stdout, NULL);    // Use unbuffered output

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 26: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
          if (verbose) printf("\nUSAGE EXAMPLE"
                              "\n=============\n");
          {
              // Test use case 1.
              createTestTreeExample();
          }
          {
              // Test use-case 2.
              bslma::TestAllocator ta;
              const int VALUES[]   = { 8, 10, 2, 1, 3, 7, 9, 4, 5, 6 };
              const int NUM_VALUES = sizeof(VALUES) / sizeof (*VALUES);

              IntSet x(&ta); const IntSet& X = x;
              for (int i = 0; i < NUM_VALUES; ++i) {
                  IntSet y(X, &ta); const IntSet &Y = y;

                  LOOP_ASSERT(i,  X == Y);
                  ASSERT(!(X != Y));

                  ASSERT(X.end() == X.find(VALUES[i]));
                  x.insert(VALUES[i]);
                  x.insert(VALUES[i]);
                  ASSERT(VALUES[i] == *X.find(VALUES[i]));

                  ASSERT(i + 1 == X.size());

                  ASSERT(  X != Y);
                  ASSERT(!(X == Y));
              }

              int i = 1;
              for (IntSet::const_iterator it = X.begin();
                                          it != X.end();
                                          ++it) {
                  ASSERT(i++ == *it);
              }
              ASSERT(X.end() == X.find(11));

              for (int i = 0; i < NUM_VALUES; ++i) {
                  IntSet y(x, &ta); const IntSet &Y = y;

                  ASSERT(  X == Y);
                  ASSERT(!(X != Y));

                  IntSet::const_iterator it = X.find(VALUES[i]);
                  ASSERT(VALUES[i] == *it);

                  IntSet::const_iterator next(it);
                  ++next;

                  ASSERT(next == x.erase(it));
                  ASSERT(X.end() == X.find(VALUES[i]));

                  ASSERT(  X != Y);
                  ASSERT(!(X == Y));

                  ASSERT(NUM_VALUES - i - 1 == X.size());
              }
          }
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // CLASS METHOD: copyTree (Additional Exception Safety Tests)
        //
        // Concern:
        //   1 Additional testing of exception safety of 'copyTree' using
        //     standard 'bslma::Allocator' exception test machinery (rather
        //     than machinery defined in this component).  Note that exception
        //     safety has already been verified.
        //
        // Plan:
        //   1 Create a test tree, then, use the
        //     'BSLMA_TESTALLOCATOR_EXCEPTION' macros to test copying that
        //     tree.
        //
        // Testing:
        //
        // --------------------------------------------------------------------
        if (verbose)
            printf("\nEXCEPTION SAFETY 'copyTree' w/ bslma::Allocator"
                   "\n==============================================\n");

        {
            int       VALUES[]   = { 0, 1, 2, 3, 4, 5, 6, 7};
            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);
            IntNodeComparator nodeComparator;

            bslma::TestAllocator oa;

            RbTreeAnchor tree, copiedTree;
            IntNode      nodes[NUM_VALUES];
            for (int i = 0; i < NUM_VALUES; ++i) {
                nodes[i].setLeftChild(0);
                nodes[i].setRightChild(0);
                nodes[i].setParent(0);
                nodes[i].value() = VALUES[i];

                Obj::insert(&tree, nodeComparator, &nodes[i]);
            }
            ThrowableIntNodeAllocator allocator(&oa);
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                Obj::copyTree(&copiedTree, tree, &allocator);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERTV(areTreesEqual(tree.rootNode(),
                                  copiedTree.rootNode(),
                                  nodeComparator));
            ASSERTV(tree.numNodes() == copiedTree.numNodes());

            ASSERTV(Obj::isWellFormed(tree, nodeComparator));
            ASSERTV(Obj::isWellFormed(copiedTree, nodeComparator));

            Obj::deleteTree(&copiedTree, &allocator);
            ASSERTV(oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
        }
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // CLASS METHODS: printTreeStructure
        //
        // Concerns:
        //: 1 That 'printTreeStructure' writes to the supplied 'FILE' object.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 That 'printTreeStructure' calls the supplied functor for writing
        //:   values.
        //:
        //: 4 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values.
        //
        // Plan:
        //: 1 Create a series of test trees, with varying values for level and
        //:   spacesPerLevel, and their expected print output.  Print to a
        //:   temporary file and verify the output matches the expected
        //:   output.
        //
        // Testing:
        //   void printTreeStructure(FILE *,
        //                           const RbTreeNode *,
        //                           void (*)(FILE *, const RbTreeNode *),
        //                           int,
        //                           int );
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHODS: printTreeStructure"
                            "\n=================================\n");

        const char *fileName = tempFileName(veryVerbose);

        enum {
            BUFFER_SIZE = 4096,
            LINE_SIZE = 128
        };
        char buffer[BUFFER_SIZE];
        char line[LINE_SIZE];

        if (veryVeryVerbose) {
            P(fileName);
        }

        struct {
            int         d_line;
            const char *d_spec;
            int         d_level;
            int         d_spacesPerLevel;
            const char *d_expectedResult;
        } VALUES[] = {
            { L_, ".", 0, -1, "[] "},
            { L_, ".", 2, -1, "  [  ] "},
            { L_, ".", 2, 2, "    [    ]\n"},
            { L_, "{3:R {1:B},{4:B}}", 0, 4,
              "[ 3 {RED}\n"
              "left:\n"
              "    [ 1 {BLACK}\n"
              "    ]\n"
              "right:\n"
              "    [ 4 {BLACK}\n"
              "    ]\n"
              "]\n" },
            { L_, "{3:R {1:B},{4:B}}", 2, 2,
              "    [ 3 {RED}\n"
              "    left:\n"
              "      [ 1 {BLACK}\n"
              "      ]\n"
              "    right:\n"
              "      [ 4 {BLACK}\n"
              "      ]\n"
              "    ]\n" }
        };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            const int   LINE   = VALUES[i].d_line;
            const char *SPEC   = VALUES[i].d_spec;
            const int   LEVEL  = VALUES[i].d_level;
            const int   SPACES = VALUES[i].d_spacesPerLevel;
            const char *EXP    = VALUES[i].d_expectedResult;

            bslma::TestAllocator ta;

            Array<IntNode>      nodes(&ta);

            RbTreeAnchor x; const RbTreeAnchor& X = x;
            gg(&x, &nodes, SPEC, &assignIntNodeValue);

            FILE *output = fopen(fileName, "w");
            ASSERTV(LINE, fileName, errno, output);

            Obj::printTreeStructure(output,
                                    X.rootNode(),
                                    printIntNodeValue,
                                    LEVEL,
                                    SPACES);
            fflush(output);
            fclose(output);

            FILE *input  = fopen(fileName, "r");
            ASSERTV(LINE, fileName, errno, input);

            memset(buffer, 0, BUFFER_SIZE);
            while (0 != fgets(line, LINE_SIZE, input)) {
                strncat(buffer, line, BUFFER_SIZE);
            }
            fclose(input);

            ASSERTV(LINE, buffer, EXP, 0 == strcmp(buffer, EXP));
            if (veryVeryVerbose) {
                P(buffer); P(EXP);
            }
        }

        if (veryVerbose) printf("\tNegative testing.\n");
        {
            bsls::AssertTestHandlerGuard hG;

            RbTreeNode node; const RbTreeNode& NODE = node;
            (void) NODE;  // Suppress 'unused variable' warnings
                          // in non-SAFE modes
            ASSERT_FAIL(Obj::printTreeStructure(0,
                                                &NODE,
                                                printIntNodeValue,
                                                0,
                                                0));
        }
        removeFile(fileName);

      } break;
      case 23: {
        // --------------------------------------------------------------------
        // CLASS METHODS: rotateLeft, rotateRight
        //
        // Concerns:
        //: 1 That 'rotateLeft' rotates a node's right child (the pivot) to
        //:   be the node's parent, and attach the pivot's left child as the
        //:   node's right child.
        //:
        //: 2 That 'rotateRight' the rotates the node's left child (the pivot)
        //:   to be the node's parent, and attach the pivot's right child as
        //:   the node's left child.
        //:
        //: 3 That neither 'rotateLeft' or 'rotateRight' modify any other than
        //:   the rotated node, the pivot node, their children and respective
        //:   parents.
        //:
        //: 4 Calling 'rotateLeft' and then 'rotateRight' on the same node
        //:   leaves the tree to its initial state.  Similarly, calling
        //:   'rotateRight' and then 'rotateLeft' on the same node leaves the
        //:   tree in its initial state.
        //:
        //: 5 That neither 'rotateLeft' or 'rotateRight' modify the relative
        //:   ordering of the tree in which the rotation occurs.
        //:
        //: 6 QoI: Asserted precondition violations are detected when
        //:   enabled.
        //
        // Plan:
        //: 1 Create a series of test trees with a selected rotation node, and
        //:   an expected result for the rotation.  Verify a left-rotation
        //:   generates the expected result, and that the tree is correctly
        //:   ordered after the rotation.
        //:
        //: 2 Create a series of test trees with a selected rotation node, and
        //:   an expected result for the rotation.  Verify a right-rotation
        //:   generates the expected result, and that the tree is correctly
        //:   ordered after the rotation.
        //:
        //: 3 Call 'rotateLeft' and 'rotateRight' with 0 for the rotation
        //:   node, or 0 for the pivot node, verify that the correct
        //:   assertions are invoked in the appropriate build mode.
        //
        // Testing:
        //    void rotateLeft(RbTreeNode *);
        //    void rotateRight(RbTreeNode *);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHODS: rotateLeft, rotateRight"
                            "\n======================================\n");

        void (*P)(FILE *file, const RbTreeNode *node) = printIntNodeValue;

        if (veryVerbose)
            printf("\tVerify left-rotations.\n");

        {
            // Left Rotation
            //..
            //     (node)              (pivot)
            //     /    \              /     \.
            //    a   (pivot)  --->  (node)   c
            //         /   \         /    \.
            //        b     c       a      b
            //..

            struct TestTrees {
                int         d_line;
                int         d_node;
                const char *d_initialState;
                const char *d_resultSpec;
            } VALUES[] = {
{ L_, 1, "{1:R .,{3:B}}", "{3:B {1:R},.}"},
{ L_, 1, "{1:R {0:B},{3:B}}", "{3:B {1:R {0:B},.},.}" },
{ L_, 1, "{1:R .,{3:B {2:R},.}}", "{3:B {1:R .,{2:R}},.}" },
{ L_, 1, "{1:R .,{3:B .,{4:R}}}", "{3:B {1:R},{4:R}}" },
{ L_, 1, "{1:R {0:B},{3:B {2:R},{4:R}}}", "{3:B {1:R {0:B}, {2:R}}, {4:R}}" },
{ L_, 1, "{9:B {1:R .,{3:B}},.}", "{9:B {3:B {1:R},.},.}"},
            };
            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE         = VALUES[i].d_line;
                const char *INITIAL_SPEC = VALUES[i].d_initialState;
                const int   ROTATE_NODE  = VALUES[i].d_node;
                const char *RESULT_SPEC  = VALUES[i].d_resultSpec;

                bslma::TestAllocator    ta;
                IntNodeComparator      C;
                IntNodeValueComparator VC;
                Array<IntNode> nodesX(&ta), nodesExp(&ta);

                RbTreeAnchor x; const RbTreeAnchor& X = x;
                gg(&x, &nodesX, INITIAL_SPEC, &assignIntNodeValue);

                RbTreeAnchor exp; const RbTreeAnchor& EXP = exp;
                gg(&exp, &nodesExp, RESULT_SPEC, &assignIntNodeValue);

                RbTreeNode *node = Obj::find(x, VC, ROTATE_NODE);
                ASSERTV(LINE, 0 != node);

                ASSERTV(LINE, isValidSearchTree(X.rootNode(), C, P));

                Obj::rotateLeft(node);

                ASSERTV(LINE, isValidSearchTree(X.rootNode(), C, P));
                ASSERTV(LINE, areTreesEqual(X.rootNode(), EXP.rootNode(), C));
            }
        }
        if (veryVerbose)
            printf("\tVerify right-rotations.\n");
        {
            // Right Rotation
            //..
            //       (node)            (pivot)
            //       /    \            /     \.
            //   (pivot)   c   --->   a     (node)
            //    /  \                      /    \.
            //   a    b                    b      c
            //..

            struct TestTrees {
                int         d_line;
                int         d_node;
                const char *d_initialState;
                const char *d_resultSpec;
            } VALUES[] = {
{ L_, 3, "{3:B {1:R},.}", "{1:R .,{3:B}}",},
{ L_, 3, "{3:B {1:R {0:B},.},.}", "{1:R {0:B},{3:B}}"},
{ L_, 3, "{3:B {1:R .,{2:R}},.}", "{1:R .,{3:B {2:R},.}}" },
{ L_, 3, "{3:B {1:R},{4:R}}", "{1:R .,{3:B .,{4:R}}}" },
{ L_, 3, "{3:B {1:R {0:B}, {2:R}}, {4:R}}" , "{1:R {0:B},{3:B {2:R},{4:R}}}" },
{ L_, 3, "{9:B {3:B {1:R},.},.}", "{9:B {1:R .,{3:B}},.}" },
            };
            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE         = VALUES[i].d_line;
                const char *INITIAL_SPEC = VALUES[i].d_initialState;
                const int   ROTATE_NODE  = VALUES[i].d_node;
                const char *RESULT_SPEC  = VALUES[i].d_resultSpec;

                bslma::TestAllocator    ta;
                IntNodeComparator      C;
                IntNodeValueComparator VC;
                Array<IntNode> nodesX(&ta), nodesExp(&ta);

                RbTreeAnchor x; const RbTreeAnchor& X = x;
                gg(&x, &nodesX, INITIAL_SPEC, &assignIntNodeValue);

                RbTreeAnchor exp; const RbTreeAnchor& EXP = exp;
                gg(&exp, &nodesExp, RESULT_SPEC, &assignIntNodeValue);

                RbTreeNode *node = Obj::find(x, VC, ROTATE_NODE);
                ASSERTV(LINE, 0 != node);

                ASSERTV(LINE, isValidSearchTree(X.rootNode(), C, P));

                Obj::rotateRight(node);

                ASSERTV(LINE, isValidSearchTree(X.rootNode(), C, P));
                ASSERTV(LINE, areTreesEqual(X.rootNode(), EXP.rootNode(), C));
            }
        }
        if (veryVerbose) printf("\tNegative testing.\n");
        {
            bsls::AssertTestHandlerGuard hG;
            RbTreeNode sentinel, node, pivot;

            sentinel.reset(0, &node, &node, BLACK);
            node.reset(&sentinel, 0, 0, BLACK);
            pivot.reset(&node, 0, 0, BLACK);

            ASSERT_FAIL(Obj::rotateLeft(0));
            ASSERT_FAIL(Obj::rotateRight(0));

            ASSERT_FAIL(Obj::rotateLeft(&node));
            ASSERT_FAIL(Obj::rotateRight(&node));

            sentinel.reset(0, &node, &node, BLACK);
            node.reset(&sentinel, 0, &pivot, BLACK);
            pivot.reset(&node, 0, 0, BLACK);

            ASSERT_FAIL(Obj::rotateRight(&node));
            ASSERT_PASS(Obj::rotateLeft(&node));

            sentinel.reset(0, &node, &node, BLACK);
            node.reset(&sentinel, &pivot, 0, BLACK);
            pivot.reset(&node, 0, 0, BLACK);

            ASSERT_FAIL(Obj::rotateLeft(&node));
            ASSERT_PASS(Obj::rotateRight(&node));
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // CLASS METHODS: isLeftChild, isRightChild
        //
        // Concerns:
        //:  1 That 'isLeftChild' returns 'true' if a node is the left-child
        //:    of its parent, and 'false' if it is the right child.
        //:
        //:  2 That 'isRightChild' returns 'true' if a node is the right-child
        //:    of its parent, and 'false' if it is the left child.
        //:
        //:  3 QoI: Asserted precondition violations are detected when
        //:    enabled.
        //
        // Plan:
        //:  1 Call 'isLeftChild' and 'isRightChild' on several configurations
        //:    of three nodes. (C-1, C-2)
        //:
        //:  2 Call isLeftChild and isRightChild with a 0 node, and a node
        //:    with a 0 parent, and verify that assertions are invoked in
        //:    appropriate build modes. (C-3)
        //
        // Testing:
        //    bool isLeftChild(const RbTreeNode *);
        //    bool isRightChild(const RbTreeNode *);
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHODS: isLeftChild, isRightChild"
                            "\n=======================================\n");

        if (veryVerbose)
            printf("\tVerify methods.\n");

        {
            RbTreeNode a, b, c;

            //      a
            //    /  \.
            //   b    c

            a.reset( 0, &b, &c, BLACK);
            b.reset(&a,  0,  0, BLACK);
            c.reset(&a,  0,  0, BLACK);

            ASSERT( Obj::isLeftChild(&b));
            ASSERT(!Obj::isLeftChild(&c));
            ASSERT(!Obj::isRightChild(&b));
            ASSERT( Obj::isRightChild(&c));

            //      a
            //    /  \.
            //   c    b

            a.reset( 0, &c, &b, BLACK);

            ASSERT(!Obj::isLeftChild(&b));
            ASSERT( Obj::isLeftChild(&c));
            ASSERT( Obj::isRightChild(&b));
            ASSERT(!Obj::isRightChild(&c));

            //      a
            //    /
            //   c

            a.reset( 0, &c,  0, BLACK);
            b.reset( 0,  0,  0, BLACK);

            ASSERT( Obj::isLeftChild(&c));
            ASSERT(!Obj::isRightChild(&c));

            //      a
            //       \.
            //        c

            a.reset( 0,  0, &c, BLACK);

            ASSERT(! Obj::isLeftChild(&c));
            ASSERT( Obj::isRightChild(&c));
        }

        if (veryVerbose) printf("\tNegative testing.\n");
        {
            bsls::AssertTestHandlerGuard hG;
            RbTreeNode x;
            x.reset(0, &x, &x, BLACK);
            ASSERT_SAFE_FAIL(Obj::isLeftChild(0));
            ASSERT_SAFE_FAIL(Obj::isLeftChild(&x));
            ASSERT_SAFE_FAIL(Obj::isRightChild(0));
            ASSERT_SAFE_FAIL(Obj::isRightChild(&x));
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // CLASS METHOD: swapTree
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHODS: swapTree"
                            "\n=======================\n");

        TestDriver<int>::testCase21();

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // CLASS METHOD: copyTree
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHODS: copyTree"
                            "\n=======================\n");

        RUN_ON_ALL_TEST_TYPES(TestDriver, testCase20);

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // CLASS METHODS: deleteTree
        //
        // Concerns:
        //:  1 That 'deleteTree' does nothing on an empty tree.
        //:
        //:  2 That 'deleteTree' calls the factory provided 'deleteNode' on all
        //:    the nodes of the supplied tree.
        //:
        //:  3 QoI: Asserted precondition violations are detected when
        //:    enabled.
        //
        // Plan:
        //:  1 Create a series of test trees made of 'DeleteTestNode' objects
        //:    and then call 'deleteTree' using a 'DeleteTestNodeFactory' and
        //:    verify that all the nodes are marked as deleted.
        //
        // Testing:
        //    void deleteTree(RbTreeAnchor *, FACTORY *);
        // --------------------------------------------------------------------


        if (verbose) printf("\nCLASS METHODS: deleteTree"
                            "\n=========================\n");

        if (veryVerbose)
            printf("\tUse test deleter on a set of test trees.\n");
        {
            const TreeSpec *VALUES     = TREE_VALUES;
            const int       NUM_VALUES = NUM_TREE_VALUES;
            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE        = VALUES[i].d_line;
                const char *SPEC        = VALUES[i].d_spec;

                bslma::TestAllocator ta;

                RbTreeAnchor tree;
                Array<DeleteTestNode> testNodes(&ta);
                gg(&tree, &testNodes, SPEC, &deleteTestNodeAssign);

                DeleteTestNodeFactory testDeleter;
                Obj::deleteTree(&tree, &testDeleter);
                for (int j = 0; j < testNodes.size(); ++j) {
                    ASSERTV(LINE, j, testNodes[j].d_deleted);
                }
                ASSERTV(LINE, testNodes.size()== testDeleter.numInvocations());
                ASSERTV(LINE, tree.sentinel() == tree.firstNode());
                ASSERTV(LINE, 0               == tree.rootNode());
                ASSERTV(LINE, 0               == tree.numNodes());
            }
        }

        if (veryVerbose)
            printf("\tUse test deleter on a couple very large trees");
        {
            const int VALUES[] = { 900, 1000, 50000 };
            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                bslma::TestAllocator ta;
                RbTreeAnchor x; const RbTreeAnchor& X = x;
                DeleteTestNodeComparator comp;
                DeleteTestNodeFactory    testDeleter;
                Array<DeleteTestNode> testNodes(&ta);
                testNodes.reset(VALUES[i]);
                for (int nodeIdx = 0; nodeIdx < VALUES[i]; ++nodeIdx) {
                    testNodes[nodeIdx].d_value = nodeIdx;
                    Obj::insert(&x, comp, &testNodes[nodeIdx]);
                }
                ASSERTV(VALUES[i], Obj::isWellFormed(X, comp));
                ASSERTV(VALUES[i], VALUES[i] == X.numNodes());

                Obj::deleteTree(&x, &testDeleter);
                for (int nodeIdx = 0; nodeIdx < VALUES[i]; ++nodeIdx) {
                    ASSERTV(nodeIdx, testNodes[nodeIdx].d_deleted);
                }
                ASSERT(VALUES[i]    == testDeleter.numInvocations());
                ASSERT(X.sentinel() == X.firstNode());
                ASSERT(0            == X.rootNode());
                ASSERT(0            == X.numNodes());
            }
        }

        if (veryVerbose) printf("\tNegative testing.\n");
        {
            DeleteTestNodeFactory testDeleter;

            bsls::AssertTestHandlerGuard hG;
            RbTreeAnchor x;
            ASSERT_SAFE_FAIL(Obj::deleteTree(  0, &testDeleter));
            ASSERT_SAFE_FAIL(Obj::deleteTree( &x,(DeleteTestNodeFactory *)0));
            ASSERT_PASS(Obj::deleteTree( &x, &testDeleter));
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // CLASS METHOD: remove
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHODS: remove"
                            "\n=====================\n");

        RUN_ON_ALL_TEST_TYPES(TestDriver, testCase18);

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // CLASS METHOD: insertAt
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHODS: insertAt"
                            "\n=======================\n");

        RUN_ON_ALL_TEST_TYPES(TestDriver, testCase17);

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // CLASS METHOD: findUniqueInsertLocation w/ Hint
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nCLASS METHODS: findUniqueInsertLocation w/ Hint"
                   "\n===============================================\n");

        RUN_ON_ALL_TEST_TYPES(TestDriver, testCase16);

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // CLASS METHOD: findUniqueInsertLocation
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHODS: findUniqueInsertLocation"
                            "\n=======================================\n");

        RUN_ON_ALL_TEST_TYPES(TestDriver, testCase15);

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // CLASS METHOD: findInsertLocation w/ Hint
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHODS: findInsertLocation w/ Hint"
                            "\n=========================================\n");

        RUN_ON_ALL_TEST_TYPES(TestDriver, testCase14);


      } break;
      case 13: {
        // --------------------------------------------------------------------
        // CLASS METHOD: findInsertLocation
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHODS: findInsertLocation"
                            "\n=================================\n");

        RUN_ON_ALL_TEST_TYPES(TestDriver, testCase13);

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // CLASS METHOD: upperBound
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHODS: upperBound"
                            "\n=========================\n");

        RUN_ON_ALL_TEST_TYPES(TestDriver, testCase12);

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // CLASS METHOD: lowerBound
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHODS: lowerBound"
                            "\n=========================\n");

        RUN_ON_ALL_TEST_TYPES(TestDriver, testCase11);

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // CLASS METHOD: find
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHODS: find"
                            "\n===================\n");

        RUN_ON_ALL_TEST_TYPES(TestDriver, testCase10);

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // CLASS METHOD: insert
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHODS: insert"
                            "\n=====================\n");

        RUN_ON_ALL_TEST_TYPES(TestDriver, testCase9);

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // CLASS METHOD: isWellFormed
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHODS: isWellFormed"
                            "\n===========================\n");

        RUN_ON_ALL_TEST_TYPES(TestDriver, testCase8);

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // CLASS METHOD: validateRbTree
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHODS: validateRbTree"
                            "\n=============================\n");

        RUN_ON_ALL_TEST_TYPES(TestDriver, testCase7);

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // CLASS METHODS: previous
        //
        // Concerns:
        //: 1 That 'previous' returns the previous node in an infix traversal
        //:   of a binary tree.
        //:
        //: 2 That the right most node of the tree is returned if previous' is
        //:   called on the tree's sentinel node.
        //:
        //: 3 That 'previous' is defined for both const and non-'const'
        //:   'RbTreeNode' objects
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a test table with describing the specification for
        //:   various trees and call 'previous" on each node in each tree and
        //:   verify the results. (C-1, C-2, C-3)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid input (using the 'BSLS_ASSERTTEST_*'
        //:   macros). (C-3)
        //
        // Testing:
        //   static const RbTreeNode *previous(const RbTreeNode *);
        //   static       RbTreeNode *previous(      RbTreeNode *);
        // --------------------------------------------------------------------
        if (verbose) printf("\nCLASS METHODS: previous"
                            "\n=======================\n");

        if (veryVerbose) printf("\tTest behavior of with test table\n");
        {

            struct {
                int         d_line;
                const char *d_spec;
                const char *d_expectedOrderSpec;
            } VALUES[] = {
                // Note that expected values are indices into a node array
                // (having node value + 1)

    // Every possible tree with 1, 2, and 3 nodes.
   { L_, "{1:R}"                                                 , "0" },

   { L_, "{1:R {2:B},.}"                                         , "10"},
   { L_, "{1:R .,{2:B}}"                                         , "01"},

   { L_, "{1:R {2:R {3:R},.},.}"                                 , "210"},
   { L_, "{1:R {2:R .,{3:R}},.}"                                 , "120"},
   { L_, "{1:R {2:B},{3:B}}"                                     , "102"},
   { L_, "{1:R .,{2:B {3:B},.}}"                                 , "021"},
   { L_, "{1:R .,{2:B .,{3:B}}}"                                 , "012"},

    // Ad hoc tree's of 4 to 8 nodes.
   { L_, "{1:R {2:B {3:R},.},{4:B}}"                             , "2103"},
   { L_, "{1:R {2:R ., {3:R}}, {4:R}}"                           , "1203"},
   { L_, "{1:R {2:R ., {3:R}}, {4:R {5:R},.}}"                   , "12043"},
   { L_, "{1:R {2:R {3:R}, {4:R}}, {5:R {6:R},.}}"               , "213054"},
   { L_, "{1:R {2:R {3:R}, {4:R}}, {5:R .,{6:R}}}"               , "213045"},
   { L_, "{1:R {2:R {3:R}, {4:R}}, {5:R {6:R},{7:R}}}"           , "2130546"},
   { L_, "{1:R {2:R {3:R {4:R},.}, {5:R}}, {6:R {7:R},{8:R}}}"   , "32140657"},
   { L_, "{1:R {2:R {3:R ., {4:R}}, {5:R}}, {6:R {7:R},{8:R}}}"  , "23140657"},
   { L_, "{1:R ., {2:R ., {3:R ., {4:R}}}}"                      , "0123"},
   { L_, "{1:R {2:R {3:R {4:R}, .},.},.}"                        , "3210"}
            };

            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);
            for (int i = 0; i < NUM_VALUES; ++i) {
                const int     LINE            = VALUES[i].d_line;
                const char   *SPEC            = VALUES[i].d_spec;
                const char   *EXPECTED_ORDER  = VALUES[i].d_expectedOrderSpec;
                const size_t  EXPECTED_LENGTH = strlen(EXPECTED_ORDER);

                if (veryVeryVerbose) {
                    P_(SPEC); P(EXPECTED_ORDER);
                }

                bslma::TestAllocator      ta;
                RbTreeAnchor             tree;
                Array<IntNode> nodes(&ta);

                gg(&tree, &nodes, SPEC, &assignIntNodeValue);

                for (size_t j = 1; j < EXPECTED_LENGTH; ++j) {
                    int currentIndex = EXPECTED_ORDER[j]-'0';
                    int previousIndex = EXPECTED_ORDER[j-1]-'0';

                    RbTreeNode       *node     = &nodes[currentIndex];
                    const RbTreeNode *NODE     = node;
                    const RbTreeNode *EXPECTED = &nodes[previousIndex];

                    ASSERTV(LINE, j, SPEC, EXPECTED == Obj::previous(node));
                    ASSERTV(LINE, j, SPEC, EXPECTED == Obj::previous(NODE));
                }
                int lastIndex = EXPECTED_ORDER[EXPECTED_LENGTH-1]-'0';
                const RbTreeNode *LAST = &nodes[lastIndex];
                RbTreeNode       *node = tree.sentinel();
                const RbTreeNode *NODE = node;

                ASSERTV(LINE, i, SPEC, LAST == Obj::previous(node));
                ASSERTV(LINE, i, SPEC, LAST == Obj::previous(NODE));

            }
        }
        if (veryVerbose) printf("\tNegative Testing\n");
        {
            bsls::AssertTestHandlerGuard hG;

            RbTreeNode *node = 0; const RbTreeNode *NODE = 0;

            ASSERT_FAIL(Obj::previous(node));
            ASSERT_FAIL(Obj::previous(NODE));

            bslma::TestAllocator      ta;
            RbTreeAnchor             tree;
            Array<IntNode> nodes(&ta);
            gg(&tree, &nodes, "{1:R}", &assignIntNodeValue);

            node = tree.sentinel(); NODE = node;

            ASSERT_PASS(Obj::previous(node));
            ASSERT_PASS(Obj::previous(NODE));

        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CLASS METHODS: next
        //
        // Concerns:
        //: 1 That 'next' returns the next node in an infix traversal of a
        //:   binary tree.
        //:
        //: 2 That the tree's sentinel node is returned if 'next' is called on
        //:   the tree's 'rightmost' node.
        //:
        //: 3 That 'next' is defined for both const and non-'const'
        //:   'RbTreeNode' objects
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a test table with describing the specification for
        //:   various trees and call 'next" on each node in each tree and
        //:   verify the results. (C-1, C-2, C-3)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid input (using the 'BSLS_ASSERTTEST_*'
        //:   macros). (C-3)
        //
        // Testing:
        //   static const RbTreeNode *next(const RbTreeNode *);
        //   static       RbTreeNode *next(      RbTreeNode *);
        // --------------------------------------------------------------------
        if (verbose) printf("\nCLASS METHODS: next"
                            "\n===================\n");

        if (veryVerbose) printf("\tTest behavior of with test table\n");
        {

            struct {
                int         d_line;
                const char *d_spec;
                const char *d_expectedOrderSpec;
            } VALUES[] = {
                // Note that expected values are indices into a node array
                // (having node value + 1)

    // Every possible tree with 1, 2, and 3 nodes.
   { L_, "{1:R}"                                                 , "0" },

   { L_, "{1:R {2:B},.}"                                         , "10"},
   { L_, "{1:R .,{2:B}}"                                         , "01"},

   { L_, "{1:R {2:R {3:R},.},.}"                                 , "210"},
   { L_, "{1:R {2:R .,{3:R}},.}"                                 , "120"},
   { L_, "{1:R {2:B},{3:B}}"                                     , "102"},
   { L_, "{1:R .,{2:B {3:B},.}}"                                 , "021"},
   { L_, "{1:R .,{2:B .,{3:B}}}"                                 , "012"},

    // Ad hoc tree's of 4 to 8 nodes.
   { L_, "{1:R {2:B {3:R},.},{4:B}}"                             , "2103"},
   { L_, "{1:R {2:R ., {3:R}}, {4:R}}"                           , "1203"},
   { L_, "{1:R {2:R ., {3:R}}, {4:R {5:R},.}}"                   , "12043"},
   { L_, "{1:R {2:R {3:R}, {4:R}}, {5:R {6:R},.}}"               , "213054"},
   { L_, "{1:R {2:R {3:R}, {4:R}}, {5:R .,{6:R}}}"               , "213045"},
   { L_, "{1:R {2:R {3:R}, {4:R}}, {5:R {6:R},{7:R}}}"           , "2130546"},
   { L_, "{1:R {2:R {3:R {4:R},.}, {5:R}}, {6:R {7:R},{8:R}}}"   , "32140657"},
   { L_, "{1:R {2:R {3:R ., {4:R}}, {5:R}}, {6:R {7:R},{8:R}}}"  , "23140657"},
   { L_, "{1:R ., {2:R ., {3:R ., {4:R}}}}"                      , "0123"},
   { L_, "{1:R {2:R {3:R {4:R}, .},.},.}"                        , "3210"}
            };

            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);
            for (int i = 0; i < NUM_VALUES; ++i) {
                const int     LINE            = VALUES[i].d_line;
                const char   *SPEC            = VALUES[i].d_spec;
                const char   *EXPECTED_ORDER  = VALUES[i].d_expectedOrderSpec;
                const size_t  EXPECTED_LENGTH = strlen(EXPECTED_ORDER);

                if (veryVeryVerbose) {
                    P_(SPEC); P(EXPECTED_ORDER);
                }

                bslma::TestAllocator      ta;
                RbTreeAnchor             tree;
                Array<IntNode> nodes(&ta);

                gg(&tree, &nodes, SPEC, &assignIntNodeValue);

                for (size_t j = 0; j < EXPECTED_LENGTH - 1; ++j) {
                    int currentIndex = EXPECTED_ORDER[j]-'0';
                    int nextIndex    = EXPECTED_ORDER[j + 1]-'0';

                    RbTreeNode       *node     = &nodes[currentIndex];
                    const RbTreeNode *NODE     = node;
                    const RbTreeNode *EXPECTED = &nodes[nextIndex];

                    ASSERTV(LINE, j, SPEC, EXPECTED == Obj::next(node));
                    ASSERTV(LINE, j, SPEC, EXPECTED == Obj::next(NODE));
                }
                int lastIndex = EXPECTED_ORDER[EXPECTED_LENGTH-1]-'0';
                RbTreeNode       *node     = &nodes[lastIndex];
                const RbTreeNode *NODE     = node;

                ASSERTV(LINE, i, SPEC, tree.sentinel() == Obj::next(node));
                ASSERTV(LINE, i, SPEC, tree.sentinel() == Obj::next(NODE));
            }
        }
        if (veryVerbose) printf("\tNegative Testing\n");
        {
            bsls::AssertTestHandlerGuard hG;

            RbTreeNode *node = 0; const RbTreeNode *NODE = 0;

            ASSERT_FAIL(Obj::next(node));
            ASSERT_FAIL(Obj::next(NODE));

            bslma::TestAllocator      ta;
            RbTreeAnchor             tree;
            Array<IntNode> nodes(&ta);
            gg(&tree, &nodes, "{1:R}", &assignIntNodeValue);

            node = tree.rootNode(); NODE = node;

            ASSERT_PASS(Obj::next(node));
            ASSERT_PASS(Obj::next(NODE));

        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CLASS METHODS: leftmost, rightmost
        //
        // Concerns:
        //: 1 That 'leftmost' returns the left-most node of a binary tree and
        //:   'rightmost' returns the right-most node of a binary tree.
        //
        //: 2 That 'leftmost' and 'rightmost' methods are defined on both
        //:   'const' and non-'const' 'RbTreeNode' objects
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a test table with describing the specification for
        //:   various trees and call 'leftmost' and 'rightmost' on each and
        //:   verify the results. (C-1, C-2)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid input (using the 'BSLS_ASSERTTEST_*'
        //:   macros). (C-3)
        //
        // Testing:
        //   static const RbTreeNode *leftmost(const RbTreeNode *);
        //   static       RbTreeNode *leftmost(      RbTreeNode *);
        //   static const RbTreeNode *rightmost(const RbTreeNode *);
        //   static       RbTreeNode *rightmost(      RbTreeNode *);
        // --------------------------------------------------------------------
        if (verbose) printf("\nCLASS METHODS: leftmost, rightmost"
                            "\n==================================\n");

        if (veryVerbose) printf("\tTest behavior of with test table\n");
        {

            struct {
                int         d_line;
                const char *d_spec;
                int         d_leftmostIndex;
                int         d_rightmostIndex;
            } VALUES[] = {
                // Note that expected values are indices into a node array
                // (having node value + 1)

      // Every possible tree with 1, 2, and 3 nodes.
      { L_, "{1:R}"                                                 , 0, 0},

      { L_, "{1:R {2:B},.}"                                         , 1, 0},
      { L_, "{1:R .,{2:B}}"                                         , 0, 1},

      { L_, "{1:R {2:R {3:R},.},.}"                                 , 2, 0},
      { L_, "{1:R {2:R .,{3:R}},.}"                                 , 1, 0},
      { L_, "{1:R {2:B},{3:B}}"                                     , 1, 2},
      { L_, "{1:R .,{2:B {3:B},.}}"                                 , 0, 1},
      { L_, "{1:R .,{2:B .,{3:B}}}"                                 , 0, 2},

      // Ad hoc tree's of 4 to 8 nodes.
      { L_, "{1:R {2:B {3:R},.},{4:B}}"                             , 2, 3},
      { L_, "{1:R {2:R ., {3:R}}, {4:R}}"                           , 1, 3},
      { L_, "{1:R {2:R ., {3:R}}, {4:R {5:R},.}}"                   , 1, 3},
      { L_, "{1:R {2:R {3:R}, {4:R}}, {5:R {6:R},.}}"               , 2, 4},
      { L_, "{1:R {2:R {3:R}, {4:R}}, {5:R .,{6:R}}}"               , 2, 5},
      { L_, "{1:R {2:R {3:R}, {4:R}}, {5:R {6:R},{7:R}}}"           , 2, 6},
      { L_, "{1:R {2:R {3:R {4:R},.}, {5:R}}, {6:R {7:R},{8:R}}}"   , 3, 7},
      { L_, "{1:R {2:R {3:R ., {4:R}}, {5:R}}, {6:R {7:R},{8:R}}}"  , 2, 7},
      { L_, "{1:R ., {2:R ., {3:R ., {4:R}}}}"                      , 0, 3},
      { L_, "{1:R {2:R {3:R {4:R}, .},.},.}"                        , 3, 0},
            };

            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);
            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE      = VALUES[i].d_line;
                const char *SPEC      = VALUES[i].d_spec;
                const int   LEFTMOST  = VALUES[i].d_leftmostIndex;
                const int   RIGHTMOST = VALUES[i].d_rightmostIndex;

                if (veryVeryVerbose) {
                    P(SPEC);
                }

                bslma::TestAllocator      ta;
                RbTreeAnchor             tree;
                Array<IntNode> nodes(&ta);

                gg(&tree, &nodes, SPEC, &assignIntNodeValue);

                RbTreeNode       *root = tree.rootNode();
                const RbTreeNode *ROOT = tree.rootNode();

                ASSERTV(LINE, SPEC, &nodes[LEFTMOST]  == Obj::leftmost(root));
                ASSERTV(LINE, SPEC, &nodes[LEFTMOST]  == Obj::leftmost(ROOT));
                ASSERTV(LINE, SPEC, &nodes[RIGHTMOST] == Obj::rightmost(root));
                ASSERTV(LINE, SPEC, &nodes[RIGHTMOST] == Obj::rightmost(ROOT));
            }
        }

        if (veryVerbose) printf("\tNegative Testing\n");
        {
            bsls::AssertTestHandlerGuard hG;

            RbTreeNode *node = 0; const RbTreeNode *NODE = 0;

            ASSERT_FAIL(Obj::leftmost(node));
            ASSERT_FAIL(Obj::leftmost(NODE));
            ASSERT_FAIL(Obj::rightmost(node));
            ASSERT_FAIL(Obj::rightmost(NODE));

            RbTreeNode testNode;
            testNode.setLeftChild(0);
            testNode.setRightChild(0);
            testNode.setParent(0);
            testNode.makeBlack();

            node = &testNode; NODE = &testNode;
            ASSERT_PASS(Obj::leftmost(node));
            ASSERT_PASS(Obj::leftmost(NODE));
            ASSERT_PASS(Obj::rightmost(node));
            ASSERT_PASS(Obj::rightmost(NODE));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'gg'
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING GENERATOR FUNCTION: 'gg'"
                            "\n================================\n");

        if (veryVerbose) printf("\tTest empty tree\n");
        {
            bslma::TestAllocator ta;
            RbTreeAnchor x; const RbTreeAnchor& X = x;
            Array<IntNode> nodes(&ta);

            gg(&x, &nodes, ".", &assignIntNodeValue);

            ASSERT(0            == X.rootNode());
            ASSERT(X.sentinel() == X.firstNode());
            ASSERT(0            == X.numNodes());
        }

        if (veryVerbose) printf("\tTest one node trees\n");
        {
            struct TestValue{
                int         d_line;
                const char *d_spec;
                int         d_value;
                Color       d_color;
            } VALUES[] = {
                { L_,  "{0:B}",  0, BLACK },
                { L_,  "{1:B}",  1, BLACK },
                { L_,  "{2:R}",  2,   RED },
                { L_, "{10:B}", 10, BLACK },
                { L_, "{57:R}", 57,   RED },
            };
            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE  = VALUES[i].d_line;
                const char *SPEC  = VALUES[i].d_spec;
                const int   VALUE = VALUES[i].d_value;
                const Color COLOR = VALUES[i].d_color;

                bslma::TestAllocator ta;
                RbTreeAnchor x; const RbTreeAnchor& X = x;
                Array<IntNode> nodes(&ta);

                gg(&x, &nodes, SPEC, &assignIntNodeValue);

                ASSERTV(LINE, 1            == X.numNodes());
                ASSERTV(LINE, 0            != X.rootNode());
                ASSERTV(LINE, X.rootNode() == X.firstNode());
                ASSERTV(LINE, X.rootNode() != X.sentinel());

                const IntNode *NODE =
                    static_cast<const IntNode *>(X.rootNode());

                ASSERTV(LINE, COLOR == NODE->color());
                ASSERTV(LINE, VALUE == NODE->value());
            }
        }
        void (*P)(FILE *file, const RbTreeNode *node) = printIntNodeValue;
        if (veryVerbose) printf("\tTest more complicated tree\n");
        {
            const char *SPEC = "{4:B {2:B},{6:B .,{8:R}}}";

            RbTreeAnchor y; const RbTreeAnchor& Y = y;
            IntNode nodes[4];
            nodes[0].value() = 4;
            nodes[1].value() = 2;
            nodes[2].value() = 6;
            nodes[3].value() = 8;

            y.reset(&nodes[0], &nodes[1], 4);
            nodes[0].reset(y.sentinel(), &nodes[1], &nodes[2], BLACK);
            nodes[1].reset(   &nodes[0],         0,         0, BLACK);
            nodes[2].reset(   &nodes[0],         0, &nodes[3], BLACK);
            nodes[3].reset(   &nodes[2],         0,         0,   RED);

            bslma::TestAllocator ta;
            RbTreeAnchor x; const RbTreeAnchor& X = x;
            Array<IntNode> nodesX(&ta);

            gg(&x, &nodesX, SPEC, &assignIntNodeValue);

            if (veryVeryVerbose) {
                Obj::printTreeStructure(stdout, X.rootNode(), P, 0, 3);
                Obj::printTreeStructure(stdout, Y.rootNode(), P, 0, 3);
            }

            IntNodeComparator nodeComparator;
            ASSERT(areTreesEqual(X.rootNode(), Y.rootNode(), nodeComparator));
        }
        {
            const char *SPEC = "{4:R .,{2:R {6:B {8:B},.},.}} ";

            RbTreeAnchor y; const RbTreeAnchor& Y = y;
            IntNode nodes[4];
            nodes[0].value() = 4;
            nodes[1].value() = 2;
            nodes[2].value() = 6;
            nodes[3].value() = 8;

            y.reset(&nodes[0], &nodes[1], 4);
            nodes[0].reset(y.sentinel(),         0, &nodes[1],   RED);
            nodes[1].reset(   &nodes[0], &nodes[2],         0,   RED);
            nodes[2].reset(   &nodes[1], &nodes[3],         0, BLACK);
            nodes[3].reset(   &nodes[2],         0,         0, BLACK);

            bslma::TestAllocator ta;
            RbTreeAnchor x; const RbTreeAnchor& X = x;
            Array<IntNode> nodesX(&ta);

            gg(&x, &nodesX, SPEC, &assignIntNodeValue);

            if (veryVeryVerbose) {
                Obj::printTreeStructure(stdout, X.rootNode(), P, 0, 3);
                Obj::printTreeStructure(stdout, Y.rootNode(), P, 0, 3);
            }

            IntNodeComparator nodeComparator;
            ASSERT(areTreesEqual(X.rootNode(), Y.rootNode(), nodeComparator));
        }


      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CLASS METHODS: RbTreeUtil_Validator::isWellFormedAnchor
        //
        // Concerns:
        //: 1 That 'isWellFormedAnchor' returns 'false' if 'tree.rootNode()' is
        //:   0, and 'tree.firstNode() != tree.sentinelNode'
        //:
        //: 2 That 'isWellFormedAnchor' returns 'false' if 'tree.rootNode' is
        //:   not 0, and
        //:   'tree.firstNode() != RbTreeUtil::leftmost(tree.rootNode())'
        //:
        //: 3 That 'isWellFormedAnchor' returns 'false' if 'tree.numNodes()'
        //:   does not equal the number of nodes in the tree
        //:
        //: 4 That 'isWellFormedAnchor' returns 'false' if
        //:   'tree.sentinel()->leftChild() != tree.rootNode()'
        //:
        //: 5 That 'isWellFormedAnchor' returns 'false' if 'tree.rootNode()'
        //:    is not 0, and 'tree.rootNode()->parent != tree.sentinel()'.
        //:
        //: 6 That 'isWellFormedAnchor' returns 'false' if 'tree.rootNode()'
        //:    is not 0, and 'tree.rootNode()->isBlack()' is 'false.
        //:
        //: 7 That 'isWellFormedAnchor' returns 'true' if none of the above
        //:   criteria are violated.
        //
        // Plan:
        //: 1 Create a valid empty tree, verify 'isWellFormedAnchor' returns
        //:   'true, adjust various properties so that it violates each
        //:   different rule independently, and verify 'isWellFormedAnchor'
        //:   returns 'false'.
        //:
        //: 2 Create a 3 node tree, verify 'isWellFormedAnchor' returns
        //:   'true, adjust various properties so that it violates each
        //:   different rule independently, and verify 'isWellFormedAnchor'
        //:   returns 'false'.
        //:
        //: 3 Create a larger tree, verify 'isWellFormedAnchor' returns
        //:   'true, adjust various properties so that it violates each
        //:   different rule independently, and verify 'isWellFormedAnchor'
        //:   returns 'false'.
        //
        // Testing:
        //   static bool isWellFormedAnchor(const RbTreeAnchor& );
        // --------------------------------------------------------------------

        if (verbose) printf("\nCLASS METHODS: isWellFormedAnchor"
                            "\n=================================\n");
        RbTreeNode dummyNode;
        dummyNode.reset(0, 0, 0, RED);

        if (veryVerbose) printf("\tTest an empty tree\n");

        {
            RbTreeAnchor tree;
            ASSERT(true  == RbTreeUtil_Validator::isWellFormedAnchor(tree));

            // 1 That 'isWellFormedAnchor' returns 'false' if
            //   'tree.rootNode()' is 0, and
            //   'tree.firstNode() != tree.sentinelNode'
            ASSERT(true  == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.setFirstNode(&dummyNode);
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.setFirstNode(tree.sentinel());

            //  3 That 'isWellFormedAnchor' returns 'false' if
            //    'tree.numNodes()' does not equal the number of nodes in the
            //    tree
            ASSERT(true  == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.setNumNodes(1);
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.setNumNodes(0);

            //  4 That 'isWellFormedAnchor' returns 'false' if
            //    'tree.sentinel()->leftChild() != tree.rootNode()'
            ASSERT(true  == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.sentinel()->setLeftChild(&dummyNode);
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.sentinel()->setLeftChild(0);

            // Violate different combinations of conditions
            ASSERT(true  == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.setFirstNode(&dummyNode);
            tree.sentinel()->setLeftChild(&dummyNode);
            tree.setNumNodes(1);
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.setNumNodes(0);
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));

        }

        if (veryVerbose) printf("\tTesting on a 3 node tree\n");
        {
            RbTreeAnchor tree;
            RbTreeNode   nodeA, nodeB, nodeC;
            nodeA.reset(tree.sentinel(), &nodeB, 0, BLACK);
            nodeB.reset(&nodeA, &nodeC, 0, RED);
            nodeC.reset(&nodeB, 0, 0, RED);

            tree.reset(&nodeA, &nodeC, 3);

            // 2 That 'isWellFormedAnchor' returns 'false' if 'tree.rootNode'
            //   is not 0, and
            //   'tree.firstNode() != RbTreeUtil::leftmost(tree.rootNode())'
            ASSERT(true  == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.setFirstNode(&dummyNode);
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.setFirstNode(&nodeB);
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.setFirstNode(&nodeA);
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.setFirstNode(&nodeC);

            //  3 That 'isWellFormedAnchor' returns 'false' if
            //  'tree.numNodes()' does not equal the number of nodes in the
            //  tree
            ASSERT(true == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.setNumNodes(0);
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.setNumNodes(2);
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.setNumNodes(3);

            //  4 That 'isWellFormedAnchor' returns 'false' if
            //    'tree.sentinel()->leftChild() != tree.rootNode()'
            ASSERT(true == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.sentinel()->setLeftChild(&dummyNode);
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.sentinel()->setLeftChild(&nodeB);
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.sentinel()->setLeftChild(&nodeC);
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.sentinel()->setLeftChild(&nodeA);

            //  5 That 'isWellFormedAnchor' returns 'false' if
            //  'tree.rootNode()' is not 0, and
            // 'tree.rootNode()->parent != tree.sentinel()'.
            ASSERT(true  == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.rootNode()->setParent(0);
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.rootNode()->setParent(&dummyNode);
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.rootNode()->setParent(&nodeC);
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.rootNode()->setParent(tree.sentinel());

            // 6 That 'isWellFormedAnchor' returns 'false' if 'tree.rootNode()'
            //   is not 0, and 'tree.rootNode()->isBlack()' is 'false.
            ASSERT(true  == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.rootNode()->makeRed();
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.rootNode()->makeBlack();

            // Violate different combinations of conditions
            ASSERT(true  == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.setFirstNode(&dummyNode);
            tree.setNumNodes(0);
            tree.sentinel()->setLeftChild(&dummyNode);
            tree.rootNode()->setParent(0);
            tree.rootNode()->makeRed();
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.rootNode()->makeBlack();
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.setNumNodes(3);
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.sentinel()->setLeftChild(&nodeA);
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.setFirstNode(&nodeC);
            tree.sentinel()->setLeftChild(&dummyNode);
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.rootNode()->setParent(tree.sentinel());
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.sentinel()->setLeftChild(&nodeA);

            ASSERT(true  == RbTreeUtil_Validator::isWellFormedAnchor(tree));
        }
        if (veryVerbose) printf("\tTesting on a many node tree\n");
        {
            typedef RbTreeUtil_Validator Validator;
            const char *TREE_SPEC =
                "{0:B {1:R {2:R ., {3:R}}, {4:R}}, {5:R {6:R},{7:R}}}";
            const int NUM_NODES = 8;
            //                      0
            //                   /    \.
            //                  1      5
            //                 / \.   /  \.
            //                2   4  6    7
            //                 \.
            //                  3

            bslma::TestAllocator      ta;
            RbTreeAnchor             tree;
            Array<IntNode> nodes(&ta);

            gg(&tree, &nodes, TREE_SPEC, &assignIntNodeValue);

            ASSERT(true == Validator::isWellFormedAnchor(tree));

            // 2 That 'isWellFormedAnchor' returns 'false' if 'tree.rootNode'
            //   is not 0, and
            //   'tree.firstNode() != RbTreeUtil::leftmost(tree.rootNode())'
            ASSERT(true == Validator::isWellFormedAnchor(tree));
            for (int i = 0; i < NUM_NODES; ++i) {
                tree.setFirstNode(&nodes[i]);
                ASSERTV(i, (i == 2) == Validator::isWellFormedAnchor(tree));
            }
            tree.setFirstNode(&nodes[2]);

            //  3 That 'isWellFormedAnchor' returns 'false' if
            //  'tree.numNodes()' does not equal the number of nodes in the
            //  tree
            ASSERT(true == Validator::isWellFormedAnchor(tree));
            for (int i = 0; i < NUM_NODES; ++i) {
                tree.setNumNodes(i);
                ASSERTV(i, false == Validator::isWellFormedAnchor(tree));
            }
            tree.setNumNodes(NUM_NODES);

            //  4 That 'isWellFormedAnchor' returns 'false' if
            //    'tree.sentinel()->leftChild() != tree.rootNode()'
            ASSERT(true == Validator::isWellFormedAnchor(tree));
            for (int i = 0; i < NUM_NODES; ++i) {
                tree.sentinel()->setLeftChild(&nodes[i]);
                ASSERTV(i, (i == 0) == Validator::isWellFormedAnchor(tree));
            }
            tree.sentinel()->setLeftChild(&nodes[0]);

            //  5 That 'isWellFormedAnchor' returns 'false' if
            //  'tree.rootNode()' is not 0, and
            // 'tree.rootNode()->parent != tree.sentinel()'.
            ASSERT(true == Validator::isWellFormedAnchor(tree));
            for (int i = 0; i < NUM_NODES; ++i) {
                tree.rootNode()->setParent(&nodes[i]);
                ASSERTV(i, false == Validator::isWellFormedAnchor(tree));
            }
            tree.rootNode()->setParent(tree.sentinel());
            ASSERT(true == Validator::isWellFormedAnchor(tree));


            // 6 That 'isWellFormedAnchor' returns 'false' if 'tree.rootNode()'
            //   is not 0, and 'tree.rootNode()->isBlack()' is 'false.
            ASSERT(true  == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.rootNode()->makeRed();
            ASSERT(false == RbTreeUtil_Validator::isWellFormedAnchor(tree));
            tree.rootNode()->makeBlack();
            ASSERT(true  == RbTreeUtil_Validator::isWellFormedAnchor(tree));

        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Plan:
        //   Perform and ad-hoc test of the primary modifiers and accessors.
        //   In this instance, create a breathing test for each individual
        //   function.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");


        {
            if (veryVerbose) {
                printf("\tTest generator gg\n");
            }
            const char *spec[] = {
                "{1:R}",
                "{1:B}",
                "{2:R}",
                "{2:B .,.}",
                "{1:R {2:B},.}",
                "{1:R {2:B},{4:B}}",
                "{1:R {2:B {3:R},.},{4:B}}",
            };
            const int SPEC_LEN = sizeof(spec)/sizeof(*spec);
            for (int i = 0; i < SPEC_LEN; ++i) {
                bslma::TestAllocator ta;
                RbTreeAnchor tree;
                Array<IntNode> nodes(&ta);

                gg(&tree, &nodes, spec[i], &assignIntNodeValue);

                if (veryVeryVerbose) {
                    Obj::printTreeStructure(stdout,
                                            nodes.data(),
                                            printIntNodeValue, 0, 3);
                }
            }
        }

        {
            if (veryVerbose) {
                printf("\tvalidateRbTree\n");
            }
    //..
    // {N : [R|B]} - N indicates the index of the node in 'nodes'
    //             - [R|B] indicates whether the node is Red or Black
    //
    //                          20{0:B}
    //                        /        \.
    //                       /          \.
    //                    10{1:R}        30{5:R}
    //                   /    \         /    \.
    //                5{2:B}  15{4:B} 25{6:B}  35{8:B}
    //                /                \.
    //               1{3:R}            27{7:R}
    //..

            RbTreeAnchor tree;
            createBreathingTestTree(&tree);

            // 'createBreathingTestTree' returns a contiguous sequences of 9
            // nodes.
            IntNode *nodes = (IntNode *)tree.rootNode();

            const char       *ed = 0;
            const RbTreeNode *en = 0;
            IntNodeComparator comp;

            // Testing 0 value.
            ASSERT(0 == Obj::validateRbTree(&en, &ed, 0, comp));

            // Testing valid values.
            ASSERT(2 == Obj::validateRbTree(&en, &ed, &nodes[0], comp));
            ASSERT(1 == Obj::validateRbTree(&en, &ed, &nodes[1], comp));
            ASSERT(1 == Obj::validateRbTree(&en, &ed, &nodes[2], comp));
            ASSERT(0 == Obj::validateRbTree(&en, &ed, &nodes[3], comp));
            ASSERT(1 == Obj::validateRbTree(&en, &ed, &nodes[4], comp));
            ASSERT(1 == Obj::validateRbTree(&en, &ed, &nodes[5], comp));
            ASSERT(1 == Obj::validateRbTree(&en, &ed, &nodes[6], comp));
            ASSERT(0 == Obj::validateRbTree(&en, &ed, &nodes[7], comp));
            ASSERT(1 == Obj::validateRbTree(&en, &ed, &nodes[8], comp));

            // Testing invalid values.
            nodes[4].setColor(RED);
            ASSERT(0 > Obj::validateRbTree(&en, &ed, &nodes[0], comp));
            ASSERT(0 != en);
            ASSERT(0 != ed);

            en = 0; ed = 0;
            nodes[4].setColor(BLACK);
            nodes[4].setParent(&nodes[2]);
            ASSERT(0 > Obj::validateRbTree(&en, &ed, &nodes[0], comp));
            ASSERT(0 != en);
            ASSERT(0 != ed);

            en = 0; ed = 0;
            nodes[4].setParent(&nodes[1]);
            nodes[3].setColor(BLACK);
            ASSERT(0 > Obj::validateRbTree(&en, &ed, &nodes[0], comp));
            ASSERT(0 != en);
            ASSERT(0 != ed);

            en = 0; ed = 0;
            nodes[3].setColor(RED);
            nodes[4].value() = 21;
            ASSERT(0 > Obj::validateRbTree(&en, &ed, &nodes[0], comp));
            ASSERT(0 != en);
            ASSERT(0 != ed);

        }
        {
            if (veryVerbose) {
                printf("\tisWellFormed\n");
            }

    //..
    // {N : [R|B]} - N indicates the index of the node in 'nodes'
    //             - [R|B] indicates whether the node is Red or Black
    //
    //                          20{0:B}
    //                        /        \.
    //                       /          \.
    //                    10{1:R}        30{5:R}
    //                   /    \         /    \.
    //                5{2:B}  15{4:B} 25{6:B}  35{8:B}
    //                /                \.
    //               1{3:R}            27{7:R}
    //..
            IntNodeComparator comp;
            RbTreeAnchor      tree;
            ASSERT(Obj::isWellFormed(tree, comp));

            createBreathingTestTree(&tree);
            IntNode *nodes = (IntNode *)tree.rootNode();

            ASSERT(Obj::isWellFormed(tree, comp));

            // Rule 1: The root node refers to a valid red-black tree.
            {
                // Modify 'tree' so its not a valid red-black tree.  Note
                // that we've already verified 'validateRbTree'.

                nodes[4].setColor(RED);
                ASSERT(!Obj::isWellFormed(tree, comp));
                nodes[4].setColor(BLACK);
                ASSERT( Obj::isWellFormed(tree, comp));
            }

            // Rule 2: The first node refers to the leftmost node in the tree.
            {
                for (int i = 0; i < 9; ++i) {
                    tree.setFirstNode(&nodes[i]);
                    ASSERT((i == 3) == Obj::isWellFormed(tree, comp));
                }
                tree.setFirstNode(tree.sentinel());
                ASSERT(!Obj::isWellFormed(tree, comp));
                tree.setFirstNode(&nodes[3]);
                ASSERT( Obj::isWellFormed(tree, comp));
            }

            // Rule 3: The node count is the number of nodes in the tree.
            {
                for (int i = 0; i < 20; ++i) {
                    tree.setNumNodes(i);
                    ASSERT((i == 9) == Obj::isWellFormed(tree, comp));
                }
                tree.setNumNodes(9);
                ASSERT( Obj::isWellFormed(tree, comp));
            }

            // Rule 4: The sentinel node refers to the root node as its left
            //         child, and the root node refers to the sentinel as its
            //         parent.
            {
                RbTreeNode *sentinel = tree.sentinel();
                RbTreeNode *root     = tree.rootNode();

                root->setParent(0);
                ASSERT(!Obj::isWellFormed(tree, comp));
                for (int i = 0; i < 9; ++i) {
                    root->setParent(&nodes[i]);
                    ASSERT(!Obj::isWellFormed(tree, comp));
                }
                root->setParent(sentinel);
                ASSERT( Obj::isWellFormed(tree, comp));

                sentinel->setLeftChild(0);
                ASSERT(!Obj::isWellFormed(tree, comp));
                for (int i = 0; i < 9; ++i) {
                    sentinel->setLeftChild(&nodes[i]);
                    ASSERT((i == 0) == Obj::isWellFormed(tree, comp));
                }
                sentinel->setLeftChild(&nodes[0]);
                ASSERT(Obj::isWellFormed(tree, comp));
            }

        }
        if (veryVerbose) {
            printf("\tNavigation operations\n");
        }
        {
            RbTreeAnchor tree;
            createBreathingTestTree(&tree);
            if (veryVeryVerbose) {
                printIntTree(tree);
            }
    //..
    // {N : [R|B]} - N indicates the index of the node in 'nodes'
    //             - [R|B] indicates whether the node is Red or Black
    //
    //                          20{0:B}
    //                        /        \.
    //                       /          \.
    //                    10{1:R}        30{5:R}
    //                   /    \         /    \.
    //                5{2:B}  15{4:B} 25{6:B}  35{8:B}
    //                /                \.
    //               1{3:R}            27{7:R}
    //..
            struct {
               int d_line;
               int d_index;
               int d_minIndex;
               int d_maxIndex;
               int d_nextIndex;
               int d_prevIndex;
            } DATA[] = {
            //   LINE INDEX MIN MAX NEXT PREV
            //   ---- ----- --- --- ---- ----
                 { L_,    0,  3,  8,  6,   4 },
                 { L_,    1,  3,  4,  4,   2 },
                 { L_,    2,  3,  2,  1,   3 },
                 { L_,    3,  3,  3,  2,  -1 },
                 { L_,    4,  4,  4,  0,   1 },
                 { L_,    5,  6,  8,  8,   7 },
                 { L_,    6,  6,  7,  7,   0 },
                 { L_,    7,  7,  7,  5,   6 },
                 { L_,    8,  8,  8, -1,   5 },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            const IntNode *nodes = (IntNode *)tree.rootNode();
            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE = DATA[i].d_line;
                const RbTreeNode *node       = &nodes[DATA[i].d_index];
                const RbTreeNode *minResult  = &nodes[DATA[i].d_minIndex];
                const RbTreeNode *maxResult  = &nodes[DATA[i].d_maxIndex];
                const RbTreeNode *nextResult =
                             testNodeAtIndex(nodes, tree, DATA[i].d_nextIndex);
                const RbTreeNode *prevResult =
                             testNodeAtIndex(nodes, tree, DATA[i].d_prevIndex);

                if (veryVeryVerbose) {
                    P(LINE);
                }

                ASSERTV(LINE, minResult  == Obj::leftmost(node));
                ASSERTV(LINE, maxResult  == Obj::rightmost(node));
                ASSERTV(LINE, nextResult == Obj::next(node));
                if (minResult != node) {
                    ASSERTV(LINE, prevResult == Obj::previous(node));
                }
            }
        }
        if (veryVerbose) {
            printf("\tlowerBound and upperBound\n");
        }
        {
            RbTreeAnchor tree;
            createBreathingTestTree(&tree);
    //..
    // {N : [R|B]} - N indicates the index of the node in 'nodes'
    //             - [R|B] indicates whether the node is Red or Black
    //
    //                          20{0:B}
    //                        /        \.
    //                       /          \.
    //                    10{1:R}        30{5:R}
    //                   /    \         /    \.
    //                5{2:B}  15{4:B} 25{6:B}  35{8:B}
    //                /                \.
    //               1{3:R}            27{7:R}
    //..

            struct {
               int d_line;
               int d_value;
               int d_lowerBoundIdx;
               int d_upperBoundIdx;
               int d_findIdx;
            } DATA[] = {
            //   LINE VALUE LOWER UPPER  FIND
            //   ---- ----- ----- -----  -----
                { L_,    0,     3,    3,  -1 },
                { L_,    1,     3,    2,   3 },
                { L_,    2,     2,    2,  -1 },
                { L_,    4,     2,    2,  -1 },
                { L_,    5,     2,    1,   2 },
                { L_,    6,     1,    1,  -1 },
                { L_,    9,     1,    1,  -1 },
                { L_,   10,     1,    4,   1 },
                { L_,   11,     4,    4,  -1 },
                { L_,   14,     4,    4,  -1 },
                { L_,   15,     4,    0,   4 },
                { L_,   16,     0,    0,  -1 },
                { L_,   19,     0,    0,  -1 },
                { L_,   20,     0,    6,   0 },
                { L_,   21,     6,    6,  -1 },
                { L_,   24,     6,    6,  -1 },
                { L_,   25,     6,    7,   6 },
                { L_,   26,     7,    7,  -1 },
                { L_,   27,     7,    5,   7 },
                { L_,   28,     5,    5,  -1 },
                { L_,   29,     5,    5,  -1 },
                { L_,   30,     5,    8,   5 },
                { L_,   31,     8,    8,  -1 },
                { L_,   34,     8,    8,  -1 },
                { L_,   35,     8,   -1,   8 },
                { L_,   36,    -1,   -1,  -1 },
            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            const IntNode *nodes = (IntNode *)tree.rootNode();
            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE  = DATA[i].d_line;
                const int VALUE = DATA[i].d_value;
                const RbTreeNode *lowerResult =
                    testNodeAtIndex(nodes, tree, DATA[i].d_lowerBoundIdx);
                const RbTreeNode *upperResult =
                    testNodeAtIndex(nodes, tree, DATA[i].d_upperBoundIdx);
                const RbTreeNode *findResult  =
                    testNodeAtIndex(nodes, tree, DATA[i].d_findIdx);

                IntNodeValueComparator comparator;
                if (veryVeryVerbose) {
                    P(LINE);
                }
                LOOP_ASSERT(LINE,
                            lowerResult ==
                            Obj::lowerBound(tree, comparator, VALUE));
                LOOP_ASSERT(LINE,
                            upperResult ==
                            Obj::upperBound(tree, comparator, VALUE));
                LOOP_ASSERT(LINE,
                            findResult == Obj::find(tree, comparator, VALUE));

            }
        }
        {
            if (veryVerbose) {
                printf("\tinsert every possible combination of [0..7]\n");
            }
            int       VALUES[]   = { 0, 1, 2, 3, 4, 5, 6, 7 };
            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);
            IntNodeComparator nodeComparator;

            do {
                RbTreeAnchor tree;
                IntNode  nodes[NUM_VALUES];
                ASSERT(0 <= validateIntRbTree(tree.rootNode()));
                for (int i = 0; i < NUM_VALUES; ++i) {
                    nodes[i].setLeftChild((RbTreeNode *)0xdeadbeef);
                    nodes[i].setRightChild((RbTreeNode *)0xdeadbeef);
                    nodes[i].setParent((RbTreeNode *)0xdeadc0de);
                    nodes[i].value()  = VALUES[i];

                    if (veryVeryVeryVerbose) {
                        printIntTree(tree);
                    }

                    Obj::insert(&tree, nodeComparator, &nodes[i]);

                    ASSERT(0 <= validateIntRbTree(tree.rootNode()));
                    ASSERT(i + 1 == tree.numNodes());
                    ASSERT(Obj::leftmost(tree.rootNode()) == tree.firstNode());
                }
            } while (std::next_permutation(VALUES,
                                           VALUES + NUM_VALUES,
                                           intLess));
        }
        {
            if (veryVerbose) {
                printf("\tinsert two possible shuffles of [0..1000]\n");
            }
            IntNodeComparator nodeComparator;
            enum {
                NUM_VALUES = 1000
            };
            int  VALUES[NUM_VALUES];
            for (int i = 0; i < NUM_VALUES; ++i) {
                VALUES[i] = i;
            }

            for (int i = 0; i < 2; ++i) {
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
                std::shuffle(VALUES,
                             VALUES + NUM_VALUES,
                             g_randomSource);
#else  // fall-back for C++03, potentially unsupported in C++17
                std::random_shuffle(VALUES, VALUES + NUM_VALUES);
#endif

                RbTreeAnchor tree;
                IntNode      nodes[NUM_VALUES];
                ASSERT(0 <= validateIntRbTree(tree.rootNode()));
                for (int j = 0; j < NUM_VALUES; ++j) {
                    nodes[j].value()  = VALUES[j];

                    Obj::insert(&tree, nodeComparator, &nodes[j]);

                    ASSERT(0 <= validateIntRbTree(tree.rootNode()));
                    ASSERT(j + 1 == tree.numNodes());
                    ASSERT(Obj::leftmost(tree.rootNode()) == tree.firstNode());
                }
            }
        }
        {
            if (veryVerbose) {
                printf("\tinsert series with duplicate values\n");
            }
            int       VALUES[]   = { 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3  };
            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

            IntNodeComparator nodeComparator;

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
            std::shuffle(VALUES, VALUES + NUM_VALUES, g_randomSource);
#else  // fall-back for C++03, potentially unsupported in C++17
            std::random_shuffle(VALUES, VALUES + NUM_VALUES);
#endif

            RbTreeAnchor tree;
            IntNode    nodes[NUM_VALUES];
            ASSERT(0 <= validateIntRbTree(tree.rootNode()));
            for (int i = 0; i < NUM_VALUES; ++i) {
                nodes[i].value()  = VALUES[i];

                Obj::insert(&tree, nodeComparator, &nodes[i]);

                ASSERT(0 <= validateIntRbTree(tree.rootNode()));
                ASSERT(i + 1 == tree.numNodes());
                ASSERT(Obj::leftmost(tree.rootNode()) == tree.firstNode());
            }
            if (veryVeryVeryVerbose) {
                printIntTree(tree);
            }
        }
        {
            if (veryVerbose) {
                printf("\tfindInsertLocation with a hint.\n");
            }

            const int NUM_VALUES = 20;
            IntNodeComparator      nodeComparator;
            IntNodeValueComparator valueComparator;
            RbTreeAnchor           tree;
            IntNode                nodes[NUM_VALUES];

            // Insert odd values, up to '2 * NUM_VALUES + 1' into a tree.
            for (int i = 0; i < NUM_VALUES; ++i) {
                nodes[i].value() = 2 * i + 1;
                Obj::insert(&tree, nodeComparator, &nodes[i]);
            }
            ASSERT(0 <= validateIntRbTree(tree.rootNode()));
            ASSERT(Obj::leftmost(tree.rootNode()) == tree.firstNode());
            if (veryVeryVeryVerbose) {
                printIntTree(tree);
            }

            // Test, using reasonable valid hints & 'upperBound'
            for (int i = 0; i < 2 * NUM_VALUES; ++i ) {
                const int VALUE = i;
                bool left;
                IntNode *hint = toIntNode(Obj::upperBound(tree,
                                                          valueComparator,
                                                          VALUE));
                IntNode *result =
                        toIntNode(Obj::findInsertLocation(&left,
                                                          &tree,
                                                          valueComparator,
                                                          VALUE,
                                                          hint));

                // This value should not be in the map
                ASSERTV(VALUE, left ? !result->leftChild()
                                    : !result->rightChild());
                int prevVal = prevIntNodeValue(tree, result);
                int nextVal = nextIntNodeValue(tree, result);
                ASSERTV(VALUE, prevVal <= VALUE);
                ASSERTV(VALUE, nextVal > VALUE);
            }
            // Test, using reasonable valid hints & 'lowerBound'
            for (int i = 0; i < 2 * NUM_VALUES; ++i ) {
                const int VALUE = i;
                bool left;
                IntNode *hint = toIntNode(Obj::upperBound(tree,
                                                          valueComparator,
                                                          VALUE));
                IntNode *result =
                        toIntNode(Obj::findInsertLocation(&left,
                                                          &tree,
                                                          valueComparator,
                                                          VALUE,
                                                          hint));

                // This value should not be in the map
                ASSERTV(VALUE, left ? !result->leftChild()
                                    : !result->rightChild());
                int prevVal = prevIntNodeValue(tree, result);
                int nextVal = nextIntNodeValue(tree, result);
                ASSERTV(VALUE, prevVal <= VALUE);
                ASSERTV(VALUE, nextVal > VALUE);
            }

            // Test, using random hints
            for (int i = 0; i < 2 * NUM_VALUES; ++i ) {
                for (int j = 0; j < NUM_VALUES; ++j) {
                    const int VALUE = i;
                    const int HINT_VALUE = 2 * j + 1;
                    bool left;
                    IntNode *hint = toIntNode(Obj::find(tree,
                                                        valueComparator,
                                                        HINT_VALUE));

                    ASSERT(hint); ASSERT(HINT_VALUE == hint->value());
                    IntNode *result =
                             toIntNode(Obj::findInsertLocation(&left,
                                                               &tree,
                                                               valueComparator,
                                                               VALUE,
                                                               hint));


                    ASSERTV(VALUE, HINT_VALUE, left
                                             ? !result->leftChild()
                                             : !result->rightChild());

                    int prevVal = prevIntNodeValue(tree, result);
                    int nextVal = nextIntNodeValue(tree, result);
                    ASSERTV(VALUE, HINT_VALUE, prevVal <= VALUE);
                    ASSERTV(VALUE, HINT_VALUE, nextVal >= VALUE);
                }
            }
        }
        {
            if (veryVerbose) {
                printf("\tfindUniqueInsertLocation with a hint.\n");
            }

            const int NUM_VALUES = 20;
            IntNodeComparator      nodeComparator;
            IntNodeValueComparator valueComparator;
            RbTreeAnchor           tree;
            IntNode                nodes[NUM_VALUES];

            // Insert odd values, up to '2 * NUM_VALUES + 1' into a tree.
            for (int i = 0; i < NUM_VALUES; ++i) {
                nodes[i].value() = 2 * i + 1;
                Obj::insert(&tree, nodeComparator, &nodes[i]);
            }
            ASSERT(0 <= validateIntRbTree(tree.rootNode()));
            ASSERT(Obj::leftmost(tree.rootNode()) == tree.firstNode());
            if (veryVeryVeryVerbose) {
                printIntTree(tree);
            }

            // Test, using reasonable valid hints.
            for (int i = 0; i < 2 * NUM_VALUES; ++i ) {
                const int VALUE = i;
                int comparisonResult;
                IntNode *hint = (IntNode *)Obj::lowerBound(tree,
                                                           valueComparator,
                                                           VALUE);
                IntNode *result =
                    (IntNode *)Obj::findUniqueInsertLocation(&comparisonResult,
                                                             &tree,
                                                             valueComparator,
                                                             VALUE,
                                                             hint);

                // This value should not be in the map
                if (0 == i % 2) {
                    ASSERTV(VALUE, 0 != comparisonResult);
                    ASSERTV(VALUE, comparisonResult < 0
                                   ? !result->leftChild()
                                   : !result->rightChild());
                    int prevVal = prevIntNodeValue(tree, result);
                    int nextVal = nextIntNodeValue(tree, result);
                    ASSERTV(VALUE, prevVal < VALUE);
                    ASSERTV(VALUE, nextVal > VALUE);
                }
                else {
                    // This value should be in the map.
                    ASSERTV(VALUE, result->value(), VALUE == result->value());
                    ASSERTV(VALUE, 0 == comparisonResult);
                }
            }

            // Test, using random hints
            for (int i = 0; i < 2 * NUM_VALUES; ++i ) {
                for (int j = 0; j < NUM_VALUES; ++j) {
                    const int VALUE = i;
                    const int HINT_VALUE = 2 * j + 1;
                    int comparisonResult;
                    IntNode *hint = (IntNode *)Obj::find(tree,
                                                         valueComparator,
                                                         HINT_VALUE);
                    ASSERT(hint); ASSERT(HINT_VALUE == hint->value());
                    IntNode *result =
                        (IntNode *)Obj::findUniqueInsertLocation(
                                                         &comparisonResult,
                                                         &tree,
                                                         valueComparator,
                                                         VALUE,
                                                         hint);

                    // This value should not be in the map
                    if (0 == i % 2) {
                        ASSERTV(VALUE, HINT_VALUE, 0 != comparisonResult);
                        ASSERTV(VALUE, HINT_VALUE, comparisonResult < 0
                                                   ? !result->leftChild()
                                                   : !result->rightChild());

                        int prevVal = prevIntNodeValue(tree, result);
                        int nextVal = nextIntNodeValue(tree, result);
                        ASSERTV(VALUE, HINT_VALUE, prevVal < VALUE);
                        ASSERTV(VALUE, HINT_VALUE, nextVal > VALUE);
                    }
                    else {
                        // This value should be in the map.
                        ASSERTV(VALUE, HINT_VALUE, result->value(),
                                VALUE == result->value());
                        ASSERTV(VALUE, HINT_VALUE, 0 == comparisonResult);
                    }
                }
            }
        }


        {
            if (veryVerbose) {
                printf("\terase every possible combination of [0..7]\n");
            }
            int       VALUES[]   = { 0, 1, 2, 3, 4, 5, 6, 7};
            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);
            IntNodeComparator nodeComparator;

            do {
                RbTreeAnchor tree;
                IntNode      nodes[NUM_VALUES];

                for (int i = 0; i < NUM_VALUES; ++i) {
                    nodes[i].setLeftChild(0);
                    nodes[i].setRightChild(0);
                    nodes[i].setParent(0);
                    nodes[i].value()  = i;

                    Obj::insert(&tree, nodeComparator, &nodes[i]);
                    ASSERT(0 <= validateIntRbTree(tree.rootNode()));
                }

                ASSERT(NUM_VALUES == tree.numNodes());
                ASSERT(Obj::leftmost(tree.rootNode()) == tree.firstNode());

                IntNodeValueComparator valueComparator;
                IntNodeComparator      nodeComparator;
                for (int i = 0; i < NUM_VALUES; ++i) {
                    if (veryVeryVeryVerbose) {
                        printf("===================\n");
                        printf("Remove %d\n", VALUES[i]);
                        printIntTree(tree);
                    }
                    RbTreeNode *node = Obj::find(tree,
                                                 valueComparator,
                                                 VALUES[i]);
                    ASSERT(0 != node);
                    ASSERT(VALUES[i] == static_cast<IntNode *>(node)->value());
                    Obj::remove(&tree, node);

                    if (veryVeryVeryVerbose) {
                        printIntTree(tree);

                    }
                    ASSERT(isValidSearchTree(tree.rootNode(),
                                             nodeComparator,
                                             &printIntNodeValue));
                    ASSERT(0 <= validateIntRbTree(tree.rootNode()));

                    ASSERT(NUM_VALUES - i - 1 == tree.numNodes());
                    if (i == NUM_VALUES - 1) {
                        ASSERT(tree.sentinel() == tree.firstNode());
                        ASSERT(0 == tree.rootNode());
                    }
                    else {
                        ASSERT(Obj::leftmost(tree.rootNode()) ==
                               tree.firstNode());
                    }
                }
                if (veryVeryVeryVerbose) {
                    printf("--------------------------------------------\n");
                }
            } while (std::next_permutation(VALUES,
                                           VALUES + NUM_VALUES,
                                           intLess));
        }
        {
            if (veryVerbose) {
                printf("\terase two possible shuffles of [0..1000]\n");
            }
            IntNodeValueComparator valueComparator;
            IntNodeComparator      nodeComparator;
            enum {
                NUM_VALUES = 1000
            };
            int  VALUES[NUM_VALUES];
            for (int i = 0; i < NUM_VALUES; ++i) {
                VALUES[i] = i;
            }

            for (int i = 0; i < 2; ++i) {
                RbTreeAnchor tree;
                IntNode      nodes[NUM_VALUES];
                ASSERT(0 <= validateIntRbTree(tree.rootNode()));
                for (int j = 0; j < NUM_VALUES; ++j) {
                    nodes[j].setLeftChild(0);
                    nodes[j].setRightChild(0);
                    nodes[j].setParent(0);
                    nodes[j].value()  = VALUES[j];

                    Obj::insert(&tree, nodeComparator, &nodes[j]);
                    ASSERT(0 <= validateIntRbTree(tree.rootNode()));
                }
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
                std::shuffle(VALUES,
                             VALUES + NUM_VALUES,
                             g_randomSource);
#else  // fall-back for C++03, potentially unsupported in C++17
                std::random_shuffle(VALUES, VALUES + NUM_VALUES);
#endif
                if (veryVeryVeryVerbose) {
                    printIntTree(tree);
                }
                for (int j = 0; j < NUM_VALUES; ++j) {

                    RbTreeNode *node = Obj::find(tree,
                                                 valueComparator,
                                                 VALUES[j]);
                    ASSERT(0 != node);
                    ASSERT(VALUES[j] == static_cast<IntNode *>(node)->value());

                    if (veryVeryVeryVerbose) {
                        printf("Removing: %d", VALUES[j]);
                    }
                    Obj::remove(&tree, node);

                    if (veryVeryVeryVerbose) {
                        printIntTree(tree);
                    }
                    ASSERT(isValidSearchTree(tree.rootNode(),
                                             nodeComparator,
                                             &printIntNodeValue));
                    ASSERT(0 <= validateIntRbTree(tree.rootNode()));
                    if (j == NUM_VALUES - 1) {
                        ASSERT(tree.sentinel() == tree.firstNode());
                        ASSERT(0 == tree.rootNode());
                    }
                    else {
                        ASSERT(Obj::leftmost(tree.rootNode()) ==
                               tree.firstNode());
                    }

                }
            }
        }
        {
            if (veryVerbose) {
                printf("\tDelete several example trees\n");
            }

            // Note the actual values in this tree are unimportant.
            const char *SPECS[] = {
                "{1:R}",
                "{1:R {1:B},.}",
                "{1:R {1:B},{1:B}}",
                "{1:R {1:B {1:R},.},{1:B}}",
                "{1:R {1:B {1:R},{1:B}},{1:B}}",
                "{1:R .,{1:B {1:R}, {1:B}}}",
                "{1:R {1:B {1:R},{1:R}},{1:B {1:R}, {1:B}}}",
            };
            const int NUM_SPECS = sizeof(SPECS) / sizeof(*SPECS);

            for (int i = 0; i < NUM_SPECS; ++i) {
                bslma::TestAllocator ta;

                RbTreeAnchor tree;
                Array<DeleteTestNode> testNodes(&ta);
                gg(&tree, &testNodes, SPECS[i], &deleteTestNodeAssign);

                if (veryVeryVerbose) {
                    P_(i); P(SPECS[i]);
                }
                if (veryVeryVeryVerbose) {
                    printIntTree(tree);
                }
                DeleteTestNodeFactory testDeleter;
                Obj::deleteTree(&tree, &testDeleter);
                for (int j = 0; j < testNodes.size(); ++j) {
                    ASSERT(testNodes[j].d_deleted);
                }
                ASSERTV(tree.sentinel() == tree.firstNode());
                ASSERTV(0               == tree.rootNode());
                ASSERTV(0               == tree.numNodes());
            }

        }
        {
            if (veryVerbose) {
                printf("\tcopy every possible combination of [0..7]\n");
            }
            int       VALUES[]   = { 0, 1, 2, 3, 4, 5, 6, 7};
            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);
            IntNodeComparator nodeComparator;

            do {
                RbTreeAnchor tree, copiedTree;
                IntNode      nodes[NUM_VALUES];
                IntNode      copiedNodes[NUM_VALUES];
                for (int i = 0; i < NUM_VALUES; ++i) {
                    nodes[i].setLeftChild(0);
                    nodes[i].setRightChild(0);
                    nodes[i].setParent(0);
                    nodes[i].value() = VALUES[i];

                    Obj::insert(&tree, nodeComparator, &nodes[i]);
                }
                IntNodeAllocator allocator(copiedNodes, NUM_VALUES);
                Obj::copyTree(&copiedTree, tree, &allocator);

                ASSERT(copiedTree.rootNode() >= copiedNodes &&
                       copiedTree.rootNode() <= copiedNodes + NUM_VALUES);
                ASSERT(areTreesEqual(tree.rootNode(),
                                     copiedTree.rootNode(),
                                     nodeComparator));
                ASSERT(tree.numNodes() == copiedTree.numNodes());

                ASSERT(0 <= validateIntRbTree(copiedTree.rootNode()));
                if (veryVeryVeryVerbose) {
                    printIntTree(tree);
                    printIntTree(copiedTree);
                }
            } while (std::next_permutation(VALUES,
                                           VALUES + NUM_VALUES,
                                           intLess));
        }

        {
            if (veryVerbose) {
                printf("\tswap a example trees\n");
            }

            const char *SPECS[] = {
                ".",
                "{1:B}",
                "{1:B ., {2:R}}",
                "{2:B {1:R},{3:R}}",
                "{2:B {1:B},{3:B .,{4:R}}}",
                "{2:B {1:B},{4:B {3:R},{5:R}}}",
            };
            const int NUM_SPECS = sizeof(SPECS) / sizeof(*SPECS);

            for (int i = 0; i < NUM_SPECS; ++i) {
                for (int j = 0; j < NUM_SPECS; ++j) {
                    if (veryVeryVeryVerbose) {
                        P(SPECS[i]);
                        P(SPECS[j]);
                    }

                    bslma::TestAllocator ta;
                    RbTreeAnchor treeA, treeB;
                    Array<IntNode> testNodesA(&ta), testNodesB(&ta);
                    gg(&treeA, &testNodesA, SPECS[i], &assignIntNodeValue);
                    gg(&treeB, &testNodesB, SPECS[j], &assignIntNodeValue);

                    RbTreeAnchor tmpA(treeA.rootNode(),
                                      treeA.firstNode(),
                                      treeA.numNodes());
                    RbTreeAnchor tmpB(treeB.rootNode(),
                                      treeB.firstNode(),
                                      treeB.numNodes());

                    Obj::swap(&treeA, &treeB);
                    ASSERT(tmpA.rootNode()  == treeB.rootNode());
                    if (treeB.rootNode()) {
                        ASSERT(tmpA.firstNode() == treeB.firstNode());
                    }
                    else {
                        ASSERT(treeB.sentinel() == treeB.firstNode());
                    }
                    ASSERT(tmpA.numNodes()  == treeB.numNodes());

                    ASSERT(tmpB.rootNode()  == treeA.rootNode());
                    if (treeA.rootNode()) {
                        ASSERT(tmpB.firstNode() == treeA.firstNode());
                    }
                    else {
                        ASSERT(treeA.sentinel() == treeA.firstNode());
                    }
                    ASSERT(tmpB.numNodes()  == treeA.numNodes());

                    ASSERT(0 <= validateIntRbTree(treeA.rootNode()));
                    ASSERT(0 <= validateIntRbTree(treeB.rootNode()));

                    if (treeA.rootNode()) {
                        ASSERT(treeA.sentinel() == treeA.rootNode()->parent());
                        RbTreeNode *itA = treeA.firstNode();
                        RbTreeNode *itExA = tmpB.firstNode();
                        while (treeA.sentinel() != itA) {
                            ASSERT(itA == itExA);
                            itA = Obj::next(itA);
                            itExA = Obj::next(itExA);
                        }
                    }
                    if (treeB.rootNode()) {
                        ASSERT(treeB.sentinel() == treeB.rootNode()->parent());
                        RbTreeNode *itB = treeB.firstNode();
                        RbTreeNode *itExB = tmpA.firstNode();
                        while (treeB.sentinel() != itB) {
                            ASSERT(itB == itExB);
                            itB = Obj::next(itB);
                            itExB = Obj::next(itExB);
                        }
                    }
                }

            }
        }
       } break;
      case -1: {
        // --------------------------------------------------------------------
        // Tree Generator:
        //
        //   The test-case code below can be used to generate tree
        //   specifications that are accepted by the 'gg' generator function,
        //   and was used to create several of the random trees in
        //   'TREE_VALUES'
        // --------------------------------------------------------------------
          IntNodeComparator nodeComparator;
          if (veryVerbose)
              printf("\tCreate each combination of 5 nodes with duplicates\n");

          int       VALUES[]   = { 2, 4, 4, 6, 6  };
          const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);
          do {

              RbTreeAnchor tree;
              IntNode  nodes[NUM_VALUES];

              for (int i = 0; i < NUM_VALUES; ++i) {
                  nodes[i].setLeftChild(0);
                  nodes[i].setRightChild(0);
                  nodes[i].setParent(0);
                  nodes[i].value() = VALUES[i];

                  Obj::insert(&tree, nodeComparator, &nodes[i]);
              }

              intNodeTreeToSpec(static_cast<IntNode *>(tree.rootNode()));
              printf("\n");

          } while (std::next_permutation(VALUES,
                                         VALUES + NUM_VALUES,
                                         intLess));


          if (veryVerbose) {
              printf("\tcreate random 10-15 nodes with duplicates\n");
          }
          srand(static_cast<unsigned int>(time(0)));
          for (int length = 10; length < 15; ++length) {
              int *values = new int[length];

              for (int j = 0; j < length; ++j) {
                  values[j] = ((j/3) + 1) *2;
              }
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
              std::shuffle(values, values + length, g_randomSource);
#else  // fall-back for C++03, potentially unsupported in C++17
              std::random_shuffle(values, values + length);
#endif

              bslma::TestAllocator ta;
              RbTreeAnchor         tree;
              Array<IntNode>       nodes(&ta);

              nodes.reset(length);

              for (int j = 0; j < length; ++j) {
                  nodes[j].setLeftChild(0);
                  nodes[j].setRightChild(0);
                  nodes[j].setParent(0);
                  nodes[j].value() = values[j];
                  Obj::insert(&tree, nodeComparator, &nodes[j]);
              }
              intNodeTreeToSpec(static_cast<IntNode *>(tree.rootNode()));
              printf("\n");
              delete [] values;

          }
      } break;

      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
