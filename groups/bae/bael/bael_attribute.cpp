// bael_attribute.cpp                 -*-C++-*-
#include <bael_attribute.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bael_attribute_cpp,"$Id$ $CSID$")

#include <bdeu_hashutil.h>

#include <bsl_cstring.h>
#include <bsl_functional.h>
#include <bsl_ostream.h>

namespace BloombergLP {

// CLASS METHODS
int bael_Attribute::hash(const bael_Attribute& attribute, int size)
{
    if (attribute.d_hashValue < 0 || attribute.d_hashSize != size) {

        unsigned int hash = bdeu_HashUtil::hash1(attribute.d_name,
                                                bsl::strlen(attribute.d_name));

        if (attribute.d_value.is<int>()) {
            hash += bdeu_HashUtil::hash1(attribute.d_value.the<int>());
        } else if (attribute.d_value.is<bsls_PlatformUtil::Int64>()) {
            hash += bdeu_HashUtil::hash1(
                           attribute.d_value.the<bsls_PlatformUtil::Int64>());
        }
        else {
            hash += bdeu_HashUtil::hash1(
                attribute.d_value.the<bsl::string>().c_str(),
                attribute.d_value.the<bsl::string>().length());
        }

        attribute.d_hashValue = hash % size;
        attribute.d_hashSize  = size;
    }
    return attribute.d_hashValue;
}

// ACCESSORS
bsl::ostream& bael_Attribute::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "[ " << d_name << " = ";
    d_value.print(stream, 0, -1);
    stream << " ]";
    return stream;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&            output,
                         const bael_Attribute&    attribute)
{
    attribute.print(output, 0, -1);
    return output;
}

} // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
