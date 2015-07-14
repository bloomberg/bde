// ball_scopedattribute.cpp   -*-C++-*-

#include <ball_scopedattribute.h>

#include <bdlb_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

namespace ball {
                    // ------------------------------------
                    // class ScopedAttribute_Container
                    // ------------------------------------

// CREATORS
ScopedAttribute_Container::~ScopedAttribute_Container()
{
}

// ACCESSORS
bsl::ostream&
ScopedAttribute_Container::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    const char EL = spacesPerLevel < 0 ? ' ' : '\n';
    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "[ " << d_attribute << ']' << EL;
    return stream;
}
}  // close package namespace

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
