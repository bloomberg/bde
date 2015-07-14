// baltzo_zoneinfo.cpp                                                -*-C++-*-

#include <baltzo_zoneinfo.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baltzo_zoneinfo_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>

#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_ostream.h>

namespace BloombergLP {

// STATIC HELPER FUNCTIONS
static
bool containsDescriptor(
                     const bsl::vector<baltzo::ZoneinfoTransition>& transitions,
                     const baltzo::LocalTimeDescriptor&             descriptor)
    // Return 'true' if the specified 'transitions' contain a transition with
    // the specified 'descriptor', and 'false' otherwise.
{
    baltzo::Zoneinfo::TransitionConstIterator it  = transitions.begin();
    baltzo::Zoneinfo::TransitionConstIterator end = transitions.end();
    for (; it != end; ++it) {
        if (descriptor == it->descriptor()) {
            return true;                                              // RETURN
        }
    }
    return false;
}

namespace baltzo {
                        // -------------------------------
                        // class ZoneinfoTransition
                        // -------------------------------

// ACCESSORS
bsl::ostream&
ZoneinfoTransition::print(bsl::ostream& stream,
                                 int           level,
                                 int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bdlb::Print::indent(stream, level, spacesPerLevel);

    stream << "[";

    if (level < 0) {
        level = -level;
    }

    ++level;

    bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);

    bdlt::Datetime utcDatetime;
    int rc = Zoneinfo::convertFromTimeT64(&utcDatetime, d_utcTime);
    stream << "time = ";
    if (!rc) {
        stream << utcDatetime;
    }
    else {
        stream << d_utcTime;
    }

    bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << "descriptor = ";
    d_descriptor_p->print(stream, -level, spacesPerLevel);

    if (spacesPerLevel < 0) {
        stream << ' ';
    }
    else {
        bdlb::Print::indent(stream, level - 1, spacesPerLevel);
    }

    stream << "]";

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}
}  // close package namespace

// FREE OPERATORS
bsl::ostream& baltzo::operator<<(bsl::ostream&                    stream,
                         const ZoneinfoTransition& object)
{
    stream << "[ ";

    bdlt::Datetime utcDatetime;
    int rc = Zoneinfo::convertFromTimeT64(&utcDatetime, 
                                                 object.utcTime());
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
                        // class baltzo::Zoneinfo::DescriptorLess
                        // -------------------------------------

// ACCESSORS
bool baltzo::Zoneinfo::DescriptorLess::operator()(
                                   const baltzo::LocalTimeDescriptor& lhs,
                                   const baltzo::LocalTimeDescriptor& rhs) const
{
    if (lhs.utcOffsetInSeconds() != rhs.utcOffsetInSeconds()) {
        return lhs.utcOffsetInSeconds() < rhs.utcOffsetInSeconds();   // RETURN
    }

    if (lhs.description() != rhs.description()) {
        return lhs.description() < rhs.description();                 // RETURN
    }

    return lhs.dstInEffectFlag() < rhs.dstInEffectFlag();
}

namespace baltzo {
                        // ---------------------
                        // class Zoneinfo
                        // ---------------------

// CLASS METHODS
bdlt::EpochUtil::TimeT64 Zoneinfo::convertToTimeT64(
                                                const bdlt::Datetime& datetime)
{
    int elaspedDays = 
        bdlt::DelegatingDateImpUtil::ymdToSerial(datetime.year(), 
                                                 datetime.month(),
                                                 datetime.day()) -
        bdlt::DelegatingDateImpUtil::ymdToSerial(1970, 1, 1);

    return elaspedDays * bdlt::TimeUnitRatio::k_SECONDS_PER_DAY + 
        (datetime.hour() * 60 + datetime.minute()) * 60 + datetime.second();
}

int Zoneinfo::convertFromTimeT64(bdlt::Datetime           *result,
                                        bdlt::EpochUtil::TimeT64  time)
{
    typedef bdlt::DelegatingDateImpUtil DateUtil;
    if (( DateUtil::isProlepticGregorianMode() && -62135596800LL > time) ||
        (!DateUtil::isProlepticGregorianMode() && -62135769600LL > time) ||
        253402300799LL < time) {  // December  31, 9999 23:59:59
        return 1;                                                     // RETURN
    }

    time += bdlt::DelegatingDateImpUtil::ymdToSerial(1970, 1, 1) *
            bdlt::TimeUnitRatio::k_SECONDS_PER_DAY;

    int serialDate = int(time / bdlt::TimeUnitRatio::k_SECONDS_PER_DAY);
    int seconds    = int(time % bdlt::TimeUnitRatio::k_SECONDS_PER_DAY);

    int year, month, day;
    bdlt::DelegatingDateImpUtil::serialToYmd(&year, &month, &day, serialDate);
    *result = bdlt::Datetime(
        bdlt::Date(year, month, day),
        bdlt::Time(0) + bdlt::DatetimeInterval(0, 0, 0,seconds));
    return 0;
}



// CREATORS
Zoneinfo::Zoneinfo(const Zoneinfo&  original,
                                 bslma::Allocator       *basicAllocator)
: d_identifier(original.d_identifier, basicAllocator)
, d_descriptors(original.d_descriptors, basicAllocator)
, d_transitions(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    TransitionConstIterator it  = original.d_transitions.begin();
    TransitionConstIterator end = original.d_transitions.end();
    for (; it != end; ++it) {
        addTransition(it->utcTime(), it->descriptor());
    }
}

// MANIPULATORS
void Zoneinfo::addTransition(
                                  bdlt::EpochUtil::TimeT64              utcTime,
                                  const LocalTimeDescriptor& descriptor)
{
    typedef bsl::vector<ZoneinfoTransition>::iterator
                                                            TransitionIterator;

    // Insert the description in the set and get back an iterator pointing to
    // the inserted item.

    DescriptorSet::iterator descriptorIterator =
                                        d_descriptors.insert(descriptor).first;

    ZoneinfoTransition newTransition(utcTime, &(*descriptorIterator));

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
Zoneinfo::TransitionConstIterator
Zoneinfo::findTransitionForUtcTime(const bdlt::Datetime& utcTime) const
{
    BSLS_ASSERT_SAFE(numTransitions() > 0);
    BSLS_ASSERT_SAFE(d_transitions.front().utcTime() <=
                                                    convertToTimeT64(utcTime));

    LocalTimeDescriptor dummyDescriptor;

    const bdlt::EpochUtil::TimeT64 utcTimeT64 = convertToTimeT64(utcTime);
    TransitionConstIterator it = bsl::upper_bound(
                                     d_transitions.begin(),
                                     d_transitions.end(),
                                     ZoneinfoTransition(
                                                            utcTimeT64,
                                                            &dummyDescriptor));

    if (d_transitions.begin() != it) {
        --it;
    }

    return it;
}

bsl::ostream& Zoneinfo::print(bsl::ostream& stream,
                                     int           level,
                                     int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bdlb::Print::indent(stream, level, spacesPerLevel);

    stream << "[";

    if (level < 0) {
        level = -level;
    }

    ++level;

    bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << "identifier = " << '"' << d_identifier << '"';

    const char SEP = 0 <= spacesPerLevel ? '\n' : ' ';

    bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << "transitions = [" << SEP;

    TransitionConstIterator itT = d_transitions.begin();
    for ( ; itT != d_transitions.end(); ++itT) {
        if (spacesPerLevel >= 0) {
            bdlb::Print::indent(stream, level, spacesPerLevel);
        }
        itT->print(stream, -level, spacesPerLevel);
        if (spacesPerLevel < 0) {
            stream << ' ';
        }
    }

    if (spacesPerLevel >= 0) {
        bdlb::Print::indent(stream, level, spacesPerLevel);
    }
    stream << "]";

    bdlb::Print::newlineAndIndent(stream, level - 1, spacesPerLevel);
    stream << "]";

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}
}  // close package namespace

// FREE OPERATORS
bsl::ostream& baltzo::operator<<(bsl::ostream&          stream,
                         const Zoneinfo& object)
{
    stream << "[ \"" << object.identifier() << "\" [";

    Zoneinfo::TransitionConstIterator it  = object.beginTransitions();
    Zoneinfo::TransitionConstIterator end = object.endTransitions();
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
