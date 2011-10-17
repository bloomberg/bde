// bslalg_rbtreeprimitives.cpp                                        -*-C++-*-
#include <bslalg_rbtreeprimitives.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_assert.h>

#include <stdlib.h>
#include <stdio.h>

namespace BloombergLP {
namespace bslalg {

static const RbTreeNode::Color RED   = RbTreeNode::BSLALG_RED;
static const RbTreeNode::Color BLACK = RbTreeNode::BSLALG_BLACK;

#if 0
// TBD: Development purposes.  Remove.

void debugPrintNodeValue(FILE *file, const RbTreeNode *node)
{
    fprintf(file, "%d", node->d_value);
}

void debugPrintTree(FILE *file, const RbTreeNode *root)
{
    RbTreePrimitives::printTree(file, root, debugPrintNodeValue, 0, 3);
}

#define DEBUG_PRINT_TREE(tree) debugPrintTree(stdout, tree);
#define DEBUG_PRINT_NODE(node)                                                \
        printf(#node ": ");                                                   \
        if (node) debugPrintNodeValue(stdout, node); else printf("nil");      \
        printf("\n");
#define DEBUG_PRINT(message) printf(message);
#else
#define DEBUG_PRINT_TREE(tree) 
#define DEBUG_PRINT_NODE(node)
#define DEBUG_PRINT(message)
#endif

static void indent(FILE *file, int level, int spacesPerLevel)
   // Emit, to the specified 'file', the number of spaces specified by
   // absolute value of the product of 'level' and 'spacesPerLevel' to the
   // specified output 'stream' and return a reference to the modifiable
   // 'stream'.  If the 'level' is negative, this function has no effect.
{
    if (level < 0) { 
        return;                                                       // RETURN
    }
    int numSpaces = abs(level * spacesPerLevel);

    static const char SPACES[]    = "                                        ";
           const int  SPACES_SIZE = sizeof(SPACES) - 1;

    while (SPACES_SIZE < numSpaces) {
        fwrite(SPACES, 1, SPACES_SIZE, file);
        numSpaces -= SPACES_SIZE;
    }

    if (0 < numSpaces) {
        fwrite(SPACES, 1, numSpaces, file);
    }
}

static 
RbTreeNode *recolorTreeAfterRemoval(RbTreeNode *rootNode,
                                    RbTreeNode *node,
                                    RbTreeNode *parentOfNode)
     // Rebalance the nodes in the tree rooted at the specified 'rootNode',
     // which has been potentially unbalanced by the insertion of the the
     // specified 'node' as the child of the specified 'parentOfNode'.  The
     // behavior is undefined unless 'rootNode' refers to a valid binary
     // search tree (but not necessarily a valid red-black tree), and
     // 'rootNode' would be a valid red-black tree if 'node' were removed
     // (i.e., the potential violation of red-black constraints is localized
     // to 'node').
{
    // Implementation Note:  The following is adapted with few changes from
    // "Intoduction to Algorithms" [Cormen, Leiserson, Rivest] , except that
    // explicit conditions are required to treat NULL values as Black, and
    // consequently to avoid setting a color (BLACK) on a (already BLACK) null
    // node.

    typedef RbTreePrimitives Op;
    while (node != rootNode && (0 == node || BLACK == node->color())) {
        DEBUG_PRINT("-----------\n");
        DEBUG_PRINT_NODE(node);
        DEBUG_PRINT_NODE(parentOfNode);
        DEBUG_PRINT_TREE(rootNode);
        if (node == parentOfNode->leftChild()) {
            RbTreeNode *uncle = parentOfNode->rightChild();
            if (RED == uncle->color()) {
                // Case 1.
                uncle->setColor(BLACK);
                parentOfNode->setColor(RED);
                rootNode = Op::rotateLeft(rootNode, parentOfNode);
                uncle = parentOfNode->rightChild();
            }

            if ((0 == uncle->leftChild()  || 
                                     BLACK == uncle->leftChild()->color() ) &&
                (0 == uncle->rightChild() || 
                                     BLACK == uncle->rightChild()->color())) {
                // Case 2.
                uncle->setColor(RED);
                node           = parentOfNode;
                parentOfNode   = node->parent();
            }
            else {
                if (0     == uncle->rightChild() || 
                    BLACK == uncle->rightChild()->color()) {
                    // Case 3.
                    if (uncle->leftChild()) {
                        uncle->leftChild()->setColor(BLACK);
                    }
                    uncle->setColor(RED);
                    rootNode = Op::rotateRight(rootNode, uncle);
                    uncle    = parentOfNode->rightChild();
                }
                // Case 4.
                uncle->setColor(parentOfNode->color());
                parentOfNode->setColor(BLACK);
                if (uncle->rightChild()) {
                    uncle->rightChild()->setColor(BLACK);
                }
                rootNode = Op::rotateLeft(rootNode, parentOfNode);
                break;
            }
        }
        else {
            RbTreeNode *uncle = parentOfNode->leftChild();
            if (RED == uncle->color()) {
                // Case 1.
                uncle->setColor(BLACK);
                parentOfNode->setColor(RED);
                rootNode = Op::rotateRight(rootNode, parentOfNode);
                uncle = parentOfNode->leftChild();
            }
            if ((0 == uncle->rightChild() ||
                                   BLACK == uncle->rightChild()->color() ) &&
                (0 == uncle->leftChild()  || 
                                   BLACK == uncle->leftChild()->color()  )) {
                // Case 2.
                uncle->setColor(RED);
                node           = parentOfNode;
                parentOfNode   = node->parent();
            }
            else {
                if (0     == uncle->leftChild() || 
                    BLACK == uncle->leftChild()->color()) {
                    // Case 3.
                    if (uncle->rightChild()) {
                        uncle->rightChild()->setColor(BLACK);
                    }
                    uncle->setColor(RED);
                    rootNode = Op::rotateLeft(rootNode, uncle);
                    uncle    = parentOfNode->leftChild();
                }
                // Case 4.
                uncle->setColor(parentOfNode->color());
                parentOfNode->setColor(BLACK);
                if (uncle->leftChild()) {
                    uncle->leftChild()->setColor(BLACK);
                }
                rootNode = Op::rotateRight(rootNode, parentOfNode);
                break;
            }
        }
    }
    if (node) {
        node->setColor(BLACK);
    }
    DEBUG_PRINT_TREE(rootNode);
    DEBUG_PRINT("Done.\n");
    return rootNode;
}

                        // ----------------------
                        // class RbTreePrimitives
                        // ----------------------
// CLASS METHODS
const RbTreeNode *RbTreePrimitives::minimum(const RbTreeNode *node)
{
    BSLS_ASSERT(node);
    while (node->leftChild()) {
        node = node->leftChild();
    }
    return node;
}


const RbTreeNode *RbTreePrimitives::maximum(const RbTreeNode *node)
{
    BSLS_ASSERT(node);
    while (node->rightChild()) {
        node = node->rightChild();
    }
    return node;
}

const RbTreeNode *RbTreePrimitives::next(const RbTreeNode *node) 
{
    BSLS_ASSERT(node);
    if (node->rightChild()) {
        return minimum(node->rightChild());
    }
    const RbTreeNode *parent = node->parent();
    while (parent) {
        if (parent->leftChild() == node) {
            return parent;
        }
        node   = parent;
        parent = node->parent();
    }
    return 0;
}

const RbTreeNode *RbTreePrimitives::previous(const RbTreeNode *node) 
{
    BSLS_ASSERT(node);
    if (node->leftChild()) {
        return maximum(node->leftChild());
    }
    const RbTreeNode *parent = node->parent();
    while (parent) {
        if (parent->rightChild() == node) {
            return parent;
        }
        node   = parent;
        parent = node->parent();
    }
    return 0;
}

RbTreeNode *RbTreePrimitives::insertAt(RbTreeNode *rootNode, 
                                       RbTreeNode *parentNode,
                                       bool        leftChild,
                                       RbTreeNode *newNode)
{
    const RbTreeNode::Color RED   = RbTreeNode::BSLALG_RED;
    const RbTreeNode::Color BLACK = RbTreeNode::BSLALG_BLACK;

    newNode->setLeftChild(0);
    newNode->setRightChild(0);

    if (0 == parentNode) {
        // The tree is empty, make 'newNode' the new root.

        BSLS_ASSERT(0 == rootNode);

        newNode->setParent(0);
        newNode->setColor(BLACK);
        return newNode;
    }

    // Insert the node as a leaf in the tree, and assign its parent
    // pointer. 

    newNode->setColor(RED);
    newNode->setParent(parentNode);
    if (leftChild) {
        parentNode->setLeftChild(newNode);
    }
    else {
        parentNode->setRightChild(newNode);
    }

    // Fix the tree coloring (if necessary).
    // Implementation Note:  The following is adapted with few changes from
    // "Intoduction to Algorithms" [Cormen, Leiserson, Rivest] , except that
    // explicit conditions are required to treat NULL values as Black, and
    // consequently to avoid setting a color (BLACK) on a (already BLACK) null
    // node.

    RbTreeNode *node = newNode;
    while (node != rootNode && node->parent()->color() == RED) {
        if (isLeftChild(node->parent())) {
            RbTreeNode *greatUncle = node->parent()->parent()->rightChild();
            // Test if 'greatUncle' is BLACK (0 is considered BLACK)

            if (greatUncle && RED == greatUncle->color()) {                
                // Case 1:  grandParent[node] ->  (X:B)
                //                               /    \
                //           parent[node]- > (Y:R)     (greatUncle:R)
                //                          /
                //                      (node:R)

                node->parent()->parent()->setColor(RED);
                node->parent()->setColor(BLACK);
                greatUncle->setColor(BLACK);

                node = node->parent()->parent();
            }
            else {
                if (isRightChild(node)) {
                    // Case 2:  grandParent[node] -> (X:B)
                    //                              /     \
                    //           parent[node]- > (Y:R)    (greatUncle:B)
                    //                             \
                    //                              (node:R)
                    //
                    // Perform a left rotation on parent[node] to reduce to
                    // case 3.

                    node = node->parent();
                    rootNode = rotateLeft(rootNode, node);
                }

                // Case 3:  grandParent[node] -> (X:B)
                //                              /     \
                //           parent[node]- > (Y:R)    (greatUncle:B)
                //                            /
                //                        (node:R)
                //
                // Recolor the parent black, the grand parent-red, then rotate
                // the grand-parent right, so that its the new root of the
                // sub-tree. 

                node->parent()->setColor(BLACK);
                node->parent()->parent()->setColor(RED);
                rootNode = rotateRight(rootNode, node->parent()->parent());
            }                
        }
        else { 
            // The following mirrors the cases above, but with right and left
            // exchanged. 
            RbTreeNode *greatUncle = node->parent()->parent()->leftChild();
            if (greatUncle && RED == greatUncle->color()) {                
                node->parent()->parent()->setColor(RED);
                node->parent()->setColor(BLACK);
                greatUncle->setColor(BLACK);

                node = node->parent()->parent();
            }
            else {
                if (isLeftChild(node)) {
                    node = node->parent();
                    rootNode = rotateRight(rootNode, node);
                    
                }
                node->parent()->setColor(BLACK);
                node->parent()->parent()->setColor(RED);
                rootNode = rotateLeft(rootNode, node->parent()->parent());
            }             
        }
    }
    rootNode->setColor(BLACK);
    return rootNode;
}

RbTreeNode *RbTreePrimitives::remove(RbTreeNode *rootNode, RbTreeNode *node)
{
    BSLS_ASSERT(0 != node);
    BSLS_ASSERT(0 != rootNode);

    RbTreeNode        *x, *y;
    RbTreeNode        *parentOfX;
    RbTreeNode::Color  yColor;   

    // Implementation Note:  This implementation has been adjusted from the
    // one described in "Intoduction to Algorithms" [Cormen, Leiserson,
    // Rivest] (i.e., CLR) to avoid swapping node values (swapping nodes is
    // potentially inefficient and inappropriate for an STL map).
    // Specifically, int case where 'node' has two (non-null) children, CLR
    // (confusingly) swaps the value of the node with its replacement; instead
    // we move node's successor to the position of node, and then recolor its
    // value with the same result).

    // Case 1: If either child of the node being removed is 0, then 'node' can
    // be replaced by its non-null child (or by a null child if 'node' has no
    // children).
    if (0 == node->leftChild()) {
        y = node;
        x = node->rightChild();   
    }
    else if (0 == node->rightChild()) {
        y = node;
        x = node->leftChild();
    }
    else {
        // Case 2: Otherwise the 'node' will be replaced by its successor in
        // the tree. 

        y = minimum(node->rightChild());
        x = y->rightChild();
    }
    yColor = y->color();

    if (y == node) {
        // We should be in case 1, where 'node' has (at least 1) null child,
        // and will simply be replaced by one of its children.  In this
        // context, 'x' refers to the node that will replace 'node'.  Simply
        // point the parent of 'node' to its new child, 'x'.

        BSLS_ASSERT_SAFE(0 == node->leftChild() || 0 == node->rightChild());
        if (0 == node->parent()) {
            rootNode = x;
        }
        else if (isLeftChild(node)) {
            node->parent()->setLeftChild(x);
        }
        else {
            node->parent()->setRightChild(x);
        }                

        parentOfX = node->parent();
        if (x) {
            x->setParent(node->parent());
        }

    }
    else {
        // We should be in case 2, where 'node' has two non-null children.  In
        // this context 'y' is the successor to 'node' which will be used to
        // replace 'node'.

        BSLS_ASSERT_SAFE(0 != node->leftChild() && 0 != node->rightChild());
        BSLS_ASSERT_SAFE(0 == y->leftChild());
        BSLS_ASSERT_SAFE(x == y->rightChild());

        if (0 == node->parent()) {
            rootNode = y;
        }
        else if (isLeftChild(node)) {
            node->parent()->setLeftChild(y);
        }
        else {
            node->parent()->setRightChild(y);
        }        
        y->setLeftChild(node->leftChild()); 
        y->leftChild()->setParent(y);

        if (y->parent() != node) {
            // The following logic only applies if the replacement node 'y' is
            // not a direct descendent of the 'node' being replaced, otherwise
            // it is a degenerate case.

            BSLS_ASSERT_SAFE(y->parent()->leftChild() == y);           
            
            parentOfX = y->parent();
            y->parent()->setLeftChild(x);  // 'x' is y->rightChild()
            if (x) {
                x->setParent(y->parent());
            }

            y->setRightChild(node->rightChild());
            y->rightChild()->setParent(y);
        }  
        else {
            parentOfX = y;
        }
        y->setParent(node->parent()); 
        y->setColor(node->color());
    }

    if (yColor == BLACK) {
        DEBUG_PRINT_TREE(rootNode);
        DEBUG_PRINT_NODE(x);
        DEBUG_PRINT_NODE(parentOfX); 
        rootNode = recolorTreeAfterRemoval(rootNode, x, parentOfX); 
    }
    return rootNode;
}

void RbTreePrimitives::printTree(FILE                   *file,
                                 const RbTreeNode       *node,
                                 printNodeValueCallback  printValueCallback,
                                 int                     level,
                                 int                     spacesPerLevel)
{
    // TBD: Rewrite iteratively(?)

    indent(file, level, spacesPerLevel);
    fprintf(file, "[");    
    char SEP = (spacesPerLevel < 0) ? ' ' : '\n';
   
    if (node) {
        fprintf(file, " ");
        printValueCallback(file, node);
        fprintf(file,
                " {%s}%c", 
                node->color() == RED ? "RED" : "BLACK",
                SEP);
        if (node->leftChild()) {
            indent(file, level, spacesPerLevel);
            fprintf(file, "left:\n");
            printTree(file, 
                      node->leftChild(), 
                      printValueCallback, 
                      level+1,
                      spacesPerLevel);
        }
        if (node->rightChild()) {
            indent(file, level, spacesPerLevel);
            fprintf(file, "right:\n");
            printTree(file, 
                      node->rightChild(), 
                      printValueCallback, 
                      level+1,
                      spacesPerLevel);
        }
    }   
    indent(file, level, spacesPerLevel);
    fprintf(file, "]%c", SEP);
}
     
RbTreeNode *RbTreePrimitives::rotateLeft(RbTreeNode *rootNode,
                                         RbTreeNode *node)
{
    RbTreeNode *pivot = node->rightChild();
    BSLS_ASSERT(pivot);

    pivot->setParent(node->parent());
    node->setRightChild(pivot->leftChild());
    if (node->rightChild()) {
        node->rightChild()->setParent(node);
    }
    pivot->setLeftChild(node);
    node->setParent(pivot);

    if (!pivot->parent()) {
        return pivot;                                                 // RETURN
    }
    if (pivot->parent()->leftChild() == node) {
        pivot->parent()->setLeftChild(pivot);
    }
    else {
        pivot->parent()->setRightChild(pivot);
    }
    return rootNode;
}

RbTreeNode *RbTreePrimitives::rotateRight(RbTreeNode *rootNode,
                                          RbTreeNode *node)
{
    RbTreeNode *pivot = node->leftChild();
    BSLS_ASSERT(pivot);

    pivot->setParent(node->parent());
    node->setLeftChild(pivot->rightChild());
    if (node->leftChild()) {
        node->leftChild()->setParent(node);
    }
    pivot->setRightChild(node);
    node->setParent(pivot);

    if (!pivot->parent()) {
        return pivot;                                                 // RETURN
    }
    if (pivot->parent()->leftChild() == node) {
        pivot->parent()->setLeftChild(pivot);
    }
    else {
        pivot->parent()->setRightChild(pivot);
    }
    return rootNode;
}

}  // close namespace bslalg
}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
