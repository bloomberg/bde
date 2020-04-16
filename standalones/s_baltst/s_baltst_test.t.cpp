// s_baltst_test.t.cpp                                                -*-C++-*-

#include <s_baltst_test.h>

#include <s_baltst_address.h>
#include <s_baltst_basicrecord.h>
#include <s_baltst_bigrecord.h>
#include <s_baltst_customint.h>
#include <s_baltst_customizedstring.h>
#include <s_baltst_customstring.h>
#include <s_baltst_employee.h>
#include <s_baltst_enumerated.h>
#include <s_baltst_featuretestmessage.h>
#include <s_baltst_mychoice.h>
#include <s_baltst_myenumeration.h>
#include <s_baltst_mysequence.h>
#include <s_baltst_mysequencewithanonymouschoice.h>
#include <s_baltst_mysequencewithanonymouschoicechoice.h>
#include <s_baltst_mysequencewitharray.h>
#include <s_baltst_mysequencewithattributes.h>
#include <s_baltst_mysequencewithnillable.h>
#include <s_baltst_mysequencewithnillables.h>
#include <s_baltst_mysequencewithnullable.h>
#include <s_baltst_mysequencewithnullables.h>
#include <s_baltst_mysimplecontent.h>
#include <s_baltst_mysimpleintcontent.h>
#include <s_baltst_ratsnest.h>
#include <s_baltst_rawdata.h>
#include <s_baltst_rawdataswitched.h>
#include <s_baltst_rawdataunformatted.h>
#include <s_baltst_request.h>
#include <s_baltst_response.h>
#include <s_baltst_sequencewithanonymity.h>
#include <s_baltst_sequencewithanonymitychoice.h>
#include <s_baltst_sequencewithanonymitychoice1.h>
#include <s_baltst_sequencewithanonymitychoice2.h>
#include <s_baltst_simplerequest.h>
#include <s_baltst_sqrt.h>
#include <s_baltst_timingrequest.h>
#include <s_baltst_topchoice.h>
#include <s_baltst_unsignedsequence.h>
#include <s_baltst_voidsequence.h>

#include <bslim_testutil.h>

#include <bsl_iostream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

int main(int argc, char **argv)
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;    (void) veryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concern:
        //: o That all classes in the package can be instantiated and
        //:   compared.
        //
        // Plan:
        //: o For each type in the package, default construct a pair of
        //:   objects.
        //:
        //: o Use '==' and '!=' to compare them.
        //:
        //: o Use 'ASSERTV' printing out values to ensure that 'operator<<' is
        //:   is defined.
        //:
        //: o Skip 's_baltst::Enumerated' and 's_baltst::MyEnumeration' because
        //:   they are utility classes without equality comparisons.
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        {
            s_baltst::Address a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::BasicRecord a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::BigRecord a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::Choice1 a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::Choice2 a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::Choice3 a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::CustomInt a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::CustomizedString a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::CustomString a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::Employee a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::FeatureTestMessage a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::MyChoice a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::MySequence a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::MySequenceWithAnonymousChoice a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::MySequenceWithAnonymousChoiceChoice a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::MySequenceWithArray a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::MySequenceWithAttributes a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::MySequenceWithNillable a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::MySequenceWithNillables a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::MySequenceWithNullable a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::MySequenceWithNullables a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::MySimpleContent a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::MySimpleIntContent a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::RawData a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::RawDataSwitched a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::RawDataUnformatted a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::Request a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::Response a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::Sequence1 a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::Sequence2 a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::Sequence3 a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::Sequence4 a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::Sequence5 a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::Sequence6 a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::SequenceWithAnonymity a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::SequenceWithAnonymityChoice a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::SequenceWithAnonymityChoice1 a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::SequenceWithAnonymityChoice2 a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::SimpleRequest a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::Sqrt a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::TimingRequest a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::Topchoice a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::UnsignedSequence a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
        }

        {
            s_baltst::VoidSequence a, b;
            ASSERTV(a, b, a == b);
            ASSERTV(a, b, !(a != b));
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
