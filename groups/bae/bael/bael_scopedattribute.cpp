// bael_scopedattribute.cpp   -*-C++-*-

#include <bael_scopedattribute.h>

#include <bdeu_print.h>

#include <bsl_ostream.h>

namespace BloombergLP {

                    // ------------------------------------
                    // class bael_ScopedAttribute_Container
                    // ------------------------------------

// CREATORS
bael_ScopedAttribute_Container::~bael_ScopedAttribute_Container()
{
}

// ACCESSORS
bsl::ostream&
bael_ScopedAttribute_Container::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    const char EL = spacesPerLevel < 0 ? ' ' : '\n';
    bdeu_Print::indent(stream, level, spacesPerLevel);
    stream << "[ " << d_attribute << ']' << EL;
    return stream;
}

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
