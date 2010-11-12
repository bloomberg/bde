// baelu_severity.h              -*-C++-*-
#ifndef INCLUDED_BAELU_SEVERITY
#define INCLUDED_BAELU_SEVERITY

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a suite of utility functions on 'bael_Severity' levels.
//
//@DEPRECATED: Use 'bael_severityutil' instead.
//
//@CLASSES:
//   baelu_Severity: namespace for pure procedures on 'bael_Severity::Level'
//
//@AUTHOR: Steven Breitstein (sbreitst)
//
//@SEE_ALSO: bael_severity
//
//@DESCRIPTION: [!DEPRECATED!] This component provides a suite of pure
// procedures that apply to the 'bael_Severity::Level' enumeration.  In
// particular, the 'baelu_Severity' 'struct' provides a 'fromAscii' function
// that returns the 'bael_Severity::Level' enumerator value corresponding to a
// given ASCII string, and an 'isValidName' function that confirms that a
// given string corresponds to one of the enumerators in the
// 'bael_Severity::Level' enumeration.
//
///Synopsis
///--------
// The following is a list of functions available in this component:
//..
//    static int  baelu_Severity::fromAscii(bael_Severity::Level *level,
//                                          const char           *name);
//
//    static bool baelu_Severity::isValidName(const char *name);
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
//    assert(baelu_Severity::isValidName(argv[0]));
//    assert(baelu_Severity::isValidName(argv[1]));
//    assert(baelu_Severity::isValidName(argv[2]));
//    assert(baelu_Severity::isValidName(argv[3]));
//
//    bael_Severity::Level record;
//    bael_Severity::Level pass;
//    bael_Severity::Level trigger;
//    bael_Severity::Level triggerAll;
//
//    assert(0 == baelu_Severity::fromAscii(&record,     argv[0]));
//    assert(0 == baelu_Severity::fromAscii(&pass,       argv[1]));
//    assert(0 == baelu_Severity::fromAscii(&trigger,    argv[2]));
//    assert(0 == baelu_Severity::fromAscii(&triggerAll, argv[3]));
//
//    assert(bael_Severity::BAEL_INFO  == record);
//    assert(bael_Severity::BAEL_WARN  == pass);
//    assert(bael_Severity::BAEL_ERROR == trigger);
//    assert(bael_Severity::BAEL_FATAL == triggerAll);
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEL_SEVERITYUTIL
#include <bael_severityutil.h>
#endif

namespace BloombergLP {

                            // =====================
                            // struct baelu_Severity
                            // =====================

typedef bael_SeverityUtil baelu_Severity;



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
