// bslalg_rbtreeutil.cpp                                              -*-C++-*-
#include <bslalg_rbtreeutil.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bslalg_arraydestructionprimitives.h>   // for testing purposes only
#include <bslalg_arrayprimitives.h>              // for testing purposes only
#include <bslalg_rangecompare.h>                 // for testing purposes only

#include <bsls_assert.h>

#include <stdlib.h>
#include <stdio.h>

namespace BloombergLP {
namespace bslalg {

static inline bool isBlackOrNull(const RbTreeNode *node)
    // Return 'true' if 'node' is 0 or colored black, and 'false' otherwise.
{
    return !node || node->isBlack();
}

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

static void recolorTreeAfterRemoval(RbTreeAnchor *tree,
                                    RbTreeNode   *node,
                                    RbTreeNode   *parentOfNode)
     // Rebalance the nodes in the specified 'tree', which has been
     // potentially unbalanced by the insertion of the the specified 'node' as
     // the child of the specified 'parentOfNode'.  The behavior is undefined
     // unless 'tree' refers to a valid binary search tree (but not
     // necessarily a valid red-black tree), and 'tree' would be a valid
     // red-black tree if 'node' were removed (i.e., the potential violation
     // of red-black constraints is localized to 'node').
{
    // Implementation Note:  The following is adapted with few changes from
    // "Introduction to Algorithms" [Cormen, Leiserson, Rivest] , except that
    // explicit conditions are required to treat NULL values as Black, and
    // consequently to avoid setting a color (BLACK) on a (already BLACK) null
    // node.

    typedef RbTreeUtil Op;
    while (node != tree->rootNode() && (0 == node || node->isBlack())) {
        if (node == parentOfNode->leftChild()) {
            RbTreeNode *sibling = parentOfNode->rightChild();
            if (sibling->isRed()) {
                // Case 1.

                sibling->makeBlack();
                parentOfNode->makeRed();
                Op::rotateLeft(parentOfNode);
                sibling = parentOfNode->rightChild();
            }

            if (isBlackOrNull(sibling->leftChild()) &&
                isBlackOrNull(sibling->rightChild())) {
                // Case 2.

                sibling->makeRed();
                node         = parentOfNode;
                parentOfNode = node->parent();
            }
            else {
                if (isBlackOrNull(sibling->rightChild())) {
                    // Case 3.

                    if (sibling->leftChild()) {
                        sibling->leftChild()->makeBlack();
                    }
                    sibling->makeRed();
                    Op::rotateRight(sibling);
                    sibling = parentOfNode->rightChild();
                }
                // Case 4.

                sibling->setColor(parentOfNode->color());
                parentOfNode->makeBlack();
                if (sibling->rightChild()) {
                    sibling->rightChild()->makeBlack();
                }
                Op::rotateLeft(parentOfNode);
                break;
            }
        }
        else {
            RbTreeNode *sibling = parentOfNode->leftChild();
            if (sibling->isRed()) {
                // Case 1.

                sibling->makeBlack();
                parentOfNode->makeRed();
                Op::rotateRight(parentOfNode);
                sibling = parentOfNode->leftChild();
            }

            if (isBlackOrNull(sibling->rightChild()) &&
                isBlackOrNull(sibling->leftChild())) {
                // Case 2.

                sibling->makeRed();
                node         = parentOfNode;
                parentOfNode = node->parent();
            }
            else {
                if (isBlackOrNull(sibling->leftChild())) {
                    // Case 3.

                    if (sibling->rightChild()) {
                        sibling->rightChild()->makeBlack();
                    }
                    sibling->makeRed();
                    Op::rotateLeft(sibling);
                    sibling = parentOfNode->leftChild();
                }
                // Case 4.

                sibling->setColor(parentOfNode->color());
                parentOfNode->makeBlack();
                if (sibling->leftChild()) {
                    sibling->leftChild()->makeBlack();
                }
                Op::rotateRight(parentOfNode);
                break;
            }
        }
    }
    if (node) {
        node->makeBlack();
    }
}

                        // ----------------
                        // class RbTreeUtil
                        // ----------------
// CLASS METHODS
const RbTreeNode *RbTreeUtil::leftmost(const RbTreeNode *subtree)
{
    BSLS_ASSERT(subtree);

    while (subtree->leftChild()) {
        subtree = subtree->leftChild();
    }
    return subtree;
}


const RbTreeNode *RbTreeUtil::rightmost(const RbTreeNode *subtree)
{
    BSLS_ASSERT(subtree);

    while (subtree->rightChild()) {
        subtree = subtree->rightChild();
    }
    return subtree;
}

const RbTreeNode *RbTreeUtil::next(const RbTreeNode *node)
{
    BSLS_ASSERT(node);

    if (node->rightChild()) {
        return leftmost(node->rightChild());                          // RETURN
    }
    const RbTreeNode *parent = node->parent();
    while (node != parent->leftChild()) {
        node   = parent;
        parent = node->parent();
    }

    return parent;
}

const RbTreeNode *RbTreeUtil::previous(const RbTreeNode *node)
{
    BSLS_ASSERT(node);

    if (node->leftChild()) {
        return rightmost(node->leftChild());                          // RETURN
    }
    const RbTreeNode *parent = node->parent();
    while (node == parent->leftChild()) {
        node   = parent;
        parent = node->parent();
    }

    return parent;
}

void RbTreeUtil::insertAt(RbTreeAnchor *tree,
                          RbTreeNode   *parentNode,
                          bool          leftChildFlag,
                          RbTreeNode   *newNode)
{
    BSLS_ASSERT(parentNode);
    BSLS_ASSERT(newNode);
    BSLS_ASSERT(tree);

    newNode->setLeftChild(0);
    newNode->setRightChild(0);

    // Insert the node as a leaf in the tree, and assign its parent
    // pointer.

    newNode->makeRed();
    newNode->setParent(parentNode);
    if (leftChildFlag) {
        parentNode->setLeftChild(newNode);
        if (parentNode == tree->firstNode()) {
            tree->setFirstNode(newNode);
        }
    }
    else {
        parentNode->setRightChild(newNode);
    }

    // Fix the tree coloring (if necessary).
    // Implementation Note:  The following is adapted with few changes from
    // "Introduction to Algorithms" [Cormen, Leiserson, Rivest] , except that
    // explicit conditions are required to treat NULL values as Black, and
    // consequently to avoid setting a color (BLACK) on a (already BLACK) null
    // node.

    RbTreeNode *node = newNode;
    while (node != tree->rootNode() && node->parent()->isRed()) {
        if (isLeftChild(node->parent())) {
            RbTreeNode *uncle = node->parent()->parent()->rightChild();
            // Test if 'uncle' is BLACK (0 is considered BLACK)

            if (uncle && uncle->isRed()) {
                // Case 1:  grandParent[node] ->  (X:B)
                //                               /    \.
                //           parent[node]- > (Y:R)     (uncle:R)
                //                          /
                //                      (node:R)

                node->parent()->parent()->makeRed();
                node->parent()->makeBlack();
                uncle->makeBlack();

                node = node->parent()->parent();
            }
            else {
                if (isRightChild(node)) {
                    // Case 2:  grandParent[node] -> (X:B)
                    //                              /     \.
                    //           parent[node]- > (Y:R)    (uncle:B)
                    //                             \.
                    //                              (node:R)
                    //
                    // Perform a left rotation on parent[node] to reduce to
                    // case 3.

                    node = node->parent();
                    rotateLeft(node);
                }

                // Case 3:  grandParent[node] -> (X:B)
                //                              /     \.
                //           parent[node]- > (Y:R)    (uncle:B)
                //                            /
                //                        (node:R)
                //
                // Recolor the parent black, the grand parent-red, then rotate
                // the grand-parent right, so that its the new root of the
                // sub-tree.

                node->parent()->makeBlack();
                node->parent()->parent()->makeRed();
                rotateRight(node->parent()->parent());
            }
        }
        else {
            // The following mirrors the cases above, but with right and left
            // exchanged.

            RbTreeNode *uncle = node->parent()->parent()->leftChild();
            if (uncle && uncle->isRed()) {
                node->parent()->parent()->makeRed();
                node->parent()->makeBlack();
                uncle->makeBlack();

                node = node->parent()->parent();
            }
            else {
                if (isLeftChild(node)) {
                    node = node->parent();
                    rotateRight(node);

                }
                node->parent()->makeBlack();
                node->parent()->parent()->makeRed();
                rotateLeft(node->parent()->parent());
            }
        }
    }
    BSLS_ASSERT(tree->sentinel() == tree->rootNode()->parent());
    tree->rootNode()->makeBlack();
    tree->incrementNumNodes();
}

void RbTreeUtil::remove(RbTreeAnchor *tree, RbTreeNode *node)
{
    BSLS_ASSERT(0 != node);
    BSLS_ASSERT(0 != tree);
    BSLS_ASSERT(0 != tree->rootNode());

    RbTreeNode *x, *y;
    RbTreeNode *parentOfX;
    bool        yIsBlackFlag;

    // Implementation Note:  This implementation has been adjusted from the
    // one described in "Introduction to Algorithms" [Cormen, Leiserson,
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

        y = leftmost(node->rightChild());
        x = y->rightChild();
    }
    yIsBlackFlag = y->isBlack();

    if (y == node) {
        // We should be in case 1, where 'node' has (at least 1) null child,
        // and will simply be replaced by one of its children.  In this
        // context, 'x' refers to the node that will replace 'node'.  Simply
        // point the parent of 'node' to its new child, 'x'.  Note that in
        // this context, we may have to set the first and last node of the
        // tree.

        BSLS_ASSERT_SAFE(0 == node->leftChild() || 0 == node->rightChild());
        if (isLeftChild(node)) {
            // If the node being removed is to the left of its parent, it may
            // be the first node of the tree.

            if (node == tree->firstNode()) {
                tree->setFirstNode(next(node));
            }
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
        // replace 'node'.  Note that in this context, we never need to set
        // the first or last node of the tree (as the node being removed has
        // two children).

        BSLS_ASSERT_SAFE(0 != node->leftChild() && 0 != node->rightChild());
        BSLS_ASSERT_SAFE(0 == y->leftChild());
        BSLS_ASSERT_SAFE(x == y->rightChild());

        if (isLeftChild(node)) {
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

    if (yIsBlackFlag) {
        recolorTreeAfterRemoval(tree, x, parentOfX);
    }
    BSLS_ASSERT(!tree->rootNode() ||
                tree->sentinel() == tree->rootNode()->parent());
    tree->decrementNumNodes();
}

void RbTreeUtil::printTreeStructure(
                    FILE             *file,
                    const RbTreeNode *subtree,
                    void (*printNodeValueCallback)(FILE *, const RbTreeNode *),
                    int               level,
                    int               spacesPerLevel)
{
    BSLS_ASSERT(file);
    indent(file, level, spacesPerLevel);
    fprintf(file, "[");
    char SEP = (spacesPerLevel < 0) ? ' ' : '\n';
    if (subtree) {
        fprintf(file, " ");
        printNodeValueCallback(file, subtree);
        fprintf(file,
                " {%s}%c",
                subtree->color() == RbTreeNode::BSLALG_RED ? "RED" : "BLACK",
                SEP);
        if (subtree->leftChild()) {
            indent(file, level, spacesPerLevel);
            fprintf(file, "left:\n");
            printTreeStructure(file,
                               subtree->leftChild(),
                               printNodeValueCallback,
                               level+1,
                               spacesPerLevel);
        }
        if (subtree->rightChild()) {
            indent(file, level, spacesPerLevel);
            fprintf(file, "right:\n");
            printTreeStructure(file,
                               subtree->rightChild(),
                               printNodeValueCallback,
                               level+1,
                               spacesPerLevel);
        }
    }
    indent(file, level, spacesPerLevel);
    fprintf(file, "]%c", SEP);
}

void RbTreeUtil::rotateLeft(RbTreeNode *node)
{
    BSLS_ASSERT(node);
    BSLS_ASSERT(node->rightChild());

    RbTreeNode *pivot = node->rightChild();

    pivot->setParent(node->parent());
    node->setRightChild(pivot->leftChild());
    if (node->rightChild()) {
        node->rightChild()->setParent(node);
    }
    pivot->setLeftChild(node);
    node->setParent(pivot);

    if (pivot->parent()->leftChild() == node) {
        pivot->parent()->setLeftChild(pivot);
    }
    else {
        pivot->parent()->setRightChild(pivot);
    }
}

void RbTreeUtil::rotateRight(RbTreeNode *node)
{
    BSLS_ASSERT(node);
    BSLS_ASSERT(node->leftChild());

    RbTreeNode *pivot = node->leftChild();

    pivot->setParent(node->parent());
    node->setLeftChild(pivot->rightChild());
    if (node->leftChild()) {
        node->leftChild()->setParent(node);
    }
    pivot->setRightChild(node);
    node->setParent(pivot);

    if (pivot->parent()->leftChild() == node) {
        pivot->parent()->setLeftChild(pivot);
    }
    else {
        pivot->parent()->setRightChild(pivot);
    }
}

void RbTreeUtil::swap(RbTreeAnchor *a, RbTreeAnchor *b)
{
    BSLS_ASSERT(a);
    BSLS_ASSERT(b);

    RbTreeAnchor tmp(a->rootNode(),
                     a->firstNode(),
                     a->numNodes());
    a->reset(b->rootNode(), b->firstNode(), b->numNodes());
    b->reset(tmp.rootNode(), tmp.firstNode(), tmp.numNodes());

    // Readjust the sentinel nodes of both trees.

    if (0 == a->numNodes()) {
        a->setFirstNode(a->sentinel());
    }
    else {
        a->rootNode()->setParent(a->sentinel());
    }

    if (0 == b->numNodes()) {
        b->setFirstNode(b->sentinel());
    }
    else {
        b->rootNode()->setParent(b->sentinel());
    }
}

                      // --------------------------
                      // class RbTreeUtil_Validator
                      // --------------------------

// CLASS METHODS
bool RbTreeUtil_Validator::isWellFormedAnchor(const RbTreeAnchor& tree)
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

    //: 4 'tree.rootNode()' is 0 or 'tree.rootNode().isBlack()' is 'true'

    if (tree.rootNode() && !tree.rootNode()->isBlack()) {
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

}  // close package namespace
}  // close enterprise namespace

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
