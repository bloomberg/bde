// ball_attribute.cpp                 -*-C++-*-
#include <ball_attribute.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_attribute_cpp,"$Id$ $CSID$")

#include <bdlb_hashutil.h>

#include <bsls_types.h>

#include <bsl_cstring.h>
#include <bsl_functional.h>
#include <bsl_ostream.h>

namespace BloombergLP {

namespace ball {
// CLASS METHODS
int Attribute::hash(const Attribute& attribute, int size)
{
    if (attribute.d_hashValue < 0 || attribute.d_hashSize != size) {

        unsigned int hash = bdlb::HashUtil::hash1(attribute.d_name,
                                                bsl::strlen(attribute.d_name));

        if (attribute.d_value.is<int>()) {
            hash += bdlb::HashUtil::hash1(attribute.d_value.the<int>());
        } else if (attribute.d_value.is<bsls::Types::Int64>()) {
            hash += bdlb::HashUtil::hash1(
                                  attribute.d_value.the<bsls::Types::Int64>());
        }
        else {
            hash += bdlb::HashUtil::hash1(
                attribute.d_value.the<bsl::string>().c_str(),
                attribute.d_value.the<bsl::string>().length());
        }

        attribute.d_hashValue = hash % size;
        attribute.d_hashSize  = size;
    }
    return attribute.d_hashValue;
}

// ACCESSORS
bsl::ostream& Attribute::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "[ " << d_name << " = ";
    d_value.print(stream, 0, -1);
    stream << " ]";
    return stream;
}
}  // close package namespace

// FREE OPERATORS
bsl::ostream& ball::operator<<(bsl::ostream&            output,
                         const Attribute&    attribute)
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
