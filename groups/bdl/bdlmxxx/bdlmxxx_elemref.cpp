// bdlmxxx_elemref.cpp                                                   -*-C++-*-
#include <bdlmxxx_elemref.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_elemref_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                        // -----------------------
                        // class bdlmxxx::ConstElemRef
                        // -----------------------

// FREE OPERATORS
bool bdlmxxx::operator==(const ConstElemRef& lhs, const ConstElemRef& rhs)
{
    if (lhs.isBound() && rhs.isBound()) {
        if (lhs.d_descriptor_p->d_elemEnum != rhs.d_descriptor_p->d_elemEnum) {
            // References with different descriptors cannot be equal.

            return false;
        }

        if (lhs.isNonNull() && rhs.isNonNull()) {
            // Both are non-null, so forward to the descriptor's 'areEqual'
            // function.

            return lhs.d_descriptor_p->areEqual(lhs.d_constData_p,
                                                rhs.d_constData_p);
        }

        return lhs.isNull() == rhs.isNull();
    }

    return lhs.isBound() == rhs.isBound();
}

namespace bdlmxxx {
// ACCESSORS
bsl::ostream& ConstElemRef::print(bsl::ostream& stream,
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
                        // class ElemRef
                        // ------------------

// REFERENCED-VALUE MANIPULATORS
void ElemRef::replaceValue(const ConstElemRef& referenceObject) const
{
    BSLS_ASSERT_SAFE(isBound());
    BSLS_ASSERT_SAFE(referenceObject.isBound());
    BSLS_ASSERT_SAFE(type() == referenceObject.type());

    // Catch 'isNull' before the call to 'assign' in case 'referenceObject' is
    // a sub-object of the thing to which 'this' refers.

    const bool isNull = referenceObject.isNull();

    d_descriptor_p->assign(d_data_p, referenceObject.d_constData_p);

    if (isNull) {
        setNullnessBit();
    } else {
        clearNullnessBit();
    }
}

// ACCESSORS
void *ElemRef::dataRaw() const
{
    return d_data_p;
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
