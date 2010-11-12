// bael_severityutil.h              -*-C++-*-
#ifndef INCLUDED_BAEL_SEVERITYUTIL
#define INCLUDED_BAEL_SEVERITYUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a suite of utility functions on 'bael_Severity' levels.
//
//@CLASSES:
//   bael_SeverityUtil: namespace for pure procedures on 'bael_Severity::Level'
//
//@AUTHOR: Steven Breitstein (sbreitst), Henry Verschell (hversche)
//
//@SEE_ALSO: bael_severity
//
//@DESCRIPTION: This component provides a suite of pure procedures that apply
// to the 'bael_Severity::Level' enumeration.  In particular, the
// 'bael_SeverityUtil' 'struct' provides a 'fromAsciiCaseless' function that
// returns the 'bael_Severity::Level' enumerator value corresponding to a
// given ASCII string (without regard to the case of the characters in the
// string) and an 'isValidNameCaseless' function that confirms that a given
// string corresponds to one of the enumerators in the 'bael_Severity::Level'
// enumeration (similarly, without regard to the case of the characters in the
// string).
//
///Synopsis
///--------
// The following is a list of functions available in this component:
//..
//    static int  bael_SeverityUtil::fromAsciiCaseless(
//                                              bael_Severity::Level *level,
//                                              const char           *name);
//
//    static bool bael_SeverityUtil::isValidNameCaseless(const char *name);
//..
///Usage
///-----
// In this example, we show how to validate that a set of C-style strings
// correspond to 'bael_Severity::Level' enumerators, and then use those strings
// to generate enumerator values that, in turn, may be used to administer a
// logger manager.  Here, for convenience, we define our strings in an array,
// much as how we might receive them from a command line:
//..
//    const char *argv[] = {
//        "INFO",   // record
//        "WARN",   // pass
//        "ERROR",  // trigger
//        "FATAL"   // trigger-all
//    };
//
//    assert(bael_SeverityUtil::isValidNameCaseless(argv[0]));
//    assert(bael_SeverityUtil::isValidNameCaseless(argv[1]));
//    assert(bael_SeverityUtil::isValidNameCaseless(argv[2]));
//    assert(bael_SeverityUtil::isValidNameCaseless(argv[3]));
//
//    bael_Severity::Level record;
//    bael_Severity::Level pass;
//    bael_Severity::Level trigger;
//    bael_Severity::Level triggerAll;
//
//    assert(0 == bael_SeverityUtil::fromAsciiCaseless(&record,     argv[0]));
//    assert(0 == bael_SeverityUtil::fromAsciiCaseless(&pass,       argv[1]));
//    assert(0 == bael_SeverityUtil::fromAsciiCaseless(&trigger,    argv[2]));
//    assert(0 == bael_SeverityUtil::fromAsciiCaseless(&triggerAll, argv[3]));
//
//    assert(bael_Severity::BAEL_INFO  == record);
//    assert(bael_Severity::BAEL_WARN  == pass);
//    assert(bael_Severity::BAEL_ERROR == trigger);
//    assert(bael_Severity::BAEL_FATAL == triggerAll);
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEL_SEVERITY
#include <bael_severity.h>
#endif

namespace BloombergLP {

                        // ========================
                        // struct bael_SeverityUtil
                        // ========================

struct bael_SeverityUtil {
    // This 'struct' provides a namespace for non-primitive procedures on the
    // 'bael_Severity::Level' enumeration.

    // CLASS METHODS
    static int fromAsciiCaseless(bael_Severity::Level *level,
                                 const char           *name);
        // Load into the specified 'level' the value of the
        // 'bael_Severity::Level' enumerator corresponding to the specified
        // 'name' (without regard to the case of the characters in 'name').
        // Return 0 on success, and a non-zero value with no effect on
        // 'level' otherwise.  The behavior is undefined unless 'level' is
        // non-null and 'name' is a null-terminated (C-style) string.  Note
        // that this procedure will fail unless true == isValidName(name).

    static bool isValidNameCaseless(const char *name);
        // Return 'true' if the specified 'name' corresponds to an enumerator
        // in the 'bael_Severity::Level' enumeration (without regard to the
        // characters in 'name'), and 'false' otherwise.  The behavior is
        // undefined unless 'name' is a null-terminated (C-style) string.
        // Note that the names corresponding to 'bael_Severity::Level'
        // enumerators are case *insensitive*.

    static int fromAscii(bael_Severity::Level *level, const char *name);
        // Load into the specified 'level' the value of the
        // 'bael_Severity::Level' enumerator corresponding to the specified
        // 'name' (without regard to the case of the characters in 'name').
        // Return 0 on success, and a non-zero value with no effect on
        // 'level' otherwise.  The behavior is undefined unless 'level' is
        // non-null and 'name' is a null-terminated (C-style) string.  Note
        // that this procedure will fail unless true == isValidName(name).
        //
        // DEPRECATED: Use 'fromAsciiCaseless'.  Note that this method
        // provides backwards compatibility for clients of the deprecated
        // 'baelu_Severity'.

    static bool isValidName(const char *name);
        // Return 'true' if the specified 'name' corresponds to an enumerator
        // in the 'bael_Severity::Level' enumeration, and 'false' otherwise.
        // The behavior is undefined unless 'name' is a null-terminated
        // (C-style) string.  Note that the names corresponding to
        // 'bael_Severity::Level' enumerators are case *insensitive*.
        //
        // DEPRECATED: Use 'isValidNameCaseless'.  Note that this method
        // provides backwards compatibility for clients of the deprecated
        // 'baelu_Severity'.
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ------------------------
                        // struct bael_SeverityUtil
                        // ------------------------

// CLASS METHODS
inline
int bael_SeverityUtil::fromAscii(bael_Severity::Level *level,
                                 const char           *name)
{
    return fromAsciiCaseless(level, name);
}

inline
bool bael_SeverityUtil::isValidName(const char *name)
{
    return isValidNameCaseless(name);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
