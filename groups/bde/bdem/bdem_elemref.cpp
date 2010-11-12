// bdem_elemref.cpp              -*-C++-*-
#include <bdem_elemref.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdem_elemref_cpp,"$Id$ $CSID$")


#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                        // -----------------------
                        // class bdem_ConstElemRef
                        // -----------------------

// FREE OPERATORS
bool operator==(const bdem_ConstElemRef& lhs, const bdem_ConstElemRef& rhs)
{
    if (lhs.isBound() && rhs.isBound()) {
        if (lhs.d_descriptor_p->d_elemEnum != rhs.d_descriptor_p->d_elemEnum) {
            // references with different descriptors cannot be equal
            return false;
        }

        if (lhs.isNonNull() && rhs.isNonNull()) {
            // both are non null defer to the descriptor's areEqual function
            return lhs.d_descriptor_p->areEqual(lhs.d_constData_p,
                                                rhs.d_constData_p);
        }

        return lhs.isNull() == rhs.isNull();
    }

    return lhs.isBound() == rhs.isBound();
}

// ACCESSORS
bsl::ostream& bdem_ConstElemRef::print(bsl::ostream& stream,
                                       int           level,
                                       int           spacesPerLevel) const
{
    if (!isBound()) {
        return stream << "unbound";
    }

    if (isNull()) {
        return stream << "NULL";
    }

    return d_descriptor_p->print(d_constData_p, stream, level, spacesPerLevel);
}

                        // ------------------
                        // class bdem_ElemRef
                        // ------------------

// REFERENCED-VALUE MANIPULATORS
void bdem_ElemRef::replaceValue(const bdem_ConstElemRef& referenceObject) const
{
    // Catch isNull before the assign in case the 'referenceObject' is a
    // subobject of the thing 'this' refers to.
    bool isNull = referenceObject.isNull();

    d_descriptor_p->assign(d_data_p,
                           referenceObject.d_constData_p);

    if (isNull) {
        setNullnessBit();
    } else {
        clearNullnessBit();
    }
}

// ACCESSORS
void *bdem_ElemRef::dataRaw() const
{
    return d_data_p;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
