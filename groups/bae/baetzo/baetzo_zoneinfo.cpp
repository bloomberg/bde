// baetzo_zoneinfo.cpp                                                -*-C++-*-

#include <baetzo_zoneinfo.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(baetzo_zoneinfo_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

// STATIC HELPER FUNCTIONS
static
bool containsDescriptor(
                     const bsl::vector<baetzo_ZoneinfoTransition>& transitions,
                     const baetzo_LocalTimeDescriptor&             descriptor)
    // Return 'true' if the specified 'transitions' contain a transition with
    // the specified 'descriptor', and 'false' otherwise.
{
    baetzo_Zoneinfo::TransitionConstIterator it  = transitions.begin();
    baetzo_Zoneinfo::TransitionConstIterator end = transitions.end();
    for (; it != end; ++it) {
        if (descriptor == it->descriptor()) {
            return true;                                              // RETURN
        }
    }
    return false;
}

                        // -------------------------------
                        // class baetzo_ZoneinfoTransition
                        // -------------------------------

// ACCESSORS
bsl::ostream&
baetzo_ZoneinfoTransition::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << "[";

    if (level < 0) {
        level = -level;
    }

    ++level;

    bdeu_Print::newlineAndIndent(stream, level, spacesPerLevel);

    bdet_Datetime utcDatetime;
    int rc = bdetu_Epoch::convertFromTimeT64(&utcDatetime, d_utcTime);
    stream << "time = ";
    if (!rc) {
        stream << utcDatetime;
    }
    else {
        stream << d_utcTime;
    }

    bdeu_Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << "descriptor = ";
    d_descriptor_p->print(stream, -level, spacesPerLevel);

    if (spacesPerLevel < 0) {
        stream << ' ';
    }
    else {
        bdeu_Print::indent(stream, level - 1, spacesPerLevel);
    }

    stream << "]";

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&                    stream,
                         const baetzo_ZoneinfoTransition& object)
{
    stream << "[ ";

    bdet_Datetime utcDatetime;
    int rc = bdetu_Epoch::convertFromTimeT64(&utcDatetime, object.utcTime());
    if (!rc) {
        stream << utcDatetime << " ";
    }
    else {
        stream << object.utcTime() << " ";
    }

    stream << object.descriptor()
           << " ]";
    return stream;
}

                        // -------------------------------------
                        // class baetzo_Zoneinfo::DescriptorLess
                        // -------------------------------------

// ACCESSORS
bool baetzo_Zoneinfo::DescriptorLess::operator()(
                                   const baetzo_LocalTimeDescriptor& lhs,
                                   const baetzo_LocalTimeDescriptor& rhs) const
{
    if (lhs.utcOffsetInSeconds() != rhs.utcOffsetInSeconds()) {
        return lhs.utcOffsetInSeconds() < rhs.utcOffsetInSeconds();   // RETURN
    }

    if (lhs.description() != rhs.description()) {
        return lhs.description() < rhs.description();                 // RETURN
    }

    return lhs.dstInEffectFlag() < rhs.dstInEffectFlag();
}

                        // ---------------------
                        // class baetzo_Zoneinfo
                        // ---------------------

// CREATORS
baetzo_Zoneinfo::baetzo_Zoneinfo(const baetzo_Zoneinfo&  original,
                                 bslma_Allocator        *basicAllocator)
: d_identifier(original.d_identifier, basicAllocator)
, d_descriptors(original.d_descriptors, basicAllocator)
, d_transitions(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
    TransitionConstIterator it  = original.d_transitions.begin();
    TransitionConstIterator end = original.d_transitions.end();
    for (; it != end; ++it) {
        addTransition(it->utcTime(), it->descriptor());
    }
}

// MANIPULATORS
void baetzo_Zoneinfo::addTransition(
                                  bdetu_Epoch::TimeT64              utcTime,
                                  const baetzo_LocalTimeDescriptor& descriptor)
{
    typedef bsl::vector<baetzo_ZoneinfoTransition>::iterator
                                                            TransitionIterator;

    // Insert the description in the set and get back an iterator pointing to
    // the inserted item.

    DescriptorSet::iterator descriptorIterator =
                                        d_descriptors.insert(descriptor).first;

    baetzo_ZoneinfoTransition newTransition(utcTime, &(*descriptorIterator));

    if (0 == d_transitions.size()) {
        d_transitions.push_back(newTransition);
        return;                                                       // RETURN
    }

    TransitionIterator it = bsl::lower_bound(d_transitions.begin(),
                                             d_transitions.end(),
                                             newTransition);

    if (it != d_transitions.end()
     && it->utcTime() == newTransition.utcTime()) {
        // Replace the existing transition with a new descriptor.  Once the
        // descriptor is replaced, if there are no more references to the
        // previous descriptor in 'd_transitions', then remove the descriptor
        // from 'd_descriptors'.

        DescriptorSet::iterator descIt = d_descriptors.find(it->descriptor());
        BSLS_ASSERT_SAFE(descIt != d_descriptors.end());

        *it = newTransition;
        if (!containsDescriptor(d_transitions, *descIt)) {
            d_descriptors.erase(descIt);
        }
    }
    else {

        d_transitions.insert(it, newTransition);
    }

    return;
}

// ACCESSORS
baetzo_Zoneinfo::TransitionConstIterator
baetzo_Zoneinfo::findTransitionForUtcTime(const bdet_Datetime& utcTime) const
{
    BSLS_ASSERT_SAFE(numTransitions() > 0);
    BSLS_ASSERT_SAFE(d_transitions.front().utcTime() <=
                                       bdetu_Epoch::convertToTimeT64(utcTime));

    baetzo_LocalTimeDescriptor dummyDescriptor;

    const bdetu_Epoch::TimeT64 utcTimeT64 =
                                        bdetu_Epoch::convertToTimeT64(utcTime);
    TransitionConstIterator it = bsl::upper_bound(
                                     d_transitions.begin(),
                                     d_transitions.end(),
                                     baetzo_ZoneinfoTransition(
                                                            utcTimeT64,
                                                            &dummyDescriptor));

    if (d_transitions.begin() != it) {
        --it;
    }

    return it;
}

bsl::ostream& baetzo_Zoneinfo::print(bsl::ostream& stream,
                                     int           level,
                                     int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bdeu_Print::indent(stream, level, spacesPerLevel);

    stream << "[";

    if (level < 0) {
        level = -level;
    }

    ++level;

    bdeu_Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << "identifier = " << '"' << d_identifier << '"';

    const char SEP = 0 <= spacesPerLevel ? '\n' : ' ';

    bdeu_Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << "transitions = [" << SEP;

    TransitionConstIterator itT = d_transitions.begin();
    for ( ; itT != d_transitions.end(); ++itT) {
        if (spacesPerLevel >= 0) {
            bdeu_Print::indent(stream, level, spacesPerLevel);
        }
        itT->print(stream, -level, spacesPerLevel);
        if (spacesPerLevel < 0) {
            stream << ' ';
        }
    }

    if (spacesPerLevel >= 0) {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }
    stream << "]";

    bdeu_Print::newlineAndIndent(stream, level - 1, spacesPerLevel);
    stream << "]";

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&          stream,
                         const baetzo_Zoneinfo& object)
{
    stream << "[ \"" << object.identifier() << "\" [";

    baetzo_Zoneinfo::TransitionConstIterator it  = object.beginTransitions();
    baetzo_Zoneinfo::TransitionConstIterator end = object.endTransitions();
    for ( ; it != end; ++it) {
        stream << ' ' << *it;
    }
    stream << " ] ]";

    return stream;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
