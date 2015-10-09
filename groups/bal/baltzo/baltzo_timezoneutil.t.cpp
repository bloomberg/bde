// baltzo_timezoneutil.t.cpp                                          -*-C++-*-
#include <baltzo_timezoneutil.h>

#include <baltzo_datafileloader.h>
#include <baltzo_defaultzoneinfocache.h>
#include <baltzo_errorcode.h>
#include <baltzo_localtimeperiod.h>
#include <baltzo_testloader.h>

#include <baltzo_localdatetime.h>

#include <ball_administration.h>
#include <ball_defaultobserver.h>
#include <ball_log.h>
#include <ball_loggermanager.h>
#include <ball_loggermanagerconfiguration.h>
#include <ball_severity.h>

#include <bdlt_iso8601util.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

#include <bsls_asserttest.h>
#include <bsls_types.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                              TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test provide a utility for converting time values to and
// from a local time.
//
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 6] convertUtcToLocalTime(LclDatetm *, const char *, const Datetm&);
// [ 6] convertUtcToLocalTime(DatetmTz *, const char *, const Datetm&);
// [ 8] convertLocalToLocalTime(LclDatetm *, const ch *, const LclDatetm&)
// [ 8] convertLocalToLocalTime(LclDatetm *, const ch *, const DatetmTz&);
// [ 8] convertLocalToLocalTime(DatetmTz *, const ch *, const LclDatetm&);
// [ 8] convertLocalToLocalTime(DatetmTz *, const char *, const DatetmTz&)
// [ 8] convertLocalToLocalTime(LclDatetm *, const ch *, const Datetm& ...
// [ 8] convertLocalToLocalTime(DatetmTz *, const ch *, const Datetm&, ...
// [ 4] initLocalTime(DatetmTz *, const Datetm&, const ch *, Dst);
// [ 4] initLocalTime(LclDatetm *, const Datetm&, const ch *, Dst);
// [ 4] initLocalTime(DatetmTz *, Valid *, const Datetm&, const ch *, ...
// [ 4] initLocalTime(LclDatetm *, Valid *, const Datetm&, const ch *, ...
// [ 5] convertLocalToUtc(Datetm *, const Datetm&, const ch *, Dst);
// [ 5] convertLocalToUtc(LclDatetm *, const Datetm&, const ch *, Dst);
// [ 3] loadLocalTimePeriod(LclTmPeriod *, const LclDatetm&);
// [ 3] loadLocalTimePeriod(LclTmPeriod *, const DatetmTz&, const ch *);
// [ 2] loadLocalTimePeriodForUtc(LclTmPeriod *, const ch *, const Date...
// [ 7] addInterval(LclDatetm *, const LclDatetm&, const TimeInterval&);
// [ 9] validateLocalTime(bool * result, const LclDatetm& lcTime);
// [ 9] validateLocalTime(bool * result, const DatetmTz&, const char *TZ);
// ============================================================================

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
// ----------------------------------------------------------------------------
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
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_  cout << "\t" << flush;          // Print a tab (w/o newline)
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
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef baltzo::TimeZoneUtil        Obj;
typedef baltzo::LocalTimeValidity   Validity;
typedef baltzo::ErrorCode           Err;
typedef baltzo::DstPolicy           Dst;
typedef baltzo::LocalTimeDescriptor Descriptor;

// ============================================================================
//                             GLOBAL CONSTANTS
// ----------------------------------------------------------------------------
const Dst::Enum      UNSP = Dst::e_UNSPECIFIED;
const Dst::Enum      DST  = Dst::e_DST;
const Dst::Enum      STD  = Dst::e_STANDARD;
const Validity::Enum UNI  = Validity::e_VALID_UNIQUE;
const Validity::Enum AMB  = Validity::e_VALID_AMBIGUOUS;
const Validity::Enum INV  = Validity::e_INVALID;
const Err::Enum      EUID = Err::k_UNSUPPORTED_ID;

const char *NY  = "America/New_York";
const char *RY  = "Asia/Riyadh";
const char *SA  = "Asia/Saigon";
const char *GMT = "Etc/GMT";
const char *GP1 = "Etc/GMT+1";
const char *GM1 = "Etc/GMT-1";


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

    bool d_verbose;             // verbose mode does not disable logging
    int  d_defaultPassthrough;  // default passthrough log level

    LogVerbosityGuard(bool verbose = false)
        // If the optionally specified 'verbose' is 'false' disable logging
        // until this guard is destroyed.
    {
        d_verbose = verbose;
        if (!d_verbose) {
            d_defaultPassthrough =
                  ball::LoggerManager::singleton().defaultPassThresholdLevel();

            ball::Administration::setDefaultThresholdLevels(
                                              ball::Severity::e_OFF,
                                              ball::Severity::e_OFF,
                                              ball::Severity::e_OFF,
                                              ball::Severity::e_OFF);
            ball::Administration::setThresholdLevels(
                                              "*",
                                              ball::Severity::e_OFF,
                                              ball::Severity::e_OFF,
                                              ball::Severity::e_OFF,
                                              ball::Severity::e_OFF);

        }
    }

    ~LogVerbosityGuard()
        // Set the logging verbosity back to its default state.
    {
        if (!d_verbose) {
            ball::Administration::setDefaultThresholdLevels(
                                              ball::Severity::e_OFF,
                                              d_defaultPassthrough,
                                              ball::Severity::e_OFF,
                                              ball::Severity::e_OFF);
            ball::Administration::setThresholdLevels(
                                              "*",
                                              ball::Severity::e_OFF,
                                              d_defaultPassthrough,
                                              ball::Severity::e_OFF,
                                              ball::Severity::e_OFF);
        }
    }
};

// ============================================================================
//                             HELPER FUNCTIONS
// ----------------------------------------------------------------------------
static bdlt::DatetimeTz toDatetimeTz(const char *iso8601TimeString)
    // Return the datetime value indicated by the specified
    // 'iso8601TimeString'.  The behavior is undefined unless
    // 'iso8601TimeString' is a null-terminated C-string containing a time
    // description matching the iso8601 specification (see 'bdlt_iso8601util').
{
    bdlt::DatetimeTz time;
    int rc = bdlt::Iso8601Util::parse(&time,
                                  iso8601TimeString,
                                  bsl::strlen(iso8601TimeString));
    BSLS_ASSERT(0 == rc);
    return time;
}

static bdlt::Datetime toDatetime(const char *iso8601TimeString)
    // Return the datetime value indicated by the specified
    // 'iso8601TimeString'.  The behavior is undefined unless
    // 'iso8601TimeString' is a null-terminated C-string containing a time
    // description matching the iso8601 specification (see 'bdlt_iso8601util').
{
    bdlt::Datetime time;
    int rc = bdlt::Iso8601Util::parse(&time,
                                  iso8601TimeString,
                                  bsl::strlen(iso8601TimeString));
    BSLS_ASSERT(0 == rc);
    return time;
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

    ball::DefaultObserver observer(&bsl::cout);
    ball::LoggerManagerConfiguration configuration;
    ball::LoggerManager::initSingleton(&observer, configuration);

    bslma::TestAllocator allocator, defaultAllocator;
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);
    bslma::TestAllocator *Z = &allocator;

    baltzo::TestLoader     testLoader(Z);
    baltzo::ZoneinfoCache  testCache(&testLoader, Z);
    ASSERT(0 == testLoader.setTimeZone("GMT",
                         reinterpret_cast<const char *>(GMT_DATA),
                         sizeof(GMT_DATA)));
    ASSERT(0 == testLoader.setTimeZone("Etc/GMT",
                         reinterpret_cast<const char *>(ETC_GMT_DATA),
                         sizeof(ETC_GMT_DATA)));
    ASSERT(0 == testLoader.setTimeZone("Etc/GMT+1",
                         reinterpret_cast<const char *>(ETC_GMTP1_DATA),
                         sizeof(ETC_GMTP1_DATA)));
    ASSERT(0 == testLoader.setTimeZone("Etc/GMT+2",
                         reinterpret_cast<const char *>(ETC_GMTP2_DATA),
                         sizeof(ETC_GMTP2_DATA)));
    ASSERT(0 == testLoader.setTimeZone("Etc/GMT-1",
                         reinterpret_cast<const char *>(ETC_GMTM1_DATA),
                         sizeof(ETC_GMTM1_DATA)));
    ASSERT(0 == testLoader.setTimeZone("America/New_York",
                         reinterpret_cast<const char *>(AMERICA_NEW_YORK_DATA),
                         sizeof(AMERICA_NEW_YORK_DATA)));
    ASSERT(0 == testLoader.setTimeZone("Asia/Riyadh",
                         reinterpret_cast<const char *>(ASIA_RIYADH_DATA),
                         sizeof(ASIA_RIYADH_DATA)));
    ASSERT(0 == testLoader.setTimeZone("Asia/Saigon",
                         reinterpret_cast<const char *>(ASIA_SAIGON_DATA),
                         sizeof(ASIA_SAIGON_DATA)));
    ASSERT(0 == testLoader.setTimeZone("Europe/Rome",
                         reinterpret_cast<const char *>(EUROPE_ROME_DATA),
                         sizeof(EUROPE_ROME_DATA)));

    baltzo::DefaultZoneinfoCache::setDefaultCache(&testCache);

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING USAGE EXAMPLE" << endl
                                  << "=====================" << endl;
        bslma::Allocator *allocator = bslma::Default::allocator(0);

        baltzo::DefaultZoneinfoCache::setDefaultCache(&testCache);
        {
            if (veryVerbose) {
                cout << "\tExample 1: Converting UTC to local-time" << endl;
            }
///Example 1: Converting a UTC time to a Local Time
/// - - - - - - - - - - - - - - - - - - - - - - - -
// In this usage example we illustrate how to convert a UTC time to its
// corresponding local time in a given time zone.  We start by creating a
// 'baltzo::LocalDatetime' object to hold the result of the conversion
// operation:
//..
    baltzo::LocalDatetime newYorkTime;
//..
// Then, we create a 'bdlt::Datetime' object holding the UTC time
// "July 31, 2010 15:00:00":
//..
    bdlt::Datetime utcTime(2010, 7, 31, 15, 0, 0);
//..
// Now, we call the 'convertLocalToLocalTime' function in
// 'baltzo::TimeZoneUtil':
//..
    int status = baltzo::TimeZoneUtil::convertUtcToLocalTime(&newYorkTime,
                                                            "America/New_York",
                                                            utcTime);
    if (0 != status) {
        // A non-zero 'status' indicates there was an error in the conversion
        // (e.g., the time zone id was not valid or the environment has not
        // been correctly configured).
//
        return 1;                                                     // RETURN
    }
//..
// Finally, we observe that the result in 'newYorkTime' is
// "July 31, 2010 11:00:00" and that the offset from UTC applied was -4 hours;
//..
    const bdlt::Datetime test = newYorkTime.datetimeTz().localDatetime();
    ASSERT(2010 == test.year());  ASSERT(11 == test.hour());
    ASSERT(   7 == test.month()); ASSERT( 0 == test.minute());
    ASSERT(  31 == test.day());   ASSERT( 0 == test.second());
//
    ASSERT( -4 * 60 == newYorkTime.datetimeTz().offset());
//..
//
    if (veryVerbose) {
        P(newYorkTime);
    }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }

        {
            if (veryVerbose) {
                cout << "\tExample 2: Converting a local-time" << endl;
            }

///Example 2: Converting a Local Time in One Time Zone to Another Time Zone
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we illustrate how to convert a local time in a given time
// zone to its corresponding local time in another time zone.  In particular we
// want to convert the time "July 31, 2010 15:00:00" in New York to its
// corresponding time in Italy.
//
// First, we create a 'bdlt::Datetime' object representing the time
// "July 31, 2010 15:00:00" in New York:
//..
    bdlt::Datetime newYorkTime(2010, 7, 31, 15, 0, 0);
//..
// Now, let's apply the conversion operation to obtain a
// 'baltzo::LocalDatetime' object representing the corresponding local time in
// Italy.
//..
    baltzo::LocalDatetime romeTime(allocator);
    int status = baltzo::TimeZoneUtil::convertLocalToLocalTime(
                                                          &romeTime,
                                                          "Europe/Rome",
                                                          newYorkTime,
                                                          "America/New_York");
    if (0 != status) {
        // A non-zero 'status' indicates there was an error in the conversion
        // (e.g., the time zone id was not valid or the environment has not
        // been correctly configured).
//
        return 1;                                                     // RETURN
    }
//..
// Notice that we use the default value for the optional 'DstPolicy' argument
// to 'convertLocalToLocalTime' -- the default values should be appropriate
// for most users.
//
// Finally, we verify that the value of 'romeTime' is "July 31 21:00:00",
// which is the time in Italy corresponding to "July 31, 2010 15:00:00" in New
// York:
//..
    const bdlt::Datetime uniqueTest = romeTime.datetimeTz().localDatetime();
    ASSERT(2010 == uniqueTest.year());  ASSERT(21 == uniqueTest.hour());
    ASSERT(   7 == uniqueTest.month()); ASSERT( 0 == uniqueTest.minute());
    ASSERT(  31 == uniqueTest.day());   ASSERT( 0 == uniqueTest.second());
//
    ASSERT( 2 * 60 == romeTime.datetimeTz().offset());
//..
    if (veryVerbose) {
        P(romeTime);
    }
    ASSERT(0 == defaultAllocator.numBytesInUse());
        }

        if (veryVerbose) cout << "\tExample 3: initLocalTime" << endl;
        {
///Example 3: Initializing a local time
/// - - - - - - - - - - - - - - - - - -
// In this example we illustrate how to create a 'baltzo::LocalDatetime' from a
// 'bdlt::Datetime', which may not represent a unique (or valid) clock time.
//
// First, we create a 'bdlt::Datetime' object for the New York local time
// "Jul 31, 2010 15:00:00".  Note that this local date-time occurs during a DST
// transition and is an invalid date-time.
//..
    bdlt::Datetime uniqueTime(2010, 7, 31, 15, 0, 0);
//..
// Then, we call 'initLocalTime', which returns a 'baltzo::LocalDatetime'
// object.  'initLocalTime' also optionally returns
// 'baltzo::LocalTimeValidity::Enum', indicating whether the provided input was
// a valid and unique clock time.  Note that invalid or ambiguous times are
// resolved using the optionally provided 'baltzo::DstPolicy::Enum' (see the
// section 'Daylight-Saving Time (DST) Policies and Disambiguation'):
//..
    baltzo::LocalDatetime             localTime;
    baltzo::LocalTimeValidity::Enum validity;
    int status = baltzo::TimeZoneUtil::initLocalTime(&localTime,
                                                    &validity,
                                                    uniqueTime,
                                                    "America/New_York");
    if (0 != status) {
        return 1;                                                     // RETURN
    }
//..
// Now, we verify the value of 'localTime' is "Jul 31, 2010 15:00:00" with an
// offset of -4:00 from GMT, in the time zone "America/New_York".
//..
    const bdlt::Datetime invalidTest = localTime.datetimeTz().localDatetime();
    ASSERT(2010 == invalidTest.year());  ASSERT(15 == invalidTest.hour());
    ASSERT(   7 == invalidTest.month()); ASSERT( 0 == invalidTest.minute());
    ASSERT(  31 == invalidTest.day());   ASSERT( 0 == invalidTest.second());
//
    ASSERT( -4 * 60 == localTime.datetimeTz().offset());
    ASSERT("America/New_York" == localTime.timeZoneId());
//..
// In addition, the time provided represents a unique and valid clock time in
// New York (because it does not fall near a daylight-saving time
// transition):
//..
    ASSERT(baltzo::LocalTimeValidity::e_VALID_UNIQUE == validity);
//..
// By contrast, if we call 'initLocalTime' for a time value that falls during a
// during a daylight-saving time transition, the returned
// 'baltzo::LocalTimeValidity::Enum' will indicate if the supplied time either
// does not represent a valid clock time in the time zone (as may occur when
// clocks are set forward), or does not represent a unique clock time (as may
// occur when clocks are set back).
//
// For example, suppose we call 'initLocalTime' for "Mar 14, 2010 02:30";
// this clock time does not occurs in New York, as clocks are set forward by an
// hour at 2am local time:
//..
    bdlt::Datetime invalidTime(2010, 3, 14, 2, 30, 0);
    status = baltzo::TimeZoneUtil::initLocalTime(&localTime,
                                                &validity,
                                                invalidTime,
                                               "America/New_York");
    if (0 != status) {
        return 1;                                                     // RETURN
    }
//..
// Now, we verify the value of 'localTime' represents a valid and unique time
// of "Mar 14, 2010 03:30:00-04:00" in the "America/New_York" time zone.
//..
    const bdlt::Datetime test = localTime.datetimeTz().localDatetime();
    ASSERT(2010 == test.year());  ASSERT( 3 == test.hour());
    ASSERT(   3 == test.month()); ASSERT(30 == test.minute());
    ASSERT(  14 == test.day());   ASSERT( 0 == test.second());
//
    ASSERT("America/New_York" == localTime.timeZoneId());
    ASSERT( -4 * 60 == localTime.datetimeTz().offset());
//..
// Finally, we verify that the validity status returned for 'invalidTime' is
// 'e_INVALID':
//..
    ASSERT(baltzo::LocalTimeValidity::e_INVALID == validity);
//..
        }

        if (veryVerbose) cout << "\tExample 4: loadLocalTimePeriod" << endl;
        {
///Example 4: Obtaining Information about a Time Value
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we illustrate how to obtain additional information about a
// local time in a given time zone using the 'loadLocalTimePeriod' method.
// Using 'loadLocalTimePeriod' a client can determine, for a point in time, the
// attributes that characterize local time in a given time zone (e.g., the
// offset from UTC, whether it is daylight-saving time) as well as the interval
// over which those attributes apply (see 'baltzo_localtimeperiod').
//
// First, we create a 'baltzo::LocalDatetime' object for the New York local
// time "Jul 31, 2010 15:00:00-04:00".  Note that this 'baltzo::LocalDatetime'
// may also be created as in example 3.
//..
    bdlt::DatetimeTz localTimeTz(
                               bdlt::Datetime(2010, 7, 31, 15, 0, 0), -4 * 60);
    baltzo::LocalDatetime localTime(localTimeTz, "America/New_York");
//..
// Then, we call 'loadLocalTimePeriod', which returns a
// 'baltzo::LocalTimePeriod' object that is loaded with attributes
// characterizing local time in New York on "Mar 14, 2010 03:30:00", and the
// interval over which those attributes are in effect.
//..
    baltzo::LocalTimePeriod period;
    int status = baltzo::TimeZoneUtil::loadLocalTimePeriod(&period, localTime);
    if (0 != status) {
        // A non-zero 'status' indicates there was an error in the conversion
        // (e.g., the time zone id was not valid or the environment has not
        // been correctly configured).
//
        return 1;                                                     // RETURN
    }
//..
// Now we examine the returned properties.  "Mar 14, 2010 03:30:00" is during
// daylight-saving time, which is -4:00 GMT, and the type of local time is
// sometimes abbreviated "EDT" for "Eastern Daylight Time".  "Eastern
// Daylight Time" is in effect from "Mar 14, 2010 7am UTC" to "Nov 7, 2010 6am
// UTC".  Note that the abbreviation provided ("EDT") is not canonical or
// localized.  In general the provided abbreviations should not be displayed
// to users (they are inteded for development and debugging only):
//..
    ASSERT(true         == period.descriptor().dstInEffectFlag());
    ASSERT(-4 * 60 * 60 == period.descriptor().utcOffsetInSeconds());
    ASSERT("EDT"        == period.descriptor().description());
    ASSERT(bdlt::Datetime(2010,  3, 14, 7, 0, 0) == period.utcStartTime());
    ASSERT(bdlt::Datetime(2010, 11,  7, 6, 0, 0) == period.utcEndTime());
//..

            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        ASSERT(0 == defaultAllocator.numBytesInUse());
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'validateLocalTime'
        //
        // Concerns:
        //: 1 This method returns a non-zero value when given a bogus id.
        //:
        //: 2 This method loads the correct value for trivial time zones.
        //:
        //: 3 This method may loads 'true' for more than one offset if the time
        //:   is ambiguous.
        //:
        //: 4 This method loads 'false' for any invalid local time.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Test that a non-zero value is returned with a time zone id that
        //:   does not exist. (C-1)
        //:
        //: 2 Use a table based approach with a) boundary values, b) ambiguous
        //:   time values, and c) invalid time values, and verify that the
        //:   expect result returned. (C-2..4)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid input (using the 'BSLS_ASSERTTEST_*'
        //:   macros). (C-5)
        //
        // Testing:
        //   validateLocalTime(bool * result, const LclDatetm& lcTime);
        //   validateLocalTime(bool * result, const DatetmTz&, const char *TZ);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CLASS METHOD 'validateLocalTime'" << endl
                          << "=================================" << endl;

        if (verbose) cout << "\tTest invalid 'timeZoneId'." << endl;
        {
            const bdlt::DatetimeTz    TIME_TZ(bdlt::Datetime(2011, 1, 1), 0);
            const char               BOGUS_ID[] = "bogusId";
            const baltzo::LocalDatetime LCL_TIME(TIME_TZ, BOGUS_ID);
            bool                     result;

            ASSERT(EUID ==
                         Obj::validateLocalTime(&result, TIME_TZ, BOGUS_ID));
            ASSERT(EUID == Obj::validateLocalTime(&result, LCL_TIME));
        }

        if (verbose) cout << "\tTest 'validateLocalTime'." << endl;
        {
            static const struct {
                int         d_line;
                const char *d_datetimeTz;           // iso8601 w/o tz offset
                const char *d_timeZoneId;
                bool        d_expectedResult;
            } DATA[] = {

            { L_,  "0001-01-01T00:00:00.000-00:00",  GMT,   true },
            { L_,  "0001-01-01T00:00:00.000-00:01",  GMT,  false },
            { L_,  "9999-12-31T23:59:59.999+00:01",  GMT,  false },
            { L_,  "9999-12-31T23:59:59.999-00:00",  GMT,   true },

            { L_,  "2011-11-07T01:30:00.000-00:00",  GP1,  false },
            { L_,  "2011-11-07T01:30:00.000-00:59",  GP1,  false },
            { L_,  "2011-11-07T01:30:00.000-01:00",  GP1,   true },
            { L_,  "2011-11-07T01:30:00.000-01:01",  GP1,  false },
            { L_,  "2011-11-07T01:30:00.000-02:00",  GP1,  false },

            { L_,  "2011-11-07T01:30:00.000+00:00",  GM1,  false },
            { L_,  "2011-11-07T01:30:00.000+00:59",  GM1,  false },
            { L_,  "2011-11-07T01:30:00.000+01:00",  GM1,  true },
            { L_,  "2011-11-07T01:30:00.000+01:01",  GM1,  false },
            { L_,  "2011-11-07T01:30:00.000+02:00",  GM1,  false },

            { L_,  "2010-01-01T00:00:00-05:00",      NY,    true },
            { L_,  "2010-07-01T00:00:00-04:00",      NY,    true },

            { L_,  "2010-03-14T01:59:59.999-05:00",  NY,    true },
            { L_,  "2010-03-14T02:00:00.000-05:00",  NY,   false },
            { L_,  "2010-03-14T02:59:59.999-05:00",  NY,   false },
            { L_,  "2010-03-14T03:00:00.000-05:00",  NY,   false },
            { L_,  "2010-03-14T01:59:59.999-04:00",  NY,   false },
            { L_,  "2010-03-14T02:00:00.000-04:00",  NY,   false },
            { L_,  "2010-03-14T02:59:59.999-04:00",  NY,   false },
            { L_,  "2010-03-14T03:00:00.000-04:00",  NY,    true },

            { L_,  "2010-11-07T00:59:59.999-04:00",  NY,    true },
            { L_,  "2010-11-07T01:00:00.000-04:00",  NY,    true },
            { L_,  "2010-11-07T01:59:59.999-04:00",  NY,    true },
            { L_,  "2010-11-07T02:00:00.000-04:00",  NY,   false },
            { L_,  "2010-11-07T00:59:59.999-05:00",  NY,   false },
            { L_,  "2010-11-07T01:00:00.000-05:00",  NY,    true },
            { L_,  "2010-11-07T01:59:59.999-05:00",  NY,    true },
            { L_,  "2010-11-07T02:00:00.000-05:00",  NY,    true },

            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int              LINE    = DATA[i].d_line;
                const bdlt::DatetimeTz& TIME_TZ =
                                            toDatetimeTz(DATA[i].d_datetimeTz);
                const char            *TZID    = DATA[i].d_timeZoneId;
                const bool             EXP     = DATA[i].d_expectedResult;

                bool result;

                LOOP_ASSERT(LINE,
                            0 ==
                             Obj::validateLocalTime(&result, TIME_TZ, TZID));

                LOOP_ASSERT(LINE, EXP == result);

                const baltzo::LocalDatetime LCL_TIME(TIME_TZ, TZID);

                LOOP_ASSERT(LINE,
                            0 == Obj::validateLocalTime(&result, LCL_TIME));
                LOOP_ASSERT(LINE, EXP == result);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout <<
                            "\t'validateLocalTime' class method" << endl;
            {
                const bdlt::DatetimeTz      TIME_TZ(
                                                bdlt::Datetime(2011, 1, 1), 0);
                const char                  TZ_ID[] = "America/New_York";
                const baltzo::LocalDatetime LCL_TIME(TIME_TZ, TZ_ID);
                bool                        result;

                ASSERT_PASS(Obj::validateLocalTime(&result, TIME_TZ, TZ_ID));
                ASSERT_FAIL(Obj::validateLocalTime(0, TIME_TZ, TZ_ID));
                ASSERT_FAIL(Obj::validateLocalTime(&result, TIME_TZ, 0));

                ASSERT_SAFE_PASS(Obj::validateLocalTime(&result, LCL_TIME));
                ASSERT_SAFE_FAIL(Obj::validateLocalTime(0, LCL_TIME));
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'convertLocalToLocal'
        //
        // Concerns:
        //: 1 'k_UNSUPPORTED_ID' is returned when an invalid identifier is
        //:   supplied.
        //:
        //: 2 Resulting 'baltzo::LocalDatetime' has the same time zone as the
        //:   supplied 'targetTimeZoneId'.
        //:
        //: 3 'baltzo::TimeZoneUtilImp::convertLocalToLocalTime' is invoked to
        //:   return the correct result.
        //:
        //: 4 'dstPolicy' is default to 'e_UNSPECIFIED' if not specified.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Test that 'k_UNSUPPORTED_ID' is returned when given a time zone
        //:   identifier that does not exist.  (C-1)
        //:
        //: 2 Use a table-based approach with widely varying input values and
        //:   verify that the method returns the expected result.  (C-2..3)
        //:
        //: 3 Use a table-based approach with time values that are DST,
        //:   standard time, ambiguous and invalid, and test that the expected
        //:   result is returned.  (C-4)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid input (using the 'BSLS_ASSERTTEST_*'
        //:   macros). (C-5)
        //
        // Testing:
        //   convertLocalToLocalTime(LclDatetm *, const ch *, const LclDatetm&)
        //   convertLocalToLocalTime(LclDatetm *, const ch *, const DatetmTz&);
        //   convertLocalToLocalTime(DatetmTz *, const ch *, const LclDatetm&);
        //   convertLocalToLocalTime(DatetmTz *, const char *, const DatetmTz&)
        //   convertLocalToLocalTime(LclDatetm *, const ch *, const Datetm& ...
        //   convertLocalToLocalTime(DatetmTz *, const ch *, const Datetm&, ...
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CLASS METHOD 'convertLocalToLocal'" << endl
                          << "==================================" << endl;

        if (verbose) cout << "\tTest invalid time zone id." << endl;
        {
            LogVerbosityGuard guard;
            const char GMT[] = "Etc/GMT";
            const bdlt::Datetime      TIME(2010, 1, 1, 12, 0);
            const bdlt::DatetimeTz    TIME_TZ(TIME, 0);
            const baltzo::LocalDatetime LCL_TIME(TIME_TZ, GMT);

            bdlt::Datetime      result;
            bdlt::DatetimeTz    resultTz;
            baltzo::LocalDatetime resultLclTime(Z);

            ASSERT(EUID == Obj::convertLocalToLocalTime(&resultLclTime,
                                                        "bogusId",
                                                        LCL_TIME));

            // ----------------------------------------------------------------

            ASSERT(EUID == Obj::convertLocalToLocalTime(&resultLclTime,
                                                        "bogusId",
                                                        TIME_TZ));

            // ----------------------------------------------------------------

            ASSERT(EUID == Obj::convertLocalToLocalTime(&resultTz,
                                                        "bogusId",
                                                        LCL_TIME));

            // ----------------------------------------------------------------

            ASSERT(EUID == Obj::convertLocalToLocalTime(&resultTz,
                                                        "bogusId",
                                                        TIME_TZ));

            // ----------------------------------------------------------------

            ASSERT(EUID == Obj::convertLocalToLocalTime(&resultLclTime,
                                                        "bogusId",
                                                        TIME,
                                                        GMT,
                                                        UNSP));

            ASSERT(EUID == Obj::convertLocalToLocalTime(&resultLclTime,
                                                        GMT,
                                                        TIME,
                                                        "bogusId",
                                                        UNSP));

            // ----------------------------------------------------------------

            ASSERT(EUID == Obj::convertLocalToLocalTime(&resultTz,
                                                        "bogusId",
                                                        TIME,
                                                        GMT,
                                                        UNSP));

            ASSERT(EUID == Obj::convertLocalToLocalTime(&resultTz,
                                                        GMT,
                                                        TIME,
                                                        "bogusId",
                                                        UNSP));
        }

        if (verbose) cout <<
                     "\tTest converting time between local time zone." << endl;
        {
            static const struct {
                int         d_line;
                const char *d_srcTimeZoneId;
                const char *d_input;           // iso8601 w/o tz offset
                const char *d_targetTimeZoneId;
                int         d_policy;          // Obj::DstPolicy
                const char *d_expectedResult;  // iso8601 w/ tz offset
            } DATA[] = {

            // New York
            { L_,  NY, "2010-03-14T02:30:00-05:00", NY, UNSP,
                                                 "2010-03-14T03:30:00-04:00" },
            { L_,  NY, "2010-03-14T02:30:00-04:00", SA,  DST,
                                                 "2010-03-14T13:30:00+07:00" },
            { L_,  NY, "2010-03-14T02:30:00-05:00", GMT,  STD,
                                                 "2010-03-14T07:30:00-00:00" },

            { L_,  NY, "2010-11-07T01:30:00-05:00", GMT, UNSP,
                                                 "2010-11-07T06:30:00-00:00" },
            { L_,  NY, "2010-11-07T01:30:00-04:00", NY,  DST,
                                                 "2010-11-07T01:30:00-04:00" },
            { L_,  NY, "2010-11-07T01:30:00-05:00", SA,  STD,
                                                 "2010-11-07T13:30:00+07:00" },

            // Trivial time zones
            { L_, GMT, "2010-11-07T01:30:00-00:00", GMT,  STD,
                                                 "2010-11-07T01:30:00-00:00" },
            { L_, GP1, "2010-11-07T01:30:00-01:00", GMT,  STD,
                                                 "2010-11-07T02:30:00-00:00" },
            { L_, GM1, "2010-11-07T01:30:00+01:00", GMT,  STD,
                                                 "2010-11-07T00:30:00-00:00" },
            { L_, GMT, "0001-01-01T00:00:00-00:00", GMT,  STD,
                                                 "0001-01-01T00:00:00-00:00" },
            { L_, GMT, "9999-12-31T23:59:59.999-00:00", GMT,  STD,
                                             "9999-12-31T23:59:59.999-00:00" },
            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int               LINE     = DATA[i].d_line;
                const char             *SRC_TZID = DATA[i].d_srcTimeZoneId;
                const bdlt::DatetimeTz&  TIME_TZ  =
                                                 toDatetimeTz(DATA[i].d_input);
                const char             *TGT_TZID = DATA[i].d_targetTimeZoneId;
                const Dst::Enum         POLICY   = (Dst::Enum)DATA[i].d_policy;

                const bdlt::DatetimeTz   EXP_TIME_TZ =
                                        toDatetimeTz(DATA[i].d_expectedResult);

                const bdlt::Datetime      TIME(TIME_TZ.localDatetime());
                const baltzo::LocalDatetime LCL_TIME(TIME_TZ, SRC_TZID);
                const baltzo::LocalDatetime EXP_LCL_TIME(EXP_TIME_TZ, TGT_TZID,
                                                      Z);

                if (veryVeryVerbose) {
                    P_(LINE) P_(SRC_TZID) P_(TIME_TZ) P(EXP_TIME_TZ)
                }

                bdlt::DatetimeTz    resultTz;
                baltzo::LocalDatetime resultLclTime;

                LOOP_ASSERT(LINE,
                            0 == Obj::convertLocalToLocalTime(&resultLclTime,
                                                              TGT_TZID,
                                                              LCL_TIME));
                LOOP2_ASSERT(LINE, resultLclTime,
                             EXP_LCL_TIME == resultLclTime);

                // ------------------------------------------------------------

                LOOP_ASSERT(LINE,
                            0 == Obj::convertLocalToLocalTime(&resultLclTime,
                                                              TGT_TZID,
                                                              TIME_TZ));
                LOOP2_ASSERT(LINE, resultLclTime,
                             EXP_LCL_TIME == resultLclTime);

                // ------------------------------------------------------------

                LOOP_ASSERT(LINE,
                            0 == Obj::convertLocalToLocalTime(&resultTz,
                                                              TGT_TZID,
                                                              LCL_TIME));
                LOOP2_ASSERT(LINE, resultTz,
                             EXP_TIME_TZ == resultTz);

                // ------------------------------------------------------------

                LOOP_ASSERT(LINE,
                            0 == Obj::convertLocalToLocalTime(&resultTz,
                                                              TGT_TZID,
                                                              TIME_TZ));
                LOOP2_ASSERT(LINE, resultTz,
                             EXP_TIME_TZ == resultTz);

                // ------------------------------------------------------------

                LOOP_ASSERT(LINE,
                            0 == Obj::convertLocalToLocalTime(&resultTz,
                                                              TGT_TZID,
                                                              TIME,
                                                              SRC_TZID,
                                                              POLICY));
                LOOP2_ASSERT(LINE, resultTz, EXP_TIME_TZ == resultTz);

                // ------------------------------------------------------------

                LOOP_ASSERT(LINE,
                            0 == Obj::convertLocalToLocalTime(&resultLclTime,
                                                              TGT_TZID,
                                                              TIME,
                                                              SRC_TZID,
                                                              POLICY));
                LOOP2_ASSERT(LINE, resultLclTime,
                             EXP_LCL_TIME == resultLclTime);
            }
        }

        if (verbose) cout << "\nTest default 'dstPolicy'." << endl;
        {
            static const struct {
                int         d_line;
                const char *d_timeZoneId;
                const char *d_input;           // iso8601 w/o tz offset
                const char *d_expectedResult;  // iso8601 w/ tz offset
            } DATA[] = {

// LINE TZID  INPUT TIME                 EXPECTED TIME
// ---- ----  -------------------------  -------------------------------
{ L_,   NY,   "2009-11-01T00:59:59.999", "2009-11-01T00:59:59.999-04:00" },
{ L_,   NY,   "2009-11-01T01:00:00.000", "2009-11-01T01:00:00.000-05:00" },
{ L_,   NY,   "2010-03-14T01:59:59.999", "2010-03-14T01:59:59.999-05:00" },
{ L_,   NY,   "2010-03-14T02:00:00.000", "2010-03-14T03:00:00.000-04:00" },

            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int              LINE        = DATA[ti].d_line;
                const char            *TZID        = DATA[ti].d_timeZoneId;
                const bdlt::Datetime    TIME        =
                                                  toDatetime(DATA[ti].d_input);
                const bdlt::DatetimeTz  EXP_TIME_TZ =
                                       toDatetimeTz(DATA[ti].d_expectedResult);

                const baltzo::LocalDatetime EXP_LCL_TIME(EXP_TIME_TZ, TZID, Z);

                if (veryVerbose) {
                    P_(LINE); P_(TIME); P(EXP_LCL_TIME);
                }

                bdlt::DatetimeTz    resultTz;
                baltzo::LocalDatetime resultLclTime(Z);
                ASSERT(0 == Obj::convertLocalToLocalTime(&resultTz,
                                                         TZID,
                                                         TIME,
                                                         TZID));
                LOOP2_ASSERT(LINE, resultTz, EXP_TIME_TZ == resultTz);

                ASSERT(0 == Obj::convertLocalToLocalTime(&resultLclTime,
                                                         TZID,
                                                         TIME,
                                                         TZID));
                LOOP2_ASSERT(LINE, resultLclTime,
                             EXP_LCL_TIME == resultLclTime);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout <<
                            "\t'convertLocalToLocalTime' class method" << endl;
            {
                bdlt::DatetimeTz    resultTz;
                baltzo::LocalDatetime resultLcl;

                const bdlt::Datetime      TIME(2010, 1, 1, 12, 0);
                const bdlt::DatetimeTz    TIME_TZ(TIME, -5 * 60);
                const char               SRC_TZID[] = "America/New_York";
                const baltzo::LocalDatetime LCL_TIME(TIME_TZ, SRC_TZID);
                const char               TGT_TZID[] = "Etc/UTC";

                ASSERT_SAFE_PASS(Obj::convertLocalToLocalTime(&resultLcl,
                                                              TGT_TZID,
                                                              LCL_TIME));
                ASSERT_SAFE_FAIL(Obj::convertLocalToLocalTime(
                                                    (baltzo::LocalDatetime*) 0,
                                                    TGT_TZID,
                                                    LCL_TIME));
                ASSERT_SAFE_FAIL(Obj::convertLocalToLocalTime(&resultLcl,
                                                              0,
                                                              LCL_TIME));

                // ------------------------------------------------------------

                ASSERT_SAFE_PASS(Obj::convertLocalToLocalTime(&resultLcl,
                                                              TGT_TZID,
                                                              TIME_TZ));
                ASSERT_SAFE_FAIL(Obj::convertLocalToLocalTime(
                                                    (baltzo::LocalDatetime*) 0,
                                                    TGT_TZID,
                                                    TIME_TZ));
                ASSERT_SAFE_FAIL(Obj::convertLocalToLocalTime(&resultLcl,
                                                              0,
                                                              TIME_TZ));

                // ------------------------------------------------------------

                ASSERT_SAFE_PASS(Obj::convertLocalToLocalTime(&resultTz,
                                                              TGT_TZID,
                                                              LCL_TIME));
                ASSERT_SAFE_FAIL(Obj::convertLocalToLocalTime(
                                                        (bdlt::DatetimeTz *) 0,
                                                        TGT_TZID,
                                                        LCL_TIME));
                ASSERT_SAFE_FAIL(Obj::convertLocalToLocalTime(&resultLcl,
                                                              0,
                                                              LCL_TIME));

                // ------------------------------------------------------------

                ASSERT_SAFE_PASS(Obj::convertLocalToLocalTime(&resultTz,
                                                              TGT_TZID,
                                                              TIME_TZ));
                ASSERT_SAFE_FAIL(Obj::convertLocalToLocalTime(
                                                        (bdlt::DatetimeTz *) 0,
                                                        TGT_TZID,
                                                        TIME_TZ));
                ASSERT_SAFE_FAIL(Obj::convertLocalToLocalTime(&resultLcl,
                                                              0,
                                                              TIME_TZ));

                // ------------------------------------------------------------

                ASSERT_PASS(Obj::convertLocalToLocalTime(&resultLcl,
                                                         TGT_TZID,
                                                         TIME,
                                                         SRC_TZID));
                ASSERT_FAIL(Obj::convertLocalToLocalTime(
                                                    (baltzo::LocalDatetime*) 0,
                                                    TGT_TZID,
                                                    TIME,
                                                    SRC_TZID));
                ASSERT_FAIL(Obj::convertLocalToLocalTime(&resultLcl,
                                                         0,
                                                         TIME,
                                                         SRC_TZID));
                ASSERT_FAIL(Obj::convertLocalToLocalTime(&resultLcl,
                                                         TGT_TZID,
                                                         TIME,
                                                         0));

                // ------------------------------------------------------------

                ASSERT_PASS(Obj::convertLocalToLocalTime(&resultTz,
                                                         TGT_TZID,
                                                         TIME,
                                                         SRC_TZID));
                ASSERT_FAIL(Obj::convertLocalToLocalTime(
                                                        (bdlt::DatetimeTz *) 0,
                                                        TGT_TZID,
                                                        TIME,
                                                        SRC_TZID));
                ASSERT_FAIL(Obj::convertLocalToLocalTime(&resultTz,
                                                         0,
                                                         TIME,
                                                         SRC_TZID));
                ASSERT_FAIL(Obj::convertLocalToLocalTime(&resultTz,
                                                         TGT_TZID,
                                                         TIME,
                                                         0));
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'addInterval'
        //
        // Concerns:
        //: 1 Resulting 'baltzo::LocalDatetime' have the same identifier.
        //:
        //: 2 Result is correct when an interval is added through a transition.
        //:
        //: 3 Result is correct when the resulting time is right on a
        //:   transition
        //:
        //: 4 Result is correct when the interval spans through more than one
        //:   transition.
        //:
        //: 5 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Test that the method returns 'k_UNSUPPORTED_ID' when supplied
        //:   with a time zone identifier that does not exist.  (C-1)
        //:
        //: 2 Use a table-based approach with interval that spans across: a) no
        //:   transition boundary, b) a single transition, and c) multiple
        //:   transitions; verify that the method returns the expected local
        //:   time.  (C-2..4)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid input (using the 'BSLS_ASSERTTEST_*'
        //:   macros). (C-5)
        //
        // Testing:
        //   addInterval(LclDatetm *, const LclDatetm&, const TimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CLASS METHOD 'addInterval'" << endl
                          << "==========================" << endl;

        const bsls::Types::Int64 MS = 1;               // milliseconds
        const bsls::Types::Int64 DY = 24LL * 60LL * 60LL * 1000LL ;
                                                      // milliseconds in a day
        const bsls::Types::Int64 YR = 365LL * 24LL * 60LL * 60LL * 1000LL;
                                                      // milliseconds in a year
        static const struct {
            int                 d_line;
            const char         *d_timeZoneId;
            const char         *d_startTime;       // iso8601 w/ tz offset
            bsls::Types::Int64  d_intervalMs;
            const char         *d_expectedResult;  // iso8601 w/ tz offset
        } DATA[] = {
            // No transitions
            { L_, GMT, "0001-01-01T00:00:00-00:00", 0,
                                                 "0001-01-01T00:00:00-00:00" },
            { L_, GMT, "9999-12-31T23:59:59.999-00:00", 0,
                                             "9999-12-31T23:59:59.999-00:00" },
            { L_, GMT, "2010-11-07T01:30:00-00:00", DY,
                                                 "2010-11-08T01:30:00-00:00" },

            // test around a fall (DST->STD) transition
            { L_, NY, "2009-11-01T01:59:59.998-04:00", MS,
                                             "2009-11-01T01:59:59.999-04:00" },
            { L_, NY, "2009-11-01T01:59:59.999-04:00", MS,
                                             "2009-11-01T01:00:00.000-05:00" },
            { L_, NY, "2009-11-01T01:00:00.000-05:00", MS,
                                             "2009-11-01T01:00:00.001-05:00" },

            // test around a spring (STD->DST) transition
            { L_, NY, "2010-03-14T01:59:59.998-05:00", MS,
                                             "2010-03-14T01:59:59.999-05:00" },
            { L_, NY, "2010-03-14T01:59:59.999-05:00", MS,
                                             "2010-03-14T03:00:00.000-04:00" },
            { L_, NY, "2010-03-14T03:00:00.000-04:00", MS,
                                             "2010-03-14T03:00:00.001-04:00" },

            // multiple transitions
            { L_, NY, "2010-06-01T01:00:00-04:00", YR / 2,
                                                 "2010-11-30T12:00:00-05:00" },
            { L_, NY, "2010-06-01T01:00:00-04:00", YR,
                                                 "2011-06-01T01:00:00-04:00" },
            { L_, NY, "2010-06-01T01:00:00-04:00", YR * 3 / 2,
                                                 "2011-11-30T12:00:00-05:00" },
            { L_, NY, "2010-06-01T01:00:00-04:00", YR * 2,
                                                 "2012-05-31T01:00:00-04:00" },

        };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        if (verbose) cout << "\nTest addInterval with TimeInterval"
                          << endl;
        {

            for (int i = 0; i < NUM_DATA; ++i) {
                const int              LINE = DATA[i].d_line;
                const char            *TZID = DATA[i].d_timeZoneId;
                const bdlt::DatetimeTz  TIME =
                                             toDatetimeTz(DATA[i].d_startTime);
                const bdlt::DatetimeTz  EXP_TIME_TZ =
                                        toDatetimeTz(DATA[i].d_expectedResult);

                bsls::TimeInterval interval;
                interval.addMilliseconds(DATA[i].d_intervalMs);

                baltzo::LocalDatetime LCL_TIME(TIME, TZID);

                if (veryVeryVerbose) {
                    P_(LINE); P_(TZID); P_(TIME); P(EXP_TIME_TZ);
                    P(interval); P(DATA[i].d_intervalMs);
                }

                baltzo::LocalDatetime resultLcl;
                ASSERT(0 == Obj::addInterval(&resultLcl, LCL_TIME, interval));

                LOOP3_ASSERT(LINE, resultLcl, EXP_TIME_TZ,
                             EXP_TIME_TZ == resultLcl.datetimeTz());

                LOOP3_ASSERT(LINE, resultLcl, EXP_TIME_TZ,
                             TZID == resultLcl.timeZoneId());

            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const bdlt::Datetime      TIME(2010, 1, 1, 12, 0);
            const bdlt::DatetimeTz    TIME_TZ(TIME, -5 * 60);
            const baltzo::LocalDatetime LCL_TIME(TIME_TZ, "America/New_York");

            if (veryVerbose) cout << "\twith 'TimeInterval'." << endl;
            {
                const bsls::TimeInterval INTERVAL(0, 0);

                baltzo::LocalDatetime resultLcl;
                ASSERT_PASS(0 == Obj::addInterval(&resultLcl,
                                                  LCL_TIME,
                                                  INTERVAL));
                ASSERT_FAIL(0 == Obj::addInterval(0, LCL_TIME, INTERVAL));
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'convertUtcToLocalTime'
        //
        // Concerns:
        //: 1 'k_UNSUPPORTED_ID' is returned when an invalid identifier is
        //:   supplied.
        //:
        //: 2 'baltzo::TimeZoneUtilImp::convertUtcToLocalTime' is invoked to
        //:   return the correct result.
        //:
        //: 3 Resulting 'baltzo::LocalDatetime' holds the right string
        //:   identifier.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Test that the method returns 'k_UNSUPPORTED_ID' when supplied
        //:   with a time zone identifier that does not exist.  (C-1)
        //:
        //: 2 Use a table-based approach with widely varying input values and
        //:   verify that the method returns the expected result.  (C-2..3)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid input (using the 'BSLS_ASSERTTEST_*'
        //:   macros). (C-4)
        //
        // Testing:
        //   convertUtcToLocalTime(LclDatetm *, const char *, const Datetm&);
        //   convertUtcToLocalTime(DatetmTz *, const char *, const Datetm&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CLASS METHOD 'convertUtcToLocalTime'" << endl
                          << "====================================" << endl;
        {

            if (veryVerbose) cout << "\tTest convertUtcToLocalTime." << endl;
            {
                // Test with an invalid time zone id.
                LogVerbosityGuard guard;

                const bdlt::Datetime   TIME(2010, 1, 1, 12, 0);
                bdlt::DatetimeTz       result;
                ASSERT(EUID == Obj::convertUtcToLocalTime(&result,
                                                          "bogusId",
                                                          TIME));
            }

            struct TestData {
                int         d_line;
                const char *d_timeZoneId;
                const char *d_input;           // iso8601 w/o tz offset
                const char *d_expectedResult;  // iso8601 w/ tz offset
            } DATA[] = {

// test well-defined values
 { L_,  NY, "2010-01-01T12:00:00", "2010-01-01T07:00:00-05:00" },
 { L_,  NY, "2010-01-01T00:00:00", "2009-12-31T19:00:00-05:00" },

// test around a fall (DST->STD) transition
 { L_,  NY, "2009-11-01T05:59:59.999", "2009-11-01T01:59:59.999-04:00" },
 { L_,  NY, "2009-11-01T06:00:00.000", "2009-11-01T01:00:00.000-05:00" },
 { L_,  NY, "2009-11-01T06:59:59.999", "2009-11-01T01:59:59.999-05:00" },
 { L_,  NY, "2009-11-01T07:00:00.000", "2009-11-01T02:00:00.000-05:00" },

// test around a spring (STD->DST) transition
 { L_,  NY, "2010-03-14T06:59:59.999", "2010-03-14T01:59:59.999-05:00" },
 { L_,  NY, "2010-03-14T07:00:00.000", "2010-03-14T03:00:00.000-04:00" },
 { L_,  NY, "2010-03-14T07:59:59.999", "2010-03-14T03:59:59.999-04:00" },
 { L_,  NY, "2010-03-14T08:00:00.000", "2010-03-14T04:00:00.000-04:00" },

// Trivial time zones (GMT, GMT+1, GMT-1)
 { L_,  GMT, "2010-01-01T12:00:00", "2010-01-01T12:00:00-00:00" },
 { L_,  GMT, "2010-01-01T00:00:00", "2010-01-01T00:00:00-00:00" },
 { L_,  GP1, "2010-01-01T12:00:00", "2010-01-01T11:00:00-01:00" },
 { L_,  GP1, "2010-01-01T00:00:00", "2009-12-31T23:00:00-01:00" },
 { L_,  GM1, "2010-01-01T12:00:00", "2010-01-01T13:00:00+01:00" },
 { L_,  GM1, "2010-01-01T00:00:00", "2010-01-01T01:00:00+01:00" },

// Time zone with multiple sequential STD transitions (Saigon)
 { L_,  SA, "1912-04-30T16:59:59.999", "1912-04-30T23:59:59.999+07:00" },
 { L_,  SA, "1912-04-30T17:00:00.000", "1912-05-01T01:00:00.000+08:00" },
 { L_,  SA, "1912-04-30T17:01:00.000", "1912-05-01T01:01:00.000+08:00" },

            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int              LINE = DATA[i].d_line;
                const char            *TZID = DATA[i].d_timeZoneId;
                const bdlt::Datetime    TIME = toDatetime(DATA[i].d_input);
                const bdlt::DatetimeTz  EXP_TIME_TZ =
                                        toDatetimeTz(DATA[i].d_expectedResult);

                if (veryVeryVerbose) {
                    P_(LINE); P_(TZID); P_(TIME); P(EXP_TIME_TZ);
                }

                bdlt::DatetimeTz resultTz;
                LOOP_ASSERT(LINE,
                            0 == Obj::convertUtcToLocalTime(&resultTz,
                                                            TZID,
                                                            TIME));

                LOOP2_ASSERT(LINE, resultTz, resultTz == EXP_TIME_TZ);

                baltzo::LocalDatetime resultLcl;
                LOOP_ASSERT(LINE,
                            0 == Obj::convertUtcToLocalTime(&resultLcl,
                                                            TZID,
                                                            TIME));

                LOOP2_ASSERT(LINE, resultLcl.datetimeTz(),
                             EXP_TIME_TZ == resultLcl.datetimeTz());

                LOOP2_ASSERT(LINE, resultLcl.timeZoneId(),
                             TZID == resultLcl.timeZoneId());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout <<
                             "\t'convertUtcToLocalTime' class method." << endl;
            {
                const bdlt::Datetime TIME(2010, 1, 1, 12, 0);

                bdlt::DatetimeTz    resultTz;
                baltzo::LocalDatetime resultLcl;

                ASSERT_PASS(0 == Obj::convertUtcToLocalTime(&resultLcl,
                                                            "America/New_York",
                                                            TIME));
                ASSERT_FAIL(0 == Obj::convertUtcToLocalTime(
                                                   (baltzo::LocalDatetime *) 0,
                                                   "America/New_York",
                                                   TIME));
                ASSERT_FAIL(0 == Obj::convertUtcToLocalTime(&resultLcl,
                                                            0,
                                                            TIME));

                // ------------------------------------------------------------

                ASSERT_SAFE_PASS(0 == Obj::convertUtcToLocalTime(
                                                            &resultTz,
                                                            "America/New_York",
                                                            TIME));
                ASSERT_SAFE_FAIL(0 == Obj::convertUtcToLocalTime(
                                                        (bdlt::DatetimeTz *) 0,
                                                        "America/New_York",
                                                        TIME));
                ASSERT_SAFE_FAIL(0 == Obj::convertUtcToLocalTime(&resultTz,
                                                                 0,
                                                                 TIME));

            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'convertLocalToUtc'
        //
        // Concerns:
        //: 1 'k_UNSUPPORTED_ID' is returned when an invalid identifier is
        //:   passed in.
        //:
        //: 2 'baltzo::TimeZoneUtilImp::convertLocalToUtc' is invoked return
        //:   the correct result.
        //:
        //: 3 'dstPolicy' is default to 'e_UNSPECIFIED' if not specified.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Test that the method returns 'k_UNSUPPORTED_ID' when supplied
        //:   with a time zone identifier that does not exist.  (C-1)
        //:
        //: 2 Use a table-based approach with widely varying input values and
        //:   verify that the method returns the expected result.  (C-2)
        //:
        //: 3 Use a table-based approach with time values that are DST,
        //:   standard time, ambiguous and invalid with without supplying DST
        //:   policy, and test that the expected result is returned.  (C-3)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid input (using the 'BSLS_ASSERTTEST_*'
        //:   macros). (C-4)
        //
        // Testing:
        //   convertLocalToUtc(Datetm *, const Datetm&, const ch *, Dst);
        //   convertLocalToUtc(LclDatetm *, const Datetm&, const ch *, Dst);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CLASS METHOD 'convertLocalToUtc'" << endl
                          << "================================" << endl;

        if (verbose) cout << "\tTest invalid time zone id." << endl;
        {
            LogVerbosityGuard guard;

            const bdlt::Datetime TIME(2010, 1, 1, 12, 0);

            bdlt::Datetime      result;
            baltzo::LocalDatetime resultLclTime(Z);

            ASSERT(EUID == Obj::convertLocalToUtc(&result,
                                                  TIME,
                                                  "bogusId",
                                                  UNSP));
            ASSERT(EUID == Obj::convertLocalToUtc(&resultLclTime,
                                                  TIME,
                                                  "bogusId",
                                                  UNSP));
        }

        if (verbose) cout <<
                    "\nTesting 'convertLocalToUtc' with varying data." << endl;
        {
            static const struct {
                int         d_line;
                const char *d_timeZoneId;
                const char *d_input;           // iso8601 w/o tz offset
                int         d_policy;          // Obj::DstPolicy
                const char *d_expectedResult;  // iso8601 w/ tz offset
            } DATA[] = {

            // Test cases from component documentation
            { L_,  NY, "2010-03-14T02:30:00", UNSP, "2010-03-14T07:30:00" },
            { L_,  NY, "2010-03-14T02:30:00",  DST, "2010-03-14T06:30:00" },
            { L_,  NY, "2010-03-14T02:30:00",  STD, "2010-03-14T07:30:00" },

            { L_,  NY, "2010-11-07T01:30:00", UNSP, "2010-11-07T06:30:00" },
            { L_,  NY, "2010-11-07T01:30:00",  DST, "2010-11-07T05:30:00" },
            { L_,  NY, "2010-11-07T01:30:00",  STD, "2010-11-07T06:30:00" },

            // Trivial time zones
            { L_, GMT, "2010-11-07T01:30:00",  STD, "2010-11-07T01:30:00" },
            { L_, GP1, "2010-11-07T01:30:00",  STD, "2010-11-07T02:30:00" },
            { L_, GM1, "2010-11-07T01:30:00",  STD, "2010-11-07T00:30:00" },
            { L_, GMT, "0001-01-01T00:00:00",  STD, "0001-01-01T00:00:00" },
            { L_, GMT, "9999-12-31T23:59:59.999", STD,
                                                   "9999-12-31T23:59:59.999" },

            // Time zone with sequential STD transitions (Saigon)
            { L_, SA, "1912-05-01T00:30:00",  UNSP, "1912-04-30T17:30:00" },
            { L_, SA, "1912-05-01T00:30:00",   STD, "1912-04-30T16:30:00" },
            { L_, SA, "1912-05-01T00:30:00",   DST, "1912-04-30T16:30:00" }

            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int              LINE   = DATA[i].d_line;
                const char            *TZID   = DATA[i].d_timeZoneId;
                const bdlt::Datetime    TIME   = toDatetime(DATA[i].d_input);
                const Dst::Enum        POLICY = (Dst::Enum)DATA[i].d_policy;
                const bdlt::DatetimeTz  EXP_TIME_TZ =
                                        toDatetimeTz(DATA[i].d_expectedResult);

                const baltzo::LocalDatetime EXP_LCL_TIME(EXP_TIME_TZ, TZID, Z);

                if (veryVeryVerbose) {
                    P_(LINE); P_(POLICY); P_(TIME); P(EXP_LCL_TIME);
                }

                bdlt::Datetime      resultUtc;
                baltzo::LocalDatetime resultUtcLclTime;
                LOOP_ASSERT(LINE,
                            0 == Obj::convertLocalToUtc(&resultUtc,
                                                        TIME,
                                                        TZID,
                                                        POLICY));
                LOOP_ASSERT(LINE,
                            0 == Obj::convertLocalToUtc(&resultUtcLclTime,
                                                        TIME,
                                                        TZID,
                                                        POLICY));

                const bdlt::Datetime EXP_UTC_TIME = EXP_TIME_TZ.utcDatetime();

                const baltzo::LocalDatetime EXP_UTC_LCL_TIME(
                                             bdlt::DatetimeTz(EXP_UTC_TIME, 0),
                                             "Etc/UTC");

                LOOP2_ASSERT(LINE, resultUtc, EXP_UTC_TIME == resultUtc);
                LOOP2_ASSERT(LINE, resultUtcLclTime,
                             EXP_UTC_LCL_TIME == resultUtcLclTime);
            }
        }

        if (verbose) cout << "\nTesting default 'dstPolicy'." << endl;
        {
            static const struct {
                int         d_line;
                const char *d_timeZoneId;
                const char *d_input;           // iso8601 w/o tz offset
                const char *d_expectedResult;  // iso8601 w/ tz offset
            } DATA[] = {
            // L   ID  INPUT TIME                 EXPECTED TIME
            // --- --- -------------------------- -------------------------
            {  L_, NY, "2009-11-01T00:59:59.999", "2009-11-01T04:59:59.999" },
            {  L_, NY, "2009-11-01T01:00:00.000", "2009-11-01T06:00:00.000" },
            {  L_, NY, "2010-03-14T01:59:59.999", "2010-03-14T06:59:59.999" },
            {  L_, NY, "2010-03-14T02:00:00.000", "2010-03-14T07:00:00.000" },

            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int              LINE       = DATA[ti].d_line;
                const char            *TZID       = DATA[ti].d_timeZoneId;
                const bdlt::Datetime    TIME = toDatetime(DATA[ti].d_input);
                const bdlt::DatetimeTz  EXP_TIME_TZ =
                                       toDatetimeTz(DATA[ti].d_expectedResult);

                const baltzo::LocalDatetime EXP_LCL_TIME(EXP_TIME_TZ, TZID, Z);

                if (veryVeryVerbose) {
                    P_(LINE) P_(TIME) P(EXP_LCL_TIME);
                }

                bdlt::Datetime      resultUtc;
                baltzo::LocalDatetime resultUtcLclTime;
                LOOP_ASSERT(LINE,
                            0 == Obj::convertLocalToUtc(&resultUtc,
                                                        TIME,
                                                        TZID));
                LOOP_ASSERT(LINE,
                            0 == Obj::convertLocalToUtc(&resultUtcLclTime,
                                                        TIME,
                                                        TZID));

                const bdlt::Datetime EXP_UTC_TIME(EXP_TIME_TZ.utcDatetime());

                const baltzo::LocalDatetime EXP_UTC_LCL_TIME(
                                             bdlt::DatetimeTz(EXP_UTC_TIME, 0),
                                             "Etc/UTC");

                LOOP2_ASSERT(LINE, resultUtc, EXP_UTC_TIME == resultUtc);
                LOOP2_ASSERT(LINE, resultUtcLclTime,
                             EXP_UTC_LCL_TIME == resultUtcLclTime);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout <<
                                  "\t'convertLocalToUtc' class method" << endl;
            {
                const bdlt::Datetime TIME(2010, 1, 1, 12, 0);

                bdlt::Datetime      resultTime;
                baltzo::LocalDatetime resultLcl;

                ASSERT_PASS(Obj::convertLocalToUtc(&resultTime,
                                                   TIME,
                                                   "America/New_York"));

                ASSERT_FAIL(Obj::convertLocalToUtc((bdlt::Datetime *) 0,
                                                   TIME,
                                                   "America/New_York"));

                ASSERT_FAIL(Obj::convertLocalToUtc(&resultTime,
                                                   TIME,
                                                   0));

                // ------------------------------------------------------------

                ASSERT_PASS(Obj::convertLocalToUtc(&resultLcl,
                                                   TIME,
                                                   "America/New_York"));

                ASSERT_FAIL(Obj::convertLocalToUtc((baltzo::LocalDatetime *) 0,
                                                   TIME,
                                                   "America/New_York"));

                ASSERT_FAIL(Obj::convertLocalToUtc(&resultLcl,
                                                   TIME,
                                                   0));
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'initLocalTime'
        //
        // Concerns:
        //: 1 'k_UNSUPPORTED_ID' is returned when an invalid identifier is
        //:   passed in.
        //:
        //: 2 'baltzo::TimeZoneUtilImp::initLocalTime' is invoked return the
        //:   correct result.
        //:
        //: 3 'dstPolicy' is default to 'e_UNSPECIFIED' if not specified.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Test that the method returns 'k_UNSUPPORTED_ID' when supplied
        //:   with a time zone identifier that does not exist.  (C-1)
        //:
        //: 2 Use a table-based approach with widely varying input values and
        //:   verify that the method returns the expected result. (C-2)
        //:
        //: 3 Use a table-based approach with time values that are DST,
        //:   standard time, ambiguous and invalid with without supplying DST
        //:   policy, and test that the expected result is returned.  (C-3)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid input (using the 'BSLS_ASSERTTEST_*'
        //:   macros). (C-4)
        //
        // Testing:
        //   initLocalTime(DatetmTz *, const Datetm&, const ch *, Dst);
        //   initLocalTime(LclDatetm *, const Datetm&, const ch *, Dst);
        //   initLocalTime(DatetmTz *, Validity *, const Datetm&, const ch ...
        //   initLocalTime(LclDatetm *, Validity *, const Datetm&, const ch...
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CLASS METHOD 'initLocalTime'" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTest invalid time zone id." << endl;
        {
            LogVerbosityGuard guard;
            bdlt::Datetime TIME(2010, 1, 1, 12, 0);

            bdlt::DatetimeTz    resultTz;
            baltzo::LocalDatetime resultLclTime(Z);
            ASSERT(EUID == Obj::initLocalTime(&resultTz,
                                              TIME,
                                              "bogusId",
                                              UNSP));

            ASSERT(EUID == Obj::initLocalTime(&resultLclTime,
                                              TIME,
                                              "bogusId",
                                              UNSP));
        }

        if (verbose) cout <<
                        "\nTesting 'initLocalTime' with varying data." << endl;
        {
            static const struct {
                int         d_line;
                const char *d_timeZoneId;
                const char *d_input;           // iso8601 w/o tz offset
                int         d_policy;          // Obj::DstPolicy
                int         d_validity;
                const char *d_expectedResult;  // iso8601 w/ tz offset
            } DATA[] = {

// New York
{ L_,  NY, "2010-03-14T02:30:00",    UNSP, INV, "2010-03-14T03:30:00-04:00" },
{ L_,  NY, "2010-03-14T02:30:00",     DST, INV, "2010-03-14T01:30:00-05:00" },
{ L_,  NY, "2010-03-14T02:30:00",     STD, INV, "2010-03-14T03:30:00-04:00" },

{ L_,  NY, "2010-11-07T01:30:00",    UNSP, AMB, "2010-11-07T01:30:00-05:00" },
{ L_,  NY, "2010-11-07T01:30:00",     DST, AMB, "2010-11-07T01:30:00-04:00" },
{ L_,  NY, "2010-11-07T01:30:00",     STD, AMB, "2010-11-07T01:30:00-05:00" },

// Trivial time zones
{ L_, GMT, "2010-11-07T01:30:00",     STD, UNI, "2010-11-07T01:30:00-00:00" },
{ L_, GP1, "2010-11-07T01:30:00",     STD, UNI, "2010-11-07T01:30:00-01:00" },
{ L_, GM1, "2010-11-07T01:30:00",     STD, UNI, "2010-11-07T01:30:00+01:00" },
{ L_, GMT, "0001-01-01T00:00:00",     STD, UNI, "0001-01-01T00:00:00-00:00" },
{ L_, GMT, "9999-12-31T23:59:59.999", STD, UNI,
                                             "9999-12-31T23:59:59.999-00:00" },

// Time zone with sequential STD transitions (Saigon)
{ L_, SA, "1912-05-01T00:30:00",     UNSP, INV, "1912-05-01T01:30:00+08:00" },
{ L_, SA, "1912-05-01T00:30:00",      STD, INV, "1912-04-30T23:30:00+07:00" },
{ L_, SA, "1912-05-01T00:30:00",      DST, INV, "1912-04-30T23:30:00+07:00" }

            };
            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            {
                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int              LINE   = DATA[ti].d_line;
                    const char            *TZID   = DATA[ti].d_timeZoneId;
                    const Dst::Enum        POLICY =
                                                  (Dst::Enum)DATA[ti].d_policy;
                    const Validity::Enum   VALIDITY =
                                          (Validity::Enum) DATA[ti].d_validity;
                    const bdlt::Datetime    TIME =
                                                  toDatetime(DATA[ti].d_input);
                    const bdlt::DatetimeTz  EXP_TIME_TZ =
                                       toDatetimeTz(DATA[ti].d_expectedResult);

                    const baltzo::LocalDatetime EXP_LCL_TIME(EXP_TIME_TZ,
                                                          TZID,
                                                          Z);

                    if (veryVerbose) {
                        P_(LINE); P_(POLICY); P_(TIME); P(EXP_LCL_TIME);
                    }

                    bdlt::DatetimeTz    resultTz;
                    baltzo::LocalDatetime resultLclTime(Z);
                    ASSERT(0 == Obj::initLocalTime(&resultTz,
                                                   TIME,
                                                   TZID,
                                                   POLICY));
                    LOOP2_ASSERT(LINE, resultTz, EXP_TIME_TZ == resultTz);

                    ASSERT(0 == Obj::initLocalTime(&resultLclTime,
                                                   TIME,
                                                   TZID,
                                                   POLICY));
                    LOOP2_ASSERT(LINE, resultLclTime,
                                 EXP_LCL_TIME == resultLclTime);

                    Validity::Enum resultValidity;
                    ASSERT(0 == Obj::initLocalTime(&resultTz,
                                                   &resultValidity,
                                                   TIME,
                                                   TZID,
                                                   POLICY));
                    LOOP2_ASSERT(LINE, resultTz, EXP_TIME_TZ == resultTz);
                    LOOP2_ASSERT(LINE, resultValidity,
                                 VALIDITY == resultValidity);

                    ASSERT(0 == Obj::initLocalTime(&resultLclTime,
                                                   &resultValidity,
                                                   TIME,
                                                   TZID,
                                                   POLICY));
                    LOOP2_ASSERT(LINE, resultLclTime,
                                 EXP_LCL_TIME == resultLclTime);
                    LOOP2_ASSERT(LINE, resultValidity,
                                 VALIDITY == resultValidity);
                }
            }
        }

        if (verbose) cout << "\nTest default 'dstPolicy'." << endl;
        {
            static const struct {
                int         d_line;
                const char *d_timeZoneId;
                const char *d_input;           // iso8601 w/o tz offset
                const char *d_expectedResult;  // iso8601 w/ tz offset
            } DATA[] = {

{ L_, NY, "2009-11-01T00:59:59.999", "2009-11-01T00:59:59.999-04:00" },
{ L_, NY, "2009-11-01T01:00:00",     "2009-11-01T01:00:00-05:00" },
{ L_, NY, "2010-03-14T01:59:59.999", "2010-03-14T01:59:59.999-05:00" },
{ L_, NY, "2010-03-14T02:00:00",     "2010-03-14T03:00:00-04:00" },

            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int              LINE   = DATA[ti].d_line;
                const char            *TZID   = DATA[ti].d_timeZoneId;
                const bdlt::Datetime    TIME   = toDatetime(DATA[ti].d_input);
                const bdlt::DatetimeTz  EXP_TIME_TZ =
                                       toDatetimeTz(DATA[ti].d_expectedResult);

                const baltzo::LocalDatetime EXP_LCL_TIME(EXP_TIME_TZ, TZID, Z);

                if (veryVerbose) {
                    P_(LINE); P_(TIME); P(EXP_LCL_TIME);
                }

                bdlt::DatetimeTz    resultTz;
                baltzo::LocalDatetime resultLclTime(Z);
                ASSERT(0 == Obj::initLocalTime(&resultTz,
                                               TIME,
                                               TZID));
                LOOP2_ASSERT(LINE, resultTz, EXP_TIME_TZ == resultTz);

                ASSERT(0 == Obj::initLocalTime(&resultLclTime,
                                               TIME,
                                               TZID));
                LOOP2_ASSERT(LINE, resultLclTime,
                             EXP_LCL_TIME == resultLclTime);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'initLocalTime' class method" << endl;
            {
                const bdlt::Datetime TIME(2010, 1, 1, 12, 0);

                bdlt::Datetime      resultTime;
                bdlt::DatetimeTz    resultTz;
                baltzo::LocalDatetime resultLcl;
                Validity::Enum     resultValidity;

                ASSERT_SAFE_PASS(Obj::initLocalTime(&resultTz,
                                                    TIME,
                                                    "America/New_York"));

                ASSERT_SAFE_FAIL(Obj::initLocalTime((bdlt::DatetimeTz *) 0,
                                                    TIME,
                                                    "America/New_York"));

                ASSERT_SAFE_FAIL(Obj::initLocalTime(&resultTz,
                                                    TIME,
                                                    0));

                // ------------------------------------------------------------

                ASSERT_PASS(Obj::initLocalTime(&resultLcl,
                                               TIME,
                                               "America/New_York"));

                ASSERT_FAIL(Obj::initLocalTime((baltzo::LocalDatetime *) 0,
                                               TIME,
                                               "America/New_York"));

                ASSERT_FAIL(Obj::initLocalTime(&resultLcl,
                                               TIME,
                                               0));

                // ------------------------------------------------------------

                ASSERT_SAFE_PASS(Obj::initLocalTime(&resultTz,
                                                    &resultValidity,
                                                    TIME,
                                                    "America/New_York"));

                ASSERT_SAFE_FAIL(Obj::initLocalTime((bdlt::DatetimeTz *) 0,
                                                    &resultValidity,
                                                    TIME,
                                                    "America/New_York"));

                ASSERT_SAFE_FAIL(Obj::initLocalTime(&resultTz,
                                                    0,
                                                    TIME,
                                                    "America/New_York"));

                ASSERT_SAFE_FAIL(Obj::initLocalTime(&resultTz,
                                                    &resultValidity,
                                                    TIME,
                                                    0));

                // ------------------------------------------------------------

                ASSERT_PASS(Obj::initLocalTime(&resultLcl,
                                               &resultValidity,
                                               TIME,
                                               "America/New_York"));

                ASSERT_FAIL(Obj::initLocalTime((baltzo::LocalDatetime *) 0,
                                               &resultValidity,
                                               TIME,
                                               "America/New_York"));

                ASSERT_FAIL(Obj::initLocalTime(&resultLcl,
                                               0,
                                               TIME,
                                               "America/New_York"));

                ASSERT_FAIL(Obj::initLocalTime(&resultLcl,
                                               &resultValidity,
                                               TIME,
                                               0));
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'loadLocalTimePeriod'
        //
        // Concerns:
        //: 1 'k_UNSUPPORTED_ID' is returned when an invalid identifier is
        //:   passed in.
        //:
        //: 2 'loadLocalTimePeriodForUtc' is invoked to return the correct
        //:   result.
        //:
        //: 3 'dstPolicy' is default to 'e_UNSPECIFIED'.
        //:
        //: 4 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Test that 'k_UNSUPPORTED_ID' is returned when given a time zone
        //:   identifier that does not exist.  (C-1)
        //:
        //: 2 Use a table-based approach with widely varying input values and
        //:   verify that the method returns the expected result.  (C-2..3)
        //:
        //: 3 Use a table-based approach with time values that are DST,
        //:   standard time, ambiguous and invalid, and test that the expected
        //:   result is returned.  (C-4)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid input (using the 'BSLS_ASSERTTEST_*'
        //:   macros). (C-4)
        //
        // Testing:
        //   loadLocalTimePeriod(LclTmPeriod *, const LclDatetm&);
        //   loadLocalTimePeriod(LclTmPeriod *, const DatetmTz&, const ch *);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CLASS METHOD 'loadLocalTimePeriod'" << endl
                          << "==================================" << endl;

        if (verbose) cout <<
                      "\tCreate a table of local time and the result." << endl;
        {
            static const struct {
                int                d_line;
                const char        *d_timeZoneId;
                const char        *d_input;   // iso8601 w/o tz offset
                int                d_policy;  // DstPolicy
                const char        *d_start;   // iso8601 w/o tz offset
                const char        *d_end;     // iso8601 w/o tz offset
                const char        *d_abbrev;  // "EDT"
                int                d_utcOffset;
                bool               d_isDst;
            } DATA[] = {

// NY, Fall transition 2009, 3 policies
{ L_, NY, "2009-11-01T01:00:00-05:00", UNSP, "2009-11-01T06:00:00",
                                 "2010-03-14T07:00:00", "EST", -18000, false },
{ L_, NY, "2009-11-01T01:00:00-04:00", DST, "2009-03-08T07:00:00",
                                 "2009-11-01T06:00:00", "EDT", -14400, true, },
{ L_, NY, "2009-11-01T01:00:00-05:00", STD, "2009-11-01T06:00:00",
                                 "2010-03-14T07:00:00", "EST", -18000, false },
// NY, 1ms prior to spring transition 2010
{ L_, NY, "2010-03-14T01:59:59.999-05:00", UNSP, "2009-11-01T06:00:00",
                                 "2010-03-14T07:00:00", "EST", -18000, false },
{ L_, NY, "2010-03-14T01:59:59.999-04:00", DST, "2009-11-01T06:00:00",
                                 "2010-03-14T07:00:00", "EST", -18000, false },
{ L_, NY, "2010-03-14T01:59:59.999-05:00", STD, "2009-11-01T06:00:00",
                                 "2010-03-14T07:00:00", "EST", -18000, false },
// NY, Spring transition 2010
{ L_, NY, "2010-03-14T02:00:00-05:00", UNSP, "2010-03-14T07:00:00",
                                 "2010-11-07T06:00:00", "EDT", -14400,  true },
{ L_, NY, "2010-03-14T02:00:00-04:00", DST, "2009-11-01T06:00:00",
                                 "2010-03-14T07:00:00", "EST", -18000, false },
{ L_, NY, "2010-03-14T02:00:00-05:00", STD, "2010-03-14T07:00:00",
                                 "2010-11-07T06:00:00", "EDT", -14400,  true },
// GMT
{ L_, GMT, "2010-03-14T07:00:00-00:00", UNSP, "0001-01-01T00:00:00",
                             "9999-12-31T23:59:59.999", "GMT",      0, false },
{ L_, GMT, "2010-03-14T07:00:00", DST, "0001-01-01T00:00:00",
                       "9999-12-31T23:59:59.999-00:00", "GMT",      0, false },
{ L_, GMT, "2010-03-14T07:00:00", STD, "0001-01-01T00:00:00",
                       "9999-12-31T23:59:59.999-00:00", "GMT",      0, false },

            };

            const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

            if (veryVerbose) cout <<
                                   "\tTest all 'loadLocalTimePeriod'." << endl;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int              LINE    = DATA[ti].d_line;
                const char            *TZID    = DATA[ti].d_timeZoneId;
                const char            *ABBREV  = DATA[ti].d_abbrev;
                const bdlt::DatetimeTz  TIME_TZ =
                                                toDatetimeTz(DATA[ti].d_input);
                const Dst::Enum        POLICY  = (Dst::Enum) DATA[ti].d_policy;
                const bdlt::Datetime    START   = toDatetime(DATA[ti].d_start);
                const bdlt::Datetime    END     = toDatetime(DATA[ti].d_end);
                const int              OFFSET  = DATA[ti].d_utcOffset;
                const bool             ISDST   = DATA[ti].d_isDst;

                const baltzo::LocalDatetime LCL_TIME(TIME_TZ, TZID);

                for (char cfg = 'a'; cfg <= 'b'; ++cfg) {
                    const char CONFIG = cfg;

                    baltzo::LocalTimePeriod result;

                    if (veryVerbose) { T_ T_ P(CONFIG) }

                    switch (CONFIG){
                      case 'a': {
                        LOOP2_ASSERT(LINE, CONFIG,
                               0 == Obj::loadLocalTimePeriod(&result,
                                                             LCL_TIME));
                      } break;
                      case 'b': {
                        LOOP2_ASSERT(LINE, CONFIG,
                               0 == Obj::loadLocalTimePeriod(&result,
                                                             TIME_TZ,
                                                             TZID));
                      } break;
                      default: {
                        LOOP2_ASSERT(LINE, CONFIG, !"Bad test config.");
                      } break;
                    }

                    if (veryVeryVerbose) {
                        T_ T_ P_(LINE) P_(POLICY) P_(TIME_TZ) P(result)
                    }

                    const Descriptor& desc = result.descriptor();
                    LOOP3_ASSERT(LINE, CONFIG, result.utcStartTime(),
                                 START == result.utcStartTime());
                    LOOP3_ASSERT(LINE, CONFIG, result.utcEndTime(),
                                 END == result.utcEndTime());
                    LOOP3_ASSERT(LINE, CONFIG, desc.description(),
                                 ABBREV == desc.description());
                    LOOP3_ASSERT(LINE, CONFIG, desc.dstInEffectFlag(),
                                 ISDST  == desc.dstInEffectFlag());
                    LOOP3_ASSERT(LINE, CONFIG, desc.utcOffsetInSeconds(),
                                 OFFSET == desc.utcOffsetInSeconds());
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout <<
                                "\t'loadLocalTimePeriod' class method" << endl;
            {
                baltzo::LocalTimePeriod resultPeriod;

                const bdlt::Datetime      TIME(2010, 1, 1, 12, 0);
                const bdlt::DatetimeTz    inputTz(TIME, -5 * 60);
                const char               TZID[] = "America/New_York";
                const baltzo::LocalDatetime inputLcl(inputTz, TZID);

                ASSERT_SAFE_PASS(Obj::loadLocalTimePeriod(&resultPeriod,
                                                          inputLcl));
                ASSERT_SAFE_FAIL(Obj::loadLocalTimePeriod(0,
                                                          inputLcl));

                ASSERT_SAFE_PASS(Obj::loadLocalTimePeriod(&resultPeriod,
                                                          inputTz,
                                                          TZID));
                ASSERT_SAFE_FAIL(Obj::loadLocalTimePeriod(0,
                                                          inputTz,
                                                          TZID));
                ASSERT_SAFE_FAIL(Obj::loadLocalTimePeriod(&resultPeriod,
                                                          inputTz,
                                                          0));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CLASS METHOD 'loadLocalTimePeriodForUtc'
        //
        // Concerns:
        //: 1 returns 'k_UNSUPPORTED_ID' if 'timeZoneId' is not recognized.
        //:
        //: 2 'baltzo::TimeZoneUtilImp::loadLocalTimePeriodForUtc' is correctly
        //:   called by this function.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Test that 'loadLocalTimePeriodForUtc' returns 'k_UNSUPPORTED_ID'
        //:   when given a time zone identifier that does not exist.  (C-1)
        //:
        //: 2 Use a table-based approach with widely varying input values and
        //:   verify that the method returns the expected result.  (C-2)
        //:
        //: 4 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid input (using the 'BSLS_ASSERTTEST_*'
        //:   macros). (C-3)
        //
        // Testing:
        //   loadLocalTimePeriodForUtc(LclTmPeriod *, const ch *, const Date...
        // --------------------------------------------------------------------

        if (verbose) cout << "\tTesting invalid identifier." << endl;
        {
            // Test with an invalid time zone id.
            LogVerbosityGuard guard;

            bdlt::Datetime TIME(2010, 1, 1, 12, 0);

            baltzo::LocalTimePeriod result;
            ASSERT(EUID == Obj::loadLocalTimePeriodForUtc(&result,
                                                          "bogusId",
                                                          TIME));
        }

        if (verbose) cout <<
                 "\nCreate a table of local time and expected result." << endl;
        static const struct {
            int         d_line;
            const char *d_timeZoneId;
            const char *d_input;           // iso8601 w/o tz offset
            const char *d_start;           // iso8601 w/o tz offset
            const char *d_end;             // iso8601 w/o tz offset
            const char *d_abbrev;          // "EDT"
            int         d_utcOffset;
            bool        d_isDst;
        } DATA[] = {

        // NY, Mid Winter 2010
        { L_, NY, "2010-01-01T01:00:00", "2009-11-01T06:00:00",
                                 "2010-03-14T07:00:00", "EST", -18000, false },
        // NY, Fall transition 2009
        { L_, NY, "2009-11-01T06:00:00", "2009-11-01T06:00:00",
                             "2010-03-14T07:00:00", "EST", -18000, false },
        // NY, 1ms prior to spring transition 2010
        { L_, NY, "2010-03-14T06:59:59.999", "2009-11-01T06:00:00",
                                 "2010-03-14T07:00:00", "EST", -18000, false },
        // NY, spring transition 2010
        { L_, NY, "2010-03-14T07:00:00", "2010-03-14T07:00:00",
                                 "2010-11-07T06:00:00", "EDT", -14400,  true },
        // GMT, earliest 'bdlt::Datetime'
        { L_, GMT, "0001-01-01T00:00:00", "0001-01-01T00:00:00",
                             "9999-12-31T23:59:59.999", "GMT",      0, false },
        // GMT, latest 'bdlt::Datetime'
        { L_, GMT, "9999-12-31T23:59:59.999", "0001-01-01T00:00:00",
                             "9999-12-31T23:59:59.999", "GMT",      0, false },
        // GMT, typical 'bdlt::Datetime'
        { L_, GMT, "2010-03-14T07:00:00", "0001-01-01T00:00:00",
                             "9999-12-31T23:59:59.999", "GMT",      0, false },
        // Riyadh, prior to first transition.
        { L_, RY, "1940-01-01T00:00:00", "0001-01-01T00:00:00",
                                 "1949-12-31T20:53:08", "LMT",  11212, false },
        // Riyadh, after final transition
        { L_, RY, "1990-01-01T00:00:00", "1949-12-31T20:53:08",
                             "9999-12-31T23:59:59.999", "AST",  10800, false },

        };
        const int NUM_DATA = sizeof(DATA) / sizeof(*DATA);

        if (verbose) cout << "\tTesting 'loadLocalTimePeriodForUtc'." << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int            LINE   = DATA[ti].d_line;
            const char          *TZID   = DATA[ti].d_timeZoneId;
            const bdlt::Datetime  TIME   = toDatetime(DATA[ti].d_input);
            const bdlt::Datetime  START  = toDatetime(DATA[ti].d_start);
            const bdlt::Datetime  END    = toDatetime(DATA[ti].d_end);
            const char          *ABBREV = DATA[ti].d_abbrev;
            const int            OFFSET = DATA[ti].d_utcOffset;
            const bool           ISDST  = DATA[ti].d_isDst;

            baltzo::LocalTimePeriod result;

            LOOP_ASSERT(LINE, 0 == Obj::loadLocalTimePeriodForUtc(&result,
                                                                  TZID,
                                                                  TIME));

            if (veryVeryVerbose) {
                P_(LINE); P_(TIME); P(result);
            }

            const Descriptor& desc = result.descriptor();
            LOOP_ASSERT(LINE, START  == result.utcStartTime());
            LOOP_ASSERT(LINE, END    == result.utcEndTime());
            LOOP_ASSERT(LINE, ABBREV == desc.description());
            LOOP_ASSERT(LINE, ISDST  == desc.dstInEffectFlag());
            LOOP_ASSERT(LINE, OFFSET == desc.utcOffsetInSeconds());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout <<
                                "\t'loadLocalTimePeriod' class method" << endl;
            {
                const bdlt::Datetime TIME(2010, 1, 1, 12, 0);

                baltzo::LocalTimePeriod resultPeriod;

                ASSERT_SAFE_PASS(Obj::loadLocalTimePeriodForUtc(
                                                            &resultPeriod,
                                                            "America/New_York",
                                                            TIME));
                ASSERT_SAFE_FAIL(Obj::loadLocalTimePeriodForUtc(
                                                            0,
                                                            "America/New_York",
                                                            TIME));
                ASSERT_SAFE_FAIL(Obj::loadLocalTimePeriodForUtc(&resultPeriod,
                                                                0,
                                                                TIME));
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
                cout << "\tTest convertUtcToLocalTime." << endl;
            }
            {
                // Test with an invalid time zone id.
                LogVerbosityGuard guard;
                bdlt::Datetime inputTime(2010, 1, 1, 12, 0);

                bdlt::DatetimeTz result;
                ASSERT(EUID == Obj::convertUtcToLocalTime(&result,
                                                          "bogusId",
                                                          inputTime));
            }

            const struct TestData {
                int         d_line;
                const char *d_timeZoneId;
                const char *d_input;           // iso8601 w/o tz offset
                const char *d_expectedResult;  // iso8601 w/ tz offset
            } VALUES[] = {
// test well-defined values
 { L_,  NY, "2010-01-01T12:00:00", "2010-01-01T07:00:00-05:00" },
 { L_,  NY, "2010-01-01T00:00:00", "2009-12-31T19:00:00-05:00" },

// test around a fall (DST->STD) transition
 { L_,  NY, "2009-11-01T05:59:59.999", "2009-11-01T01:59:59.999-04:00" },
 { L_,  NY, "2009-11-01T06:00:00.000", "2009-11-01T01:00:00.000-05:00" },
 { L_,  NY, "2009-11-01T06:59:59.999", "2009-11-01T01:59:59.999-05:00" },
 { L_,  NY, "2009-11-01T07:00:00.000", "2009-11-01T02:00:00.000-05:00" },

// test around a spring (STD->DST) transition
 { L_,  NY, "2010-03-14T06:59:59.999", "2010-03-14T01:59:59.999-05:00" },
 { L_,  NY, "2010-03-14T07:00:00.000", "2010-03-14T03:00:00.000-04:00" },
 { L_,  NY, "2010-03-14T07:59:59.999", "2010-03-14T03:59:59.999-04:00" },
 { L_,  NY, "2010-03-14T08:00:00.000", "2010-03-14T04:00:00.000-04:00" },

// Trivial time zones (GMT, GMT+1, GMT-1)
 { L_,  GMT, "2010-01-01T12:00:00", "2010-01-01T12:00:00-00:00" },
 { L_,  GMT, "2010-01-01T00:00:00", "2010-01-01T00:00:00-00:00" },
 { L_,  GP1, "2010-01-01T12:00:00", "2010-01-01T11:00:00-01:00" },
 { L_,  GP1, "2010-01-01T00:00:00", "2009-12-31T23:00:00-01:00" },
 { L_,  GM1, "2010-01-01T12:00:00", "2010-01-01T13:00:00+01:00" },
 { L_,  GM1, "2010-01-01T00:00:00", "2010-01-01T01:00:00+01:00" },

// Time zone with 1 transition (2 descriptors) (Riyadh)
 { L_,  RY, "1949-12-31T20:53:07.999", "1949-12-31T23:59:07.999+03:06" },
 { L_,  RY, "1949-12-31T20:53:08.000", "1949-12-31T23:53:08.000+03:00" },
 { L_,  RY, "1949-12-31T20:53:09.000", "1949-12-31T23:53:09.000+03:00" },
 { L_,  RY, "1950-12-31T20:53:07.999", "1950-12-31T23:53:07.999+03:00" },
 { L_,  RY, "1950-12-31T20:53:08.000", "1950-12-31T23:53:08.000+03:00" },
 { L_,  RY, "1950-12-31T20:53:09.000", "1950-12-31T23:53:09.000+03:00" },

// Time zone with multiple sequential STD transitions (Saigon)
 { L_,  SA, "1912-04-30T16:59:59.999", "1912-04-30T23:59:59.999+07:00" },
 { L_,  SA, "1912-04-30T17:00:00.000", "1912-05-01T01:00:00.000+08:00" },
 { L_,  SA, "1912-04-30T17:01:00.000", "1912-05-01T01:01:00.000+08:00" },

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
                                                 inputStr.size()));
                ASSERT(0 == bdlt::Iso8601Util::parse(&expResultTime,
                                                 expResultStr.c_str(),
                                                 expResultStr.size()));

                if (veryVeryVerbose) {
                    P_(LINE); P_(timeZoneId); P_(inputTime); P(expResultTime);
                }

                bdlt::DatetimeTz resultTz;
                ASSERT(0 == Obj::convertUtcToLocalTime(&resultTz,
                                                       timeZoneId,
                                                       inputTime));

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

                bdlt::DatetimeTz    resultTz;
                baltzo::LocalDatetime resultLcl(Z);
                ASSERT(EUID == Obj::initLocalTime(&resultTz,
                                                  inputTime,
                                                  "bogusId",
                                                  UNSP));
                ASSERT(EUID == Obj::initLocalTime(&resultLcl,
                                                  inputTime,
                                                  "bogusId",
                                                  UNSP));
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
// | (daylight-saving ) |                |               |      [1]        |
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
 { L_,  NY, "2010-01-01T01:30:00", UNSP, "2010-01-01T01:30:00-05:00" },
 { L_,  NY, "2010-01-01T01:30:00",  DST, "2010-01-01T00:30:00-05:00" },
 { L_,  NY, "2010-01-01T01:30:00",  STD, "2010-01-01T01:30:00-05:00" },

 { L_,  NY, "2010-03-14T02:30:00", UNSP, "2010-03-14T03:30:00-04:00" },
 { L_,  NY, "2010-03-14T02:30:00",  DST, "2010-03-14T01:30:00-05:00" },
 { L_,  NY, "2010-03-14T02:30:00",  STD, "2010-03-14T03:30:00-04:00" },

 { L_,  NY, "2010-04-01T01:30:00", UNSP, "2010-04-01T01:30:00-04:00" },
 { L_,  NY, "2010-04-01T01:30:00",  DST, "2010-04-01T01:30:00-04:00" },
 { L_,  NY, "2010-04-01T01:30:00",  STD, "2010-04-01T02:30:00-04:00" },

 { L_,  NY, "2010-11-07T01:30:00", UNSP, "2010-11-07T01:30:00-05:00" },
 { L_,  NY, "2010-11-07T01:30:00",  DST, "2010-11-07T01:30:00-04:00" },
 { L_,  NY, "2010-11-07T01:30:00",  STD, "2010-11-07T01:30:00-05:00" },

// Trivial time zones
 { L_, GMT, "2010-11-07T01:30:00",  STD, "2010-11-07T01:30:00-00:00" },
 { L_, GP1, "2010-11-07T01:30:00",  STD, "2010-11-07T01:30:00-01:00" },
 { L_, GM1, "2010-11-07T01:30:00",  STD, "2010-11-07T01:30:00+01:00" },

 { L_, RY, "2010-11-07T01:30:00",  STD, "2010-11-07T01:30:00+03:00" },

// Time zone with sequential STD transitions (Saigon)
 { L_, SA, "1912-05-01T00:30:00",  UNSP, "1912-05-01T01:30:00+08:00" },
 { L_, SA, "1912-05-01T00:30:00",   STD, "1912-04-30T23:30:00+07:00" },
 { L_, SA, "1912-05-01T00:30:00",   DST, "1912-04-30T23:30:00+07:00" }

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
                                                 inputStr.size()));
                ASSERT(0 == bdlt::Iso8601Util::parse(&expResultTime,
                                                 expResultStr.c_str(),
                                                 expResultStr.size()));
                baltzo::LocalDatetime expLocalTime(
                                                 expResultTime, timeZoneId, Z);

                if (veryVeryVerbose) {
                    P_(LINE); P_(policy); P_(inputTime); P(expLocalTime);
                }

                bdlt::DatetimeTz    resultTz;
                baltzo::LocalDatetime resultLcl(Z);
                ASSERT(0 == Obj::initLocalTime(&resultTz,
                                               inputTime,
                                               timeZoneId,
                                               policy));
                ASSERT(0 == Obj::initLocalTime(&resultLcl,
                                               inputTime,
                                               timeZoneId,
                                               policy));

                LOOP2_ASSERT(LINE, resultTz,  resultTz   == expResultTime);
                LOOP2_ASSERT(LINE, resultLcl, resultLcl == expLocalTime);
            }
        }
        {
            if (veryVerbose) {
                cout << "\tTest loadLocalTimePeriod."
                     << endl;
            }

            struct TestData {
                int                d_line;
                const char        *d_timeZoneId;
                const char        *d_input;   // iso8601 w/o tz offset
                const char        *d_start;   // iso8601 w/o tz offset
                const char        *d_end;     // iso8601 w/o tz offset
                const char        *d_abbrev;  // "EDT"
                int                d_utcOffset;
                bool               d_isDst;
            } VALUES[] = {
// NY, Mid Winter 2010
                {
                    L_,
                    "America/New_York",
                    "2010-01-01T01:00:00-05:00",
                    "2009-11-01T06:00:00",
                    "2010-03-14T07:00:00",
                    "EST",
                    -18000,
                    false
                },

 // NY, Fall transition 2009
                {
                    L_,
                    "America/New_York",
                    "2009-11-01T01:00:00-05:00",
                    "2009-11-01T06:00:00",
                    "2010-03-14T07:00:00",
                    "EST",
                    -18000,
                    false
                },
                {
                    L_,
                    "America/New_York",
                    "2009-11-01T01:00:00-04:00",
                    "2009-03-08T07:00:00",
                    "2009-11-01T06:00:00",
                    "EDT",
                    -14400,
                    true,
                },

 // NY, 1ms prior to spring transition 2010
                {
                    L_,
                    "America/New_York",
                    "2010-03-14T01:59:59.999-05:00",
                    "2009-11-01T06:00:00",
                    "2010-03-14T07:00:00",
                    "EST",
                    -18000,
                    false
                },

 // NY, Spring transition 2010
                {
                    L_,
                    "America/New_York",
                    "2010-03-14T02:00:00-05:00",
                    "2010-03-14T07:00:00",
                    "2010-11-07T06:00:00",
                    "EDT",
                    -14400,
                    true
                },
                {
                    L_,
                    "America/New_York",
                    "2010-03-14T02:00:00-04:00",
                    "2009-11-01T06:00:00",
                    "2010-03-14T07:00:00",
                    "EST",
                    -18000,
                    false
                },
  // GMT
                {
                    L_,
                    "GMT",
                    "2010-03-14T07:00:00-00:00",
                    "0001-01-01T00:00:00",
                    "9999-12-31T23:59:59.999",
                    "GMT",
                    0,
                    false,
                },
            };

            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE       = VALUES[i].d_line;
                const char *timeZoneId = VALUES[i].d_timeZoneId;
                const char *abbrev     = VALUES[i].d_abbrev;
                const int   offset     = VALUES[i].d_utcOffset;
                const bool  isDst      = VALUES[i].d_isDst;

                const bsl::string inputStr(VALUES[i].d_input);
                const bsl::string startStr(VALUES[i].d_start);
                const bsl::string endStr(VALUES[i].d_end);

                bdlt::DatetimeTz inputTz;
                bdlt::Datetime   start, end;
                ASSERT(0 == bdlt::Iso8601Util::parse(&inputTz,
                                                 inputStr.c_str(),
                                                 inputStr.size()));
                ASSERT(0 == bdlt::Iso8601Util::parse(&start,
                                                 startStr.c_str(),
                                                 startStr.size()));
                ASSERT(0 == bdlt::Iso8601Util::parse(&end,
                                                 endStr.c_str(),
                                                 endStr.size()));

                baltzo::LocalTimePeriod result;

                ASSERT(0 == Obj::loadLocalTimePeriod(&result,
                                                     inputTz,
                                                     timeZoneId));

                if (veryVeryVerbose) {
                    P_(LINE) P_(inputTz) P(result);
                }
                LOOP_ASSERT(LINE, start  == result.utcStartTime());
                LOOP_ASSERT(LINE, end    == result.utcEndTime());
                LOOP_ASSERT(
                       LINE,
                       abbrev == result.descriptor().description());
                LOOP_ASSERT(
                       LINE,
                       isDst  == result.descriptor().dstInEffectFlag());
                LOOP_ASSERT(
                       LINE,
                       offset == result.descriptor().utcOffsetInSeconds());

            }
        }

        {
            if (veryVerbose) {
                cout << "\tTest loadLocalTimePeriodForUtc." << endl;
            }
            {
                // Test with an invalid time zone id.
                LogVerbosityGuard guard;
                bdlt::Datetime inputTime(2010, 1, 1, 12, 0);

                baltzo::LocalTimePeriod result;
                ASSERT(EUID == Obj::loadLocalTimePeriodForUtc(&result,
                                                              "bogusId",
                                                              inputTime));
            }
            struct TestData {
                int         d_line;
                const char *d_timeZoneId;
                const char *d_input;           // iso8601 w/o tz offset
                const char *d_start;           // iso8601 w/o tz offset
                const char *d_end;             // iso8601 w/o tz offset
                const char *d_abbrev;          // "EDT"
                int         d_utcOffset;
                bool        d_isDst;
            } VALUES[] = {
                {   // NY, Mid Winter 2010
                    L_,
                    "America/New_York",
                    "2010-01-01T01:00:00",
                    "2009-11-01T06:00:00",
                    "2010-03-14T07:00:00",
                    "EST",
                    -18000,
                    false
                },
                {   // NY, Fall transition 2009
                    L_,
                    "America/New_York",
                    "2009-11-01T06:00:00",
                    "2009-11-01T06:00:00",
                    "2010-03-14T07:00:00",
                    "EST",
                    -18000,
                    false
                },
                { // NY, 1ms prior to spring transition 2010
                    L_,
                    "America/New_York",
                    "2010-03-14T06:59:59.999",
                    "2009-11-01T06:00:00",
                    "2010-03-14T07:00:00",
                    "EST",
                    -18000,
                    false
                },
                { // NY, spring transition 2010
                    L_,
                    "America/New_York",
                    "2010-03-14T07:00:00",
                    "2010-03-14T07:00:00",
                    "2010-11-07T06:00:00",
                    "EDT",
                    -14400,
                    true
                },
                { // GMT
                    L_,
                    "GMT",
                    "2010-03-14T07:00:00",
                    "0001-01-01T00:00:00",
                    "9999-12-31T23:59:59.999",
                    "GMT",
                    0,
                    false,
                },
                { // Riyadh, prior to first transition.
                    L_,
                    "Asia/Riyadh",
                    "1940-01-01T00:00:00",
                    "0001-01-01T00:00:00",
                    "1949-12-31T20:53:08",
                    "LMT",
                    11212,
                    false,
                },
                { // Riyadh, after final transition
                    L_,
                    "Asia/Riyadh",
                    "1990-01-01T00:00:00",
                    "1949-12-31T20:53:08",
                    "9999-12-31T23:59:59.999",
                    "AST",
                    10800,
                    false,
                },
            };
            const int NUM_VALUES = sizeof(VALUES) / sizeof(*VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int   LINE        = VALUES[i].d_line;
                const char *timeZoneId  = VALUES[i].d_timeZoneId;
                const char *abbrev      = VALUES[i].d_abbrev;
                const int   offset      = VALUES[i].d_utcOffset;
                const bool  isDst       = VALUES[i].d_isDst;
                const bsl::string inputStr(VALUES[i].d_input);
                const bsl::string startStr(VALUES[i].d_start);
                const bsl::string endStr(VALUES[i].d_end);

                bdlt::Datetime input, start, end;
                ASSERT(0 == bdlt::Iso8601Util::parse(&input,
                                                 inputStr.c_str(),
                                                 inputStr.size()));
                ASSERT(0 == bdlt::Iso8601Util::parse(&start,
                                                 startStr.c_str(),
                                                 startStr.size()));
                ASSERT(0 == bdlt::Iso8601Util::parse(&end,
                                                 endStr.c_str(),
                                                 endStr.size()));

                baltzo::LocalTimePeriod result;
                ASSERT(0 == Obj::loadLocalTimePeriodForUtc(&result,
                                                           timeZoneId,
                                                           input));

                if (veryVeryVerbose) {
                    P_(LINE); P_(input); P(result);
                }
                LOOP_ASSERT(LINE, start == result.utcStartTime());
                LOOP_ASSERT(LINE, end   == result.utcEndTime());
                LOOP_ASSERT(
                           LINE,
                           abbrev == result.descriptor().description());
                LOOP_ASSERT(
                           LINE,
                           isDst  == result.descriptor().dstInEffectFlag());
                LOOP_ASSERT(
                           LINE,
                           offset == result.descriptor().utcOffsetInSeconds());
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
