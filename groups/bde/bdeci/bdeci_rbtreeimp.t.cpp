// bdeci_rbtreeimp.t.cpp           -*-C++-*-

#include <bdeci_rbtreeimp.h>

#include <bdes_platformutil.h>                  // for testing only

#include <bdema_bufferallocator.h>              // for testing only
#include <bdema_testallocator.h>                // for testing only
#include <bdema_testallocatorexception.h>       // for testing only

#include <ctype.h>     // isupper()
#include <cstdlib>     // atoi()
#include <cstring>     // strlen(), memset(), memcpy(), memcmp()

#include <iostream>
#include <strstream>

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// Where applicable, this test plan follows the standard approach for
// components implementing value-semantic containers.  (However, note that most
// classes in this component lack copy constructors and assignment operators.
// Moreover, 'bdex' streaming is not supported by this component at all.)  We
// have chosen as *primary* *manipulators* the 'insertAsFirst' and 'removeAll'
// methods to be used by the generator functions 'gg' and 'ggg'.  The common
// helper functions 'stretch' and 'stretchRemoveAll' are provided to facilitate
// perturbation of internal state (e.g., capacity).  Additional helper
// functions are defined to verify the structural integrity of red-black trees;
// in particular, 'walkRBTree' and 'validateRBTree' are used extensively to
// validate all trees.  Note that each manipulator must support aliasing, and
// those that perform memory allocation must be tested for exception neutrality
// via the 'bdema_testallocator' component.
//
// Note that places where test drivers in this family are likely to require
// adjustment are indicated by the tag: "ADJUST".
//-----------------------------------------------------------------------------
// 'bdeci_RbTreeImp' private methods (tested indirectly):
// [15] void listToTree();
// [ 9] Node *insertAfter(Node *node);
// [10] void remove(Node *node);
// [10] void remove(Node *node, Node *previous);
//
// 'bdeci_RbTreeImp' public interface:
// [ 2] explicit bdeci_RbTreeImp(int nb, bdema_Allocator *ba=0);
// [11] bdeci_RbTreeImp(nb, const InitialCapacity& ne, *ba=0);
// [ 2] ~bdeci_RbTreeImp();
// [ 9] void *insertAfter(const void *node);
// [ 9] void *insertAsFirst();
// [10] void remove(const void *node);
// [10] void remove(const void *node, const void *previous);
// [ 2] void removeAll();
// [11] void reserveCapacity(int ne);
// [ 5] const void *first() const;
// [ 5] const void *root() const;
// [ 5] int length() const;
// [ 8] ostream& print(ostream& stream, int level, int spl) const;
// [ 7] ostream& operator<<(ostream& stream, const bdeci_RbTreeImp& rhs);
//-----------------------------------------------------------------------------
// 'bdeci_RbTreeImpIter'
// [ 4] bdeci_RbTreeImpIter();
// [ 4] bdeci_RbTreeImpIter(const bdeci_RbTreeImp& tree);
// [12] bdeci_RbTreeImpIter(const void *node);
// [12] bdeci_RbTreeImpIter(const bdeci_RbTreeImpIter& original);
// [ 4] ~bdeci_RbTreeImpIter();
// [13] bdeci_RbTreeImpIter& operator=(const bdeci_RbTreeImpIter& rhs);
// [ 6] void moveLeft();
// [ 6] void moveNext();
// [ 6] void moveRight();
// [ 6] operator const void *() const;
// [ 6] const void *operator()() const;
// [ 6] int hasLeft() const;
// [ 6] int hasRight() const;
// [ 6] int isBlack() const;
// [ 6] int isRed() const;
//
// [12] int operator==(const Iter& lhs, const Iter& rhs);
// [12] int operator!=(const Iter& lhs, const Iter& rhs);
//-----------------------------------------------------------------------------
// 'bdeci_RbTreeImpManip'
// [14] bdeci_RbTreeImpManip(bdeci_RbTreeImp *tree);
// [14] ~bdeci_RbTreeImpManip();
// [14] void *operator()();
// [14] void advance();
// [14] void *insertAfter();
// [14] void *insertBefore();
// [14] void remove();
// [14] operator const void *() const;
// [14] const void *operator()() const;
//-----------------------------------------------------------------------------
// 'bdeci_RbTreeImpBatchManip'
// [15] bdeci_RbTreeImpBatchManip(bdeci_RbTreeImp *tree);
// [15] ~bdeci_RbTreeImpBatchManip();
// [15] void *operator()();
// [15] void advance();
// [15] void *insertAfter();
// [15] void *insertBefore();
// [15] void remove();
// [15] operator const void *() const;
// [15] const void *operator()() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST: 'bdeci_RbTreeImp' and 'bdeci_RbTreeImpIter'
// [16] USAGE EXAMPLE
// [ 2] BOOTSTRAP: void *insertAsFirst();
// [ 3] CONCERN: Is the internal memory organization behaving as intended?
// [ 3] void stretch(Obj *object, int size);
// [ 3] void stretchRemoveAll(Obj *object, int size);
// [ 3] int ggg(Obj *object, const char *spec, int vF = 1);
// [ 3] Obj& gg(Obj *object, const char *spec);
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  STANDARD BDEMA EXCEPTION TEST MACROS
//-----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
#define BEGIN_BDEMA_EXCEPTION_TEST {                                       \
    {                                                                      \
        static int firstTime = 1;                                          \
        if (veryVerbose && firstTime) cout <<                              \
            "### BDEMA EXCEPTION TEST -- (ENABLED) --" << endl;            \
        firstTime = 0;                                                     \
    }                                                                      \
    if (veryVeryVerbose) cout <<                                           \
        "### Begin bdema exception test." << endl;                         \
    int bdemaExceptionCounter = 0;                                         \
    static int bdemaExceptionLimit = 100;                                  \
    testAllocator.setAllocationLimit(bdemaExceptionCounter);               \
    do {                                                                   \
        try {

#define END_BDEMA_EXCEPTION_TEST                                           \
        } catch (bdema_TestAllocatorException& e) {                        \
            if (veryVerbose && bdemaExceptionLimit || veryVeryVerbose) {   \
                --bdemaExceptionLimit;                                     \
                cout << "(*** " <<                                         \
                bdemaExceptionCounter << ')';                              \
                if (veryVeryVerbose) { cout << " BEDMA_EXCEPTION: "        \
                    << "alloc limit = " << bdemaExceptionCounter << ", "   \
                    << "last alloc size = " << e.numBytes();               \
                }                                                          \
                else if (0 == bdemaExceptionLimit) {                       \
                     cout << " [ Note: 'bdemaExceptionLimit' reached. ]";  \
                }                                                          \
                cout << endl;                                              \
            }                                                              \
            testAllocator.setAllocationLimit(++bdemaExceptionCounter);     \
            continue;                                                      \
        }                                                                  \
        testAllocator.setAllocationLimit(-1);                              \
        break;                                                             \
    } while (1);                                                           \
    if (veryVeryVerbose) cout <<                                           \
        "### End bdema exception test." << endl;                           \
}
#else
#define BEGIN_BDEMA_EXCEPTION_TEST                                         \
{                                                                          \
    static int firstTime = 1;                                              \
    if (verbose && firstTime) { cout <<                                    \
        "### BDEMA EXCEPTION TEST -- (NOT ENABLED) --" << endl;            \
        firstTime = 0;                                                     \
    }                                                                      \
}
#define END_BDEMA_EXCEPTION_TEST
#endif

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define TAB cout << '\t';

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdeci_RbTreeImp           Obj;
typedef bdeci_RbTreeImpIter       Iter;
typedef bdeci_RbTreeImpManip      Manip;
typedef bdeci_RbTreeImpBatchManip BatchManip;
typedef bdeci_RbTreeImp_Node      Node;

const int NB[] = {
    1                                 ,
    bdes_Alignment::MAX_ALIGNMENT - 1 ,
    bdes_Alignment::MAX_ALIGNMENT     ,
    bdes_Alignment::MAX_ALIGNMENT + 1 ,
    bdes_Alignment::MAX_ALIGNMENT * 3 ,
};
const int NUM_NB = sizeof NB / sizeof *NB;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void stretch(Obj *object, int size)
   // Using only primary manipulators, extend the length of the specified
   // 'object' by the specified size.  The resulting value is not specified.
   // The behavior is undefined unless 0 <= size.
{
    ASSERT(object);
    ASSERT(0 <= size);

    const char *codes  = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static int codeLen = std::strlen(codes);
    for (int i = 0; i < size; ++i) {
        char *node = (char *)object->insertAsFirst();
        *node = codes[i % codeLen];
    }

    ASSERT(object->length() >= size);
}

void stretchRemoveAll(Obj *object, int size)
   // Using only primary manipulators, extend the capacity of the specified
   // 'object' to (at least) the specified size; then remove all elements
   // leaving 'object' empty.  The behavior is undefined unless 0 <= size.
{
    ASSERT(object);
    ASSERT(0 <= size);

    stretch(object, size);
    object->removeAll();

    ASSERT(0 == object->length());
}

// Warning: keep in sync with component header file!
inline
const Node *toNode(const void *address)
    // Given an address pointing to the 'd_memory' member (last member) of a
    // 'Node' object, compute the address of the 'Node' object itself.
    // The behavior is undefined unless 'address' points to the 'd_memory'
    // field of a 'Node' object.
{
    // Offset address by size of 'd_memory' member to find end of 'Node', then
    // convert to 'Node*' and subtract one to get start of 'Node'.
    return (const Node *)((const char *)address +
                          sizeof(bdes_Alignment::MaxAlignedType)) - 1;
}

int walkRBTree(const Iter& I, int height, const char *colors, const char *tags)
    // Recursively walk the specified sub-tree associated with the specified
    // iterator 'I' verifying: (1) that the tree has the specified 'height';
    // (2) that the nodes in the tree are colored according to the specified
    // 'colors'; and (3) that the nodes of the tree are tagged with 'char'
    // values according to the specified 'tags'.  Return the total number of
    // nodes visited during the tree walk (including nil nodes).
{
    ASSERT(0 <= height);
    ASSERT(colors);
    ASSERT(tags);

    if ('x' == *colors) {
        ASSERT('x' == *tags);
        ASSERT(!I);
        return 1;
    }

    ASSERT('r' == *colors || 'b' == *colors);
    ASSERT(isupper(*tags));

    ASSERT(I && I());

    const int isRed = *colors == 'r';
    ASSERT( isRed == I.isRed());
    ASSERT(!isRed == I.isBlack());

    const char *tag = (char *)I();
    ASSERT(*tag == *tags);
    const Node *parent = toNode(I());

    // look left
    const char *colorsLeft = colors + 1;
    const char *tagsLeft   = tags + 1;
    ASSERT(('x' != *colorsLeft) == I.hasLeft());

    Iter left(I());
    left.moveLeft();
    if (left) {
        // Check left child's parent link.
        const Node *leftNode = toNode(left());
        ASSERT(parent == leftNode->d_parent_p);
    }
    const int nodesLeft = walkRBTree(left, height - 1, colorsLeft, tagsLeft);

    // look right
    const char *colorsRight = colors + nodesLeft + 1;
    const char *tagsRight   = tags + nodesLeft + 1;
    ASSERT(('x' != *colorsRight) == I.hasRight());

    Iter right(I());
    right.moveRight();
    if (right) {
        // Check right child's parent link.
        const Node *rightNode = toNode(right());
        ASSERT(parent == rightNode->d_parent_p);
    }
    const int nodesRight =
                         walkRBTree(right, height - 1, colorsRight, tagsRight);

    return nodesLeft + nodesRight + 1;
}

static int validateRBSubTree(const Iter& I)
    // Recursively verify that the sub-tree associated with the specified
    // iterator 'I' satisfies the requirements of a red-black tree.  Return the
    // "black-height" of the sub-tree.
{
    // Requirement: every node is either red or black.
    ASSERT(I.isRed() || I.isBlack());

    Iter left(I());   left.moveLeft();
    Iter right(I());  right.moveRight();

    // Requirement: if a node is red, then each of its children are black.
    // (Leaf (nil) nodes are implicitly black.)
    if (I.isRed()) {
        ASSERT(!left  || left.isBlack());
        ASSERT(!right || right.isBlack());
    }

    // Requirement: every path from a node to its descendent leaves has the
    // same number of black nodes (i.e., equivalent Black Heights).
    int leftBH  = 1;
    int rightBH = 1;
    if (left) {
        leftBH = validateRBSubTree(left);
    }
    if (right) {
        rightBH = validateRBSubTree(right);
    }
    ASSERT(leftBH == rightBH);

    if (I.isBlack()) {
        ++leftBH;
    }
    return leftBH;
}

inline
static void validateRBTree(const Obj& tree)
    // Verify that the specified 'tree' satisfies the requirements of a
    // red-black tree.
{
    const Iter root(tree.root());
    if (root) {
        // Requirement: the root is black.
        ASSERT(root.isBlack());
        validateRBSubTree(root);
    }
}

inline
static void scribble(char *address, int value, int size)
    // Assign the specified 'value' to each of the specified 'size' bytes
    // starting at the specified 'address'.
{
    memset(address, value, size);
}

//=============================================================================
//              GENERATOR FUNCTIONS 'g' AND 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right to configure an object according to a custom language.  Uppercase
// letters [A .. Z] correspond to char values that are used to tag the nodes
// of a red-black tree as they are inserted.  Lowercase letters [a .. z]
// indicate positions in the tree after which new nodes should be inserted.
// 'a' corresponds to the first position, 'b' to the second, etc.
// The absence of a position specification indicates that a new node is to be
// inserted as the first node in the tree.  A tilde ('~') indicates that the
// logical (but not necessarily physical) state of the object is to be set to
// its initial, empty state (via the 'removeAll' method).
//
// LANGUAGE SPECIFICATION:
// -----------------------
//
// <SPEC>       ::= <EMPTY>   | <LIST>
//
// <EMPTY>      ::=
//
// <LIST>       ::= <ITEM>    | <ITEM> <LIST>
//
// <ITEM>       ::= <ELEMENT> | <REMOVE_ALL>
//
// <ELEMENT>    ::= <POSITION> <TAG>
//
// <POSITION>   ::= 'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' |
//                  'j' | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' |
//                  's' | 't' | 'u' | 'v' | 'w' | 'x' | 'y' | 'z'
//
// <TAG>        ::= 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' |
//                  'J' | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' |
//                  'S' | 'T' | 'U' | 'V' | 'W' | 'X' | 'Y' | 'Z'
//
// <REMOVE_ALL> ::= '~'
//
// Spec String  Description
// -----------  ---------------------------------------------------------------
// ""           Has no effect; leaves the object unchanged.
// "A"          Add a node at the first position of the tree and tag it with
//              the char value 'A'.
// "AA"         Add a node at the first position of the tree and tag it with
//              the char value 'A'.  Then add a second node at the first
//              position of the tree and also tag it with the char value 'A'.
// "ABC"        Add a node at the first position of the tree and tag it with
//              the char value 'A'.  Then add a second node at the first
//              position of the tree and tag it with the char value 'B'.
//              Finally, add a third node at the first position of the tree and
//              tag it with the char value 'C'.
// "ABaC"       Add a node at the first position of the tree and tag it with
//              the char value 'A'.  Then add a second node at the first
//              position of the tree and tag it with the char value 'B'.
//              Finally, add a third node following the first position of the
//              tree and tag it with the char value 'C'.
// "ABaCbD"     Add a node at the first position of the tree and tag it with
//              the char value 'A'.  Then add a second node at the first
//              position of the tree and tag it with the char value 'B'.
//              Then add a third node following the first position of the tree
//              and tag it with the char value 'C'.  Finally add a fourth node
//              following the second position of the tree and tag it with the
//              char value 'D'.
// "ABC~"       Add three nodes, tagged with the char values 'A', 'B' and 'C',
//              respectively, and then remove all the elements (set numElements
//              to 0).  Note that this spec yields an object that is logically
//              equivalent (but not necessarily identical internally) to one
//              yielded by ("~").
// "ABC~DE"     Add three nodes, tagged with the char values 'A', 'B' and 'C',
//              respectively; empty the object; then add two nodes tagged with
//              the char values 'D' and 'E', respectively.  Note that this spec
//              yields an object that is logically equivalent (but not
//              necessarily identical internally) to one yielded by ("~DE").
//
//-----------------------------------------------------------------------------

int ggg(Obj *object, const char *spec, int verboseFlag = 1)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulator functions 'insertAsFirst' and
    // 'insertAfter' and white-box manipulator 'removeAll'.  Optionally
    // specify a zero 'verboseFlag' to suppress 'spec' syntax error messages.
    // Return the index of the first invalid character, and a negative value
    // otherwise.  Note that this function is used to implement 'gg' as well
    // as allow for verification of syntax error detection.
{
    enum { SUCCESS = -1 };

    const char *codes = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    enum { MAX_NODES = 256 };
    void *nodes[MAX_NODES];
    int nNodes = 0;

    int i;
    for (i = 0; spec[i]; ++i) {
        const char *code = strchr(codes, spec[i]);
        if (code) {  // *code isalpha
            char *node;
            char tag;
            if (isupper(*code)) {
                node = (char *)object->insertAsFirst();
                tag  = *code;
            }
            else if (isupper(spec[i+1])) {  // *code islower
                int index = *code - 'a';
                if (index >= nNodes) {
                    goto error;
                }
                node = (char *)object->insertAfter(nodes[index]);
                tag  = spec[++i];
            }
            else {
                goto error;
            }
            *node = tag;
            ASSERT(MAX_NODES > nNodes);
            nodes[nNodes++] = node;
        }
        else if ('~' == spec[i]) {
            object->removeAll();
            nNodes = 0;
        }
        else {
            goto error;
        }
    }
    return SUCCESS;

error:
    if (verboseFlag) {
        cout << "Error, bad character ('" << spec[i]  << "') in spec \""
             << spec << "\" at position " << i << '.' << endl;
    }
    return i;  // Discontinue processing this spec.
}

Obj& gg(Obj *object, const char *spec)
    // Return, by reference, the specified 'object' with its value adjusted
    // according to the specified 'spec'.
{
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

//=============================================================================
//                  GLOBAL DATA COMMON ACROSS TEST CASES
//-----------------------------------------------------------------------------

static const struct {
    int         d_lineNum;    // source line number
    const char *d_spec;       // specification string
    const char *d_listColors; // expected r-b list coloration
    const char *d_listTags;   // expected r-b list node tags
    int         d_height;     // expected r-b tree height
    const char *d_treeColors; // expected r-b tree coloration
    const char *d_treeTags;   // expected r-b tree node tags
} GDATA_ITERATE[] = {
    //line spec          ht   list/tree colors   list/tree tags       // ADJUST
    //---- ----          --   ----------------   --------------

    { L_,  "",                "",                "",
                         0,   "x",               "x"                         },

    { L_,  "A",               "b",               "A",
                         1,   "bxx",             "Axx"                       },

    { L_,  "AB",              "rb",              "BA",
                         2,   "brxxx",           "ABxxx"                     },

    { L_,  "ABC",             "rbr",             "CBA",
                         2,   "brxxrxx",         "BCxxAxx"                   },

    { L_,  "ABCD",            "rbbb",            "DCBA",
                         3,   "bbrxxxbxx",       "BCDxxxAxx"                 },

    { L_,  "ABCDE",           "rbrbb",           "EDCBA",
                         3,   "bbrxxrxxbxx",     "BDExxCxxAxx"               },

    { L_,  "ABCDEF",          "rbrbbb",          "FEDCBA",
                         4,   "brbrxxxbxxbxx",   "BDEFxxxCxxAxx"             },

    { L_,  "ABCDEFG",         "rbrrbbb",         "GFEDCBA",
                         4,   "brbrxxrxxbxxbxx", "BDFGxxExxCxxAxx"           },

    { L_,  "ABCDEFGH",        "rbrbbbrb",        "HGFEDCBA",
                         4,   "brbrxxxbxxrbxxbxx",
                                                 "DFGHxxxExxBCxxAxx"         },

    { L_,  "ABCDEFGHI",       "rbrrbbbrb",       "IHGFEDCBA",
                         4,   "brbrxxrxxbxxrbxxbxx",
                                                 "DFHIxxGxxExxBCxxAxx"       },

    { L_,  "ABCDEFGHIJ",      "rbrbbbbbbb",      "JIHGFEDCBA",
                         5,   "bbrbrxxxbxxbxxbbxxbxx",
                                                 "DFHIJxxxGxxExxBCxxAxx"     },

    { L_,  "ABCDEFGHIJK",     "rbrrbbbbbbb",     "KJIHGFEDCBA",
                         5,   "bbrbrxxrxxbxxbxxbbxxbxx",
                                                 "DFHJKxxIxxGxxExxBCxxAxx"   },

    { L_,  "ABCDEFGHIJKL",    "rbrbbbrbbbbb",    "LKJIHGFEDCBA",
                         5,   "bbrbrxxxbxxrbxxbxxbbxxbxx",
                                                 "DHJKLxxxIxxFGxxExxBCxxAxx" },

    { L_,  "ABCDEFGHIJKLM",   "rbrrbbbrbbbbb",   "MLKJIHGFEDCBA",
                         5,   "bbrbrxxrxxbxxrbxxbxxbbxxbxx",
                                               "DHJLMxxKxxIxxFGxxExxBCxxAxx" },

    { L_,  "ABCDEFGHIJKLMN",  "rbrbbbrbbbbbbb",  "NMLKJIHGFEDCBA",
                         6,   "brbrbrxxxbxxbxxbbxxbxxbbxxbxx",
                                             "DHJLMNxxxKxxIxxFGxxExxBCxxAxx" },

    { L_,  "ABCDEFGHIJKLMNO", "rbrrbbbrbbbbbbb", "ONMLKJIHGFEDCBA",
                         6,   "brbrbrxxrxxbxxbxxbbxxbxxbbxxbxx",
                                           "DHJLNOxxMxxKxxIxxFGxxExxBCxxAxx" },
};

static const struct {
    int         d_lineNum;       // source line number
    const char *d_spec;          // specification string
    int         d_pos;           // insert after this position
    const char *d_listColors;    // expected r-b list coloration
    const char *d_listTags;      // expected r-b list node tags
    int         d_height;        // expected r-b tree height
    const char *d_treeColors;    // expected r-b tree coloration
    const char *d_treeTags;      // expected r-b tree node tags
} GDATA_INSERTAFTER[] = {
    //line spec      pos/ht   list/tree colors       list/tree tags   // ADJUST
    //---- ----      ------   ----------------       --------------
    { L_,  "A",          0,   "br",                  "AX",
                         2,   "bxrxx",               "AxXxx"                 },

    { L_,  "AB",         0,   "rbr",                 "BXA",
                         2,   "brxxrxx",             "XBxxAxx"               },
    { L_,  "AB",         1,   "rbr",                 "BAX",
                         2,   "brxxrxx",             "ABxxXxx"               },

    { L_,  "ABC",        0,   "brbb",                "CXBA",
                         3,   "bbxrxxbxx",           "BCxXxxAxx"             },
    { L_,  "ABC",        1,   "bbrb",                "CBXA",
                         3,   "bbxxbrxxx",           "BCxxAXxxx"             },
    { L_,  "ABC",        2,   "bbbr",                "CBAX",
                         3,   "bbxxbxrxx",           "BCxxAxXxx"             },

    { L_,  "ABCD",       0,   "rbrbb",               "DXCBA",
                         3,   "bbrxxrxxbxx",         "BXDxxCxxAxx"           },
    { L_,  "ABCD",       1,   "rbrbb",               "DCXBA",
                         3,   "bbrxxrxxbxx",         "BCDxxXxxAxx"           },
    { L_,  "ABCD",       2,   "rbbrb",               "DCBXA",
                         3,   "bbrxxxbrxxx",         "BCDxxxAXxxx"           },
    { L_,  "ABCD",       3,   "rbbbr",               "DCBAX",
                         3,   "bbrxxxbxrxx",         "BCDxxxAxXxx"           },

    { L_,  "ABCDE",      0,   "brrbbb",              "EXDCBA",
                         4,   "brbxrxxbxxbxx",       "BDExXxxCxxAxx"         },
    { L_,  "ABCDE",      1,   "brrbbb",              "EDXCBA",
                         4,   "brbxxbrxxxbxx",       "BDExxCXxxxAxx"         },
    { L_,  "ABCDE",      2,   "brbrbb",              "EDCXBA",
                         4,   "brbxxbxrxxbxx",       "BDExxCxXxxAxx"         },
    { L_,  "ABCDE",      3,   "rbrbrb",              "EDCBXA",
                         4,   "bbrxxrxxbrxxx",       "BDExxCxxAXxxx"         },
    { L_,  "ABCDE",      4,   "rbrbbr",              "EDCBAX",
                         4,   "bbrxxrxxbxrxx",       "BDExxCxxAxXxx"         },

    { L_,  "ABCDEF",     0,   "rbrrbbb",             "FXEDCBA",
                         4,   "brbrxxrxxbxxbxx",     "BDXFxxExxCxxAxx"       },
    { L_,  "ABCDEF",     1,   "rbrrbbb",             "FEXDCBA",
                         4,   "brbrxxrxxbxxbxx",     "BDEFxxXxxCxxAxx"       },
    { L_,  "ABCDEF",     2,   "rbrrbbb",             "FEDXCBA",
                         4,   "brbrxxxbrxxxbxx",     "BDEFxxxCXxxxAxx"       },
    { L_,  "ABCDEF",     3,   "rbrbrbb",             "FEDCXBA",
                         4,   "brbrxxxbxrxxbxx",     "BDEFxxxCxXxxAxx"       },
    { L_,  "ABCDEF",     4,   "rbrbbrb",             "FEDCBXA",
                         4,   "brbrxxxbxxbrxxx",     "BDEFxxxCxxAXxxx"       },
    { L_,  "ABCDEF",     5,   "rbrbbbr",             "FEDCBAX",
                         4,   "brbrxxxbxxbxrxx",     "BDEFxxxCxxAxXxx"       },

    { L_,  "ABCDEFG",    0,   "brrbbbrb",            "GXFEDCBA",
                         4,   "brbxrxxbxxrbxxbxx",   "DFGxXxxExxBCxxAxx"     },
    { L_,  "ABCDEFG",    1,   "brrbbbrb",            "GFXEDCBA",
                         4,   "brbxxbrxxxrbxxbxx",   "DFGxxEXxxxBCxxAxx"     },
    { L_,  "ABCDEFG",    2,   "brbrbbrb",            "GFEXDCBA",
                         4,   "brbxxbxrxxrbxxbxx",   "DFGxxExXxxBCxxAxx"     },
    { L_,  "ABCDEFG",    3,   "rbrrrbbb",            "GFEDXCBA",
                         4,   "brbrxxrxxbrxxxbxx",   "BDFGxxExxCXxxxAxx"     },
    { L_,  "ABCDEFG",    4,   "rbrrbrbb",            "GFEDCXBA",
                         4,   "brbrxxrxxbxrxxbxx",   "BDFGxxExxCxXxxAxx"     },
    { L_,  "ABCDEFG",    5,   "rbrrbbrb",            "GFEDCBXA",
                         4,   "brbrxxrxxbxxbrxxx",   "BDFGxxExxCxxAXxxx"     },
    { L_,  "ABCDEFG",    6,   "rbrrbbbr",            "GFEDCBAX",
                         4,   "brbrxxrxxbxxbxrxx",   "BDFGxxExxCxxAxXxx"     },

    { L_,  "ABCDEFGH",   0,   "rbrrbbbrb",           "HXGFEDCBA",
                         4,   "brbrxxrxxbxxrbxxbxx", "DFXHxxGxxExxBCxxAxx"   },
    { L_,  "ABCDEFGH",   1,   "rbrrbbbrb",           "HGXFEDCBA",
                         4,   "brbrxxrxxbxxrbxxbxx", "DFGHxxXxxExxBCxxAxx"   },
    { L_,  "ABCDEFGH",   2,   "rbrrbbbrb",           "HGFXEDCBA",
                         4,   "brbrxxxbrxxxrbxxbxx", "DFGHxxxEXxxxBCxxAxx"   },
    { L_,  "ABCDEFGH",   3,   "rbrbrbbrb",           "HGFEXDCBA",
                         4,   "brbrxxxbxrxxrbxxbxx", "DFGHxxxExXxxBCxxAxx"   },
    { L_,  "ABCDEFGH",   4,   "rbrbbrbrb",           "HGFEDXCBA",
                         4,   "brbrxxxbxxrbrxxxbxx", "DFGHxxxExxBCXxxxAxx"   },
    { L_,  "ABCDEFGH",   5,   "rbrbbbrrb",           "HGFEDCXBA",
                         4,   "brbrxxxbxxrbxrxxbxx", "DFGHxxxExxBCxXxxAxx"   },
    { L_,  "ABCDEFGH",   6,   "rbrbbbrrb",           "HGFEDCBXA",
                         4,   "brbrxxxbxxrbxxbrxxx", "DFGHxxxExxBCxxAXxxx"   },
    { L_,  "ABCDEFGH",   7,   "rbrbbbrbr",           "HGFEDCBAX",
                         4,   "brbrxxxbxxrbxxbxrxx", "DFGHxxxExxBCxxAxXxx"   },

    { L_,  "ABCDEFGHI",  0,   "brrbbbbbbb",          "IXHGFEDCBA",
                         5,   "bbrbxrxxbxxbxxbbxxbxx",
                                                     "DFHIxXxxGxxExxBCxxAxx" },
    { L_,  "ABCDEFGHI",  1,   "brrbbbbbbb",          "IHXGFEDCBA",
                         5,   "bbrbxxbrxxxbxxbbxxbxx",
                                                     "DFHIxxGXxxxExxBCxxAxx" },
    { L_,  "ABCDEFGHI",  2,   "brbrbbbbbb",          "IHGXFEDCBA",
                         5,   "bbrbxxbxrxxbxxbbxxbxx",
                                                     "DFHIxxGxXxxExxBCxxAxx" },
    { L_,  "ABCDEFGHI",  3,   "rbrrrbbbrb",          "IHGFXEDCBA",
                         5,   "brbrxxrxxbrxxxrbxxbxx",
                                                     "DFHIxxGxxEXxxxBCxxAxx" },
    { L_,  "ABCDEFGHI",  4,   "rbrrbrbbrb",          "IHGFEXDCBA",
                         5,   "brbrxxrxxbxrxxrbxxbxx",
                                                     "DFHIxxGxxExXxxBCxxAxx" },
    { L_,  "ABCDEFGHI",  5,   "rbrrbbrbrb",          "IHGFEDXCBA",
                         5,   "brbrxxrxxbxxrbrxxxbxx",
                                                     "DFHIxxGxxExxBCXxxxAxx" },
    { L_,  "ABCDEFGHI",  6,   "rbrrbbbrrb",          "IHGFEDCXBA",
                         5,   "brbrxxrxxbxxrbxrxxbxx",
                                                     "DFHIxxGxxExxBCxXxxAxx" },
    { L_,  "ABCDEFGHI",  7,   "rbrrbbbrrb",          "IHGFEDCBXA",
                         5,   "brbrxxrxxbxxrbxxbrxxx",
                                                     "DFHIxxGxxExxBCxxAXxxx" },
    { L_,  "ABCDEFGHI",  8,   "rbrrbbbrbr",          "IHGFEDCBAX",
                         5,   "brbrxxrxxbxxrbxxbxrxx",
                                                     "DFHIxxGxxExxBCxxAxXxx" },

    { L_,  "ABCDEFGHIJ", 0,   "rbrrbbbbbbb",         "JXIHGFEDCBA",
                         5,   "bbrbrxxrxxbxxbxxbbxxbxx",
                                                   "DFHXJxxIxxGxxExxBCxxAxx" },
    { L_,  "ABCDEFGHIJ", 1,   "rbrrbbbbbbb",         "JIXHGFEDCBA",
                         5,   "bbrbrxxrxxbxxbxxbbxxbxx",
                                                   "DFHIJxxXxxGxxExxBCxxAxx" },
    { L_,  "ABCDEFGHIJ", 2,   "rbrrbbbbbbb",         "JIHXGFEDCBA",
                         5,   "bbrbrxxxbrxxxbxxbbxxbxx",
                                                   "DFHIJxxxGXxxxExxBCxxAxx" },
    { L_,  "ABCDEFGHIJ", 3,   "rbrbrbbbbbb",         "JIHGXFEDCBA",
                         5,   "bbrbrxxxbxrxxbxxbbxxbxx",
                                                   "DFHIJxxxGxXxxExxBCxxAxx" },
    { L_,  "ABCDEFGHIJ", 4,   "rbrbbrbbbbb",         "JIHGFXEDCBA",
                         5,   "bbrbrxxxbxxbrxxxbbxxbxx",
                                                   "DFHIJxxxGxxEXxxxBCxxAxx" },
    { L_,  "ABCDEFGHIJ", 5,   "rbrbbbrbbbb",         "JIHGFEXDCBA",
                         5,   "bbrbrxxxbxxbxrxxbbxxbxx",
                                                   "DFHIJxxxGxxExXxxBCxxAxx" },
    { L_,  "ABCDEFGHIJ", 6,   "rbrbbbbrbbb",         "JIHGFEDXCBA",
                         5,   "bbrbrxxxbxxbxxbbrxxxbxx",
                                                   "DFHIJxxxGxxExxBCXxxxAxx" },
    { L_,  "ABCDEFGHIJ", 7,   "rbrbbbbbrbb",         "JIHGFEDCXBA",
                         5,   "bbrbrxxxbxxbxxbbxrxxbxx",
                                                   "DFHIJxxxGxxExxBCxXxxAxx" },
    { L_,  "ABCDEFGHIJ", 8,   "rbrbbbbbbrb",         "JIHGFEDCBXA",
                         5,   "bbrbrxxxbxxbxxbbxxbrxxx",
                                                   "DFHIJxxxGxxExxBCxxAXxxx" },
    { L_,  "ABCDEFGHIJ", 9,   "rbrbbbbbbbr",         "JIHGFEDCBAX",
                         5,   "bbrbrxxxbxxbxxbbxxbxrxx",
                                                   "DFHIJxxxGxxExxBCxxAxXxx" },
};

static const struct {
    int         d_lineNum;       // source line number
    const char *d_spec;          // specification string
    int         d_pos;           // remove at this position
    const char *d_listColors;    // expected r-b list coloration
    const char *d_listTags;      // expected r-b list node tags
    int         d_height;        // expected r-b tree height
    const char *d_treeColors;    // expected r-b tree coloration
    const char *d_treeTags;      // expected r-b tree node tags
} GDATA_REMOVE[] = {
    //line spec      pos/ht    list/tree colors   list/tree tags      // ADJUST
    //---- ----      ------    ----------------   --------------
    { L_,  "A",          0,    "",                    "",
                         0,    "x",                   "x"                    },

    { L_,  "AB",         0,    "b",                   "A",
                         1,    "bxx",                 "Axx"                  },
    { L_,  "AB",         1,    "b",                   "B",
                         1,    "bxx",                 "Bxx"                  },

    { L_,  "ABC",        0,    "br",                  "BA",
                         2,    "bxrxx",               "BxAxx"                },
    { L_,  "ABC",        1,    "rb",                  "CA",
                         2,    "brxxx",               "ACxxx"                },
    { L_,  "ABC",        2,    "rb",                  "CB",
                         2,    "brxxx",               "BCxxx"                },

    { L_,  "ABCD",       0,    "bbb",                 "CBA",
                         3,    "bbxxbxx",             "BCxxAxx"              },
    { L_,  "ABCD",       1,    "bbb",                 "DBA",
                         3,    "bbxxbxx",             "BDxxAxx"              },
    { L_,  "ABCD",       2,    "bbb",                 "DCA",
                         3,    "bbxxbxx",             "CDxxAxx"              },
    { L_,  "ABCD",       3,    "bbb",                 "DCB",
                         3,    "bbxxbxx",             "CDxxBxx"              },

    { L_,  "ABCDE",      0,    "brbb",                "DCBA",
                         3,    "bbxrxxbxx",           "BDxCxxAxx"            },
    { L_,  "ABCDE",      1,    "rbbb",                "ECBA",
                         3,    "bbrxxxbxx",           "BCExxxAxx"            },
    { L_,  "ABCDE",      2,    "rbbb",                "EDBA",
                         3,    "bbrxxxbxx",           "BDExxxAxx"            },
    { L_,  "ABCDE",      3,    "bbrb",                "EDCA",
                         3,    "bbxxbrxxx",           "DExxACxxx"            },
    { L_,  "ABCDE",      4,    "bbrb",                "EDCB",
                         3,    "bbxxbrxxx",           "DExxBCxxx"            },

    { L_,  "ABCDEF",     0,    "brbbb",               "EDCBA",
                         3,    "brbxxbxxbxx",         "BDExxCxxAxx"          },
    { L_,  "ABCDEF",     1,    "brbbb",               "FDCBA",
                         3,    "brbxxbxxbxx",         "BDFxxCxxAxx"          },
    { L_,  "ABCDEF",     2,    "brbbb",               "FECBA",
                         3,    "brbxxbxxbxx",         "BEFxxCxxAxx"          },
    { L_,  "ABCDEF",     3,    "brbbb",               "FEDBA",
                         3,    "brbxxbxxbxx",         "BEFxxDxxAxx"          },
    { L_,  "ABCDEF",     4,    "rbbrb",               "FEDCA",
                         3,    "bbrxxxbrxxx",         "DEFxxxACxxx"          },
    { L_,  "ABCDEF",     5,    "rbbrb",               "FEDCB",
                         3,    "bbrxxxbrxxx",         "DEFxxxBCxxx"          },

    { L_,  "ABCDEFG",    0,    "brrbbb",              "FEDCBA",
                         4,    "brbxrxxbxxbxx",       "BDFxExxCxxAxx"        },
    { L_,  "ABCDEFG",    1,    "rbrbbb",              "GEDCBA",
                         4,    "brbrxxxbxxbxx",       "BDEGxxxCxxAxx"        },
    { L_,  "ABCDEFG",    2,    "rbrbbb",              "GFDCBA",
                         4,    "brbrxxxbxxbxx",       "BDFGxxxCxxAxx"        },
    { L_,  "ABCDEFG",    3,    "brrbbb",              "GFECBA",
                         4,    "brbxxbrxxxbxx",       "BFGxxCExxxAxx"        },
    { L_,  "ABCDEFG",    4,    "brrbbb",              "GFEDBA",
                         4,    "brbxxbrxxxbxx",       "BFGxxDExxxAxx"        },
    { L_,  "ABCDEFG",    5,    "rbrbrb",              "GFEDCA",
                         4,    "bbrxxrxxbrxxx",       "DFGxxExxACxxx"        },
    { L_,  "ABCDEFG",    6,    "rbrbrb",              "GFEDCB",
                         4,    "bbrxxrxxbrxxx",       "DFGxxExxBCxxx"        },

    { L_,  "ABCDEFGH",   0,    "brbbbrb",             "GFEDCBA",
                         4,    "brbxxbxxrbxxbxx",     "DFGxxExxBCxxAxx"      },
    { L_,  "ABCDEFGH",   1,    "brbbbrb",             "HFEDCBA",
                         4,    "brbxxbxxrbxxbxx",     "DFHxxExxBCxxAxx"      },
    { L_,  "ABCDEFGH",   2,    "brbbbrb",             "HGEDCBA",
                         4,    "brbxxbxxrbxxbxx",     "DGHxxExxBCxxAxx"      },
    { L_,  "ABCDEFGH",   3,    "brbbbrb",             "HGFDCBA",
                         4,    "brbxxbxxrbxxbxx",     "DGHxxFxxBCxxAxx"      },
    { L_,  "ABCDEFGH",   4,    "rbrbbbr",             "HGFECBA",
                         4,    "brbrxxxbxxbxrxx",     "CFGHxxxExxBxAxx"      },
    { L_,  "ABCDEFGH",   5,    "rbrbbbr",             "HGFEDBA",
                         4,    "brbrxxxbxxbxrxx",     "DFGHxxxExxBxAxx"      },
    { L_,  "ABCDEFGH",   6,    "rbrbbrb",             "HGFEDCA",
                         4,    "brbrxxxbxxbrxxx",     "DFGHxxxExxACxxx"      },
    { L_,  "ABCDEFGH",   7,    "rbrbbrb",             "HGFEDCB",
                         4,    "brbrxxxbxxbrxxx",     "DFGHxxxExxBCxxx"      },

    { L_,  "ABCDEFGHI",  0,    "brrbbbrb",            "HGFEDCBA",
                         4,    "brbxrxxbxxrbxxbxx",   "DFHxGxxExxBCxxAxx"    },
    { L_,  "ABCDEFGHI",  1,    "rbrbbbrb",            "IGFEDCBA",
                         4,    "brbrxxxbxxrbxxbxx",   "DFGIxxxExxBCxxAxx"    },
    { L_,  "ABCDEFGHI",  2,    "rbrbbbrb",            "IHFEDCBA",
                         4,    "brbrxxxbxxrbxxbxx",   "DFHIxxxExxBCxxAxx"    },
    { L_,  "ABCDEFGHI",  3,    "brrbbbrb",            "IHGEDCBA",
                         4,    "brbxxbrxxxrbxxbxx",   "DHIxxEGxxxBCxxAxx"    },
    { L_,  "ABCDEFGHI",  4,    "brrbbbrb",            "IHGFDCBA",
                         4,    "brbxxbrxxxrbxxbxx",   "DHIxxFGxxxBCxxAxx"    },
    { L_,  "ABCDEFGHI",  5,    "rbrrbbbr",            "IHGFECBA",
                         4,    "brbrxxrxxbxxbxrxx",   "CFHIxxGxxExxBxAxx"    },
    { L_,  "ABCDEFGHI",  6,    "rbrrbbbr",            "IHGFEDBA",
                         4,    "brbrxxrxxbxxbxrxx",   "DFHIxxGxxExxBxAxx"    },
    { L_,  "ABCDEFGHI",  7,    "rbrrbbrb",            "IHGFEDCA",
                         4,    "brbrxxrxxbxxbrxxx",   "DFHIxxGxxExxACxxx"    },
    { L_,  "ABCDEFGHI",  8,    "rbrrbbrb",            "IHGFEDCB",
                         4,    "brbrxxrxxbxxbrxxx",   "DFHIxxGxxExxBCxxx"    },

    { L_,  "ABCDEFGHIJ", 0,    "brbbbbbbb",           "IHGFEDCBA",
                         4,    "bbrbxxbxxbxxbbxxbxx", "DFHIxxGxxExxBCxxAxx"  },
    { L_,  "ABCDEFGHIJ", 1,    "brbbbbbbb",           "JHGFEDCBA",
                         4,    "bbrbxxbxxbxxbbxxbxx", "DFHJxxGxxExxBCxxAxx"  },
    { L_,  "ABCDEFGHIJ", 2,    "brbbbbbbb",           "JIGFEDCBA",
                         4,    "bbrbxxbxxbxxbbxxbxx", "DFIJxxGxxExxBCxxAxx"  },
    { L_,  "ABCDEFGHIJ", 3,    "brbbbbbbb",           "JIHFEDCBA",
                         4,    "bbrbxxbxxbxxbbxxbxx", "DFIJxxHxxExxBCxxAxx"  },
    { L_,  "ABCDEFGHIJ", 4,    "rbbrbbbbb",           "JIHGEDCBA",
                         4,    "bbbrxxxbrxxxbbxxbxx", "DHIJxxxEGxxxBCxxAxx"  },
    { L_,  "ABCDEFGHIJ", 5,    "rbbrbbbbb",           "JIHGFDCBA",
                         4,    "bbbrxxxbrxxxbbxxbxx", "DHIJxxxFGxxxBCxxAxx"  },
    { L_,  "ABCDEFGHIJ", 6,    "rbbbbbbbr",           "JIHGFECBA",
                         4,    "bbbrxxxbxxbbxxbxrxx", "FHIJxxxGxxCExxBxAxx"  },
    { L_,  "ABCDEFGHIJ", 7,    "rbbbbbbbr",           "JIHGFEDBA",
                         4,    "bbbrxxxbxxbbxxbxrxx", "FHIJxxxGxxDExxBxAxx"  },
    { L_,  "ABCDEFGHIJ", 8,    "rbbbbbbrb",           "JIHGFEDCA",
                         4,    "bbbrxxxbxxbbxxbrxxx", "FHIJxxxGxxDExxACxxx"  },
    { L_,  "ABCDEFGHIJ", 9,    "rbbbbbbrb",           "JIHGFEDCB",
                         4,    "bbbrxxxbxxbbxxbrxxx", "FHIJxxxGxxDExxBCxxx"  },
};

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

// sortedints.h

class my_SortedInts {
    // This class is a container for a sorted collection of 'int' values.
    // The values are maintained in ascending order.  Duplicates are allowed.

    bdeci_RbTreeImp d_tree;   // Red-black tree container of values.

    // PRIVATE METHODS
    void printTreeImp(ostream&                   stream,
                      const bdeci_RbTreeImpIter& iterator,
                      int                        depth) const;
        // Recursively output the sorted collection of integers in the sub-tree
        // associated with the specified 'iterator' to the specified 'stream'.
        // Output one value per line with the value indented by the specified
        // 'depth' number of spaces to indicate the depth of the value in the
        // containing red-black tree.  Each line is prefixed with an initial
        // "T" to indicate that the collection is output in tree format.

  public:
    // CREATORS
    my_SortedInts();
        // Create a sorted collection of integers that is initially empty.

    ~my_SortedInts();
        // Destroy this sorted collection of integers.

    // MANIPULATORS
    void insert(int value);
        // Insert into this sorted collection of integers the specified
        // 'value' in O[log(N)] time.

    void insert(const int *sortedArray, int numElements);
        // Insert into this sorted collection of integers the specified
        // 'numElements' values from the specified 'sortedArray'.  The
        // behavior is undefined unless 0 <= numElements and 'sortedArray'
        // is the address of an array containing 'numElements' values sorted
        // in ascending order.

    void removeEven();
        // Remove the even values from this sorted collection of integers.

    void removeOdd();
        // Remove the odd values from this sorted collection of integers.

    // ACCESSORS
    int length();
        // Return the number of elements in this collection.

    void printList(ostream& stream) const;
        // Output this sorted collection of integers to the specified 'stream'.
        // Output all values in ascending order on a single line.  Prefix the
        // line with an initial "L" to indicate that the values are output in
        // list format.

    void printTree(ostream& stream) const;
        // Output this sorted collection of integers to the specified 'stream'.
        // Output one value per line with the value indented by an amount
        // proportional to the value's depth in the containing red-black tree.
        // Prefix each line with an initial "T" to indicate that the collection
        // is output in tree format.
};

// FREE OPERATORS
ostream& operator<<(ostream& stream, const my_SortedInts& rhs);
// Write the specified 'rhs' sorted collection of integers to the specified
// output 'stream' and return a reference to the modifiable 'stream'.  Output
// the values in the collection twice.  First output the collection linearly
// (in list format); then output it hierarchically (in tree format).


// sortedints.cpp

// PRIVATE METHODS
void my_SortedInts::printTreeImp(ostream&                   stream,
                                 const bdeci_RbTreeImpIter& iterator,
                                 int                        depth) const
{
    enum { INDENT = 2 };
    if (iterator) {
        stream << 'T';
        int n = depth;
        while (n-- > 0) {
            stream << ' ';
        }
        stream << *(int *)iterator() << endl;

        if (iterator.hasLeft()) {
            bdeci_RbTreeImpIter left(iterator());
            left.moveLeft();
            printTreeImp(stream, left, depth + INDENT);
        }
        if (iterator.hasRight()) {
            bdeci_RbTreeImpIter right(iterator());
            right.moveRight();
            printTreeImp(stream, right, depth + INDENT);
        }
    }
}

// CREATORS
my_SortedInts::my_SortedInts() : d_tree(sizeof(int)) {}

my_SortedInts::~my_SortedInts() {}

// MANIPULATORS
void my_SortedInts::insert(int value)
{
    bdeci_RbTreeImpIter it(d_tree);
    bdeci_RbTreeImpIter prev;       // Created in "unassociated" state.

    while (it) {          // Locate position to insert 'value'.
        const int x = *(int *)it();
        if (x > value) {
            it.moveLeft();
        }
        else {
            prev = it;
            if (x == value) {
                break;    // allow duplicates
            }
            else {
                it.moveRight();
            }
        }
    }

    int *node;
    if (!prev) {
        node = (int *)d_tree.insertAsFirst();
    }
    else {
        node = (int *)d_tree.insertAfter(prev());
    }
    *node = value;
}

void my_SortedInts::insert(const int *sortedArray, int numElements)
{
    bdeci_RbTreeImpBatchManip manip(&d_tree);   // "batch" manipulator

    for (int i = 0; i < numElements; ++i) {
        const int value = sortedArray[i];
        while (manip) {    // Locate position to insert 'value'.
            const int x = *(int *)manip();
            if (x >= value) {
                break;
            }
            else {
                manip.advance();
            }
        }

        int *node = (int *)manip.insertBefore();
        *node = value;
    }
}

void my_SortedInts::removeOdd()
{
    bdeci_RbTreeImpManip manip(&d_tree);        // Non-"batch" manipulator.

    while (manip) {
        const int x = *(int *)manip();
        if (x % 2) {
            manip.remove();
        }
        else {
            manip.advance();
        }
    }
}

// ACCESSORS
int my_SortedInts::length()
{
    return d_tree.length();
}

void my_SortedInts::printList(ostream& stream) const
{
    bdeci_RbTreeImpIter it(d_tree.first());

    stream << "L";
    while (it) {
        stream << *(int *)it() << ' ';
        it.moveNext();
    }
    stream << endl;
}

void my_SortedInts::printTree(ostream& stream) const
{
    printTreeImp(stream, bdeci_RbTreeImpIter(d_tree.root()), 0);
}

// FREE OPERATORS
ostream& operator<<(ostream& stream, const my_SortedInts& rhs)
{
    rhs.printList(stream);
    rhs.printTree(stream);

    return stream;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

#if defined(BDES_PLATFORM__CMP_EPC)
    // The EPC C++ compiler on DGUX has a bug in the constructor for
    // ostrstream that take a buffer and a length.  The constructor will
    // destroy the last byte of the variable immediately preceding the
    // buffer used to hold the stream.  If the parameter specifying the
    // buffer passed to the constructor is 1 less than the size of the buffer
    // stack integrity is maintained.
    const int epcBug = 1;
#else
    const int epcBug = 0;
#endif

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bdema_TestAllocator testAllocator(veryVeryVerbose);
    bdema_Allocator     *Z = &testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
      case 16: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.  Suppress
        //   all 'cout' statements in non-verbose mode, and add streaming to
        //   a buffer to test programmatically the printing examples.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Usage Example"
                          << endl << "=====================" << endl;

        const int SIZE = 1024;
        char buf[SIZE + epcBug];
        ostrstream out(buf, SIZE);

        my_SortedInts x;       ASSERT(0 == x.length());

        // 1.
        if (veryVerbose) cout << "1. Insert selected integers." << endl;
        x.insert(4);           ASSERT(1 == x.length());
        x.insert(6);           ASSERT(2 == x.length());
        x.insert(6);           ASSERT(3 == x.length());
        x.insert(3);           ASSERT(4 == x.length());
        x.insert(2);           ASSERT(5 == x.length());
        x.insert(17);          ASSERT(6 == x.length());
        x.insert(5);           ASSERT(7 == x.length());

        // 1a.
#define NL "\n"
        const char *OUT1 = "L2 3 4 5 6 6 17 "  NL
                           "T6"                NL
                           "T  3"              NL
                           "T    2"            NL
                           "T    4"            NL
                           "T      5"          NL
                           "T  6"              NL
                           "T    17"           NL;
        if (veryVerbose) cout << "EXPECTED FORMAT:" << endl << OUT1 << endl;
        out.seekp(0);  out << x << ends;
        if (veryVerbose) cout << "ACTUAL FORMAT:"   << endl << buf   << endl;
        ASSERT(0 == memcmp(buf, OUT1, std::strlen(OUT1)));

        // 2.
        if (veryVerbose) cout << "2. Batch-insert an array of integers."
                              << endl;
        const int V[] = { 3, 8, 9, 12 };
        x.insert(V, sizeof V / sizeof *V);       ASSERT(11 == x.length());

        // 2a.
        const char *OUT2 = "L2 3 3 4 5 6 6 8 9 12 17 "  NL
                           "T4"                         NL
                           "T  3"                       NL
                           "T    2"                     NL
                           "T    3"                     NL
                           "T  8"                       NL
                           "T    6"                     NL
                           "T      5"                   NL
                           "T      6"                   NL
                           "T    12"                    NL
                           "T      9"                   NL
                           "T      17"                  NL;
        if (veryVerbose) cout << "EXPECTED FORMAT:" << endl << OUT2 << endl;
        out.seekp(0);  out << x << ends;
        if (veryVerbose) cout << "ACTUAL FORMAT:"   << endl << buf   << endl;
        ASSERT(0 == memcmp(buf, OUT2, std::strlen(OUT2)));

        // 3.
        if (veryVerbose) cout << "3. Remove odd integers." << endl;
        x.removeOdd();           ASSERT(6 == x.length());

        // 3a.
        const char *OUT3 = "L2 4 6 6 8 12 "  NL
                           "T8"              NL
                           "T  4"            NL
                           "T    2"          NL
                           "T    6"          NL
                           "T      6"        NL
                           "T  12"           NL;
#undef NL

        if (veryVerbose) cout << "EXPECTED FORMAT:" << endl << OUT3 << endl;
        out.seekp(0);  out << x << ends;
        if (veryVerbose) cout << "ACTUAL FORMAT:"   << endl << buf   << endl;
        ASSERT(0 == memcmp(buf, OUT3, std::strlen(OUT3)));
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'bdeci_RbTreeImpBatchManip'
        //
        // Concerns:
        //   We have the following specific concerns:
        //    1. The constructor initializes a manipulator to be positioned
        //       at the first node of a tree.  The destructor is trivial.
        //    2. The 'advance' manipulator and direct accessors operate as
        //       expected.
        //    3. insert:
        //        a. A new node may be inserted after any node of a tree
        //           ('insertAfter'); a new node may be inserted before any
        //           position in a tree ('insertBefore').
        //        b. Each insertion operation produces the expected result:
        //            i.   The sequence of nodes is correct.
        //            ii.  The result is a valid red-black tree.
        //            iii. (white-box) The internal pointers (i.e., parent,
        //                             child, next) are consistent.
        //        c. The current member of the manipulator is correct.
        //        d. The container increases capacity as needed.
        //        e. The methods are exception neutral w.r.t. allocation.
        //    4. remove:
        //        a. Any node may be removed from a tree.
        //        b. Each removal operation produces the expected result:
        //            i.   The sequence of nodes is correct.
        //            ii.  The result is a valid red-black tree.
        //            iii. (white-box) The internal pointers (i.e., parent,
        //                             child, next) are consistent.
        //        c. The current member of the manipulator is correct.
        //        d. Memory is released back to the allocator for later reuse.
        //
        // Plan:
        //   To address concerns 1 & 2, specify a set S of representative trees
        //   ordered by increasing length.  For each value t in S, initialize a
        //   newly-constructed manipulator m with t.  Using direct accessors,
        //   verify that m is initially positioned at the first node of t.
        //   Then use the 'advance' method to iterate over the entire tree.
        //   Assert the expected result of relevant accessors at each step in
        //   the iteration.
        //
        //   To address concern 3, specify a set S of representative trees
        //   ordered by increasing length.  For each value t in S, initialize a
        //   newly-constructed manipulator m with t.  Then:
        //    1. 'insertAfter' a new node following every node in the tree and
        //       verify that the expected result is produced using the
        //       appropriate helper functions.
        //    2. Insert a new node at every position in t (via 'insertBefore')
        //       and again use our helper functions to verify the result.
        //   Verify that the current member of m is unchanged.  All insertion
        //   tests are performed within the 'bdema' exception testing
        //   apparatus.
        //
        //   To address concern 4, specify a set S of representative trees
        //   ordered by increasing length.  For each tree t in S, initialize a
        //   newly-constructed manipulator m with t.  Test that any node may be
        //   removed and verify that the expected result is produced using the
        //   appropriate helper functions.  Verify that the current member of m
        //   is advanced to the successor of the removed node.  To address
        //   concern 4d, remove all nodes from t, then repopulate t with the
        //   same number of nodes that it contained before being emptied.
        //   Verify that no new memory is allocated while repopulating t.
        //
        // Testing:
        //  ^void bdeci_RbTreeImp::listToTree();
        //
        //   bdeci_RbTreeImpBatchManip(bdeci_RbTreeImp *tree);
        //   ~bdeci_RbTreeImpBatchManip();
        //   void *operator()();
        //   void advance();
        //   void *insertAfter();
        //   void *insertBefore();
        //   void remove();
        //   operator const void *() const;
        //   const void *operator()() const;
        //
        // Note: '^' indicates a private method which is tested indirectly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'bdeci_RbTreeImpBatchManip'"
                          << endl << "==================================="
                          << endl;

        if (verbose)
            cout << "\tTesting primary manipulators and direct accessors."
                 << endl;

        {
            static const struct {
                int         d_lineNum;    // source line number
                const char *d_spec;       // specification string
                const char *d_listColors; // expected r-b list coloration
                const char *d_listTags;   // expected r-b list node tags
                int         d_height;     // expected r-b tree height
                const char *d_treeColors; // expected r-b tree coloration
                const char *d_treeTags;   // expected r-b tree node tags
            } DATA[] = {                                              // ADJUST
                //line spec          ht   list/tree colors   list/tree tags
                //---- ----          --   ----------------   --------------

                { L_,  "",                "",                "",
                                     0,   "x",               "x"             },

                { L_,  "A",               "b",               "A",
                                     1,   "bxx",             "Axx"           },

                { L_,  "AB",              "br",              "BA",
                                     2,   "bxrxx",           "BxAxx"         },

                { L_,  "ABC",             "bbb",             "CBA",
                                     2,   "bbxxbxx",         "BCxxAxx"       },

                { L_,  "ABCD",            "bbbr",            "DCBA",
                                     3,   "bbxxbxrxx",       "CDxxBxAxx"     },

                { L_,  "ABCDE",           "bbbrb",           "EDCBA",
                                     3,   "bbxxrbxxbxx",     "DExxBCxxAxx"   },

                { L_,  "ABCDEF",          "bbbrbr",          "FEDCBA",
                                     4,   "bbxxrbxxbxrxx",   "EFxxCDxxBxAxx" },

                { L_,  "ABCDEFG",         "bbbbbbb",         "GFEDCBA",
                                     4,   "bbbxxbxxbbxxbxx",
                                                           "DFGxxExxBCxxAxx" },

                { L_,  "ABCDEFGH",        "bbbbbbbr",        "HGFEDCBA",
                                     4,   "bbbxxbxxbbxxbxrxx",
                                                         "EGHxxFxxCDxxBxAxx" },

                { L_,  "ABCDEFGHI",       "bbbbbbbrb",       "IHGFEDCBA",
                                     4,   "bbbxxbxxbbxxrbxxbxx",
                                                       "FHIxxGxxDExxBCxxAxx" },

                { L_,  "ABCDEFGHIJ",      "bbbbbbbrbr",      "JIHGFEDCBA",
                                     5,   "bbbxxbxxbbxxrbxxbxrxx",
                                                     "GIJxxHxxEFxxCDxxBxAxx" },

                { L_,  "ABCDEFGHIJK",     "bbbbbbbrbbb",     "KJIHGFEDCBA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "HJKxxIxxDFGxxExxBCxxAxx" },

                { L_,  "ABCDEFGHIJKL",    "bbbbbbbrbbbr",    "LKJIHGFEDCBA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxrxx",
                                                 "IKLxxJxxEGHxxFxxCDxxBxAxx" },

                { L_,  "ABCDEFGHIJKLM",   "bbbbbbbrbbbrb",   "MLKJIHGFEDCBA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxrbxxbxx",
                                               "JLMxxKxxFHIxxGxxDExxBCxxAxx" },

                { L_,  "ABCDEFGHIJKLMN",  "bbbbbbbrbbbrbr",  "NMLKJIHGFEDCBA",
                                     6,   "bbbxxbxxrbbxxbxxbbxxrbxxbxrxx",
                                             "KMNxxLxxGIJxxHxxEFxxCDxxBxAxx" },

                { L_,  "ABCDEFGHIJKLMNO", "bbbbbbbbbbbbbbb", "ONMLKJIHGFEDCBA",
                                     6,   "bbbbxxbxxbbxxbxxbbbxxbxxbbxxbxx",
                                           "HLNOxxMxxJKxxIxxDFGxxExxBCxxAxx" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec;
                const char *const LCOLS = DATA[ti].d_listColors;
                const char *const LTAGS = DATA[ti].d_listTags;
                const int         HT    = DATA[ti].d_height;
                const char *const TCOLS = DATA[ti].d_treeColors;
                const char *const TTAGS = DATA[ti].d_treeTags;
                if (veryVerbose) { P_(LINE); P_(SPEC); P_(LCOLS); P(LTAGS); }
                const int specLen = std::strlen(SPEC);
                ASSERT(specLen  == (int)std::strlen(LCOLS));
                ASSERT(specLen  == (int)std::strlen(LTAGS));
                const int numNodes = std::strlen(TCOLS);
                ASSERT(numNodes == (int)std::strlen(TTAGS));

                for (int zi = 0; zi < NUM_NB; ++zi) {
                    Obj mX(NB[zi]);  const Obj& X = gg(&mX, SPEC);
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\tBEFORE:"; P_(NB[zi]); P(X); }
                    {
                        // In nested scope to exercise 'listToTree'.
                        BatchManip mB(&mX);  const BatchManip& B = mB;
                        LOOP2_ASSERT(LINE, zi,
                                     specLen == 0 || (B && B() == X.first()));
                        for (int i = 0; i < specLen; ++i) {
                            LOOP3_ASSERT(LINE, zi, i, B);
                            void *vp = mB();
                            LOOP3_ASSERT(LINE, zi, i, 0 != vp);
                            const void *cvp = B();
                            LOOP3_ASSERT(LINE, zi, i, 0 != cvp);
                            LOOP3_ASSERT(LINE, zi, i, LTAGS[i] == *(char *)vp);
                            LOOP3_ASSERT(LINE, zi, i,
                                               LTAGS[i] == *(const char *)cvp);
                            mB.advance();
                        }
                        LOOP2_ASSERT(LINE, zi, !B);
                    }
                    if (veryVerbose) { cout << "\t AFTER:"; P_(NB[zi]); P(X); }

                    Iter mI(X.first());  const Iter& I = mI;
                    const char *colors = LCOLS;
                    const char *tags   = LTAGS;
                    for (; *tags && *colors; ++tags, ++colors) {
                        ASSERT('r' == *colors || 'b' == *colors);
                        ASSERT(isupper(*tags));

                        LOOP2_ASSERT(LINE, zi, I && I());

                        const int isRed = *colors == 'r';
                        LOOP2_ASSERT(LINE, zi,  isRed == I.isRed());
                        LOOP2_ASSERT(LINE, zi, !isRed == I.isBlack());

                        const char *tag = (char *)mI();
                        LOOP2_ASSERT(LINE, zi, *tag == *tags);

                        mI.moveNext();
                    }
                    LOOP2_ASSERT(LINE, zi, !I);
                    LOOP2_ASSERT(LINE, zi,
                            numNodes == walkRBTree(Iter(X), HT, TCOLS, TTAGS));
                }
            }
        }

        if (verbose) cout << "\tTesting 'insertAfter' method." << endl;
        {
            static const struct {
                int         d_lineNum;       // source line number
                const char *d_spec;          // specification string
                int         d_pos;           // insert after this position
                const char *d_listColors;    // expected r-b list coloration
                const char *d_listTags;      // expected r-b list node tags
                int         d_height;        // expected r-b tree height
                const char *d_treeColors;    // expected r-b tree coloration
                const char *d_treeTags;      // expected r-b tree node tags
            } DATA[] = {                                              // ADJUST
                //line spec      pos/ht   list/tree colors  list/tree tags
                //---- ----      ------   ----------------  --------------
                { L_,  "A",          0,   "br",             "AX",
                                     2,   "bxrxx",          "AxXxx"          },

                { L_,  "AB",         0,   "bbb",            "BXA",
                                     2,   "bbxxbxx",        "XBxxAxx"        },
                { L_,  "AB",         1,   "bbb",            "BAX",
                                     2,   "bbxxbxx",        "ABxxXxx"        },

                { L_,  "ABC",        0,   "bbbr",           "CXBA",
                                     3,   "bbxxbxrxx",      "XCxxBxAxx"      },
                { L_,  "ABC",        1,   "bbbr",           "CBXA",
                                     3,   "bbxxbxrxx",      "BCxxXxAxx"      },
                { L_,  "ABC",        2,   "bbbr",           "CBAX",
                                     3,   "bbxxbxrxx",      "BCxxAxXxx"      },

                { L_,  "ABCD",       0,   "bbbrb",          "DXCBA",
                                     3,   "bbxxrbxxbxx",    "XDxxBCxxAxx"    },
                { L_,  "ABCD",       1,   "bbbrb",          "DCXBA",
                                     3,   "bbxxrbxxbxx",    "CDxxBXxxAxx"    },
                { L_,  "ABCD",       2,   "bbbrb",          "DCBXA",
                                     3,   "bbxxrbxxbxx",    "CDxxXBxxAxx"    },
                { L_,  "ABCD",       3,   "bbbrb",          "DCBAX",
                                     3,   "bbxxrbxxbxx",    "CDxxABxxXxx"    },

                { L_,  "ABCDE",      0,   "bbbrbr",         "EXDCBA",
                                     4,   "bbxxrbxxbxrxx",  "XExxCDxxBxAxx"  },
                { L_,  "ABCDE",      1,   "bbbrbr",         "EDXCBA",
                                     4,   "bbxxrbxxbxrxx",  "DExxCXxxBxAxx"  },
                { L_,  "ABCDE",      2,   "bbbrbr",         "EDCXBA",
                                     4,   "bbxxrbxxbxrxx",  "DExxXCxxBxAxx"  },
                { L_,  "ABCDE",      3,   "bbbrbr",         "EDCBXA",
                                     4,   "bbxxrbxxbxrxx",  "DExxBCxxXxAxx"  },
                { L_,  "ABCDE",      4,   "bbbrbr",         "EDCBAX",
                                     4,   "bbxxrbxxbxrxx",  "DExxBCxxAxXxx"  },

                { L_,  "ABCDEF",     0,   "bbbbbbb",        "FXEDCBA",
                                     4,   "bbbxxbxxbbxxbxx",
                                                           "DXFxxExxBCxxAxx" },
                { L_,  "ABCDEF",     1,   "bbbbbbb",        "FEXDCBA",
                                     4,   "bbbxxbxxbbxxbxx",
                                                           "DEFxxXxxBCxxAxx" },
                { L_,  "ABCDEF",     2,   "bbbbbbb",        "FEDXCBA",
                                     4,   "bbbxxbxxbbxxbxx",
                                                           "XEFxxDxxBCxxAxx" },
                { L_,  "ABCDEF",     3,   "bbbbbbb",        "FEDCXBA",
                                     4,   "bbbxxbxxbbxxbxx",
                                                           "CEFxxDxxBXxxAxx" },
                { L_,  "ABCDEF",     4,   "bbbbbbb",        "FEDCBXA",
                                     4,   "bbbxxbxxbbxxbxx",
                                                           "CEFxxDxxXBxxAxx" },
                { L_,  "ABCDEF",     5,   "bbbbbbb",        "FEDCBAX",
                                     4,   "bbbxxbxxbbxxbxx",
                                                           "CEFxxDxxABxxXxx" },

                { L_,  "ABCDEFG",    0,   "bbbbbbbr",       "GXFEDCBA",
                                     4,   "bbbxxbxxbbxxbxrxx",
                                                         "EXGxxFxxCDxxBxAxx" },
                { L_,  "ABCDEFG",    1,   "bbbbbbbr",       "GFXEDCBA",
                                     4,   "bbbxxbxxbbxxbxrxx",
                                                         "EFGxxXxxCDxxBxAxx" },
                { L_,  "ABCDEFG",    2,   "bbbbbbbr",       "GFEXDCBA",
                                     4,   "bbbxxbxxbbxxbxrxx",
                                                         "XFGxxExxCDxxBxAxx" },
                { L_,  "ABCDEFG",    3,   "bbbbbbbr",       "GFEDXCBA",
                                     4,   "bbbxxbxxbbxxbxrxx",
                                                         "DFGxxExxCXxxBxAxx" },
                { L_,  "ABCDEFG",    4,   "bbbbbbbr",       "GFEDCXBA",
                                     4,   "bbbxxbxxbbxxbxrxx",
                                                         "DFGxxExxXCxxBxAxx" },
                { L_,  "ABCDEFG",    5,   "bbbbbbbr",       "GFEDCBXA",
                                     4,   "bbbxxbxxbbxxbxrxx",
                                                         "DFGxxExxBCxxXxAxx" },
                { L_,  "ABCDEFG",    6,   "bbbbbbbr",       "GFEDCBAX",
                                     4,   "bbbxxbxxbbxxbxrxx",
                                                         "DFGxxExxBCxxAxXxx" },

                { L_,  "ABCDEFGH",   0,   "bbbbbbbrb",      "HXGFEDCBA",
                                     4,   "bbbxxbxxbbxxrbxxbxx",
                                                       "FXHxxGxxDExxBCxxAxx" },
                { L_,  "ABCDEFGH",   1,   "bbbbbbbrb",      "HGXFEDCBA",
                                     4,   "bbbxxbxxbbxxrbxxbxx",
                                                       "FGHxxXxxDExxBCxxAxx" },
                { L_,  "ABCDEFGH",   2,   "bbbbbbbrb",      "HGFXEDCBA",
                                     4,   "bbbxxbxxbbxxrbxxbxx",
                                                       "XGHxxFxxDExxBCxxAxx" },
                { L_,  "ABCDEFGH",   3,   "bbbbbbbrb",      "HGFEXDCBA",
                                     4,   "bbbxxbxxbbxxrbxxbxx",
                                                       "EGHxxFxxDXxxBCxxAxx" },
                { L_,  "ABCDEFGH",   4,   "bbbbbbbrb",      "HGFEDXCBA",
                                     4,   "bbbxxbxxbbxxrbxxbxx",
                                                       "EGHxxFxxXDxxBCxxAxx" },
                { L_,  "ABCDEFGH",   5,   "bbbbbbbrb",      "HGFEDCXBA",
                                     4,   "bbbxxbxxbbxxrbxxbxx",
                                                       "EGHxxFxxCDxxBXxxAxx" },
                { L_,  "ABCDEFGH",   6,   "bbbbbbbrb",      "HGFEDCBXA",
                                     4,   "bbbxxbxxbbxxrbxxbxx",
                                                       "EGHxxFxxCDxxXBxxAxx" },
                { L_,  "ABCDEFGH",   7,   "bbbbbbbrb",      "HGFEDCBAX",
                                     4,   "bbbxxbxxbbxxrbxxbxx",
                                                       "EGHxxFxxCDxxABxxXxx" },

                { L_,  "ABCDEFGHI",  0,   "bbbbbbbrbr",     "IXHGFEDCBA",
                                     5,   "bbbxxbxxbbxxrbxxbxrxx",
                                                     "GXIxxHxxEFxxCDxxBxAxx" },
                { L_,  "ABCDEFGHI",  1,   "bbbbbbbrbr",     "IHXGFEDCBA",
                                     5,   "bbbxxbxxbbxxrbxxbxrxx",
                                                     "GHIxxXxxEFxxCDxxBxAxx" },
                { L_,  "ABCDEFGHI",  2,   "bbbbbbbrbr",     "IHGXFEDCBA",
                                     5,   "bbbxxbxxbbxxrbxxbxrxx",
                                                     "XHIxxGxxEFxxCDxxBxAxx" },
                { L_,  "ABCDEFGHI",  3,   "bbbbbbbrbr",     "IHGFXEDCBA",
                                     5,   "bbbxxbxxbbxxrbxxbxrxx",
                                                     "FHIxxGxxEXxxCDxxBxAxx" },
                { L_,  "ABCDEFGHI",  4,   "bbbbbbbrbr",     "IHGFEXDCBA",
                                     5,   "bbbxxbxxbbxxrbxxbxrxx",
                                                     "FHIxxGxxXExxCDxxBxAxx" },
                { L_,  "ABCDEFGHI",  5,   "bbbbbbbrbr",     "IHGFEDXCBA",
                                     5,   "bbbxxbxxbbxxrbxxbxrxx",
                                                     "FHIxxGxxDExxCXxxBxAxx" },
                { L_,  "ABCDEFGHI",  6,   "bbbbbbbrbr",     "IHGFEDCXBA",
                                     5,   "bbbxxbxxbbxxrbxxbxrxx",
                                                     "FHIxxGxxDExxXCxxBxAxx" },
                { L_,  "ABCDEFGHI",  7,   "bbbbbbbrbr",     "IHGFEDCBXA",
                                     5,   "bbbxxbxxbbxxrbxxbxrxx",
                                                     "FHIxxGxxDExxBCxxXxAxx" },
                { L_,  "ABCDEFGHI",  8,   "bbbbbbbrbr",     "IHGFEDCBAX",
                                     5,   "bbbxxbxxbbxxrbxxbxrxx",
                                                     "FHIxxGxxDExxBCxxAxXxx" },

                { L_,  "ABCDEFGHIJ", 0,   "bbbbbbbrbbb",    "JXIHGFEDCBA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "HXJxxIxxDFGxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 1,   "bbbbbbbrbbb",    "JIXHGFEDCBA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "HIJxxXxxDFGxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 2,   "bbbbbbbrbbb",    "JIHXGFEDCBA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "XIJxxHxxDFGxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 3,   "bbbbbbbrbbb",    "JIHGXFEDCBA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "GIJxxHxxDFXxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 4,   "bbbbbbbrbbb",    "JIHGFXEDCBA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "GIJxxHxxDXFxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 5,   "bbbbbbbrbbb",    "JIHGFEXDCBA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "GIJxxHxxDEFxxXxxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 6,   "bbbbbbbrbbb",    "JIHGFEDXCBA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "GIJxxHxxXEFxxDxxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 7,   "bbbbbbbrbbb",    "JIHGFEDCXBA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "GIJxxHxxCEFxxDxxBXxxAxx" },
                { L_,  "ABCDEFGHIJ", 8,   "bbbbbbbrbbb",    "JIHGFEDCBXA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "GIJxxHxxCEFxxDxxXBxxAxx" },
                { L_,  "ABCDEFGHIJ", 9,   "bbbbbbbrbbb",    "JIHGFEDCBAX",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "GIJxxHxxCEFxxDxxABxxXxx" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec;
                const int         POS   = DATA[ti].d_pos;
                const char *const LCOLS = DATA[ti].d_listColors;
                const char *const LTAGS = DATA[ti].d_listTags;
                const int         HT    = DATA[ti].d_height;
                const char *const TCOLS = DATA[ti].d_treeColors;
                const char *const TTAGS = DATA[ti].d_treeTags;
                if (veryVerbose) { P_(LINE); P_(SPEC); P_(LCOLS); P(LTAGS); }
                const int specLen = std::strlen(SPEC);
                ASSERT(specLen + 1 == (int)std::strlen(LCOLS));
                ASSERT(specLen + 1 == (int)std::strlen(LTAGS));
                const int numNodes = std::strlen(TCOLS);
                ASSERT(numNodes    == (int)std::strlen(TTAGS));

                for (int zi = 0; zi < NUM_NB; ++zi) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj mX(NB[zi]);  const Obj& X = gg(&mX, SPEC);
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\tBEFORE:"; P_(NB[zi]); P(X); }
                    {
                        // In nested scope to exercise 'listToTree'.
                        BatchManip mB(&mX);  const BatchManip& B = mB;
                        for (int i = 0; i < POS; ++i) {
                            mB.advance();
                        }
                        LOOP2_ASSERT(LINE, zi, B);
                        char *node = (char *)mB.insertAfter();
                        *node = 'X';
                        LOOP2_ASSERT(LINE, zi, 'X' == *(char *)B());
                                                        // New node is current.
                    }
                    LOOP2_ASSERT(LINE, zi, specLen + 1 == X.length());
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\t AFTER:"; P_(NB[zi]); P(X); }

                    Iter mI(X.first());  const Iter& I = mI;
                    const char *colors = LCOLS;
                    const char *tags   = LTAGS;
                    for (; *tags && *colors; ++tags, ++colors) {
                        ASSERT('r' == *colors || 'b' == *colors);
                        ASSERT(isupper(*tags));

                        LOOP2_ASSERT(LINE, zi, I && I());

                        const int isRed = *colors == 'r';
                        LOOP2_ASSERT(LINE, zi,  isRed == I.isRed());
                        LOOP2_ASSERT(LINE, zi, !isRed == I.isBlack());

                        const char *tag = (char *)mI();
                        LOOP2_ASSERT(LINE, zi, *tag == *tags);

                        mI.moveNext();
                    }
                    LOOP2_ASSERT(LINE, zi, !I);
                    LOOP2_ASSERT(LINE, zi,
                            numNodes == walkRBTree(Iter(X), HT, TCOLS, TTAGS));
                  } END_BDEMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout << "\tTesting 'insertBefore' method." << endl;
        {
            static const struct {
                int         d_lineNum;       // source line number
                const char *d_spec;          // specification string
                int         d_pos;           // insert before this position
                const char *d_listColors;    // expected r-b list coloration
                const char *d_listTags;      // expected r-b list node tags
                int         d_height;        // expected r-b tree height
                const char *d_treeColors;    // expected r-b tree coloration
                const char *d_treeTags;      // expected r-b tree node tags
            } DATA[] = {                                              // ADJUST
                //line spec      pos/ht   list/tree colors  list/tree tags
                //---- ----      ------   ----------------  --------------
                { L_,  "",           0,   "b",              "X",
                                     1,   "bxx",            "Xxx"            },

                { L_,  "A",          0,   "br",             "XA",
                                     2,   "bxrxx",          "XxAxx"          },
                { L_,  "A",          1,   "br",             "AX",
                                     2,   "bxrxx",          "AxXxx"          },

                { L_,  "AB",         0,   "bbb",            "XBA",
                                     2,   "bbxxbxx",        "BXxxAxx"        },
                { L_,  "AB",         1,   "bbb",            "BXA",
                                     2,   "bbxxbxx",        "XBxxAxx"        },
                { L_,  "AB",         2,   "bbb",            "BAX",
                                     2,   "bbxxbxx",        "ABxxXxx"        },

                { L_,  "ABC",        0,   "bbbr",           "XCBA",
                                     3,   "bbxxbxrxx",      "CXxxBxAxx"      },
                { L_,  "ABC",        1,   "bbbr",           "CXBA",
                                     3,   "bbxxbxrxx",      "XCxxBxAxx"      },
                { L_,  "ABC",        2,   "bbbr",           "CBXA",
                                     3,   "bbxxbxrxx",      "BCxxXxAxx"      },
                { L_,  "ABC",        3,   "bbbr",           "CBAX",
                                     3,   "bbxxbxrxx",      "BCxxAxXxx"      },

                { L_,  "ABCD",       0,   "bbbrb",          "XDCBA",
                                     3,   "bbxxrbxxbxx",    "DXxxBCxxAxx"    },
                { L_,  "ABCD",       1,   "bbbrb",          "DXCBA",
                                     3,   "bbxxrbxxbxx",    "XDxxBCxxAxx"    },
                { L_,  "ABCD",       2,   "bbbrb",          "DCXBA",
                                     3,   "bbxxrbxxbxx",    "CDxxBXxxAxx"    },
                { L_,  "ABCD",       3,   "bbbrb",          "DCBXA",
                                     3,   "bbxxrbxxbxx",    "CDxxXBxxAxx"    },
                { L_,  "ABCD",       4,   "bbbrb",          "DCBAX",
                                     3,   "bbxxrbxxbxx",    "CDxxABxxXxx"    },

                { L_,  "ABCDE",      0,   "bbbrbr",         "XEDCBA",
                                     4,   "bbxxrbxxbxrxx",  "EXxxCDxxBxAxx"  },
                { L_,  "ABCDE",      1,   "bbbrbr",         "EXDCBA",
                                     4,   "bbxxrbxxbxrxx",  "XExxCDxxBxAxx"  },
                { L_,  "ABCDE",      2,   "bbbrbr",         "EDXCBA",
                                     4,   "bbxxrbxxbxrxx",  "DExxCXxxBxAxx"  },
                { L_,  "ABCDE",      3,   "bbbrbr",         "EDCXBA",
                                     4,   "bbxxrbxxbxrxx",  "DExxXCxxBxAxx"  },
                { L_,  "ABCDE",      4,   "bbbrbr",         "EDCBXA",
                                     4,   "bbxxrbxxbxrxx",  "DExxBCxxXxAxx"  },
                { L_,  "ABCDE",      5,   "bbbrbr",         "EDCBAX",
                                     5,   "bbxxrbxxbxrxx",  "DExxBCxxAxXxx"  },

                { L_,  "ABCDEF",     0,   "bbbbbbb",        "XFEDCBA",
                                     4,   "bbbxxbxxbbxxbxx",
                                                           "DFXxxExxBCxxAxx" },
                { L_,  "ABCDEF",     1,   "bbbbbbb",        "FXEDCBA",
                                     4,   "bbbxxbxxbbxxbxx",
                                                           "DXFxxExxBCxxAxx" },
                { L_,  "ABCDEF",     2,   "bbbbbbb",        "FEXDCBA",
                                     4,   "bbbxxbxxbbxxbxx",
                                                           "DEFxxXxxBCxxAxx" },
                { L_,  "ABCDEF",     3,   "bbbbbbb",        "FEDXCBA",
                                     4,   "bbbxxbxxbbxxbxx",
                                                           "XEFxxDxxBCxxAxx" },
                { L_,  "ABCDEF",     4,   "bbbbbbb",        "FEDCXBA",
                                     4,   "bbbxxbxxbbxxbxx",
                                                           "CEFxxDxxBXxxAxx" },
                { L_,  "ABCDEF",     5,   "bbbbbbb",        "FEDCBXA",
                                     4,   "bbbxxbxxbbxxbxx",
                                                           "CEFxxDxxXBxxAxx" },
                { L_,  "ABCDEF",     6,   "bbbbbbb",        "FEDCBAX",
                                     4,   "bbbxxbxxbbxxbxx",
                                                           "CEFxxDxxABxxXxx" },

                { L_,  "ABCDEFG",    0,   "bbbbbbbr",       "XGFEDCBA",
                                     4,   "bbbxxbxxbbxxbxrxx",
                                                         "EGXxxFxxCDxxBxAxx" },
                { L_,  "ABCDEFG",    1,   "bbbbbbbr",       "GXFEDCBA",
                                     4,   "bbbxxbxxbbxxbxrxx",
                                                         "EXGxxFxxCDxxBxAxx" },
                { L_,  "ABCDEFG",    2,   "bbbbbbbr",       "GFXEDCBA",
                                     4,   "bbbxxbxxbbxxbxrxx",
                                                         "EFGxxXxxCDxxBxAxx" },
                { L_,  "ABCDEFG",    3,   "bbbbbbbr",       "GFEXDCBA",
                                     4,   "bbbxxbxxbbxxbxrxx",
                                                         "XFGxxExxCDxxBxAxx" },
                { L_,  "ABCDEFG",    4,   "bbbbbbbr",       "GFEDXCBA",
                                     4,   "bbbxxbxxbbxxbxrxx",
                                                         "DFGxxExxCXxxBxAxx" },
                { L_,  "ABCDEFG",    5,   "bbbbbbbr",       "GFEDCXBA",
                                     4,   "bbbxxbxxbbxxbxrxx",
                                                         "DFGxxExxXCxxBxAxx" },
                { L_,  "ABCDEFG",    6,   "bbbbbbbr",       "GFEDCBXA",
                                     4,   "bbbxxbxxbbxxbxrxx",
                                                         "DFGxxExxBCxxXxAxx" },
                { L_,  "ABCDEFG",    7,   "bbbbbbbr",       "GFEDCBAX",
                                     4,   "bbbxxbxxbbxxbxrxx",
                                                         "DFGxxExxBCxxAxXxx" },

                { L_,  "ABCDEFGH",   0,   "bbbbbbbrb",      "XHGFEDCBA",
                                     4,   "bbbxxbxxbbxxrbxxbxx",
                                                       "FHXxxGxxDExxBCxxAxx" },
                { L_,  "ABCDEFGH",   1,   "bbbbbbbrb",      "HXGFEDCBA",
                                     4,   "bbbxxbxxbbxxrbxxbxx",
                                                       "FXHxxGxxDExxBCxxAxx" },
                { L_,  "ABCDEFGH",   2,   "bbbbbbbrb",      "HGXFEDCBA",
                                     4,   "bbbxxbxxbbxxrbxxbxx",
                                                       "FGHxxXxxDExxBCxxAxx" },
                { L_,  "ABCDEFGH",   3,   "bbbbbbbrb",      "HGFXEDCBA",
                                     4,   "bbbxxbxxbbxxrbxxbxx",
                                                       "XGHxxFxxDExxBCxxAxx" },
                { L_,  "ABCDEFGH",   4,   "bbbbbbbrb",      "HGFEXDCBA",
                                     4,   "bbbxxbxxbbxxrbxxbxx",
                                                       "EGHxxFxxDXxxBCxxAxx" },
                { L_,  "ABCDEFGH",   5,   "bbbbbbbrb",      "HGFEDXCBA",
                                     4,   "bbbxxbxxbbxxrbxxbxx",
                                                       "EGHxxFxxXDxxBCxxAxx" },
                { L_,  "ABCDEFGH",   6,   "bbbbbbbrb",      "HGFEDCXBA",
                                     4,   "bbbxxbxxbbxxrbxxbxx",
                                                       "EGHxxFxxCDxxBXxxAxx" },
                { L_,  "ABCDEFGH",   7,   "bbbbbbbrb",      "HGFEDCBXA",
                                     4,   "bbbxxbxxbbxxrbxxbxx",
                                                       "EGHxxFxxCDxxXBxxAxx" },
                { L_,  "ABCDEFGH",   8,   "bbbbbbbrb",      "HGFEDCBAX",
                                     4,   "bbbxxbxxbbxxrbxxbxx",
                                                       "EGHxxFxxCDxxABxxXxx" },

                { L_,  "ABCDEFGHI",  0,   "bbbbbbbrbr",     "XIHGFEDCBA",
                                     5,   "bbbxxbxxbbxxrbxxbxrxx",
                                                     "GIXxxHxxEFxxCDxxBxAxx" },
                { L_,  "ABCDEFGHI",  1,   "bbbbbbbrbr",     "IXHGFEDCBA",
                                     5,   "bbbxxbxxbbxxrbxxbxrxx",
                                                     "GXIxxHxxEFxxCDxxBxAxx" },
                { L_,  "ABCDEFGHI",  2,   "bbbbbbbrbr",     "IHXGFEDCBA",
                                     5,   "bbbxxbxxbbxxrbxxbxrxx",
                                                     "GHIxxXxxEFxxCDxxBxAxx" },
                { L_,  "ABCDEFGHI",  3,   "bbbbbbbrbr",     "IHGXFEDCBA",
                                     5,   "bbbxxbxxbbxxrbxxbxrxx",
                                                     "XHIxxGxxEFxxCDxxBxAxx" },
                { L_,  "ABCDEFGHI",  4,   "bbbbbbbrbr",     "IHGFXEDCBA",
                                     5,   "bbbxxbxxbbxxrbxxbxrxx",
                                                     "FHIxxGxxEXxxCDxxBxAxx" },
                { L_,  "ABCDEFGHI",  5,   "bbbbbbbrbr",     "IHGFEXDCBA",
                                     5,   "bbbxxbxxbbxxrbxxbxrxx",
                                                     "FHIxxGxxXExxCDxxBxAxx" },
                { L_,  "ABCDEFGHI",  6,   "bbbbbbbrbr",     "IHGFEDXCBA",
                                     5,   "bbbxxbxxbbxxrbxxbxrxx",
                                                     "FHIxxGxxDExxCXxxBxAxx" },
                { L_,  "ABCDEFGHI",  7,   "bbbbbbbrbr",     "IHGFEDCXBA",
                                     5,   "bbbxxbxxbbxxrbxxbxrxx",
                                                     "FHIxxGxxDExxXCxxBxAxx" },
                { L_,  "ABCDEFGHI",  8,   "bbbbbbbrbr",     "IHGFEDCBXA",
                                     5,   "bbbxxbxxbbxxrbxxbxrxx",
                                                     "FHIxxGxxDExxBCxxXxAxx" },
                { L_,  "ABCDEFGHI",  9,   "bbbbbbbrbr",     "IHGFEDCBAX",
                                     5,   "bbbxxbxxbbxxrbxxbxrxx",
                                                     "FHIxxGxxDExxBCxxAxXxx" },

                { L_,  "ABCDEFGHIJ", 0,   "bbbbbbbrbbb",    "XJIHGFEDCBA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "HJXxxIxxDFGxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 1,   "bbbbbbbrbbb",    "JXIHGFEDCBA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "HXJxxIxxDFGxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 2,   "bbbbbbbrbbb",    "JIXHGFEDCBA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "HIJxxXxxDFGxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 3,   "bbbbbbbrbbb",    "JIHXGFEDCBA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "XIJxxHxxDFGxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 4,   "bbbbbbbrbbb",    "JIHGXFEDCBA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "GIJxxHxxDFXxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 5,   "bbbbbbbrbbb",    "JIHGFXEDCBA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "GIJxxHxxDXFxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 6,   "bbbbbbbrbbb",    "JIHGFEXDCBA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "GIJxxHxxDEFxxXxxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 7,   "bbbbbbbrbbb",    "JIHGFEDXCBA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "GIJxxHxxXEFxxDxxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 8,   "bbbbbbbrbbb",    "JIHGFEDCXBA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "GIJxxHxxCEFxxDxxBXxxAxx" },
                { L_,  "ABCDEFGHIJ", 9,   "bbbbbbbrbbb",    "JIHGFEDCBXA",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "GIJxxHxxCEFxxDxxXBxxAxx" },
                { L_,  "ABCDEFGHIJ",10,   "bbbbbbbrbbb",    "JIHGFEDCBAX",
                                     5,   "bbbxxbxxrbbxxbxxbbxxbxx",
                                                   "GIJxxHxxCEFxxDxxABxxXxx" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec;
                const int         POS   = DATA[ti].d_pos;
                const char *const LCOLS = DATA[ti].d_listColors;
                const char *const LTAGS = DATA[ti].d_listTags;
                const int         HT    = DATA[ti].d_height;
                const char *const TCOLS = DATA[ti].d_treeColors;
                const char *const TTAGS = DATA[ti].d_treeTags;
                if (veryVerbose) { P_(LINE); P_(SPEC); P_(LCOLS); P(LTAGS); }
                const int specLen = std::strlen(SPEC);
                ASSERT(specLen + 1 == (int)std::strlen(LCOLS));
                ASSERT(specLen + 1 == (int)std::strlen(LTAGS));
                const int numNodes = std::strlen(TCOLS);
                ASSERT(numNodes    == (int)std::strlen(TTAGS));

                for (int zi = 0; zi < NUM_NB; ++zi) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj mX(NB[zi]);  const Obj& X = gg(&mX, SPEC);
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\tBEFORE:"; P_(NB[zi]); P(X); }
                    {
                        // In nested scope to exercise 'listToTree'.
                        BatchManip mB(&mX);  const BatchManip& B = mB;
                        for (int i = 0; i < POS; ++i) {
                            mB.advance();
                        }
                        LOOP2_ASSERT(LINE, zi, (POS == specLen) == !B);
                        char *node = (char *)mB.insertBefore();
                        *node = 'X';
                        LOOP2_ASSERT(LINE, zi, 'X' == *(char *)B());
                                                        // New node is current.
                    }
                    LOOP2_ASSERT(LINE, zi, specLen + 1 == X.length());
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\t AFTER:"; P_(NB[zi]); P(X); }

                    Iter mI(X.first());  const Iter& I = mI;
                    const char *colors = LCOLS;
                    const char *tags   = LTAGS;
                    for (; *tags && *colors; ++tags, ++colors) {
                        ASSERT('r' == *colors || 'b' == *colors);
                        ASSERT(isupper(*tags));

                        LOOP2_ASSERT(LINE, zi, I && I());

                        const int isRed = *colors == 'r';
                        LOOP2_ASSERT(LINE, zi,  isRed == I.isRed());
                        LOOP2_ASSERT(LINE, zi, !isRed == I.isBlack());

                        const char *tag = (char *)mI();
                        LOOP2_ASSERT(LINE, zi, *tag == *tags);

                        mI.moveNext();
                    }
                    LOOP2_ASSERT(LINE, zi, !I);
                    LOOP2_ASSERT(LINE, zi,
                            numNodes == walkRBTree(Iter(X), HT, TCOLS, TTAGS));
                  } END_BDEMA_EXCEPTION_TEST
                }
            }
        }

        static const struct {
            int         d_lineNum;       // source line number
            const char *d_spec;          // specification string
            int         d_pos;           // remove at this position
            const char *d_listColors;    // expected r-b list coloration
            const char *d_listTags;      // expected r-b list node tags
            int         d_height;        // expected r-b tree height
            const char *d_treeColors;    // expected r-b tree coloration
            const char *d_treeTags;      // expected r-b tree node tags
        } DATA[] = {                                                  // ADJUST
            //line spec      pos/ht    list/tree colors   list/tree tags
            //---- ----      ------    ----------------   --------------
            { L_,  "A",          0,    "",                "",
                                 0,    "x",               "x"                },

            { L_,  "AB",         0,    "b",               "A",
                                 1,    "bxx",             "Axx"              },
            { L_,  "AB",         1,    "b",               "B",
                                 1,    "bxx",             "Bxx"              },

            { L_,  "ABC",        0,    "br",              "BA",
                                 2,    "bxrxx",           "BxAxx"            },
            { L_,  "ABC",        1,    "br",              "CA",
                                 2,    "bxrxx",           "CxAxx"            },
            { L_,  "ABC",        2,    "br",              "CB",
                                 2,    "bxrxx",           "CxBxx"            },

            { L_,  "ABCD",       0,    "bbb",             "CBA",
                                 3,    "bbxxbxx",         "BCxxAxx"          },
            { L_,  "ABCD",       1,    "bbb",             "DBA",
                                 3,    "bbxxbxx",         "BDxxAxx"          },
            { L_,  "ABCD",       2,    "bbb",             "DCA",
                                 3,    "bbxxbxx",         "CDxxAxx"          },
            { L_,  "ABCD",       3,    "bbb",             "DCB",
                                 3,    "bbxxbxx",         "CDxxBxx"          },

            { L_,  "ABCDE",      0,    "bbbr",            "DCBA",
                                 3,    "bbxxbxrxx",       "CDxxBxAxx"        },
            { L_,  "ABCDE",      1,    "bbbr",            "ECBA",
                                 3,    "bbxxbxrxx",       "CExxBxAxx"        },
            { L_,  "ABCDE",      2,    "bbbr",            "EDBA",
                                 3,    "bbxxbxrxx",       "DExxBxAxx"        },
            { L_,  "ABCDE",      3,    "bbbr",            "EDCA",
                                 3,    "bbxxbxrxx",       "DExxCxAxx"        },
            { L_,  "ABCDE",      4,    "bbbr",            "EDCB",
                                 3,    "bbxxbxrxx",       "DExxCxBxx"        },

            { L_,  "ABCDEF",     0,    "bbbrb",           "EDCBA",
                                 3,    "bbxxrbxxbxx",     "DExxBCxxAxx"      },
            { L_,  "ABCDEF",     1,    "bbbrb",           "FDCBA",
                                 3,    "bbxxrbxxbxx",     "DFxxBCxxAxx"      },
            { L_,  "ABCDEF",     2,    "bbbrb",           "FECBA",
                                 3,    "bbxxrbxxbxx",     "EFxxBCxxAxx"      },
            { L_,  "ABCDEF",     3,    "bbbrb",           "FEDBA",
                                 3,    "bbxxrbxxbxx",     "EFxxBDxxAxx"      },
            { L_,  "ABCDEF",     4,    "bbbrb",           "FEDCA",
                                 3,    "bbxxrbxxbxx",     "EFxxCDxxAxx"      },
            { L_,  "ABCDEF",     5,    "bbbrb",           "FEDCB",
                                 3,    "bbxxrbxxbxx",     "EFxxCDxxBxx"      },

            { L_,  "ABCDEFG",    0,    "bbbrbr",          "FEDCBA",
                                 4,    "bbxxrbxxbxrxx",   "EFxxCDxxBxAxx"    },
            { L_,  "ABCDEFG",    1,    "bbbrbr",          "GEDCBA",
                                 4,    "bbxxrbxxbxrxx",   "EGxxCDxxBxAxx"    },
            { L_,  "ABCDEFG",    2,    "bbbrbr",          "GFDCBA",
                                 4,    "bbxxrbxxbxrxx",   "FGxxCDxxBxAxx"    },
            { L_,  "ABCDEFG",    3,    "bbbrbr",          "GFECBA",
                                 4,    "bbxxrbxxbxrxx",   "FGxxCExxBxAxx"    },
            { L_,  "ABCDEFG",    4,    "bbbrbr",          "GFEDBA",
                                 4,    "bbxxrbxxbxrxx",   "FGxxDExxBxAxx"    },
            { L_,  "ABCDEFG",    5,    "bbbrbr",          "GFEDCA",
                                 4,    "bbxxrbxxbxrxx",   "FGxxDExxCxAxx"    },
            { L_,  "ABCDEFG",    6,    "bbbrbr",          "GFEDCB",
                                 4,    "bbxxrbxxbxrxx",   "FGxxDExxCxBxx"    },

            { L_,  "ABCDEFGH",   0,    "bbbbbbb",         "GFEDCBA",
                                 4,    "bbbxxbxxbbxxbxx", "DFGxxExxBCxxAxx"  },
            { L_,  "ABCDEFGH",   1,    "bbbbbbb",         "HFEDCBA",
                                 4,    "bbbxxbxxbbxxbxx", "DFHxxExxBCxxAxx"  },
            { L_,  "ABCDEFGH",   2,    "bbbbbbb",         "HGEDCBA",
                                 4,    "bbbxxbxxbbxxbxx", "DGHxxExxBCxxAxx"  },
            { L_,  "ABCDEFGH",   3,    "bbbbbbb",         "HGFDCBA",
                                 4,    "bbbxxbxxbbxxbxx", "DGHxxFxxBCxxAxx"  },
            { L_,  "ABCDEFGH",   4,    "bbbbbbb",         "HGFECBA",
                                 4,    "bbbxxbxxbbxxbxx", "EGHxxFxxBCxxAxx"  },
            { L_,  "ABCDEFGH",   5,    "bbbbbbb",         "HGFEDBA",
                                 4,    "bbbxxbxxbbxxbxx", "EGHxxFxxBDxxAxx"  },
            { L_,  "ABCDEFGH",   6,    "bbbbbbb",         "HGFEDCA",
                                 4,    "bbbxxbxxbbxxbxx", "EGHxxFxxCDxxAxx"  },
            { L_,  "ABCDEFGH",   7,    "bbbbbbb",         "HGFEDCB",
                                 4,    "bbbxxbxxbbxxbxx", "EGHxxFxxCDxxBxx"  },

            { L_,  "ABCDEFGHI",  0,    "bbbbbbbr",        "HGFEDCBA",
                                 4,    "bbbxxbxxbbxxbxrxx",
                                                         "EGHxxFxxCDxxBxAxx" },
            { L_,  "ABCDEFGHI",  1,    "bbbbbbbr",        "IGFEDCBA",
                                 4,    "bbbxxbxxbbxxbxrxx",
                                                         "EGIxxFxxCDxxBxAxx" },
            { L_,  "ABCDEFGHI",  2,    "bbbbbbbr",        "IHFEDCBA",
                                 4,    "bbbxxbxxbbxxbxrxx",
                                                         "EHIxxFxxCDxxBxAxx" },
            { L_,  "ABCDEFGHI",  3,    "bbbbbbbr",        "IHGEDCBA",
                                 4,    "bbbxxbxxbbxxbxrxx",
                                                         "EHIxxGxxCDxxBxAxx" },
            { L_,  "ABCDEFGHI",  4,    "bbbbbbbr",        "IHGFDCBA",
                                 4,    "bbbxxbxxbbxxbxrxx",
                                                         "FHIxxGxxCDxxBxAxx" },
            { L_,  "ABCDEFGHI",  5,    "bbbbbbbr",        "IHGFECBA",
                                 4,    "bbbxxbxxbbxxbxrxx",
                                                         "FHIxxGxxCExxBxAxx" },
            { L_,  "ABCDEFGHI",  6,    "bbbbbbbr",        "IHGFEDBA",
                                 4,    "bbbxxbxxbbxxbxrxx",
                                                         "FHIxxGxxDExxBxAxx" },
            { L_,  "ABCDEFGHI",  7,    "bbbbbbbr",        "IHGFEDCA",
                                 4,    "bbbxxbxxbbxxbxrxx",
                                                         "FHIxxGxxDExxCxAxx" },
            { L_,  "ABCDEFGHI",  8,    "bbbbbbbr",        "IHGFEDCB",
                                 4,    "bbbxxbxxbbxxbxrxx",
                                                         "FHIxxGxxDExxCxBxx" },

            { L_,  "ABCDEFGHIJ", 0,    "bbbbbbbrb",       "IHGFEDCBA",
                                 4,    "bbbxxbxxbbxxrbxxbxx",
                                                       "FHIxxGxxDExxBCxxAxx" },
            { L_,  "ABCDEFGHIJ", 1,    "bbbbbbbrb",       "JHGFEDCBA",
                                 4,    "bbbxxbxxbbxxrbxxbxx",
                                                       "FHJxxGxxDExxBCxxAxx" },
            { L_,  "ABCDEFGHIJ", 2,    "bbbbbbbrb",       "JIGFEDCBA",
                                 4,    "bbbxxbxxbbxxrbxxbxx",
                                                       "FIJxxGxxDExxBCxxAxx" },
            { L_,  "ABCDEFGHIJ", 3,    "bbbbbbbrb",       "JIHFEDCBA",
                                 4,    "bbbxxbxxbbxxrbxxbxx",
                                                       "FIJxxHxxDExxBCxxAxx" },
            { L_,  "ABCDEFGHIJ", 4,    "bbbbbbbrb",       "JIHGEDCBA",
                                 4,    "bbbxxbxxbbxxrbxxbxx",
                                                       "GIJxxHxxDExxBCxxAxx" },
            { L_,  "ABCDEFGHIJ", 5,    "bbbbbbbrb",       "JIHGFDCBA",
                                 4,    "bbbxxbxxbbxxrbxxbxx",
                                                       "GIJxxHxxDFxxBCxxAxx" },
            { L_,  "ABCDEFGHIJ", 6,    "bbbbbbbrb",       "JIHGFECBA",
                                 4,    "bbbxxbxxbbxxrbxxbxx",
                                                       "GIJxxHxxEFxxBCxxAxx" },
            { L_,  "ABCDEFGHIJ", 7,    "bbbbbbbrb",       "JIHGFEDBA",
                                 4,    "bbbxxbxxbbxxrbxxbxx",
                                                       "GIJxxHxxEFxxBDxxAxx" },
            { L_,  "ABCDEFGHIJ", 8,    "bbbbbbbrb",       "JIHGFEDCA",
                                 4,    "bbbxxbxxbbxxrbxxbxx",
                                                       "GIJxxHxxEFxxCDxxAxx" },
            { L_,  "ABCDEFGHIJ", 9,    "bbbbbbbrb",       "JIHGFEDCB",
                                 4,    "bbbxxbxxbbxxrbxxbxx",
                                                       "GIJxxHxxEFxxCDxxBxx" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\tTesting 'remove' method." << endl;
        {

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec;
                const int         POS   = DATA[ti].d_pos;
                const char *const LCOLS = DATA[ti].d_listColors;
                const char *const LTAGS = DATA[ti].d_listTags;
                const int         HT    = DATA[ti].d_height;
                const char *const TCOLS = DATA[ti].d_treeColors;
                const char *const TTAGS = DATA[ti].d_treeTags;
                if (veryVerbose) { P_(LINE); P_(SPEC); P_(LCOLS); P(LTAGS); }
                const int specLen = std::strlen(SPEC);
                ASSERT(specLen - 1 == (int)std::strlen(LCOLS));
                ASSERT(specLen - 1 == (int)std::strlen(LTAGS));
                const int numNodes = std::strlen(TCOLS);
                ASSERT(numNodes    == (int)std::strlen(TTAGS));

                for (int zi = 0; zi < NUM_NB; ++zi) {
                    Obj mX(NB[zi], Z);  const Obj& X = gg(&mX, SPEC);
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\tBEFORE:"; P_(NB[zi]); P(X); }
                    {
                        // In nested scope to exercise 'listToTree'.
                        BatchManip mB(&mX);  const BatchManip& B = mB;
                        for (int i = 0; i < POS; ++i) {
                            mB.advance();
                        }
                        LOOP2_ASSERT(LINE, zi, B);
                        const char CURR = *(char *)B();
                        const char NEXT = CURR == 'A' ? 0 : CURR - 1;
                        mB.remove();
                        LOOP2_ASSERT(LINE, zi,
                                    (!B && !NEXT) || NEXT == *(char *)B());
                    }
                    LOOP2_ASSERT(LINE, zi, specLen - 1 == X.length());
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\t AFTER:"; P_(NB[zi]); P(X); }

                    Iter mI(X.first());  const Iter& I = mI;
                    const char *colors = LCOLS;
                    const char *tags   = LTAGS;
                    for (; *tags && *colors; ++tags, ++colors) {
                        ASSERT('r' == *colors || 'b' == *colors);
                        ASSERT(isupper(*tags));

                        LOOP2_ASSERT(LINE, zi, I && I());

                        const int isRed = *colors == 'r';
                        LOOP2_ASSERT(LINE, zi,  isRed == I.isRed());
                        LOOP2_ASSERT(LINE, zi, !isRed == I.isBlack());

                        const char *tag = (char *)mI();
                        LOOP2_ASSERT(LINE, zi, *tag == *tags);

                        mI.moveNext();
                    }
                    LOOP2_ASSERT(LINE, zi, !I);
                    LOOP2_ASSERT(LINE, zi,
                            numNodes == walkRBTree(Iter(X), HT, TCOLS, TTAGS));

                    // Remove all, then repopulate to same length.
                    const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                    const int NUM_BYTES  = testAllocator.numBytesInUse();
                    {
                        // In nested scope to exercise 'listToTree'.
                        BatchManip mB(&mX);  const BatchManip& B = mB;
                        while (B) {
                            mB.remove();
                        }
                    }
                    LOOP2_ASSERT(LINE, zi, 0 == X.length());
                    for (int j = 0; j < specLen; ++j) {
                        mX.insertAsFirst();
                    }
                    LOOP2_ASSERT(LINE, zi, specLen    == X.length());
                    LOOP2_ASSERT(LINE, zi, NUM_BLOCKS ==
                                               testAllocator.numBlocksTotal());
                    LOOP2_ASSERT(LINE, zi, NUM_BYTES  ==
                                                testAllocator.numBytesInUse());
                }
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'bdeci_RbTreeImpManip'
        //
        // Concerns:
        //   We have the following specific concerns:
        //    1. The constructor initializes a manipulator to be positioned
        //       at the first node of a tree.  The destructor is trivial.
        //    2. The 'advance' manipulator and direct accessors operate as
        //       expected.
        //    3. insert:
        //        a. A new node may be inserted after any node of a tree
        //           ('insertAfter'); a new node may be inserted before any
        //           position in a tree ('insertBefore').
        //        b. Each insertion operation produces the expected result:
        //            i.   The sequence of nodes is correct.
        //            ii.  The result is a valid red-black tree.
        //            iii. (white-box) The internal pointers (i.e., parent,
        //                             child, next) are consistent.
        //        c. The current member of the manipulator is correct.
        //        d. The container increases capacity as needed.
        //        e. The methods are exception neutral w.r.t. allocation.
        //    4. remove:
        //        a. Any node may be removed from a tree.
        //        b. Each removal operation produces the expected result:
        //            i.   The sequence of nodes is correct.
        //            ii.  The result is a valid red-black tree.
        //            iii. (white-box) The internal pointers (i.e., parent,
        //                             child, next) are consistent.
        //        c. The current member of the manipulator is correct.
        //        d. Memory is released back to the allocator for later reuse.
        //
        // Plan:
        //   To address concerns 1 & 2, specify a set S of representative trees
        //   ordered by increasing length.  For each value t in S, initialize a
        //   newly-constructed manipulator m with t.  Using direct accessors,
        //   verify that m is initially positioned at the first node of t.
        //   Then use the 'advance' method to iterate over the entire tree.
        //   Assert the expected result of relevant accessors at each step in
        //   the iteration.
        //
        //   To address concern 3, specify a set S of representative trees
        //   ordered by increasing length.  For each value t in S, initialize a
        //   newly-constructed manipulator m with t.  Then:
        //    1. 'insertAfter' a new node following every node in the tree and
        //       verify that the expected result is produced using the
        //       appropriate helper functions.
        //    2. Insert a new node at every position in t (via 'insertBefore')
        //       and again use our helper functions to verify the result.
        //   Verify that the current member of m is unchanged.  All insertion
        //   tests are performed within the 'bdema' exception testing
        //   apparatus.
        //
        //   To address concern 4, specify a set S of representative trees
        //   ordered by increasing length.  For each tree t in S, initialize a
        //   newly-constructed manipulator m with t.  Test that any node may be
        //   removed and verify that the expected result is produced using the
        //   appropriate helper functions.  Verify that the current member of m
        //   is advanced to the successor of the removed node.  To address
        //   concern 4d, remove all nodes from t, then repopulate t with the
        //   same number of nodes that it contained before being emptied.
        //   Verify that no new memory is allocated while repopulating t.
        //
        // Testing:
        //   bdeci_RbTreeImpManip(bdeci_RbTreeImp *tree);
        //   ~bdeci_RbTreeImpManip();
        //   void *operator()();
        //   void advance();
        //   void *insertAfter();
        //   void *insertBefore();
        //   void remove();
        //   operator const void *() const;
        //   const void *operator()() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'bdeci_RbTreeImpManip'"
                          << endl << "==============================" << endl;

        if (verbose)
            cout << "\tTesting primary manipulators and direct accessors."
                 << endl;
        {
            const int NUM_DATA = sizeof GDATA_ITERATE / sizeof *GDATA_ITERATE;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = GDATA_ITERATE[ti].d_lineNum;
                const char *const SPEC  = GDATA_ITERATE[ti].d_spec;
                const char *const LCOLS = GDATA_ITERATE[ti].d_listColors;
                const char *const LTAGS = GDATA_ITERATE[ti].d_listTags;
                const int         HT    = GDATA_ITERATE[ti].d_height;
                const char *const TCOLS = GDATA_ITERATE[ti].d_treeColors;
                const char *const TTAGS = GDATA_ITERATE[ti].d_treeTags;
                if (veryVerbose) { P_(LINE); P_(SPEC); P_(LCOLS); P(LTAGS); }
                const int specLen = std::strlen(SPEC);
                ASSERT(specLen  == (int)std::strlen(LCOLS));
                ASSERT(specLen  == (int)std::strlen(LTAGS));
                const int numNodes = std::strlen(TCOLS);
                ASSERT(numNodes == (int)std::strlen(TTAGS));

                for (int zi = 0; zi < NUM_NB; ++zi) {
                    Obj mX(NB[zi]);  const Obj& X = gg(&mX, SPEC);
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\tBEFORE:"; P_(NB[zi]); P(X); }
                    {
                        // In nested scope to exercise destructor.
                        Manip mM(&mX);  const Manip& M = mM;
                        LOOP2_ASSERT(LINE, zi,
                                     specLen == 0 || (M && M() == X.first()));
                        for (int i = 0; i < specLen; ++i) {
                            LOOP2_ASSERT(LINE, zi, M);
                            void *vp = mM();
                            LOOP2_ASSERT(LINE, zi, 0 != vp);
                            const void *cvp = M();
                            LOOP2_ASSERT(LINE, zi, 0 != cvp);
                            LOOP2_ASSERT(LINE, zi, LTAGS[i] == *(char *)vp);
                            LOOP2_ASSERT(LINE, zi,
                                               LTAGS[i] == *(const char *)cvp);
                            mM.advance();
                        }
                        LOOP2_ASSERT(LINE, zi, !M);
                    }
                    if (veryVerbose) { cout << "\t AFTER:"; P_(NB[zi]); P(X); }

                    Iter mI(X.first());  const Iter& I = mI;
                    const char *colors = LCOLS;
                    const char *tags   = LTAGS;
                    for (; *tags && *colors; ++tags, ++colors) {
                        ASSERT('r' == *colors || 'b' == *colors);
                        ASSERT(isupper(*tags));

                        LOOP2_ASSERT(LINE, zi, I && I());

                        const int isRed = *colors == 'r';
                        LOOP2_ASSERT(LINE, zi,  isRed == I.isRed());
                        LOOP2_ASSERT(LINE, zi, !isRed == I.isBlack());

                        const char *tag = (char *)mI();
                        LOOP2_ASSERT(LINE, zi, *tag == *tags);

                        mI.moveNext();
                    }
                    LOOP2_ASSERT(LINE, zi, !I);
                    LOOP2_ASSERT(LINE, zi,
                            numNodes == walkRBTree(Iter(X), HT, TCOLS, TTAGS));
                }
            }
        }

        if (verbose) cout << "\tTesting 'insertAfter' method." << endl;
        {
            const int NUM_DATA =
                          sizeof GDATA_INSERTAFTER / sizeof *GDATA_INSERTAFTER;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = GDATA_INSERTAFTER[ti].d_lineNum;
                const char *const SPEC  = GDATA_INSERTAFTER[ti].d_spec;
                const int         POS   = GDATA_INSERTAFTER[ti].d_pos;
                const char *const LCOLS = GDATA_INSERTAFTER[ti].d_listColors;
                const char *const LTAGS = GDATA_INSERTAFTER[ti].d_listTags;
                const int         HT    = GDATA_INSERTAFTER[ti].d_height;
                const char *const TCOLS = GDATA_INSERTAFTER[ti].d_treeColors;
                const char *const TTAGS = GDATA_INSERTAFTER[ti].d_treeTags;
                if (veryVerbose) { P_(LINE); P_(SPEC); P_(LCOLS); P(LTAGS); }
                const int specLen = std::strlen(SPEC);
                ASSERT(specLen + 1 == (int)std::strlen(LCOLS));
                ASSERT(specLen + 1 == (int)std::strlen(LTAGS));
                const int numNodes = std::strlen(TCOLS);
                ASSERT(numNodes    == (int)std::strlen(TTAGS));

                for (int zi = 0; zi < NUM_NB; ++zi) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj mX(NB[zi]);  const Obj& X = gg(&mX, SPEC);
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\tBEFORE:"; P_(NB[zi]); P(X); }
                    {
                        // In nested scope to exercise destructor.
                        Manip mM(&mX);  const Manip& M = mM;
                        for (int i = 0; i < POS; ++i) {
                            mM.advance();
                        }
                        LOOP2_ASSERT(LINE, zi, M);
                        char *node = (char *)mM.insertAfter();
                        *node = 'X';
                        LOOP2_ASSERT(LINE, zi, 'X' == *(char *)M());
                                                        // New node is current.
                    }
                    LOOP2_ASSERT(LINE, zi, specLen + 1 == X.length());
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\t AFTER:"; P_(NB[zi]); P(X); }

                    Iter mI(X.first());  const Iter& I = mI;
                    const char *colors = LCOLS;
                    const char *tags   = LTAGS;
                    for (; *tags && *colors; ++tags, ++colors) {
                        ASSERT('r' == *colors || 'b' == *colors);
                        ASSERT(isupper(*tags));

                        LOOP2_ASSERT(LINE, zi, I && I());

                        const int isRed = *colors == 'r';
                        LOOP2_ASSERT(LINE, zi,  isRed == I.isRed());
                        LOOP2_ASSERT(LINE, zi, !isRed == I.isBlack());

                        const char *tag = (char *)mI();
                        LOOP2_ASSERT(LINE, zi, *tag == *tags);

                        mI.moveNext();
                    }
                    LOOP2_ASSERT(LINE, zi, !I);
                    LOOP2_ASSERT(LINE, zi,
                            numNodes == walkRBTree(Iter(X), HT, TCOLS, TTAGS));
                  } END_BDEMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout << "\tTesting 'insertBefore' method." << endl;
        {
            static const struct {
                int         d_lineNum;       // source line number
                const char *d_spec;          // specification string
                int         d_pos;           // insert before this position
                const char *d_listColors;    // expected r-b list coloration
                const char *d_listTags;      // expected r-b list node tags
                int         d_height;        // expected r-b tree height
                const char *d_treeColors;    // expected r-b tree coloration
                const char *d_treeTags;      // expected r-b tree node tags
            } DATA[] = {                                              // ADJUST
                //line spec      pos/ht   list/tree colors  list/tree tags
                //---- ----      ------   ----------------  --------------
                { L_,  "",           0,   "b",              "X",
                                     1,   "bxx",            "Xxx"            },

                { L_,  "A",          0,   "rb",             "XA",
                                     2,   "brxxx",          "AXxxx"          },
                { L_,  "A",          1,   "br",             "AX",
                                     2,   "bxrxx",          "AxXxx"          },

                { L_,  "AB",         0,   "rbr",            "XBA",
                                     2,   "brxxrxx",        "BXxxAxx"        },
                { L_,  "AB",         1,   "rbr",            "BXA",
                                     2,   "brxxrxx",        "XBxxAxx"        },
                { L_,  "AB",         2,   "rbr",            "BAX",
                                     2,   "brxxrxx",        "ABxxXxx"        },

                { L_,  "ABC",        0,   "rbbb",           "XCBA",
                                     3,   "bbrxxxbxx",      "BCXxxxAxx"      },
                { L_,  "ABC",        1,   "brbb",           "CXBA",
                                     3,   "bbxrxxbxx",      "BCxXxxAxx"      },
                { L_,  "ABC",        2,   "bbrb",           "CBXA",
                                     3,   "bbxxbrxxx",      "BCxxAXxxx"      },
                { L_,  "ABC",        3,   "bbbr",           "CBAX",
                                     3,   "bbxxbxrxx",      "BCxxAxXxx"      },

                { L_,  "ABCD",       0,   "rbrbb",          "XDCBA",
                                     3,   "bbrxxrxxbxx",    "BDXxxCxxAxx"    },
                { L_,  "ABCD",       1,   "rbrbb",          "DXCBA",
                                     3,   "bbrxxrxxbxx",    "BXDxxCxxAxx"    },
                { L_,  "ABCD",       2,   "rbrbb",          "DCXBA",
                                     3,   "bbrxxrxxbxx",    "BCDxxXxxAxx"    },
                { L_,  "ABCD",       3,   "rbbrb",          "DCBXA",
                                     3,   "bbrxxxbrxxx",    "BCDxxxAXxxx"    },
                { L_,  "ABCD",       4,   "rbbbr",          "DCBAX",
                                     3,   "bbrxxxbxrxx",    "BCDxxxAxXxx"    },

                { L_,  "ABCDE",      0,   "rbrbbb",         "XEDCBA",
                                     4,   "brbrxxxbxxbxx",  "BDEXxxxCxxAxx"  },
                { L_,  "ABCDE",      1,   "brrbbb",         "EXDCBA",
                                     4,   "brbxrxxbxxbxx",  "BDExXxxCxxAxx"  },
                { L_,  "ABCDE",      2,   "brrbbb",         "EDXCBA",
                                     4,   "brbxxbrxxxbxx",  "BDExxCXxxxAxx"  },
                { L_,  "ABCDE",      3,   "brbrbb",         "EDCXBA",
                                     4,   "brbxxbxrxxbxx",  "BDExxCxXxxAxx"  },
                { L_,  "ABCDE",      4,   "rbrbrb",         "EDCBXA",
                                     4,   "bbrxxrxxbrxxx",  "BDExxCxxAXxxx"  },
                { L_,  "ABCDE",      5,   "rbrbbr",         "EDCBAX",
                                     4,   "bbrxxrxxbxrxx",  "BDExxCxxAxXxx"  },

                { L_,  "ABCDEF",     0,   "rbrrbbb",        "XFEDCBA",
                                     4,   "brbrxxrxxbxxbxx",
                                                           "BDFXxxExxCxxAxx" },
                { L_,  "ABCDEF",     1,   "rbrrbbb",        "FXEDCBA",
                                     4,   "brbrxxrxxbxxbxx",
                                                           "BDXFxxExxCxxAxx" },
                { L_,  "ABCDEF",     2,   "rbrrbbb",        "FEXDCBA",
                                     4,   "brbrxxrxxbxxbxx",
                                                           "BDEFxxXxxCxxAxx" },
                { L_,  "ABCDEF",     3,   "rbrrbbb",        "FEDXCBA",
                                     4,   "brbrxxxbrxxxbxx",
                                                           "BDEFxxxCXxxxAxx" },
                { L_,  "ABCDEF",     4,   "rbrbrbb",        "FEDCXBA",
                                     4,   "brbrxxxbxrxxbxx",
                                                           "BDEFxxxCxXxxAxx" },
                { L_,  "ABCDEF",     5,   "rbrbbrb",        "FEDCBXA",
                                     4,   "brbrxxxbxxbrxxx",
                                                           "BDEFxxxCxxAXxxx" },
                { L_,  "ABCDEF",     6,   "rbrbbbr",        "FEDCBAX",
                                     4,   "brbrxxxbxxbxrxx",
                                                           "BDEFxxxCxxAxXxx" },

                { L_,  "ABCDEFG",    0,   "rbrbbbrb",       "XGFEDCBA",
                                     4,   "brbrxxxbxxrbxxbxx",
                                                         "DFGXxxxExxBCxxAxx" },
                { L_,  "ABCDEFG",    1,   "brrbbbrb",       "GXFEDCBA",
                                     4,   "brbxrxxbxxrbxxbxx",
                                                         "DFGxXxxExxBCxxAxx" },
                { L_,  "ABCDEFG",    2,   "brrbbbrb",       "GFXEDCBA",
                                     4,   "brbxxbrxxxrbxxbxx",
                                                         "DFGxxEXxxxBCxxAxx" },
                { L_,  "ABCDEFG",    3,   "brbrbbrb",       "GFEXDCBA",
                                     4,   "brbxxbxrxxrbxxbxx",
                                                         "DFGxxExXxxBCxxAxx" },
                { L_,  "ABCDEFG",    4,   "rbrrrbbb",       "GFEDXCBA",
                                     4,   "brbrxxrxxbrxxxbxx",
                                                         "BDFGxxExxCXxxxAxx" },
                { L_,  "ABCDEFG",    5,   "rbrrbrbb",       "GFEDCXBA",
                                     4,   "brbrxxrxxbxrxxbxx",
                                                         "BDFGxxExxCxXxxAxx" },
                { L_,  "ABCDEFG",    6,   "rbrrbbrb",       "GFEDCBXA",
                                     4,   "brbrxxrxxbxxbrxxx",
                                                         "BDFGxxExxCxxAXxxx" },
                { L_,  "ABCDEFG",    7,   "rbrrbbbr",       "GFEDCBAX",
                                     4,   "brbrxxrxxbxxbxrxx",
                                                         "BDFGxxExxCxxAxXxx" },

                { L_,  "ABCDEFGH",   0,   "rbrrbbbrb",      "XHGFEDCBA",
                                     4,   "brbrxxrxxbxxrbxxbxx",
                                                       "DFHXxxGxxExxBCxxAxx" },
                { L_,  "ABCDEFGH",   1,   "rbrrbbbrb",      "HXGFEDCBA",
                                     4,   "brbrxxrxxbxxrbxxbxx",
                                                      "DFXHxxGxxExxBCxxAxx" },
                { L_,  "ABCDEFGH",   2,   "rbrrbbbrb",      "HGXFEDCBA",
                                     4,   "brbrxxrxxbxxrbxxbxx",
                                                       "DFGHxxXxxExxBCxxAxx" },
                { L_,  "ABCDEFGH",   3,   "rbrrbbbrb",      "HGFXEDCBA",
                                     4,   "brbrxxxbrxxxrbxxbxx",
                                                       "DFGHxxxEXxxxBCxxAxx" },
                { L_,  "ABCDEFGH",   4,   "rbrbrbbrb",      "HGFEXDCBA",
                                     4,   "brbrxxxbxrxxrbxxbxx",
                                                       "DFGHxxxExXxxBCxxAxx" },
                { L_,  "ABCDEFGH",   5,   "rbrbbrbrb",      "HGFEDXCBA",
                                     4,   "brbrxxxbxxrbrxxxbxx",
                                                       "DFGHxxxExxBCXxxxAxx" },
                { L_,  "ABCDEFGH",   6,   "rbrbbbrrb",      "HGFEDCXBA",
                                     4,   "brbrxxxbxxrbxrxxbxx",
                                                       "DFGHxxxExxBCxXxxAxx" },
                { L_,  "ABCDEFGH",   7,   "rbrbbbrrb",      "HGFEDCBXA",
                                     4,   "brbrxxxbxxrbxxbrxxx",
                                                       "DFGHxxxExxBCxxAXxxx" },
                { L_,  "ABCDEFGH",   8,   "rbrbbbrbr",      "HGFEDCBAX",
                                     4,   "brbrxxxbxxrbxxbxrxx",
                                                       "DFGHxxxExxBCxxAxXxx" },

                { L_,  "ABCDEFGHI",  0,   "rbrbbbbbbb",     "XIHGFEDCBA",
                                     5,   "bbrbrxxxbxxbxxbbxxbxx",
                                                     "DFHIXxxxGxxExxBCxxAxx" },
                { L_,  "ABCDEFGHI",  1,   "brrbbbbbbb",     "IXHGFEDCBA",
                                     5,   "bbrbxrxxbxxbxxbbxxbxx",
                                                     "DFHIxXxxGxxExxBCxxAxx" },
                { L_,  "ABCDEFGHI",  2,   "brrbbbbbbb",     "IHXGFEDCBA",
                                     5,   "bbrbxxbrxxxbxxbbxxbxx",
                                                     "DFHIxxGXxxxExxBCxxAxx" },
                { L_,  "ABCDEFGHI",  3,   "brbrbbbbbb",     "IHGXFEDCBA",
                                     5,   "bbrbxxbxrxxbxxbbxxbxx",
                                                     "DFHIxxGxXxxExxBCxxAxx" },
                { L_,  "ABCDEFGHI",  4,   "rbrrrbbbrb",     "IHGFXEDCBA",
                                     5,   "brbrxxrxxbrxxxrbxxbxx",
                                                     "DFHIxxGxxEXxxxBCxxAxx" },
                { L_,  "ABCDEFGHI",  5,   "rbrrbrbbrb",     "IHGFEXDCBA",
                                     5,   "brbrxxrxxbxrxxrbxxbxx",
                                                     "DFHIxxGxxExXxxBCxxAxx" },
                { L_,  "ABCDEFGHI",  6,   "rbrrbbrbrb",     "IHGFEDXCBA",
                                     5,   "brbrxxrxxbxxrbrxxxbxx",
                                                     "DFHIxxGxxExxBCXxxxAxx" },
                { L_,  "ABCDEFGHI",  7,   "rbrrbbbrrb",     "IHGFEDCXBA",
                                     5,   "brbrxxrxxbxxrbxrxxbxx",
                                                     "DFHIxxGxxExxBCxXxxAxx" },
                { L_,  "ABCDEFGHI",  8,   "rbrrbbbrrb",     "IHGFEDCBXA",
                                     5,   "brbrxxrxxbxxrbxxbrxxx",
                                                     "DFHIxxGxxExxBCxxAXxxx" },
                { L_,  "ABCDEFGHI",  9,   "rbrrbbbrbr",     "IHGFEDCBAX",
                                     5,   "brbrxxrxxbxxrbxxbxrxx",
                                                     "DFHIxxGxxExxBCxxAxXxx" },

                { L_,  "ABCDEFGHIJ", 0,   "rbrrbbbbbbb",    "XJIHGFEDCBA",
                                     5,   "bbrbrxxrxxbxxbxxbbxxbxx",
                                                   "DFHJXxxIxxGxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 1,   "rbrrbbbbbbb",    "JXIHGFEDCBA",
                                     5,   "bbrbrxxrxxbxxbxxbbxxbxx",
                                                   "DFHXJxxIxxGxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 2,   "rbrrbbbbbbb",    "JIXHGFEDCBA",
                                     5,   "bbrbrxxrxxbxxbxxbbxxbxx",
                                                   "DFHIJxxXxxGxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 3,   "rbrrbbbbbbb",    "JIHXGFEDCBA",
                                     5,   "bbrbrxxxbrxxxbxxbbxxbxx",
                                                   "DFHIJxxxGXxxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 4,   "rbrbrbbbbbb",    "JIHGXFEDCBA",
                                     5,   "bbrbrxxxbxrxxbxxbbxxbxx",
                                                   "DFHIJxxxGxXxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 5,   "rbrbbrbbbbb",    "JIHGFXEDCBA",
                                     5,   "bbrbrxxxbxxbrxxxbbxxbxx",
                                                   "DFHIJxxxGxxEXxxxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 6,   "rbrbbbrbbbb",    "JIHGFEXDCBA",
                                     5,   "bbrbrxxxbxxbxrxxbbxxbxx",
                                                   "DFHIJxxxGxxExXxxBCxxAxx" },
                { L_,  "ABCDEFGHIJ", 7,   "rbrbbbbrbbb",    "JIHGFEDXCBA",
                                     5,   "bbrbrxxxbxxbxxbbrxxxbxx",
                                                   "DFHIJxxxGxxExxBCXxxxAxx" },
                { L_,  "ABCDEFGHIJ", 8,   "rbrbbbbbrbb",    "JIHGFEDCXBA",
                                     5,   "bbrbrxxxbxxbxxbbxrxxbxx",
                                                   "DFHIJxxxGxxExxBCxXxxAxx" },
                { L_,  "ABCDEFGHIJ", 9,   "rbrbbbbbbrb",    "JIHGFEDCBXA",
                                     5,   "bbrbrxxxbxxbxxbbxxbrxxx",
                                                   "DFHIJxxxGxxExxBCxxAXxxx" },
                { L_,  "ABCDEFGHIJ",10,   "rbrbbbbbbbr",    "JIHGFEDCBAX",
                                     5,   "bbrbrxxxbxxbxxbbxxbxrxx",
                                                   "DFHIJxxxGxxExxBCxxAxXxx" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec;
                const int         POS   = DATA[ti].d_pos;
                const char *const LCOLS = DATA[ti].d_listColors;
                const char *const LTAGS = DATA[ti].d_listTags;
                const int         HT    = DATA[ti].d_height;
                const char *const TCOLS = DATA[ti].d_treeColors;
                const char *const TTAGS = DATA[ti].d_treeTags;
                if (veryVerbose) { P_(LINE); P_(SPEC); P_(LCOLS); P(LTAGS); }
                const int specLen = std::strlen(SPEC);
                ASSERT(specLen + 1 == (int)std::strlen(LCOLS));
                ASSERT(specLen + 1 == (int)std::strlen(LTAGS));
                const int numNodes = std::strlen(TCOLS);
                ASSERT(numNodes    == (int)std::strlen(TTAGS));

                for (int zi = 0; zi < NUM_NB; ++zi) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj mX(NB[zi]);  const Obj& X = gg(&mX, SPEC);
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\tBEFORE:"; P_(NB[zi]); P(X); }
                    {
                        // In nested scope to exercise destructor.
                        Manip mM(&mX);  const Manip& M = mM;
                        for (int i = 0; i < POS; ++i) {
                            mM.advance();
                        }
                        LOOP2_ASSERT(LINE, zi, (POS == specLen) == !M);
                        char *node = (char *)mM.insertBefore();
                        *node = 'X';
                        LOOP2_ASSERT(LINE, zi, 'X' == *(char *)M());
                                                        // New node is current.
                    }
                    LOOP2_ASSERT(LINE, zi, specLen + 1 == X.length());
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\t AFTER:"; P_(NB[zi]); P(X); }

                    Iter mI(X.first());  const Iter& I = mI;
                    const char *colors = LCOLS;
                    const char *tags   = LTAGS;
                    for (; *tags && *colors; ++tags, ++colors) {
                        ASSERT('r' == *colors || 'b' == *colors);
                        ASSERT(isupper(*tags));

                        LOOP2_ASSERT(LINE, zi, I && I());

                        const int isRed = *colors == 'r';
                        LOOP2_ASSERT(LINE, zi,  isRed == I.isRed());
                        LOOP2_ASSERT(LINE, zi, !isRed == I.isBlack());

                        const char *tag = (char *)mI();
                        LOOP2_ASSERT(LINE, zi, *tag == *tags);

                        mI.moveNext();
                    }
                    LOOP2_ASSERT(LINE, zi, !I);
                    LOOP2_ASSERT(LINE, zi,
                            numNodes == walkRBTree(Iter(X), HT, TCOLS, TTAGS));
                  } END_BDEMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout << "\tTesting 'remove(node)' method." << endl;
        {
            const int NUM_DATA = sizeof GDATA_REMOVE / sizeof *GDATA_REMOVE;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = GDATA_REMOVE[ti].d_lineNum;
                const char *const SPEC  = GDATA_REMOVE[ti].d_spec;
                const int         POS   = GDATA_REMOVE[ti].d_pos;
                const char *const LCOLS = GDATA_REMOVE[ti].d_listColors;
                const char *const LTAGS = GDATA_REMOVE[ti].d_listTags;
                const int         HT    = GDATA_REMOVE[ti].d_height;
                const char *const TCOLS = GDATA_REMOVE[ti].d_treeColors;
                const char *const TTAGS = GDATA_REMOVE[ti].d_treeTags;
                if (veryVerbose) { P_(LINE); P_(SPEC); P_(LCOLS); P(LTAGS); }
                const int specLen = std::strlen(SPEC);
                ASSERT(specLen - 1 == (int)std::strlen(LCOLS));
                ASSERT(specLen - 1 == (int)std::strlen(LTAGS));
                const int numNodes = std::strlen(TCOLS);
                ASSERT(numNodes    == (int)std::strlen(TTAGS));

                for (int zi = 0; zi < NUM_NB; ++zi) {
                    Obj mX(NB[zi], Z);  const Obj& X = gg(&mX, SPEC);
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\tBEFORE:"; P_(NB[zi]); P(X); }
                    {
                        // In nested scope to exercise destructor.
                        Manip mM(&mX);  const Manip& M = mM;
                        for (int i = 0; i < POS; ++i) {
                            mM.advance();
                        }
                        LOOP2_ASSERT(LINE, zi, M);
                        const char CURR = *(char *)M();
                        const char NEXT = CURR == 'A' ? 0 : CURR - 1;
                        mM.remove();
                        LOOP2_ASSERT(LINE, zi,
                                     (!M && !NEXT) || NEXT == *(char *)M());
                    }
                    LOOP2_ASSERT(LINE, zi, specLen - 1 == X.length());
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\t AFTER:"; P_(NB[zi]); P(X); }

                    Iter mI(X.first());  const Iter& I = mI;
                    const char *colors = LCOLS;
                    const char *tags   = LTAGS;
                    for (; *tags && *colors; ++tags, ++colors) {
                        ASSERT('r' == *colors || 'b' == *colors);
                        ASSERT(isupper(*tags));

                        LOOP2_ASSERT(LINE, zi, I && I());

                        const int isRed = *colors == 'r';
                        LOOP2_ASSERT(LINE, zi,  isRed == I.isRed());
                        LOOP2_ASSERT(LINE, zi, !isRed == I.isBlack());

                        const char *tag = (char *)mI();
                        LOOP2_ASSERT(LINE, zi, *tag == *tags);

                        mI.moveNext();
                    }
                    LOOP2_ASSERT(LINE, zi, !I);
                    LOOP2_ASSERT(LINE, zi,
                            numNodes == walkRBTree(Iter(X), HT, TCOLS, TTAGS));

                    // Remove all, then repopulate to same length.
                    const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                    const int NUM_BYTES  = testAllocator.numBytesInUse();
                    {
                        Manip mM(&mX);  const Manip& M = mM;
                        while (M) {
                            mM.remove();
                        }
                    }
                    LOOP2_ASSERT(LINE, zi, 0 == X.length());
                    for (int j = 0; j < specLen; ++j) {
                        mX.insertAsFirst();
                    }
                    LOOP2_ASSERT(LINE, zi, specLen    == X.length());
                    LOOP2_ASSERT(LINE, zi, NUM_BLOCKS ==
                                               testAllocator.numBlocksTotal());
                    LOOP2_ASSERT(LINE, zi, NUM_BYTES  ==
                                                testAllocator.numBytesInUse());
                }
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'bdeci_RbTreeImpIter' ASSIGNMENT OPERATOR
        //
        // Concerns:
        //   The concerns are as follows:
        //    1. An iterator may be made to refer to any node of a
        //       red-black tree by assignment.
        //    2. The underlying tree is unaffected.
        //
        // Plan:
        //   Construct a red-black tree t of some reasonable size and
        //   initialize a control iterator i to refer to the first node of t.
        //   Use i to sequentially iterate over each node of t.  As each node
        //   is visited by i, construct a second iterator j, assign i to j, and
        //   verify that i and j refer to the same red-black tree node.  Use
        //   the newly-modified j to sequentially iterate over the set of tree
        //   nodes accessible from i's current position.  Verify that the
        //   respective iterations using i and j produce the expected result
        //   with appropriate assertions.  After the iterator testing is
        //   completed, verify that t is unchanged.
        //
        // Testing:
        //   bdeci_RbTreeImpIter& operator=(const bdeci_RbTreeImpIter& rhs);
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "Testing 'bdeci_RbTreeImpIter' 'operator='"
                 << endl << "========================================="
                 << endl;

        {
            Iter mI;  const Iter& I = mI;
            ASSERT(!I);

            Obj mX(1);   const Obj& X = gg(&mX, "ABCD");
            Iter mJ(X);  const Iter& J = mJ;
            ASSERT(J && J() == X.root());

            mJ = I;   // Valid iterator becomes invalid.
            ASSERT(!J);
        }

        {
            const char *const SPEC  = "ABCDEFGHIJKLMNO";
            const char *const LCOLS = "rbrrbbbrbbbbbbb";
            const char *const LTAGS = "ONMLKJIHGFEDCBA";
            const char *const TCOLS = "brbrbrxxrxxbxxbxxbbxxbxxbbxxbxx";
            const char *const TTAGS = "DHJLNOxxMxxKxxIxxFGxxExxBCxxAxx";
            const int         HT    = 6;
            if (veryVerbose) { P_(SPEC); P_(LCOLS); P(LTAGS); }

            const int numNodes = std::strlen(TCOLS);
            const int specLen  = std::strlen(SPEC);
            ASSERT(specLen == (int)std::strlen(LCOLS));
            ASSERT(specLen == (int)std::strlen(LTAGS));

            Obj mX(1);  const Obj& X = gg(&mX, SPEC);
            validateRBTree(X);
            ASSERT(numNodes == walkRBTree(Iter(X), HT, TCOLS, TTAGS));
            if (veryVerbose) { TAB; P(X); }

            Iter mI(X.first());  const Iter& I = mI;  // control

            const char *colorsI = LCOLS;
            const char *tagsI   = LTAGS;
            for (; *tagsI && *colorsI; ++tagsI, ++colorsI) {
                ASSERT('r' == *colorsI || 'b' == *colorsI);
                ASSERT(isupper(*tagsI));

                ASSERT(I && I());

                const int isRed = *colorsI == 'r';
                ASSERT( isRed == I.isRed());
                ASSERT(!isRed == I.isBlack());

                const char *tag = (char *)I();
                ASSERT(*tag == *tagsI);

                Iter mJ;  const Iter& J = mJ;  ASSERT(!J);
                ASSERT(!J);

                mJ = I;  // Invalid iterator becomes valid.
                ASSERT(J && I() == J());

                const char *colorsJ = colorsI;
                const char *tagsJ   = tagsI;
                for (; *tagsJ && *colorsJ; ++tagsJ, ++colorsJ) {
                    ASSERT('r' == *colorsJ || 'b' == *colorsJ);
                    ASSERT(isupper(*tagsJ));

                    ASSERT(J && J());

                    const int isRed = *colorsJ == 'r';
                    ASSERT( isRed == J.isRed());
                    ASSERT(!isRed == J.isBlack());

                    const char *tag = (char *)J();
                    ASSERT(*tag == *tagsJ);

                    mJ.moveNext();
                }
                ASSERT(!J);

                mI.moveNext();
            }
            ASSERT(!I);
            validateRBTree(X);
            ASSERT(numNodes == walkRBTree(Iter(X), HT, TCOLS, TTAGS));
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'bdeci_RbTreeImpIter' COPY CTOR, ETC.
        //
        // Concerns:
        //   The concerns are as follows:
        //    1. An iterator may be initialized to refer to any node of a
        //       red-black tree.
        //    2. The underlying tree is unaffected by this iterator.
        //
        // Plan:
        //   Construct a red-black tree t of some reasonable size and
        //   initialize a control iterator i to refer to the first node of t.
        //   Use i to sequentially iterate over each node of t.  As each node
        //   is visited by i, construct two other iterators, j & k, using the
        //   two constructors under test.  Use the two newly-constructed
        //   iterators to sequentially iterate over the set of tree nodes
        //   accessible from i's current position.  Verify that the respective
        //   iterations using i, j and k produce the expected result with
        //   appropriate assertions.  After the iterator testing is completed,
        //   verify that t is unchanged.
        //
        // Testing:
        //   bdeci_RbTreeImpIter(const void *node);
        //   bdeci_RbTreeImpIter(const bdeci_RbTreeImpIter& original);
        //   int operator==(const Iter& lhs, const Iter& rhs);
        //   int operator!=(const Iter& lhs, const Iter& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'Iter' copy constructor, etc."
                          << endl << "====================================="
                          << endl;

        {
            // Iter(*node) constructor
            Iter mI(0);  const Iter& I = mI;
            ASSERT(!I);

            // Iter copy constructor
            Iter mJ(I);  const Iter& J = mJ;
            ASSERT((I == J == 1) && (I != J == 0));
        }

        {
            const char *const SPEC  = "ABCDEFGHIJKLMNO";
            const char *const LCOLS = "rbrrbbbrbbbbbbb";
            const char *const LTAGS = "ONMLKJIHGFEDCBA";
            const char *const TCOLS = "brbrbrxxrxxbxxbxxbbxxbxxbbxxbxx";
            const char *const TTAGS = "DHJLNOxxMxxKxxIxxFGxxExxBCxxAxx";
            const int         HT    = 6;
            if (veryVerbose) { P_(SPEC); P_(LCOLS); P(LTAGS); }

            const int numNodes = std::strlen(TCOLS);
            const int specLen  = std::strlen(SPEC);
            ASSERT(specLen == (int)std::strlen(LCOLS));
            ASSERT(specLen == (int)std::strlen(LTAGS));

            Obj mX(1);  const Obj& X = gg(&mX, SPEC);
            validateRBTree(X);
            ASSERT(numNodes == walkRBTree(Iter(X), HT, TCOLS, TTAGS));
            if (veryVerbose) { TAB; P(X); }

            Iter mI(X.first());  const Iter& I = mI;  // control

            const char *colorsI = LCOLS;
            const char *tagsI   = LTAGS;
            for (; *tagsI && *colorsI; ++tagsI, ++colorsI) {
                ASSERT('r' == *colorsI || 'b' == *colorsI);
                ASSERT(isupper(*tagsI));

                ASSERT(I && I());

                const int isRed = *colorsI == 'r';
                ASSERT( isRed == I.isRed());
                ASSERT(!isRed == I.isBlack());

                const char *tag = (char *)I();
                ASSERT(*tag == *tagsI);

                // Iter(*node) constructor
                Iter mJ(I());  const Iter& J = mJ;
                ASSERT(J && I() == J());
                ASSERT((I == J == 1) && (I != J == 0));

                const char *colorsJ = colorsI;
                const char *tagsJ   = tagsI;
                for (; *tagsJ && *colorsJ; ++tagsJ, ++colorsJ) {
                    ASSERT('r' == *colorsJ || 'b' == *colorsJ);
                    ASSERT(isupper(*tagsJ));

                    ASSERT(J && J());

                    const int isRed = *colorsJ == 'r';
                    ASSERT( isRed == J.isRed());
                    ASSERT(!isRed == J.isBlack());

                    const char *tag = (char *)J();
                    ASSERT(*tag == *tagsJ);

                    mJ.moveNext();
                    ASSERT((I == J == 0) && (I != J == 1));
                }
                ASSERT(!J);

                // Iter copy constructor
                Iter mK(I);  const Iter& K = mK;
                ASSERT(K && I() == K());
                ASSERT((I == K == 1) && (I != K == 0));

                const char *colorsK = colorsI;
                const char *tagsK   = tagsI;
                for (; *tagsK && *colorsK; ++tagsK, ++colorsK) {
                    ASSERT('r' == *colorsK || 'b' == *colorsK);
                    ASSERT(isupper(*tagsK));

                    ASSERT(K && K());

                    const int isRed = *colorsK == 'r';
                    ASSERT( isRed == K.isRed());
                    ASSERT(!isRed == K.isBlack());

                    const char *tag = (char *)K();
                    ASSERT(*tag == *tagsK);

                    mK.moveNext();
                    ASSERT((I == K == 0) && (I != K == 1));
                }
                ASSERT(!K);

                mI.moveNext();
                ASSERT((I == J == !I) && (I != J == !!I));
                ASSERT((I == K == !I) && (I != K == !!I));
            }
            ASSERT(!I);
            validateRBTree(X);
            ASSERT(numNodes == walkRBTree(Iter(X), HT, TCOLS, TTAGS));
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING CAPACITY-RESERVING CONSTRUCTOR AND METHOD
        //
        // Concerns:
        //   The concerns are as follows:
        //    1. capacity-reserving constructor:
        //       a. The initial value is correct (empty).
        //       b. The initial capacity is correct.
        //       c. The constructor is exception neutral w.r.t. allocation.
        //       d. The internal memory management system is hooked up properly
        //          so that *all* internally allocated memory draws from a
        //          user-supplied allocator whenever one is specified.
        //    2. 'reserveCapacity' method:
        //       a. The resulting value is correct (unchanged).
        //       b. The resulting capacity is correct (not less than initial).
        //       c. The method is exception neutral w.r.t. allocation.
        //
        // Plan:
        //   In a loop, use the capacity-reserving constructor to create empty
        //   objects with increasing initial capacity.  Verify that each object
        //   has the same value as a control default object.  Then, insert as
        //   many values as the requested initial capacity, and use
        //   'bdema_TestAllocator' to verify that no additional allocations
        //   have occurred.  Perform each test in the standard 'bdema'
        //   exception-testing macro block.
        //
        //   Repeat the constructor test initially specifying no allocator and
        //   again, specifying a static buffer allocator.  These tests (without
        //   specifying a 'bdema_TestAllocator') cannot confirm correct
        //   capacity-reserving behavior, but can test for rudimentary correct
        //   object behavior via the destructor and Purify, and, in
        //   'veryVerbose' mode, via the print statements.
        //
        //   To test 'reserveCapacity', specify a table of initial object
        //   values and subsequent capacities to reserve.  Construct each
        //   tabulated value and call 'reserveCapacity' with the tabulated
        //   number of elements.  Then insert as many values as required to
        //   bring the test object's size to the specified number of elements,
        //   and use 'bdema_TestAllocator' to verify that no additional
        //   allocations have occurred.  Perform each test in the standard
        //   'bdema' exception-testing macro block.  Perform a separate test to
        //   verify that the object's value is unaffected by 'reserveCapacity'.
        //
        // Testing:
        //   bdeci_RbTreeImp(nb, const InitialCapacity& ne, *ba=0);
        //   void reserveCapacity(int ne);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "Testing Capacity Reserving Constructor and Method" << endl
            << "=================================================" << endl;

        if (verbose) cout <<
            "\nTesting 'bdeci_RbTreeImp(nb, capacity, ba)' ctor." << endl;

        if (verbose) cout << "\tWith a 'bdema_TestAllocator'." << endl;
        {
            const int MAX_NUM_ELEMS = 9;
            for (int zi = 0; zi < NUM_NB; ++zi) {
                for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                  if (veryVerbose) { cout << "\t\t"; P(ne) }
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    const Obj::InitialCapacity NE(ne);
                    Obj mX(NB[zi], NE, &testAllocator);  const Obj &X = mX;
                    const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                    const int NUM_BYTES  = testAllocator.numBytesInUse();
                    if (veryVerbose) P_(X);
                    for (int i = 0; i < ne; ++i) {
                        mX.insertAsFirst();
                    }
                    LOOP2_ASSERT(zi, ne, ne == X.length());
                    validateRBTree(X);
                    if (veryVerbose) P(X);
                    LOOP2_ASSERT(zi, ne, NUM_BLOCKS ==
                                               testAllocator.numBlocksTotal());
                    LOOP2_ASSERT(zi, ne, NUM_BYTES  ==
                                                testAllocator.numBytesInUse());
                  } END_BDEMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout << "\tWith no allocator (exercise only)." << endl;
        {
            const int MAX_NUM_ELEMS = 9;
            for (int zi = 0; zi < NUM_NB; ++zi) {
                for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                    if (veryVerbose) { cout << "\t\t"; P(ne) }
                    const Obj::InitialCapacity NE(ne);
                    Obj mX(NB[zi], NE);  const Obj &X = mX;
                    if (veryVerbose) P_(X);
                    for (int i = 0; i < ne; ++i) {
                        mX.insertAsFirst();
                    }
                    LOOP2_ASSERT(zi, ne, ne == X.length());
                    validateRBTree(X);
                    if (veryVerbose) P(X);
                }
            }
        }

        if (verbose) cout << "\tWith a buffer allocator (exercise only)."
                          << endl;
        {
            char memory[1024];
            const int MAX_NUM_ELEMS = 9;
            for (int zi = 0; zi < NUM_NB; ++zi) {
                for (int ne = 0; ne <= MAX_NUM_ELEMS; ++ne) {
                    bdema_BufferAllocator a(memory, sizeof memory);
                    if (veryVerbose) { cout << "\t\t"; P(ne) }
                    const Obj::InitialCapacity NE(ne);
                    Obj *doNotDelete = new(a.allocate(sizeof(Obj)))
                                                           Obj(NB[zi], NE, &a);
                    Obj &mX = *doNotDelete;  const Obj &X = mX;
                    if (veryVerbose) P_(X);
                    for (int i = 0; i < ne; ++i) {
                        mX.insertAsFirst();
                    }
                    LOOP2_ASSERT(zi, ne, ne == X.length());
                    validateRBTree(X);
                    if (veryVerbose) P(X);
                }
                // No destructor is called; will produce memory leak in purify
                // if internal allocators are not hooked up properly.
            }
        }

        if (verbose) cout << "\nTesting the 'reserveCapacity' method" << endl;
        {
            static const struct {
                int         d_lineNum;    // source line number
                const char *d_spec;       // specification string
                int         d_ne;         // total no. of elements to reserve
                int         d_height;     // expected r-b tree height
                const char *d_treeColors; // expected r-b tree coloration
                const char *d_treeTags;   // expected r-b tree node tags
            } DATA[] = {                                              // ADJUST
                //line  spec      ne    ht  tree colors     tree tags
                //----  ----      --    --  -----------     ---------
                { L_,   "",        0,    0, "x",             "x"             },
                { L_,   "",        1,    0, "x",             "x"             },
                { L_,   "",        2,    0, "x",             "x"             },
                { L_,   "",        3,    0, "x",             "x"             },
                { L_,   "",        4,    0, "x",             "x"             },
                { L_,   "",        5,    0, "x",             "x"             },
                { L_,   "",       15,    0, "x",             "x"             },
                { L_,   "",       16,    0, "x",             "x"             },
                { L_,   "",       17,    0, "x",             "x"             },

                { L_,   "A",       0,    1, "bxx",           "Axx"           },
                { L_,   "A",       1,    1, "bxx",           "Axx"           },
                { L_,   "A",       2,    1, "bxx",           "Axx"           },
                { L_,   "A",       3,    1, "bxx",           "Axx"           },
                { L_,   "A",       4,    1, "bxx",           "Axx"           },
                { L_,   "A",       5,    1, "bxx",           "Axx"           },
                { L_,   "A",      15,    1, "bxx",           "Axx"           },
                { L_,   "A",      16,    1, "bxx",           "Axx"           },
                { L_,   "A",      17,    1, "bxx",           "Axx"           },

                { L_,   "AB",      0,    2, "brxxx",         "ABxxx"         },
                { L_,   "AB",      1,    2, "brxxx",         "ABxxx"         },
                { L_,   "AB",      2,    2, "brxxx",         "ABxxx"         },
                { L_,   "AB",      3,    2, "brxxx",         "ABxxx"         },
                { L_,   "AB",      4,    2, "brxxx",         "ABxxx"         },
                { L_,   "AB",      5,    2, "brxxx",         "ABxxx"         },
                { L_,   "AB",     15,    2, "brxxx",         "ABxxx"         },
                { L_,   "AB",     16,    2, "brxxx",         "ABxxx"         },
                { L_,   "AB",     17,    2, "brxxx",         "ABxxx"         },

                { L_,   "ABCDE",   0,    3, "bbrxxrxxbxx",   "BDExxCxxAxx"   },
                { L_,   "ABCDE",   1,    3, "bbrxxrxxbxx",   "BDExxCxxAxx"   },
                { L_,   "ABCDE",   2,    3, "bbrxxrxxbxx",   "BDExxCxxAxx"   },
                { L_,   "ABCDE",   3,    3, "bbrxxrxxbxx",   "BDExxCxxAxx"   },
                { L_,   "ABCDE",   4,    3, "bbrxxrxxbxx",   "BDExxCxxAxx"   },
                { L_,   "ABCDE",   5,    3, "bbrxxrxxbxx",   "BDExxCxxAxx"   },
                { L_,   "ABCDE",  15,    3, "bbrxxrxxbxx",   "BDExxCxxAxx"   },
                { L_,   "ABCDE",  16,    3, "bbrxxrxxbxx",   "BDExxCxxAxx"   },
                { L_,   "ABCDE",  17,    3, "bbrxxrxxbxx",   "BDExxCxxAxx"   },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            static const int EXTEND[] = {
                0, 1, 4, 5, 7, 17, 23, 100
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE  = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec;
                const int         NE    = DATA[ti].d_ne;
                const int         HT    = DATA[ti].d_height;
                const char *const TCOLS = DATA[ti].d_treeColors;
                const char *const TTAGS = DATA[ti].d_treeTags;
                if (veryVerbose) { cout << "\t\t"; P_(SPEC); P(NE); }

                for (int zi = 0; zi < NUM_NB; ++zi) {
                    for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                      BEGIN_BDEMA_EXCEPTION_TEST {
                        Obj mX(NB[zi], &testAllocator);
                        stretchRemoveAll(&mX, EXTEND[ei]);
                        const Obj &X = mX;
                        mX.reserveCapacity(0);
                        LOOP3_ASSERT(LINE, zi, ei, 0 == X.length());
                        mX.reserveCapacity(NE);
                        const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                        const int NUM_BYTES  = testAllocator.numBytesInUse();
                        if (veryVerbose) P_(X);
                        for (int i = X.length(); i < NE; ++i) {
                            mX.insertAsFirst();
                        }
                        LOOP3_ASSERT(LINE, zi, ei, NE == X.length());
                        validateRBTree(X);
                        if (veryVerbose) P(X);
                        LOOP3_ASSERT(LINE, zi, ei, NUM_BLOCKS ==
                                               testAllocator.numBlocksTotal());
                        LOOP3_ASSERT(LINE, zi, ei, NUM_BYTES  ==
                                                testAllocator.numBytesInUse());
                      } END_BDEMA_EXCEPTION_TEST

                      Obj mY(NB[zi]);  const Obj& Y = gg(&mY, SPEC);
                      const int numNodes = std::strlen(TCOLS);
                      LOOP3_ASSERT(LINE, zi, ei,
                            numNodes == walkRBTree(Iter(Y), HT, TCOLS, TTAGS));
                      mY.reserveCapacity(NE);
                      LOOP3_ASSERT(LINE, zi, ei,
                            numNodes == walkRBTree(Iter(Y), HT, TCOLS, TTAGS));
                      validateRBTree(Y);
                    }
                }
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING REMOVE METHODS
        //
        // Concerns:
        //   We have the following specific concerns:
        //    1. Any node may be removed from a tree.
        //    2. Each removal operation produces the expected result:
        //       a. The sequence of nodes is correct.
        //       b. The result is a valid red-black tree.
        //       c. (white-box) The internal pointers (i.e., parent, child,
        //                      next) are consistent.
        //    3. Memory is released back to the allocator for later reuse.
        //
        // Plan:
        //   To address concerns 1-2, specify a set S of representative trees
        //   ordered by increasing length.  For each tree t in S, test that any
        //   node may be removed and verify that the expected result is
        //   produced using the appropriate helper functions.  To address
        //   concern 3, remove all nodes from t, then repopulate t with the
        //   same number of nodes that it contained before being emptied.
        //   Verify that no new memory is allocated while repopulating t.
        //
        // Testing:
        //  ^void remove(Node *node);
        //  ^void remove(Node *node, Node *previous);
        //
        //   void remove(const void *node);
        //   void remove(const void *node, const void *previous);
        //
        // Note: '^' indicates a private method which is tested indirectly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'remove' methods." << endl
                                  << "=========================" << endl;

        const int NUM_DATA = sizeof GDATA_REMOVE / sizeof *GDATA_REMOVE;

        if (verbose) cout << "\tTesting 'remove(node)' method." << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = GDATA_REMOVE[ti].d_lineNum;
                const char *const SPEC  = GDATA_REMOVE[ti].d_spec;
                const int         POS   = GDATA_REMOVE[ti].d_pos;
                const char *const LCOLS = GDATA_REMOVE[ti].d_listColors;
                const char *const LTAGS = GDATA_REMOVE[ti].d_listTags;
                const int         HT    = GDATA_REMOVE[ti].d_height;
                const char *const TCOLS = GDATA_REMOVE[ti].d_treeColors;
                const char *const TTAGS = GDATA_REMOVE[ti].d_treeTags;
                if (veryVerbose) { P_(LINE); P_(SPEC); P_(LCOLS); P(LTAGS); }
                const int specLen = std::strlen(SPEC);
                ASSERT(specLen - 1 == (int)std::strlen(LCOLS));
                ASSERT(specLen - 1 == (int)std::strlen(LTAGS));
                const int numNodes = std::strlen(TCOLS);
                ASSERT(numNodes    == (int)std::strlen(TTAGS));

                for (int zi = 0; zi < NUM_NB; ++zi) {
                    Obj mX(NB[zi], Z);  const Obj& X = gg(&mX, SPEC);
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\tBEFORE:"; P_(NB[zi]); P(X); }
                    Iter mI(X.first());  const Iter& I = mI;
                    for (int i = 0; i < POS; ++i) {
                        mI.moveNext();
                    }
                    mX.remove(I());
                    LOOP2_ASSERT(LINE, zi, specLen - 1 == X.length());
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\t AFTER:"; P_(NB[zi]); P(X); }

                    Iter mJ(X.first());  const Iter& J = mJ;
                    const char *colors = LCOLS;
                    const char *tags   = LTAGS;
                    for (; *tags && *colors; ++tags, ++colors) {
                        ASSERT('r' == *colors || 'b' == *colors);
                        ASSERT(isupper(*tags));

                        LOOP2_ASSERT(LINE, zi, J && J());

                        const int isRed = *colors == 'r';
                        LOOP2_ASSERT(LINE, zi,  isRed == J.isRed());
                        LOOP2_ASSERT(LINE, zi, !isRed == J.isBlack());

                        const char *tag = (char *)mJ();
                        LOOP2_ASSERT(LINE, zi, *tag == *tags);

                        mJ.moveNext();
                    }
                    LOOP2_ASSERT(LINE, zi, !J);
                    LOOP2_ASSERT(LINE, zi,
                            numNodes == walkRBTree(Iter(X), HT, TCOLS, TTAGS));

                    // Remove all, then repopulate to same length.
                    const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                    const int NUM_BYTES  = testAllocator.numBytesInUse();
                    while (X.first()) {
                        mX.remove(X.first());
                    }
                    LOOP2_ASSERT(LINE, zi, 0 == X.length());
                    for (int j = 0; j < specLen; ++j) {
                        mX.insertAsFirst();
                    }
                    LOOP2_ASSERT(LINE, zi, specLen    == X.length());
                    LOOP2_ASSERT(LINE, zi, NUM_BLOCKS ==
                                               testAllocator.numBlocksTotal());
                    LOOP2_ASSERT(LINE, zi, NUM_BYTES  ==
                                                testAllocator.numBytesInUse());
                }
            }
        }

        if (verbose) cout << "\tTesting 'remove(node, previous)' method."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = GDATA_REMOVE[ti].d_lineNum;
                const char *const SPEC  = GDATA_REMOVE[ti].d_spec;
                const int         POS   = GDATA_REMOVE[ti].d_pos;
                const char *const LCOLS = GDATA_REMOVE[ti].d_listColors;
                const char *const LTAGS = GDATA_REMOVE[ti].d_listTags;
                const int         HT    = GDATA_REMOVE[ti].d_height;
                const char *const TCOLS = GDATA_REMOVE[ti].d_treeColors;
                const char *const TTAGS = GDATA_REMOVE[ti].d_treeTags;
                if (veryVerbose) { P_(LINE); P_(SPEC); P_(LCOLS); P(LTAGS); }
                const int specLen = std::strlen(SPEC);
                ASSERT(specLen - 1 == (int)std::strlen(LCOLS));
                ASSERT(specLen - 1 == (int)std::strlen(LTAGS));
                const int numNodes = std::strlen(TCOLS);
                ASSERT(numNodes    == (int)std::strlen(TTAGS));

                for (int zi = 0; zi < NUM_NB; ++zi) {
                    Obj mX(NB[zi], Z);  const Obj& X = gg(&mX, SPEC);
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\tBEFORE:"; P_(NB[zi]); P(X); }
                    Iter mI(X.first());  const Iter& I = mI;
                    Iter mP;  // previous
                    for (int i = 0; i < POS; ++i) {
                        mP = I;
                        mI.moveNext();
                    }
                    mX.remove(I(), mP ? mP() : 0);
                    LOOP2_ASSERT(LINE, zi, specLen - 1 == X.length());
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\t AFTER:"; P_(NB[zi]); P(X); }

                    Iter mJ(X.first());  const Iter& J = mJ;
                    const char *colors = LCOLS;
                    const char *tags   = LTAGS;
                    for (; *tags && *colors; ++tags, ++colors) {
                        ASSERT('r' == *colors || 'b' == *colors);
                        ASSERT(isupper(*tags));

                        LOOP2_ASSERT(LINE, zi, J && J());

                        const int isRed = *colors == 'r';
                        LOOP2_ASSERT(LINE, zi,  isRed == J.isRed());
                        LOOP2_ASSERT(LINE, zi, !isRed == J.isBlack());

                        const char *tag = (char *)mJ();
                        LOOP2_ASSERT(LINE, zi, *tag == *tags);

                        mJ.moveNext();
                    }
                    LOOP2_ASSERT(LINE, zi, !J);
                    LOOP2_ASSERT(LINE, zi,
                            numNodes == walkRBTree(Iter(X), HT, TCOLS, TTAGS));

                    // Remove all, then repopulate to same length.
                    const int NUM_BLOCKS = testAllocator.numBlocksTotal();
                    const int NUM_BYTES  = testAllocator.numBytesInUse();
                    while (X.first()) {
                        Iter mI(X.first());  const Iter& I = mI;
                        mI.moveNext();
                        if (I) {
                            mX.remove(I(), X.first());
                        }
                        else {
                            mX.remove(X.first(), 0);
                        }
                    }
                    LOOP2_ASSERT(LINE, zi, 0 == X.length());
                    for (int j = 0; j < specLen; ++j) {
                        mX.insertAsFirst();
                    }
                    LOOP2_ASSERT(LINE, zi, specLen    == X.length());
                    LOOP2_ASSERT(LINE, zi, NUM_BLOCKS ==
                                               testAllocator.numBlocksTotal());
                    LOOP2_ASSERT(LINE, zi, NUM_BYTES  ==
                                                testAllocator.numBytesInUse());
                }
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING INSERT METHODS
        //
        // Concerns:
        //   We have the following specific concerns:
        //    1. A new node may be inserted at any position in a tree.
        //    2. Each insertion operation produces the expected result:
        //       a. The sequence of nodes is correct.
        //       b. The result is a valid red-black tree.
        //       c. (white-box) The internal pointers (i.e., parent, child,
        //                      next) are consistent.
        //    3. The container increases capacity as needed.
        //    4. The methods are exception neutral w.r.t. allocation.
        //
        // Plan:
        //   To address concerns 1-3, specify a set S of representative trees
        //   ordered by increasing length.  For each tree t in S:
        //    1. 'insertAsFirst' a new node and verify that the expected result
        //       is produced using the appropriate helper functions.
        //    2. Insert a new node at every position in t (via 'insertAfter')
        //       and again use our helper functions to verify the result.
        //
        //   Concern 4 is addressed by performing all of the insertion tests
        //   within the 'bdema' exception testing apparatus.
        //
        // Testing:
        //  ^Node *insertAfter(Node *node);
        //
        //   void *insertAfter(const void *node);
        //   void *insertAsFirst();
        //
        // Note: '^' indicates a private method which is tested indirectly.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'insert' methods."
                          << endl << "=========================" << endl;

        if (verbose) cout << "\tTesting 'insertAsFirst' method." << endl;
        {
            static const struct {
                int         d_lineNum;       // source line number
                const char *d_spec;          // specification string
                const char *d_listColors;    // expected r-b list coloration
                const char *d_listTags;      // expected r-b list node tags
                int         d_height;        // expected r-b tree height
                const char *d_treeColors;    // expected r-b tree coloration
                const char *d_treeTags;      // expected r-b tree node tags
            } DATA[] = {                                              // ADJUST
                //line spec           ht  list/tree colors  list/tree tags
                //---- ----           --  ----------------  --------------
                { L_,  "",                "b",              "X",
                                      1,  "bxx",            "Xxx"            },
                { L_,  "A",               "rb",             "XA",
                                      2,  "brxxx",          "AXxxx"          },
                { L_,  "AB",              "rbr",            "XBA",
                                      2,  "brxxrxx",        "BXxxAxx"        },
                { L_,  "ABC",             "rbbb",           "XCBA",
                                      3,  "bbrxxxbxx",      "BCXxxxAxx"      },
                { L_,  "ABCD",            "rbrbb",          "XDCBA",
                                      3,  "bbrxxrxxbxx",    "BDXxxCxxAxx"    },
                { L_,  "ABCDE",           "rbrbbb",         "XEDCBA",
                                      4,  "brbrxxxbxxbxx",  "BDEXxxxCxxAxx"  },
                { L_,  "ABCDEF",          "rbrrbbb",        "XFEDCBA",
                                      4,  "brbrxxrxxbxxbxx",
                                                           "BDFXxxExxCxxAxx" },
                { L_,  "ABCDEFG",         "rbrbbbrb",       "XGFEDCBA",
                                      4,  "brbrxxxbxxrbxxbxx",
                                                         "DFGXxxxExxBCxxAxx" },
                { L_,  "ABCDEFGH",        "rbrrbbbrb",      "XHGFEDCBA",
                                      4,  "brbrxxrxxbxxrbxxbxx",
                                                       "DFHXxxGxxExxBCxxAxx" },
                { L_,  "ABCDEFGHI",       "rbrbbbbbbb",     "XIHGFEDCBA",
                                      5,  "bbrbrxxxbxxbxxbbxxbxx",
                                                     "DFHIXxxxGxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJ",      "rbrrbbbbbbb",    "XJIHGFEDCBA",
                                      5,  "bbrbrxxrxxbxxbxxbbxxbxx",
                                                   "DFHJXxxIxxGxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJK",     "rbrbbbrbbbbb",   "XKJIHGFEDCBA",
                                      5,  "bbrbrxxxbxxrbxxbxxbbxxbxx",
                                                 "DHJKXxxxIxxFGxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJKL",    "rbrrbbbrbbbbb",  "XLKJIHGFEDCBA",
                                      5,  "bbrbrxxrxxbxxrbxxbxxbbxxbxx",
                                               "DHJLXxxKxxIxxFGxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJKLM",   "rbrbbbrbbbbbbb", "XMLKJIHGFEDCBA",
                                      6,  "brbrbrxxxbxxbxxbbxxbxxbbxxbxx",
                                             "DHJLMXxxxKxxIxxFGxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJKLMN",  "rbrrbbbrbbbbbbb","XNMLKJIHGFEDCBA",
                                      6,  "brbrbrxxrxxbxxbxxbbxxbxxbbxxbxx",
                                           "DHJLNXxxMxxKxxIxxFGxxExxBCxxAxx" },
                { L_,  "ABCDEFGHIJKLMNO", "rbrbbbrbrbbbbbbb",
                                                           "XONMLKJIHGFEDCBA",
                                      6,  "brbrbrxxxbxxrbxxbxxbbxxbxxbbxxbxx",
                                         "DHLNOXxxxMxxJKxxIxxFGxxExxBCxxAxx" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec;
                const char *const LCOLS = DATA[ti].d_listColors;
                const char *const LTAGS = DATA[ti].d_listTags;
                const int         HT    = DATA[ti].d_height;
                const char *const TCOLS = DATA[ti].d_treeColors;
                const char *const TTAGS = DATA[ti].d_treeTags;
                if (veryVerbose) { P_(LINE); P_(SPEC); P_(LCOLS); P(LTAGS); }
                const int specLen = std::strlen(SPEC);
                ASSERT(specLen + 1 == (int)std::strlen(LCOLS));
                ASSERT(specLen + 1 == (int)std::strlen(LTAGS));
                const int numNodes = std::strlen(TCOLS);
                ASSERT(numNodes    == (int)std::strlen(TTAGS));

                for (int zi = 0; zi < NUM_NB; ++zi) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj mX(NB[zi], Z);  const Obj& X = gg(&mX, SPEC);
                    LOOP2_ASSERT(LINE, zi, specLen == X.length());
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\tBEFORE:"; P_(NB[zi]); P(X); }
                    char *node = (char *)mX.insertAsFirst();
                    *node = 'X';
                    LOOP2_ASSERT(LINE, zi, specLen + 1 == X.length());
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\t AFTER:"; P_(NB[zi]); P(X); }

                    Iter mI(X.first());  const Iter& I = mI;
                    const char *colors = LCOLS;
                    const char *tags   = LTAGS;
                    for (; *tags && *colors; ++tags, ++colors) {
                        ASSERT('r' == *colors || 'b' == *colors);
                        ASSERT(isupper(*tags));

                        LOOP2_ASSERT(LINE, zi, I && I());

                        const int isRed = *colors == 'r';
                        LOOP2_ASSERT(LINE, zi,  isRed == I.isRed());
                        LOOP2_ASSERT(LINE, zi, !isRed == I.isBlack());

                        const char *tag = (char *)mI();
                        LOOP2_ASSERT(LINE, zi, *tag == *tags);

                        mI.moveNext();
                    }
                    LOOP2_ASSERT(LINE, zi, !I);
                    LOOP2_ASSERT(LINE, zi,
                            numNodes == walkRBTree(Iter(X), HT, TCOLS, TTAGS));
                  } END_BDEMA_EXCEPTION_TEST
                }
            }
        }

        if (verbose) cout << "\tTesting 'insertAfter' method." << endl;
        {
            const int NUM_DATA =
                          sizeof GDATA_INSERTAFTER / sizeof *GDATA_INSERTAFTER;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = GDATA_INSERTAFTER[ti].d_lineNum;
                const char *const SPEC  = GDATA_INSERTAFTER[ti].d_spec;
                const int         POS   = GDATA_INSERTAFTER[ti].d_pos;
                const char *const LCOLS = GDATA_INSERTAFTER[ti].d_listColors;
                const char *const LTAGS = GDATA_INSERTAFTER[ti].d_listTags;
                const int         HT    = GDATA_INSERTAFTER[ti].d_height;
                const char *const TCOLS = GDATA_INSERTAFTER[ti].d_treeColors;
                const char *const TTAGS = GDATA_INSERTAFTER[ti].d_treeTags;
                if (veryVerbose) { P_(LINE); P_(SPEC); P_(LCOLS); P(LTAGS); }
                const int specLen = std::strlen(SPEC);
                ASSERT(specLen + 1 == (int)std::strlen(LCOLS));
                ASSERT(specLen + 1 == (int)std::strlen(LTAGS));
                const int numNodes = std::strlen(TCOLS);
                ASSERT(numNodes    == (int)std::strlen(TTAGS));

                for (int zi = 0; zi < NUM_NB; ++zi) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    Obj mX(NB[zi], Z);  const Obj& X = gg(&mX, SPEC);
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\tBEFORE:"; P_(NB[zi]); P(X); }
                    Iter mI(X.first());  const Iter& I = mI;
                    for (int i = 0; i < POS; ++i) {
                        mI.moveNext();
                    }
                    char *node = (char *)mX.insertAfter(I());
                    *node = 'X';
                    LOOP2_ASSERT(LINE, zi, specLen + 1 == X.length());
                    validateRBTree(X);
                    if (veryVerbose) { cout << "\t AFTER:"; P_(NB[zi]); P(X); }

                    Iter mJ(X.first());  const Iter& J = mJ;
                    const char *colors = LCOLS;
                    const char *tags   = LTAGS;
                    for (; *tags && *colors; ++tags, ++colors) {
                        ASSERT('r' == *colors || 'b' == *colors);
                        ASSERT(isupper(*tags));

                        LOOP2_ASSERT(LINE, zi, J && J());

                        const int isRed = *colors == 'r';
                        LOOP2_ASSERT(LINE, zi,  isRed == J.isRed());
                        LOOP2_ASSERT(LINE, zi, !isRed == J.isBlack());

                        const char *tag = (char *)mJ();
                        LOOP2_ASSERT(LINE, zi, *tag == *tags);

                        mJ.moveNext();
                    }
                    LOOP2_ASSERT(LINE, zi, !J);
                    LOOP2_ASSERT(LINE, zi,
                            numNodes == walkRBTree(Iter(X), HT, TCOLS, TTAGS));
                  } END_BDEMA_EXCEPTION_TEST
                }
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING PRINT METHOD
        //
        // Concerns:
        //   The print method formats the value of the object directly from
        //   the underlying state information according to supplied arguments.
        //   Ensure that the method formats properly for:
        //     - Empty and non-empty values.
        //     - Negative, 0, and positive levels.
        //     - 0 and non-zero spaces per level.
        // Plan:
        //   For each of an enumerated set of object, 'level', and
        //   'spacesPerLevel' values, ordered by increasing object length, use
        //   'ostrstream' to 'print' that object's value, using the tabulated
        //   parameters, to two separate character buffers each with different
        //   initial values.  Compare the contents of these buffers with the
        //   literal expected output format and verify that the characters
        //   beyond the null characters are unaffected in both buffers.
        //
        // Testing:
        //   ostream& print(ostream& stream, int level, int spl) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'print' method" << endl
                                  << "======================" << endl;

        if (verbose) cout << "\nTesting 'print' (ostream)." << endl;
#define NL "\n"
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  spec    indent +/-  spaces/Tab  format        // ADJUST
                //----  ----    ----------  ----------  ----------------
                { L_,   "",        0,         0,        "nil"         NL  },

                { L_,   "",        0,         2,        "nil"         NL  },

                { L_,   "",        1,         1,        " nil"        NL  },

                { L_,   "",        1,         2,        "  nil"       NL  },

                { L_,   "",       -1,         2,        "nil"         NL  },

                { L_,   "A",       0,         0,        "b"           NL
                                                        "nil"         NL
                                                        "nil"         NL  },

                { L_,   "A",      -2,         1,        "b"           NL
                                                        "   nil"      NL
                                                        "   nil"      NL  },

                { L_,   "BC",      1,         2,        "  b"         NL
                                                        "    r"       NL
                                                        "      nil"   NL
                                                        "      nil"   NL
                                                        "    nil"     NL  },

                { L_,   "BC",      2,         1,        "  b"         NL
                                                        "   r"        NL
                                                        "    nil"     NL
                                                        "    nil"     NL
                                                        "   nil"      NL  },

                { L_,   "ABCDE",   1,         3,        "   b"        NL
                                                        "      b"     NL
                                                       "         r"   NL
                                                 "            nil"    NL
                                                 "            nil"    NL
                                                       "         r"   NL
                                                 "            nil"    NL
                                                 "            nil"    NL
                                                          "      b"   NL
                                                    "         nil"    NL
                                                    "         nil"    NL  },
            };
#undef NL

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000;       // Must be able to hold output string.
            const char Z1 = (char)0xFF;  // Value 1 used for an unset char.
            const char Z2 = 0x00;        // Value 2 used for an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int         IND  = DATA[ti].d_indent;
                const int         SPL  = DATA[ti].d_spaces;
                const char *const FMT  = DATA[ti].d_fmt_p;
                const int curLen = std::strlen(SPEC);

                for (int zi = 0; zi < NUM_NB; ++zi) {
                    char buf1[SIZE + epcBug], buf2[SIZE + epcBug];
                    memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                    memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                    Obj mX(NB[zi], &testAllocator);
                    const Obj& X = gg(&mX, SPEC);
                    validateRBTree(X);
                    LOOP2_ASSERT(LINE, zi, curLen == X.length());
                                                                // same lengths

                    if (curLen != oldLen) {
                        if (verbose) cout << "\ton objects of length "
                                          << curLen << ':' << endl;
                        LOOP_ASSERT(LINE, oldLen < curLen); // strictly incr.
                        oldLen = curLen;
                    }

                    if (verbose) { cout << "\t\tSpec = \"" << SPEC << "\", ";
                                   P_(IND); P(SPL); }
                    if (veryVerbose) cout << "EXPECTED FORMAT:" << endl << FMT
                                          << endl;
                    ostrstream out1(buf1, SIZE);
                    X.print(out1, IND, SPL) << ends;

                    ostrstream out2(buf2, SIZE);
                    X.print(out2, IND, SPL) << ends;
                    if (veryVerbose) cout << "ACTUAL FORMAT:" << endl << buf1
                                          << endl;

                    const int SZ   = std::strlen(FMT) + 1;
                    const int REST = SIZE - SZ;
                    LOOP2_ASSERT(LINE, zi, SZ < SIZE);    // buffer big enough?
                    LOOP2_ASSERT(LINE, zi, Z1 == buf1[SIZE - 1]);   // overrun?
                    LOOP2_ASSERT(LINE, zi, Z2 == buf2[SIZE - 1]);   // overrun?
                    LOOP2_ASSERT(LINE, zi, 0 == strcmp(buf1, FMT));
                    LOOP2_ASSERT(LINE, zi, 0 == strcmp(buf2, FMT));
                    LOOP2_ASSERT(LINE, zi, 0 ==
                                       memcmp(buf1 + SZ, CTRL_BUF1 + SZ,REST));
                    LOOP2_ASSERT(LINE, zi, 0 ==
                                       memcmp(buf2 + SZ, CTRL_BUF2 + SZ,REST));
                }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR
        //
        // Concerns:
        //   Since the output operator is layered on basic accessors, it is
        //   sufficient to test only the output *format* (and to ensure that
        //   no additional characters are written past the terminating null).
        //
        // Plan:
        //   For each of a small representative set of object values, ordered
        //   by increasing length, use 'ostrstream' to write that object's
        //   value to two separate character buffers each with different
        //   initial values.  Compare the contents of these buffers with the
        //   literal expected output format and verify that the characters
        //   beyond the null characters are unaffected in both buffers.  Note
        //   that the output ordering is guaranteed and the function 'strcmp'
        //   is used to validate equality of the output to the expected output.
        //
        // Testing:
        //   ostream& operator<<(ostream& stream, const bdeci_RbTreeImp& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Output (<<) Operator" << endl
                                  << "============================" << endl;

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  spec         output format                    // ADJUST
                //----  ----         -------------
                { L_,   "",          "()"                                    },
                { L_,   "A",         "(b () ())"                             },
                { L_,   "AB",        "(b (r () ()) ())"                      },
                { L_,   "ABC",       "(b (r () ()) (r () ()))"               },
                { L_,   "ABCDE",     "(b (b (r () ()) (r () ())) (b () ()))" },
                { L_,   "ABCDEABCDE",
  "(b (b (r (b (r () ()) ()) (b () ())) (b () ())) (b (b () ()) (b () ())))" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000; // Must be big enough to hold output string.
            const char Z1 = 0x7f;  // Value 1 used to represent an unset char.
            const char Z2 = 0x00;  // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const char *const FMT  = DATA[ti].d_fmt_p;
                const int curLen = std::strlen(SPEC);

                for (int zi = 0; zi < NUM_NB; ++zi) {
                    char buf1[SIZE + epcBug], buf2[SIZE + epcBug];
                    memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                    memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                    Obj mX(NB[zi], &testAllocator);
                    const Obj& X = gg(&mX, SPEC);
                    validateRBTree(X);
                    LOOP_ASSERT(ti, curLen == X.length());  // same lengths

                    if (curLen != oldLen) {
                        if (verbose) cout << "\ton objects of length "
                                          << curLen << ':' << endl;
                        LOOP_ASSERT(LINE, oldLen < curLen);  // strictly incr.
                        oldLen = curLen;
                    }

                    if (verbose) cout << "\t\tSpec = \"" << SPEC << '"'
                                      << endl;
                    if (veryVerbose) cout << "EXPECTED FORMAT:" << endl << FMT
                                          << endl;
                    ostrstream out1(buf1, SIZE);  out1 << X << ends;
                    ostrstream out2(buf2, SIZE);  out2 << X << ends;
                    if (veryVerbose) cout << "ACTUAL FORMAT:" << endl << buf1
                                          << endl;

                    const int SZ   = std::strlen(FMT) + 1;
                    const int REST = SIZE - SZ;
                    LOOP2_ASSERT(LINE, zi, SZ < SIZE);   // buffer big enough?
                    LOOP2_ASSERT(LINE, zi, Z1 == buf1[SIZE - 1]);  // overrun?
                    LOOP2_ASSERT(LINE, zi, Z2 == buf2[SIZE - 1]);  // overrun?
                    LOOP2_ASSERT(LINE, zi, 0 == strcmp(buf1, FMT));
                    LOOP2_ASSERT(LINE, zi, 0 == strcmp(buf2, FMT));
                    LOOP2_ASSERT(LINE, zi, 0 ==
                                      memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                    LOOP2_ASSERT(LINE, zi, 0 ==
                                      memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
                }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING 'bdeci_RbTreeImpIter' MANIPULATORS AND DIRECT ACCESSORS
        //
        // Concerns:
        //   The 'bdeci_RbTreeImpIter' manipulators and direct accessors are
        //   trivial, so our primary concern is that they operate as expected.
        //   A secondary concern is that the associated tree is unaffected by
        //   the iterator methods under test.
        //
        // Plan:
        //   Specify a set S of representative trees ordered by increasing
        //   length.  For each value t in S:
        //    1. Initialize an iterator to the first node in t and sequentially
        //       iterate over the entire tree.  Assert the expected result of
        //       relevant accessors at each step in the iteration.  After the
        //       iteration is complete, verify that the associated tree was
        //       unchanged.
        //    2. Initialize an iterator to the root of t and recursively
        //       iterate over the entire tree.  Assert the expected result of
        //       relevant accessors at each step in the iteration.  After the
        //       iteration is complete, verify that the associated tree was
        //       unchanged.  Note that the helper function, 'walkRBTree', that
        //       effects the recursive traversal is assumed to be correct.
        //
        // Testing:
        //   void moveLeft();
        //   void moveNext();
        //   void moveRight();
        //   operator const void *() const;
        //   const void *operator()() const;
        //   int hasLeft() const;
        //   int hasRight() const;
        //   int isBlack() const;
        //   int isRed() const;
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl <<
              "Testing 'bdeci_RbTreeImpIter' Manipulators and Direct Accessors"
              "==============================================================="
                 << endl;
        }

        if (verbose) cout << "\tTesting sequential traversals." << endl;
        {
            const int NUM_DATA = sizeof GDATA_ITERATE / sizeof *GDATA_ITERATE;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = GDATA_ITERATE[ti].d_lineNum;
                const char *const SPEC  = GDATA_ITERATE[ti].d_spec;
                const char *const LCOLS = GDATA_ITERATE[ti].d_listColors;
                const char *const LTAGS = GDATA_ITERATE[ti].d_listTags;
                const int         HT    = GDATA_ITERATE[ti].d_height;
                const char *const TCOLS = GDATA_ITERATE[ti].d_treeColors;
                const char *const TTAGS = GDATA_ITERATE[ti].d_treeTags;
                if (veryVerbose) { P_(LINE); P_(SPEC); P_(LCOLS); P(LTAGS); }
                const int specLen  = std::strlen(SPEC);
                const int numNodes = std::strlen(TCOLS);
                ASSERT(specLen == (int)std::strlen(LCOLS));
                ASSERT(specLen == (int)std::strlen(LTAGS));

                for (int zi = 0; zi < NUM_NB; ++zi) {
                    Obj mX(NB[zi]);  const Obj& X = gg(&mX, SPEC);
                    validateRBTree(X);
                    LOOP2_ASSERT(LINE, zi,
                            numNodes == walkRBTree(Iter(X), HT, TCOLS, TTAGS));
                    if (veryVerbose) { TAB; P_(NB[zi]); P(X); }

                    Iter mI(X.first());  const Iter& I = mI;
                    const char *colors = LCOLS;
                    const char *tags   = LTAGS;
                    for (; *tags && *colors; ++tags, ++colors) {
                        ASSERT('r' == *colors || 'b' == *colors);
                        ASSERT(isupper(*tags));

                        LOOP2_ASSERT(LINE, zi, I && I());

                        const int isRed = *colors == 'r';
                        LOOP2_ASSERT(LINE, zi,  isRed == I.isRed());
                        LOOP2_ASSERT(LINE, zi, !isRed == I.isBlack());

                        const char *tag = (char *)mI();
                        LOOP2_ASSERT(LINE, zi, *tag == *tags);

                        mI.moveNext();
                    }
                    LOOP2_ASSERT(LINE, zi, !I);
                    validateRBTree(X);
                    LOOP2_ASSERT(LINE, zi,
                            numNodes == walkRBTree(Iter(X), HT, TCOLS, TTAGS));
                }
            }
        }

        if (verbose) cout << "\tTesting recursive traversals." << endl;
        {
            const int NUM_DATA = sizeof GDATA_ITERATE / sizeof *GDATA_ITERATE;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = GDATA_ITERATE[ti].d_lineNum;
                const char *const SPEC  = GDATA_ITERATE[ti].d_spec;
                const int         HT    = GDATA_ITERATE[ti].d_height;
                const char *const TCOLS = GDATA_ITERATE[ti].d_treeColors;
                const char *const TTAGS = GDATA_ITERATE[ti].d_treeTags;
                if (veryVerbose) { P_(LINE); P_(SPEC); P_(TCOLS); P(TTAGS); }
                const int numNodes = std::strlen(TCOLS);
                ASSERT(numNodes == (int)std::strlen(TTAGS));

                for (int zi = 0; zi < NUM_NB; ++zi) {
                    Obj mX(NB[zi]);  const Obj& X = gg(&mX, SPEC);
                    validateRBTree(X);
                    if (veryVerbose) { TAB; P_(NB[zi]); P(X); }
                    Iter mI(X.root());
                    LOOP2_ASSERT(LINE, zi,
                            numNodes == walkRBTree(Iter(X), HT, TCOLS, TTAGS));
                    validateRBTree(X);
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING 'bdeci_RbTreeImp' DIRECT ACCESSORS
        //
        // Concerns:
        //   The 'bdeci_RbTreeImp' direct accessors are trivial.  Our primary
        //   concern is that they operate properly on a small number of test
        //   cases.
        //
        // Plan:
        //   Specify a small set S of representative trees ordered by
        //   increasing length.  For each value t in S, verify with appropriate
        //   assertions that the direct accessors yield the expected result.
        //
        // Testing:
        //   const void *first() const;
        //   const void *root() const;
        //   int length() const;
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "Testing 'bdeci_RbTreeImp' Direct Accessors"
                 << endl << "=========================================="
                 << endl;

        static const struct {
            int         d_lineNum;     // source line number
            const char *d_spec;        // specification string
            int         d_ne;          // expected number of nodes
            char        d_firstTag;    // expected tag of first node
            char        d_rootTag;     // expected tag of root node
        } DATA[] = {
            //line  spec        ne     1st tag     root tag           // ADJUST
            //----  ----        --     -------     --------
            { L_,   "",         0,     'x',        'x'                       },

            { L_,   "A",        1,     'A',        'A'                       },

            { L_,   "AB",       2,     'B',        'A'                       },
            { L_,   "AaB",      2,     'A',        'A'                       },

            { L_,   "ABC",      3,     'C',        'B'                       },
            { L_,   "AaBC",     3,     'C',        'A'                       },
            { L_,   "ABaC",     3,     'B',        'A'                       },
            { L_,   "ABbC",     3,     'B',        'C'                       },
            { L_,   "AaBaC",    3,     'A',        'C'                       },
            { L_,   "AaBbC",    3,     'A',        'B'                       },

            { L_,   "ABCD",     4,     'D',        'B'                       },
            { L_,   "AaBCD",    4,     'D',        'A'                       },
            { L_,   "ABaCD",    4,     'D',        'A'                       },
            { L_,   "ABCaD",    4,     'C',        'B'                       },
            { L_,   "ABbCD",    4,     'D',        'C'                       },
            { L_,   "ABCbD",    4,     'C',        'B'                       },
            { L_,   "ABCcD",    4,     'C',        'B'                       },
            { L_,   "AaBaCD",   4,     'D',        'C'                       },
            { L_,   "AaBCaD",   4,     'C',        'A'                       },
            { L_,   "ABaCaD",   4,     'B',        'A'                       },
            { L_,   "AaBbCD",   4,     'D',        'B'                       },
            { L_,   "AaBCbD",   4,     'C',        'A'                       },
            { L_,   "ABaCbD",   4,     'B',        'A'                       },
            { L_,   "AaBCcD",   4,     'C',        'A'                       },
            { L_,   "ABaCcD",   4,     'B',        'A'                       },
            { L_,   "ABbCaD",   4,     'B',        'C'                       },
            { L_,   "ABbCbD",   4,     'B',        'C'                       },
            { L_,   "ABbCcD",   4,     'B',        'C'                       },
            { L_,   "AaBaCaD",  4,     'A',        'C'                       },
            { L_,   "AaBaCbD",  4,     'A',        'C'                       },
            { L_,   "AaBaCcD",  4,     'A',        'C'                       },
            { L_,   "AaBbCaD",  4,     'A',        'B'                       },
            { L_,   "AaBbCbD",  4,     'A',        'B'                       },
            { L_,   "AaBbCcD",  4,     'A',        'B'                       },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int         LINE     = DATA[ti].d_lineNum;
            const char *const SPEC     = DATA[ti].d_spec;
            const int         NE       = DATA[ti].d_ne;
            const char        FIRSTTAG = DATA[ti].d_firstTag;
            const char        ROOTTAG  = DATA[ti].d_rootTag;
            if (veryVerbose) {
                P_(LINE); P_(SPEC); P_(NE); P_(FIRSTTAG); P(ROOTTAG);
            }

            for (int zi = 0; zi < NUM_NB; ++zi) {
                Obj mX(NB[zi]);  const Obj& X = gg(&mX, SPEC);
                validateRBTree(X);
                if (veryVerbose) { TAB; P_(NB[zi]); P(X); }
                if (NE == 0) {
                    LOOP2_ASSERT(LINE, zi, 0 == X.first());
                    LOOP2_ASSERT(LINE, zi, 0 == X.root());
                }
                else {
                    LOOP2_ASSERT(LINE, zi, X.first());
                    const char firstTag = *(char *)X.first();
                    LOOP2_ASSERT(LINE, zi, FIRSTTAG == firstTag);

                    LOOP2_ASSERT(LINE, zi, X.root());
                    const char rootTag = *(char *)X.root();
                    LOOP2_ASSERT(LINE, zi, ROOTTAG == rootTag);
                    if (veryVerbose) { P_(firstTag); P(rootTag); }
                }
                LOOP2_ASSERT(LINE, zi, NE == X.length());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING 'bdeci_RbTreeImpIter' PRIMARY MANIPULATORS
        //
        // Concerns:
        //   Our primary concern is that the constructor accepting a tree as
        //   an argument properly initializes the iterator to be positioned at
        //   the root of the tree.  The default constructor and the destructor
        //   are trivial.
        //
        // Plan:
        //   Specify a small set S of representative trees ordered by
        //   increasing length.  For each value t in S, initialize a newly-
        //   constructed iterator i with t.  Using direct accessors, verify
        //   that i is initially positioned at the root of t.
        //
        // Testing:
        //   bdeci_RbTreeImpIter();
        //   bdeci_RbTreeImpIter(const bdeci_RbTreeImp& tree);
        //   ~bdeci_RbTreeImpIter();
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl << "Testing 'bdeci_RbTreeImpIter' ctor's and dtor"
                 << endl << "============================================="
                 << endl;

        if (verbose) cout << "\tTesting 'bdeci_RbTreeImpIter()' and 'dtor'."
                          << endl;
        {
            Iter mI;  const Iter& I = mI;
            ASSERT(!I);
        }

        if (verbose)
            cout << "\tTesting 'ctor(const bdeci_RbTreeImp&)' and 'dtor'."
                 << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec;     // specification string
            } DATA[] = {
                //line  spec
                //----  ----
                { L_,   "",                       },
                { L_,   "A",                      },
                { L_,   "AB",                     },
                { L_,   "ABC",                    },
                { L_,   "ABCD",                   },
                { L_,   "ABCDE",                  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec;
                if (veryVerbose) { P_(LINE); P(SPEC); }

                for (int zi = 0; zi < NUM_NB; ++zi) {
                    Obj mX(NB[zi]);  const Obj& X = gg(&mX, SPEC);
                    validateRBTree(X);
                    if (veryVerbose) { TAB; P_(NB[zi]); P(X); }
                    Iter mI(X);  const Iter& I = mI;
                    if (ti) {
                        LOOP2_ASSERT(LINE, zi, I && I() == X.root());
                    }
                    else {
                        LOOP2_ASSERT(LINE, zi, !I);
                    }
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PRIMITIVE GENERATOR AND HELPER FUNCTIONS
        //
        // Concerns:
        //   Having demonstrated that our primary manipulators work as expected
        //   under normal conditions, we want to verify (1) that valid
        //   generator syntax produces expected results and (2) that invalid
        //   syntax is detected and reported.
        //
        //   We want also to make trustworthy some additional test helper
        //   functionality that we will use within the first few test cases:
        //    - 'stretch'          Tested separately to observe stretch occurs.
        //    - 'stretchRemoveAll' Deliberately implemented using 'stretch'.
        //
        //   Finally we want to make sure that we can rationalize the internal
        //   memory management with respect to the primary manipulators (i.e.,
        //   precisely when new blocks are allocated and deallocated).
        //
        // Plan:
        //   For each of an enumerated sequence of 'spec' values, ordered by
        //   increasing 'spec' length, use the primitive generator function
        //   'gg' to set the state of a newly created object.  Verify that 'gg'
        //   returns a valid reference to the modified argument object and,
        //   using basic accessors, that the value of the object is as
        //   expected.  Repeat the test for a longer 'spec' generated by
        //   prepending a string ending in a '~' character (denoting
        //   'removeAll').  Note that we are testing the parser only; the
        //   primary manipulators are already assumed to work.
        //
        //   To verify that the stretching functions work as expected (and to
        //   cross-check that internal memory is being managed as intended),
        //   create a depth-ordered enumeration of initial values and sizes by
        //   which to extend the initial value.  Record as expected values the
        //   total number of memory blocks allocated during the first and
        //   second modifications of each object.  For each test vector,
        //   construct two identical objects X and Y and bring each to the
        //   initial state.  Assert that the memory allocation for the two
        //   operations are identical and consistent with the first expected
        //   value.  Next apply the 'stretch' and 'stretchRemoveAll' functions
        //   to X and Y (respectively) and again compare the memory allocation
        //   characteristics for the two functions.  Note that we will track
        //   the *total* number of *blocks* allocated as well as the *current*
        //   number of *bytes* in use -- this is to measure different aspects
        //   of the operation while remaining insensitive to 'Element' size.
        //
        // Testing:
        //   Obj& gg(Obj *object, const char *spec);
        //   int ggg(Obj *object, const char *spec, int vF = 1);
        //   void stretch(Obj *object, int size);
        //   void stretchRemoveAll(Obj *object, int size);
        //   CONCERN: Is the internal memory organization behaving as intended?
        // --------------------------------------------------------------------

        if (verbose)
            cout << endl
                 << "Testing Primitive Generator Function 'gg'" << endl
                 << "=========================================" << endl;

        const int ELT_SIZE = 7;  // > 0, but otherwise arbitrary

        if (verbose) cout << "\nTesting generator on valid specs." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_length;   // expected length
            } DATA[] = {
                //line  spec            length                        // ADJUST
                //----  ----            ------
                { L_,   "",             0,                                },

                { L_,   "A",            1,                                },
                { L_,   "B",            1,                                },
                { L_,   "~",            0,                                },

                { L_,   "CD",           2,                                },
                { L_,   "E~",           0,                                },
                { L_,   "~E",           1,                                },
                { L_,   "~~",           0,                                },

                { L_,   "ABC",          3,                                },
                { L_,   "~BC",          2,                                },
                { L_,   "A~C",          1,                                },
                { L_,   "AB~",          0,                                },
                { L_,   "~~C",          1,                                },
                { L_,   "~B~",          0,                                },
                { L_,   "A~~",          0,                                },
                { L_,   "~~~",          0,                                },

                { L_,   "ABCD",         4,                                },
                { L_,   "~BCD",         3,                                },
                { L_,   "A~CD",         2,                                },
                { L_,   "AB~D",         1,                                },
                { L_,   "ABC~",         0,                                },

                { L_,   "ABCDE",        5,                                },
                { L_,   "~BCDE",        4,                                },
                { L_,   "AB~DE",        2,                                },
                { L_,   "ABCD~",        0,                                },
                { L_,   "A~C~E",        1,                                },
                { L_,   "~B~D~",        0,                                },

                { L_,   "~CBA~~ABCDE",  5,                                },

                { L_,   "ABCDE~CDEC~E", 1,                                },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         LENGTH = DATA[ti].d_length;
                const int curLen = std::strlen(SPEC);

                Obj mX(ELT_SIZE, &testAllocator);
                const Obj& X = gg(&mX, SPEC);   // original spec
                validateRBTree(X);

                static const char *const MORE_SPEC = "~ABCDEABCDEABCDEABCDE~";
                char buf[100]; strcpy(buf, MORE_SPEC); strcat(buf, SPEC);

                Obj mY(ELT_SIZE, &testAllocator);
                const Obj& Y = gg(&mY, buf);    // extended spec
                validateRBTree(Y);

                if (curLen != oldLen) {
                    if (verbose) cout << "\tof length " << curLen << ':'
                                      << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                if (veryVerbose) {
                    cout << "\t\t   Spec = \"" << SPEC << '"' << endl;
                    cout << "\t\tBigSpec = \"" << buf  << '"' << endl;
                    cout << "\t\t\t"; P(X);
                    cout << "\t\t\t"; P(Y);
                }

                LOOP_ASSERT(LINE, LENGTH == X.length());
                LOOP_ASSERT(LINE, LENGTH == Y.length());
            }
        }

        if (verbose) cout << "\nTesting generator on invalid specs." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_index;    // offending character index
            } DATA[] = {
                //line  spec            index      // ADJUST
                //----  -------------   -----
                { L_,   "",             -1,     }, // control

                { L_,   "~",            -1,     }, // control
                { L_,   " ",             0,     },
                { L_,   ".",             0,     },
                { L_,   "1",             0,     },

                { L_,   "AE",           -1,     }, // control
                { L_,   "aE",            0,     },
                { L_,   "Ae",            1,     },
                { L_,   ".~",            0,     },
                { L_,   "~!",            1,     },
                { L_,   "  ",            0,     },

                { L_,   "ABC",          -1,     }, // control
                { L_,   " BC",           0,     },
                { L_,   "A C",           1,     },
                { L_,   "AB ",           2,     },
                { L_,   "?#:",           0,     },
                { L_,   "   ",           0,     },

                { L_,   "ABCDE",        -1,     }, // control
                { L_,   "aBCDE",         0,     },
                { L_,   "ABcDE",         2,     },
                { L_,   "ABCDe",         4,     },
                { L_,   "AbCdE",         1,     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE  = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const int         INDEX = DATA[ti].d_index;
                const int curLen = std::strlen(SPEC);

                Obj mX(ELT_SIZE, &testAllocator);

                if (curLen != oldLen) {
                    if (verbose) cout << "\tof length " << curLen << ':'
                                      << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                if (veryVerbose) cout << "\t\tSpec = \"" << SPEC << '"'
                                      << endl;

                int result = ggg(&mX, SPEC, veryVerbose);

                LOOP_ASSERT(LINE, INDEX == result);
            }
        }

        if (verbose) cout << "\nTesting 'stretch' and 'stretchRemoveAll'."
                          << endl;
        {
            static const struct {
                int         d_lineNum;       // source line number
                const char *d_spec_p;        // specification string
                int         d_size;          // amount to grow (also length)
                int         d_firstResize;   // total blocks allocated
                int         d_secondResize;  // total blocks allocated

                // Note: total blocks (first/second Resize) and whether or not
                // 'removeAll' deallocates memory depends on 'Element' type.

            } DATA[] = {                                              // ADJUST
                //line  spec            size    firstResize     secondResize
                //----  -------------   ----    -----------     ------------
                { L_,   "",             0,      0,              0            },

                { L_,   "",             1,      0,              1            },
                { L_,   "A",            0,      1,              0            },

                { L_,   "",             2,      0,              2            },
                { L_,   "A",            1,      1,              1            },
                { L_,   "AB",           0,      2,              0            },

                { L_,   "",             3,      0,              2            },
                { L_,   "A",            2,      1,              1            },
                { L_,   "AB",           1,      2,              0            },
                { L_,   "ABC",          0,      2,              0            },

                { L_,   "",             4,      0,              3            },
                { L_,   "A",            3,      1,              2            },
                { L_,   "AB",           2,      2,              1            },
                { L_,   "ABC",          1,      2,              1            },
                { L_,   "ABCD",         0,      3,              0            },

                { L_,   "",             5,      0,              3            },
                { L_,   "A",            4,      1,              2            },
                { L_,   "AB",           3,      2,              1            },
                { L_,   "ABC",          2,      2,              1            },
                { L_,   "ABCD",         1,      3,              0            },
                { L_,   "ABCDE",        0,      3,              0            },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldDepth = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE         = DATA[ti].d_lineNum;
                const char *const SPEC         = DATA[ti].d_spec_p;
                const int         size         = DATA[ti].d_size;
                const int         firstResize  = DATA[ti].d_firstResize;
                const int         secondResize = DATA[ti].d_secondResize;
                const int curLen   = std::strlen(SPEC);
                const int curDepth = curLen + size;

                Obj mX(ELT_SIZE, &testAllocator);  const Obj& X = mX;
                Obj mY(ELT_SIZE, &testAllocator);  const Obj& Y = mY;

                if (curDepth != oldDepth) {
                    if (verbose) cout << "\ton test vectors of depth "
                                      << curDepth << '.' << endl;
                    LOOP_ASSERT(LINE, oldDepth <= curDepth); // non-decreasing
                    oldDepth = curDepth;
                }

                if (veryVerbose) {
                    cout << "\t\t"; P_(SPEC); P(size);
                    P_(firstResize); P_(secondResize);
                    P_(curLen);      P(curDepth);
                }

                // Create identical objects using the gg function.
                {
                    int blocks1A = testAllocator.numBlocksTotal();
                    int bytes1A = testAllocator.numBytesInUse();

                    gg(&mX, SPEC);
                    validateRBTree(X);

                    int blocks2A = testAllocator.numBlocksTotal();
                    int bytes2A = testAllocator.numBytesInUse();

                    gg(&mY, SPEC);
                    validateRBTree(Y);

                    int blocks3A = testAllocator.numBlocksTotal();
                    int bytes3A = testAllocator.numBytesInUse();

                    int blocks12A = blocks2A - blocks1A;
                    int bytes12A = bytes2A - bytes1A;

                    int blocks23A = blocks3A - blocks2A;
                    int bytes23A = bytes3A - bytes2A;

                    if (veryVerbose) { P_(bytes12A);  P_(bytes23A);
                                       P_(blocks12A); P(blocks23A); }

                    LOOP_ASSERT(LINE, curLen == X.length());
                    LOOP_ASSERT(LINE, curLen == Y.length());

                    LOOP_ASSERT(LINE, firstResize == blocks12A);

                    LOOP_ASSERT(LINE, blocks12A == blocks23A);
                    LOOP_ASSERT(LINE, bytes12A == bytes23A);
                }

                // Apply both functions under test to the respective objects.
                {

                    int blocks1B = testAllocator.numBlocksTotal();
                    int bytes1B = testAllocator.numBytesInUse();

                    stretch(&mX, size);

                    int blocks2B = testAllocator.numBlocksTotal();
                    int bytes2B = testAllocator.numBytesInUse();

                    stretchRemoveAll(&mY, size);

                    int blocks3B = testAllocator.numBlocksTotal();
                    int bytes3B = testAllocator.numBytesInUse();

                    int blocks12B = blocks2B - blocks1B;
                    int bytes12B = bytes2B - bytes1B;

                    int blocks23B = blocks3B - blocks2B;
                    int bytes23B = bytes3B - bytes2B;

                    if (veryVerbose) { P_(bytes12B);  P_(bytes23B);
                                       P_(blocks12B); P(blocks23B); }

                    LOOP_ASSERT(LINE, curDepth == X.length());
                    LOOP_ASSERT(LINE,        0 == Y.length());

                    LOOP_ASSERT(LINE, secondResize == blocks12B);

                    LOOP_ASSERT(LINE, blocks12B == blocks23B); // Always true.

                    LOOP_ASSERT(LINE, bytes12B >= bytes23B);
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
        //
        // Concerns:
        //   The basic concern is that the default constructor, the destructor,
        //   and, under normal conditions, the primary manipulators:
        //      - insertAsFirst      (black-box)
        //      - removeAll          (white-box)
        //   operate as expected.  We have the following specific concerns:
        //    1. The default constructor
        //        a. Creates the correct initial value.
        //        b. Is exception neutral with respect to memory allocation.
        //        c. Has the internal memory management system hooked up
        //            properly so that *all* internally allocated memory
        //            draws from the same user-supplied allocator whenever
        //            one is specified.
        //    2. The destructor properly deallocates all allocated memory to
        //       its corresponding allocator from any attainable state.
        //    3. 'insertAsFirst'
        //        a. Produces the expected value.
        //        b. Increases capacity as needed.
        //        c. Maintains valid internal state.
        //        d. Is exception neutral with respect to memory allocation.
        //    4. 'removeAll'
        //        a. Produces the expected value (empty).
        //        b. Maintains valid internal state.
        //        c. Does not allocate memory.
        //
        // Plan:
        //   To address concerns 1a - 1c, create an object using the default
        //   constructor:
        //    - With and without passing in an allocator.
        //    - In the presence of exceptions during memory allocations using
        //      a 'bdema_TestAllocator' and varying its *allocation* *limit*.
        //    - Where the object is constructed entirely in static memory
        //      (using a 'bdema_BufferAllocator') and never destroyed.
        //
        //   To address concerns 3a - 3c, construct a series of independent
        //   objects, ordered by increasing length.  In each test, allow the
        //   object to leave scope without further modification, so that the
        //   destructor asserts internal object invariants appropriately.
        //   After the final insert operation in each test, use the (untested)
        //   basic accessors to cross-check the value of the object and the
        //   'bdema_TestAllocator' to confirm whether a resize has occurred.
        //
        //   To address concerns 4a-4b, construct a similar test, replacing
        //   'insertAsFirst' with 'removeAll'; this time, however, use the test
        //   allocator to record *numBlocksInUse* rather than *numBlocksTotal*.
        //
        //   To address concerns 2, 3d, 4c, create a small "area" test that
        //   exercises the construction and destruction of objects of various
        //   lengths and capacities in the presence of memory allocation
        //   exceptions.  Two separate tests will be performed.
        //
        //   Let S be the sequence of integers { 0 .. N - 1 }.
        //     (1) for each i in S, use the default constructor and
        //         'insertAsFirst' to create an instance of length i, confirm
        //         its value (using basic accessors), and let it leave scope.
        //     (2) for each (i, j) in S X S, use 'insertAsFirst' to create an
        //         instance of length i, use 'removeAll' to erase its value
        //         and confirm (with 'length'); then use 'insertAsFirst' to
        //         set the instance to a value of length j, verify the value,
        //         and allow the instance to leave scope.
        //
        //   The first test acts as a "control" in that 'removeAll' is not
        //   called; if only the second test produces an error, we know that
        //   'removeAll' is to blame.  We will rely on 'bdema_TestAllocator'
        //   and purify to address concern 2, and on the object invariant
        //   assertions in the destructor to address concerns 3d and 4d.
        //
        //   For an additional simple sanity test, scribble over the extent of
        //   all allocated elements that are returned by 'insertAsFirst'.
        //
        // Testing:
        //   bdeci_RbTreeImp(int nb, bdema_Allocator *ba=0);
        //   ~bdeci_RbTreeImp();
        //   BOOTSTRAP: void *insertAsFirst();
        //   void removeAll();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Primary Manipulators" << endl
                                  << "============================" << endl;

        if (verbose) cout << "\nTesting default ctor (thoroughly)." << endl;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            for (int zi = 0; zi < NUM_NB; ++zi) {
                const Obj X(NB[zi], (bdema_Allocator *)0);
                if (veryVerbose) { cout << "\t\t"; P(X); }
                LOOP_ASSERT(zi, 0 == X.length());
            }
        }
        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            for (int zi = 0; zi < NUM_NB; ++zi) {
                const Obj X(NB[zi], &testAllocator);
                if (veryVerbose) { cout << "\t\t"; P(X); }
                LOOP_ASSERT(zi, 0 == X.length());
            }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            for (int zi = 0; zi < NUM_NB; ++zi) {
              BEGIN_BDEMA_EXCEPTION_TEST {
                if (veryVerbose)
                    cout << "\tTesting Exceptions In Default Ctor" << endl;
                const Obj X(NB[zi], &testAllocator);
                if (veryVerbose) { cout << "\t\t"; P(X); }
                LOOP_ASSERT(zi, 0 == X.length());
              } END_BDEMA_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\tIn place using a buffer allocator." << endl;
        {
            char memory[1024];
            for (int zi = 0; zi < NUM_NB; ++zi) {
                bdema_BufferAllocator a(memory, sizeof memory);
                void *doNotDelete = new(a.allocate(sizeof(Obj)))
                                                               Obj(NB[zi], &a);
                ASSERT(doNotDelete);

                // No destructor is called; will produce memory leak in purify
                // if internal allocators are not hooked up properly.
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'insertAsFirst' (bootstrap)." << endl;
        {
            if (verbose) cout << "\tOn an object of initial length 0." << endl;
            for (int zi = 0; zi < NUM_NB; ++zi) {
                Obj mX(NB[zi], &testAllocator);  const Obj& X = mX;

                const int BB = testAllocator.numBlocksTotal();
                const int B  = testAllocator.numBlocksInUse();
                if (veryVerbose)
                    { cout << "\t\tBEFORE: "; P_(BB); P_(B); P(X); }
                char *node = (char *)mX.insertAsFirst();
                scribble(node, 'A', NB[zi]);
                validateRBTree(X);
                const int AA = testAllocator.numBlocksTotal();
                const int A  = testAllocator.numBlocksInUse();
                if (veryVerbose)
                    { cout << "\t\t AFTER: "; P_(AA); P_(A); P(X); }
                LOOP_ASSERT(zi, BB + 1 == AA);
                LOOP_ASSERT(zi, B  + 1 == A);
                LOOP_ASSERT(zi, 1      == X.length());
                node = (char *)mX.first();
                LOOP_ASSERT(zi, 'A'    == *node);
            }
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            for (int zi = 0; zi < NUM_NB; ++zi) {
                Obj mX(NB[zi], &testAllocator);  const Obj& X = mX;
                char *node = (char *)mX.insertAsFirst();
                scribble(node, 'A', NB[zi]);
                validateRBTree(X);
                LOOP_ASSERT(zi, 1 == X.length());

                const int BB = testAllocator.numBlocksTotal();
                const int B  = testAllocator.numBlocksInUse();
                if (veryVerbose)
                    { cout << "\t\tBEFORE: "; P_(BB); P_(B); P(X); }
                node = (char *)mX.insertAsFirst();
                scribble(node, 'B', NB[zi]);
                validateRBTree(X);
                const int AA = testAllocator.numBlocksTotal();
                const int A  = testAllocator.numBlocksInUse();
                if (veryVerbose)
                    { cout << "\t\t AFTER: "; P_(AA); P_(A); P(X); }
                LOOP_ASSERT(zi, BB + 1 == AA);
                LOOP_ASSERT(zi, B  + 1 == A);
                LOOP_ASSERT(zi, 2      == X.length());

                Iter mI(X.first());  const Iter &I = mI;
                LOOP_ASSERT(zi, I && I() == X.first());
                for (int n = 0; n < 2; ++n) {
                    char *node = (char *)I();
                    LOOP_ASSERT(zi, 'B' - n == *node);
                    mI.moveNext();
                }
                LOOP_ASSERT(zi, !I);
            }
        }
        {
            if (verbose) cout << "\tOn an object of initial length 2." << endl;
            for (int zi = 0; zi < NUM_NB; ++zi) {
                Obj mX(NB[zi], &testAllocator);  const Obj& X = mX;
                char *node = (char *)mX.insertAsFirst();
                scribble(node, 'A', NB[zi]);
                validateRBTree(X);
                LOOP_ASSERT(zi, 1 == X.length());
                node = (char *)mX.insertAsFirst();
                scribble(node, 'B', NB[zi]);
                validateRBTree(X);
                LOOP_ASSERT(zi, 2 == X.length());

                const int BB = testAllocator.numBlocksTotal();
                const int B  = testAllocator.numBlocksInUse();
                if (veryVerbose)
                    { cout << "\t\tBEFORE: "; P_(BB); P_(B); P(X); }
                node = (char *)mX.insertAsFirst();
                scribble(node, 'C', NB[zi]);
                validateRBTree(X);
                const int AA = testAllocator.numBlocksTotal();
                const int A  = testAllocator.numBlocksInUse();
                if (veryVerbose)
                    { cout << "\t\t AFTER: "; P_(AA); P_(A); P(X); }
                LOOP_ASSERT(zi, BB + 0 == AA);
                LOOP_ASSERT(zi, B  + 0 == A);
                LOOP_ASSERT(zi, 3      == X.length());

                Iter mI(X.first());  const Iter &I = mI;
                LOOP_ASSERT(zi, I && I() == X.first());
                for (int n = 0; n < 3; ++n) {
                    char *node = (char *)I();
                    LOOP_ASSERT(zi, 'C' - n == *node);
                    mI.moveNext();
                }
                LOOP_ASSERT(zi, !I);
            }
        }
        {
            if (verbose) cout << "\tOn an object of initial length 3." << endl;
            for (int zi = 0; zi < NUM_NB; ++zi) {
                Obj mX(NB[zi], &testAllocator);  const Obj& X = mX;
                char *node = (char *)mX.insertAsFirst();
                scribble(node, 'A', NB[zi]);
                validateRBTree(X);
                LOOP_ASSERT(zi, 1 == X.length());
                node = (char *)mX.insertAsFirst();
                scribble(node, 'B', NB[zi]);
                validateRBTree(X);
                LOOP_ASSERT(zi, 2 == X.length());
                node = (char *)mX.insertAsFirst();
                scribble(node, 'C', NB[zi]);
                validateRBTree(X);
                LOOP_ASSERT(zi, 3 == X.length());

                const int BB = testAllocator.numBlocksTotal();
                const int B  = testAllocator.numBlocksInUse();
                if (veryVerbose)
                    { cout << "\t\tBEFORE: "; P_(BB); P_(B); P(X); }
                node = (char *)mX.insertAsFirst();
                scribble(node, 'D', NB[zi]);
                validateRBTree(X);
                const int AA = testAllocator.numBlocksTotal();
                const int A  = testAllocator.numBlocksInUse();
                if (veryVerbose)
                    { cout << "\t\t AFTER: "; P_(AA); P_(A); P(X); }
                LOOP_ASSERT(zi, BB + 1 == AA);
                LOOP_ASSERT(zi, B  + 1 == A);
                LOOP_ASSERT(zi, 4      == X.length());

                Iter mI(X.first());  const Iter &I = mI;
                LOOP_ASSERT(zi, I && I() == X.first());
                for (int n = 0; n < 4; ++n) {
                    char *node = (char *)I();
                    LOOP_ASSERT(zi, 'D' - n == *node);
                    mI.moveNext();
                }
                LOOP_ASSERT(zi, !I);
            }
        }
        {
            if (verbose) cout << "\tOn an object of initial length 4." << endl;
            for (int zi = 0; zi < NUM_NB; ++zi) {
                Obj mX(NB[zi], &testAllocator);  const Obj& X = mX;
                char *node = (char *)mX.insertAsFirst();
                scribble(node, 'A', NB[zi]);
                validateRBTree(X);
                LOOP_ASSERT(zi, 1 == X.length());
                node = (char *)mX.insertAsFirst();
                scribble(node, 'B', NB[zi]);
                validateRBTree(X);
                LOOP_ASSERT(zi, 2 == X.length());
                node = (char *)mX.insertAsFirst();
                scribble(node, 'C', NB[zi]);
                validateRBTree(X);
                LOOP_ASSERT(zi, 3 == X.length());
                node = (char *)mX.insertAsFirst();
                scribble(node, 'D', NB[zi]);
                validateRBTree(X);
                LOOP_ASSERT(zi, 4 == X.length());

                const int BB = testAllocator.numBlocksTotal();
                const int B  = testAllocator.numBlocksInUse();
                if (veryVerbose)
                    { cout << "\t\tBEFORE: "; P_(BB); P_(B); P(X); }
                node = (char *)mX.insertAsFirst();
                scribble(node, 'E', NB[zi]);
                validateRBTree(X);
                const int AA = testAllocator.numBlocksTotal();
                const int A  = testAllocator.numBlocksInUse();
                if (veryVerbose)
                    { cout << "\t\t AFTER: "; P_(AA); P_(A); P(X); }
                LOOP_ASSERT(zi, BB + 0 == AA);
                LOOP_ASSERT(zi, B  + 0 == A);
                LOOP_ASSERT(zi, 5      == X.length());

                Iter mI(X.first());  const Iter &I = mI;
                LOOP_ASSERT(zi, I && I() == X.first());
                for (int n = 0; n < 5; ++n) {
                    char *node = (char *)I();
                    LOOP_ASSERT(zi, 'E' - n == *node);
                    mI.moveNext();
                }
                LOOP_ASSERT(zi, !I);
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'removeAll'." << endl;
        {
            if (verbose) cout << "\tOn an object of initial length 0." << endl;
            for (int zi = 0; zi < NUM_NB; ++zi) {
                Obj mX(NB[zi], &testAllocator);  const Obj& X = mX;
                LOOP_ASSERT(zi, 0 == X.length());

                const int B = testAllocator.numBlocksInUse();
                if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
                mX.removeAll();
                const int A = testAllocator.numBlocksInUse();
                if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
                LOOP_ASSERT(zi, B - 0 == A);
                LOOP_ASSERT(zi, 0     == X.length());
            }
        }
        {
            if (verbose) cout << "\tOn an object of initial length 1." << endl;
            for (int zi = 0; zi < NUM_NB; ++zi) {
                Obj mX(NB[zi], &testAllocator);  const Obj& X = mX;
                char *node = (char *)mX.insertAsFirst();
                scribble(node, 'A', NB[zi]);
                validateRBTree(X);
                LOOP_ASSERT(zi, 1 == X.length());

                const int B = testAllocator.numBlocksInUse();
                if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
                mX.removeAll();
                const int A = testAllocator.numBlocksInUse();
                if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
                LOOP_ASSERT(zi, B - 1 == A);
                LOOP_ASSERT(zi, 0     == X.length());
            }
        }
        {
            if (verbose) cout << "\tOn an object of initial length 2." << endl;
            for (int zi = 0; zi < NUM_NB; ++zi) {
                Obj mX(NB[zi], &testAllocator);  const Obj& X = mX;
                char *node = (char *)mX.insertAsFirst();
                scribble(node, 'A', NB[zi]);
                validateRBTree(X);
                LOOP_ASSERT(zi, 1 == X.length());
                node = (char *)mX.insertAsFirst();
                scribble(node, 'B', NB[zi]);
                validateRBTree(X);
                LOOP_ASSERT(zi, 2 == X.length());

                const int B = testAllocator.numBlocksInUse();
                if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
                mX.removeAll();
                const int A = testAllocator.numBlocksInUse();
                if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
                LOOP_ASSERT(zi, B - 2 == A);
                LOOP_ASSERT(zi, 0     == X.length());
            }
        }
        {
            if (verbose) cout << "\tOn an object of initial length 3." << endl;
            for (int zi = 0; zi < NUM_NB; ++zi) {
                Obj mX(NB[zi], &testAllocator);  const Obj& X = mX;
                char *node = (char *)mX.insertAsFirst();
                scribble(node, 'A', NB[zi]);
                validateRBTree(X);
                LOOP_ASSERT(zi, 1 == X.length());
                node = (char *)mX.insertAsFirst();
                scribble(node, 'B', NB[zi]);
                validateRBTree(X);
                LOOP_ASSERT(zi, 2 == X.length());
                node = (char *)mX.insertAsFirst();
                scribble(node, 'C', NB[zi]);
                validateRBTree(X);
                LOOP_ASSERT(zi, 3 == X.length());

                const int B = testAllocator.numBlocksInUse();
                if (veryVerbose) { cout << "\t\tBEFORE: "; P_(B); P(X); }
                mX.removeAll();
                const int A = testAllocator.numBlocksInUse();
                if (veryVerbose) { cout << "\t\t AFTER: "; P_(A); P(X); }
                LOOP_ASSERT(zi, B - 2 == A);
                LOOP_ASSERT(zi, 0     == X.length());
            }
        }

        // --------------------------------------------------------------------

        if (verbose) cout <<
            "\nTesting the destructor and exception neutrality." << endl;

        if (verbose) cout << "\tWith 'insertAsFirst' only" << endl;
        {
            // For each lengths i up to some modest limit:
            //   1. create an instance
            //   2. insertAsFirst up to length i
            //   3. verify initial length and contents
            //   4. allow the instance to leave scope

            const int NUM_TRIALS = 10;
            for (int i = 0; i < NUM_TRIALS; ++i) { // i is the length
                if (verbose) cout << "\t\tOn an object of length " << i << '.'
                                  << endl;

                for (int zi = 0; zi < NUM_NB; ++zi) {
                  BEGIN_BDEMA_EXCEPTION_TEST {
                    int k; // loop index

                    Obj mX(NB[zi], &testAllocator);  const Obj& X = mX;   // 1.
                    for (k = 0; k < i; ++k) {                             // 2.
                        char *node = (char *)mX.insertAsFirst();
                        scribble(node, 'A' + k, NB[zi]);
                        LOOP_ASSERT(zi, k + 1 == X.length());
                    }
                    validateRBTree(X);

                    LOOP_ASSERT(i, i == X.length());                      // 3.
                    Iter mI(X.first());  const Iter &I = mI;
                    if (i) LOOP_ASSERT(zi, I && I() == X.first());
                    for (k = 0; k < i; ++k) {
                        char *node = (char *)I();
                        LOOP_ASSERT(zi, 'A' + i - k -1 == *node);
                        mI.moveNext();
                    }
                    LOOP_ASSERT(zi, !I);

                  } END_BDEMA_EXCEPTION_TEST                              // 4.
                }
            }
        }

        if (verbose) cout << "\tWith 'insertAsFirst' and 'removeAll'" << endl;
        {
            // For each pair of lengths (i, j) up to some modest limit:
            //   1. create an instance
            //   2. insertAsFirst up to a length of i
            //   3. verify initial length and contents
            //   4. removeAll contents from instance
            //   5. verify length is 0
            //   6. insertAsFirst up to length j
            //   7. verify new length and contents
            //   8. allow the instance to leave scope

            const int NUM_TRIALS = 10;
            for (int i = 0; i < NUM_TRIALS; ++i) { // i is first length
                if (verbose) cout << "\t\tOn an object of initial length "
                                  << i << '.' << endl;

                for (int j = 0; j < NUM_TRIALS; ++j) { // j is second length
                    if (veryVerbose) cout << "\t\t\tAnd with final length "
                                          << j << '.' << endl;

                    for (int zi = 0; zi < NUM_NB; ++zi) {
                      BEGIN_BDEMA_EXCEPTION_TEST {
                        int k; // loop index

                        Obj mX(NB[zi], &testAllocator);                   // 1.
                        const Obj& X = mX;
                        for (k = 0; k < i; ++k) {                         // 2.
                            char *node = (char *)mX.insertAsFirst();
                            scribble(node, 'A' + k, NB[zi]);
                            LOOP_ASSERT(zi, k + 1 == X.length());
                        }
                        validateRBTree(X);

                        LOOP3_ASSERT(i, j, zi, i == X.length());          // 3.
                        Iter mI(X.first());  const Iter &I = mI;
                        if (i) LOOP_ASSERT(zi, I && I() == X.first());
                        for (k = 0; k < i; ++k) {
                            char *node = (char *)I();
                            LOOP_ASSERT(zi, 'A' + i - k -1 == *node);
                            mI.moveNext();
                        }
                        LOOP_ASSERT(zi, !I);

                        mX.removeAll();                                   // 4.
                        LOOP3_ASSERT(i, j, zi, 0 == X.length());          // 5.

                        for (k = 0; k < j; ++k) {                         // 6.
                            char *node = (char *)mX.insertAsFirst();
                            scribble(node, 'A' + k, NB[zi]);
                            LOOP_ASSERT(zi, k + 1 == X.length());
                        }
                        validateRBTree(X);

                        LOOP3_ASSERT(i, j, zi, j == X.length());          // 7.
                        Iter mJ(X.first());  const Iter &J = mJ;
                        if (j) LOOP_ASSERT(zi, J && J() == X.first());
                        for (k = 0; k < j; ++k) {
                            char *node = (char *)J();
                            LOOP_ASSERT(zi, 'A' + j - k -1 == *node);
                            mJ.moveNext();
                        }
                        LOOP_ASSERT(zi, !J);

                      } END_BDEMA_EXCEPTION_TEST                          // 8.
                    }
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST: 'bdeci_RbTreeImp' AND 'bdeci_RbTreeImpIter'
        //
        // Concerns:
        //   We want to exercise basic functionality.  In particular, we want
        //   to demonstrate a base-line level of correct operation of:
        //    1. the following methods and operators of 'bdeci_RbTreeImp':
        //       - constructor and destructor
        //       - primary manipulators: 'insertAfter', 'insertAsFirst',
        //                               'remove' and 'removeAll'
        //       - direct accessors: 'root', 'first' and 'length'
        //       - output operator: 'operator<<'
        //
        //    2. the following methods and operators of 'bdeci_RbTreeImpIter':
        //       - constructors and destructor
        //       - primary manipulators: 'moveLeft', 'moveRight' and 'moveNext'
        //       - assignment operator: 'operator='
        //       - direct accessors: 'hasLeft', 'hasRight', 'isBlack', 'isRed',
        //                           'operator const void *' and
        //                           'const void *operator()'
        //
        // Plan:
        //   Create a red-black tree for 1-byte sized elements.  Insert a small
        //   number of elements into the tree, then remove the elements from
        //   the tree.  At each stage, assert the expected state of the tree
        //   and output the tree to a buffer to compare it with the expected
        //   format.  Also regularly attach iterators to the tree, walk
        //   the tree via the iterators, and assert the expected state of the
        //   nodes of the tree during the iteration.  Display the value of the
        //   tree frequently in verbose mode:
        //
        //    1.   Create an (empty) r-b tree t (element size = 1).
        //      a. Check initial state of t.
        //      b. Print t to a buffer (operator<<).
        //    2.   Create an r-b tree iterator i1 (default ctor).
        //    3.   Insert (as first) an element value A into t.
        //      a. Check new state of t.
        //      b. Print t to a buffer.
        //    4.   Create an r-b tree iterator i2(t).
        //      a. Iterate over t via i2.
        //    5.   Insert (as first) a second element value B into t.
        //      a. Check new state of t.
        //      b. Print t to a buffer.
        //    6.   Create an r-b tree iterator i3(t.root).
        //      a. Assign i3 to i1.
        //      b. Iterate over t via i1.
        //    7.   Insert (after A) a third element value C into t.
        //      a. Check new state of t.
        //      b. Print t to a buffer.
        //    8.   Create an r-b tree iterator i4(t.first).
        //      a. Iterate over t via i4.
        //    9.   Remove element A from t.
        //      a. Check new state of t.
        //      b. Print t to a buffer.
        //   10.   Create an r-b tree iterator i5(t).
        //      a. Iterate over t via i5.
        //   11.   Remove remaining elements from t.
        //      a. Check new state of t.
        //      b. Print t to a buffer.
        //   12.   Let t and i[1-5] go out of scope.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Breathing Test"
                          << endl << "==============" << endl;

        const int ELT_SIZE = 1;
        char *rTag, *fTag;

        const int SIZE = 256;
        char buf[SIZE];
        ostrstream out(buf, SIZE);

        //  1. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n  1. Create an (empty) r-b tree t "
                             "(element size = 1)." << endl;
        Obj mX(ELT_SIZE, &testAllocator);  const Obj& X = mX;
        if (verbose) { TAB; P(X); }

        if (verbose) cout << "\ta. Check initial state of t." << endl;
        ASSERT(0 == X.length());
        ASSERT(0 == X.root());
        ASSERT(0 == X.first());

        if (verbose) cout << "\tb. Print t to a buffer (operator<<)." << endl;
        const char *TREE1 = "()";
        if (veryVerbose) cout << "\tEXPECTED TREE: " << TREE1 << endl;
        out << X << ends;
        if (veryVerbose) cout << "\tACTUAL TREE:   " << buf   << endl;
        ASSERT(0 == strcmp(buf, TREE1));
        //  2. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n  2. Create an r-b tree iterator i1 "
                             "(default ctor)." << endl;
        Iter mI1;  const Iter &I1 = mI1;                  ASSERT(!I1);

        //  3. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n  3. Insert (as first) an element value A "
                             "into t." << endl;
        char *first = (char *)mX.insertAsFirst();         ASSERT(first);
        *first = 'A';
        validateRBTree(X);
        if (verbose) { TAB; P(X); }

        if (verbose) cout << "\ta. Check new state of t." << endl;
        ASSERT(1 == X.length());
        ASSERT(X.root());    rTag = (char *)X.root();     ASSERT('A' == *rTag);
        ASSERT(X.first());   fTag = (char *)X.first();    ASSERT('A' == *fTag);
        if (verbose) { TAB; P_(rTag); P(fTag); }

        if (verbose) cout << "\tb. Print t to a buffer." << endl;
        const char *TREE2 = "(b () ())";
        if (veryVerbose) cout << "\tEXPECTED TREE: " << TREE2 << endl;
        out.seekp(0);
        out << X << ends;
        if (veryVerbose) cout << "\tACTUAL TREE:   " << buf   << endl;
        ASSERT(0 == strcmp(buf, TREE2));

        //  4. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n  4. Create an r-b tree iterator i2(t)."
                          << endl;
        Iter mI2(X);  const Iter &I2 = mI2;  ASSERT(I2);  ASSERT(I2());

        if (verbose) cout << "\ta. Iterate over t via i2." << endl;
        ASSERT( 1  == I2.isBlack());
        ASSERT( 0  == I2.isRed());
        ASSERT( 0  == I2.hasLeft());
        ASSERT( 0  == I2.hasRight());
        ASSERT('A' == *(char *)I2());

        mI2.moveLeft();                                   ASSERT(!I2);

        //  5. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n  5. Insert (as first) a second element value "
                             "B into t." << endl;
        char *second = (char *)mX.insertAsFirst();        ASSERT(second);
        *second = 'B';
        validateRBTree(X);
        if (verbose) { TAB; P(X); }

        if (verbose) cout << "\ta. Check new state of t." << endl;
        ASSERT(2 == X.length());
        ASSERT(X.root());    rTag = (char *)X.root();     ASSERT('A' == *rTag);
        ASSERT(X.first());   fTag = (char *)X.first();    ASSERT('B' == *fTag);
        if (verbose) { TAB; P_(rTag); P(fTag); }

        if (verbose) cout << "\tb. Print t to a buffer." << endl;
        const char *TREE3 = "(b (r () ()) ())";
        if (veryVerbose) cout << "\tEXPECTED TREE: " << TREE3 << endl;
        out.seekp(0);
        out << X << ends;
        if (veryVerbose) cout << "\tACTUAL TREE:   " << buf   << endl;
        ASSERT(0 == strcmp(buf, TREE3));

        //  6. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n  6. Create an r-b tree iterator i3(t.root)."
                          << endl;
        Iter mI3(X.root());  const Iter &I3 = mI3;
        ASSERT(I3);
        ASSERT(I3());

        if (verbose) cout << "\ta. Assign i3 to i1." << endl;
        mI1 = I3;
        ASSERT(I1);
        ASSERT(I1());

        if (verbose) cout << "\tb. Iterate over t via i1." << endl;
        ASSERT( 1  == I1.isBlack());
        ASSERT( 0  == I1.isRed());
        ASSERT( 1  == I1.hasLeft());
        ASSERT( 0  == I1.hasRight());
        ASSERT('A' == *(char *)I1());

        mI1.moveLeft();
        ASSERT(       I1);
        ASSERT(       I1());
        ASSERT( 0  == I1.isBlack());
        ASSERT( 1  == I1.isRed());
        ASSERT( 0  == I1.hasLeft());
        ASSERT( 0  == I1.hasRight());
        ASSERT('B' == *(char *)I1());

        mI1.moveRight();
        ASSERT(!I1);

        //  7. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n  7. Insert (after A) a third element value C "
                             "into t." << endl;
        char *third = (char *)mX.insertAfter(first);     ASSERT(third);
        *third = 'C';
        validateRBTree(X);
        if (verbose) { TAB; P(X); }

        if (verbose) cout << "\ta. Check new state of t." << endl;
        ASSERT(3 == X.length());
        ASSERT(X.root());    rTag = (char *)X.root();    ASSERT('A' == *rTag);
        ASSERT(X.first());   fTag = (char *)X.first();   ASSERT('B' == *fTag);
        if (verbose) { TAB; P_(rTag); P(fTag); }

        if (verbose) cout << "\tb. Print t to a buffer." << endl;
        const char *TREE4 = "(b (r () ()) (r () ()))";
        if (veryVerbose) cout << "\tEXPECTED TREE: " << TREE4 << endl;
        out.seekp(0);
        out << X << ends;
        if (veryVerbose) cout << "\tACTUAL TREE:   " << buf   << endl;
        ASSERT(0 == strcmp(buf, TREE4));

        //  8. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n  8. Create an r-b tree iterator i4(t.first)."
                          << endl;
        Iter mI4(X.first());  const Iter &I4 = mI4;
        ASSERT(I4);
        ASSERT(I4());

        if (verbose) cout << "\ta. Iterate over t via i4." << endl;
        ASSERT( 0  == I4.isBlack());
        ASSERT( 1  == I4.isRed());
        ASSERT( 0  == I4.hasLeft());
        ASSERT( 0  == I4.hasRight());
        ASSERT('B' == *(char *)I4());

        mI4.moveNext();
        ASSERT(       I4);
        ASSERT(       I4());
        ASSERT( 1  == I4.isBlack());
        ASSERT( 0  == I4.isRed());
        ASSERT( 1  == I4.hasLeft());
        ASSERT( 1  == I4.hasRight());
        ASSERT('A' == *(char *)I4());

        mI4.moveNext();
        ASSERT(       I4);
        ASSERT(       I4());
        ASSERT( 0  == I4.isBlack());
        ASSERT( 1  == I4.isRed());
        ASSERT( 0  == I4.hasLeft());
        ASSERT( 0  == I4.hasRight());
        ASSERT('C' == *(char *)I4());

        mI4.moveNext();
        ASSERT(!I4);

        //  9. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n  9. Remove element A from t." << endl;
        mX.remove(first);
        validateRBTree(X);
        if (verbose) { TAB; P(X); }

        if (verbose) cout << "\ta. Check new state of t." << endl;
        ASSERT(2 == X.length());
        ASSERT(X.root());    rTag = (char *)X.root();    ASSERT('C' == *rTag);
        ASSERT(X.first());   fTag = (char *)X.first();   ASSERT('B' == *fTag);

        if (verbose) cout << "\tb. Print t to a buffer." << endl;
        const char *TREE5 = "(b (r () ()) ())";
        if (veryVerbose) cout << "\tEXPECTED TREE: " << TREE5 << endl;
        out.seekp(0);
        out << X << ends;
        if (veryVerbose) cout << "\tACTUAL TREE:   " << buf   << endl;
        ASSERT(0 == strcmp(buf, TREE5));

        // 10. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 10. Create an r-b tree iterator i5(t)."
                          << endl;
        Iter mI5(X);  const Iter &I5 = mI5;
        ASSERT(I5);
        ASSERT(I5());

        if (verbose) cout << "\ta. Iterate over t via i5." << endl;
        ASSERT( 1  == I5.isBlack());
        ASSERT( 0  == I5.isRed());
        ASSERT( 1  == I5.hasLeft());
        ASSERT( 0  == I5.hasRight());
        ASSERT('C' == *(char *)I5());

        mI5.moveLeft();
        ASSERT(       I5);
        ASSERT(       I5());
        ASSERT( 0  == I5.isBlack());
        ASSERT( 1  == I5.isRed());
        ASSERT( 0  == I5.hasLeft());
        ASSERT( 0  == I5.hasRight());
        ASSERT('B' == *(char *)I5());

        mI5.moveRight();
        ASSERT(!I5);

        // 11. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 11. Remove remaining elements from t."
                          << endl;
        mX.removeAll();
        if (verbose) { TAB; P(X); }

        if (verbose) cout << "\ta. Check new state of t." << endl;
        ASSERT(0 == X.length());
        ASSERT(0 == X.first());
        ASSERT(0 == X.root());

        if (verbose) cout << "\tb. Print t to a buffer." << endl;
        if (veryVerbose) cout << "\tEXPECTED TREE: " << TREE1 << endl;
        out.seekp(0);
        out << X << ends;
        if (veryVerbose) cout << "\tACTUAL TREE:   " << buf   << endl;
        ASSERT(0 == strcmp(buf, TREE1));

        // 12. - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 12. Let t and i[1-5] go out of scope."
                          << endl;
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
