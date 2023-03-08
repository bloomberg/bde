// baltzo_timezoneutilimp.t.cpp                                       -*-C++-*-
#include <baltzo_timezoneutilimp.h>

#include <baltzo_datafileloader.h>
#include <baltzo_defaultzoneinfocache.h>
#include <baltzo_errorcode.h>
#include <baltzo_localtimedescriptor.h>
#include <baltzo_localtimeperiod.h>
#include <baltzo_testloader.h>
#include <baltzo_zoneinfoutil.h>

#include <baltzo_localdatetime.h>


#include <bsl_memory.h>

#include <bdlt_iso8601util.h>
#include <bdlt_epochutil.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_log.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

#undef DS

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
// The component under test implements a namespace for utility functions that
// convert time values to, and from, local time.
//
// Class Methods:
//: o 'convertUtcToLocalTime'
//: o 'createLocalTimePeriod'
//: o 'initLocalTime'
//: o 'loadLocalTimePeriodForUtc'
//: o 'resolveLocalTime'
//-----------------------------------------------------------------------------
//=============================================================================
// CLASS METHODS
// [ 2] convertUtcToLocalTime(Datetime *, char *, Datetime&, Cache *)
// [ 3] resolveLocalTime(...)
// [ 4] 'initLocalTime(DatetimeTz *, Datetime& , char *, Dst, Cache *)
// [ 5] 'createLocalTimePeriod(Period *, TransitionConstIter, Zoneinfo)'
// [ 6] 'loadLocalTimePeriodForUtc(DatetimeTz *, Datetime& , char *, Cache *)
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE
//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;
static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                   STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                    << J << "\t" \
                    << #K << ": " << K <<  "\n"; aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                     SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------
#define P(X)  cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X)  cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_    cout << "\t" << flush;          // Print a tab (w/o newline)
#define L_ __LINE__                           // current Line number
// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------
#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)
// ============================================================================
//                              GLOBAL TYPEDEFS
// ----------------------------------------------------------------------------
typedef baltzo::TimeZoneUtilImp                   Obj;
typedef baltzo::LocalTimeDescriptor               Descriptor;
typedef baltzo::LocalTimeValidity                 Validity;
typedef baltzo::Zoneinfo::TransitionConstIterator Iterator;
typedef baltzo::ErrorCode                         Err;
typedef baltzo::DstPolicy                         Dst;
// ============================================================================
//                             GLOBAL CONSTANTS
// ----------------------------------------------------------------------------
const Dst::Enum      DU   = Dst::e_UNSPECIFIED;
const Dst::Enum      DD   = Dst::e_DST;
const Dst::Enum      DS   = Dst::e_STANDARD;
const Validity::Enum VU   = Validity::e_VALID_UNIQUE;
const Validity::Enum VA   = Validity::e_VALID_AMBIGUOUS;
const Validity::Enum VI   = Validity::e_INVALID;
const Err::Enum      EUID = Err::k_UNSUPPORTED_ID;

// Standard Identifiers
const char *NY       = "America/New_York";
const char *RY       = "Asia/Riyadh";
const char *SA       = "Asia/Saigon";
const char *GMT      = "Etc/GMT";
const char *GP1      = "Etc/GMT+1";
const char *GP2      = "Etc/GMT+2";
const char *GM1      = "Etc/GMT-1";
const char *RM       = "Europe/Rome";

// Synthetic Identifiers
const char *ALLDST   = "ALLDST";
const char *OLDDST   = "OLDDST";

// ============================================================================
//                            TEST TIME ZONE DATA
// ----------------------------------------------------------------------------

// America/New_York
static const unsigned char AMERICA_NEW_YORK_DATA[] = {
    0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xeb,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x10, 0x9e, 0xa6, 0x1e, 0x70,
    0x9f, 0xba, 0xeb, 0x60, 0xa0, 0x86, 0x00, 0x70, 0xa1, 0x9a, 0xcd, 0x60,
    0xa2, 0x65, 0xe2, 0x70, 0xa3, 0x83, 0xe9, 0xe0, 0xa4, 0x6a, 0xae, 0x70,
    0xa5, 0x35, 0xa7, 0x60, 0xa6, 0x53, 0xca, 0xf0, 0xa7, 0x15, 0x89, 0x60,
    0xa8, 0x33, 0xac, 0xf0, 0xa8, 0xfe, 0xa5, 0xe0, 0xaa, 0x13, 0x8e, 0xf0,
    0xaa, 0xde, 0x87, 0xe0, 0xab, 0xf3, 0x70, 0xf0, 0xac, 0xbe, 0x69, 0xe0,
    0xad, 0xd3, 0x52, 0xf0, 0xae, 0x9e, 0x4b, 0xe0, 0xaf, 0xb3, 0x34, 0xf0,
    0xb0, 0x7e, 0x2d, 0xe0, 0xb1, 0x9c, 0x51, 0x70, 0xb2, 0x67, 0x4a, 0x60,
    0xb3, 0x7c, 0x33, 0x70, 0xb4, 0x47, 0x2c, 0x60, 0xb5, 0x5c, 0x15, 0x70,
    0xb6, 0x27, 0x0e, 0x60, 0xb7, 0x3b, 0xf7, 0x70, 0xb8, 0x06, 0xf0, 0x60,
    0xb9, 0x1b, 0xd9, 0x70, 0xb9, 0xe6, 0xd2, 0x60, 0xbb, 0x04, 0xf5, 0xf0,
    0xbb, 0xc6, 0xb4, 0x60, 0xbc, 0xe4, 0xd7, 0xf0, 0xbd, 0xaf, 0xd0, 0xe0,
    0xbe, 0xc4, 0xb9, 0xf0, 0xbf, 0x8f, 0xb2, 0xe0, 0xc0, 0xa4, 0x9b, 0xf0,
    0xc1, 0x6f, 0x94, 0xe0, 0xc2, 0x84, 0x7d, 0xf0, 0xc3, 0x4f, 0x76, 0xe0,
    0xc4, 0x64, 0x5f, 0xf0, 0xc5, 0x2f, 0x58, 0xe0, 0xc6, 0x4d, 0x7c, 0x70,
    0xc7, 0x0f, 0x3a, 0xe0, 0xc8, 0x2d, 0x5e, 0x70, 0xc8, 0xf8, 0x57, 0x60,
    0xca, 0x0d, 0x40, 0x70, 0xca, 0xd8, 0x39, 0x60, 0xcb, 0x88, 0xf0, 0x70,
    0xd2, 0x23, 0xf4, 0x70, 0xd2, 0x60, 0xfb, 0xe0, 0xd3, 0x75, 0xe4, 0xf0,
    0xd4, 0x40, 0xdd, 0xe0, 0xd5, 0x55, 0xc6, 0xf0, 0xd6, 0x20, 0xbf, 0xe0,
    0xd7, 0x35, 0xa8, 0xf0, 0xd8, 0x00, 0xa1, 0xe0, 0xd9, 0x15, 0x8a, 0xf0,
    0xd9, 0xe0, 0x83, 0xe0, 0xda, 0xfe, 0xa7, 0x70, 0xdb, 0xc0, 0x65, 0xe0,
    0xdc, 0xde, 0x89, 0x70, 0xdd, 0xa9, 0x82, 0x60, 0xde, 0xbe, 0x6b, 0x70,
    0xdf, 0x89, 0x64, 0x60, 0xe0, 0x9e, 0x4d, 0x70, 0xe1, 0x69, 0x46, 0x60,
    0xe2, 0x7e, 0x2f, 0x70, 0xe3, 0x49, 0x28, 0x60, 0xe4, 0x5e, 0x11, 0x70,
    0xe5, 0x57, 0x2e, 0xe0, 0xe6, 0x47, 0x2d, 0xf0, 0xe7, 0x37, 0x10, 0xe0,
    0xe8, 0x27, 0x0f, 0xf0, 0xe9, 0x16, 0xf2, 0xe0, 0xea, 0x06, 0xf1, 0xf0,
    0xea, 0xf6, 0xd4, 0xe0, 0xeb, 0xe6, 0xd3, 0xf0, 0xec, 0xd6, 0xb6, 0xe0,
    0xed, 0xc6, 0xb5, 0xf0, 0xee, 0xbf, 0xd3, 0x60, 0xef, 0xaf, 0xd2, 0x70,
    0xf0, 0x9f, 0xb5, 0x60, 0xf1, 0x8f, 0xb4, 0x70, 0xf2, 0x7f, 0x97, 0x60,
    0xf3, 0x6f, 0x96, 0x70, 0xf4, 0x5f, 0x79, 0x60, 0xf5, 0x4f, 0x78, 0x70,
    0xf6, 0x3f, 0x5b, 0x60, 0xf7, 0x2f, 0x5a, 0x70, 0xf8, 0x28, 0x77, 0xe0,
    0xf9, 0x0f, 0x3c, 0x70, 0xfa, 0x08, 0x59, 0xe0, 0xfa, 0xf8, 0x58, 0xf0,
    0xfb, 0xe8, 0x3b, 0xe0, 0xfc, 0xd8, 0x3a, 0xf0, 0xfd, 0xc8, 0x1d, 0xe0,
    0xfe, 0xb8, 0x1c, 0xf0, 0xff, 0xa7, 0xff, 0xe0, 0x00, 0x97, 0xfe, 0xf0,
    0x01, 0x87, 0xe1, 0xe0, 0x02, 0x77, 0xe0, 0xf0, 0x03, 0x70, 0xfe, 0x60,
    0x04, 0x60, 0xfd, 0x70, 0x05, 0x50, 0xe0, 0x60, 0x06, 0x40, 0xdf, 0x70,
    0x07, 0x30, 0xc2, 0x60, 0x07, 0x8d, 0x19, 0x70, 0x09, 0x10, 0xa4, 0x60,
    0x09, 0xad, 0x94, 0xf0, 0x0a, 0xf0, 0x86, 0x60, 0x0b, 0xe0, 0x85, 0x70,
    0x0c, 0xd9, 0xa2, 0xe0, 0x0d, 0xc0, 0x67, 0x70, 0x0e, 0xb9, 0x84, 0xe0,
    0x0f, 0xa9, 0x83, 0xf0, 0x10, 0x99, 0x66, 0xe0, 0x11, 0x89, 0x65, 0xf0,
    0x12, 0x79, 0x48, 0xe0, 0x13, 0x69, 0x47, 0xf0, 0x14, 0x59, 0x2a, 0xe0,
    0x15, 0x49, 0x29, 0xf0, 0x16, 0x39, 0x0c, 0xe0, 0x17, 0x29, 0x0b, 0xf0,
    0x18, 0x22, 0x29, 0x60, 0x19, 0x08, 0xed, 0xf0, 0x1a, 0x02, 0x0b, 0x60,
    0x1a, 0xf2, 0x0a, 0x70, 0x1b, 0xe1, 0xed, 0x60, 0x1c, 0xd1, 0xec, 0x70,
    0x1d, 0xc1, 0xcf, 0x60, 0x1e, 0xb1, 0xce, 0x70, 0x1f, 0xa1, 0xb1, 0x60,
    0x20, 0x76, 0x00, 0xf0, 0x21, 0x81, 0x93, 0x60, 0x22, 0x55, 0xe2, 0xf0,
    0x23, 0x6a, 0xaf, 0xe0, 0x24, 0x35, 0xc4, 0xf0, 0x25, 0x4a, 0x91, 0xe0,
    0x26, 0x15, 0xa6, 0xf0, 0x27, 0x2a, 0x73, 0xe0, 0x27, 0xfe, 0xc3, 0x70,
    0x29, 0x0a, 0x55, 0xe0, 0x29, 0xde, 0xa5, 0x70, 0x2a, 0xea, 0x37, 0xe0,
    0x2b, 0xbe, 0x87, 0x70, 0x2c, 0xd3, 0x54, 0x60, 0x2d, 0x9e, 0x69, 0x70,
    0x2e, 0xb3, 0x36, 0x60, 0x2f, 0x7e, 0x4b, 0x70, 0x30, 0x93, 0x18, 0x60,
    0x31, 0x67, 0x67, 0xf0, 0x32, 0x72, 0xfa, 0x60, 0x33, 0x47, 0x49, 0xf0,
    0x34, 0x52, 0xdc, 0x60, 0x35, 0x27, 0x2b, 0xf0, 0x36, 0x32, 0xbe, 0x60,
    0x37, 0x07, 0x0d, 0xf0, 0x38, 0x1b, 0xda, 0xe0, 0x38, 0xe6, 0xef, 0xf0,
    0x39, 0xfb, 0xbc, 0xe0, 0x3a, 0xc6, 0xd1, 0xf0, 0x3b, 0xdb, 0x9e, 0xe0,
    0x3c, 0xaf, 0xee, 0x70, 0x3d, 0xbb, 0x80, 0xe0, 0x3e, 0x8f, 0xd0, 0x70,
    0x3f, 0x9b, 0x62, 0xe0, 0x40, 0x6f, 0xb2, 0x70, 0x41, 0x84, 0x7f, 0x60,
    0x42, 0x4f, 0x94, 0x70, 0x43, 0x64, 0x61, 0x60, 0x44, 0x2f, 0x76, 0x70,
    0x45, 0x44, 0x43, 0x60, 0x45, 0xf3, 0xa8, 0xf0, 0x47, 0x2d, 0x5f, 0xe0,
    0x47, 0xd3, 0x8a, 0xf0, 0x49, 0x0d, 0x41, 0xe0, 0x49, 0xb3, 0x6c, 0xf0,
    0x4a, 0xed, 0x23, 0xe0, 0x4b, 0x9c, 0x89, 0x70, 0x4c, 0xd6, 0x40, 0x60,
    0x4d, 0x7c, 0x6b, 0x70, 0x4e, 0xb6, 0x22, 0x60, 0x4f, 0x5c, 0x4d, 0x70,
    0x50, 0x96, 0x04, 0x60, 0x51, 0x3c, 0x2f, 0x70, 0x52, 0x75, 0xe6, 0x60,
    0x53, 0x1c, 0x11, 0x70, 0x54, 0x55, 0xc8, 0x60, 0x54, 0xfb, 0xf3, 0x70,
    0x56, 0x35, 0xaa, 0x60, 0x56, 0xe5, 0x0f, 0xf0, 0x58, 0x1e, 0xc6, 0xe0,
    0x58, 0xc4, 0xf1, 0xf0, 0x59, 0xfe, 0xa8, 0xe0, 0x5a, 0xa4, 0xd3, 0xf0,
    0x5b, 0xde, 0x8a, 0xe0, 0x5c, 0x84, 0xb5, 0xf0, 0x5d, 0xbe, 0x6c, 0xe0,
    0x5e, 0x64, 0x97, 0xf0, 0x5f, 0x9e, 0x4e, 0xe0, 0x60, 0x4d, 0xb4, 0x70,
    0x61, 0x87, 0x6b, 0x60, 0x62, 0x2d, 0x96, 0x70, 0x63, 0x67, 0x4d, 0x60,
    0x64, 0x0d, 0x78, 0x70, 0x65, 0x47, 0x2f, 0x60, 0x65, 0xed, 0x5a, 0x70,
    0x67, 0x27, 0x11, 0x60, 0x67, 0xcd, 0x3c, 0x70, 0x69, 0x06, 0xf3, 0x60,
    0x69, 0xad, 0x1e, 0x70, 0x6a, 0xe6, 0xd5, 0x60, 0x6b, 0x96, 0x3a, 0xf0,
    0x6c, 0xcf, 0xf1, 0xe0, 0x6d, 0x76, 0x1c, 0xf0, 0x6e, 0xaf, 0xd3, 0xe0,
    0x6f, 0x55, 0xfe, 0xf0, 0x70, 0x8f, 0xb5, 0xe0, 0x71, 0x35, 0xe0, 0xf0,
    0x72, 0x6f, 0x97, 0xe0, 0x73, 0x15, 0xc2, 0xf0, 0x74, 0x4f, 0x79, 0xe0,
    0x74, 0xfe, 0xdf, 0x70, 0x76, 0x38, 0x96, 0x60, 0x76, 0xde, 0xc1, 0x70,
    0x78, 0x18, 0x78, 0x60, 0x78, 0xbe, 0xa3, 0x70, 0x79, 0xf8, 0x5a, 0x60,
    0x7a, 0x9e, 0x85, 0x70, 0x7b, 0xd8, 0x3c, 0x60, 0x7c, 0x7e, 0x67, 0x70,
    0x7d, 0xb8, 0x1e, 0x60, 0x7e, 0x5e, 0x49, 0x70, 0x7f, 0x98, 0x00, 0x60,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
    0x02, 0x03, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0xff, 0xff, 0xc7, 0xc0, 0x01,
    0x00, 0xff, 0xff, 0xb9, 0xb0, 0x00, 0x04, 0xff, 0xff, 0xc7, 0xc0, 0x01,
    0x08, 0xff, 0xff, 0xc7, 0xc0, 0x01, 0x0c, 0x45, 0x44, 0x54, 0x00, 0x45,
    0x53, 0x54, 0x00, 0x45, 0x57, 0x54, 0x00, 0x45, 0x50, 0x54, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x54, 0x5a, 0x69, 0x66, 0x32,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xec, 0x00, 0x00, 0x00, 0x05, 0x00,
    0x00, 0x00, 0x14, 0xff, 0xff, 0xff, 0xff, 0x5e, 0x03, 0xf0, 0x90, 0xff,
    0xff, 0xff, 0xff, 0x9e, 0xa6, 0x1e, 0x70, 0xff, 0xff, 0xff, 0xff, 0x9f,
    0xba, 0xeb, 0x60, 0xff, 0xff, 0xff, 0xff, 0xa0, 0x86, 0x00, 0x70, 0xff,
    0xff, 0xff, 0xff, 0xa1, 0x9a, 0xcd, 0x60, 0xff, 0xff, 0xff, 0xff, 0xa2,
    0x65, 0xe2, 0x70, 0xff, 0xff, 0xff, 0xff, 0xa3, 0x83, 0xe9, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0xa4, 0x6a, 0xae, 0x70, 0xff, 0xff, 0xff, 0xff, 0xa5,
    0x35, 0xa7, 0x60, 0xff, 0xff, 0xff, 0xff, 0xa6, 0x53, 0xca, 0xf0, 0xff,
    0xff, 0xff, 0xff, 0xa7, 0x15, 0x89, 0x60, 0xff, 0xff, 0xff, 0xff, 0xa8,
    0x33, 0xac, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xa8, 0xfe, 0xa5, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0xaa, 0x13, 0x8e, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xaa,
    0xde, 0x87, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xab, 0xf3, 0x70, 0xf0, 0xff,
    0xff, 0xff, 0xff, 0xac, 0xbe, 0x69, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xad,
    0xd3, 0x52, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xae, 0x9e, 0x4b, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0xaf, 0xb3, 0x34, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xb0,
    0x7e, 0x2d, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xb1, 0x9c, 0x51, 0x70, 0xff,
    0xff, 0xff, 0xff, 0xb2, 0x67, 0x4a, 0x60, 0xff, 0xff, 0xff, 0xff, 0xb3,
    0x7c, 0x33, 0x70, 0xff, 0xff, 0xff, 0xff, 0xb4, 0x47, 0x2c, 0x60, 0xff,
    0xff, 0xff, 0xff, 0xb5, 0x5c, 0x15, 0x70, 0xff, 0xff, 0xff, 0xff, 0xb6,
    0x27, 0x0e, 0x60, 0xff, 0xff, 0xff, 0xff, 0xb7, 0x3b, 0xf7, 0x70, 0xff,
    0xff, 0xff, 0xff, 0xb8, 0x06, 0xf0, 0x60, 0xff, 0xff, 0xff, 0xff, 0xb9,
    0x1b, 0xd9, 0x70, 0xff, 0xff, 0xff, 0xff, 0xb9, 0xe6, 0xd2, 0x60, 0xff,
    0xff, 0xff, 0xff, 0xbb, 0x04, 0xf5, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xbb,
    0xc6, 0xb4, 0x60, 0xff, 0xff, 0xff, 0xff, 0xbc, 0xe4, 0xd7, 0xf0, 0xff,
    0xff, 0xff, 0xff, 0xbd, 0xaf, 0xd0, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xbe,
    0xc4, 0xb9, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xbf, 0x8f, 0xb2, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0xc0, 0xa4, 0x9b, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xc1,
    0x6f, 0x94, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xc2, 0x84, 0x7d, 0xf0, 0xff,
    0xff, 0xff, 0xff, 0xc3, 0x4f, 0x76, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xc4,
    0x64, 0x5f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xc5, 0x2f, 0x58, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0xc6, 0x4d, 0x7c, 0x70, 0xff, 0xff, 0xff, 0xff, 0xc7,
    0x0f, 0x3a, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xc8, 0x2d, 0x5e, 0x70, 0xff,
    0xff, 0xff, 0xff, 0xc8, 0xf8, 0x57, 0x60, 0xff, 0xff, 0xff, 0xff, 0xca,
    0x0d, 0x40, 0x70, 0xff, 0xff, 0xff, 0xff, 0xca, 0xd8, 0x39, 0x60, 0xff,
    0xff, 0xff, 0xff, 0xcb, 0x88, 0xf0, 0x70, 0xff, 0xff, 0xff, 0xff, 0xd2,
    0x23, 0xf4, 0x70, 0xff, 0xff, 0xff, 0xff, 0xd2, 0x60, 0xfb, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0xd3, 0x75, 0xe4, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xd4,
    0x40, 0xdd, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xd5, 0x55, 0xc6, 0xf0, 0xff,
    0xff, 0xff, 0xff, 0xd6, 0x20, 0xbf, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xd7,
    0x35, 0xa8, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xd8, 0x00, 0xa1, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0xd9, 0x15, 0x8a, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xd9,
    0xe0, 0x83, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xda, 0xfe, 0xa7, 0x70, 0xff,
    0xff, 0xff, 0xff, 0xdb, 0xc0, 0x65, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xdc,
    0xde, 0x89, 0x70, 0xff, 0xff, 0xff, 0xff, 0xdd, 0xa9, 0x82, 0x60, 0xff,
    0xff, 0xff, 0xff, 0xde, 0xbe, 0x6b, 0x70, 0xff, 0xff, 0xff, 0xff, 0xdf,
    0x89, 0x64, 0x60, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x9e, 0x4d, 0x70, 0xff,
    0xff, 0xff, 0xff, 0xe1, 0x69, 0x46, 0x60, 0xff, 0xff, 0xff, 0xff, 0xe2,
    0x7e, 0x2f, 0x70, 0xff, 0xff, 0xff, 0xff, 0xe3, 0x49, 0x28, 0x60, 0xff,
    0xff, 0xff, 0xff, 0xe4, 0x5e, 0x11, 0x70, 0xff, 0xff, 0xff, 0xff, 0xe5,
    0x57, 0x2e, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xe6, 0x47, 0x2d, 0xf0, 0xff,
    0xff, 0xff, 0xff, 0xe7, 0x37, 0x10, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xe8,
    0x27, 0x0f, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xe9, 0x16, 0xf2, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0xea, 0x06, 0xf1, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xea,
    0xf6, 0xd4, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xeb, 0xe6, 0xd3, 0xf0, 0xff,
    0xff, 0xff, 0xff, 0xec, 0xd6, 0xb6, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xed,
    0xc6, 0xb5, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xee, 0xbf, 0xd3, 0x60, 0xff,
    0xff, 0xff, 0xff, 0xef, 0xaf, 0xd2, 0x70, 0xff, 0xff, 0xff, 0xff, 0xf0,
    0x9f, 0xb5, 0x60, 0xff, 0xff, 0xff, 0xff, 0xf1, 0x8f, 0xb4, 0x70, 0xff,
    0xff, 0xff, 0xff, 0xf2, 0x7f, 0x97, 0x60, 0xff, 0xff, 0xff, 0xff, 0xf3,
    0x6f, 0x96, 0x70, 0xff, 0xff, 0xff, 0xff, 0xf4, 0x5f, 0x79, 0x60, 0xff,
    0xff, 0xff, 0xff, 0xf5, 0x4f, 0x78, 0x70, 0xff, 0xff, 0xff, 0xff, 0xf6,
    0x3f, 0x5b, 0x60, 0xff, 0xff, 0xff, 0xff, 0xf7, 0x2f, 0x5a, 0x70, 0xff,
    0xff, 0xff, 0xff, 0xf8, 0x28, 0x77, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xf9,
    0x0f, 0x3c, 0x70, 0xff, 0xff, 0xff, 0xff, 0xfa, 0x08, 0x59, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0xfa, 0xf8, 0x58, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xfb,
    0xe8, 0x3b, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xd8, 0x3a, 0xf0, 0xff,
    0xff, 0xff, 0xff, 0xfd, 0xc8, 0x1d, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xfe,
    0xb8, 0x1c, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xa7, 0xff, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x97, 0xfe, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x87, 0xe1, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x02, 0x77, 0xe0, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x03, 0x70, 0xfe, 0x60, 0x00, 0x00, 0x00, 0x00, 0x04,
    0x60, 0xfd, 0x70, 0x00, 0x00, 0x00, 0x00, 0x05, 0x50, 0xe0, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x06, 0x40, 0xdf, 0x70, 0x00, 0x00, 0x00, 0x00, 0x07,
    0x30, 0xc2, 0x60, 0x00, 0x00, 0x00, 0x00, 0x07, 0x8d, 0x19, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x09, 0x10, 0xa4, 0x60, 0x00, 0x00, 0x00, 0x00, 0x09,
    0xad, 0x94, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x0a, 0xf0, 0x86, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x0b, 0xe0, 0x85, 0x70, 0x00, 0x00, 0x00, 0x00, 0x0c,
    0xd9, 0xa2, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0d, 0xc0, 0x67, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x0e, 0xb9, 0x84, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x0f,
    0xa9, 0x83, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x10, 0x99, 0x66, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x11, 0x89, 0x65, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x12,
    0x79, 0x48, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x13, 0x69, 0x47, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x14, 0x59, 0x2a, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x15,
    0x49, 0x29, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x16, 0x39, 0x0c, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x17, 0x29, 0x0b, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x18,
    0x22, 0x29, 0x60, 0x00, 0x00, 0x00, 0x00, 0x19, 0x08, 0xed, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x1a, 0x02, 0x0b, 0x60, 0x00, 0x00, 0x00, 0x00, 0x1a,
    0xf2, 0x0a, 0x70, 0x00, 0x00, 0x00, 0x00, 0x1b, 0xe1, 0xed, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x1c, 0xd1, 0xec, 0x70, 0x00, 0x00, 0x00, 0x00, 0x1d,
    0xc1, 0xcf, 0x60, 0x00, 0x00, 0x00, 0x00, 0x1e, 0xb1, 0xce, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x1f, 0xa1, 0xb1, 0x60, 0x00, 0x00, 0x00, 0x00, 0x20,
    0x76, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x21, 0x81, 0x93, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x22, 0x55, 0xe2, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x23,
    0x6a, 0xaf, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x24, 0x35, 0xc4, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x25, 0x4a, 0x91, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x26,
    0x15, 0xa6, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x27, 0x2a, 0x73, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x27, 0xfe, 0xc3, 0x70, 0x00, 0x00, 0x00, 0x00, 0x29,
    0x0a, 0x55, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x29, 0xde, 0xa5, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x2a, 0xea, 0x37, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x2b,
    0xbe, 0x87, 0x70, 0x00, 0x00, 0x00, 0x00, 0x2c, 0xd3, 0x54, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x2d, 0x9e, 0x69, 0x70, 0x00, 0x00, 0x00, 0x00, 0x2e,
    0xb3, 0x36, 0x60, 0x00, 0x00, 0x00, 0x00, 0x2f, 0x7e, 0x4b, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x30, 0x93, 0x18, 0x60, 0x00, 0x00, 0x00, 0x00, 0x31,
    0x67, 0x67, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x32, 0x72, 0xfa, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x33, 0x47, 0x49, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x34,
    0x52, 0xdc, 0x60, 0x00, 0x00, 0x00, 0x00, 0x35, 0x27, 0x2b, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x36, 0x32, 0xbe, 0x60, 0x00, 0x00, 0x00, 0x00, 0x37,
    0x07, 0x0d, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x38, 0x1b, 0xda, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x38, 0xe6, 0xef, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x39,
    0xfb, 0xbc, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3a, 0xc6, 0xd1, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x3b, 0xdb, 0x9e, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x3c,
    0xaf, 0xee, 0x70, 0x00, 0x00, 0x00, 0x00, 0x3d, 0xbb, 0x80, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x3e, 0x8f, 0xd0, 0x70, 0x00, 0x00, 0x00, 0x00, 0x3f,
    0x9b, 0x62, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x40, 0x6f, 0xb2, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x41, 0x84, 0x7f, 0x60, 0x00, 0x00, 0x00, 0x00, 0x42,
    0x4f, 0x94, 0x70, 0x00, 0x00, 0x00, 0x00, 0x43, 0x64, 0x61, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x44, 0x2f, 0x76, 0x70, 0x00, 0x00, 0x00, 0x00, 0x45,
    0x44, 0x43, 0x60, 0x00, 0x00, 0x00, 0x00, 0x45, 0xf3, 0xa8, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x47, 0x2d, 0x5f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x47,
    0xd3, 0x8a, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x49, 0x0d, 0x41, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x49, 0xb3, 0x6c, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x4a,
    0xed, 0x23, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x4b, 0x9c, 0x89, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x4c, 0xd6, 0x40, 0x60, 0x00, 0x00, 0x00, 0x00, 0x4d,
    0x7c, 0x6b, 0x70, 0x00, 0x00, 0x00, 0x00, 0x4e, 0xb6, 0x22, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x4f, 0x5c, 0x4d, 0x70, 0x00, 0x00, 0x00, 0x00, 0x50,
    0x96, 0x04, 0x60, 0x00, 0x00, 0x00, 0x00, 0x51, 0x3c, 0x2f, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x52, 0x75, 0xe6, 0x60, 0x00, 0x00, 0x00, 0x00, 0x53,
    0x1c, 0x11, 0x70, 0x00, 0x00, 0x00, 0x00, 0x54, 0x55, 0xc8, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x54, 0xfb, 0xf3, 0x70, 0x00, 0x00, 0x00, 0x00, 0x56,
    0x35, 0xaa, 0x60, 0x00, 0x00, 0x00, 0x00, 0x56, 0xe5, 0x0f, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x58, 0x1e, 0xc6, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x58,
    0xc4, 0xf1, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x59, 0xfe, 0xa8, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x5a, 0xa4, 0xd3, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x5b,
    0xde, 0x8a, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x5c, 0x84, 0xb5, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x5d, 0xbe, 0x6c, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x5e,
    0x64, 0x97, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x5f, 0x9e, 0x4e, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x60, 0x4d, 0xb4, 0x70, 0x00, 0x00, 0x00, 0x00, 0x61,
    0x87, 0x6b, 0x60, 0x00, 0x00, 0x00, 0x00, 0x62, 0x2d, 0x96, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x63, 0x67, 0x4d, 0x60, 0x00, 0x00, 0x00, 0x00, 0x64,
    0x0d, 0x78, 0x70, 0x00, 0x00, 0x00, 0x00, 0x65, 0x47, 0x2f, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x65, 0xed, 0x5a, 0x70, 0x00, 0x00, 0x00, 0x00, 0x67,
    0x27, 0x11, 0x60, 0x00, 0x00, 0x00, 0x00, 0x67, 0xcd, 0x3c, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x69, 0x06, 0xf3, 0x60, 0x00, 0x00, 0x00, 0x00, 0x69,
    0xad, 0x1e, 0x70, 0x00, 0x00, 0x00, 0x00, 0x6a, 0xe6, 0xd5, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x6b, 0x96, 0x3a, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x6c,
    0xcf, 0xf1, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x6d, 0x76, 0x1c, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x6e, 0xaf, 0xd3, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x6f,
    0x55, 0xfe, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x70, 0x8f, 0xb5, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x71, 0x35, 0xe0, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x72,
    0x6f, 0x97, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x73, 0x15, 0xc2, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x74, 0x4f, 0x79, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x74,
    0xfe, 0xdf, 0x70, 0x00, 0x00, 0x00, 0x00, 0x76, 0x38, 0x96, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x76, 0xde, 0xc1, 0x70, 0x00, 0x00, 0x00, 0x00, 0x78,
    0x18, 0x78, 0x60, 0x00, 0x00, 0x00, 0x00, 0x78, 0xbe, 0xa3, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x79, 0xf8, 0x5a, 0x60, 0x00, 0x00, 0x00, 0x00, 0x7a,
    0x9e, 0x85, 0x70, 0x00, 0x00, 0x00, 0x00, 0x7b, 0xd8, 0x3c, 0x60, 0x00,
    0x00, 0x00, 0x00, 0x7c, 0x7e, 0x67, 0x70, 0x00, 0x00, 0x00, 0x00, 0x7d,
    0xb8, 0x1e, 0x60, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x5e, 0x49, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x7f, 0x98, 0x00, 0x60, 0x02, 0x01, 0x02, 0x01, 0x02,
    0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02,
    0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02,
    0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02,
    0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x03, 0x04, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
    0x02, 0x01, 0x02, 0xff, 0xff, 0xba, 0x9e, 0x00, 0x00, 0xff, 0xff, 0xc7,
    0xc0, 0x01, 0x04, 0xff, 0xff, 0xb9, 0xb0, 0x00, 0x08, 0xff, 0xff, 0xc7,
    0xc0, 0x01, 0x0c, 0xff, 0xff, 0xc7, 0xc0, 0x01, 0x10, 0x4c, 0x4d, 0x54,
    0x00, 0x45, 0x44, 0x54, 0x00, 0x45, 0x53, 0x54, 0x00, 0x45, 0x57, 0x54,
    0x00, 0x45, 0x50, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x0a, 0x45, 0x53, 0x54, 0x35, 0x45, 0x44, 0x54, 0x2c,
    0x4d, 0x33, 0x2e, 0x32, 0x2e, 0x30, 0x2c, 0x4d, 0x31, 0x31, 0x2e, 0x31,
    0x2e, 0x30, 0x0a
};

// Europe/Rome
static const unsigned char EUROPE_ROME_DATA[] = {
    0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06,
    0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xaa,
    0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x09, 0x9b, 0x37, 0xa6, 0xf0,
    0x9b, 0xd5, 0xda, 0xf0, 0x9c, 0xc5, 0xcb, 0xf0, 0x9d, 0xb5, 0xbc, 0xf0,
    0x9e, 0x89, 0xfe, 0x70, 0x9f, 0x9e, 0xd9, 0x70, 0xa0, 0x60, 0xa5, 0xf0,
    0xa1, 0x7e, 0xbb, 0x70, 0xa2, 0x5c, 0x37, 0x70, 0xa3, 0x4c, 0x28, 0x70,
    0xc8, 0x6c, 0x35, 0xf0, 0xcc, 0xe7, 0x4b, 0x10, 0xcd, 0xa9, 0x17, 0x90,
    0xce, 0xa2, 0x43, 0x10, 0xcf, 0x92, 0x34, 0x10, 0xd0, 0x07, 0x5f, 0x60,
    0xd0, 0x6e, 0x42, 0x70, 0xd1, 0x72, 0x16, 0x10, 0xd2, 0x4c, 0xd2, 0xf0,
    0xd3, 0x3e, 0x31, 0x90, 0xd4, 0x49, 0xd2, 0x10, 0xd5, 0x1d, 0xf7, 0x70,
    0xd6, 0x29, 0x97, 0xf0, 0xd6, 0xeb, 0x80, 0x90, 0xd8, 0x09, 0x96, 0x10,
    0xf9, 0x33, 0xb5, 0xf0, 0xf9, 0xd9, 0xc4, 0xe0, 0xfb, 0x1c, 0xd2, 0x70,
    0xfb, 0xb9, 0xa6, 0xe0, 0xfc, 0xfc, 0xb4, 0x70, 0xfd, 0x99, 0x88, 0xe0,
    0xfe, 0xe5, 0xd0, 0xf0, 0xff, 0x82, 0xa5, 0x60, 0x00, 0xc5, 0xb2, 0xf0,
    0x01, 0x62, 0x87, 0x60, 0x02, 0x9c, 0x5a, 0x70, 0x03, 0x42, 0x77, 0x70,
    0x04, 0x85, 0x76, 0xf0, 0x05, 0x2b, 0x85, 0xe0, 0x06, 0x6e, 0x93, 0x70,
    0x07, 0x0b, 0x67, 0xe0, 0x08, 0x45, 0x3a, 0xf0, 0x08, 0xeb, 0x49, 0xe0,
    0x0a, 0x2e, 0x57, 0x70, 0x0a, 0xcb, 0x39, 0xf0, 0x0c, 0x0e, 0x39, 0x70,
    0x0c, 0xab, 0x1b, 0xf0, 0x0d, 0xe4, 0xe0, 0xf0, 0x0e, 0x8a, 0xfd, 0xf0,
    0x0f, 0xcd, 0xfd, 0x70, 0x10, 0x74, 0x1a, 0x70, 0x11, 0xad, 0xdf, 0x70,
    0x12, 0x53, 0xfc, 0x70, 0x12, 0xce, 0x97, 0xf0, 0x13, 0x4d, 0x44, 0x10,
    0x14, 0x33, 0xfa, 0x90, 0x15, 0x23, 0xeb, 0x90, 0x16, 0x13, 0xdc, 0x90,
    0x17, 0x03, 0xcd, 0x90, 0x17, 0xf3, 0xbe, 0x90, 0x18, 0xe3, 0xaf, 0x90,
    0x19, 0xd3, 0xa0, 0x90, 0x1a, 0xc3, 0x91, 0x90, 0x1b, 0xbc, 0xbd, 0x10,
    0x1c, 0xac, 0xae, 0x10, 0x1d, 0x9c, 0x9f, 0x10, 0x1e, 0x8c, 0x90, 0x10,
    0x1f, 0x7c, 0x81, 0x10, 0x20, 0x6c, 0x72, 0x10, 0x21, 0x5c, 0x63, 0x10,
    0x22, 0x4c, 0x54, 0x10, 0x23, 0x3c, 0x45, 0x10, 0x24, 0x2c, 0x36, 0x10,
    0x25, 0x1c, 0x27, 0x10, 0x26, 0x0c, 0x18, 0x10, 0x27, 0x05, 0x43, 0x90,
    0x27, 0xf5, 0x34, 0x90, 0x28, 0xe5, 0x25, 0x90, 0x29, 0xd5, 0x16, 0x90,
    0x2a, 0xc5, 0x07, 0x90, 0x2b, 0xb4, 0xf8, 0x90, 0x2c, 0xa4, 0xe9, 0x90,
    0x2d, 0x94, 0xda, 0x90, 0x2e, 0x84, 0xcb, 0x90, 0x2f, 0x74, 0xbc, 0x90,
    0x30, 0x64, 0xad, 0x90, 0x31, 0x5d, 0xd9, 0x10, 0x32, 0x72, 0xb4, 0x10,
    0x33, 0x3d, 0xbb, 0x10, 0x34, 0x52, 0x96, 0x10, 0x35, 0x1d, 0x9d, 0x10,
    0x36, 0x32, 0x78, 0x10, 0x36, 0xfd, 0x7f, 0x10, 0x38, 0x1b, 0x94, 0x90,
    0x38, 0xdd, 0x61, 0x10, 0x39, 0xfb, 0x76, 0x90, 0x3a, 0xbd, 0x43, 0x10,
    0x3b, 0xdb, 0x58, 0x90, 0x3c, 0xa6, 0x5f, 0x90, 0x3d, 0xbb, 0x3a, 0x90,
    0x3e, 0x86, 0x41, 0x90, 0x3f, 0x9b, 0x1c, 0x90, 0x40, 0x66, 0x23, 0x90,
    0x41, 0x84, 0x39, 0x10, 0x42, 0x46, 0x05, 0x90, 0x43, 0x64, 0x1b, 0x10,
    0x44, 0x25, 0xe7, 0x90, 0x45, 0x43, 0xfd, 0x10, 0x46, 0x05, 0xc9, 0x90,
    0x47, 0x23, 0xdf, 0x10, 0x47, 0xee, 0xe6, 0x10, 0x49, 0x03, 0xc1, 0x10,
    0x49, 0xce, 0xc8, 0x10, 0x4a, 0xe3, 0xa3, 0x10, 0x4b, 0xae, 0xaa, 0x10,
    0x4c, 0xcc, 0xbf, 0x90, 0x4d, 0x8e, 0x8c, 0x10, 0x4e, 0xac, 0xa1, 0x90,
    0x4f, 0x6e, 0x6e, 0x10, 0x50, 0x8c, 0x83, 0x90, 0x51, 0x57, 0x8a, 0x90,
    0x52, 0x6c, 0x65, 0x90, 0x53, 0x37, 0x6c, 0x90, 0x54, 0x4c, 0x47, 0x90,
    0x55, 0x17, 0x4e, 0x90, 0x56, 0x2c, 0x29, 0x90, 0x56, 0xf7, 0x30, 0x90,
    0x58, 0x15, 0x46, 0x10, 0x58, 0xd7, 0x12, 0x90, 0x59, 0xf5, 0x28, 0x10,
    0x5a, 0xb6, 0xf4, 0x90, 0x5b, 0xd5, 0x0a, 0x10, 0x5c, 0xa0, 0x11, 0x10,
    0x5d, 0xb4, 0xec, 0x10, 0x5e, 0x7f, 0xf3, 0x10, 0x5f, 0x94, 0xce, 0x10,
    0x60, 0x5f, 0xd5, 0x10, 0x61, 0x7d, 0xea, 0x90, 0x62, 0x3f, 0xb7, 0x10,
    0x63, 0x5d, 0xcc, 0x90, 0x64, 0x1f, 0x99, 0x10, 0x65, 0x3d, 0xae, 0x90,
    0x66, 0x08, 0xb5, 0x90, 0x67, 0x1d, 0x90, 0x90, 0x67, 0xe8, 0x97, 0x90,
    0x68, 0xfd, 0x72, 0x90, 0x69, 0xc8, 0x79, 0x90, 0x6a, 0xdd, 0x54, 0x90,
    0x6b, 0xa8, 0x5b, 0x90, 0x6c, 0xc6, 0x71, 0x10, 0x6d, 0x88, 0x3d, 0x90,
    0x6e, 0xa6, 0x53, 0x10, 0x6f, 0x68, 0x1f, 0x90, 0x70, 0x86, 0x35, 0x10,
    0x71, 0x51, 0x3c, 0x10, 0x72, 0x66, 0x17, 0x10, 0x73, 0x31, 0x1e, 0x10,
    0x74, 0x45, 0xf9, 0x10, 0x75, 0x11, 0x00, 0x10, 0x76, 0x2f, 0x15, 0x90,
    0x76, 0xf0, 0xe2, 0x10, 0x78, 0x0e, 0xf7, 0x90, 0x78, 0xd0, 0xc4, 0x10,
    0x79, 0xee, 0xd9, 0x90, 0x7a, 0xb0, 0xa6, 0x10, 0x7b, 0xce, 0xbb, 0x90,
    0x7c, 0x99, 0xc2, 0x90, 0x7d, 0xae, 0x9d, 0x90, 0x7e, 0x79, 0xa4, 0x90,
    0x7f, 0x8e, 0x7f, 0x90, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
    0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x02, 0x01, 0x02, 0x01, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02,
    0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x03, 0x04, 0x05,
    0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05,
    0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05,
    0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05,
    0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05,
    0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05,
    0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05,
    0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05,
    0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05,
    0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05,
    0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x00, 0x00, 0x1c, 0x20, 0x01, 0x00,
    0x00, 0x00, 0x0e, 0x10, 0x00, 0x05, 0x00, 0x00, 0x1c, 0x20, 0x01, 0x00,
    0x00, 0x00, 0x0e, 0x10, 0x00, 0x05, 0x00, 0x00, 0x1c, 0x20, 0x01, 0x00,
    0x00, 0x00, 0x0e, 0x10, 0x00, 0x05, 0x43, 0x45, 0x53, 0x54, 0x00, 0x43,
    0x45, 0x54, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x01, 0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0xac, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x11, 0xff,
    0xff, 0xff, 0xff, 0x3d, 0xbe, 0x1e, 0xcc, 0xff, 0xff, 0xff, 0xff, 0x70,
    0xbc, 0x83, 0xcc, 0xff, 0xff, 0xff, 0xff, 0x9b, 0x37, 0xa6, 0xf0, 0xff,
    0xff, 0xff, 0xff, 0x9b, 0xd5, 0xda, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x9c,
    0xc5, 0xcb, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x9d, 0xb5, 0xbc, 0xf0, 0xff,
    0xff, 0xff, 0xff, 0x9e, 0x89, 0xfe, 0x70, 0xff, 0xff, 0xff, 0xff, 0x9f,
    0x9e, 0xd9, 0x70, 0xff, 0xff, 0xff, 0xff, 0xa0, 0x60, 0xa5, 0xf0, 0xff,
    0xff, 0xff, 0xff, 0xa1, 0x7e, 0xbb, 0x70, 0xff, 0xff, 0xff, 0xff, 0xa2,
    0x5c, 0x37, 0x70, 0xff, 0xff, 0xff, 0xff, 0xa3, 0x4c, 0x28, 0x70, 0xff,
    0xff, 0xff, 0xff, 0xc8, 0x6c, 0x35, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xcc,
    0xe7, 0x4b, 0x10, 0xff, 0xff, 0xff, 0xff, 0xcd, 0xa9, 0x17, 0x90, 0xff,
    0xff, 0xff, 0xff, 0xce, 0xa2, 0x43, 0x10, 0xff, 0xff, 0xff, 0xff, 0xcf,
    0x92, 0x34, 0x10, 0xff, 0xff, 0xff, 0xff, 0xd0, 0x07, 0x5f, 0x60, 0xff,
    0xff, 0xff, 0xff, 0xd0, 0x6e, 0x42, 0x70, 0xff, 0xff, 0xff, 0xff, 0xd1,
    0x72, 0x16, 0x10, 0xff, 0xff, 0xff, 0xff, 0xd2, 0x4c, 0xd2, 0xf0, 0xff,
    0xff, 0xff, 0xff, 0xd3, 0x3e, 0x31, 0x90, 0xff, 0xff, 0xff, 0xff, 0xd4,
    0x49, 0xd2, 0x10, 0xff, 0xff, 0xff, 0xff, 0xd5, 0x1d, 0xf7, 0x70, 0xff,
    0xff, 0xff, 0xff, 0xd6, 0x29, 0x97, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xd6,
    0xeb, 0x80, 0x90, 0xff, 0xff, 0xff, 0xff, 0xd8, 0x09, 0x96, 0x10, 0xff,
    0xff, 0xff, 0xff, 0xf9, 0x33, 0xb5, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xf9,
    0xd9, 0xc4, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xfb, 0x1c, 0xd2, 0x70, 0xff,
    0xff, 0xff, 0xff, 0xfb, 0xb9, 0xa6, 0xe0, 0xff, 0xff, 0xff, 0xff, 0xfc,
    0xfc, 0xb4, 0x70, 0xff, 0xff, 0xff, 0xff, 0xfd, 0x99, 0x88, 0xe0, 0xff,
    0xff, 0xff, 0xff, 0xfe, 0xe5, 0xd0, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x82, 0xa5, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc5, 0xb2, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x62, 0x87, 0x60, 0x00, 0x00, 0x00, 0x00, 0x02,
    0x9c, 0x5a, 0x70, 0x00, 0x00, 0x00, 0x00, 0x03, 0x42, 0x77, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x04, 0x85, 0x76, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x05,
    0x2b, 0x85, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x06, 0x6e, 0x93, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x07, 0x0b, 0x67, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x08,
    0x45, 0x3a, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x08, 0xeb, 0x49, 0xe0, 0x00,
    0x00, 0x00, 0x00, 0x0a, 0x2e, 0x57, 0x70, 0x00, 0x00, 0x00, 0x00, 0x0a,
    0xcb, 0x39, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0e, 0x39, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x0c, 0xab, 0x1b, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x0d,
    0xe4, 0xe0, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x8a, 0xfd, 0xf0, 0x00,
    0x00, 0x00, 0x00, 0x0f, 0xcd, 0xfd, 0x70, 0x00, 0x00, 0x00, 0x00, 0x10,
    0x74, 0x1a, 0x70, 0x00, 0x00, 0x00, 0x00, 0x11, 0xad, 0xdf, 0x70, 0x00,
    0x00, 0x00, 0x00, 0x12, 0x53, 0xfc, 0x70, 0x00, 0x00, 0x00, 0x00, 0x12,
    0xce, 0x97, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x13, 0x4d, 0x44, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x14, 0x33, 0xfa, 0x90, 0x00, 0x00, 0x00, 0x00, 0x15,
    0x23, 0xeb, 0x90, 0x00, 0x00, 0x00, 0x00, 0x16, 0x13, 0xdc, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x17, 0x03, 0xcd, 0x90, 0x00, 0x00, 0x00, 0x00, 0x17,
    0xf3, 0xbe, 0x90, 0x00, 0x00, 0x00, 0x00, 0x18, 0xe3, 0xaf, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x19, 0xd3, 0xa0, 0x90, 0x00, 0x00, 0x00, 0x00, 0x1a,
    0xc3, 0x91, 0x90, 0x00, 0x00, 0x00, 0x00, 0x1b, 0xbc, 0xbd, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x1c, 0xac, 0xae, 0x10, 0x00, 0x00, 0x00, 0x00, 0x1d,
    0x9c, 0x9f, 0x10, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x8c, 0x90, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x1f, 0x7c, 0x81, 0x10, 0x00, 0x00, 0x00, 0x00, 0x20,
    0x6c, 0x72, 0x10, 0x00, 0x00, 0x00, 0x00, 0x21, 0x5c, 0x63, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x22, 0x4c, 0x54, 0x10, 0x00, 0x00, 0x00, 0x00, 0x23,
    0x3c, 0x45, 0x10, 0x00, 0x00, 0x00, 0x00, 0x24, 0x2c, 0x36, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x25, 0x1c, 0x27, 0x10, 0x00, 0x00, 0x00, 0x00, 0x26,
    0x0c, 0x18, 0x10, 0x00, 0x00, 0x00, 0x00, 0x27, 0x05, 0x43, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x27, 0xf5, 0x34, 0x90, 0x00, 0x00, 0x00, 0x00, 0x28,
    0xe5, 0x25, 0x90, 0x00, 0x00, 0x00, 0x00, 0x29, 0xd5, 0x16, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x2a, 0xc5, 0x07, 0x90, 0x00, 0x00, 0x00, 0x00, 0x2b,
    0xb4, 0xf8, 0x90, 0x00, 0x00, 0x00, 0x00, 0x2c, 0xa4, 0xe9, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x2d, 0x94, 0xda, 0x90, 0x00, 0x00, 0x00, 0x00, 0x2e,
    0x84, 0xcb, 0x90, 0x00, 0x00, 0x00, 0x00, 0x2f, 0x74, 0xbc, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x30, 0x64, 0xad, 0x90, 0x00, 0x00, 0x00, 0x00, 0x31,
    0x5d, 0xd9, 0x10, 0x00, 0x00, 0x00, 0x00, 0x32, 0x72, 0xb4, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x33, 0x3d, 0xbb, 0x10, 0x00, 0x00, 0x00, 0x00, 0x34,
    0x52, 0x96, 0x10, 0x00, 0x00, 0x00, 0x00, 0x35, 0x1d, 0x9d, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x36, 0x32, 0x78, 0x10, 0x00, 0x00, 0x00, 0x00, 0x36,
    0xfd, 0x7f, 0x10, 0x00, 0x00, 0x00, 0x00, 0x38, 0x1b, 0x94, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x38, 0xdd, 0x61, 0x10, 0x00, 0x00, 0x00, 0x00, 0x39,
    0xfb, 0x76, 0x90, 0x00, 0x00, 0x00, 0x00, 0x3a, 0xbd, 0x43, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x3b, 0xdb, 0x58, 0x90, 0x00, 0x00, 0x00, 0x00, 0x3c,
    0xa6, 0x5f, 0x90, 0x00, 0x00, 0x00, 0x00, 0x3d, 0xbb, 0x3a, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x3e, 0x86, 0x41, 0x90, 0x00, 0x00, 0x00, 0x00, 0x3f,
    0x9b, 0x1c, 0x90, 0x00, 0x00, 0x00, 0x00, 0x40, 0x66, 0x23, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x41, 0x84, 0x39, 0x10, 0x00, 0x00, 0x00, 0x00, 0x42,
    0x46, 0x05, 0x90, 0x00, 0x00, 0x00, 0x00, 0x43, 0x64, 0x1b, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x44, 0x25, 0xe7, 0x90, 0x00, 0x00, 0x00, 0x00, 0x45,
    0x43, 0xfd, 0x10, 0x00, 0x00, 0x00, 0x00, 0x46, 0x05, 0xc9, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x47, 0x23, 0xdf, 0x10, 0x00, 0x00, 0x00, 0x00, 0x47,
    0xee, 0xe6, 0x10, 0x00, 0x00, 0x00, 0x00, 0x49, 0x03, 0xc1, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x49, 0xce, 0xc8, 0x10, 0x00, 0x00, 0x00, 0x00, 0x4a,
    0xe3, 0xa3, 0x10, 0x00, 0x00, 0x00, 0x00, 0x4b, 0xae, 0xaa, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x4c, 0xcc, 0xbf, 0x90, 0x00, 0x00, 0x00, 0x00, 0x4d,
    0x8e, 0x8c, 0x10, 0x00, 0x00, 0x00, 0x00, 0x4e, 0xac, 0xa1, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x4f, 0x6e, 0x6e, 0x10, 0x00, 0x00, 0x00, 0x00, 0x50,
    0x8c, 0x83, 0x90, 0x00, 0x00, 0x00, 0x00, 0x51, 0x57, 0x8a, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x52, 0x6c, 0x65, 0x90, 0x00, 0x00, 0x00, 0x00, 0x53,
    0x37, 0x6c, 0x90, 0x00, 0x00, 0x00, 0x00, 0x54, 0x4c, 0x47, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x55, 0x17, 0x4e, 0x90, 0x00, 0x00, 0x00, 0x00, 0x56,
    0x2c, 0x29, 0x90, 0x00, 0x00, 0x00, 0x00, 0x56, 0xf7, 0x30, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x58, 0x15, 0x46, 0x10, 0x00, 0x00, 0x00, 0x00, 0x58,
    0xd7, 0x12, 0x90, 0x00, 0x00, 0x00, 0x00, 0x59, 0xf5, 0x28, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x5a, 0xb6, 0xf4, 0x90, 0x00, 0x00, 0x00, 0x00, 0x5b,
    0xd5, 0x0a, 0x10, 0x00, 0x00, 0x00, 0x00, 0x5c, 0xa0, 0x11, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x5d, 0xb4, 0xec, 0x10, 0x00, 0x00, 0x00, 0x00, 0x5e,
    0x7f, 0xf3, 0x10, 0x00, 0x00, 0x00, 0x00, 0x5f, 0x94, 0xce, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x60, 0x5f, 0xd5, 0x10, 0x00, 0x00, 0x00, 0x00, 0x61,
    0x7d, 0xea, 0x90, 0x00, 0x00, 0x00, 0x00, 0x62, 0x3f, 0xb7, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x63, 0x5d, 0xcc, 0x90, 0x00, 0x00, 0x00, 0x00, 0x64,
    0x1f, 0x99, 0x10, 0x00, 0x00, 0x00, 0x00, 0x65, 0x3d, 0xae, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x66, 0x08, 0xb5, 0x90, 0x00, 0x00, 0x00, 0x00, 0x67,
    0x1d, 0x90, 0x90, 0x00, 0x00, 0x00, 0x00, 0x67, 0xe8, 0x97, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x68, 0xfd, 0x72, 0x90, 0x00, 0x00, 0x00, 0x00, 0x69,
    0xc8, 0x79, 0x90, 0x00, 0x00, 0x00, 0x00, 0x6a, 0xdd, 0x54, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x6b, 0xa8, 0x5b, 0x90, 0x00, 0x00, 0x00, 0x00, 0x6c,
    0xc6, 0x71, 0x10, 0x00, 0x00, 0x00, 0x00, 0x6d, 0x88, 0x3d, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x6e, 0xa6, 0x53, 0x10, 0x00, 0x00, 0x00, 0x00, 0x6f,
    0x68, 0x1f, 0x90, 0x00, 0x00, 0x00, 0x00, 0x70, 0x86, 0x35, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x71, 0x51, 0x3c, 0x10, 0x00, 0x00, 0x00, 0x00, 0x72,
    0x66, 0x17, 0x10, 0x00, 0x00, 0x00, 0x00, 0x73, 0x31, 0x1e, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x74, 0x45, 0xf9, 0x10, 0x00, 0x00, 0x00, 0x00, 0x75,
    0x11, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x76, 0x2f, 0x15, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x76, 0xf0, 0xe2, 0x10, 0x00, 0x00, 0x00, 0x00, 0x78,
    0x0e, 0xf7, 0x90, 0x00, 0x00, 0x00, 0x00, 0x78, 0xd0, 0xc4, 0x10, 0x00,
    0x00, 0x00, 0x00, 0x79, 0xee, 0xd9, 0x90, 0x00, 0x00, 0x00, 0x00, 0x7a,
    0xb0, 0xa6, 0x10, 0x00, 0x00, 0x00, 0x00, 0x7b, 0xce, 0xbb, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x7c, 0x99, 0xc2, 0x90, 0x00, 0x00, 0x00, 0x00, 0x7d,
    0xae, 0x9d, 0x90, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x79, 0xa4, 0x90, 0x00,
    0x00, 0x00, 0x00, 0x7f, 0x8e, 0x7f, 0x90, 0x01, 0x03, 0x02, 0x03, 0x02,
    0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02,
    0x04, 0x03, 0x04, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x04, 0x05,
    0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05, 0x04, 0x05,
    0x04, 0x05, 0x04, 0x05, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03, 0x02, 0x03,
    0x02, 0x03, 0x05, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06,
    0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06,
    0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06,
    0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06,
    0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06,
    0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06,
    0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06,
    0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06,
    0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06,
    0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x06, 0x07, 0x00,
    0x00, 0x0b, 0xb4, 0x00, 0x00, 0x00, 0x00, 0x0b, 0xb4, 0x00, 0x04, 0x00,
    0x00, 0x1c, 0x20, 0x01, 0x08, 0x00, 0x00, 0x0e, 0x10, 0x00, 0x0d, 0x00,
    0x00, 0x1c, 0x20, 0x01, 0x08, 0x00, 0x00, 0x0e, 0x10, 0x00, 0x0d, 0x00,
    0x00, 0x1c, 0x20, 0x01, 0x08, 0x00, 0x00, 0x0e, 0x10, 0x00, 0x0d, 0x4c,
    0x4d, 0x54, 0x00, 0x52, 0x4d, 0x54, 0x00, 0x43, 0x45, 0x53, 0x54, 0x00,
    0x43, 0x45, 0x54, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x0a, 0x43, 0x45, 0x54,
    0x2d, 0x31, 0x43, 0x45, 0x53, 0x54, 0x2c, 0x4d, 0x33, 0x2e, 0x35, 0x2e,
    0x30, 0x2c, 0x4d, 0x31, 0x30, 0x2e, 0x35, 0x2e, 0x30, 0x2f, 0x33, 0x0a,
};

// GMT
static const unsigned char GMT_DATA[] = {
    0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x47, 0x4d, 0x54, 0x00, 0x00, 0x00, 0x54, 0x5a, 0x69, 0x66,
    0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x4d,
    0x54, 0x00, 0x00, 0x00, 0x0a, 0x47, 0x4d, 0x54, 0x30, 0x0a,
};

// Etc/GMT
static const unsigned char ETC_GMT_DATA[] = {
    0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x47, 0x4d, 0x54, 0x00, 0x00, 0x00, 0x54, 0x5a, 0x69, 0x66,
    0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x47, 0x4d,
    0x54, 0x00, 0x00, 0x00, 0x0a, 0x47, 0x4d, 0x54, 0x30, 0x0a,
};

// Etc/GMT+1
static const unsigned char ETC_GMTP1_DATA[] = {
    0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0xff, 0xff, 0xf1, 0xf0,
    0x00, 0x00, 0x47, 0x4d, 0x54, 0x2b, 0x31, 0x00, 0x00, 0x00, 0x54, 0x5a,
    0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0xff, 0xff, 0xf1, 0xf0, 0x00, 0x00,
    0x47, 0x4d, 0x54, 0x2b, 0x31, 0x00, 0x00, 0x00, 0x0a, 0x3c, 0x47, 0x4d,
    0x54, 0x2b, 0x31, 0x3e, 0x31, 0x0a,
};

// Etc/GMT+2
static const unsigned char ETC_GMTP2_DATA[] = {
    0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0xff, 0xff, 0xd5, 0xd0,
    0x00, 0x00, 0x47, 0x4d, 0x54, 0x2b, 0x33, 0x00, 0x00, 0x00, 0x54, 0x5a,
    0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0xff, 0xff, 0xd5, 0xd0, 0x00, 0x00,
    0x47, 0x4d, 0x54, 0x2b, 0x33, 0x00, 0x00, 0x00, 0x0a, 0x3c, 0x47, 0x4d,
    0x54, 0x2b, 0x33, 0x3e, 0x33, 0x0a,
};

// Etc/GMT-1
static const unsigned char ETC_GMTM1_DATA[] = {
    0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x0e, 0x10,
    0x00, 0x00, 0x47, 0x4d, 0x54, 0x2d, 0x31, 0x00, 0x00, 0x00, 0x54, 0x5a,
    0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x0e, 0x10, 0x00, 0x00,
    0x47, 0x4d, 0x54, 0x2d, 0x31, 0x00, 0x00, 0x00, 0x0a, 0x3c, 0x47, 0x4d,
    0x54, 0x2d, 0x31, 0x3e, 0x2d, 0x31, 0x0a,
};

// Asia/Riyadh
static const unsigned char ASIA_RIYADH_DATA[] = {
    0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0xda, 0x61, 0x36, 0xb4,
    0x01, 0x00, 0x00, 0x2b, 0xcc, 0x00, 0x00, 0x00, 0x00, 0x2a, 0x30, 0x00,
    0x04, 0x4c, 0x4d, 0x54, 0x00, 0x41, 0x53, 0x54, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x08, 0xff, 0xff, 0xff,
    0xff, 0xda, 0x61, 0x36, 0xb4, 0x01, 0x00, 0x00, 0x2b, 0xcc, 0x00, 0x00,
    0x00, 0x00, 0x2a, 0x30, 0x00, 0x04, 0x4c, 0x4d, 0x54, 0x00, 0x41, 0x53,
    0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0a, 0x41, 0x53, 0x54, 0x2d, 0x33,
    0x0a,
};

// Asia/Saigon
static const unsigned char ASIA_SAIGON_DATA[] = {
    0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0c, 0x88, 0x6f, 0x42, 0x80,
    0x91, 0x5f, 0xee, 0xd0, 0x93, 0x85, 0xb1, 0x90, 0xb7, 0x41, 0xbc, 0x00,
    0x01, 0x02, 0x03, 0x02, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x63, 0xec, 0x00, 0x04, 0x00, 0x00, 0x62, 0x70, 0x00, 0x08, 0x00, 0x00,
    0x70, 0x80, 0x00, 0x08, 0x4c, 0x4d, 0x54, 0x00, 0x53, 0x4d, 0x54, 0x00,
    0x49, 0x43, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x54, 0x5a, 0x69, 0x66, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x0c, 0xff, 0xff, 0xff, 0xff,
    0x88, 0x6f, 0x42, 0x80, 0xff, 0xff, 0xff, 0xff, 0x91, 0x5f, 0xee, 0xd0,
    0xff, 0xff, 0xff, 0xff, 0x93, 0x85, 0xb1, 0x90, 0xff, 0xff, 0xff, 0xff,
    0xb7, 0x41, 0xbc, 0x00, 0x01, 0x02, 0x03, 0x02, 0x00, 0x00, 0x64, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x63, 0xec, 0x00, 0x04, 0x00, 0x00, 0x62, 0x70,
    0x00, 0x08, 0x00, 0x00, 0x70, 0x80, 0x00, 0x08, 0x4c, 0x4d, 0x54, 0x00,
    0x53, 0x4d, 0x54, 0x00, 0x49, 0x43, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0a, 0x49, 0x43, 0x54, 0x2d, 0x37, 0x0a,
};

// ============================================================================
//                        GLOBAL CLASSES FOR TESTING
// ----------------------------------------------------------------------------

struct LogVerbosityGuard {
    // The Logger verbosity guard disables logging on construction, and
    // re-enables logging, based on the prior default pass-through level, when
    // it goes out of scope and is destroyed.  It is intended to suppress
    // logged output for intentional errors when the test driver is run in
    // non-verbose mode.

    bool                    d_verbose;             // verbose mode does not
                                                   // disable logging

    bsls::LogSeverity::Enum d_defaultPassthrough;  // default passthrough
                                                   // log level

    explicit LogVerbosityGuard(bool verbose = false)
        // If the optionally specified 'verbose' is 'false' disable logging
        // until this guard is destroyed.
    {
        d_verbose            = verbose;
        d_defaultPassthrough = bsls::Log::severityThreshold();

        if (!d_verbose) {
            bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_FATAL);
        }
    }

    ~LogVerbosityGuard()
        // Set the logging verbosity back to its default state.
    {
        if (!d_verbose) {
            bsls::Log::setSeverityThreshold(d_defaultPassthrough);
        }
    }
};

// ============================================================================
//                             HELPER FUNCTIONS
// ----------------------------------------------------------------------------
static bdlt::Datetime fromTimeT(bdlt::EpochUtil::TimeT64 value)
    // Return the 'bdlt::Datetime' representation of the interval in seconds
    // from UNIX epoch time of the specified 'value'.  Note that this method is
    // shorthand for 'bdlt::EpochUtil::convertFromTimeT64'.
{
    return bdlt::EpochUtil::convertFromTimeT64(value);
}

static bdlt::EpochUtil::TimeT64 toTimeT(const bdlt::Datetime& value)
    // Return the interval in seconds from UNIX epoch time of the specified
    // 'value'.  Note that this method is shorthand for
    // 'bdlt::EpochUtil::convertToTimeT64'.
{
    return bdlt::EpochUtil::convertToTimeT64(value);
}

static bdlt::Datetime fromIso8601 (const char *iso8601TimeString)
    // Return the datetime value indicated by the specified
    // 'iso8601TimeString'.  The behavior is undefined unless
    // 'iso8601TimeString' is a null-terminated C - string containing a time
    // description matching the iso8601 specification (see 'bdlt_iso8601util').
{
    bdlt::Datetime time;
    int rc = bdlt::Iso8601Util::parse(&time,
                            iso8601TimeString,
                            static_cast<int>(bsl::strlen (iso8601TimeString)));
    BSLS_ASSERT (0 == rc);
    return time;
}

struct TransitionDescription {
    // A 'struct' describing a transitions.  Note that this type is meant to
    // be used to create data tables for use with 'addTransitions'.

   int         d_line;
   const char *d_transitionTime;
   int         d_offsetMins;
   const char *d_abbrev;
   bool        d_isDst;
};

static void addTransitions(baltzo::Zoneinfo            *result,
                           const TransitionDescription *descriptions,
                           int                          numDescriptions)
    // Insert to the specified 'result' the contiguous sequence of specified
    // 'descriptions', of length 'numDescriptions'.

{
    BSLS_ASSERT(result);
    for (int i = 0; i < numDescriptions; ++i) {
        const char *TRANS = descriptions[i].d_transitionTime;
        baltzo::LocalTimeDescriptor desc(descriptions[i].d_offsetMins * 60,
                                        descriptions[i].d_isDst,
                                        descriptions[i].d_abbrev);
        result->addTransition(toTimeT(fromIso8601(TRANS)), desc);
     }
}


// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int             test = argc > 1 ? atoi(argv[1]) : 0;
    bool         verbose = argc > 2;
    bool     veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma::TestAllocator allocator, defaultAllocator;
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);
    bslma::TestAllocator *Z = &allocator;

    baltzo::TestLoader     testLoader(Z);
    baltzo::ZoneinfoCache  testCache(&testLoader, Z);

    if (verbose) cout << "Create synthetic data" << endl;

    if (veryVerbose) cout <<"\tCreate \"ALLDST\" time zone" << endl;
    baltzo::Zoneinfo allDstTimeZone(Z);
    {
        static const TransitionDescription DESC[] = {
            // LINE  TRANSITION            OFF  DESC     DST
            // ---- ---------------------  ---- -------  ----
             { L_,  "0001-01-01T00:00:00",  60, "ADST1", true },
             { L_,  "2000-01-01T00:00:00", 120, "ADST2", true },
             { L_,  "2001-01-01T00:00:00",  60, "ADST3", true },
        };
        const int NUM_TRANSITIONS = sizeof(DESC) / sizeof(DESC[0]);

        allDstTimeZone.setIdentifier(ALLDST);
        addTransitions(&allDstTimeZone, DESC, NUM_TRANSITIONS);
    }

    if (veryVerbose) cout <<"\tCreate \"OLDDST\" time zone" << endl;
    baltzo::Zoneinfo oldDstTimeZone(Z);
    {
        static const TransitionDescription DESC[] = {
            // LINE  TRANSITION            OFF  DESC     DST
            // ---   --------------------  ---- -------  -----
             { L_,  "0001-01-01T00:00:00", 240, "ODST1", true  },
             { L_,  "2000-01-01T00:00:00", 120, "ODST2", false },
             { L_,  "2001-01-01T00:00:00",  60, "ODST3", false },
        };
        const int NUM_TRANSITIONS = sizeof(DESC) / sizeof(DESC[0]);

        oldDstTimeZone.setIdentifier(OLDDST);
        addTransitions(&oldDstTimeZone, DESC, NUM_TRANSITIONS);
    }

    ASSERT(0 == testLoader.setTimeZone(
                                     "GMT",
                                     reinterpret_cast<const char  *>(GMT_DATA),
                                     sizeof(GMT_DATA)));
    ASSERT(0 == testLoader.setTimeZone(
                                 GMT,
                                 reinterpret_cast<const char  *>(ETC_GMT_DATA),
                                 sizeof(ETC_GMT_DATA)));
    ASSERT(0 == testLoader.setTimeZone(
                               GP1,
                               reinterpret_cast<const char  *>(ETC_GMTP1_DATA),
                               sizeof(ETC_GMTP1_DATA)));
    ASSERT(0 == testLoader.setTimeZone(
                               GP2,
                               reinterpret_cast<const char  *>(ETC_GMTP2_DATA),
                               sizeof(ETC_GMTP2_DATA)));
    ASSERT(0 == testLoader.setTimeZone(
                               GM1,
                               reinterpret_cast<const char  *>(ETC_GMTM1_DATA),
                               sizeof(ETC_GMTM1_DATA)));
    ASSERT(0 == testLoader.setTimeZone(
                        NY,
                        reinterpret_cast<const char  *>(AMERICA_NEW_YORK_DATA),
                        sizeof(AMERICA_NEW_YORK_DATA)));
    ASSERT(0 == testLoader.setTimeZone(
                             RY,
                             reinterpret_cast<const char  *>(ASIA_RIYADH_DATA),
                             sizeof(ASIA_RIYADH_DATA)));
    ASSERT(0 == testLoader.setTimeZone(
                             SA,
                             reinterpret_cast<const char  *>(ASIA_SAIGON_DATA),
                             sizeof(ASIA_SAIGON_DATA)));
    ASSERT(0 == testLoader.setTimeZone(
                             RM,
                             reinterpret_cast<const char  *>(EUROPE_ROME_DATA),
                             sizeof(EUROPE_ROME_DATA)));

    // Synthetic
    testLoader.setTimeZone(allDstTimeZone);
    testLoader.setTimeZone(oldDstTimeZone);

    ASSERT(0 != testCache.getZoneinfo("GMT"));
    ASSERT(0 != testCache.getZoneinfo(GMT));
    ASSERT(0 != testCache.getZoneinfo(GP1));
    ASSERT(0 != testCache.getZoneinfo(GP2));
    ASSERT(0 != testCache.getZoneinfo(GM1));
    ASSERT(0 != testCache.getZoneinfo(NY));
    ASSERT(0 != testCache.getZoneinfo(RY));
    ASSERT(0 != testCache.getZoneinfo(SA));
    ASSERT(0 != testCache.getZoneinfo(RM));
    ASSERT(0 != testCache.getZoneinfo(ALLDST));
    ASSERT(0 != testCache.getZoneinfo(OLDDST));

    // Install a cache with and empty test loader, to ensure that the default
    // time zone cache is not accidentally used in any test.

    baltzo::TestLoader badLoader(Z);
    baltzo::ZoneinfoCache badCache(&badLoader, Z);
    baltzo::DefaultZoneinfoCache::setDefaultCache(&badCache);

    switch (test) { case 0:
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING USAGE EXAMPLE" << endl
                                  << "=====================" << endl;

///Usage
///-----
// The following examples demonstrate how to use a 'baltzo::TimeZoneUtilImp' to
// perform common operations on time values:
//
///Prologue: Initializing a Example 'baltzo::ZoneinfoCache' object.
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Before using the methods provided by 'baltzo::TimeZoneUtilImp' we must first
// define a 'baltzo::ZoneinfoCache' object containing information about various
// time zones.  For the purposes of this example, we will define a sample
// cache containing only data for New York loaded through a
// 'baltzo::TestLoader' object.  Note that, in general, clients should use data
// from an external data source (see 'baltzo_datafileloader').
//
// First, we create a Zoneinfo object for New York, and populate 'newYork' with
// the correct time zone identifier:
//..
    baltzo::Zoneinfo newYork;
    newYork.setIdentifier("America/New_York");
//..
// Next we create two local-time descriptors, one for standard time and one
// for daylight-saving time:
//..
    baltzo::LocalTimeDescriptor est(-18000, false, "EST");
    baltzo::LocalTimeDescriptor edt(-14400, true,  "EDT");
//..
// Then we set the initial descriptor for 'newYork' to Eastern Standard
// Time.  Note that such an initial transition is required for a
// 'baltzo::Zoneinfo' object to be considered Well-Defined (see
// 'baltzo_zoneinfoutil'):
//..
    newYork.addTransition(bdlt::EpochUtil::convertToTimeT64(
                                                      bdlt::Datetime(1, 1, 1)),
                          est);
//..
// Next we create a series of transitions between these local-time descriptors
// for the years 2007-2011.  Note that the United States transitions to
// daylight saving time on the second Sunday in March, at 2am local time
// (07:00 UTC), and transitions back to standard time on the first Sunday in
// November at 2am local time (06:00 UTC), resulting in an even number of
// transitions:
//..
    static const bdlt::Datetime TRANSITION_TIMES[] = {
        bdlt::Datetime(2007,  3, 11, 7),
        bdlt::Datetime(2007, 11,  4, 6),
        bdlt::Datetime(2008,  3,  9, 7),
        bdlt::Datetime(2008, 11,  2, 6),
        bdlt::Datetime(2009,  3,  8, 7),
        bdlt::Datetime(2009, 11,  1, 6),
        bdlt::Datetime(2010,  3, 14, 7),
        bdlt::Datetime(2010, 11,  7, 6),
        bdlt::Datetime(2011,  3, 13, 7),
        bdlt::Datetime(2011, 11,  6, 6),
    };
    const int NUM_TRANSITION_TIMES =
                            sizeof TRANSITION_TIMES / sizeof *TRANSITION_TIMES;
    ASSERT(0 == NUM_TRANSITION_TIMES % 2);
//
    for (int i = 0; i < NUM_TRANSITION_TIMES; i += 2) {
        newYork.addTransition(bdlt::EpochUtil::convertToTimeT64(
                                                          TRANSITION_TIMES[i]),
                              edt);
        newYork.addTransition(bdlt::EpochUtil::convertToTimeT64(
                                                      TRANSITION_TIMES[i + 1]),
                              est);
    }
//..
// Next, we verify that the time zone information we have created is
// considered well-defined (as discussed above):
//..
    ASSERT(true == baltzo::ZoneinfoUtil::isWellFormed(newYork));
//..
// Finally, we create a 'baltzo::TestLoader' object, provide it the description
// of 'newYork', and use it to initialize a 'baltzo::ZoneinfoCache' object:
// definition for New York:
//..
    baltzo::TestLoader loader;
    loader.setTimeZone(newYork);
    baltzo::ZoneinfoCache cache(&loader);
//..
//
///Example 1: Converting from a UTC time to a local time.
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we demonstrate how to convert a UTC time to the
// corresponding local time using the 'convertUtcToLocalTime' class method.
//
// We start by creating a 'bdlt::Datetime' representing the UTC time
// "Dec 12, 2010 15:00":
//..
    bdlt::Datetime utcTime(2010, 12, 12, 15, 0, 0);
//..
// Now, we call 'convertUtcToLocalTime' and supply as input 'utcTime', the
// time zone identifier for New York ("America/New_York"), and the cache of
// time zone information created in the prologue:
//..
    bdlt::DatetimeTz localNYTime;
    baltzo::TimeZoneUtilImp::convertUtcToLocalTime(&localNYTime,
                                                  "America/New_York",
                                                  utcTime,
                                                  &cache);
//..
// Finally we verify that 'localNYTime' is "Dec 12, 2010 10:00+5:00", the time
// in New York corresponding to the UTC time "Dec 12, 2010 15:00".
//..
    ASSERT(utcTime                         == localNYTime.utcDatetime());
    ASSERT(bdlt::Datetime(2010, 12, 12, 10) == localNYTime.localDatetime());
    ASSERT(-5 * 60                         == localNYTime.offset());
//..

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'loadLocalTimePeriodForUtc':
        //
        // Concerns:
        //: 1 The datetime range indicated by 'result' is the correct one.
        //:
        //: 2 The local-time descriptor held by 'result' has the correct value.
        //:
        //: 3 Return 'Err::k_UNSUPPORTED_ID' if an invalid time zone id is
        //:   passed.
        //:
        //: 4 Does not return 0 or 'Err::k_UNSUPPORTED_ID' if another error
        //:   occurs.
        //
        // Plan:
        // 1 Invoke 'loadLocalTimePeriodForUtc' passing an invalid time zone id
        //   and check the result.  (C-3)
        //
        // 2 Invoke 'loadLocalTimePeriodForUtc' passing an invalid time zone
        //   cache and check the result.  (C-4)
        //
        // 3 Using the table-driven technique:
        //   1 Specify a set values (one per row), including (a)
        //     time zone id, (b) utc time values to be passed in as input,
        //     (c) expected start and end utc datetimes of 'result', and (d)
        //     the attribute values describing the expected local-time
        //     descriptor held by 'result'.  (C-1,2)
        //
        // Testing:
        //   'loadLocalTimePeriodForUtc(...)
        // --------------------------------------------------------------------
        if (verbose) cout << "Testing 'loadLocalTimePeriodForUtc.'" << endl
                          << "====================================" << endl;

        if (verbose) cout << "\nTesting an invalid time zone id." << endl;
        {
            // Test with an invalid time zone id.

            LogVerbosityGuard guard;
            bdlt::Datetime VALID_INPUT(2010, 1, 1, 12, 0);

            baltzo::LocalTimePeriod result;
            ASSERT(EUID == Obj::loadLocalTimePeriodForUtc(&result,
                                                          "bogusId",
                                                          VALID_INPUT,
                                                          &testCache));
        }

        if (verbose) cout << "\nTesting an invalid loader." << endl;
        {
            // Test with a cache having an invalid loader

            LogVerbosityGuard guard(true);
            bdlt::Datetime VALID_INPUT(2010, 1, 1, 12, 0);

            baltzo::DataFileLoader bogusLoader(Z);
            bogusLoader.configureRootPath("STEFANOBOGUSPATH");
            baltzo::ZoneinfoCache bogusCache(&bogusLoader, Z);

            baltzo::LocalTimePeriod result;
            const int RC = Obj::loadLocalTimePeriodForUtc(&result,
                                                          "America/New_York",
                                                          VALID_INPUT,
                                                          &bogusCache);
            ASSERT(EUID != RC);
            ASSERT(0    != RC);
        }

        if (verbose) cout <<
            "\nCreate a table of input and expected output values." << endl;

        struct TestData {
            int             d_line;
            const char     *d_timeZoneId;
            const char     *d_input;                  // iso8601 w/o tz offset
            const char     *d_expectedStart;          // iso8601 w/o tz offset
            const char     *d_expectedEnd;            // iso8601 w/o tz offset
            const int       d_expectedOffsetFromUtc;
            bool            d_expectedDst;
            const char     *d_expectedDescription;
        } VALUES[] = {
// test well-defined values
 { __LINE__,  NY, "2010-01-01T12:00:00", "2009-11-01T06:00:00",
                         "2010-03-14T07:00:00",       -18000, false, "EST"   },

 { __LINE__,  NY, "2010-01-01T00:00:00", "2009-11-01T06:00:00",
                         "2010-03-14T07:00:00",       -18000, false, "EST"   },

// test around a fall (DST->STD) transition
 { __LINE__,  NY, "2009-11-01T05:59:59.999",  "2009-03-08T07:00:00",
                         "2009-11-01T06:00:00",       -14400,  true, "EDT"   },

 { __LINE__,  NY, "2009-11-01T06:00:00.000", "2009-11-01T06:00:00",
                         "2010-03-14T07:00:00",       -18000, false, "EST"   },

 { __LINE__,  NY, "2009-11-01T06:00:00.001", "2009-11-01T06:00:00",
                         "2010-03-14T07:00:00",       -18000, false, "EST"   },

// test around a spring (STD->DST) transition
 { __LINE__,  NY, "2010-03-14T06:59:59.999", "2009-11-01T06:00:00",
                         "2010-03-14T07:00:00",       -18000, false, "EST"   },

 { __LINE__,  NY, "2010-03-14T07:00:00.000", "2010-03-14T07:00:00",
                         "2010-11-07T06:00:00",       -14400,  true, "EDT"   },

 { __LINE__,  NY, "2010-03-14T07:00:00.001", "2010-03-14T07:00:00",
                         "2010-11-07T06:00:00",       -14400,  true, "EDT"   },

// Trivial time zones (GMT, GMT+1, GMT-1)
 { __LINE__,  GMT, "2010-01-01T12:00:00", "0001-01-01T00:00:00",
                         "9999-12-31T23:59:59.999999",     0, false, "GMT"   },

 { __LINE__,  GMT, "2010-01-01T00:00:00", "0001-01-01T00:00:00",
                         "9999-12-31T23:59:59.999999",     0, false, "GMT"   },

 { __LINE__,  GP1, "2010-01-01T12:00:00", "0001-01-01T00:00:00",
                         "9999-12-31T23:59:59.999999", -3600, false, "GMT+1" },

 { __LINE__,  GP1, "2010-01-01T00:00:00", "0001-01-01T00:00:00",
                         "9999-12-31T23:59:59.999999", -3600, false, "GMT+1" },

 { __LINE__,  GM1, "2010-01-01T12:00:00", "0001-01-01T00:00:00",
                         "9999-12-31T23:59:59.999999",  3600, false, "GMT-1" },

 { __LINE__,  GM1, "2010-01-01T00:00:00", "0001-01-01T00:00:00",
                         "9999-12-31T23:59:59.999999",  3600, false, "GMT-1" },


// Time zone with 1 transition (2 descriptors) (Riyadh)
 { __LINE__,  RY, "1949-12-31T20:53:07.999", "0001-01-01T00:00:00",
                         "1949-12-31T20:53:08",        11212, false, "LMT"   },

 { __LINE__,  RY, "1949-12-31T20:53:08.000", "1949-12-31T20:53:08",
                         "9999-12-31T23:59:59.999999", 10800, false, "AST"   },

 { __LINE__,  RY, "1949-12-31T20:53:09.000", "1949-12-31T20:53:08",
                         "9999-12-31T23:59:59.999999", 10800, false, "AST"   },

 { __LINE__,  RY, "1950-12-31T20:53:07.999","1949-12-31T20:53:08",
                         "9999-12-31T23:59:59.999999", 10800, false, "AST"   },

 { __LINE__,  RY, "1950-12-31T20:53:08.000", "1949-12-31T20:53:08",
                         "9999-12-31T23:59:59.999999", 10800, false, "AST"   },

 { __LINE__,  RY, "1950-12-31T20:53:09.000", "1949-12-31T20:53:08",
                         "9999-12-31T23:59:59.999999", 10800, false, "AST"   },
        };

        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            const int             LINE         = VALUES[i].d_line;
            const char           *TIME_ZONE_ID = VALUES[i].d_timeZoneId;

            const bdlt::Datetime   INPUT_TIME   =
                                     fromIso8601(VALUES[i].d_input);
            const bdlt::Datetime   EXP_START    =
                                     fromIso8601(VALUES[i].d_expectedStart);
            const bdlt::Datetime   EXP_END      =
                                     fromIso8601(VALUES[i].d_expectedEnd);

            const Descriptor EXP_DESC(VALUES[i].d_expectedOffsetFromUtc,
                                      VALUES[i].d_expectedDst,
                                      VALUES[i].d_expectedDescription);

            if (veryVerbose)  {
                T_ P_(LINE) P(INPUT_TIME)
                T_ P_(EXP_START) P_(EXP_END) P(EXP_DESC)
            }

            baltzo::LocalTimePeriod result;
            const int RC = Obj::loadLocalTimePeriodForUtc(&result,
                                                          TIME_ZONE_ID,
                                                          INPUT_TIME,
                                                          &testCache);
            ASSERT(0 == RC);

            const bdlt::Datetime& START = result.utcStartTime();
            const bdlt::Datetime& END   = result.utcEndTime();
            const Descriptor&    DESC  = result.descriptor();

            LOOP2_ASSERT(LINE, START,          EXP_START    == START);
            LOOP2_ASSERT(LINE, END,            EXP_END      == END);
            LOOP2_ASSERT(LINE, DESC,           EXP_DESC     == DESC);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) cout <<
                         "\t'loadLocalTimePeriodForUtc' class method " << endl;
            {
                baltzo::LocalTimePeriod result;
                bdlt::Datetime VALID_INPUT(2010, 1, 1, 12, 0);

                ASSERT_PASS(Obj::loadLocalTimePeriodForUtc(&result,
                                                           NY,
                                                           VALID_INPUT,
                                                           &testCache));

                ASSERT_FAIL(Obj::loadLocalTimePeriodForUtc(0,
                                                           NY,
                                                           VALID_INPUT,
                                                           &testCache));

                ASSERT_FAIL(Obj::loadLocalTimePeriodForUtc(&result,
                                                           0,
                                                           VALID_INPUT,
                                                           &testCache));

                ASSERT_FAIL(Obj::loadLocalTimePeriodForUtc(&result,
                                                           NY,
                                                           VALID_INPUT,
                                                           0));
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'createLocalTimePeriod':
        //
        // Concerns:
        //: 1 The datetime range of the local-time period corresponds to the
        //:   specified start and end date.
        //:
        //: 2 The local-time descriptor held by the local-time period populated
        //:   by 'createLocalTimePeriod' has the same value as the one pointed
        //:   by 'transition'.
        //:
        //: 4 If the input iterator refers to a transition is the last
        //:   transition in 'timeZone', the end date-time of 'result' is
        //:   'Dec 31, 9999 23:59:59.999.999'.
        //:
        //: 5 If the input iterator refers to a transition is the first
        //:   transition in 'timeZone', the start date-time of 'result' is
        //:   'Jan 1, 1 00:00:00.000'.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:   1 Specify a set of values (one per row) including (a)
        //:     time zone id, (b) local time values to be passed in as input,
        //:     (c) DST policy passed in as input, (d) expected validity status
        //:     of the input local time, (e) start and end utc datetimes of
        //:     'result', and (f) the attribute values describing the expected
        //:     local-time descriptor held by 'result'.  (C-1,2,3,4,5)
        //
        // Testing:
        //   'createLocalTimePeriod(Period *, TransitionConstIter, Zoneinfo)'
        // --------------------------------------------------------------------
        if (verbose) cout << "Testing 'createLocalTimePeriod'." << endl
                          << "================================" << endl;

        if (verbose) cout << "\nCreate a table of time zones to test." << endl;

        struct TestData {
            int         d_line;
            const char *d_timeZoneId;
        } VALUES[] = {
            { __LINE__, NY  },
            { __LINE__, GMT },
            { __LINE__, GP1 },
            { __LINE__, GM1 },
            { __LINE__, SA  },
        };

        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            const char *TIME_ZONE_ID = VALUES[i].d_timeZoneId;

            const baltzo::Zoneinfo *currentTimeZone =
                                        testCache.lookupZoneinfo(TIME_ZONE_ID);

            ASSERT(currentTimeZone);

            const bdlt::Datetime FIRST(1, 1, 1);
            const bdlt::Datetime LAST(9999, 12, 31, 23, 59, 59, 999, 999);

            for (Iterator it = currentTimeZone->beginTransitions();
                          it != currentTimeZone->endTransitions();
                 ++it)
            {
                baltzo::LocalTimePeriod result;
                Obj::createLocalTimePeriod(&result, it, *currentTimeZone);

                const bdlt::Datetime& START = result.utcStartTime();
                const bdlt::Datetime& END   = result.utcEndTime();
                const Descriptor&     DESC  = result.descriptor();

                // Test the start date of the local-time period looking at the
                // transition time to the current local time.

                if (currentTimeZone->beginTransitions() == it) {
                    LOOP_ASSERT(START, FIRST == START);
                }
                else {
                    LOOP_ASSERT(START, fromTimeT(it->utcTime()) == START);
                }

                // Test the start date of the local-time period looking at the
                // next transition time, when the current local time ends.

                Iterator nextIt = it;
                nextIt++;
                if (currentTimeZone->endTransitions() == nextIt) {
                    LOOP_ASSERT(END, LAST == END);
                }
                else {
                    LOOP_ASSERT( END, fromTimeT(nextIt->utcTime()) == END);
                }

                // Double check that the descriptor of transition referred by
                // 'it' has the same value then the one loaded in 'result'.

                LOOP_ASSERT(DESC, it->descriptor() == DESC);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) cout <<
                             "\t'createLocalTimePeriod' class method " << endl;
            {
                baltzo::LocalTimePeriod result;

                const baltzo::Zoneinfo BADZI;
                const baltzo::Zoneinfo *NYZI   = testCache.lookupZoneinfo(NY);
                const Iterator        ITER    = NYZI->beginTransitions();
                const Iterator        BADITER = NYZI->endTransitions();

                ASSERT_PASS(Obj::createLocalTimePeriod(&result,
                                                       ITER,
                                                       *NYZI));

                ASSERT_FAIL(Obj::createLocalTimePeriod(0,
                                                       ITER,
                                                       *NYZI));

                ASSERT_FAIL(Obj::createLocalTimePeriod(&result,
                                                       BADITER,
                                                       *NYZI));

                ASSERT_FAIL(Obj::createLocalTimePeriod(&result,
                                                       ITER,
                                                       BADZI));
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'initLocalTime':
        //
        // Concerns:
        //: 1 The parameters are correctly forwarded to 'resolveLocalTime'.
        //:
        //: 3 Return 'Err::k_UNSUPPORTED_ID' if an invalid time zone id is
        //:   passed.
        //:
        //: 4 Does not return 0 or 'Err::k_UNSUPPORTED_ID' if another error
        //:   occurs.
        //
        // Plan:
        //: 1 Invoke 'initLocalTime' passing an invalid time zone id
        //:   and check the result.  (C-6)
        //:
        //: 2 Invoke 'initLocalTime' passing an invalid time zone
        //:   cache and check the result.  (C-7)
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set values (one per row), including (a) time zone id,
        //:     (b) local time values to be passed in as input, (c) DST policy
        //:     passed in as input, (d) the expected validity of the input
        //:     time, and (e) the expected local time value to be loaded in the
        //:     result (C-1).
        //
        // Testing:
        //   'initLocalTime(DatetimeTz *, Datetime& , char *, Dst, Cache *)
        // --------------------------------------------------------------------
        if (verbose) cout << "Testing 'initLocalTime'" << endl
                          << "=======================" << endl;

        if (verbose) cout << "\nTesting an invalid time zone id." << endl;
        {
            // Test with an invalid time zone id.

            LogVerbosityGuard guard;
            bdlt::Datetime VALID_INPUT(2010, 1, 1, 12, 0);

            bdlt::DatetimeTz resultTz;
            Validity::Enum  resultValidity;
            ASSERT(EUID == Obj::initLocalTime(&resultTz,
                                              &resultValidity,
                                              VALID_INPUT,
                                              "bogusId",
                                              DU,
                                              &testCache));
        }

        if (verbose) cout << "\nTesting an invalid loader." << endl;
        {
            // Test with a cache having an invalid loader

            LogVerbosityGuard guard(true);
            const bdlt::Datetime VALID_INPUT(2010, 1, 1, 12, 0);

            baltzo::DataFileLoader bogusLoader(Z);
            bogusLoader.configureRootPath("STEFANOBOGUSPATH");
            baltzo::ZoneinfoCache bogusCache(&bogusLoader, Z);

            bdlt::DatetimeTz resultTz;
            Validity::Enum  resultValidity;
            const int RC = Obj::initLocalTime(&resultTz,
                                              &resultValidity,
                                              VALID_INPUT,
                                              "America/New_York",
                                              DU,
                                              &bogusCache);
            ASSERT(EUID != RC);
            ASSERT(0    != RC);
        }

///Result of 'initLocalTime' for Various 'DstPolicy'  Values.
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//..
//                  (result format: "local time +- offset")
// ,-----------------------------------------------------------------------.
// | Input in New York  |                   DstPolicy                      |
// |     Local Time     |--------------------------------------------------|
// |                    | *_UNSPECIFIED  |      *_DST    |   *_STANDARD    |
// |=======================================================================|
// | Jan  1, 2010 01:30 | 01:30:00-5:00  | 00:30:00-5:00 | 01:30:00-5:00   |
// |   (standard time)  |                |      [1]      |                 |
// |--------------------|--------------------------------------------------|
// | Mar 14, 2010 02:30 | 03:30:00-4:00  | 01:30:00-5:00 | 03:30:00-4:00   |
// |     (invalid)      |                |      [2]      |      [2]        |
// |--------------------|--------------------------------------------------|
// | Apr  1, 2010 01:30 | 01:30:00-4:00  | 01:30:00-4:00 | 02:30:00-4:00   |
// | (daylight-saving)  |                |               |      [1]        |
// |--------------------|--------------------------------------------------|
// | Nov  7, 2010 01:30 | 01:30:00-5:00  | 01:30:00-4:00 | 01:30:00-5:00   |
// |    (ambiguous)     |                |               |                 |
// `-----------------------------------------------------------------------'

        if (verbose) cout <<
            "\nCreate a table of input and expected output values." << endl;


        struct TestData {
            int             d_line;
            const char     *d_timeZoneId;
            const char     *d_input;             // iso8601 w/o tz offset
            int             d_policy;            // Obj::DstPolicy
            Validity::Enum  d_expectedValidity;
            const char     *d_expectedResult;    // iso8601 w/ tz offset
         } VALUES[] = {
// DST->STD
 { __LINE__,  NY, "2009-11-01T00:59:59.999", DU, VU,
                                             "2009-11-01T00:59:59.999-04:00" },

 { __LINE__,  NY, "2009-11-01T00:59:59.999", DD, VU,
                                             "2009-11-01T00:59:59.999-04:00" },

 { __LINE__,  NY, "2009-11-01T00:59:59.999", DS, VU,
                                             "2009-11-01T01:59:59.999-04:00" },

 { __LINE__,  NY, "2009-11-01T01:00:00.000", DU, VA,
                                             "2009-11-01T01:00:00.000-05:00" },

 { __LINE__,  NY, "2009-11-01T01:00:00.000", DD, VA,
                                             "2009-11-01T01:00:00.000-04:00" },

 { __LINE__,  NY, "2009-11-01T01:00:00.000", DS, VA,
                                             "2009-11-01T01:00:00.000-05:00" },

 { __LINE__,  NY, "2009-11-01T01:00:00.001", DU, VA,
                                             "2009-11-01T01:00:00.001-05:00" },

 { __LINE__,  NY, "2009-11-01T01:00:00.001", DD, VA,
                                             "2009-11-01T01:00:00.001-04:00" },

 { __LINE__,  NY, "2009-11-01T01:00:00.001", DS, VA,
                                             "2009-11-01T01:00:00.001-05:00" },

 { __LINE__,  NY, "2009-11-01T01:59:59.999", DU, VA,
                                             "2009-11-01T01:59:59.999-05:00" },

 { __LINE__,  NY, "2009-11-01T01:59:59.999", DD, VA,
                                             "2009-11-01T01:59:59.999-04:00" },

 { __LINE__,  NY, "2009-11-01T01:59:59.999", DS, VA,
                                             "2009-11-01T01:59:59.999-05:00" },

 { __LINE__,  NY, "2009-11-01T02:00:00.000", DU, VU,
                                             "2009-11-01T02:00:00.000-05:00" },

 { __LINE__,  NY, "2009-11-01T02:00:00.000", DD, VU,
                                             "2009-11-01T01:00:00.000-05:00" },

 { __LINE__,  NY, "2009-11-01T02:00:00.000", DS, VU,
                                             "2009-11-01T02:00:00.000-05:00" },

 { __LINE__,  NY, "2009-11-01T02:00:00.001", DU, VU,
                                             "2009-11-01T02:00:00.001-05:00" },

 { __LINE__,  NY, "2009-11-01T02:00:00.001", DD, VU,
                                             "2009-11-01T01:00:00.001-05:00" },

 { __LINE__,  NY, "2009-11-01T02:00:00.001", DS, VU,
                                             "2009-11-01T02:00:00.001-05:00" },

 // STD->DST
 { __LINE__,  NY, "2010-01-01T01:30:00", DU, VU, "2010-01-01T01:30:00-05:00" },
 { __LINE__,  NY, "2010-01-01T01:30:00", DD, VU, "2010-01-01T00:30:00-05:00" },
 { __LINE__,  NY, "2010-01-01T01:30:00", DS, VU, "2010-01-01T01:30:00-05:00" },

 { __LINE__,  NY, "2010-03-14T02:30:00", DU, VI, "2010-03-14T03:30:00-04:00" },
 { __LINE__,  NY, "2010-03-14T02:30:00", DD, VI, "2010-03-14T01:30:00-05:00" },
 { __LINE__,  NY, "2010-03-14T02:30:00", DS, VI, "2010-03-14T03:30:00-04:00" },

 { __LINE__,  NY, "2010-04-01T01:30:00", DU, VU, "2010-04-01T01:30:00-04:00" },
 { __LINE__,  NY, "2010-04-01T01:30:00", DD, VU, "2010-04-01T01:30:00-04:00" },
 { __LINE__,  NY, "2010-04-01T01:30:00", DS, VU, "2010-04-01T02:30:00-04:00" },

 { __LINE__,  NY, "2010-11-07T01:30:00", DU, VA, "2010-11-07T01:30:00-05:00" },
 { __LINE__,  NY, "2010-11-07T01:30:00", DD, VA, "2010-11-07T01:30:00-04:00" },
 { __LINE__,  NY, "2010-11-07T01:30:00", DS, VA, "2010-11-07T01:30:00-05:00" },

// Trivial time zones
 { __LINE__, GMT, "2010-11-07T01:30:00", DS, VU, "2010-11-07T01:30:00-00:00" },
 { __LINE__, GP1, "2010-11-07T01:30:00", DS, VU, "2010-11-07T01:30:00-01:00" },
 { __LINE__, GM1, "2010-11-07T01:30:00", DS, VU, "2010-11-07T01:30:00+01:00" },

 { __LINE__, RY, "2010-11-07T01:30:00",  DS, VU, "2010-11-07T01:30:00+03:00" },

// Time zone with sequential DS transitions (Saigon)
 { __LINE__, SA, "1912-05-01T00:30:00",  DU, VI, "1912-05-01T01:30:00+08:00" },
 { __LINE__, SA, "1912-05-01T00:30:00",  DS, VI, "1912-04-30T23:30:00+07:00" },
 { __LINE__, SA, "1912-05-01T00:30:00",  DD, VI, "1912-04-30T23:30:00+07:00" }

        };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            const int             LINE         = VALUES[i].d_line;
            const char           *TIME_ZONE_ID = VALUES[i].d_timeZoneId;
            const Dst::Enum       DST_POLICY   = (Dst::Enum)VALUES[i].d_policy;
            const Validity::Enum  EXP_VALIDITY = VALUES[i].d_expectedValidity;
            const bsl::string     inputStr(VALUES[i].d_input);
            const bsl::string     expResultStr(VALUES[i].d_expectedResult);

            bdlt::Datetime    inputTime;
            bdlt::DatetimeTz  expResultTz;
            ASSERT(0 == bdlt::Iso8601Util::parse(&inputTime,
                                             inputStr.c_str(),
                                             (int) inputStr.size()));
            ASSERT(0 == bdlt::Iso8601Util::parse(&expResultTz,
                                             expResultStr.c_str(),
                                             (int) expResultStr.size()));
            baltzo::LocalDatetime expLocalTime(expResultTz, TIME_ZONE_ID, Z);

            if (veryVeryVerbose) {
                T_ P_(LINE); P_(DST_POLICY); P(inputTime);
                T_ P_(expLocalTime) P(EXP_VALIDITY);
            }

            bdlt::DatetimeTz resultTz;
            Validity::Enum  resultValidity;
            ASSERT(0 == Obj::initLocalTime(&resultTz,
                                           &resultValidity,
                                           inputTime,
                                           TIME_ZONE_ID,
                                           DST_POLICY,
                                           &testCache));

            LOOP2_ASSERT(LINE, resultValidity,
                        EXP_VALIDITY == resultValidity);
            LOOP2_ASSERT(LINE, resultTz,
                         expResultTz == resultTz);

            bdlt::DatetimeTz resultResolveTz;
            Validity::Enum  resultResolveValidity;
            Iterator        resultIterator;

            const baltzo::Zoneinfo *currentTimeZone =
                                        testCache.lookupZoneinfo(TIME_ZONE_ID);

            ASSERT(currentTimeZone);

            Obj::resolveLocalTime(&resultResolveTz,
                                  &resultResolveValidity,
                                  &resultIterator,
                                  inputTime,
                                  DST_POLICY,
                                  *currentTimeZone);

            LOOP2_ASSERT(LINE, resultValidity,
                         resultResolveValidity == resultValidity);
            LOOP2_ASSERT(LINE, resultTz, resultTz == resultResolveTz);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) cout << "\tCLASS METHOD 'initLocalTime'" << endl;
            {
                const bdlt::Datetime   VALID_INPUT(2011, 04, 10);
                const baltzo::Zoneinfo BAD;

                bdlt::DatetimeTz result;
                Validity::Enum  resultValidity;

                ASSERT_PASS(Obj::initLocalTime(&result,
                                               &resultValidity,
                                               VALID_INPUT,
                                               NY,
                                               DU,
                                               &testCache));

                ASSERT_FAIL(Obj::initLocalTime(0,
                                               &resultValidity,
                                               VALID_INPUT,
                                               NY,
                                               DU,
                                               &testCache));

                ASSERT_FAIL(Obj::initLocalTime(&result,
                                               0,
                                               VALID_INPUT,
                                               NY,
                                               DU,
                                               &testCache));

                ASSERT_FAIL(Obj::initLocalTime(&result,
                                               &resultValidity,
                                               VALID_INPUT,
                                               0,
                                               DU,
                                               &testCache));

                ASSERT_FAIL(Obj::initLocalTime(&result,
                                               &resultValidity,
                                               VALID_INPUT,
                                               NY,
                                               DU,
                                               0));
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'resolveLocalTime':
        //
        // Concerns:
        //: 1 The 'bdlt::DatetimeTz' value is correct.
        //:
        //: 2 The (output) validity of the input local time is correct.
        //:
        //: 3 The (output) iterator refers to a transition into the specified
        //:   input 'baltzo::Zoneinfo' and the value of that transition is
        //:   correct.
        //:
        //: 4 The (output) iterator refers to the specified input
        //:   'baltzo::Zoneinfo'.
        //:
        //: 5 Return 'Err::k_UNSUPPORTED_ID' if an invalid time zone id is
        //:   passed.
        //:
        //: 6 Does not return 0 or 'Err::k_UNSUPPORTED_ID' if another error
        //:   occurs.
        //:
        //:7 The correct transition and descriptor is applied for time zones
        //:  for which the local time always with DST *on*, passing in
        //:  different policies.
        //:
        //:8 The correct transition and descriptor is applied for time zones
        //:  for which the local time always with DST *off*, passing in
        //:  different policies.
        //:
        //:9 The correct transition and descriptor is applied for time zones
        //:  that have only one transition to DST in the past, passing in
        //:  different policies.
        //
        // Plan:
        //: 1 Invoke 'resolveLocalTime' passing an invalid time zone id
        //:   and check the result.  (C-5)
        //:
        //: 2 Invoke 'resolveLocalTime' passing an invalid time zone
        //:   cache and check the result.  (C-6)
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set values (one per row), including (a) time zone id,
        //:     (b) local time values to be passed in as input, (c) DST policy
        //:     passed in as input, (d) expected local-time result value, (e)
        //:     the expected validity of the input time, and (f) the attribute
        //:     values describing the expected local-time descriptor held by
        //:     the transition to which the loaded (output) iterator refers.
        //:     (C-1..4, 7..9)
        //
        // Testing:
        //   'resolveLocalTime(...)
        // --------------------------------------------------------------------
        struct TestData {
            int             d_line;
            const char     *d_timeZoneId;
            const char     *d_input;           // iso8601 w/o tz offset
            Dst::Enum       d_dstPolicy;
            const char     *d_expectedResult;  // iso8601 w/ tz offset
            Validity::Enum  d_expectedValidity;
            const char     *d_expectedTransitionTime;
            const int       d_expectedOffsetFromUtc;
            bool            d_expectedDst;
            const char     *d_expectedDescription;
        } VALUES[] = {
// test well-defined values
 { __LINE__,  NY, "2010-01-01T12:00:00", DU, "2010-01-01T12:00:00-05:00", VU,
                                "2009-11-01T06:00:00", -18000,  false, "EST" },
 { __LINE__,  NY, "2010-01-01T12:00:00", DD, "2010-01-01T11:00:00-05:00", VU,
                                "2009-11-01T06:00:00", -18000,  false, "EST" },
 { __LINE__,  NY, "2010-01-01T12:00:00", DS, "2010-01-01T12:00:00-05:00", VU,
                                "2009-11-01T06:00:00", -18000,  false, "EST" },
 { __LINE__,  NY, "2010-01-01T00:00:00", DU, "2010-01-01T00:00:00-05:00", VU,
                                "2009-11-01T06:00:00", -18000,  false, "EST" },
 { __LINE__,  NY, "2010-01-01T00:00:00", DD, "2009-12-31T23:00:00-05:00", VU,
                                "2009-11-01T06:00:00", -18000,  false, "EST" },
 { __LINE__,  NY, "2010-01-01T00:00:00", DS, "2010-01-01T00:00:00-05:00", VU,
                                "2009-11-01T06:00:00", -18000,  false, "EST" },

// test around a fall (DST->STD) transition
 { __LINE__,  NY, "2009-11-01T00:59:59.999", DU,
     "2009-11-01T00:59:59.999-04:00", VU, "2009-03-08T07:00:00", -14400, true,
                                                                       "EDT" },
 { __LINE__,  NY, "2009-11-01T00:59:59.999", DD,
     "2009-11-01T00:59:59.999-04:00", VU, "2009-03-08T07:00:00", -14400, true,
                                                                       "EDT" },
 { __LINE__,  NY, "2009-11-01T00:59:59.999", DS,
     "2009-11-01T01:59:59.999-04:00", VU, "2009-03-08T07:00:00", -14400, true,
                                                                       "EDT" },
 { __LINE__,  NY, "2009-11-01T01:00:00.000", DU,
     "2009-11-01T01:00:00.000-05:00", VA, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2009-11-01T01:00:00.000", DD,
     "2009-11-01T01:00:00.000-04:00", VA, "2009-03-08T07:00:00", -14400, true,
                                                                       "EDT" },
 { __LINE__,  NY, "2009-11-01T01:00:00.000", DS,
     "2009-11-01T01:00:00.000-05:00", VA, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2009-11-01T01:00:00.001", DU,
     "2009-11-01T01:00:00.001-05:00", VA, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2009-11-01T01:00:00.001", DD,
     "2009-11-01T01:00:00.001-04:00", VA, "2009-03-08T07:00:00", -14400, true,
                                                                       "EDT" },
 { __LINE__,  NY, "2009-11-01T01:00:00.001", DS,
     "2009-11-01T01:00:00.001-05:00", VA, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2009-11-01T01:59:59.999", DU,
     "2009-11-01T01:59:59.999-05:00", VA, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2009-11-01T01:59:59.999", DD,
     "2009-11-01T01:59:59.999-04:00", VA, "2009-03-08T07:00:00", -14400, true,
                                                                       "EDT" },
 { __LINE__,  NY, "2009-11-01T01:59:59.999", DS,
     "2009-11-01T01:59:59.999-05:00", VA, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2009-11-01T02:00:00.000", DU,
     "2009-11-01T02:00:00.000-05:00", VU, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2009-11-01T02:00:00.000", DD,
     "2009-11-01T01:00:00.000-05:00", VU, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2009-11-01T02:00:00.000", DS,
     "2009-11-01T02:00:00.000-05:00", VU, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2009-11-01T02:00:00.001", DU,
     "2009-11-01T02:00:00.001-05:00", VU, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2009-11-01T02:00:00.001", DD,
     "2009-11-01T01:00:00.001-05:00", VU, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2009-11-01T02:00:00.001", DS,
     "2009-11-01T02:00:00.001-05:00", VU, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
// test around a spring (STD->DST) transition
 { __LINE__,  NY, "2010-03-14T00:59:59.999", DU,
     "2010-03-14T00:59:59.999-05:00", VU, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2010-03-14T00:59:59.999", DD,
     "2010-03-13T23:59:59.999-05:00", VU, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2010-03-14T00:59:59.999", DS,
     "2010-03-14T00:59:59.999-05:00", VU, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2010-03-14T01:00:00.000", DU,
     "2010-03-14T01:00:00.000-05:00", VU, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2010-03-14T01:00:00.000", DD,
     "2010-03-14T00:00:00.000-05:00", VU, "2009-11-01T06:00:00", -18000, false,
                                                         "EST" },//double check
 { __LINE__,  NY, "2010-03-14T01:00:00.000", DS,
     "2010-03-14T01:00:00.000-05:00", VU, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2010-03-14T01:00:00.001", DU,
     "2010-03-14T01:00:00.001-05:00", VU, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2010-03-14T01:00:00.001", DD,
     "2010-03-14T00:00:00.001-05:00", VU, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2010-03-14T01:00:00.001", DS,
     "2010-03-14T01:00:00.001-05:00", VU, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2010-03-14T01:59:59.999", DU,
     "2010-03-14T01:59:59.999-05:00", VU, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2010-03-14T01:59:59.999", DD,
     "2010-03-14T00:59:59.999-05:00", VU, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2010-03-14T01:59:59.999", DS,
     "2010-03-14T01:59:59.999-05:00", VU, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2010-03-14T02:00:00.000", DU,
     "2010-03-14T03:00:00.000-04:00", VI, "2010-03-14T07:00:00", -14400, true,
                                                                       "EDT" },
 { __LINE__,  NY, "2010-03-14T02:00:00.000", DD,
     "2010-03-14T01:00:00.000-05:00", VI, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2010-03-14T02:00:00.000", DS,
     "2010-03-14T03:00:00.000-04:00", VI, "2010-03-14T07:00:00", -14400, true,
                                                                       "EDT" },
 { __LINE__,  NY, "2010-03-14T02:00:00.001", DU,
     "2010-03-14T03:00:00.001-04:00", VI, "2010-03-14T07:00:00", -14400, true,
                                                                       "EDT" },
 { __LINE__,  NY, "2010-03-14T02:00:00.001", DD,
     "2010-03-14T01:00:00.001-05:00", VI, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2010-03-14T02:00:00.001", DS,
     "2010-03-14T03:00:00.001-04:00", VI, "2010-03-14T07:00:00", -14400, true,
                                                                       "EDT" },
 { __LINE__,  NY, "2010-03-14T02:59:59.999", DU,
     "2010-03-14T03:59:59.999-04:00", VI, "2010-03-14T07:00:00", -14400, true,
                                                                       "EDT" },
 { __LINE__,  NY, "2010-03-14T02:59:59.999", DD,
     "2010-03-14T01:59:59.999-05:00", VI, "2009-11-01T06:00:00", -18000, false,
                                                                       "EST" },
 { __LINE__,  NY, "2010-03-14T02:59:59.999", DS,
     "2010-03-14T03:59:59.999-04:00", VI, "2010-03-14T07:00:00", -14400, true,
                                                                       "EDT" },
 { __LINE__,  NY, "2010-03-14T03:00:00.000", DU,
     "2010-03-14T03:00:00.000-04:00", VU, "2010-03-14T07:00:00", -14400, true,
                                                                       "EDT" },
 { __LINE__,  NY, "2010-03-14T03:00:00.000", DD,
     "2010-03-14T03:00:00.000-04:00", VU, "2010-03-14T07:00:00", -14400, true,
                                                                       "EDT" },
 { __LINE__,  NY, "2010-03-14T03:00:00.000", DS,
     "2010-03-14T04:00:00.000-04:00", VU, "2010-03-14T07:00:00", -14400, true,
                                                                       "EDT" },

 // Trivial time zones (GMT, GMT+1, GMT-1)
 { __LINE__,  GMT, "2010-01-01T12:00:00", DD,
     "2010-01-01T12:00:00-00:00", VU, "0001-01-01T00:00:00",     0, false,
                                                                     "GMT"   },
 { __LINE__,  GMT, "2010-01-01T12:00:00", DS,
     "2010-01-01T12:00:00-00:00", VU, "0001-01-01T00:00:00",     0, false,
                                                                     "GMT"   },
 { __LINE__,  GMT, "2010-01-01T12:00:00", DU,
     "2010-01-01T12:00:00-00:00", VU, "0001-01-01T00:00:00",     0, false,
                                                                     "GMT"   },
 { __LINE__,  GMT, "2010-01-01T00:00:00", DD,
     "2010-01-01T00:00:00-00:00", VU, "0001-01-01T00:00:00",     0, false,
                                                                     "GMT"   },
 { __LINE__,  GMT, "2010-01-01T00:00:00", DS,
     "2010-01-01T00:00:00-00:00", VU, "0001-01-01T00:00:00",     0, false,
                                                                     "GMT"   },
 { __LINE__,  GMT, "2010-01-01T00:00:00", DU,
     "2010-01-01T00:00:00-00:00", VU, "0001-01-01T00:00:00",     0, false,
                                                                     "GMT"   },
 { __LINE__,  GP1, "2010-01-01T12:00:00", DD,
     "2010-01-01T12:00:00-01:00", VU, "0001-01-01T00:00:00", -3600, false,
                                                                     "GMT+1" },
 { __LINE__,  GP1, "2010-01-01T12:00:00", DS,
     "2010-01-01T12:00:00-01:00", VU, "0001-01-01T00:00:00", -3600, false,
                                                                     "GMT+1" },
 { __LINE__,  GP1, "2010-01-01T12:00:00", DU,
     "2010-01-01T12:00:00-01:00", VU, "0001-01-01T00:00:00", -3600, false,
                                                                     "GMT+1" },
 { __LINE__,  GP1, "2010-01-01T00:00:00", DD,
     "2010-01-01T00:00:00-01:00", VU, "0001-01-01T00:00:00", -3600, false,
                                                                     "GMT+1" },
 { __LINE__,  GP1, "2010-01-01T00:00:00", DS,
     "2010-01-01T00:00:00-01:00", VU, "0001-01-01T00:00:00", -3600, false,
                                                                     "GMT+1" },
 { __LINE__,  GP1, "2010-01-01T00:00:00", DU,
     "2010-01-01T00:00:00-01:00", VU, "0001-01-01T00:00:00", -3600, false,
                                                                     "GMT+1" },
 { __LINE__,  GM1, "2010-01-01T12:00:00", DD,
     "2010-01-01T12:00:00+01:00", VU, "0001-01-01T00:00:00",  3600, false,
                                                                     "GMT-1" },
 { __LINE__,  GM1, "2010-01-01T12:00:00", DS,
     "2010-01-01T12:00:00+01:00", VU, "0001-01-01T00:00:00",  3600, false,
                                                                     "GMT-1" },
 { __LINE__,  GM1, "2010-01-01T12:00:00", DU,
     "2010-01-01T12:00:00+01:00", VU, "0001-01-01T00:00:00",  3600, false,
                                                                     "GMT-1" },
 { __LINE__,  GM1, "2010-01-01T00:00:00", DD,
     "2010-01-01T00:00:00+01:00", VU, "0001-01-01T00:00:00",  3600, false,
                                                                     "GMT-1" },
 { __LINE__,  GM1, "2010-01-01T00:00:00", DS,
     "2010-01-01T00:00:00+01:00", VU, "0001-01-01T00:00:00",  3600, false,
                                                                     "GMT-1" },
 { __LINE__,  GM1, "2010-01-01T00:00:00", DU,
     "2010-01-01T00:00:00+01:00", VU, "0001-01-01T00:00:00",  3600, false,
                                                                     "GMT-1" },

 // // // Time zone with 1 transition (2 descriptors) (Riyadh)
 { __LINE__,  RY, "1949-12-31T23:53:07.999", DU,
     "1949-12-31T23:53:07.999+03:06", VU, "0001-01-01T00:00:00", 11212, false,
                                                                       "LMT" },
 { __LINE__,  RY, "1949-12-31T23:53:07.999", DD,
     "1949-12-31T23:53:07.999+03:06", VU, "0001-01-01T00:00:00", 11212, false,
                                                                       "LMT" },
 { __LINE__,  RY, "1949-12-31T23:53:07.999", DS,
     "1949-12-31T23:53:07.999+03:06", VU, "0001-01-01T00:00:00", 11212, false,
                                                                       "LMT" },
 { __LINE__,  RY, "1949-12-31T23:53:08.000", DU,
     "1949-12-31T23:53:08.000+03:00", VA, "1949-12-31T20:53:08", 10800, false,
                                                                       "AST" },
 { __LINE__,  RY, "1949-12-31T23:53:08.000", DD,
     "1949-12-31T23:53:08.000+03:00", VA, "1949-12-31T20:53:08", 10800, false,
                                                                       "AST" },
 { __LINE__,  RY, "1949-12-31T23:53:08.000", DS,
     "1949-12-31T23:53:08.000+03:00", VA, "1949-12-31T20:53:08", 10800, false,
                                                                       "AST" },
 { __LINE__,  RY, "1949-12-31T23:53:08.001", DU,
     "1949-12-31T23:53:08.001+03:00", VA, "1949-12-31T20:53:08", 10800, false,
                                                                       "AST" },
 { __LINE__,  RY, "1949-12-31T23:53:08.001", DD,
     "1949-12-31T23:53:08.001+03:00", VA, "1949-12-31T20:53:08", 10800, false,
                                                                       "AST" },
 { __LINE__,  RY, "1949-12-31T23:53:08.001", DS,
     "1949-12-31T23:53:08.001+03:00", VA, "1949-12-31T20:53:08", 10800, false,
                                                                       "AST" },
 { __LINE__,  RY, "1949-12-31T23:59:59.999", DU,
     "1949-12-31T23:59:59.999+03:00", VA, "1949-12-31T20:53:08", 10800, false,
                                                                       "AST" },
 { __LINE__,  RY, "1949-12-31T23:59:59.999", DD,
     "1949-12-31T23:59:59.999+03:00", VA, "1949-12-31T20:53:08", 10800, false,
                                                                       "AST" },
 { __LINE__,  RY, "1949-12-31T23:59:59.999", DS,
     "1949-12-31T23:59:59.999+03:00", VA, "1949-12-31T20:53:08", 10800, false,
                                                                       "AST" },
 { __LINE__,  RY, "1950-01-01T00:00:00.000", DU,
     "1950-01-01T00:00:00.000+03:00", VU, "1949-12-31T20:53:08", 10800, false,
                                                                       "AST" },
 { __LINE__,  RY, "1950-01-01T00:00:00.000", DD,
     "1950-01-01T00:00:00.000+03:00", VU, "1949-12-31T20:53:08", 10800, false,
                                                                       "AST" },
 { __LINE__,  RY, "1950-01-01T00:00:00.000", DS,
     "1950-01-01T00:00:00.000+03:00", VU, "1949-12-31T20:53:08", 10800, false,
                                                                       "AST" },

 // // Time zone with multiple sequential STD transitions (Saigon)
 { __LINE__,  SA, "1912-04-30T23:59:59.999", DU,
     "1912-04-30T23:59:59.999+07:00", VU, "1911-03-10T16:54:40", 25200, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-04-30T23:59:59.999", DD,
     "1912-04-30T23:59:59.999+07:00", VU, "1911-03-10T16:54:40", 25200, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-04-30T23:59:59.999", DS,
     "1912-04-30T23:59:59.999+07:00", VU, "1911-03-10T16:54:40", 25200, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-05-01T00:00:00.000", DU,
     "1912-05-01T01:00:00.000+08:00", VI, "1912-04-30T17:00:00", 28800, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-05-01T00:00:00.000", DD,
     "1912-04-30T23:00:00.000+07:00", VI, "1911-03-10T16:54:40", 25200, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-05-01T00:00:00.000", DS,
     "1912-04-30T23:00:00.000+07:00", VI, "1911-03-10T16:54:40", 25200, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-05-01T00:00:00.001", DU,
     "1912-05-01T01:00:00.001+08:00", VI, "1912-04-30T17:00:00", 28800, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-05-01T00:00:00.001", DD,
     "1912-04-30T23:00:00.001+07:00", VI, "1911-03-10T16:54:40", 25200, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-05-01T00:00:00.001", DS,
     "1912-04-30T23:00:00.001+07:00", VI, "1911-03-10T16:54:40", 25200, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-05-01T00:59:59.999", DU,
     "1912-05-01T01:59:59.999+08:00", VI, "1912-04-30T17:00:00", 28800, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-05-01T00:59:59.999", DD,
     "1912-04-30T23:59:59.999+07:00", VI, "1911-03-10T16:54:40", 25200, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-05-01T00:59:59.999", DS,
     "1912-04-30T23:59:59.999+07:00", VI, "1911-03-10T16:54:40", 25200, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-05-01T01:00:00.000", DU,
     "1912-05-01T01:00:00.000+08:00", VU, "1912-04-30T17:00:00", 28800, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-05-01T01:00:00.000", DD,
     "1912-05-01T01:00:00.000+08:00", VU, "1912-04-30T17:00:00", 28800, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-05-01T01:00:00.000", DS,
     "1912-05-01T01:00:00.000+08:00", VU, "1912-04-30T17:00:00", 28800, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-05-01T01:00:00.000", DS,
     "1912-05-01T01:00:00.000+08:00", VU, "1912-04-30T17:00:00", 28800, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-05-01T01:00:00.000", DS,
     "1912-05-01T01:00:00.000+08:00", VU, "1912-04-30T17:00:00", 28800, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-05-01T01:00:00.000", DS,
     "1912-05-01T01:00:00.000+08:00", VU, "1912-04-30T17:00:00", 28800, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-05-01T01:00:00.000", DS,
     "1912-05-01T01:00:00.000+08:00", VU, "1912-04-30T17:00:00", 28800, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-05-01T01:00:00.001", DU,
     "1912-05-01T01:00:00.001+08:00", VU, "1912-04-30T17:00:00", 28800, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-05-01T01:00:00.001", DD,
     "1912-05-01T01:00:00.001+08:00", VU, "1912-04-30T17:00:00", 28800, false,
                                                                       "ICT" },
 { __LINE__,  SA, "1912-05-01T01:00:00.001", DS,
     "1912-05-01T01:00:00.001+08:00", VU, "1912-04-30T17:00:00", 28800, false,
                                                                       "ICT" },
 // Time zone with DST transitions only.
 { __LINE__,  ALLDST, "2010-01-01T12:00:00", DU,
     "2010-01-01T12:00:00+01:00",     VU, "2001-01-01T00:00:00",  3600, true,
                                                                     "ADST3" },
 { __LINE__,  ALLDST, "2010-01-01T12:00:00", DS,
     "2010-01-01T12:00:00+01:00",     VU, "2001-01-01T00:00:00",  3600, true,
                                                                     "ADST3" },
 { __LINE__,  ALLDST, "2010-01-01T12:00:00", DD,
     "2010-01-01T12:00:00+01:00",     VU, "2001-01-01T00:00:00",  3600, true,
                                                                     "ADST3" },
 { __LINE__,  ALLDST, "2000-02-01T12:00:00", DU,
     "2000-02-01T12:00:00+02:00",     VU, "2000-01-01T00:00:00",  7200, true,
                                                                     "ADST2" },
 { __LINE__,  ALLDST, "2000-02-01T12:00:00", DS,
     "2000-02-01T12:00:00+02:00",     VU, "2000-01-01T00:00:00",  7200, true,
                                                                     "ADST2" },
 { __LINE__,  ALLDST, "2000-02-01T12:00:00", DD,
     "2000-02-01T12:00:00+02:00",     VU, "2000-01-01T00:00:00",  7200, true,
                                                                     "ADST2" },
 // Time zone with DST transitions edge cases.
 { __LINE__,  ALLDST, "2000-01-01T00:59:59.999", DU,
     "2000-01-01T00:59:59.999+01:00", VU, "0001-01-01T00:00:00",  3600, true,
                                                                     "ADST1" },
 { __LINE__,  ALLDST, "2000-01-01T00:59:59.999", DD,
     "2000-01-01T00:59:59.999+01:00", VU, "0001-01-01T00:00:00",  3600, true,
                                                                     "ADST1" },
 { __LINE__,  ALLDST, "2000-01-01T00:59:59.999", DS,
     "2000-01-01T00:59:59.999+01:00", VU, "0001-01-01T00:00:00",  3600, true,
                                                                     "ADST1" },
 { __LINE__,  ALLDST, "2000-01-01T01:00:00",     DU,
     "2000-01-01T02:00:00+02:00",     VI, "2000-01-01T00:00:00",  7200, true,
                                                                     "ADST2" },
 { __LINE__,  ALLDST, "2000-01-01T01:00:00",     DS ,
     "2000-01-01T00:00:00+01:00",     VI, "0001-01-01T00:00:00",  3600, true,
                                                                     "ADST1" },
 { __LINE__,  ALLDST, "2000-01-01T01:00:00",     DD,
     "2000-01-01T00:00:00+01:00",     VI, "0001-01-01T00:00:00",  3600, true,
                                                                     "ADST1" },
 { __LINE__,  ALLDST, "2000-01-01T01:00:00.001", DU,
     "2000-01-01T02:00:00.001+02:00", VI, "2000-01-01T00:00:00",  7200, true,
                                                                     "ADST2" },
 { __LINE__,  ALLDST, "2000-01-01T01:00:00.001", DS,
     "2000-01-01T00:00:00.001+01:00", VI, "0001-01-01T00:00:00",  3600, true,
                                                                     "ADST1" },
 { __LINE__,  ALLDST, "2000-01-01T01:00:00.001", DD,
     "2000-01-01T00:00:00.001+01:00", VI, "0001-01-01T00:00:00",  3600, true,
                                                                     "ADST1" },
 { __LINE__,  ALLDST, "2000-01-01T01:59:59.999", DU,
     "2000-01-01T02:59:59.999+02:00", VI, "2000-01-01T00:00:00",  7200, true,
                                                                     "ADST2" },
 { __LINE__,  ALLDST, "2000-01-01T01:59:59.999", DS,
     "2000-01-01T00:59:59.999+01:00", VI, "0001-01-01T00:00:00",  3600, true,
                                                                     "ADST1" },
 { __LINE__,  ALLDST, "2000-01-01T01:59:59.999", DD,
     "2000-01-01T00:59:59.999+01:00", VI, "0001-01-01T00:00:00",  3600, true,
                                                                     "ADST1" },
 { __LINE__,  ALLDST, "2000-01-01T02:00:00.000", DU,
     "2000-01-01T02:00:00.000+02:00", VU, "2000-01-01T00:00:00",  7200, true,
                                                                     "ADST2" },
 { __LINE__,  ALLDST, "2000-01-01T02:00:00.000", DS,
     "2000-01-01T02:00:00.000+02:00", VU, "2000-01-01T00:00:00",  7200, true,
                                                                     "ADST2" },
 { __LINE__,  ALLDST, "2000-01-01T02:00:00.000", DD,
     "2000-01-01T02:00:00.000+02:00", VU, "2000-01-01T00:00:00",  7200, true,
                                                                     "ADST2" },
 { __LINE__,  ALLDST, "2000-01-01T02:00:00.001", DU,
     "2000-01-01T02:00:00.001+02:00", VU, "2000-01-01T00:00:00",  7200, true,
                                                                     "ADST2" },
 { __LINE__,  ALLDST, "2000-01-01T02:00:00.001", DS,
     "2000-01-01T02:00:00.001+02:00", VU, "2000-01-01T00:00:00",  7200, true,
                                                                     "ADST2" },
 { __LINE__,  ALLDST, "2000-01-01T02:00:00.001", DD,
     "2000-01-01T02:00:00.001+02:00", VU, "2000-01-01T00:00:00",  7200, true,
                                                                     "ADST2" },
// Time zone with a single DST transition in the past.
 { __LINE__,  OLDDST, "2000-02-01T00:59:59.999", DU,
     "2000-02-01T00:59:59.999+02:00", VU, "2000-01-01T00:00:00",  7200, false,
                                                                     "ODST2" },
 { __LINE__,  OLDDST, "2000-02-01T00:59:59.999", DD,
     "2000-01-31T22:59:59.999+02:00", VU, "2000-01-01T00:00:00",  7200, false,
                                                                     "ODST2" },
 { __LINE__,  OLDDST, "2000-02-01T00:59:59.999", DS,
     "2000-02-01T00:59:59.999+02:00", VU, "2000-01-01T00:00:00",  7200, false,
                                                                     "ODST2" },
 // Ambiguous Transition
 { __LINE__,  OLDDST, "2001-01-01T00:59:59.999", DU,
     "2001-01-01T00:59:59.999+02:00", VU, "2000-01-01T00:00:00",  7200, false,
                                                                     "ODST2" },
 { __LINE__,  OLDDST, "2001-01-01T00:59:59.999", DD,
     "2000-12-31T22:59:59.999+02:00", VU, "2000-01-01T00:00:00",  7200, false,
                                                                     "ODST2" },
 { __LINE__,  OLDDST, "2001-01-01T00:59:59.999", DS,
     "2001-01-01T00:59:59.999+02:00", VU, "2000-01-01T00:00:00",  7200, false,
                                                                     "ODST2" },
 { __LINE__,  OLDDST, "2001-01-01T01:59:59.999", DU,
     "2001-01-01T01:59:59.999+01:00", VA, "2001-01-01T00:00:00",  3600, false,
                                                                     "ODST3" },
 { __LINE__,  OLDDST, "2001-01-01T01:59:59.999", DD,
     "2000-12-31T23:59:59.999+02:00", VA, "2000-01-01T00:00:00",  7200, false,
                                                                     "ODST2" },
 { __LINE__,  OLDDST, "2001-01-01T01:59:59.999", DS,
     "2001-01-01T01:59:59.999+01:00", VA, "2001-01-01T00:00:00",  3600, false,
                                                                     "ODST3" },
 { __LINE__,  OLDDST, "2001-01-01T03:59:59.999", DU,
     "2001-01-01T03:59:59.999+01:00", VU, "2001-01-01T00:00:00",  3600, false,
                                                                     "ODST3" },
 { __LINE__,  OLDDST, "2001-01-01T03:59:59.999", DD,
     "2001-01-01T00:59:59.999+01:00", VU, "2001-01-01T00:00:00",  3600, false,
                                                                     "ODST3" },
 { __LINE__,  OLDDST, "2001-01-01T03:59:59.999", DS,
     "2001-01-01T03:59:59.999+01:00", VU, "2001-01-01T00:00:00",  3600, false,
                                                                     "ODST3" },

        };
        const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

        for (int i = 0; i < NUM_VALUES; ++i) {
            const int   LINE                  = VALUES[i].d_line;
            const char *TIME_ZONE_ID          = VALUES[i].d_timeZoneId;

            const Dst::Enum DST_POLICY        = VALUES[i].d_dstPolicy;

            const Validity::Enum EXP_VALIDITY = VALUES[i].d_expectedValidity;

            const bsl::string inputStr(VALUES[i].d_input);
            const bsl::string expResultStr(VALUES[i].d_expectedResult);
            const bsl::string expUtcTransitionTimeStr(
                                           VALUES[i].d_expectedTransitionTime);

            const Descriptor EXP_DESCRIPTOR(VALUES[i].d_expectedOffsetFromUtc,
                                            VALUES[i].d_expectedDst,
                                            VALUES[i].d_expectedDescription);

            bdlt::Datetime    inputTime;
            bdlt::DatetimeTz  expResultTime;
            bdlt::Datetime    expUtcTransitionTime;
            ASSERT(0 == bdlt::Iso8601Util::parse(&inputTime,
                                                 inputStr.c_str(),
                                                 (int)inputStr.size()));
            ASSERT(0 == bdlt::Iso8601Util::parse(&expResultTime,
                                                 expResultStr.c_str(),
                                                 (int)expResultStr.size()));
            ASSERT(0 == bdlt::Iso8601Util::parse(
                            &expUtcTransitionTime,
                            expUtcTransitionTimeStr.c_str(),
                            (int)expUtcTransitionTimeStr.size()));

            if (veryVeryVerbose) {
                P_(LINE); P_(TIME_ZONE_ID); P_(DST_POLICY); P_(inputTime);
                P_(expResultTime); P_(EXP_VALIDITY); P(EXP_DESCRIPTOR);
            }

            bdlt::DatetimeTz  resultTz;
            Validity::Enum   resultValidity;
            Iterator         resultIterator;

            const baltzo::Zoneinfo *currentTimeZone =
                                        testCache.lookupZoneinfo(TIME_ZONE_ID);

            ASSERT(currentTimeZone);

            Obj::resolveLocalTime(&resultTz,
                                  &resultValidity,
                                  &resultIterator,
                                  inputTime,
                                  DST_POLICY,
                                  *currentTimeZone);

            LOOP2_ASSERT(LINE, resultTz, expResultTime == resultTz);
            LOOP2_ASSERT(LINE, resultValidity,
                         EXP_VALIDITY  == resultValidity);

            const Descriptor& resultDescriptor = resultIterator->descriptor();

            const bdlt::Datetime resultTransitionTime =
                                          fromTimeT(resultIterator->utcTime());
            LOOP3_ASSERT(LINE,
                         resultTransitionTime,
                         expUtcTransitionTime,
                         expUtcTransitionTime == resultTransitionTime);

            LOOP2_ASSERT(LINE,
                         resultDescriptor,
                         EXP_DESCRIPTOR  == resultDescriptor);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) cout <<
                             "\t'resolveLocalTime' class method " << endl;
            {
                const bdlt::Datetime    VALID_INPUT(2011, 04, 10);
                const bdlt::Datetime    VALID_INPUT2(1, 1, 1);
                const baltzo::Zoneinfo  BAD;
                const baltzo::Zoneinfo *NYZI = testCache.lookupZoneinfo(NY);
                const baltzo::Zoneinfo *SAZI = testCache.lookupZoneinfo(SA);

                bdlt::DatetimeTz result;
                Iterator         resultIterator;
                Validity::Enum   resultValidity;

                ASSERT_PASS(Obj::resolveLocalTime(&result,
                                                  &resultValidity,
                                                  &resultIterator,
                                                  VALID_INPUT,
                                                  DU,
                                                  *NYZI));

                ASSERT_FAIL(Obj::resolveLocalTime(0,
                                                  &resultValidity,
                                                  &resultIterator,
                                                  VALID_INPUT,
                                                  DU,
                                                  *NYZI));

                ASSERT_FAIL(Obj::resolveLocalTime(&result,
                                                  0,
                                                  &resultIterator,
                                                  VALID_INPUT,
                                                  DU,
                                                  *NYZI));

                ASSERT_FAIL(Obj::resolveLocalTime(&result,
                                                  &resultValidity,
                                                  0,
                                                  VALID_INPUT,
                                                  DU,
                                                  *NYZI));

                ASSERT_FAIL(Obj::resolveLocalTime(&result,
                                                  &resultValidity,
                                                  &resultIterator,
                                                  VALID_INPUT,
                                                  DU,
                                                  BAD));

                ASSERT_FAIL(Obj::resolveLocalTime(&result,
                                                  &resultValidity,
                                                  &resultIterator,
                                                  VALID_INPUT2,
                                                  DU,
                                                  *SAZI));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'convertUtcToLocalTime':
        //
        // Concerns:
        //: 1 The parameters are correctly forwarded to
        //:   'baltzo::ZoneinfoUtil::convertUtcToLocalTime'.
        //:
        //: 2 Return 'Err::k_UNSUPPORTED_ID' if an invalid time zone id is
        //:   passed.
        //:
        //: 3 Does not return 0 or 'Err::k_UNSUPPORTED_ID' if another error
        //:   occurs.
        //
        // Plan:
        //: 1 Invoke 'convertUtcToLocalTime' passing an invalid time zone id
        //:   and check the result.  (C-6)
        //:
        //: 2 Invoke 'convertUtcToLocalTime' passing an invalid time zone
        //:   cache and check the result.  (C-7)
        //:
        //: 3 Using the table-driven technique:
        //:   1 Specify a set values (one per row), including (a)
        //:     time zone id, (b) UTC time values to be passed in as input,
        //:     and (c) the expected local time value to be loaded in the
        //:     result (C-1).
        //
        // Testing:
        //   'convertUtcToLocalTime(DatetimeTz *, char *,  Datetime&, Cache *)
        // --------------------------------------------------------------------
        if (verbose) cout << endl << "'convertUtcToLocalTime'" << endl
                                  << "=======================" << endl;

        if (veryVerbose) cout << "\tTesting an invalid time zone id." << endl;
        {
            // Test with an invalid time zone id.

            const bdlt::Datetime VALID_INPUT(2010, 1, 1, 12, 0);

            bdlt::DatetimeTz result;
            ASSERT(EUID == Obj::convertUtcToLocalTime(&result,
                                                      "bogusId",
                                                      VALID_INPUT,
                                                      &testCache));
        }

        if (veryVerbose) cout << "\tTesting an invalid loader." << endl;
        {
            // Test with a cache having an invalid loader

            LogVerbosityGuard guard(true);
            bdlt::Datetime VALID_INPUT(2010, 1, 1, 12, 0);

            baltzo::DataFileLoader bogusLoader(Z);
            bogusLoader.configureRootPath("BALTZOBOGUSPATH");
            baltzo::ZoneinfoCache bogusCache(&bogusLoader, Z);


            bdlt::DatetimeTz result;
            const int RC = Obj::convertUtcToLocalTime(&result,
                                                      "bogusId",
                                                      VALID_INPUT,
                                                      &bogusCache);

            ASSERT(EUID != RC);
            ASSERT(0    != RC);
        }

        if (veryVerbose) cout << "\tTest invalid time zone id." << endl;
        {
            // Test with an invalid time zone id.
            LogVerbosityGuard guard;
        }

        struct TestData {
            int         d_line;
            const char *d_timeZoneId;
            const char *d_input;           // iso8601 w/o tz offset
            const char *d_expectedResult;  // iso8601 w/ tz offset
        } VALUES[] = {
// test well-defined values
 { __LINE__,  NY, "2010-01-01T12:00:00", "2010-01-01T07:00:00-05:00" },
 { __LINE__,  NY, "2010-01-01T00:00:00", "2009-12-31T19:00:00-05:00" },

// test around a fall (DST->STD) transition
 { __LINE__,  NY, "2009-11-01T05:59:59.999", "2009-11-01T01:59:59.999-04:00" },
 { __LINE__,  NY, "2009-11-01T06:00:00.000", "2009-11-01T01:00:00.000-05:00" },
 { __LINE__,  NY, "2009-11-01T06:59:59.999", "2009-11-01T01:59:59.999-05:00" },
 { __LINE__,  NY, "2009-11-01T07:00:00.000", "2009-11-01T02:00:00.000-05:00" },

// test around a spring (STD->DST) transition
 { __LINE__,  NY, "2010-03-14T06:59:59.999", "2010-03-14T01:59:59.999-05:00" },
 { __LINE__,  NY, "2010-03-14T07:00:00.000", "2010-03-14T03:00:00.000-04:00" },
 { __LINE__,  NY, "2010-03-14T07:59:59.999", "2010-03-14T03:59:59.999-04:00" },
 { __LINE__,  NY, "2010-03-14T08:00:00.000", "2010-03-14T04:00:00.000-04:00" },

// Trivial time zones (GMT, GMT+1, GMT-1)
 { __LINE__,  GMT, "2010-01-01T12:00:00", "2010-01-01T12:00:00-00:00" },
 { __LINE__,  GMT, "2010-01-01T00:00:00", "2010-01-01T00:00:00-00:00" },
 { __LINE__,  GP1, "2010-01-01T12:00:00", "2010-01-01T11:00:00-01:00" },
 { __LINE__,  GP1, "2010-01-01T00:00:00", "2009-12-31T23:00:00-01:00" },
 { __LINE__,  GM1, "2010-01-01T12:00:00", "2010-01-01T13:00:00+01:00" },
 { __LINE__,  GM1, "2010-01-01T00:00:00", "2010-01-01T01:00:00+01:00" },

// Time zone with 1 transition (2 descriptors) (Riyadh)
 { __LINE__,  RY, "1949-12-31T20:53:07.999", "1949-12-31T23:59:07.999+03:06" },
 { __LINE__,  RY, "1949-12-31T20:53:08.000", "1949-12-31T23:53:08.000+03:00" },
 { __LINE__,  RY, "1949-12-31T20:53:09.000", "1949-12-31T23:53:09.000+03:00" },
 { __LINE__,  RY, "1950-12-31T20:53:07.999", "1950-12-31T23:53:07.999+03:00" },
 { __LINE__,  RY, "1950-12-31T20:53:08.000", "1950-12-31T23:53:08.000+03:00" },
 { __LINE__,  RY, "1950-12-31T20:53:09.000", "1950-12-31T23:53:09.000+03:00" },

// Time zone with multiple sequential STD transitions (Saigon)
 { __LINE__,  SA, "1912-04-30T16:59:59.999", "1912-04-30T23:59:59.999+07:00" },
 { __LINE__,  SA, "1912-04-30T17:00:00.000", "1912-05-01T01:00:00.000+08:00" },
 { __LINE__,  SA, "1912-04-30T17:01:00.000", "1912-05-01T01:01:00.000+08:00" },

            };

            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE            = VALUES[i].d_line;
                const char *timeZoneId      = VALUES[i].d_timeZoneId;
                const bsl::string inputStr(VALUES[i].d_input);
                const bsl::string expResultStr(VALUES[i].d_expectedResult);

                bdlt::Datetime    inputTime;
                bdlt::DatetimeTz  expResultTime;
                ASSERT(0 == bdlt::Iso8601Util::parse(&inputTime,
                                                 inputStr.c_str(),
                                                 (int) inputStr.size()));
                ASSERT(0 == bdlt::Iso8601Util::parse(&expResultTime,
                                                 expResultStr.c_str(),
                                                 (int) expResultStr.size()));

                if (veryVeryVerbose) {
                    P_(LINE); P_(timeZoneId); P_(inputTime); P(expResultTime);
                }

                bdlt::DatetimeTz        resultTz;
                bdlt::DatetimeTz        resultTzRaw;
                Iterator               iterator;
                const baltzo::Zoneinfo *currentTimeZone;

                ASSERT(0 == Obj::convertUtcToLocalTime(&resultTz,
                                                       timeZoneId,
                                                       inputTime,
                                                       &testCache));
                currentTimeZone = testCache.lookupZoneinfo(timeZoneId);
                ASSERT(0 != currentTimeZone);
                baltzo::ZoneinfoUtil::convertUtcToLocalTime(&resultTzRaw,
                                                           &iterator,
                                                            inputTime,
                                                           *currentTimeZone);

                LOOP2_ASSERT(LINE, resultTz,    resultTz == expResultTime);
                LOOP2_ASSERT(LINE, resultTzRaw, resultTz == expResultTime);
            }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) cout <<
                             "\tCLASS METHOD 'convertUtcToLocalTime'" << endl;
            {
                bdlt::DatetimeTz result;
                bdlt::Datetime   utcTime(2011, 04, 10);

                ASSERT_PASS(Obj::convertUtcToLocalTime(&result,
                                                       NY,
                                                       utcTime,
                                                       &testCache));

                ASSERT_FAIL(Obj::convertUtcToLocalTime(0,
                                                       NY,
                                                       utcTime,
                                                       &testCache));

                ASSERT_FAIL(Obj::convertUtcToLocalTime(&result,
                                                       0,
                                                       utcTime,
                                                       &testCache));

                ASSERT_FAIL(Obj::convertUtcToLocalTime(&result,
                                                       NY,
                                                       utcTime,
                                                       0));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Plan:
        //   Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        {

            if (veryVerbose) {
                cout << "\tTest er." << endl;
            }
            {
                // Test with an invalid time zone id.
                LogVerbosityGuard guard;
                bdlt::Datetime inputTime(2010, 1, 1, 12, 0);

                bdlt::DatetimeTz result;
                ASSERT(EUID == Obj::convertUtcToLocalTime(&result,
                                                          "bogusId",
                                                          inputTime,
                                                          &testCache));
            }

            struct TestData {
                int         d_line;
                const char *d_timeZoneId;
                const char *d_input;           // iso8601 w/o tz offset
                const char *d_expectedResult;  // iso8601 w/ tz offset
            } VALUES[] = {
// test well-defined values
 { __LINE__,  NY, "2010-01-01T12:00:00", "2010-01-01T07:00:00-05:00" },
 { __LINE__,  NY, "2010-01-01T00:00:00", "2009-12-31T19:00:00-05:00" },

// test around a fall (DST->STD) transition
 { __LINE__,  NY, "2009-11-01T05:59:59.999", "2009-11-01T01:59:59.999-04:00" },
 { __LINE__,  NY, "2009-11-01T06:00:00.000", "2009-11-01T01:00:00.000-05:00" },
 { __LINE__,  NY, "2009-11-01T06:59:59.999", "2009-11-01T01:59:59.999-05:00" },
 { __LINE__,  NY, "2009-11-01T07:00:00.000", "2009-11-01T02:00:00.000-05:00" },

// test around a spring (STD->DST) transition
 { __LINE__,  NY, "2010-03-14T06:59:59.999", "2010-03-14T01:59:59.999-05:00" },
 { __LINE__,  NY, "2010-03-14T07:00:00.000", "2010-03-14T03:00:00.000-04:00" },
 { __LINE__,  NY, "2010-03-14T07:59:59.999", "2010-03-14T03:59:59.999-04:00" },
 { __LINE__,  NY, "2010-03-14T08:00:00.000", "2010-03-14T04:00:00.000-04:00" },

// Trivial time zones (GMT, GMT+1, GMT-1)
 { __LINE__,  GMT, "2010-01-01T12:00:00", "2010-01-01T12:00:00-00:00" },
 { __LINE__,  GMT, "2010-01-01T00:00:00", "2010-01-01T00:00:00-00:00" },
 { __LINE__,  GP1, "2010-01-01T12:00:00", "2010-01-01T11:00:00-01:00" },
 { __LINE__,  GP1, "2010-01-01T00:00:00", "2009-12-31T23:00:00-01:00" },
 { __LINE__,  GM1, "2010-01-01T12:00:00", "2010-01-01T13:00:00+01:00" },
 { __LINE__,  GM1, "2010-01-01T00:00:00", "2010-01-01T01:00:00+01:00" },

// Time zone with 1 transition (2 descriptors) (Riyadh)
 { __LINE__,  RY, "1949-12-31T20:53:07.999", "1949-12-31T23:59:07.999+03:06" },
 { __LINE__,  RY, "1949-12-31T20:53:08.000", "1949-12-31T23:53:08.000+03:00" },
 { __LINE__,  RY, "1949-12-31T20:53:09.000", "1949-12-31T23:53:09.000+03:00" },
 { __LINE__,  RY, "1950-12-31T20:53:07.999", "1950-12-31T23:53:07.999+03:00" },
 { __LINE__,  RY, "1950-12-31T20:53:08.000", "1950-12-31T23:53:08.000+03:00" },
 { __LINE__,  RY, "1950-12-31T20:53:09.000", "1950-12-31T23:53:09.000+03:00" },

// Time zone with multiple sequential STD transitions (Saigon)
 { __LINE__,  SA, "1912-04-30T16:59:59.999", "1912-04-30T23:59:59.999+07:00" },
 { __LINE__,  SA, "1912-04-30T17:00:00.000", "1912-05-01T01:00:00.000+08:00" },
 { __LINE__,  SA, "1912-04-30T17:01:00.000", "1912-05-01T01:01:00.000+08:00" },

            };

            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE             = VALUES[i].d_line;
                const char *TZID             = VALUES[i].d_timeZoneId;
                const bsl::string inputStr    (VALUES[i].d_input);
                const bsl::string expResultStr(VALUES[i].d_expectedResult);

                bdlt::Datetime    inputTime;
                bdlt::DatetimeTz  expResultTime;
                ASSERT(0 == bdlt::Iso8601Util::parse(&inputTime,
                                                 inputStr.c_str(),
                                                 (int) inputStr.size()));
                ASSERT(0 == bdlt::Iso8601Util::parse(&expResultTime,
                                                 expResultStr.c_str(),
                                                 (int) expResultStr.size()));

                if (veryVeryVerbose) {
                    P_(LINE); P_(TZID); P_(inputTime); P(expResultTime);
                }

                bdlt::DatetimeTz resultTz;
                ASSERT(0 == Obj::convertUtcToLocalTime(&resultTz,
                                                       TZID,
                                                       inputTime,
                                                       &testCache));

                LOOP2_ASSERT(LINE, resultTz, resultTz == expResultTime);
            }

        }
        {
            if (veryVerbose) {
                cout << "\tTest initLocalTime." << endl;
            }

            {
                // Test with an invalid time zone id.

                LogVerbosityGuard guard;
                bdlt::Datetime inputTime(2010, 1, 1, 12, 0);

                bdlt::DatetimeTz resultTz;
                Validity::Enum  resultValidity;
                ASSERT(EUID == Obj::initLocalTime(&resultTz,
                                                  &resultValidity,
                                                  inputTime,
                                                  "bogusId",
                                                  DU,
                                                  &testCache));
            }


///Result of 'initLocalTime' for Various 'DstPolicy'  Values.
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//..
//                  (result format: "local time +- offset")
// ,-----------------------------------------------------------------------.
// | Input in New York  |                   DstPolicy                      |
// |     Local Time     |--------------------------------------------------|
// |                    | *_UNSPECIFIED  |      *_DST    |   *_STANDARD    |
// |=======================================================================|
// | Jan  1, 2010 01:30 | 01:30:00-5:00  | 00:30:00-5:00 | 01:30:00-5:00   |
// |   (standard time)  |                |      [1]      |                 |
// |--------------------|--------------------------------------------------|
// | Mar 14, 2010 02:30 | 03:30:00-4:00  | 01:30:00-5:00 | 03:30:00-4:00   |
// |     (invalid)      |                |      [2]      |      [2]        |
// |--------------------|--------------------------------------------------|
// | Apr  1, 2010 01:30 | 01:30:00-4:00  | 01:30:00-4:00 | 02:30:00-4:00   |
// | (daylight-saving)  |                |               |      [1]        |
// |--------------------|--------------------------------------------------|
// | Nov  7, 2010 01:30 | 01:30:00-5:00  | 01:30:00-4:00 | 01:30:00-5:00   |
// |    (ambiguous)     |                |               |                 |
// `-----------------------------------------------------------------------'


            struct TestData {
                int         d_line;
                const char *d_timeZoneId;
                const char *d_input;           // iso8601 w/o tz offset
                int         d_policy;          // Obj::DstPolicy
                const char *d_expectedResult;  // iso8601 w/ tz offset
            } VALUES[] = {
// Test cases from component documentation
 { __LINE__,  NY, "2010-01-01T01:30:00", DU, "2010-01-01T01:30:00-05:00" },
 { __LINE__,  NY, "2010-01-01T01:30:00", DD, "2010-01-01T00:30:00-05:00" },
 { __LINE__,  NY, "2010-01-01T01:30:00", DS, "2010-01-01T01:30:00-05:00" },

 { __LINE__,  NY, "2010-03-14T02:30:00", DU, "2010-03-14T03:30:00-04:00" },
 { __LINE__,  NY, "2010-03-14T02:30:00", DD, "2010-03-14T01:30:00-05:00" },
 { __LINE__,  NY, "2010-03-14T02:30:00", DS, "2010-03-14T03:30:00-04:00" },

 { __LINE__,  NY, "2010-04-01T01:30:00", DU, "2010-04-01T01:30:00-04:00" },
 { __LINE__,  NY, "2010-04-01T01:30:00", DD, "2010-04-01T01:30:00-04:00" },
 { __LINE__,  NY, "2010-04-01T01:30:00", DS, "2010-04-01T02:30:00-04:00" },

 { __LINE__,  NY, "2010-11-07T01:30:00", DU, "2010-11-07T01:30:00-05:00" },
 { __LINE__,  NY, "2010-11-07T01:30:00", DD, "2010-11-07T01:30:00-04:00" },
 { __LINE__,  NY, "2010-11-07T01:30:00", DS, "2010-11-07T01:30:00-05:00" },

// Trivial time zones
 { __LINE__, GMT, "2010-11-07T01:30:00", DS, "2010-11-07T01:30:00-00:00" },
 { __LINE__, GP1, "2010-11-07T01:30:00", DS, "2010-11-07T01:30:00-01:00" },
 { __LINE__, GM1, "2010-11-07T01:30:00", DS, "2010-11-07T01:30:00+01:00" },

 { __LINE__, RY, "2010-11-07T01:30:00",  DS, "2010-11-07T01:30:00+03:00" },

// Time zone with sequential DS transitions (Saigon)
 { __LINE__, SA, "1912-05-01T00:30:00",  DU, "1912-05-01T01:30:00+08:00" },
 { __LINE__, SA, "1912-05-01T00:30:00",   DS, "1912-04-30T23:30:00+07:00" },
 { __LINE__, SA, "1912-05-01T00:30:00",   DD, "1912-04-30T23:30:00+07:00" }

            };
            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE       = VALUES[i].d_line;
                const char *timeZoneId = VALUES[i].d_timeZoneId;
                const Dst::Enum policy = (Dst::Enum)VALUES[i].d_policy;
                const bsl::string inputStr(VALUES[i].d_input);
                const bsl::string expResultStr(VALUES[i].d_expectedResult);

                bdlt::Datetime    inputTime;
                bdlt::DatetimeTz  expResultTime;
                ASSERT(0 == bdlt::Iso8601Util::parse(&inputTime,
                                                     inputStr.c_str(),
                                                     (int) inputStr.size()));
                ASSERT(0 == bdlt::Iso8601Util::parse(&expResultTime,
                                                expResultStr.c_str(),
                                                (int)expResultStr.size()));
                baltzo::LocalDatetime
                                    expLocalTime(expResultTime, timeZoneId, Z);

                if (veryVeryVerbose) {
                    P_(LINE); P_(policy); P_(inputTime); P(expLocalTime);
                }

                bdlt::DatetimeTz    resultTz;
                Validity::Enum  resultValidity;
                ASSERT(0 == Obj::initLocalTime(&resultTz,
                                               &resultValidity,
                                               inputTime,
                                               timeZoneId,
                                               policy,
                                               &testCache));
                LOOP2_ASSERT(LINE, resultTz,   resultTz   == expResultTime);
            }
        }
        {
            if (veryVerbose) cout << "\tTest loadLocalTimePeriodForUtc."
                                  << endl;
            {
                // Test with an invalid time zone id.
                LogVerbosityGuard guard;
                bdlt::Datetime inputTime(2010, 1, 1, 12, 0);

                baltzo::LocalTimePeriod result;
                ASSERT(EUID == Obj::loadLocalTimePeriodForUtc(&result,
                                                              "bogusId",
                                                              inputTime,
                                                              &testCache));
            }
            struct TestData {
                int         d_line;
                const char *d_timeZoneId;
                const char *d_input;           // iso8601 w/o tz offset
                const char *d_start;           // iso8601 w/o tz offset
                const char *d_end;             // iso8601 w/o tz offset
                const char *d_description;     // "EDT"
                int         d_utcOffset;
                bool        d_isDst;
            } VALUES[] = {
                {   // NY, Mid Winter 2010
                    __LINE__,
                    "America/New_York",
                    "2010-01-01T01:00:00",
                    "2009-11-01T06:00:00",
                    "2010-03-14T07:00:00",
                    "EST",
                    -18000,
                    false
                },
                {   // NY, Fall transition 2009
                    __LINE__,
                    "America/New_York",
                    "2009-11-01T06:00:00",
                    "2009-11-01T06:00:00",
                    "2010-03-14T07:00:00",
                    "EST",
                    -18000,
                    false
                },
                { // NY, 1ms prior to spring transition 2010
                    __LINE__,
                    "America/New_York",
                    "2010-03-14T06:59:59.999",
                    "2009-11-01T06:00:00",
                    "2010-03-14T07:00:00",
                    "EST",
                    -18000,
                    false
                },
                { // NY, spring transition 2010
                    __LINE__,
                    "America/New_York",
                    "2010-03-14T07:00:00",
                    "2010-03-14T07:00:00",
                    "2010-11-07T06:00:00",
                    "EDT",
                    -14400,
                    true
                },
                { // GMT
                    __LINE__,
                    "GMT",
                    "2010-03-14T07:00:00",
                    "0001-01-01T00:00:00",
                    "9999-12-31T23:59:59.999999",
                    "GMT",
                    0,
                    false,
                },
                { // Riyadh, prior to first transition.
                    __LINE__,
                    "Asia/Riyadh",
                    "1940-01-01T00:00:00",
                    "0001-01-01T00:00:00",
                    "1949-12-31T20:53:08",
                    "LMT",
                    11212,
                    false,
                },
                { // Riyadh, after final transition
                    __LINE__,
                    "Asia/Riyadh",
                    "1990-01-01T00:00:00",
                    "1949-12-31T20:53:08",
                    "9999-12-31T23:59:59.999999",
                    "AST",
                    10800,
                    false,
                },
            };
            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE         = VALUES[i].d_line;
                const char *TZID         = VALUES[i].d_timeZoneId;
                const char *EXP_DESC     = VALUES[i].d_description;
                const int   EXP_OFF      = VALUES[i].d_utcOffset;
                const bool  EXP_DST      = VALUES[i].d_isDst;
                const bsl::string inputStr(VALUES[i].d_input);
                const bsl::string startStr(VALUES[i].d_start);
                const bsl::string endStr  (VALUES[i].d_end);

                bdlt::Datetime input, start, end;
                ASSERT(0 == bdlt::Iso8601Util::parse(&input,
                                                 inputStr.c_str(),
                                                 (int) inputStr.size()));
                ASSERT(0 == bdlt::Iso8601Util::parse(&start,
                                                 startStr.c_str(),
                                                 (int) startStr.size()));
                ASSERT(0 == bdlt::Iso8601Util::parse(&end,
                                                 endStr.c_str(),
                                                 (int) endStr.size()));

                baltzo::LocalTimePeriod result;
                ASSERT(0 == Obj::loadLocalTimePeriodForUtc(&result,
                                                           TZID,
                                                           input,
                                                           &testCache));

                if (veryVeryVerbose) {
                    P_(LINE); P_(input); P(result);
                }
                const Descriptor& RD = result.descriptor();
                LOOP_ASSERT(LINE, start    == result.utcStartTime());
                LOOP_ASSERT(LINE, end      == result.utcEndTime());
                LOOP_ASSERT(LINE, EXP_DESC == RD.description());
                LOOP_ASSERT(LINE, EXP_DST  == RD.dstInEffectFlag());
                LOOP_ASSERT(LINE, EXP_OFF  == RD.utcOffsetInSeconds());
            }
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
