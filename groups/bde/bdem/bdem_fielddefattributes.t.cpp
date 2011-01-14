// bdem_fielddefattributes.t.cpp                                      -*-C++-*-

#include <bdem_fielddefattributes.h>

#include <bdem_elemtype.h>
#include <bdem_elemref.h>

#include <bdet_datetime.h>
#include <bdet_datetimetz.h>
#include <bdet_date.h>
#include <bdet_datetz.h>
#include <bdet_time.h>
#include <bdet_timetz.h>

#include <bdetu_unset.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_testallocator.h>             // for testing only
#include <bslma_testallocatorexception.h>    // for testing only

#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// TBD doc
//
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdem_FieldDefAttributes Obj;

typedef bdem_ElemRef            ERef;
typedef bdem_ElemType           EType;
typedef bsls_Types::Int64       Int64;

//=============================================================================
//                         HELPER DATA AND FUNCTIONS
//-----------------------------------------------------------------------------

    // Create Three Distinct Exemplars For Each Scalar Element Type
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const char            A00 = 'A';
const char            B00 = 'B';
const char            N00 = bdetu_Unset<char>::unsetValue();

const short           A01 = -1;
const short           B01 = -2;
const short           N01 = bdetu_Unset<short>::unsetValue();

const int             A02 = 10;
const int             B02 = 20;
const int             N02 = bdetu_Unset<int>::unsetValue();

const Int64           A03 = -100;
const Int64           B03 = -200;
const Int64           N03 = bdetu_Unset<bsls_Types::Int64>::unsetValue();

const float           A04 = -1.5;
const float           B04 = -2.5;
const float           N04 = bdetu_Unset<float>::unsetValue();

const double          A05 = 10.5;
const double          B05 = 20.5;
const double          N05 = bdetu_Unset<double>::unsetValue();

const bsl::string     A06 = "one";
const bsl::string     B06 = "two--plus-some-extra-stuff-to-force-allocation";
const bsl::string     N06 = bdetu_Unset<bsl::string>::unsetValue();

// Note: bdet_Datetime x07 implemented in terms of x08 and x09.

const bdet_Date       A08(2000,  1, 1);
const bdet_Date       B08(9999, 12,31);
const bdet_Date       N08 = bdetu_Unset<bdet_Date>::unsetValue();

const bdet_Time       A09(0, 1, 2, 3);
const bdet_Time       B09(4, 5, 6, 789);
const bdet_Time       N09 = bdetu_Unset<bdet_Time>::unsetValue();

const bdet_Datetime   A07(A08, A09);
const bdet_Datetime   B07(B08, B09);
const bdet_Datetime   N07 = bdetu_Unset<bdet_Datetime>::unsetValue();

const bool            A22 = true;
const bool            B22 = false;
const bool            N22 = bdetu_Unset<bool>::unsetValue();

const bdet_DatetimeTz A23(A07, -7);
const bdet_DatetimeTz B23(B07, 6);
const bdet_DatetimeTz N23 = bdetu_Unset<bdet_DatetimeTz>::unsetValue();

const bdet_DateTz     A24(A08, -5);
const bdet_DateTz     B24(B08, -4);
const bdet_DateTz     N24 = bdetu_Unset<bdet_DateTz>::unsetValue();

const bdet_TimeTz     A25(A09, 3);
const bdet_TimeTz     B25(B09, 11);
const bdet_TimeTz     N25 = bdetu_Unset<bdet_TimeTz>::unsetValue();

void setDefaultValueForType(ERef ref, bool other = false)
    // Set the value of the element referenced by the specified 'ref' to a
    // non-null value appropriate for its type.  If the optionally-specified
    // 'other' flag is 'true', set the value of the element referenced by
    // 'ref' to an alternate non-null value.  The behavior is undefined unless
    // the type of the element referenced by 'ref' is a scalar 'bdem' type or
    // array of scalar 'bdem' types.
{
    EType::Type elemType = ref.type();
    if (EType::isArrayType(elemType)) {
        elemType = EType::fromArrayType(elemType);
    }

    switch (elemType) {
      case EType::BDEM_CHAR: {
        ref.theModifiableChar()       = other ? B00 : A00;
      } break;
      case EType::BDEM_SHORT: {
        ref.theModifiableShort()      = other ? B01 : A01;
      } break;
      case EType::BDEM_INT: {
        ref.theModifiableInt()        = other ? B02 : A02;
      } break;
      case EType::BDEM_INT64: {
        ref.theModifiableInt64()      = other ? B03 : A03;
      } break;
      case EType::BDEM_FLOAT: {
        ref.theModifiableFloat()      = other ? B04 : A04;
      } break;
      case EType::BDEM_DOUBLE: {
        ref.theModifiableDouble()     = other ? B05 : A05;
      } break;
      case EType::BDEM_STRING: {
        ref.theModifiableString()     = other ? B06 : A06;
      } break;
      case EType::BDEM_DATETIME: {
        ref.theModifiableDatetime()   = other ? B07 : A07;
      } break;
      case EType::BDEM_DATE: {
        ref.theModifiableDate()       = other ? B08 : A08;
      } break;
      case EType::BDEM_TIME: {
        ref.theModifiableTime()       = other ? B09 : A09;
      } break;
      case EType::BDEM_BOOL: {
        ref.theModifiableBool()       = other ? B22 : A22;
      } break;
      case EType::BDEM_DATETIMETZ: {
        ref.theModifiableDatetimeTz() = other ? B23 : A23;
      } break;
      case EType::BDEM_DATETZ: {
        ref.theModifiableDateTz()     = other ? B24 : A24;
      } break;
      case EType::BDEM_TIMETZ: {
        ref.theModifiableTimeTz()     = other ? B25 : A25;
      } break;
      default: {
        ASSERT("Invalid element passed to 'setDefaultValueForType'" && 0);
      } break;
    }
}

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bslma_TestAllocator  testAllocator(veryVeryVerbose);
    bslma_Allocator     *Z = &testAllocator;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.  Suppress
        //   all 'bsl::cout' statements in non-verbose mode, and add streaming
        //   to a buffer to test programmatically the printing examples.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTesting Usage Examples"
                               << "\n======================" << bsl::endl;
///Usage
///-----
// The following example illustrates how to create a field type object, and
// both set and access its properties.

// We start by creating a 'bdem_FieldDefAttributes' objects, for a couple
// 'bdem' types:
//..
    bslma_Allocator *allocator = bslma_Default::allocator();
    bdem_FieldDefAttributes    intType(bdem_ElemType::BDEM_INT,  allocator);
    bdem_FieldDefAttributes   listType(bdem_ElemType::BDEM_LIST, allocator);
//..
// We verify the properties of the newly created field types:
//..
    ASSERT(bdem_ElemType::BDEM_INT    == intType.elemType());
    ASSERT(bdem_ElemType::BDEM_LIST   == listType.elemType());

    ASSERT(false == intType.hasDefaultValue());
    ASSERT(false == listType.hasDefaultValue());

    ASSERT(false == intType.isNullable());
    ASSERT(false == listType.isNullable());

    ASSERT(bdeat_FormattingMode::BDEAT_DEFAULT == intType.formattingMode());
    ASSERT(bdeat_FormattingMode::BDEAT_DEFAULT == listType.formattingMode());
//..
// We assign the 'isNullable' and 'formattingMode' for the two field types.
// Note that, depending on the context the field types are used in, these
// formatting modes may, or may not, be appropriate:
//..
    intType.setIsNullable(true);
    listType.setIsNullable(true);

    intType.setFormattingMode(bdeat_FormattingMode::BDEAT_HEX);
    listType.setFormattingMode(bdeat_FormattingMode::BDEAT_DEC);

    ASSERT(bdeat_FormattingMode::BDEAT_HEX == intType.formattingMode());
    ASSERT(bdeat_FormattingMode::BDEAT_DEC == listType.formattingMode());

    ASSERT(true == intType.isNullable());
    ASSERT(true == listType.isNullable());
//..
// Finally, we set the default value for 'intType'.  We do not set the default
// value for 'listType' because only scalar 'bdem' types may have a default
// field value:
//..
    intType.defaultValue().theModifiableInt() = 1;
    // listType.defaultValue();   // *error*, undefined behavior.

    ASSERT(true  == intType.hasDefaultValue());
    ASSERT(false == listType.hasDefaultValue());
    ASSERT(1     == intType.defaultValue().theInt());
//..
// Finally, we write the two field types to the console:
//..
if (veryVerbose) {
    bsl::cout << "intType:  " << intType  << bsl::endl;
    bsl::cout << "listType: " << listType << bsl::endl;
}
//..
// The resulting console output looks like:
//..
//  intType:  { INT nullable 100 0x2 }
//  listType: { LIST nullable NO_DEFAULT 0x1 }
//..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'bdem_FieldDefAttributes' COPY-ASSIGNMENT OPERATOR AND SWAP
        //
        // Concerns:
        //   We are concerned that any field def attributes value can be
        //   assigned to any other field def attributes value, and that 'swap'
        //   also works for objects with those attribute values.
        //
        // Plan:
        //   Define a set S of varied field def attributes test values.  For
        //   each (u, v) in S x S, verify that u = v produces the expected
        //   results.  Then verify that calling 'swap' produces the expected
        //   result as well.  Exception neutrality is tested; this is relevant
        //   to 'BDEM_STRING' default values.
        //
        // Testing:
        //   bdem_FieldDefAttributes& operator=(
        //                                 const bdem_FieldDefAttributes& rhs);
        //   void swap(bdem_FieldDefAttributes& other);
        // --------------------------------------------------------------------

        if (verbose)
            bsl::cout << "\nTesting 'bdem_FieldSpec::operator=' and 'swap'"
                      << "\n=============================================="
                      << bsl::endl;

        {
            static const struct {
                int           d_lineNum;         // source line number
                EType::Type   d_type;            // 'bdem' type
                int           d_formattingMode;  // formatting mode
                bool          d_isNullable;      // nullable?
                bool          d_defaultValue;    // supply a default value?
            } DATA[] = {
                //line 'bdem' type             fmt   null    has dflt val
                //---- -----------             ---   ----    --------------
                { L_,  EType::BDEM_INT,          0,  true,   false     },
                { L_,  EType::BDEM_INT,          4,  false,  true      },

                { L_,  EType::BDEM_DATE,         0,  true,   true      },
                { L_,  EType::BDEM_DATETZ,       0,  true,   true      },

                { L_,  EType::BDEM_STRING,       5,  false,  false     },
                { L_,  EType::BDEM_STRING,      -1,  true,   true      },

                { L_,  EType::BDEM_SHORT_ARRAY,  0,  true,   false     },
                { L_,  EType::BDEM_SHORT_ARRAY,  4,  false,  true      },

                { L_,  EType::BDEM_LIST,         0,  false,  false     },
                { L_,  EType::BDEM_TABLE,        3,  true,   false     },

                { L_,  EType::BDEM_CHOICE,       5,  true,   false     },
                { L_,  EType::BDEM_CHOICE,      77,  false,  false     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         ILINE    = DATA[ti].d_lineNum;
                const EType::Type ITYPE    = DATA[ti].d_type;
                const int         IFORMAT  = DATA[ti].d_formattingMode;
                const bool        IISNULL  = DATA[ti].d_isNullable;
                const bool        IHASDFLT = DATA[ti].d_defaultValue;

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int         JLINE    = DATA[tj].d_lineNum;
                    const EType::Type JTYPE    = DATA[tj].d_type;
                    const int         JFORMAT  = DATA[tj].d_formattingMode;
                    const bool        JISNULL  = DATA[tj].d_isNullable;
                    const bool        JHASDFLT = DATA[tj].d_defaultValue;

                          Obj  mF(ITYPE, IFORMAT, IISNULL, Z);
                    const Obj&  F = mF;

                          Obj  mW(ITYPE, IFORMAT, IISNULL, Z);
                    const Obj&  W = mW;  // control

                    if (IHASDFLT) {
                        ASSERT(! EType::isAggregateType(ITYPE));
                        setDefaultValueForType(mF.defaultValue());
                        setDefaultValueForType(mW.defaultValue());
                    }

                    LOOP2_ASSERT(ILINE, JLINE,  1 == (F == W));
                    LOOP2_ASSERT(ILINE, JLINE,  0 == (F != W));

                    Obj        mG(JTYPE, JFORMAT, JISNULL, Z);
                    Obj const&  G = mG;

                    if (JHASDFLT) {
                        ASSERT(! EType::isAggregateType(JTYPE));
                        setDefaultValueForType(mG.defaultValue(), true);
                    }

                    const bool fgAreSame = ti == tj && !JHASDFLT;
                    LOOP2_ASSERT(ILINE, JLINE,  fgAreSame == (F == G));
                    LOOP2_ASSERT(ILINE, JLINE, !fgAreSame == (F != G));

                    BEGIN_BSLMA_EXCEPTION_TEST {
                      mF = G;
                    } END_BSLMA_EXCEPTION_TEST

                    LOOP2_ASSERT(ILINE, JLINE,          1 == (F == G));
                    LOOP2_ASSERT(ILINE, JLINE,          0 == (F != G));

                    LOOP2_ASSERT(ILINE, JLINE,  fgAreSame == (F == W));
                    LOOP2_ASSERT(ILINE, JLINE, !fgAreSame == (F != W));

                    // test the 'swap' method
                    // before: Fj, Gj, Wi (Fj means F corresponds to JLINE)
                    // after : Fi, Gj, Wj

                    swap(mF, mW);

                    LOOP2_ASSERT(ILINE, JLINE,          1 == (W == G));
                    LOOP2_ASSERT(ILINE, JLINE,          0 == (W != G));

                    LOOP2_ASSERT(ILINE, JLINE,  fgAreSame == (F == G));
                    LOOP2_ASSERT(ILINE, JLINE, !fgAreSame == (F != G));

                    LOOP2_ASSERT(ILINE, JLINE,  fgAreSame == (F == W));
                    LOOP2_ASSERT(ILINE, JLINE, !fgAreSame == (F != W));
                }
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING/USAGE TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout << "\tChoice and Sequence traits." << endl;

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
