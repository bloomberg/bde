// bdlmxxx_enumerationdef.cpp                                         -*-C++-*-
#include <bdlmxxx_enumerationdef.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmxxx_enumerationdef_cpp,"$Id$ $CSID$")

#include <bdlma_sequentialallocator.h>
#include <bdltuxxx_unset.h>
#include <bdlb_print.h>
#include <bdlb_string.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bsls_assert.h>

#include <bsl_cstring.h>
#include <bsl_functional.h>
#include <bsl_ostream.h>

namespace BloombergLP {

namespace {

                        // =======================
                        // local class AutoEraseId
                        // =======================

template <class Container>
class AutoEraseEntry {
    // This class provides a specialized proctor object that, upon destruction,
    // erases a managed entry from a managed container.

    typedef          Container           ManagedContainerType;
    typedef typename Container::iterator ManagedIteratorType;

    ManagedContainerType *d_container_p;  // managed container
    ManagedIteratorType   d_iter;         // iterator to managed entry

  public:
    AutoEraseEntry()
        // Create a proctor that initially manages no objects.
    : d_container_p(0)
    {
    }

    ~AutoEraseEntry()
        // If any object is under management by this proctor, then erase the
        // managed entry from the managed container; otherwise do nothing.
    {
        if (d_container_p) {
            d_container_p->erase(d_iter);
        }
    }

    void manage(ManagedContainerType *container, ManagedIteratorType iter)
        // Set this proctor to manage the specified 'container' and the entry
        // referenced by the specified 'iter'.  Any object currently under
        // management by this proctor is released from management.  The
        // behavior is undefined unless 'iter' is an iterator belonging to
        // 'container'.
    {
        d_container_p = container;
        d_iter        = iter;
    }

    void release()
        // Release from management any object managed by this proctor.
    {
        d_container_p = 0;
    }
};

}  // close unnamed namespace

namespace bdlmxxx {
                     // -------------------------
                     // class EnumerationDef
                     // -------------------------

// CLASS METHODS
bool EnumerationDef::areEquivalent(const EnumerationDef& lhs,
                                        const EnumerationDef& rhs)
{
    if (lhs.numEnumerators() != rhs.numEnumerators()) {
        return false;                                                 // RETURN
    }

    IntCharptrMap::const_iterator lhsIter = lhs.d_enumByIdMap.begin();
    IntCharptrMap::const_iterator rhsIter = rhs.d_enumByIdMap.begin();
    while (lhsIter != lhs.d_enumByIdMap.end()) {
        if (lhsIter->first != rhsIter->first
         || bsl::strcmp(lhsIter->second, rhsIter->second)) {
            return false;                                             // RETURN
        }
        ++lhsIter;
        ++rhsIter;
    }

    return true;
}

bool EnumerationDef::isSuperset(const EnumerationDef& super,
                                     const EnumerationDef& sub)
{
    if (super.numEnumerators() < sub.numEnumerators()) {
        // cheap and somewhat likely
        return false;                                                 // RETURN
    }

    IntCharptrMap::const_iterator subIter   = sub.d_enumByIdMap.begin();
    IntCharptrMap::const_iterator superIter = super.d_enumByIdMap.begin();
    while (subIter   != sub.d_enumByIdMap.end()
        && superIter != super.d_enumByIdMap.end()) {

        // Loop until we find a match or hit the end of super.  If we hit the
        // end of super return false.
        while (subIter->first > superIter->first) {
            ++superIter;

            if (superIter == super.d_enumByIdMap.end()) {
                return false;                                         // RETURN
            }
        }

        if (subIter->first != superIter->first
         || bsl::strcmp(subIter->second, superIter->second)) {
            return false;                                             // RETURN
        }

        ++subIter;
        ++superIter;
    }

    return subIter == sub.d_enumByIdMap.end();
}

// CREATORS
EnumerationDef::EnumerationDef(
                                 Schema               *schema,
                                 int                        index,
                                 const char                *name,
                                 bdlma::SequentialAllocator *writeOnceAllocator)
: d_schema_p(schema)
, d_enumIndex(index)
, d_name_p(name)
, d_enumByNameMap(bdlb::CStringLess(), writeOnceAllocator)
, d_enumByIdMap(bsl::less<int>(), writeOnceAllocator)
, d_maxId(INT_MIN)
{
}

EnumerationDef::~EnumerationDef()
{
}

// MANIPULATORS
int EnumerationDef::addEnumerator(const char *name, int id)
{
    if (0 == name || bdltuxxx::Unset<int>::isUnset(id)) {
        // Invalid name or id.
        return bdltuxxx::Unset<int>::unsetValue();                    // RETURN
    }

    // Check for duplicate name or Id
    if (d_enumByIdMap.end()   != d_enumByIdMap.find(id)
     || d_enumByNameMap.end() != d_enumByNameMap.find(name)) {
        // Insert failed -- duplicate.
        return bdltuxxx::Unset<int>::unsetValue();                    // RETURN
    }

    // Make a private, permanent copy of the enumerator name.
    name = bdlb::String::copy(name, writeOnceAllocator());

    // Insert the (id, name) pair into 'd_enumByIdMap'.  If insert fails,
    // return an error value.
    bsl::pair<IntCharptrMap::iterator, bool> idInsertResult;
    idInsertResult = d_enumByIdMap.insert(IntCharptrMap::value_type(id, name));
    if (! idInsertResult.second) {
        // Insert failed.
        return bdltuxxx::Unset<int>::unsetValue();                    // RETURN
    }

    // Proctor last insertion -- will undo insertion if next operation fails
    // or throws an exception.
    AutoEraseEntry<IntCharptrMap> idProctor;
    idProctor.manage(&d_enumByIdMap, idInsertResult.first);

    // Insert the (name, id) pair into 'd_enumByNameMap'.  If insert fails,
    // return an error value (the 'idProctor' will undo the previous insert).
    if (! d_enumByNameMap.insert(CharptrIntMap::value_type(name, id)).second) {
        // Insert failed.
        return bdltuxxx::Unset<int>::unsetValue();                    // RETURN
    }

    if (id > d_maxId) {
        d_maxId = id;
    }

    idProctor.release();  // Commit insertion.
    return id;
}

void EnumerationDef::alphabetize(int firstId)
{
    IntCharptrMap::iterator idIter   = d_enumByIdMap.begin();
    CharptrIntMap::iterator nameIter = d_enumByNameMap.begin();

    // Iterate through the ID and name maps, giving each entry a sequential id.
    int enumeratorId = firstId;
    while (idIter != d_enumByIdMap.end()) {

        const char *enumeratorName = nameIter->first;

        // Re-assign both the ID and name in the ID map.  Re-assigning the key
        // of a map is generally not recommended. (That is why it is 'const'.)
        // However, the following is safe because we are traversing the keys
        // in ascending order and re-assigning them values that are also
        // ascending, hence preserving the relationship between keys.
        const_cast<int&>(idIter->first) = enumeratorId;
        idIter->second                  = enumeratorName;

        // 'nameIter->first' is already set to 'enumeratorName'.
        nameIter->second = enumeratorId;

        ++enumeratorId;
        ++idIter;
        ++nameIter;
    }

    d_maxId = enumeratorId - 1;
}

// ACCESSORS
const char *EnumerationDef::lookupName(int id) const
{
    IntCharptrMap::const_iterator found = d_enumByIdMap.find(id);

    return found != d_enumByIdMap.end() ? found->second : 0;
}

const char *EnumerationDef::lookupName(const char *name) const
{
    if (0 == name) {
        return 0;                                                     // RETURN
    }

    CharptrIntMap::const_iterator found = d_enumByNameMap.find(name);

    return found != d_enumByNameMap.end() ? found->first : 0;
}

int EnumerationDef::lookupId(const char *name) const
{
    if (0 == name) {
        // Invalid name.
        return bdltuxxx::Unset<int>::unsetValue();                    // RETURN
    }

    CharptrIntMap::const_iterator found = d_enumByNameMap.find(name);
    return found != d_enumByNameMap.end() ? found->second
                                          : bdltuxxx::Unset<int>::unsetValue();
}

bsl::pair<const char *, int>
EnumerationDef::nextLargerName(const char *name) const
{
    CharptrIntMap::const_iterator found;

    if (0 == name) {
        // Invalid name.  Return first enumerator.
        found = d_enumByNameMap.begin();
    }
    else {
        found = d_enumByNameMap.lower_bound(name);
        if (found != d_enumByNameMap.end()
         && 0 == bsl::strcmp(found->first, name)) {
            // Found exact match.  Return enumerator AFTER this match.
            ++found;
        }
    }

    if (found != d_enumByNameMap.end()) {
        return *found;                                                // RETURN
    }

    return bsl::make_pair((const char *)0, bdltuxxx::Unset<int>::unsetValue());
}

bsl::pair<const char *, int>
EnumerationDef::nextLargerId(int id) const
{
    bsl::pair<const char *, int> ret(0, bdltuxxx::Unset<int>::unsetValue());

    IntCharptrMap::const_iterator found = d_enumByIdMap.lower_bound(id);
    if (found != d_enumByIdMap.end() && found->first == id) {
        // Found exact match.  Return enumerator AFTER this match.
        ++found;
    }

    if (found != d_enumByIdMap.end()) {
        ret.first  = found->second;
        ret.second = found->first;
    }
    return ret;
}

bsl::ostream&
EnumerationDef::print(bsl::ostream& stream,
                           int           level,
                           int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    bdlb::Print::indent(stream, level, spacesPerLevel);
    stream << "ENUMERATION ";

    if (d_name_p) {
        stream << "\"" << d_name_p << "\" ";
    }

    if (level < 0) {
        level = -level;
    }

    stream << "{";

    IntCharptrMap::const_iterator enumIter = d_enumByIdMap.begin();
    while (enumIter != d_enumByIdMap.end()) {
        bdlb::Print::newlineAndIndent(stream, level + 1, spacesPerLevel);

        stream << '"' << enumIter->second << "\" = " << enumIter->first;
        ++enumIter;
    }

    bdlb::Print::newlineAndIndent(stream, level, spacesPerLevel);
    stream << "}";
    if (spacesPerLevel >= 0) {
        stream << '\n';
    }
    return stream;
}
}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
