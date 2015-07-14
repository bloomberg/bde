// balm_category.cpp      -*-C++-*-
#include <balm_category.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(balm_category_cpp,"$Id$ $CSID$")

#include <bsl_ostream.h>

namespace BloombergLP {

namespace balm {
                   // -------------------
                   // class Category
                   // -------------------

// CREATORS
Category::~Category()
{
    // Reset the linked-list of category holders registered with this object.
    while (d_holders_p) {
        CategoryHolder *next = d_holders_p->next();
        d_holders_p->reset();
        d_holders_p = next;
    }
}

// MANIPULATORS
void Category::setEnabled(bool enabledFlag)
{
    if (d_enabled != enabledFlag) {
        d_enabled = enabledFlag;

        // Update the linked-list of category holder's registered with this
        // category.
        CategoryHolder *holder = d_holders_p;
        while (holder) {
            holder->setEnabled(enabledFlag);
            holder = holder->next();
        }
    }
}

void Category::registerCategoryHolder(CategoryHolder *holder)
{
    holder->setEnabled(d_enabled);
    holder->setCategory(this);
    holder->setNext(d_holders_p);
    d_holders_p = holder;
}

// ACCESSORS
bsl::ostream& Category::print(bsl::ostream& stream) const
{
    stream << "[ " << d_name_p << (d_enabled ? " ENABLED ]" : " DISABLED ]");
    return stream;
}

                        // -------------------------
                        // class CategoryHolder
                        // -------------------------

// MANIPULATORS
void CategoryHolder::reset()
{
    d_enabled    = false;
    d_category_p = 0;
    d_next_p     = 0;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
