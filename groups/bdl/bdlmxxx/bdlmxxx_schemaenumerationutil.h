// bdlmxxx_schemaenumerationutil.h                                       -*-C++-*-
#ifndef INCLUDED_BDLMXXX_SCHEMAENUMERATIONUTIL
#define INCLUDED_BDLMXXX_SCHEMAENUMERATIONUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utility functions on schema/enum combinations.
//
//@CLASSES:
//  bdlmxxx::SchemaEnumerationUtil: namespace for schema/enum utilities
//
//@AUTHOR: Bill Chapman (bchapman)
//
//@DESCRIPTION: This component provides a set of utility functions that operate
// on two arguments, the first being an object of one of four types from the
// 'bdlmxxx_elemtype' component: 'INT', 'STRING', 'INT_ARRAY', or 'STRING_ARRAY',
// the second being a 'bdlmxxx::EnumerationDef' (defined by the component
// 'bdlmxxx_schema').  In particular, this component provides methods for
// verifying that 'int's, strings, 'int' arrays, and string arrays conform to
// the meta-data in a 'bdlmxxx::EnumerationDef'.
//
///Definition
///----------
// A 'bdlmxxx::EnumerationDef' contains a set of int-string pairs, where each int
// is unique and each string is unique.  To be conformant to an enumeration,
// an int must be either unset or have a value that equals one of the int
// values in the pairs.  Similarly, for a string to be conformant, it must
// either be unset or have one of the string values among the values in the
// enumeration's int-string pairs.
//
///Usage
///-----
// Here we demonstrate the circumstances in which ints, strings, int arrays
// and string arrays are conformant to an enumeration and those circumstances
// in which they are not:
//..
//    typedef bdlmxxx::SchemaEnumerationUtil Util;
//
//    const int unsetInt       = bdltuxxx::Unset<int>::unsetValue();
//    const string unsetString = bdltuxxx::Unset<string>::unsetValue();
//    const vector<int>    unsetIntArray;
//    const vector<string> unsetStringArray;
//
//    bdlmxxx::Schema mS;
//
//    bdlmxxx::EnumerationDef& mE = *mS.createEnumeration();
//    const bdlmxxx::EnumerationDef& E = mE;
//
//    // Unset values are conformant to ANY enumeration.
//    assert(1 == Util::isConformant(unsetInt,            E));
//    assert(1 == Util::isConformant(unsetString,         E));
//    assert(1 == Util::isConformant(unsetString.c_str(), E));
//    assert(1 == Util::isConformant(unsetIntArray,       E));
//    assert(1 == Util::isConformant(unsetStringArray,    E));
//
//    vector<int> intArray;          intArray.push_back(5);
//    vector<string> stringArray;    stringArray.push_back("a");
//
//    // No set value is conformant to an empty enumeration.
//    assert(0 == Util::isConformant(5,           E));
//    assert(0 == Util::isConformant(string("a"), E));
//    assert(0 == Util::isConformant("a",         E));
//    assert(0 == Util::isConformant(intArray,    E));
//    assert(0 == Util::isConformant(stringArray, E));
//
//    mE.addEnumerator("a", 5);
//
//    // If the value is contained in one of the string-int pairs, it's
//    // conformant.
//    assert(1 == Util::isConformant(5,           E));
//    assert(1 == Util::isConformant(string("a"), E));
//    assert(1 == Util::isConformant("a",         E));
//    assert(1 == Util::isConformant(intArray,    E));
//    assert(1 == Util::isConformant(stringArray, E));
//
//    mE.addEnumerator("b",  7);
//    mE.addEnumerator("c", 10);
//    mE.addEnumerator("f", 15);
//
//    intArray.push_back(10);
//    stringArray.push_back("f");
//
//    // Here's another conformant example.
//    assert(1 == Util::isConformant(10,          E));
//    assert(1 == Util::isConformant(string("f"), E));
//    assert(1 == Util::isConformant("f",         E));
//    assert(1 == Util::isConformant(intArray,    E));
//    assert(1 == Util::isConformant(stringArray, E));
//
//    intArray.push_back(8);
//    stringArray.push_back("e");
//
//    // Values not contained in the enumeration are not conformant.
//    assert(0 == Util::isConformant(8,           E));
//    assert(0 == Util::isConformant(string("e"), E));
//    assert(0 == Util::isConformant("e",         E));
//    assert(0 == Util::isConformant(intArray,    E));
//    assert(0 == Util::isConformant(stringArray, E));
//
//    // Again, unset values are conformant to ANY enumeration.
//    assert(1 == Util::isConformant(unsetInt,            E));
//    assert(1 == Util::isConformant(unsetString,         E));
//    assert(1 == Util::isConformant(unsetString.c_str(), E));
//    assert(1 == Util::isConformant(unsetIntArray,       E));
//    assert(1 == Util::isConformant(unsetStringArray,    E));
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMXXX_SCHEMA
#include <bdlmxxx_schema.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif


namespace BloombergLP {

namespace bdlmxxx {
                       // =================================
                       // struct SchemaEnumerationUtil
                       // =================================

struct SchemaEnumerationUtil {
    // This 'class' provides a namespace for utility functions that operate on
    // two arguments, the first being an int, string, vector of ints
    // ('INT_ARRAY'), or vector of strings ('STRING_ARRAY'), and the second
    // being a 'EnumerationDef'.

    // CLASS METHODS
    static
    bool isConformant(int                        value,
                      const EnumerationDef& enumeration);
        // Return 'true' if the specified 'value' has either the unset value or
        // one of the values contained in the specified 'enumeration', and
        // 'false' otherwise.

    static
    bool isConformant(const bsl::string&         value,
                      const EnumerationDef& enumeration);
        // Return 'true' if the specified 'value' has either the unset value or
        // one of the values contained in the specified 'enumeration', and
        // 'false' otherwise.

    static
    bool isConformant(const bsl::vector<int>&    value,
                      const EnumerationDef& enumeration);
        // Return 'true' if each 'int' in the specified array 'value' is
        // conformant to the specified 'enumeration', and 'false' otherwise.
        // Note that if 'value' is an empty vector this method always returns
        // 'true'.

    static
    bool isConformant(const bsl::vector<bsl::string>& value,
                      const EnumerationDef&      enumeration);
        // Return 'true' if each string in the specified array 'value' is
        // conformant to the specified 'enumeration', and 'false' otherwise.
        // Note that if 'value' is an empty vector this method always returns
        // 'true'.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                       // ---------------------------------
                       // struct SchemaEnumerationUtil
                       // ---------------------------------

// CLASS METHODS
inline
bool SchemaEnumerationUtil::isConformant(
                                        int                        value,
                                        const EnumerationDef& enumeration)
{
    // Explicitly use 'INT_MIN' here instead of
    // 'bdltuxxx::Unset<int>::unsetValue()' to avoid a function call or a
    // statically-initialized constant.

    return INT_MIN == value || enumeration.lookupName(value);
}

inline
bool SchemaEnumerationUtil::isConformant(
                                        const bsl::string&         value,
                                        const EnumerationDef& enumeration)
{
    const char *pc = value.c_str();

    return !pc[0] || enumeration.lookupName(pc);
}
}  // close package namespace

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
