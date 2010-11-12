// bdeci_rbtreeimp.cpp                -*-C++-*-

#include <bdeci_rbtreeimp.h>

#include <bdeu_print.h>

#include <cassert>
#include <ostream>

namespace BloombergLP {

// STATIC METHODS

inline
static bdeci_RbTreeImp_Node::Color color(bdeci_RbTreeImp_Node *node)
    // Return the color of the specified 'node' if 'node' is non-null, and
    // 'bdeci_RbTreeImp_Node::BDECI_BLACK' otherwise.
{
    return node ? node->d_color : bdeci_RbTreeImp_Node::BDECI_BLACK;
}

static std::ostream& printTreeNode(std::ostream&              stream,
                                   const bdeci_RbTreeImpIter& iter)
    // Write the sub-tree rooted at the node indicated by the specified 'iter'
    // to the specified 'stream' in some reasonable (single-line) format, and
    // return a reference to the modifiable 'stream'.
{
    if (iter) {
        stream << (iter.isBlack() ? 'b' : 'r') << " (";
        bdeci_RbTreeImpIter tmp;
        tmp = iter;
        tmp.moveLeft();
        printTreeNode(stream, tmp);
        stream << ") (";
        tmp = iter;
        tmp.moveRight();
        printTreeNode(stream, tmp);
        stream << ')';
    }

    return stream;
}

static std::ostream& printTreeNode(std::ostream&              stream,
                                   const bdeci_RbTreeImpIter& iter,
                                   int                        level,
                                   int                        spacesPerLevel)
    // Format the sub-tree rooted at the node indicated by the specified 'iter'
    // to the specified output 'stream' at the (absolute value of the)
    // specified indentation 'level' using the specified 'spacesPerLevel' of
    // indentation, and return a reference to the modifiable 'stream'.  Making
    // 'level' negative suppresses indentation for the first line only.  The
    // behavior is undefined unless 0 <= spacesPerLevel.
{
    assert(0 <= spacesPerLevel);

    bdeu_Print::indent(stream, level, spacesPerLevel);
    if (level < 0) {
        level = -level;
    }

    if (iter) {
        stream << (iter.isBlack() ? 'b' : 'r') << '\n';
        bdeci_RbTreeImpIter tmp;
        tmp = iter;
        tmp.moveLeft();
        printTreeNode(stream, tmp, level + 1, spacesPerLevel);
        tmp = iter;
        tmp.moveRight();
        printTreeNode(stream, tmp, level + 1, spacesPerLevel);
    }
    else {
        stream << "nil\n";
    }

    return stream;
}

inline
static void rotateLeft(bdeci_RbTreeImp_Node *node, bdeci_RbTreeImp_Node **root)
    // Perform a left rotation about the specified 'node' (see diagram below)
    // and update the specified 'root' if required.  The behavior is undefined
    // if 'node', 'node->d_right_p', 'root', or '*root' is null.
    //..
    //               ___                                      ___
    //              |   |                                    |   |
    //    node ---> | 1 |                                    | 2 |
    //              |___|                                    |___|
    //             /     \                                  /     `
    //            /       \                                /       `
    //           a         \___         ----->         ___/         c
    //                     |   |                      |   |
    //                     | 2 |                      | 1 | <--- node
    //                     |___|                      |___|
    //                    /     \                    /     `
    //                   /       \                  /       `
    //                  b         c                a         b
    //..
{
    assert(node);
    assert(node->d_right_p);
    assert(root);
    assert(*root);

    bdeci_RbTreeImp_Node *p = node->d_right_p;
    bdeci_RbTreeImp_Node *pl = p->d_left_p;
    bdeci_RbTreeImp_Node *parent = node->d_parent_p;
    p->d_parent_p = parent;
    node->d_parent_p = p;
    if (pl) {
        pl->d_parent_p = node;
    }
    node->d_right_p = pl;
    p->d_left_p = node;
    if (parent) {
        if (parent->d_left_p == node) {
            parent->d_left_p = p;
        }
        else {
            parent->d_right_p = p;
        }
    }
    else {
        *root = p;
    }
}

inline
static void rotateRight(bdeci_RbTreeImp_Node  *node,
                        bdeci_RbTreeImp_Node **root)
    // Perform a right rotation about the specified 'node' (see diagram below)
    // and update the specified 'root' if required.  The behavior is undefined
    // if 'node', 'node->d_left_p', 'root', or '*root' is null.
    //..
    //               ___                        ___
    //              |   |                      |   |
    //    node ---> | 1 |                      | 2 |
    //              |___|                      |___|
    //             /     \                    /     `
    //            /       \                  /       `
    //        ___/         c     ----->     a         \___
    //       |   |                                    |   |
    //       | 2 |                                    | 1 | <--- node
    //       |___|                                    |___|
    //      /     \                                  /     `
    //     /       \                                /       `
    //    a         b                              b         c
    //..
{
    assert(node);
    assert(node->d_left_p);
    assert(root);
    assert(*root);

    bdeci_RbTreeImp_Node *p = node->d_left_p;
    bdeci_RbTreeImp_Node *pr = p->d_right_p;
    bdeci_RbTreeImp_Node *parent = node->d_parent_p;
    p->d_parent_p = parent;
    node->d_parent_p = p;
    if (pr) {
        pr->d_parent_p = node;
    }
    node->d_left_p = pr;
    p->d_right_p = node;
    if (parent) {
        if (parent->d_left_p == node) {
            parent->d_left_p = p;
        }
        else {
            parent->d_right_p = p;
        }
    }
    else {
        *root = p;
    }
}

static void correctDoubleBlack(bdeci_RbTreeImp_Node  *node,
                               bdeci_RbTreeImp_Node **root)
    // Correct the coloration and balance of that portion of the red-black
    // tree having the specified 'root' that ascends from the sibling of the
    // specified 'node'.  The behavior is undefined if 'node', 'root', or
    // '*root' is null.  Note that this method is called under some
    // circumstances following the removal of a node from a red-black tree.
{
    assert(node);
    assert(root);
    assert(*root);

    while (node->d_parent_p
        && bdeci_RbTreeImp_Node::BDECI_BLACK == node->d_color) {
        bdeci_RbTreeImp_Node *parent = node->d_parent_p;
        if (node == parent->d_left_p) {
            bdeci_RbTreeImp_Node *sibling = parent->d_right_p;
            if (bdeci_RbTreeImp_Node::BDECI_RED == sibling->d_color) {
                sibling->d_color = bdeci_RbTreeImp_Node::BDECI_BLACK;
                parent->d_color = bdeci_RbTreeImp_Node::BDECI_RED;
                rotateLeft(parent, root);
                sibling = parent->d_right_p;
            }
            if (bdeci_RbTreeImp_Node::BDECI_BLACK == color(sibling->d_left_p)
             && bdeci_RbTreeImp_Node::BDECI_BLACK ==
                                                   color(sibling->d_right_p)) {
                sibling->d_color = bdeci_RbTreeImp_Node::BDECI_RED;
                node = parent;
            }
            else {
                if (bdeci_RbTreeImp_Node::BDECI_BLACK ==
                                                   color(sibling->d_right_p)) {
                    sibling->d_left_p->d_color =
                                             bdeci_RbTreeImp_Node::BDECI_BLACK;
                    sibling->d_color = bdeci_RbTreeImp_Node::BDECI_RED;
                    rotateRight(sibling, root);
                    sibling = parent->d_right_p;
                }
                sibling->d_color = parent->d_color;
                parent->d_color = bdeci_RbTreeImp_Node::BDECI_BLACK;
                sibling->d_right_p->d_color =
                                             bdeci_RbTreeImp_Node::BDECI_BLACK;
                rotateLeft(parent, root);
                return;
            }
        }
        else {
            bdeci_RbTreeImp_Node *sibling = parent->d_left_p;
            if (bdeci_RbTreeImp_Node::BDECI_RED == sibling->d_color) {
                sibling->d_color = bdeci_RbTreeImp_Node::BDECI_BLACK;
                parent->d_color = bdeci_RbTreeImp_Node::BDECI_RED;
                rotateRight(parent, root);
                sibling = parent->d_left_p;
            }
            if (bdeci_RbTreeImp_Node::BDECI_BLACK == color(sibling->d_right_p)
             && bdeci_RbTreeImp_Node::BDECI_BLACK ==
                                                    color(sibling->d_left_p)) {
                sibling->d_color = bdeci_RbTreeImp_Node::BDECI_RED;
                node = parent;
            }
            else {
                if (bdeci_RbTreeImp_Node::BDECI_BLACK ==
                                                    color(sibling->d_left_p)) {
                    sibling->d_right_p->d_color =
                                             bdeci_RbTreeImp_Node::BDECI_BLACK;
                    sibling->d_color = bdeci_RbTreeImp_Node::BDECI_RED;
                    rotateLeft(sibling, root);
                    sibling = parent->d_left_p;
                }
                sibling->d_color = parent->d_color;
                parent->d_color = bdeci_RbTreeImp_Node::BDECI_BLACK;
                sibling->d_left_p->d_color = bdeci_RbTreeImp_Node::BDECI_BLACK;
                rotateRight(parent, root);
                return;
            }
        }
    }
    node->d_color = bdeci_RbTreeImp_Node::BDECI_BLACK;
}

static void rebalance(bdeci_RbTreeImp_Node *node, bdeci_RbTreeImp_Node **root)
    // Correct the coloration and balance of the red-black tree having the
    // specified 'root' following the insertion into the tree of the specified
    // 'node'.  The behavior is undefined if 'node', 'node->d_parent_p',
    // 'root', or '*root' is null.
{
    assert(node);
    assert(node->d_parent_p);
    assert(root);
    assert(*root);

    bdeci_RbTreeImp_Node *parent = node->d_parent_p;
    while (bdeci_RbTreeImp_Node::BDECI_RED == parent->d_color) {
        // Parent being BDECI_RED implies parent is not root.
        bdeci_RbTreeImp_Node *grandparent = parent->d_parent_p;
        if (parent == grandparent->d_left_p) {
            bdeci_RbTreeImp_Node *p = grandparent->d_right_p;
            if (p && bdeci_RbTreeImp_Node::BDECI_RED == p->d_color) {
                grandparent->d_color = bdeci_RbTreeImp_Node::BDECI_RED;
                parent->d_color = bdeci_RbTreeImp_Node::BDECI_BLACK;
                p->d_color = bdeci_RbTreeImp_Node::BDECI_BLACK;

                // Update for next iteration.
                node = grandparent;
                parent = node->d_parent_p;
                if (!parent) {
                    node->d_color = bdeci_RbTreeImp_Node::BDECI_BLACK;
                    return;
                }
            }
            else {
                if (node == parent->d_right_p) {
                    node = parent;
                    rotateLeft(node, root);
                    parent = node->d_parent_p;
                    grandparent = parent->d_parent_p;
                }
                parent->d_color = bdeci_RbTreeImp_Node::BDECI_BLACK;
                grandparent->d_color = bdeci_RbTreeImp_Node::BDECI_RED;
                rotateRight(grandparent, root);
            }
        }
        else {
            bdeci_RbTreeImp_Node *p = grandparent->d_left_p;
            if (p && bdeci_RbTreeImp_Node::BDECI_RED == p->d_color) {
                grandparent->d_color = bdeci_RbTreeImp_Node::BDECI_RED;
                parent->d_color = bdeci_RbTreeImp_Node::BDECI_BLACK;
                p->d_color = bdeci_RbTreeImp_Node::BDECI_BLACK;

                // Update for next iteration.
                node = grandparent;
                parent = node->d_parent_p;
                if (!parent) {
                    node->d_color = bdeci_RbTreeImp_Node::BDECI_BLACK;
                    return;
                }
            }
            else {
                if (node == parent->d_left_p) {
                    node = parent;
                    rotateRight(node, root);
                    parent = node->d_parent_p;
                    grandparent = parent->d_parent_p;
                }
                parent->d_color = bdeci_RbTreeImp_Node::BDECI_BLACK;
                grandparent->d_color = bdeci_RbTreeImp_Node::BDECI_RED;
                rotateLeft(grandparent, root);
            }
        }
    }
}

// PRIVATE MANIPULATORS

void bdeci_RbTreeImp::listToTree()
{
    enum { MAX_HEIGHT = 128 };  // This height supports at least 2^64 nodes.
    bdeci_RbTreeImp_Node *node = d_first_p;

    bdeci_RbTreeImp_Node *root[MAX_HEIGHT];
    bdeci_RbTreeImp_Node *join[MAX_HEIGHT];
    bdeci_RbTreeImp_Node *last = 0;
    bdeci_RbTreeImp_Node *lastInRoot;
    int height[MAX_HEIGHT];
    int num = 0;

    root[0] = 0;
    join[0] = 0;
    height[0] = 0;

    while (node) {
        node->d_color = bdeci_RbTreeImp_Node::BDECI_BLACK;
        node->d_left_p = 0;
        node->d_right_p = 0;
        node->d_parent_p = 0;
        if (join[num]) {
            ++num;
            root[num] = 0;
            join[num] = 0;
            height[num] = 0;
        }
        if (!root[num]) {
            root[num] = node;
            if (last) {
                last->d_next_p = root[num];
            }
            last = root[num];
            lastInRoot = root[num];
        }
        else {
            join[num] = node;
            last->d_next_p = join[num];
            last = join[num];
            while (num && height[num-1] == height[num]) {
                --num;
                join[num]->d_left_p = root[num];
                join[num]->d_right_p = root[num+1];
                root[num]->d_parent_p = join[num];
                root[num+1]->d_parent_p = join[num];
                root[num] = join[num];
                join[num] = join[num+1];
                ++height[num];
            }
        }
        node = node->d_next_p;
    }

    if (join[num]) {
        lastInRoot->d_right_p = join[num];
        join[num]->d_parent_p = lastInRoot;
        join[num]->d_color = bdeci_RbTreeImp_Node::BDECI_RED;
    }
    while (num) {
        int numMove = height[num-1] - height[num];
        const bdeci_RbTreeImp_Node::Color color = numMove
                                           ? bdeci_RbTreeImp_Node::BDECI_RED
                                           : bdeci_RbTreeImp_Node::BDECI_BLACK;
        --num;
        bdeci_RbTreeImp_Node **addrNode = &root[num];
        bdeci_RbTreeImp_Node *parent    = root[num]->d_parent_p;
        while (numMove) {
            parent   = *addrNode;
            addrNode = &(*addrNode)->d_right_p;
            --numMove;
        }
        join[num]->d_left_p = *addrNode;
        join[num]->d_right_p = root[num+1];
        join[num]->d_color = color;
        (*addrNode)->d_parent_p = join[num];
        root[num+1]->d_parent_p = join[num];
        if (bdeci_RbTreeImp_Node::BDECI_BLACK == color) {
            height[num]++;
        }
        join[num]->d_parent_p = parent;
        *addrNode = join[num];
    }

    d_root_p = root[0];
}

bdeci_RbTreeImp_Node *bdeci_RbTreeImp::insertAfter(bdeci_RbTreeImp_Node *node)
{
    assert(node);

    bdeci_RbTreeImp_Node *q = (bdeci_RbTreeImp_Node *)d_pool.allocate();
    bdeci_RbTreeImp_Node *n = node->d_next_p;
    q->d_left_p = 0;
    q->d_right_p = 0;
    if (node->d_right_p) {
        assert(0 == n->d_left_p);
        n->d_left_p = q;
        q->d_parent_p = n;
    }
    else {
        node->d_right_p = q;
        q->d_parent_p = node;
    }
    node->d_next_p = q;
    q->d_next_p = n;
    q->d_color = bdeci_RbTreeImp_Node::BDECI_RED;
    rebalance(q, &d_root_p);
    ++d_numElements;

    return q;
}

void bdeci_RbTreeImp::remove(bdeci_RbTreeImp_Node *node)
{
    assert(node);

    bdeci_RbTreeImp_Node *prev = 0;

    if (node->d_left_p) {
        prev = node->d_left_p;
        while (prev->d_right_p) {
            prev = prev->d_right_p;
        }
    }
    else {
        // Node's predecessor (prev) is its nearest ancestor that has
        // node in its right subtree.
        bdeci_RbTreeImp_Node *tmp    = node;
        bdeci_RbTreeImp_Node *parent = node->d_parent_p;
        while (parent && tmp != parent->d_right_p) {
            tmp = parent;
            parent = tmp->d_parent_p;
        }
        if (parent && tmp == parent->d_right_p) {
            prev = parent;
        }
    }
    remove(node, prev);
}

void bdeci_RbTreeImp::remove(bdeci_RbTreeImp_Node *node,
                             bdeci_RbTreeImp_Node *previous)
{
    assert(node);
    assert(previous ? previous->d_next_p == node : d_first_p == node);

    if (previous) {
        previous->d_next_p = node->d_next_p;
    }
    else {
        d_first_p = node->d_next_p;
    }

    bdeci_RbTreeImp_Node *nodeToRemove = 0;
    if (node->d_right_p && node->d_left_p) {

        // Remove the node after this node, which must be non-null since there
        // is a right child, and then "replace" this node with the removed one.

        nodeToRemove = node;
        node = node->d_next_p;
    }

    bdeci_RbTreeImp_Node *parent = node->d_parent_p;
    bdeci_RbTreeImp_Node *child  = node->d_left_p ? node->d_left_p
                                                  : node->d_right_p;
    if (0 == parent) {

        // Node is the root.

        d_root_p = child;
        if (child) {
            child->d_parent_p = 0;
            child->d_color = bdeci_RbTreeImp_Node::BDECI_BLACK;
        }
    }
    else if (child) {

        // Node is not the root, but has a child.

        if (node == parent->d_left_p) {
            parent->d_left_p = child;
        }
        else {
            parent->d_right_p = child;
        }
        child->d_parent_p = parent;
        if (bdeci_RbTreeImp_Node::BDECI_BLACK == node->d_color) {
            correctDoubleBlack(child, &d_root_p);
        }
    }
    else if (bdeci_RbTreeImp_Node::BDECI_RED == node->d_color) {

        // Node is not the root, has no children, and is red.

        if (node == parent->d_left_p) {
            parent->d_left_p = 0;
        }
        else {
            parent->d_right_p = 0;
        }
    }
    else if (node == parent->d_left_p) {

        // Node is not root, has no children, is black, and is a left child.

        parent->d_left_p = 0;
        bdeci_RbTreeImp_Node *sibling = parent->d_right_p;
        if (sibling) {
            if (bdeci_RbTreeImp_Node::BDECI_RED == sibling->d_color) {
                sibling->d_color = bdeci_RbTreeImp_Node::BDECI_BLACK;
                parent->d_color  = bdeci_RbTreeImp_Node::BDECI_RED;
                rotateLeft(parent, &d_root_p);
                sibling = parent->d_right_p;
            }
            if (bdeci_RbTreeImp_Node::BDECI_BLACK == color(sibling->d_left_p)
             && bdeci_RbTreeImp_Node::BDECI_BLACK ==
                                                   color(sibling->d_right_p)) {
                sibling->d_color = bdeci_RbTreeImp_Node::BDECI_RED;
                correctDoubleBlack(parent, &d_root_p);
            }
            else {
                if (bdeci_RbTreeImp_Node::BDECI_BLACK ==
                                                   color(sibling->d_right_p)) {
                    sibling->d_left_p->d_color =
                                             bdeci_RbTreeImp_Node::BDECI_BLACK;
                    sibling->d_color = bdeci_RbTreeImp_Node::BDECI_RED;
                    rotateRight(sibling, &d_root_p);
                    sibling = parent->d_right_p;
                }
                sibling->d_color = parent->d_color;
                parent->d_color  = bdeci_RbTreeImp_Node::BDECI_BLACK;
                sibling->d_right_p->d_color =
                                             bdeci_RbTreeImp_Node::BDECI_BLACK;
                rotateLeft(parent, &d_root_p);
            }
        }
    }
    else {

        // Node is not root, has no children, is black, and is a right child.

        parent->d_right_p = 0;
        bdeci_RbTreeImp_Node *sibling = parent->d_left_p;
        if (sibling) {
            if (bdeci_RbTreeImp_Node::BDECI_RED == sibling->d_color) {
                sibling->d_color = bdeci_RbTreeImp_Node::BDECI_BLACK;
                parent->d_color  = bdeci_RbTreeImp_Node::BDECI_RED;
                rotateRight(parent, &d_root_p);
                sibling = parent->d_left_p;
            }
            if (bdeci_RbTreeImp_Node::BDECI_BLACK == color(sibling->d_right_p)
             && bdeci_RbTreeImp_Node::BDECI_BLACK ==
                                                    color(sibling->d_left_p)) {
                sibling->d_color = bdeci_RbTreeImp_Node::BDECI_RED;
                correctDoubleBlack(parent, &d_root_p);
            }
            else {
                if (bdeci_RbTreeImp_Node::BDECI_BLACK ==
                                                    color(sibling->d_left_p)) {
                    sibling->d_right_p->d_color =
                                             bdeci_RbTreeImp_Node::BDECI_BLACK;
                    sibling->d_color = bdeci_RbTreeImp_Node::BDECI_RED;
                    rotateLeft(sibling, &d_root_p);
                    sibling = parent->d_left_p;
                }
                sibling->d_color = parent->d_color;
                parent->d_color  = bdeci_RbTreeImp_Node::BDECI_BLACK;
                sibling->d_left_p->d_color = bdeci_RbTreeImp_Node::BDECI_BLACK;
                rotateRight(parent, &d_root_p);
            }
        }
    }

    if (nodeToRemove) {
        node->d_parent_p = nodeToRemove->d_parent_p;
        node->d_left_p   = nodeToRemove->d_left_p;
        node->d_right_p  = nodeToRemove->d_right_p;
        node->d_color    = nodeToRemove->d_color;

        if (nodeToRemove->d_parent_p) {
            if (nodeToRemove == nodeToRemove->d_parent_p->d_left_p) {
                nodeToRemove->d_parent_p->d_left_p = node;
            }
            else {
                nodeToRemove->d_parent_p->d_right_p = node;
            }
        }
        else {
            d_root_p = node;
        }
        if (nodeToRemove->d_left_p) {
            nodeToRemove->d_left_p->d_parent_p = node;
        }
        if (nodeToRemove->d_right_p) {
            nodeToRemove->d_right_p->d_parent_p = node;
        }
        d_pool.deallocate(nodeToRemove);
    }
    else {
        d_pool.deallocate(node);
    }
    --d_numElements;
}

// MANIPULATORS

void *bdeci_RbTreeImp::insertAfter(const void *node)
{
    assert(node);

    bdeci_RbTreeImp_Node *p = insertAfter((bdeci_RbTreeImp_Node *)((char *)node
              - sizeof(bdeci_RbTreeImp_Node) + bdes_Alignment::MAX_ALIGNMENT));

    return &p->d_memory;
}

void *bdeci_RbTreeImp::insertAsFirst()
{
    bdeci_RbTreeImp_Node *q = (bdeci_RbTreeImp_Node *)d_pool.allocate();
    q->d_left_p = 0;
    q->d_right_p = 0;
    if (d_first_p) {
        d_first_p->d_left_p = q;
        q->d_parent_p = d_first_p;
        q->d_next_p = d_first_p;
        q->d_color = bdeci_RbTreeImp_Node::BDECI_RED;
        rebalance(q, &d_root_p);
    }
    else {
        d_root_p = q;
        q->d_parent_p = 0;
        q->d_next_p = 0;
        q->d_color = bdeci_RbTreeImp_Node::BDECI_BLACK;
    }
    d_first_p = q;
    ++d_numElements;

    return &q->d_memory;
}

// ACCESSORS

std::ostream& bdeci_RbTreeImp::print(std::ostream& stream,
                                     int           level,
                                     int           spacesPerLevel) const
{
    assert(0 <= spacesPerLevel);

    bdeci_RbTreeImpIter iter(*this);
    printTreeNode(stream, iter, level, spacesPerLevel);

    return stream;
}

// FREE OPERATORS

std::ostream& operator<<(std::ostream& stream, const bdeci_RbTreeImp& rhs)
{
    stream << '(';
    printTreeNode(stream, bdeci_RbTreeImpIter(rhs));
    stream << ')';

    return stream;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
