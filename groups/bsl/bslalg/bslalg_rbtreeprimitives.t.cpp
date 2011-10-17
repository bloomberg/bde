// bslag_rbtreeprimitives.t.cpp                                       -*-C++-*-
#include <bslalg_rbtreeprimitives.h>

#include <bslalg_arrayprimitives.h>
#include <bslalg_arraydestructionprimitives.h>
#include <bslalg_swaputil.h>
#include <bslma_allocator.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>

#include <algorithm>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

using namespace BloombergLP;
using namespace std;
using namespace bslalg;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// Global Concerns:
//: o Pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated.
//: o Precondition violations are detected in appropriate build modes.
//-----------------------------------------------------------------------------
// CLASS METHODS
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

static void aSsErT(bool b, const char *s, int i) {
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
# define LOOP_ASSERT(I,X) { \
    if (!(X)) { P_(I); aSsErT(!(X), #X, __LINE__); } }

# define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { P(I) P_(J);   \
                aSsErT(!(X), #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { P(I) P(J) P_(K) \
                aSsErT(!(X), #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define Q(X) printf("<| " #X " |>\n");      // Quote identifier literally.
#define P(X) dbg_print(#X " = ", X, "\n");  // Print identifier and value.
#define P_(X) dbg_print(#X " = ", X, ", "); // P(X) without '\n'
#define L_ __LINE__                         // current Line number
#define T_ putchar('\t');                   // Print a tab (w/o newline)

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
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

// Fundamental-type-specific print functions.
inline void dbg_print(bool b) { printf(b ? "true" : "false"); fflush(stdout); }
inline void dbg_print(char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(unsigned char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(signed char c) { printf("%c", c); fflush(stdout); }
inline void dbg_print(short val) { printf("%d", (int)val); fflush(stdout); }
inline void dbg_print(unsigned short val) {
    printf("%d", (int)val); fflush(stdout);
}
inline void dbg_print(int val) { printf("%d", val); fflush(stdout); }
inline void dbg_print(unsigned int val) { printf("%u", val); fflush(stdout); }
inline void dbg_print(long val) { printf("%ld", val); fflush(stdout); }
inline void dbg_print(unsigned long val) {
    printf("%lu", val); fflush(stdout);
}
inline void dbg_print(long long val) { printf("%lld", val); fflush(stdout); }
inline void dbg_print(unsigned long long val) {
    printf("%llu", val); fflush(stdout);
}
inline void dbg_print(float val) {
    printf("'%f'", (double)val); fflush(stdout);
}
inline void dbg_print(double val) { printf("'%f'", val); fflush(stdout); }
inline void dbg_print(long double val) {
    printf("'%Lf'", val); fflush(stdout);
}
inline void dbg_print(const char* s) { printf("\"%s\"", s); fflush(stdout); }
inline void dbg_print(char* s) { printf("\"%s\"", s); fflush(stdout); }
inline void dbg_print(void* p) { printf("%p", p); fflush(stdout); }

// Generic debug print function (3-arguments).
template <typename T>
void dbg_print(const char* s, const T& val, const char* nl) {
    printf("%s", s); dbg_print(val);
    printf("%s", nl);
    fflush(stdout);
}

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef RbTreePrimitives Obj;

const RbTreeNode::Color RED   = RbTreeNode::BSLALG_RED;
const RbTreeNode::Color BLACK = RbTreeNode::BSLALG_BLACK;

// ============================================================================
//                         GLOBAL CLASSES FOR TESTING
// ----------------------------------------------------------------------------

struct IntNode : RbTreeNode
{
    int d_value;
};

void printIntNodeValue(FILE *file, const RbTreeNode *node) 
{
    fprintf(file, "%d", static_cast<const IntNode *>(node)->d_value);
}

struct IntNodeComparator {
    typedef IntNode NodeType;
    bool operator()(const IntNode& lhs, const IntNode& rhs) const {
        return lhs.d_value < rhs.d_value;
    }
};

struct IntNodeValueComparator {
    typedef IntNode NodeType;

    bool operator()(const IntNode& node, int value) const {
        return node.d_value < value;
    }

    bool operator()(int value, const IntNode& node) const {
       return value < node.d_value;
    }
};

struct IntNodeCloner {
    typedef IntNode NodeType;

    IntNode     *d_nodes;
    mutable int  d_next;
    int          d_numNodes;

    IntNodeCloner(IntNode *nodes, int numNodes) 
    : d_nodes(nodes)
    , d_next(0)
    , d_numNodes(numNodes) 
    {}

    IntNode *operator()(const IntNode &original) const {        
        ASSERT(d_next < d_numNodes);
        IntNode *newNode = &d_nodes[d_next++];
        newNode->d_value =original.d_value;
        return newNode;
    }

    void operator()(IntNode *node) const {
        // Empty node removal function
    }
};

void assignIntNodeValue(IntNode *node, int value)
{
    node->d_value = value;
}


IntNode *createBreathingTestTree(IntNode *nodes)
    // Return a tree contructed from the specified array of 'nodes', of the
    // following structure:
    //..
    // {N : [R|B]} - N indicates the index of the node in 'nodes'
    //             - [R|B] indicates whether the node is Red or Black
    //
    //                          20{0:B}  
    //                        /        \
    //                       /          \
    //                    10{1:R}        30{5:R}
    //                   /    \         /    \
    //                5{2:B}  15{4:B} 25{6:B}  35{8:B}
    //                /                \
    //               1{3:R}            27{7:R}
    //..
    // The behehavior is undefined unless 'nodes' is a contiguous array of at
    // least 9 nodes.    
{
    memset(nodes, 0, sizeof(IntNode) * 9);

    nodes[0].setColor(BLACK);
    nodes[0].setLeftChild(&nodes[1]);
    nodes[0].setRightChild(&nodes[5]);
    nodes[0].setParent(0);
    nodes[0].d_value  = 20;

    nodes[1].setColor(RED);
    nodes[1].setLeftChild(&nodes[2]);
    nodes[1].setRightChild(&nodes[4]);
    nodes[1].setParent(&nodes[0]);
    nodes[1].d_value  = 10;

    nodes[2].setColor(BLACK);
    nodes[2].setLeftChild(&nodes[3]);
    nodes[2].setRightChild(0);        
    nodes[2].setParent(&nodes[1]);
    nodes[2].d_value  = 5;

    nodes[3].setColor(RED);
    nodes[3].setLeftChild(0);
    nodes[3].setRightChild(0);
    nodes[3].setParent(&nodes[2]);
    nodes[3].d_value  = 1;        

    nodes[4].setColor(BLACK);
    nodes[4].setLeftChild(0);
    nodes[4].setRightChild(0);        
    nodes[4].setParent(&nodes[1]);
    nodes[4].d_value  = 15;        

    nodes[5].setColor(RED);
    nodes[5].setLeftChild(&nodes[6]);
    nodes[5].setRightChild(&nodes[8]);
    nodes[5].setParent(&nodes[0]);
    nodes[5].d_value  = 30;        

    nodes[6].setColor(BLACK);
    nodes[6].setLeftChild(0);
    nodes[6].setRightChild(&nodes[7]);
    nodes[6].setParent(&nodes[5]);
    nodes[6].d_value  = 25;        

    nodes[7].setColor(RED);
    nodes[7].setLeftChild(0);
    nodes[7].setRightChild(0);
    nodes[7].setParent(&nodes[6]);
    nodes[7].d_value  = 27;      

    nodes[8].setColor(BLACK);
    nodes[8].setLeftChild(0);
    nodes[8].setRightChild(0);
    nodes[8].setParent(&nodes[5]);
    nodes[8].d_value  = 35;      

    return nodes;
}

bool intLess(int lhs, int rhs)
{
    return lhs < rhs;
}

inline
IntNode *testNodeAtIndex(IntNode *nodes, int nodeIndex)
   // Return the node at the specified 'index' in the specified array of
   // 'nodes' in 'nodeIndex >= 0', and 0 otherwise.
{
    return nodeIndex >= 0 ? &nodes[nodeIndex] : 0;
}

template <typename NODE_TYPE, 
          typename NODE_COMPARATOR, 
          typename NODE_PRINT_FUNC>
int validateTestRbTree(const NODE_TYPE        *node, 
                       const NODE_COMPARATOR&  comparator,
                       const NODE_PRINT_FUNC&  nodePrintFunction) 
     // Return the value of 'RbTreePrimitives::validateRbTree' for the
     // specified 'node' using the specified 'comparator', and the specified
     // 'nodePrintFunction' to record the nodes value.  Note that this method
     // is provided to record additional error information regarding an
     // invalid tree. 
{
    const char       *errorMessage;
    const RbTreeNode *errorNode;

    int ret = Obj::validateRbTree(&errorNode, 
                                  &errorMessage, 
                                  node, 
                                  comparator);
    if (ret < 0) {
        P_(errorMessage);
        printf(" ");
        nodePrintFunction(stdout, node);
        printf("\n");
    }
    return ret;
}

int validateIntRbTree(const RbTreeNode *node)
    // Return the value of 'RbTreePrimitives::validateRbTree' and log any
    // error to the console.  Note that this method is provided to record
    // additional error information regarding an invalid tree.
{
    IntNodeComparator nodeComparator;
    return validateTestRbTree(node, nodeComparator, printIntNodeValue);
}

template <typename NODE>
class RbTreeNodeArray {
    // This class provides an array of objects of the parameterized 'NODE'
    // type.  The size of the array is initialized by a call to 'reset'.  Note
    // that this provides a replacement for 'vector', which cannot be used
    // below 'bslstl', but is considerably simpler to implement because it is
    // no exception safe and does not grow dynamically.

    // DATA
    NODE            *d_data_p;
    int              d_size;
    bslma_Allocator *d_allocator_p;

    RbTreeNodeArray(const RbTreeNodeArray&);
    RbTreeNodeArray& operator=(const RbTreeNodeArray&);

  public:

    // CREATORS
    RbTreeNodeArray(bslma_Allocator *allocator)
    : d_data_p(0)
    , d_size(0)
    , d_allocator_p(allocator)
    {
    }

    ~RbTreeNodeArray()
    { 
        clear();
    }

    // MANIPULATORS
    void clear() {
        if (d_data_p) {
            bslalg_ArrayDestructionPrimitives::destroy(d_data_p,
                                                       d_data_p + d_size);
            d_allocator_p->deallocate(d_data_p);
        }        
    }

    void reset(int size) {
        clear();
        if (0 != size) {
            d_data_p = (NODE *)d_allocator_p->allocate(size * sizeof(NODE));
            bslalg_ArrayPrimitives::defaultConstruct(d_data_p, 
                                                     size, 
                                                     d_allocator_p);
        }
        d_size = size;
    }

    NODE& operator[](int offset) {
        return d_data_p[offset];
    }

    NODE *data() {
        return d_data_p;
    }
        
    int size() const {
        return d_size;
    }
};


struct DeleteTestNode : public RbTreeNode {
   // This 'struct' provides a test node type used to verify a deleter
   // has been called on a node.

   // DATA
   bool d_deleted;  // whether the deleter has been called on this node.

   DeleteTestNode() : d_deleted(false) {}
};

void printDeleteTestNode(FILE *file, const RbTreeNode *node) 
    // Write the value of 'node' to the specified 'file'.
{
    fprintf(file, 
            "%d", 
            static_cast<const DeleteTestNode *>(node)->d_deleted);
}

void deleteTestNodeAssign(DeleteTestNode *node, int n)
    // Do nothing.  Note that this signature matches that expected by the 'gg'
    // generator function, but 'node' has no value to assign.
{
}

struct DeleteTestNodeFunctor {
    typedef DeleteTestNode NodeType;

    void operator()(DeleteTestNode *node) const
        // Mark the specified 'node' as deleted, and set its left, right, and
        // parent pointers to invalid pointer values.
    {
        DeleteTestNode *removedNode = static_cast<DeleteTestNode *>(node);
        ASSERT(false == removedNode->d_deleted);
        removedNode->d_deleted = true;
        removedNode->setParent(reinterpret_cast<RbTreeNode *>(0xdeadbeef));
        removedNode->setLeftChild(reinterpret_cast<RbTreeNode *>(0xdeadbeef));
        removedNode->setRightChild(reinterpret_cast<RbTreeNode *>(0xdeadbeef));
    }
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
    typedef typename NODE_COMPARATOR::NodeType NodeType;
    if (0 == rootNode) {
        return true;                                                  // RETURN
    }

    const NodeType *node  = static_cast<const NodeType *>(rootNode);
    const NodeType *left  = static_cast<const NodeType *>(node->leftChild());
    const NodeType *right = static_cast<const NodeType *>(node->rightChild());
 
    if ((left != 0 || right != 0) && left == right) {
        printf("Two equal children: ");
        nodePrinter(stdout, node);
        printf("\n");
        return false;                                                 // RETURN
    }
    if ((left  && left->parent()  != node) ||
        (right && right->parent() != node)) {
        printf("Problem with childrens parent pointer: ");
        nodePrinter(stdout, node);
        printf("\n");
        return false;                                                 // RETURN
    }

    if ((left  && comparator(*node, *left)) || 
        (right && comparator(*right, *node))) {
        printf("Invalid search tree: %d\n ");
        nodePrinter(stdout, node);
        printf("\n");

        return false;                                                 // RETURN
    }

    return isValidSearchTree(node->leftChild(),  comparator, nodePrinter) 
         ? isValidSearchTree(node->rightChild(), comparator, nodePrinter)
         : false;
}

template <class NODE_COMPARATOR>
bool areTreesEqual(const RbTreeNode       *leftNode,
                   const RbTreeNode       *rightNode,
                   const NODE_COMPARATOR&  comparator)
    // Return 'true' if the trees rooted at the specified 'left' and
    // 'specified' right compare equal using the specified 'nodeComparator'.
    // The behavior is undefined unless 'nodeComparator' provides a weak
    // ordering on the elements of both 'left' and 'right' trees.  Note that
    // this implementation uses recusion for simplicity.
{
    typedef typename NODE_COMPARATOR::NodeType NodeType;

    const NodeType *left = static_cast<const NodeType *>(leftNode);
    const NodeType *right = static_cast<const NodeType *>(rightNode);

    if (0 == left && 0 == right) {
        return true;
    }
    if (0 == left || 0 == right) {
        return false;
    }
    return (!comparator(*left, *right) && !comparator(*right, *left))
        && areTreesEqual(left->leftChild(), right->leftChild(), comparator)
        && areTreesEqual(left->rightChild(), right->rightChild(), comparator);
}

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
//                                         /    \
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


template <class NODE_TYPE>
const char *ggParseNode(
               NODE_TYPE                              **nextNode, 
               const char                              *spec,
               void (*nodeAssignValueFunc)(NODE_TYPE *, int value))
    // Populate the specified 'nextNode' with the first node described in the
    // specified 'spec' and advance 'nextNode' to the next free node in the
    // sequence of nodes after parsing the rest of the sub-tree described by
    // the first node in 'spec'.  Return the first characted in 'spec' that
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
 
    nodeAssignValueFunc(newNode, atoi(currentChar));
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
        return currentChar + 1;
    }

    if ('.' == *currentChar) {
        ++currentChar;
        newNode->setLeftChild(0);
    }
    else {
        newNode->setLeftChild(*nextNode);
        currentChar = ggParseNode(nextNode, currentChar, nodeAssignValueFunc);
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
        currentChar = ggParseNode(nextNode, currentChar, nodeAssignValueFunc);
        newNode->rightChild()->setParent(newNode);
    }
    currentChar = ltrim(currentChar);

    PARSE_ASSERT('}' == *currentChar, spec);
    return currentChar + 1;
}

template <class NODE_TYPE>
NODE_TYPE *gg(RbTreeNodeArray<NODE_TYPE>              *nodes, 
              const char                              *spec,
               void (*nodeAssignValueFunc)(NODE_TYPE *, int value))
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
        return 0;
    }

    nodes->reset(numNodes);
    
    spec = ltrim(spec);

    NODE_TYPE *nextNode = nodes->data();
    spec = ggParseNode(&nextNode, spec, nodeAssignValueFunc);
    
    PARSE_ASSERT(!*ltrim(spec), spec);
           
    return nodes->data();
}


//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Example 2: Implementing a Set of Integers
///- - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to implement a simple container holding a set
// of (unique) integer values using a red-black binary search tree through
// using 'RbTreePrimitives'.
//
// Before defining the 'IntSet' class itself, we need to define a series of
// associated helper types:
//: 1 The node-type, for the nodes in the tree.
//: 2 An iterator, for iterating over nodes in the tree.
//: 3 A comparison functor for comparing nodes and values.
//: 4 A functor for creating nodes.
//: 5 A functor for destroying and deallocating nodes.
//
// First, we define a type 'IntSet_Node' that will represent the nodes in our
// tree of integer values.  'IntSet_Node' contains an 'int' payload, and
// inherits from 'RbTreeNode' allowing it to be operated on by
// 'RbTreePrimitives'.  Note that the underscore ("_") indicates that
// 'IntSet_Node' is private implementation type of 'IntSet', and not for use
// by clients of 'IntSet':
//..
    struct IntSet_Node : public RbTreeNode {
        // A red-black tree node containing an integer data-value.
//
        int d_value;  // actual value represented by the node
    };
//..
// Then, we define a iterator over 'IntSet_Node' objects.  We use the 'next'
// function of 'RbTreePrimitives' to increment the iterator.  Note that, for
// simplicity, this iterator is *not* a fully STL compliant iterator
// implementation:
//..
    class IntSetConstIterator {
        // This class defines an STL-style iterator over a non-modifiable tree
        // of 'IntSet_Node' objects.
//
        // DATA
        const IntSet_Node *d_node_p;  // current location of this iterator
//
      public:
//
        IntSetConstIterator() : d_node_p(0) {}
            // Create an iterator that does not refer to a node, and will
            // compare equal to 'end' iterator of a 'IntSet'.
//
        IntSetConstIterator(const IntSet_Node *node) : d_node_p(node) {}
            // Create an iterator refering to the specified 'node'.
//
    //  IntSetConstIterator(const IntSetConstIterator&) = default;
//
        // MANIPULATOR
    //  IntSetConstIterator& operator=(const IntSetConstIterator&) = default;
//
//..
// We implement the prefix-increment operator using the 'next' function of
// 'RbTreePrimitives: 
//..
        IntSetConstIterator& operator++()
           // Advance this iterator to the subsequent value it the 'IntSet',
           // and return a reference providing modifiable access to this
           // iterator.   The behavior is undefined unless this iterator
           // refers to a element in an 'IntSet'.
        {
            d_node_p = static_cast<const IntSet_Node *>(
                                             RbTreePrimitives::next(d_node_p));
            return *this;
        }
//
        // ACCESSORS
        const int& operator*() const { return d_node_p->d_value; }
            // Return a reference providing non-modifiable access to the value
            // refered to by this iterator.
//
        const int *operator->() const { return &d_node_p->d_value; }
            // Return an address providing non-modifiable access to the value
            // refered to by this iterator.
//
        const IntSet_Node *nodePtr() const { return d_node_p; }
            // Return the address of the non-modifiable int-set node refered
            // to by this iterator
    };
//
    // FREE OPERATORS
    bool operator==(const IntSetConstIterator &lhs,
                    const IntSetConstIterator &rhs)
        // Return 'true' if the 'lhs and 'rhs' objects have the same value, and
        // 'faluse' otherwise.  Two 'IntSetConstIterator' objects have the
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
// Next, we define a comparison functor for 'IntSet_Node' objects.  This
// comparator is supplied to 'RbTreePrimitives' functions that must compare
// nodes with values -- i.e., those with a 'NODE_VALUE_COMPARATOR' template
// parameter (e.g., 'find' and 'findInsertLocation'):
//..
    struct IntSet_NodeValueComparator {
        // This class defines a comparator providing comparison operations
        // between 'IntSet_Node' objects, and 'int' values.
//
        typedef IntSet_Node NodeType;  // Alias for the type of node (required
                                       // by RbTreeNodePrimitives)
//..
// Notice that we define a type 'NodeType' for our comparison function.  The
// 'RbTreePrimitives' operations taking a template parameter
// 'NODE_VALUE_COMPARATOR'  static-cast 'RBTReeNode' objects to
// 'NODE_VALUE_COMPARATOR::NodeType' to supply them to the comparison
// operations.
//..
        bool operator()(const IntSet_Node& lhs, int rhs) const
        {
            return lhs.d_value < rhs;
        }
//
        bool operator()(int lhs, const IntSet_Node& rhs) const
        {
            return lhs < rhs.d_value;
        }
    };
//..
// Next, we define a functor for creating 'IntSet_Node' objects.  This functor
// provides two 'operator()' overloads.  The first 'operator()' overload,
// which copies an 'IntSet_Node' is required by 'RbTreePrimitives' operations
// with a 'NODE_COPY_FUNCTION' template parameter (e.g., 'copyTree').  The
// second 'operator()' overload isn't used by 'RbTreePrimitives', but is
// provided here to centralize all node creation logic for 'IntSet' in
// 'IntSet_NodeCreator'.
//..
    class IntSet_NodeCreator {
        // This class defines a creator object, that when invoked, creates a
        // new 'IntSet_Node' (either from a int value, or an existing
        // 'IntSet_Node' object) using the allocator supplied at construction.
//
        bslma_Allocator *d_allocator_p;  // allocator, (held, not owned)
//
      public:
        typedef IntSet_Node NodeType;
//
        IntSet_NodeCreator(bslma_Allocator *allocator)
        : d_allocator_p(allocator)
        {
            BSLS_ASSERT_SAFE(allocator);
        }
//
//
        IntSet_Node *operator()(const IntSet_Node& node) const
        {
            IntSet_Node *newNode = new (*d_allocator_p) IntSet_Node;
            newNode->d_value = node.d_value;
            return newNode;
        }
//
        IntSet_Node *operator()(int value) const
        {
            IntSet_Node *newNode = new (*d_allocator_p) IntSet_Node;
            newNode->d_value = value;
            return newNode;
        }
    };
//..
// Then, we define a functor for destroying 'IntSet_Node' objects, and
// deallocating their memory footprint.  This functor is required by
// 'RbTreePrimitives' operations with a 'NODE_DELETE_FUNCTION' template
// parameter (e.g., 'deleteTree' and 'copyTree'):
//..
    class IntSet_NodeDeleter {
        // This class defines a deleter object that, when invoked, destroys
        // the supplied 'IntSet_Node' and deallocates its footprint using the
        // allocator supplied at construction.
//
        // DATA
        bslma_Allocator *d_allocator_p;  // allocator, (held, not owned)
//
      public:
//
        // PUBLIC TYPES
        typedef IntSet_Node NodeType;
//
        IntSet_NodeDeleter(bslma_Allocator *allocator)
        : d_allocator_p(allocator)
        {
            BSLS_ASSERT_SAFE(allocator);
        }
//
        void operator()(IntSet_Node *node) const
        {
            d_allocator_p->deleteObject(node);
        }
    };
//..
// Next, having defined the requesite helper types, we define the public
// interface for our 'IntSet' type.
//
// Note that for the purposes of illustrating the use of 'RbTreePrimitives'
// a number of simplifications have been made.  For example, this
// implementation provides only a minimal set of criticial operations, it does
// not maintain a pointer to the last node in the tree (as it also does not
// expose a reverse iterator), and it does not use the empty base-class
// optimization for the comparator, etc.
//..
    class IntSet {
        // This class implements a set of (unique) 'int' values.
//
        // DATA
        IntSet_Node           *d_root_p;       // root of the tree

        IntSet_Node           *d_first_p;      // left-most node in the tree

        IntSet_NodeValueComparator  
                               d_comparator;   // comparison functor for ints

        int                    d_numElements;  // size of the set

        bslma_Allocator       *d_allocator_p;  // allocator (held, not owned)
//
        // FRIENDS
        friend bool operator==(const IntSet& lhs, const IntSet& rhs);
//
      private:
//
        // PRIVATE CLASS METHODS
        static IntSet_Node *toNode(RbTreeNode *node);
        static const IntSet_Node *toNode(const RbTreeNode *node);
            // Return the 'IntSet_Node' refered to by the specified node.  The
            // behavior is undefined unless 'node' refers to a 'IntSet_Node'
            // object.  Note that this a convenience function for applying a
            // 'static_cast' to the 'RbTreeNode' return value for
            // 'RbTreePrimitives' operation.
//
      public:
//
        // PUBLIC TYPES
        typedef IntSetConstIterator const_iterator;
//
        // CREATORS
        IntSet(bslma_Allocator *basicAllocator = 0);
            // Create a empty 'IntSet'.  Optionally specify a 'basicAllocator'
            // used to supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.
//
        IntSet(const IntSet& original, bslma_Allocator *basicAllocator = 0);
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
            // insert it into this set, returning an iterator refering to the
            // newly added value, and return an iterator refering to the
            // existing instance of 'value' in this set otherwise.
//
        const_iterator erase(const_iterator iterator);
            // Remove the value refered to by the specified 'iterator' from
            // this set, and return an iterator refering to the value
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
            // Return an iterator referring lowest value in this set, or
            // 'end()' if this set is empty. 
//
        const_iterator end() const;
            // Return an iterator refering to the value one past the highest
            // value in this set.
//
        const_iterator find(int value) const;
            // Return a iterator refering to the specified 'value' in this
            // set, or 'end()' if 'value' is not a member of this set.
//
        int size() const;
            // Return the number of elements in this set.
    };
//
    // FREE OPERATORS
    bool operator==(const IntSet& lhs, const IntSet& rhs);
        // Return 'true' if the 'lhs and 'rhs' objects have the same value, and
        // 'false' otherwise.  Two 'IntSet' objects have the same value if
        // they contain the same number of elements, and if for each element
        // in 'lhs' there is a corresponding element in 'rhs' with the same
        // value.
//
    bool operator!=(const IntSet& lhs, const IntSet& rhs);
        // Return 'true' if the specified 'lhs' and 'rhs' objects do not have
        // the same value, and 'false' otherwise.  Two 'IntSet' objects do not
        // have the same value if they differ in the number of elements they
        // contain, of if for any element in 'lhs' there is not a
        // corresponding element in 'rhs' with the same value.
//..
// Now, we implement the methods of 'IntSet' using 'RbTreePrimitives' and the
// helper types we defined earlier:
//..
    // PRIVATE CLASS METHODS
    IntSet_Node *IntSet::toNode(RbTreeNode *node)
    {
        return static_cast<IntSet_Node *>(node);
    }
//
    const IntSet_Node *IntSet::toNode(const RbTreeNode *node) {
        return static_cast<const IntSet_Node *>(node);
    }
//
    // CREATORS
    IntSet::IntSet(bslma_Allocator *basicAllocator)
    : d_root_p(0)
    , d_first_p(0)
    , d_comparator()
    , d_numElements(0)
    , d_allocator_p(bslma_Default::allocator(basicAllocator))
    {
    }
//
    IntSet::IntSet(const IntSet& original, bslma_Allocator *basicAllocator)
    : d_root_p(0)
    , d_first_p(0)
    , d_comparator()
    , d_numElements(0)
    , d_allocator_p(bslma_Default::allocator(basicAllocator))
    {
        if (original.d_root_p) {
            // In order to copy the tree, we create an instance of our
            // node-creation functor, which we supply to 'copyTree'.
//
            IntSet_NodeCreator creator(d_allocator_p);
            IntSet_NodeDeleter deleter(d_allocator_p);
            d_root_p  = toNode(RbTreePrimitives::copyTree(original.d_root_p,
                                                          creator,
                                                          deleter));
            d_first_p = toNode(RbTreePrimitives::minimum(d_root_p));
            d_numElements = original.d_numElements;
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
        IntSet temp(rhs, d_allocator_p);
        swap(temp);
    }
//
    IntSet::const_iterator IntSet::insert(int value)
    {
//..
// To insert a value into the tree, we first find the location where the node
// would be added, and whether 'value' is unique.  If 'value' is not unique
// we do not want to incur the expense of allocating memory for a node:
//..
        bool isUnique, isLeftChild;
        RbTreeNode *insertLocation =
                       RbTreePrimitives::findUniqueInsertLocation(&isUnique,
                                                                  &isLeftChild,
                                                                  d_root_p,
                                                                  d_comparator,
                                                                  value);
        if (!isUnique) {
            return const_iterator(toNode(insertLocation));            // RETURN
        }
//..
// If 'value' is unique, we create a new node, and supply it to 'insertAt'
// along with the tree location returned by 'findUniqueInsertLocation.
//..
//
        IntSet_NodeCreator creator(d_allocator_p);
        IntSet_Node *newNode = creator(value);
        d_root_p = toNode(RbTreePrimitives::insertAt(d_root_p,
                                                     insertLocation,
                                                     isLeftChild,
                                                     newNode));
        // We may also need to update 'd_first_p', if the new node was
        // inserted as the first node in the tree.
//
        if (!insertLocation || d_first_p == insertLocation && isLeftChild) {
            BSLS_ASSERT(newNode == RbTreePrimitives::minimum(d_root_p));
            d_first_p = newNode;
        }
        ++d_numElements;
        return const_iterator(toNode(newNode));
    }
//
    IntSet::const_iterator IntSet::erase(const_iterator iterator)
    {
        BSLS_ASSERT(0 != iterator.nodePtr());
        IntSet_Node *node = const_cast<IntSet_Node *>(iterator.nodePtr());
//
        // Before removing the node, we first find the subsequent node to which
        // we will return an iterator.
//
        IntSet_Node *next = toNode(RbTreePrimitives::next(node));
        if (d_first_p == node) {
            d_first_p = next;
        }
        d_root_p = toNode(RbTreePrimitives::remove(d_root_p, node));
        IntSet_NodeDeleter deleter(d_allocator_p);
        deleter(node);
        --d_numElements;
        return const_iterator(next);
    }
//
    void IntSet::clear()
    {
        // Create an instance of the node-deletion functor, which we will
        // supply to 'deleteTree'.
//
        if (0 != d_root_p) {
            IntSet_NodeDeleter deleter(d_allocator_p);
            RbTreePrimitives::deleteTree(d_root_p, deleter);
//
            d_root_p      = 0;
            d_first_p     = 0;
            d_numElements = 0;
        }
    }
//
    void IntSet::swap(IntSet& other) {
        BSLS_ASSERT(d_allocator_p == other.d_allocator_p);
        bslalg_SwapUtil::swap(d_root_p, other.d_root_p);
        bslalg_SwapUtil::swap(d_root_p, other.d_first_p);
    }
//
    // ACCESSORS
    IntSet::const_iterator IntSet::begin() const
    {
        return const_iterator(d_first_p);
    }
//
    IntSet::const_iterator IntSet::end() const
    {
        return const_iterator(0);
    }
//
    IntSet::const_iterator IntSet::find(int value) const
    {
        const RbTreeNode *node = RbTreePrimitives::find(d_root_p,
                                                        d_comparator,
                                                        value);
        return const_iterator(toNode(node));
    }
//
    int IntSet::size() const
    {
        return d_numElements;
    }
//..
// Finally, we implement the free operators on 'IntSet':
//..
    // FREE OPERATORS
    bool operator==(const IntSet& lhs, const IntSet& rhs)
    {
        if (lhs.size() != rhs.size()) {
            return false;                                             // RETURN
        }
        IntSet::const_iterator lIt = lhs.begin();
        IntSet::const_iterator rIt = rhs.begin();
        for (; lIt != lhs.end(); ++lIt, ++rIt) {
            if (*lIt != *rIt) {
                return false;                                         // RETURN
            }
        }
        return true;
    }
//
    bool operator!=(const IntSet& lhs, const IntSet& rhs)
    {
        return !(lhs == rhs);
    }
//..

const IntSet_Node *rootIntSet(const IntSet& intSet)
{
    const RbTreeNode *root = intSet.begin().nodePtr();
    if (root) {
        while (root->parent()) { 
            root = root->parent(); 
        }
    }
    return static_cast<const IntSet_Node *>(root);
}

struct IntSet_NodeComparator {
        // This class defines a comparator providing comparison operations
        // between 'IntSet_Node' objects.
//
    typedef IntSet_Node NodeType;  
    bool operator()(const IntSet_Node& lhs, const IntSet_Node& rhs) const
    {
        return lhs.d_value < rhs.d_value;
    }
};


void printIntSetNode(FILE *file, const RbTreeNode *node)
{
    fprintf(file, "%d", static_cast<const IntSet_Node *>(node)->d_value);
}

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 2: {

          if (verbose) printf("\nUSAGE EXAMPLE"
                              "\n=============\n");

          bslma_TestAllocator ta;
          IntSet_NodeComparator comparator;
          const int VALUES[]   = { 8, 10, 2, 1, 3, 7, 9, 4, 5, 6 };
          const int NUM_VALUES = sizeof(VALUES) / sizeof (*VALUES);
          
          IntSet x(&ta); const IntSet& X = x;
          for (int i = 0; i < NUM_VALUES; ++i) {
              IntSet y(X, &ta); const IntSet &Y = y;

              ASSERT(  X == Y);
              ASSERT(!(X != Y));

              ASSERT(X.end() == X.find(VALUES[i]));
              x.insert(VALUES[i]);
              x.insert(VALUES[i]);
              ASSERT(VALUES[i] == *X.find(VALUES[i]));

              const RbTreeNode *root = rootIntSet(X);
              ASSERT(0 <= RbTreePrimitives::validateRbTree(root, comparator));
              ASSERT(i + 1 == X.size());

              ASSERT(  X != Y);
              ASSERT(!(X == Y));
          }

          int i = 1;
          for (IntSet::const_iterator it = X.begin(); it != X.end(); ++it) {
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
          
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Plan:
        //   Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (veryVerbose) {
            printf("\tEmpty tree\n");
        }
        {
            IntNode node;
            node.setLeftChild(0);
            node.setRightChild(0);
            node.setParent(0);
            node.setColor(BLACK);
            node.d_value  = 0 ;
            
            ASSERT(&node == Obj::minimum(&node));
            ASSERT(&node == Obj::maximum(&node));
            ASSERT(0     == Obj::next(&node));
            ASSERT(0     == Obj::previous(&node));            
        }
        if (veryVerbose) {
            printf("\tNavigation operations\n");
        }
        {
            IntNode nodes[9];
            createBreathingTestTree(nodes);
            if (veryVeryVerbose) {
                Obj::printTree(stdout, nodes, printIntNodeValue, 0, 3);
            }
    //..
    // {N : [R|B]} - N indicates the index of the node in 'nodes'
    //             - [R|B] indicates whether the node is Red or Black
    //
    //                          20{0:B}  
    //                        /        \
    //                       /          \
    //                    10{1:R}        30{5:R}
    //                   /    \         /    \
    //                5{2:B}  15{4:B} 25{6:B}  35{8:B}
    //                /                \
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

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE = DATA[i].d_line;
                IntNode *node = &nodes[i];
                IntNode *minResult  = 
                                    testNodeAtIndex(nodes, DATA[i].d_minIndex);
                IntNode *maxResult  = 
                                    testNodeAtIndex(nodes, DATA[i].d_maxIndex);
                IntNode *nextResult = 
                                   testNodeAtIndex(nodes, DATA[i].d_nextIndex);
                IntNode *prevResult = 
                                   testNodeAtIndex(nodes, DATA[i].d_prevIndex);

                if (veryVeryVerbose) {
                    P(LINE);
                }

                LOOP_ASSERT(LINE, minResult == Obj::minimum(node));
                LOOP_ASSERT(LINE, maxResult == Obj::maximum(node));
                LOOP_ASSERT(LINE, nextResult == Obj::next(node));
                LOOP_ASSERT(LINE, prevResult == Obj::previous(node));
            }
        }
        if (veryVerbose) {
            printf("\tlowerBound and upperBound\n");
        }
        {
            IntNode nodes[9];
            createBreathingTestTree(nodes);
    //..
    // {N : [R|B]} - N indicates the index of the node in 'nodes'
    //             - [R|B] indicates whether the node is Red or Black
    //
    //                          20{0:B}  
    //                        /        \
    //                       /          \
    //                    10{1:R}        30{5:R}
    //                   /    \         /    \
    //                5{2:B}  15{4:B} 25{6:B}  35{8:B}
    //                /                \
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

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE  = DATA[i].d_line;
                const int VALUE = DATA[i].d_value;
                IntNode *lowerResult = 
                               testNodeAtIndex(nodes, DATA[i].d_lowerBoundIdx);
                IntNode *upperResult =
                               testNodeAtIndex(nodes, DATA[i].d_upperBoundIdx);
                IntNode *findResult  =
                               testNodeAtIndex(nodes, DATA[i].d_findIdx);

                IntNode *ROOT = &nodes[0];
                IntNodeValueComparator comparator;
                if (veryVeryVerbose) {
                    P(LINE);
                }
                LOOP_ASSERT(LINE, 
                            lowerResult == 
                            Obj::lowerBound(ROOT, comparator, VALUE));
                LOOP_ASSERT(LINE, 
                            upperResult == 
                            Obj::upperBound(ROOT, comparator, VALUE));
                LOOP_ASSERT(LINE, 
                            findResult == Obj::find(ROOT, comparator, VALUE));

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
    //                        /        \
    //                       /          \
    //                    10{1:R}        30{5:R}
    //                   /    \         /    \
    //                5{2:B}  15{4:B} 25{6:B}  35{8:B}
    //                /                \
    //               1{3:R}            27{7:R}
    //..

            IntNode nodes[9];
            createBreathingTestTree(nodes);

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
            nodes[4].d_value = 21;
            ASSERT(0 > Obj::validateRbTree(&en, &ed, &nodes[0], comp));
            ASSERT(0 != en);
            ASSERT(0 != ed);
 
        }
        {
            if (veryVerbose) {
                printf("\tinsert every possible combination of [0..7]\n");
            }
            int       VALUES[]   = { 0, 1, 2, 3, 4, 5, 6, 7 };
            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);
            IntNodeComparator nodeComparator;

            do {
                RbTreeNode *root = 0;
                IntNode  nodes[NUM_VALUES];
                ASSERT(0 <= validateIntRbTree(root));               
                for (int i = 0; i < NUM_VALUES; ++i) {
                    nodes[i].setLeftChild(0);
                    nodes[i].setRightChild(0);
                    nodes[i].setParent(0);
                    nodes[i].d_value  = VALUES[i];

                    root = Obj::insert(root, nodeComparator, &nodes[i]);
                    ASSERT(0 <= validateIntRbTree(root));
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
                std::random_shuffle(VALUES, VALUES + NUM_VALUES);    

                RbTreeNode *root = 0;
                IntNode     nodes[NUM_VALUES];
                ASSERT(0 <= validateIntRbTree(root));               
                for (int j = 0; j < NUM_VALUES; ++j) {
                    nodes[j].setLeftChild(0);
                    nodes[j].setRightChild(0);
                    nodes[j].setParent(0);
                    nodes[j].d_value  = VALUES[j];

                    root = Obj::insert(root, nodeComparator, &nodes[j]);
                    ASSERT(0 <= validateIntRbTree(root));
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

            std::random_shuffle(VALUES, VALUES + NUM_VALUES);    

            RbTreeNode *root = 0; 
            IntNode    nodes[NUM_VALUES];
            ASSERT(0 <= validateIntRbTree(root));               
            for (int i = 0; i < NUM_VALUES; ++i) {
                nodes[i].setLeftChild(0);
                nodes[i].setRightChild(0);
                nodes[i].setParent(0);
                nodes[i].d_value  = VALUES[i];
                
                root = Obj::insert(root, nodeComparator, &nodes[i]);
                ASSERT(0 <= validateIntRbTree(root));
            }  
            if (veryVeryVeryVerbose) {
                Obj::printTree(stdout, root, printIntNodeValue, 0, 3);
            }
        }
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

            for (int i = 0; i < sizeof(spec)/sizeof(*spec); ++i) {
                bslma_TestAllocator ta;
                RbTreeNodeArray<IntNode> nodes(&ta);
                
                gg(&nodes, spec[i], assignIntNodeValue);
             
                if (veryVeryVerbose) {
                    Obj::printTree(stdout, 
                                   nodes.data(), 
                                   printIntNodeValue, 0, 3);
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
                RbTreeNode *root = 0;
                IntNode     nodes[NUM_VALUES];

                for (int i = 0; i < NUM_VALUES; ++i) {
                    nodes[i].setLeftChild(0);
                    nodes[i].setRightChild(0);
                    nodes[i].setParent(0);
                    nodes[i].d_value  = i;

                    root = Obj::insert(root, nodeComparator, &nodes[i]);
                    ASSERT(0 <= validateIntRbTree(root));
                }  
                IntNodeValueComparator valueComparator;
                IntNodeComparator      nodeComparator;
                for (int i = 0; i < NUM_VALUES; ++i) {
                    if (veryVeryVeryVerbose) {
                        printf("===================\n");
                        printf("Remove %d\n", VALUES[i]);
                        Obj::printTree(stdout, root, printIntNodeValue, 0, 3);
                    }
                    RbTreeNode *node = Obj::find(root,
                                                 valueComparator, 
                                                 VALUES[i]);
                    ASSERT(0 != node);
                    ASSERT(VALUES[i] == static_cast<IntNode *>(node)->d_value);
                    root = Obj::remove(root, node);

                    if (veryVeryVeryVerbose) {
                        Obj::printTree(stdout, root, printIntNodeValue, 0, 3);
                        printf("===================\n");
                    }
                    ASSERT(isValidSearchTree(root, 
                                             nodeComparator, 
                                             printIntNodeValue));
                    ASSERT(0 <= validateIntRbTree(root));     
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
                RbTreeNode *root = 0;
                IntNode     nodes[NUM_VALUES];
                ASSERT(0 <= validateIntRbTree(root));               
                for (int j = 0; j < NUM_VALUES; ++j) {
                    nodes[j].setLeftChild(0);
                    nodes[j].setRightChild(0);
                    nodes[j].setParent(0);
                    nodes[j].d_value  = VALUES[j];

                    root = Obj::insert(root, nodeComparator, &nodes[j]);
                    ASSERT(0 <= validateIntRbTree(root));
                }  
                std::random_shuffle(VALUES, VALUES + NUM_VALUES);    
                if (veryVeryVeryVerbose) {
                    Obj::printTree(stdout, root, printIntNodeValue, 0, 3);
                }
                for (int j = 0; j < NUM_VALUES; ++j) {

                    RbTreeNode *node = Obj::find(root,
                                                 valueComparator, 
                                                 VALUES[j]);
                    ASSERT(0 != node);
                    ASSERT(VALUES[j] == static_cast<IntNode *>(node)->d_value);
                    
                    if (veryVeryVeryVerbose) {
                        printf("Removing: %d", VALUES[j]);
                    }
                    root = Obj::remove(root, node);

                    if (veryVeryVeryVerbose) {
                        Obj::printTree(stdout, root, printIntNodeValue, 0, 3);
                        printf("===================\n");
                    }
                    ASSERT(isValidSearchTree(root, 
                                             nodeComparator,
                                             printIntNodeValue));
                    ASSERT(0 <= validateIntRbTree(root));     

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
                bslma_TestAllocator ta;
                RbTreeNodeArray<DeleteTestNode> testNodes(&ta);
                DeleteTestNode *root = 
                               gg(&testNodes, SPECS[i], deleteTestNodeAssign);

                if (veryVeryVerbose) {
                    Obj::printTree(stdout, root, printDeleteTestNode, 0, 3);
                }
                DeleteTestNodeFunctor testDeleter;
                Obj::deleteTree(root, testDeleter);
                for (int j = 0; j < testNodes.size(); ++j) {
                    ASSERT(testNodes[j].d_deleted);
                }
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
                RbTreeNode *root = 0;
                IntNode     nodes[NUM_VALUES];
                IntNode     copiedNodes[NUM_VALUES];
                for (int i = 0; i < NUM_VALUES; ++i) {
                    nodes[i].setLeftChild(0);
                    nodes[i].setRightChild(0);
                    nodes[i].setParent(0);
                    nodes[i].d_value  = i;

                    root = Obj::insert(root, nodeComparator, &nodes[i]);
                    ASSERT(0 <= validateIntRbTree(root));
                }  
                IntNodeCloner cloner(copiedNodes, NUM_VALUES);
                RbTreeNode *cloneRoot = Obj::copyTree(root, cloner, cloner);

                ASSERT(cloneRoot >= copiedNodes && 
                       cloneRoot <= copiedNodes + NUM_VALUES);
                ASSERT(areTreesEqual(root, cloneRoot, nodeComparator));
                ASSERT(0 <= validateIntRbTree(cloneRoot));     
                if (veryVeryVeryVerbose) {
                    Obj::printTree(stdout, root, printIntNodeValue, 0, 3);
                    printf("--------------------------------------------\n");
                    Obj::printTree(stdout, cloneRoot, printIntNodeValue, 0, 3);
                }
            } while (std::next_permutation(VALUES, 
                                           VALUES + NUM_VALUES,
                                           intLess)); 
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
