// baem_category.cpp      -*-C++-*-
#include <baem_category.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baem_category_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

                   // -------------------
                   // class baem_Category
                   // -------------------

// CREATORS
baem_Category::~baem_Category()
{
    // Reset the linked-list of category holders registered with this object.
    while (d_holders_p) {
        baem_CategoryHolder *next = d_holders_p->next();
        d_holders_p->reset();
        d_holders_p = next;
    }
}

// MANIPULATORS
void baem_Category::setEnabled(bool enabledFlag)
{
    if (d_enabled != enabledFlag) {
        d_enabled = enabledFlag;

        // Update the linked-list of category holder's registered with this
        // category.
        baem_CategoryHolder *holder = d_holders_p;
        while (holder) {
            holder->setEnabled(enabledFlag);
            holder = holder->next();
        }
    }
}

void baem_Category::registerCategoryHolder(baem_CategoryHolder *holder)
{
    holder->setEnabled(d_enabled);
    holder->setCategory(this);
    holder->setNext(d_holders_p);
    d_holders_p = holder;
}

// ACCESSORS
bsl::ostream& baem_Category::print(bsl::ostream& stream) const
{
    stream << "[ " << d_name_p << (d_enabled ? " ENABLED ]" : " DISABLED ]");
    return stream;
}

                        // -------------------------
                        // class baem_CategoryHolder
                        // -------------------------

// MANIPULATORS
void baem_CategoryHolder::reset()
{
    d_enabled    = false;
    d_category_p = 0;
    d_next_p     = 0;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
