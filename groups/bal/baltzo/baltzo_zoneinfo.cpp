// baltzo_zoneinfo.cpp                                                -*-C++-*-

#include <baltzo_zoneinfo.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(baltzo_zoneinfo_cpp,"$Id$ $CSID$")

#include <bdlb_print.h>

#include <bdlt_epochutil.h>
#include <bdlt_time.h>
#include <bdlt_timeunitratio.h>

#include <bsls_assert.h>
#include <bsls_review.h>

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

                          // ------------------------
                          // class ZoneinfoTransition
                          // ------------------------

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
    int rc = bdlt::EpochUtil::convertFromTimeT64(&utcDatetime, d_utcTime);
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
bsl::ostream& baltzo::operator<<(bsl::ostream&             stream,
                                 const ZoneinfoTransition& object)
{
    stream << "[ ";

    bdlt::Datetime utcDatetime;
    int rc = bdlt::EpochUtil::convertFromTimeT64(&utcDatetime,
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

namespace baltzo {

                       // ------------------------------
                       // class Zoneinfo::DescriptorLess
                       // ------------------------------

// ACCESSORS
bool Zoneinfo::DescriptorLess::operator()(const LocalTimeDescriptor& lhs,
                                          const LocalTimeDescriptor& rhs) const
{
    if (lhs.utcOffsetInSeconds() != rhs.utcOffsetInSeconds()) {
        return lhs.utcOffsetInSeconds() < rhs.utcOffsetInSeconds();   // RETURN
    }

    if (lhs.description() != rhs.description()) {
        return lhs.description() < rhs.description();                 // RETURN
    }

    return lhs.dstInEffectFlag() < rhs.dstInEffectFlag();
}

                               // --------------
                               // class Zoneinfo
                               // --------------

#ifndef BDE_OPENSOURCE_PUBLICATION
// CLASS METHODS
bdlt::EpochUtil::TimeT64 Zoneinfo::convertToTimeT64(
                                                const bdlt::Datetime& datetime)
{
    return bdlt::EpochUtil::convertToTimeT64(datetime);
}

int Zoneinfo::convertFromTimeT64(bdlt::Datetime           *result,
                                 bdlt::EpochUtil::TimeT64  time)
{
    return bdlt::EpochUtil::convertFromTimeT64(result, time);
}
#endif

// CREATORS
Zoneinfo::Zoneinfo(const Zoneinfo& original, const allocator_type &allocator)
: d_identifier(original.d_identifier, allocator)
, d_descriptors(original.d_descriptors, allocator)
, d_transitions(allocator)
, d_posixExtendedRangeDescription(original.d_posixExtendedRangeDescription,
                                  allocator)
{
    d_transitions.reserve(original.d_transitions.size());

    TransitionConstIterator it  = original.d_transitions.begin();
    TransitionConstIterator end = original.d_transitions.end();
    for (; it != end; ++it) {
        addTransition(it->utcTime(), it->descriptor());
    }
}

Zoneinfo::Zoneinfo(bslmf::MovableRef<Zoneinfo> original) BSLS_KEYWORD_NOEXCEPT
: d_identifier(bslmf::MovableRefUtil::move(
      bslmf::MovableRefUtil::access(original).d_identifier))
, d_descriptors(bslmf::MovableRefUtil::move(
      bslmf::MovableRefUtil::access(original).d_descriptors))
, d_transitions(bslmf::MovableRefUtil::move(
      bslmf::MovableRefUtil::access(original).d_transitions))
, d_posixExtendedRangeDescription(bslmf::MovableRefUtil::move(
      bslmf::MovableRefUtil::access(original).d_posixExtendedRangeDescription))
{
}

Zoneinfo::Zoneinfo(bslmf::MovableRef<Zoneinfo> original,
                   const allocator_type&       allocator)
: d_identifier(bslmf::MovableRefUtil::move(
                   bslmf::MovableRefUtil::access(original).d_identifier),
               allocator)
, d_descriptors(allocator)
, d_transitions(allocator)
, d_posixExtendedRangeDescription(
      bslmf::MovableRefUtil::move(bslmf::MovableRefUtil::access(original)
                                      .d_posixExtendedRangeDescription),
      allocator)
{
    const Zoneinfo& origRef = bslmf::MovableRefUtil::access(original);

    d_transitions.reserve(origRef.d_transitions.size());

    TransitionConstIterator it  = origRef.d_transitions.begin();
    TransitionConstIterator end = origRef.d_transitions.end();
    for (; it != end; ++it) {
        addTransition(it->utcTime(), it->descriptor());
    }
}

// MANIPULATORS
Zoneinfo& Zoneinfo::operator=(bslmf::MovableRef<Zoneinfo> rhs)
{
    Zoneinfo &rhsRef = bslmf::MovableRefUtil::access(rhs);

    if (get_allocator() != rhsRef.get_allocator()) {
        // It's non-trivial to handle the possible exception cases when the
        // allocators don't match, so we'll defer to the op=(const&).

        *this = rhsRef;
        return *this;                                                 // RETURN
    }

    d_identifier  = bslmf::MovableRefUtil::move(rhsRef.d_identifier);
    d_descriptors = bslmf::MovableRefUtil::move(rhsRef.d_descriptors);
    d_transitions = bslmf::MovableRefUtil::move(rhsRef.d_transitions);

    d_posixExtendedRangeDescription =
           bslmf::MovableRefUtil::move(rhsRef.d_posixExtendedRangeDescription);

    return *this;
}

void Zoneinfo::addTransition(bdlt::EpochUtil::TimeT64   utcTime,
                             const LocalTimeDescriptor& descriptor)
{
    typedef bsl::vector<ZoneinfoTransition>::iterator TransitionIterator;

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
        BSLS_ASSERT(descIt != d_descriptors.end());

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
    BSLS_ASSERT(numTransitions() > 0);
    BSLS_ASSERT(d_transitions.front().utcTime() <=
                                   bdlt::EpochUtil::convertToTimeT64(utcTime));

    LocalTimeDescriptor dummyDescriptor;

    const bdlt::EpochUtil::TimeT64 utcTimeT64 =
                                    bdlt::EpochUtil::convertToTimeT64(utcTime);
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

    bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << "TZ = " << '"' << d_posixExtendedRangeDescription << '"';

    bdlb::Print::newlineAndIndent(stream, level - 1, spacesPerLevel);
    stream << "]";

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

}  // close package namespace

// FREE OPERATORS
bsl::ostream& baltzo::operator<<(bsl::ostream& stream, const Zoneinfo& object)
{
    stream << "[ \"" << object.identifier() << "\" "
           <<   "\"" << object.posixExtendedRangeDescription() << "\" [";

    Zoneinfo::TransitionConstIterator it  = object.beginTransitions();
    Zoneinfo::TransitionConstIterator end = object.endTransitions();
    for ( ; it != end; ++it) {
        stream << ' ' << *it;
    }
    stream << " ] ]";

    return stream;
}

// FREE FUNCTIONS
void baltzo::swap(Zoneinfo& a, Zoneinfo& b)
{
    if (a.get_allocator() == b.get_allocator()) {
        a.swap(b);

        return;                                                       // RETURN
    }

    Zoneinfo futureA(b, a.get_allocator());
    Zoneinfo futureB(a, b.get_allocator());

    futureA.swap(a);
    futureB.swap(b);
}

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
